/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4; fill-column: 100 -*- */
/*
 * This file is part of Luno.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "enumtype.hxx"

#include <com/sun/star/reflection/XEnumTypeDescription.hpp>

namespace uk::co::busydoingnothing::luno
{
void EnumType::pushEnumType(
    lua_State* pLuaState,
    const css::uno::Reference<css::reflection::XEnumTypeDescription>& xTypeDescription,
    const Runtime& rRuntime)
{
    void* pUserData = lua_newuserdatauv(pLuaState, sizeof(EnumType), 0);

    // Use placement new to initialize the type in the memory that Lua allocated
    new (pUserData) EnumType(xTypeDescription, rRuntime);

    pushMetatable(pLuaState);
    lua_setmetatable(pLuaState, -2);
}

void EnumType::pushMetatable(lua_State* pLuaState)
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

EnumType* EnumType::checkEnumType(lua_State* pLuaState, int nArg)
{
    return reinterpret_cast<EnumType*>(luaL_checkudata(pLuaState, nArg, CLASS_NAME));
}

EnumType* EnumType::testEnumType(lua_State* pLuaState, int nArg)
{
    return reinterpret_cast<EnumType*>(luaL_testudata(pLuaState, nArg, CLASS_NAME));
}

int EnumType::gc(lua_State* pLuaState)
{
    EnumType* pEnumType = checkEnumType(pLuaState, 1);

    // The memory was allocated by Lua so we only need to call the destructor
    pEnumType->~EnumType();

    return 0;
}

int EnumType::doIndexUncached(lua_State* pLuaState)
{
    if (!m_xTypeDescription.is())
    {
        luaL_error(pLuaState, "__index called on enum in an invalid state");
        return 0;
    }

    size_t nKeyLength;
    const char* pKey = luaL_checklstring(pLuaState, 2, &nKeyLength);

    rtl::OUString sKey(pKey, nKeyLength, RTL_TEXTENCODING_UTF8);
    css::uno::Sequence<rtl::OUString> aEnumNames = m_xTypeDescription->getEnumNames();

    for (int i = 0, nCount = aEnumNames.getLength(); i < nCount; ++i)
    {
        if (aEnumNames[i] == sKey)
        {
            css::uno::Type xType(css::uno::TypeClass_ENUM, m_xTypeDescription->getName());
            EnumValue::pushEnumValue(pLuaState, xType, m_xTypeDescription->getEnumValues()[i]);
            return 1;
        }
    }

    lua_pushnil(pLuaState);
    return 1;
}

int EnumType::doIndex(lua_State* pLuaState)
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
        // Lazily look up the value
        lua_pop(pLuaState, 1);
        doIndexUncached(pLuaState);

        if (!lua_isnil(pLuaState, -1))
        {
            // Add the value to the cache
            lua_pushvalue(pLuaState, 2);
            lua_pushvalue(pLuaState, -2);
            lua_rawset(pLuaState, -4);
        }
    }

    // Remove the cache table from the stack
    lua_remove(pLuaState, -2);

    return 1;
}

int EnumType::index(lua_State* pLuaState)
{
    EnumType* pEnumType = checkEnumType(pLuaState, 1);

    return pEnumType->doIndex(pLuaState);
}
}

/* vim:set shiftwidth=4 softtabstop=4 expandtab cinoptions=b1,g0,N-s cinkeys+=0=break: */
