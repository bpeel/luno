/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4; fill-column: 100 -*- */
/*
 * This file is part of Luno.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#pragma once

#include <lua.hpp>
#include <com/sun/star/uno/Reference.hxx>
#include "runtime.hxx"

namespace com::sun::star::reflection
{
class XIdlClass;
}

namespace uk::co::busydoingnothing::luno
{
class Type
{
public:
    static void pushType(lua_State* pLuaState,
                         const css::uno::Reference<css::reflection::XIdlClass>& xIdlClass,
                         const Runtime& rRuntime);

    static Type* testType(lua_State* pLuaState, int nArg);
    css::uno::Reference<css::reflection::XIdlClass> getIdlClass() { return m_xIdlClass; }

private:
    Type(const css::uno::Reference<css::reflection::XIdlClass>& xIdlClass, const Runtime& rRuntime)
        : m_rRuntime(rRuntime)
        , m_xIdlClass(xIdlClass)
    {
    }

    static constexpr const char* CLASS_NAME = "Luno_Type";

    const Runtime& m_rRuntime;
    css::uno::Reference<css::reflection::XIdlClass> m_xIdlClass;

    static void pushMetatable(lua_State* pLuaState);
    static Type* checkType(lua_State* pLuaState, int nArg);
    static int gc(lua_State* pLuaState);
    int doNewFunc(lua_State* pLuaState);
    static int newFunc(lua_State* pLuaState);
    int doEq(lua_State* pLuaState);
    static int eq(lua_State* pLuaState);
    int doIsSubclassOf(lua_State* pLuaState);
    static int isSubclassOf(lua_State* pLuaState);
    static void applyStructTable(lua_State* pLuaState);
};
}

/* vim:set shiftwidth=4 softtabstop=4 expandtab cinoptions=b1,g0,N-s cinkeys+=0=break: */
