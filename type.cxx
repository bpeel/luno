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

#include "conversions.hxx"
#include "struct.hxx"

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

    // Create the __index table
    lua_pushliteral(pLuaState, "__index");
    lua_newtable(pLuaState);

    lua_pushliteral(pLuaState, "new");
    lua_pushcfunction(pLuaState, newFunc);
    lua_rawset(pLuaState, -3);

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

void Type::applyStructTable(lua_State* pLuaState)
{
    if (lua_isnil(pLuaState, 2))
        return;

    if (!lua_istable(pLuaState, 2))
        luaL_typeerror(pLuaState, 2, "table");

    // Try setting all of the key/value pairs from the table argument on the new struct instance
    lua_pushnil(pLuaState);
    while (lua_next(pLuaState, 2))
    {
        lua_pushvalue(pLuaState, -2);
        lua_pushvalue(pLuaState, -2);
        lua_settable(pLuaState, -5);
        // Pop the value and leave the key for the next iteration
        lua_pop(pLuaState, 1);
    }
}

int Type::doNewFunc(lua_State* pLuaState)
{
    if (!m_xIdlClass.is() || !m_rRuntime.isValid())
        luaL_error(pLuaState, "new called on a type in an invalid state");

    bool hasArgument = lua_gettop(pLuaState) > 1;

    try
    {
        css::uno::Any xAny;
        m_xIdlClass->createObject(xAny);

        if (!xAny.hasValue())
        {
            lua_pushliteral(pLuaState, "new called on a type which can not be constructed");
            goto set_lua_error;
        }

        pushAny(pLuaState, xAny, m_rRuntime);
    }
    catch (css::uno::Exception& e)
    {
        rtl::OString sMessage = rtl::OUStringToOString(e.Message, RTL_TEXTENCODING_UTF8);
        lua_pushlstring(pLuaState, sMessage.getStr(), sMessage.getLength());
        goto set_lua_error;
    }

    if (hasArgument && Struct::testStruct(pLuaState, -1))
        applyStructTable(pLuaState);

    return 1;

    // The goto is to make sure the destructors are all called before letting Lua do a longjmp
 set_lua_error:
    lua_error(pLuaState);
    return 0;
}

int Type::newFunc(lua_State* pLuaState)
{
    Type* pType = checkType(pLuaState, 1);
    return pType->doNewFunc(pLuaState);
}
}
