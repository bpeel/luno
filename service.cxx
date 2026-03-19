/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4; fill-column: 100 -*- */
/*
 * This file is part of Luno.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "service.hxx"

#include <com/sun/star/beans/IllegalTypeException.hpp>
#include <com/sun/star/lang/IllegalArgumentException.hpp>
#include <com/sun/star/reflection/InvocationTargetException.hpp>
#include <com/sun/star/reflection/XServiceTypeDescription2.hpp>
#include <com/sun/star/script/CannotConvertException.hpp>
#include <com/sun/star/script/XTypeConverter.hpp>
#include <com/sun/star/uno/XComponentContext.hpp>
#include <cassert>
#include <rtl/ustrbuf.hxx>

#include "conversions.hxx"
#include "pushexception.hxx"
#include "serviceconstructor.hxx"

namespace uk::co::busydoingnothing::luno
{
void Service::pushService(
    lua_State* pLuaState,
    const css::uno::Reference<css::reflection::XServiceTypeDescription2>& xTypeDescription,
    const Runtime& rRuntime)
{
    void* pUserData = lua_newuserdatauv(pLuaState, sizeof(Service), 0);

    // Use placement new to initialize the service in the memory that Lua allocated
    new (pUserData) Service(xTypeDescription, rRuntime);

    pushMetatable(pLuaState);
    lua_setmetatable(pLuaState, -2);
}

void Service::pushMetatable(lua_State* pLuaState)
{
    if (luaL_newmetatable(pLuaState, CLASS_NAME) == 0)
    {
        // The table has already been created so there’s nothing else to do
        return;
    }

    lua_pushliteral(pLuaState, "__gc");
    lua_pushcfunction(pLuaState, gc);
    lua_rawset(pLuaState, -3);

    lua_pushliteral(pLuaState, "__index");
    lua_pushcfunction(pLuaState, index);
    lua_rawset(pLuaState, -3);
}

Service* Service::checkService(lua_State* pLuaState, int nArg)
{
    return reinterpret_cast<Service*>(luaL_checkudata(pLuaState, nArg, CLASS_NAME));
}

Service* Service::testService(lua_State* pLuaState, int nArg)
{
    return reinterpret_cast<Service*>(luaL_testudata(pLuaState, nArg, CLASS_NAME));
}

int Service::gc(lua_State* pLuaState)
{
    Service* pService = checkService(pLuaState, 1);

    // The memory was allocated by Lua so we only need to call the destructor
    pService->~Service();

    return 0;
}

int Service::doIndexUncached(lua_State* pLuaState)
{
    size_t nKeyLength;
    const char* pKey = luaL_checklstring(pLuaState, 2, &nKeyLength);

    if (!m_xTypeDescription.is())
    {
        lua_pushliteral(pLuaState, "__index called an service in an invalid state");
        goto set_lua_error;
    }

    {
        rtl::OUString sKey(pKey, nKeyLength, RTL_TEXTENCODING_UTF8);
        css::uno::Reference<css::reflection::XServiceConstructorDescription> xConstructor;

        for (const auto& xCheckConstructor : m_xTypeDescription->getConstructors())
        {
            rtl::OUString sName = xCheckConstructor->getName();

            if (sName.isEmpty())
            {
                if (xCheckConstructor->isDefaultConstructor())
                    sName = "create";
                else
                    continue;
            }

            if (sName == sKey)
            {
                xConstructor = xCheckConstructor;
                break;
            }
        }

        if (!xConstructor.is())
        {
            lua_pushnil(pLuaState);
            return 1;
        }

        ServiceConstructor::pushServiceConstructor(pLuaState, xConstructor);
        lua_pushcclosure(pLuaState, call, 1);
    }

    return 1;

    // The goto is to ensure that we call all of the destructors before letting Lua do a longjmp
set_lua_error:
    lua_error(pLuaState);
    return 0;
}

int Service::doIndex(lua_State* pLuaState)
{
    if (lua_getiuservalue(pLuaState, 1, 1) == LUA_TNIL)
    {
        // Lazily create the cache table
        lua_pop(pLuaState, 1);
        lua_newtable(pLuaState);
        lua_pushvalue(pLuaState, -1);
        lua_setiuservalue(pLuaState, 1, 1);
    }

    lua_pushvalue(pLuaState, 2);
    if (lua_rawget(pLuaState, -2) == LUA_TNIL)
    {
        // Lazily look up the constructor
        lua_pop(pLuaState, 1);
        doIndexUncached(pLuaState);

        if (!lua_isnil(pLuaState, -1))
        {
            // Add the constructor to the cache
            lua_pushvalue(pLuaState, 2);
            lua_pushvalue(pLuaState, -2);
            lua_rawset(pLuaState, -4);
        }
    }

    // Remove the cache table from the stack
    lua_remove(pLuaState, -2);

    return 1;
}

int Service::index(lua_State* pLuaState)
{
    Service* pService = checkService(pLuaState, 1);

    return pService->doIndex(pLuaState);
}

int Service::call(lua_State* pLuaState, ServiceConstructor* pConstructor)
{
    int nParams = lua_gettop(pLuaState) - 1;
    const std::vector<css::uno::TypeDescription>& aParameterTypes
        = pConstructor->getParameterTypes();

    {
        // context, fixed params
        sal_Int32 nMinParams = 1 + aParameterTypes.size();
        sal_Int32 nMaxParams;

        if (pConstructor->hasRest())
        {
            // The last parameter can be empty
            nMinParams--;
            nMaxParams = SAL_MAX_INT32;
        }
        else
            nMaxParams = nMinParams;

        if (nParams < nMinParams || nParams > nMaxParams)
        {
            css::uno::Reference<css::reflection::XServiceConstructorDescription> xConstructor
                = pConstructor->getDescription();

            rtl::OUString sConstructorName = xConstructor->getName();

            if (sConstructorName.isEmpty() && xConstructor->isDefaultConstructor())
                sConstructorName = "create";

            rtl::OUStringBuffer buf(m_xTypeDescription->getName() + "::" + sConstructorName
                                    + " requires ");

            if (pConstructor->hasRest())
                buf.append("at least ");

            buf.append(rtl::OUString::number(nMinParams) + " argument");

            if (nMinParams > 1)
                buf.append('s');

            lua_pushstring(
                pLuaState,
                OUStringToOString(buf.makeStringAndClear(), RTL_TEXTENCODING_ASCII_US).getStr());
            goto set_lua_error;
        }

        try
        {
            css::uno::Any contextAny = getAny(pLuaState, 2);
            css::uno::Reference<css::uno::XComponentContext> xContext;

            if (!(contextAny >>= xContext) || !xContext.is())
            {
                lua_pushliteral(pLuaState, "First argument to a service constructor must be an "
                                           "XComponentContext");
                goto set_lua_error;
            }

            assert(nParams > 0
                   && "nParams >= 1 after bounds check against nMinParams which is always >= 1");
            css::uno::Sequence<css::uno::Any> aParams(nParams - 1);
            css::uno::Any* pParams = aParams.getArray();

            for (sal_Int32 i = 0; i < nParams - 1; ++i)
            {
                css::uno::Any param = getAny(pLuaState, i + 3);

                // Use the type of this parameter or the last one if we’re building the rest
                // parameters
                std::size_t nConstructorParam
                    = std::min(std::size_t(i), aParameterTypes.size() - 1);
                typelib_TypeDescription* pDestType = aParameterTypes[nConstructorParam].get();

                // Try to coax the any to the right type. This is needed for example to allow
                // passing None to as an interface reference
                pParams[i] = m_rRuntime.m_xTypeConverter->convertTo(param, pDestType->pWeakRef);
            }

            css::uno::Reference<css::uno::XInterface> xInterface
                = xContext->getServiceManager()->createInstanceWithArgumentsAndContext(
                    m_xTypeDescription->getName(), aParams, xContext);
            pushAny(pLuaState, css::uno::Any(std::move(xInterface)), m_rRuntime);
        }
        catch (const css::reflection::InvocationTargetException& e)
        {
            pushExceptionFromAny(pLuaState, e.TargetException, m_rRuntime);
            goto set_lua_error;
        }
        catch (const css::script::CannotConvertException& e)
        {
            pushExceptionFromAny(pLuaState, css::uno::Any(e), m_rRuntime);
            goto set_lua_error;
        }
        catch (const css::beans::IllegalTypeException& e)
        {
            pushExceptionFromAny(pLuaState, css::uno::Any(e), m_rRuntime);
            goto set_lua_error;
        }
        catch (const css::lang::IllegalArgumentException& e)
        {
            pushExceptionFromAny(pLuaState, css::uno::Any(e), m_rRuntime);
            goto set_lua_error;
        }
        catch (const css::uno::RuntimeException& e)
        {
            pushExceptionFromAny(pLuaState, css::uno::Any(e), m_rRuntime);
            goto set_lua_error;
        }
    }

    return 1;

    // The goto is to make sure the destructors are all called before letting Lua do a longjmp
set_lua_error:
    lua_error(pLuaState);
    return 0;
}

int Service::call(lua_State* pLuaState)
{
    ServiceConstructor* pConstructor
        = ServiceConstructor::checkServiceConstructor(pLuaState, lua_upvalueindex(1));
    Service* pService = checkService(pLuaState, 1);

    return pService->call(pLuaState, pConstructor);
}
}

/* vim:set shiftwidth=4 softtabstop=4 expandtab cinoptions=b1,g0,N-s cinkeys+=0=break: */
