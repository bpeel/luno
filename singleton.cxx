/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4; fill-column: 100 -*- */
/*
 * This file is part of Luno.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "singleton.hxx"

#include <com/sun/star/beans/IllegalTypeException.hpp>
#include <com/sun/star/reflection/XSingletonTypeDescription2.hpp>
#include <com/sun/star/uno/XComponentContext.hpp>

#include "conversions.hxx"
#include "pushexception.hxx"

namespace uk::co::busydoingnothing::luno
{
void Singleton::pushSingleton(
    lua_State* pLuaState,
    const css::uno::Reference<css::reflection::XSingletonTypeDescription2>& xTypeDescription,
    const Runtime& rRuntime)
{
    void* pUserData = lua_newuserdatauv(pLuaState, sizeof(Singleton), 0);

    // Use placement new to initialize the singleton in the memory that Lua allocated
    new (pUserData) Singleton(xTypeDescription, rRuntime);

    pushMetatable(pLuaState);
    lua_setmetatable(pLuaState, -2);
}

void Singleton::pushMetatable(lua_State* pLuaState)
{
    if (luaL_newmetatable(pLuaState, CLASS_NAME) == 0)
    {
        // The table has already been created so there’s nothing else to do
        return;
    }

    lua_pushliteral(pLuaState, "__gc");
    lua_pushcfunction(pLuaState, gc);
    lua_rawset(pLuaState, -3);

    // Make a table for the __index meta entry
    lua_pushliteral(pLuaState, "__index");

    lua_newtable(pLuaState);
    lua_pushliteral(pLuaState, "get");
    lua_pushcfunction(pLuaState, get);
    lua_rawset(pLuaState, -3);

    lua_rawset(pLuaState, -3);
}

Singleton* Singleton::checkSingleton(lua_State* pLuaState, int nArg)
{
    return reinterpret_cast<Singleton*>(luaL_checkudata(pLuaState, nArg, CLASS_NAME));
}

Singleton* Singleton::testSingleton(lua_State* pLuaState, int nArg)
{
    return reinterpret_cast<Singleton*>(luaL_testudata(pLuaState, nArg, CLASS_NAME));
}

int Singleton::gc(lua_State* pLuaState)
{
    Singleton* pSingleton = checkSingleton(pLuaState, 1);

    // The memory was allocated by Lua so we only need to call the destructor
    pSingleton->~Singleton();

    return 0;
}

int Singleton::doGet(lua_State* pLuaState)
{
    {
        css::uno::Any xContextAny;

        try
        {
            xContextAny = getAny(pLuaState, 2);
        }
        catch (const css::beans::IllegalTypeException& e)
        {
            pushExceptionFromAny(pLuaState, css::uno::Any(e), m_rRuntime);
            goto set_lua_error;
        }
        catch (const css::uno::Exception& e)
        {
            pushExceptionFromAny(pLuaState, css::uno::Any(e), m_rRuntime);
            goto set_lua_error;
        }

        css::uno::Reference<css::uno::XComponentContext> xContext;

        if (!(xContextAny >>= xContext) || !xContext.is())
        {
            lua_pushliteral(pLuaState, "The first argument to a singleton getter must be an "
                                       "XComponentContext");
            goto set_lua_error;
        }

        rtl::OUString sKey = "/singletons/" + m_xTypeDescription->getName();

        css::uno::Any xSingletonAny = xContext->getValueByName(sKey);

        if (xSingletonAny.getValueTypeClass() != css::uno::TypeClass_INTERFACE)
        {
            rtl::OUString sMessage
                = "Error getting singleton value for " + m_xTypeDescription->getName();
            rtl::OString sMessageUtf8 = rtl::OUStringToOString(sMessage, RTL_TEXTENCODING_UTF8);
            lua_pushstring(pLuaState, sMessageUtf8.getStr());
            goto set_lua_error;
        }

        try
        {
            pushAny(pLuaState, xSingletonAny, m_rRuntime);
        }
        catch (const css::beans::IllegalTypeException& e)
        {
            pushExceptionFromAny(pLuaState, css::uno::Any(e), m_rRuntime);
            goto set_lua_error;
        }
        catch (const css::uno::Exception& e)
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

int Singleton::get(lua_State* pLuaState)
{
    Singleton* pSingleton = checkSingleton(pLuaState, 1);

    return pSingleton->doGet(pLuaState);
}
}

/* vim:set shiftwidth=4 softtabstop=4 expandtab cinoptions=b1,g0,N-s cinkeys+=0=break: */
