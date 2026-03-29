/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4; fill-column: 100 -*- */
/*
 * This file is part of Luno.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "enumvalue.hxx"

#include <assert.h>

namespace uk::co::busydoingnothing::luno
{
void EnumValue::pushEnumValue(lua_State* pLuaState, const css::uno::Type& xType, sal_Int32 nValue)
{
    assert(xType.getTypeClass() == css::uno::TypeClass_ENUM);

    void* pUserData = lua_newuserdatauv(pLuaState, sizeof(EnumValue), 0);

    // Use placement new to initialize the type in the memory that Lua allocated
    new (pUserData) EnumValue(xType, nValue);

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

    lua_pushboolean(pLuaState, pOther != nullptr && m_xType == pOther->m_xType
                                   && m_nValue == pOther->m_nValue);

    return 1;
}

int EnumValue::eq(lua_State* pLuaState)
{
    EnumValue* pEnumValue = checkEnumValue(pLuaState, 1);

    return pEnumValue->doEq(pLuaState);
}
}

/* vim:set shiftwidth=4 softtabstop=4 expandtab cinoptions=b1,g0,N-s cinkeys+=0=break: */
