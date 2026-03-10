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

#include "object.hxx"

namespace uk::co::busydoingnothing::luno
{
void Object::pushObject(lua_State* pLuaState,
                        const css::uno::Reference<css::uno::XInterface>& xInterface,
                        const css::uno::Reference<css::lang::XSingleServiceFactory>&
                        xInvocationFactory)
{
    void *pUserData = lua_newuserdatauv(pLuaState, sizeof(Object), 0);

    // Use placement new to initialize the object in the memory that Lua allocated
    new(pUserData) Object(xInterface, xInvocationFactory);

    pushMetatable(pLuaState);
    lua_setmetatable(pLuaState, -2);
}

void Object::pushMetatable(lua_State* pLuaState)
{
    if (luaL_newmetatable(pLuaState, "Luno_Object") == 0)
    {
        // The table has already been created so there’s nothing else to do
        return;
    }

    lua_pushliteral(pLuaState, "__gc");
    lua_pushcfunction(pLuaState, gc);
    lua_rawset(pLuaState, -3);
}

int Object::gc(lua_State* pLuaState)
{
    if (lua_gettop(pLuaState) != 1)
        return 0;

    Object* pObject = reinterpret_cast<Object*>(lua_touserdata(pLuaState, 1));
    // The memory was allocated by Lua so we only need to call the destructor
    pObject->~Object();

    return 0;
}
}
