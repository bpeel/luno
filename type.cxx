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

#include "type.hxx"

#include <com/sun/star/reflection/XIdlClass.hpp>

namespace uk::co::busydoingnothing::luno
{
void Type::pushType(lua_State* pLuaState,
                    const css::uno::Reference<css::reflection::XIdlClass>& xIdlClass,
                    const Runtime& rRuntime)
{
    void *pUserData = lua_newuserdatauv(pLuaState, sizeof(Type), 0);

    // Use placement new to initialize the type in the memory that Lua allocated
    new(pUserData) Type(xIdlClass, rRuntime);

    pushMetatable(pLuaState);
    lua_setmetatable(pLuaState, -2);
}

void Type::pushMetatable(lua_State* pLuaState)
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

Type* Type::checkType(lua_State* pLuaState, int nArg)
{
    return reinterpret_cast<Type*>(luaL_checkudata(pLuaState, nArg, CLASS_NAME));
}

Type* Type::testType(lua_State* pLuaState, int nArg)
{
    return reinterpret_cast<Type*>(luaL_testudata(pLuaState, nArg, CLASS_NAME));
}

int Type::gc(lua_State* pLuaState)
{
    Type* pType = checkType(pLuaState, 1);

    // The memory was allocated by Lua so we only need to call the destructor
    pType->~Type();

    return 0;
}
}
