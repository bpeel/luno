/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4; fill-column: 100 -*- */
/*
 * This file is part of Luno.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "type.hxx"

#include <com/sun/star/reflection/XIdlClass.hpp>
#include <vector>

#include "conversions.hxx"
#include "struct.hxx"
#include "pushexception.hxx"

namespace uk::co::busydoingnothing::luno
{
void Type::pushType(lua_State* pLuaState,
                    const css::uno::Reference<css::reflection::XIdlClass>& xIdlClass,
                    const Runtime& rRuntime)
{
    void* pUserData = lua_newuserdatauv(pLuaState, sizeof(Type), 0);

    // Use placement new to initialize the type in the memory that Lua allocated
    new (pUserData) Type(xIdlClass, rRuntime);

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

    lua_pushliteral(pLuaState, "__eq");
    lua_pushcfunction(pLuaState, eq);
    lua_rawset(pLuaState, -3);

    // Create the __index table
    lua_pushliteral(pLuaState, "__index");
    lua_newtable(pLuaState);

    lua_pushliteral(pLuaState, "new");
    lua_pushcfunction(pLuaState, newFunc);
    lua_rawset(pLuaState, -3);

    lua_pushliteral(pLuaState, "issubclassof");
    lua_pushcfunction(pLuaState, isSubclassOf);
    lua_rawset(pLuaState, -3);

    lua_rawset(pLuaState, -3);
}

Type* Type::checkType(lua_State* pLuaState, int nArg)
{
    return static_cast<Type*>(luaL_checkudata(pLuaState, nArg, CLASS_NAME));
}

Type* Type::testType(lua_State* pLuaState, int nArg)
{
    return static_cast<Type*>(luaL_testudata(pLuaState, nArg, CLASS_NAME));
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
        pushExceptionFromAny(pLuaState, css::uno::Any(e), m_rRuntime);
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

int Type::doEq(lua_State* pLuaState)
{
    Type* pOther = testType(pLuaState, 2);

    lua_pushboolean(pLuaState, pOther != nullptr && pOther->m_xIdlClass.is() && m_xIdlClass.is()
                                   && m_xIdlClass->equals(pOther->m_xIdlClass));

    return 1;
}

int Type::eq(lua_State* pLuaState)
{
    Type* pType = checkType(pLuaState, 1);
    return pType->doEq(pLuaState);
}

int Type::doIsSubclassOf(lua_State* pLuaState)
{
    Type* pOther = checkType(pLuaState, 2);

    if (!m_xIdlClass.is() || !pOther->m_xIdlClass.is())
    {
        lua_pushboolean(pLuaState, false);
        return 1;
    }

    // Shortcut the case where the types are the same
    if (m_xIdlClass->equals(pOther->m_xIdlClass))
    {
        lua_pushboolean(pLuaState, true);
        return 1;
    }

    // Recursively check all of the parents subclasses
    std::vector<css::uno::Reference<css::reflection::XIdlClass>> aStack{ m_xIdlClass };

    while (!aStack.empty())
    {
        css::uno::Reference<css::reflection::XIdlClass> xParent = std::move(aStack.end()[-1]);
        aStack.pop_back();

        if (xParent->equals(pOther->m_xIdlClass))
        {
            lua_pushboolean(pLuaState, true);
            return 1;
        }

        const css::uno::Sequence<css::uno::Reference<css::reflection::XIdlClass>> aParents
            = xParent->getSuperclasses();
        aStack.insert(aStack.end(), aParents.begin(), aParents.end());
    }

    lua_pushboolean(pLuaState, false);
    return 1;
}

int Type::isSubclassOf(lua_State* pLuaState)
{
    Type* pType = checkType(pLuaState, 1);
    return pType->doIsSubclassOf(pLuaState);
}
}

/* vim:set shiftwidth=4 softtabstop=4 expandtab cinoptions=b1,g0,N-s cinkeys+=0=break: */
