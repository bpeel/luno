// Luno – LibreOffice bindings for Lua
// Copyright (C) 2026  Neil Roberts
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.

#include "method.hxx"

namespace uk::co::busydoingnothing::luno
{
void Method::pushMethod(lua_State* pLuaState,
                        int nMethodNamePos,
                        const rtl::OUString& sMethodName,
                        lua_CFunction pFunc)
{
    // Make sure the method pos isn’t relative to the top of the stack because we are going to push
    // things onto it
    if (nMethodNamePos < 0)
        nMethodNamePos += lua_gettop(pLuaState) + 1;

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

    lua_pushvalue(pLuaState, nMethodNamePos);
    lua_gettable(pLuaState, -2);

    if (lua_isnil(pLuaState, -1))
    {
        // We don’t have a cached method so we need to create it
        lua_pop(pLuaState, 1);

        void *pUserData = lua_newuserdatauv(pLuaState, sizeof(Method), 0);

        // Use placement new to initialize the method in the memory that Lua allocated
        new(pUserData) Method(sMethodName, pFunc);

        pushMetatable(pLuaState, pFunc);
        lua_setmetatable(pLuaState, -2);

        lua_pushvalue(pLuaState, nMethodNamePos);
        lua_pushvalue(pLuaState, -2);
        lua_rawset(pLuaState, -4);
    }

    // Remove the cache table from the stack
    lua_remove(pLuaState, -2);
}

void Method::pushMetatable(lua_State* pLuaState, lua_CFunction pFunc)
{
    if (luaL_newmetatable(pLuaState, CLASS_NAME) == 0)
    {
        // The table has already been created so there’s nothing else to do
        return;
    }

    lua_pushliteral(pLuaState, "__gc");
    lua_pushcfunction(pLuaState, gc);
    lua_rawset(pLuaState, -3);

    lua_pushliteral(pLuaState, "__call");
    lua_pushcfunction(pLuaState, pFunc);
    lua_rawset(pLuaState, -3);
}

Method* Method::checkMethod(lua_State* pLuaState, int nArg)
{
    return reinterpret_cast<Method*>(luaL_checkudata(pLuaState, nArg, CLASS_NAME));
}

int Method::gc(lua_State* pLuaState)
{
    Method* pMethod = checkMethod(pLuaState, 1);

    // The memory was allocated by Lua so we only need to call the destructor
    pMethod->~Method();

    return 0;
}
}
