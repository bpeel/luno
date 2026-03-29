/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4; fill-column: 100 -*- */
/*
 * This file is part of Luno.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "method.hxx"

#include <com/sun/star/reflection/XIdlMethod.hpp>

namespace uk::co::busydoingnothing::luno
{
void Method::pushMethod(lua_State* pLuaState,
                        const css::uno::Reference<css::reflection::XIdlMethod>& xMethod)
{
    // Check if we already have a cached method with this name in the registry
    lua_pushstring(pLuaState, METHOD_CACHE_NAME);
    lua_gettable(pLuaState, LUA_REGISTRYINDEX);

    if (!lua_istable(pLuaState, -1))
    {
        // Lazily create the method cache table
        lua_pop(pLuaState, 1);
        lua_newtable(pLuaState);
        lua_pushstring(pLuaState, METHOD_CACHE_NAME);
        lua_pushvalue(pLuaState, -2);
        lua_rawset(pLuaState, LUA_REGISTRYINDEX);
    }

    rtl::OString sClassName
        = rtl::OUStringToOString(xMethod->getDeclaringClass()->getName(), RTL_TEXTENCODING_UTF8);
    rtl::OString sMethodName = rtl::OUStringToOString(xMethod->getName(), RTL_TEXTENCODING_UTF8);
    rtl::OString sCombinedName = sClassName + ":" + sMethodName;
    lua_pushlstring(pLuaState, sCombinedName.getStr(), sCombinedName.getLength());

    lua_pushvalue(pLuaState, -1);

    if (lua_rawget(pLuaState, -3) == LUA_TNIL)
    {
        // We don’t have a cached method so we need to create it
        lua_pop(pLuaState, 1);

        void* pUserData = lua_newuserdatauv(pLuaState, sizeof(Method), 0);

        // Use placement new to initialize the method in the memory that Lua allocated
        new (pUserData) Method(xMethod);

        pushMetatable(pLuaState);
        lua_setmetatable(pLuaState, -2);

        lua_pushvalue(pLuaState, -2);
        lua_pushvalue(pLuaState, -2);
        lua_rawset(pLuaState, -5);
    }

    // Remove the method name from the stack
    lua_remove(pLuaState, -2);
    // Remove the cache table from the stack
    lua_remove(pLuaState, -2);
}

void Method::pushMetatable(lua_State* pLuaState)
{
    if (luaL_newmetatable(pLuaState, CLASS_NAME) == 0)
    {
        // The table has already been created so there’s nothing else to do
        return;
    }

    lua_pushliteral(pLuaState, "__gc");
    lua_pushcfunction(pLuaState, gc);
    lua_rawset(pLuaState, -3);
}

Method* Method::checkMethod(lua_State* pLuaState, int nArg)
{
    return static_cast<Method*>(luaL_checkudata(pLuaState, nArg, CLASS_NAME));
}

int Method::gc(lua_State* pLuaState)
{
    Method* pMethod = checkMethod(pLuaState, 1);

    // The memory was allocated by Lua so we only need to call the destructor
    pMethod->~Method();

    return 0;
}
}

/* vim:set shiftwidth=4 softtabstop=4 expandtab cinoptions=b1,g0,N-s cinkeys+=0=break: */
