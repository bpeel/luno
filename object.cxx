/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4; fill-column: 100 -*- */
/*
 * This file is part of Luno.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "object.hxx"

#include <com/sun/star/beans/MethodConcept.hpp>
#include <com/sun/star/beans/XIntrospection.hpp>
#include <com/sun/star/beans/XIntrospectionAccess.hpp>
#include <com/sun/star/lang/NoSuchMethodException.hpp>
#include <com/sun/star/reflection/XIdlReflection.hpp>
#include <com/sun/star/uno/Sequence.hxx>

#include "method.hxx"
#include "conversions.hxx"
#include "struct.hxx"

namespace uk::co::busydoingnothing::luno
{
void Object::pushObject(lua_State* pLuaState,
                        const css::uno::Reference<css::uno::XInterface>& xInterface,
                        const Runtime& rRuntime)
{
    // One user value to store a cache of methods
    void *pUserData = lua_newuserdatauv(pLuaState, sizeof(Object), 1);

    // Use placement new to initialize the object in the memory that Lua allocated
    new(pUserData) Object(xInterface, rRuntime);

    pushMetatable(pLuaState);
    lua_setmetatable(pLuaState, -2);
}

void Object::pushMetatable(lua_State* pLuaState)
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

    lua_pushliteral(pLuaState, "__eq");
    lua_pushcfunction(pLuaState, eq);
    lua_rawset(pLuaState, -3);
}

Object* Object::checkObject(lua_State* pLuaState, int nArg)
{
    return reinterpret_cast<Object*>(luaL_checkudata(pLuaState, nArg, CLASS_NAME));
}

Object* Object::testObject(lua_State* pLuaState, int nArg)
{
    return reinterpret_cast<Object*>(luaL_testudata(pLuaState, nArg, CLASS_NAME));
}

int Object::gc(lua_State* pLuaState)
{
    Object* pObject = checkObject(pLuaState, 1);

    // The memory was allocated by Lua so we only need to call the destructor
    pObject->~Object();

    return 0;
}

int Object::doIndexUncached(lua_State* pLuaState)
{
    size_t nKeyLength;
    const char* pKey = luaL_checklstring(pLuaState, 2, &nKeyLength);

    {
        if (!m_xIntrospectionAccess.is())
        {
            if (!m_rRuntime.isValid() || !m_xInterface.is())
                goto state_error;

            css::uno::Any xAny;
            xAny <<= m_xInterface;

            m_xIntrospectionAccess = m_rRuntime.m_xIntrospection->inspect(xAny);

            if (!m_xIntrospectionAccess.is())
                goto state_error;
        }

        rtl::OUString sKey(pKey, nKeyLength, RTL_TEXTENCODING_UTF8);

        css::uno::Reference<css::reflection::XIdlMethod> xMethod;

        try
        {
            xMethod = m_xIntrospectionAccess->getMethod(
                sKey, css::beans::MethodConcept::ALL & ~css::beans::MethodConcept::DANGEROUS);
        }
        catch (css::lang::NoSuchMethodException&)
        {
        }

        if (xMethod.is())
            Method::pushMethod(pLuaState, xMethod, call);
        else
            lua_pushnil(pLuaState);
    }

    return 1;

    // The goto is to ensure that we call all of the destructors before letting Lua do a longjmp
 state_error:
    luaL_error(pLuaState, "__index called an object in an invalid state");
    return 0;
}

int Object::doIndex(lua_State* pLuaState)
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
        // Lazily look up the method
        lua_pop(pLuaState, 1);
        doIndexUncached(pLuaState);

        if (!lua_isnil(pLuaState, -1))
        {
            // Add the method to the cache
            lua_pushvalue(pLuaState, 2);
            lua_pushvalue(pLuaState, -2);
            lua_rawset(pLuaState, -4);
        }
    }

    // Remove the cache table from the stack
    lua_remove(pLuaState, -2);

    return 1;
}

int Object::index(lua_State* pLuaState)
{
    Object* pObject = checkObject(pLuaState, 1);

    return pObject->doIndex(pLuaState);
}

int Object::doEq(lua_State* pLuaState)
{
    Object* pOther = testObject(pLuaState, 2);

    lua_pushboolean(pLuaState, pOther != nullptr && m_xInterface == pOther->m_xInterface);

    return 1;
}

int Object::eq(lua_State* pLuaState)
{
    Object* pObject = checkObject(pLuaState, 1);

    return pObject->doEq(pLuaState);
}

namespace
{
bool copyStruct(lua_State* pLuaState, int nArg, const css::uno::Any& xAny)
{
    Struct *pStruct = Struct::testStruct(pLuaState, nArg);

    if (pStruct == nullptr)
    {
        lua_pushliteral(pLuaState, "internal error: expected struct for inout parameter");
        return false;
    }

    pStruct->setValue(xAny);

    return true;
}

bool copySequence(lua_State* pLuaState, int nArg, const css::uno::Any& xAny,
                  const Runtime& rRuntime)
{
    if (!lua_istable(pLuaState, nArg))
    {
        lua_pushliteral(pLuaState, "internal error: expected table for inout parameter");
        return false;
    }

    {
        css::uno::Reference<css::reflection::XIdlClass> xClass
            = rRuntime.m_xIdlReflection->getType(xAny);

        if (!xClass.is())
            goto internal_error;

        css::uno::Reference<css::reflection::XIdlArray> xIdlArray = xClass->getArray();

        if (!xIdlArray.is())
            goto internal_error;

        int nCount = xIdlArray->getLen(xAny);

        for (int i = 0; i < nCount; ++i)
        {
            try
            {
                pushAny(pLuaState, xIdlArray->get(xAny, i), rRuntime);
            }
            catch (css::uno::Exception&)
            {
                goto internal_error;
            }

            lua_rawseti(pLuaState, nArg, i + 1);
        }

        // Remove any remaining values in the table
        for (int i = lua_rawlen(pLuaState, nArg); i > nCount; --i)
        {
            lua_pushnil(pLuaState);
            lua_rawseti(pLuaState, nArg, i);
        }
    }

    return true;

 internal_error:
    lua_pushliteral(pLuaState, "internal error while copying sequence inout parameter back");
    return false;
}
}

int Object::call(lua_State* pLuaState, Method *pMethod)
{
    int nArgs = lua_gettop(pLuaState) - 2;

    {
        css::uno::Reference<css::reflection::XIdlMethod> xIdlMethod = pMethod->getIdlMethod();
        const css::uno::Sequence<css::reflection::ParamInfo>& rParamInfos
            = xIdlMethod->getParameterInfos();

        int nInArgs = std::count_if(
            rParamInfos.begin(), rParamInfos.end(),
            [] (const css::reflection::ParamInfo& rParamInfo)
            {
                return rParamInfo.aMode == css::reflection::ParamMode_IN ||
                    rParamInfo.aMode == css::reflection::ParamMode_INOUT;
            });

        if (nInArgs != nArgs)
        {
            rtl::OString sMethodName
                = rtl::OUStringToOString(xIdlMethod->getName(), RTL_TEXTENCODING_UTF8);
            lua_pushfstring(pLuaState,
                            "Wrong number of arguments in call to %s. "
                            "Expected: %I, actual: %I",
                            sMethodName.getStr(), lua_Integer(nInArgs), lua_Integer(nArgs));
            goto set_lua_error;
        }

        css::uno::Sequence<css::uno::Any> aArgs(rParamInfos.getLength());
        css::uno::Any xTarget(m_xInterface);
        css::uno::Any xResult;
        int nReturnValues = 0;

        try
        {
            int nInArg = 0;

            for (int i = 0; i < rParamInfos.getLength(); ++i)
            {
                // Skip out-only params
                if (rParamInfos[i].aMode == css::reflection::ParamMode_OUT)
                    continue;

                aArgs[i] = getAnyAsType(pLuaState, nInArg + 3, rParamInfos[i].aType, m_rRuntime);

                ++nInArg;
            }

            xResult = xIdlMethod->invoke(xTarget, aArgs);

            css::uno::Reference<css::reflection::XIdlClass> xReturnType
                = xIdlMethod->getReturnType();

            if (xReturnType.is() && xReturnType->getTypeClass() != css::uno::TypeClass_VOID)
            {
                pushAny(pLuaState, xResult, m_rRuntime);
                ++nReturnValues;
            }

            nInArg = 3;

            for (int i = 0; i < rParamInfos.getLength(); ++i)
            {
                css::reflection::ParamMode eParamMode = rParamInfos[i].aMode;

                if (eParamMode == css::reflection::ParamMode_INOUT &&
                    rParamInfos[i].aType->getTypeClass() == css::uno::TypeClass_STRUCT)
                {
                    // Instead of returning the struct, copy the values directly back into struct
                    // held by Lua
                    if (!copyStruct(pLuaState, nInArg, aArgs[i]))
                        goto set_lua_error;
                }
                else if (eParamMode == css::reflection::ParamMode_INOUT &&
                         rParamInfos[i].aType->getTypeClass() == css::uno::TypeClass_SEQUENCE)
                {
                    // Instead of returning the sequence, copy the values directly back into the
                    // table held by Lua
                    if (!copySequence(pLuaState, nInArg, aArgs[i], m_rRuntime))
                        goto set_lua_error;
                }
                else if (eParamMode == css::reflection::ParamMode_OUT ||
                         eParamMode == css::reflection::ParamMode_INOUT)
                {
                    pushAny(pLuaState, aArgs[i], m_rRuntime);
                    ++nReturnValues;
                }

                if (eParamMode == css::reflection::ParamMode_IN ||
                    eParamMode == css::reflection::ParamMode_INOUT)
                    ++nInArg;
            }
        }
        catch (const css::uno::Exception& e)
        {
            rtl::OString sMessage = rtl::OUStringToOString(e.Message, RTL_TEXTENCODING_UTF8);
            lua_pushlstring(pLuaState, sMessage.getStr(), sMessage.getLength());
            goto set_lua_error;
        }

        return nReturnValues;
    }

    // The goto is to make sure the destructors are all called before letting Lua do a longjmp
 set_lua_error:
    lua_error(pLuaState);
    return 0;
}

int Object::call(lua_State* pLuaState)
{
    Method *pMethod = Method::checkMethod(pLuaState, 1);
    Object *pObject = checkObject(pLuaState, 2);

    return pObject->call(pLuaState, pMethod);
}

}

/* vim:set shiftwidth=4 softtabstop=4 expandtab cinoptions=b1,g0,N-s cinkeys+=0=break: */
