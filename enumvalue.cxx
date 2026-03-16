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

#include "enumvalue.hxx"

#include <assert.h>

namespace uk::co::busydoingnothing::luno
{
void EnumValue::pushEnumValue(lua_State* pLuaState, const css::uno::Type& xType, sal_Int32 nValue,
                              const Runtime& rRuntime)
{
    assert(xType.getTypeClass() == css::uno::TypeClass_ENUM);

    void *pUserData = lua_newuserdatauv(pLuaState, sizeof(EnumValue), 0);

    // Use placement new to initialize the type in the memory that Lua allocated
    new(pUserData) EnumValue(xType, nValue, rRuntime);

    pushMetatable(pLuaState);
    lua_setmetatable(pLuaState, -2);
}

void EnumValue::pushMetatable(lua_State* pLuaState)
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

EnumValue* EnumValue::checkEnumValue(lua_State* pLuaState, int nArg)
{
    return reinterpret_cast<EnumValue*>(luaL_checkudata(pLuaState, nArg, CLASS_NAME));
}

EnumValue* EnumValue::testEnumValue(lua_State* pLuaState, int nArg)
{
    return reinterpret_cast<EnumValue*>(luaL_testudata(pLuaState, nArg, CLASS_NAME));
}

int EnumValue::gc(lua_State* pLuaState)
{
    EnumValue* pEnumValue = checkEnumValue(pLuaState, 1);

    // The memory was allocated by Lua so we only need to call the destructor
    pEnumValue->~EnumValue();

    return 0;
}

int EnumValue::doIndex(lua_State* pLuaState)
{
    // Check if the key is “value”
    lua_pushliteral(pLuaState, "value");
    int nIsEqual = lua_compare(pLuaState, 2, -1, LUA_OPEQ);
    lua_pop(pLuaState, 1);

    if (nIsEqual)
    {
        lua_pushinteger(pLuaState, m_nValue);
        return 1;
    }
    else
        return 0;
}

int EnumValue::index(lua_State* pLuaState)
{
    EnumValue* pEnumValue = checkEnumValue(pLuaState, 1);

    return pEnumValue->doIndex(pLuaState);
}

int EnumValue::doEq(lua_State* pLuaState)
{
    EnumValue* pOther = testEnumValue(pLuaState, 2);

    lua_pushboolean(pLuaState, pOther != nullptr && m_xType == pOther->m_xType &&
                    m_nValue == pOther->m_nValue);

    return 1;
}

int EnumValue::eq(lua_State* pLuaState)
{
    EnumValue* pEnumValue = checkEnumValue(pLuaState, 1);

    return pEnumValue->doEq(pLuaState);
}
}
