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
                        const rtl::OUString& sMethodName,
                        lua_CFunction pFunc)
{
    void *pUserData = lua_newuserdatauv(pLuaState, sizeof(Method), 0);

    // Use placement new to initialize the method in the memory that Lua allocated
    new(pUserData) Method(sMethodName, pFunc);

    pushMetatable(pLuaState, pFunc);
    lua_setmetatable(pLuaState, -2);
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
