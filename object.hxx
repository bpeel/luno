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

namespace com::sun::star::uno
{
class XInterface;
}

namespace com::sun::star::beans
{
class XIntrospectionAccess;
}

namespace uk::co::busydoingnothing::luno
{
class Method;

class Object
{
public:
    static void pushObject(lua_State* pLuaState,
                           const css::uno::Reference<css::uno::XInterface>& xInterface,
                           const Runtime& rRuntime);

    static Object* testObject(lua_State* pLuaState, int nArg);
    css::uno::Reference<css::uno::XInterface> getInterface() { return m_xInterface; }

private:
    Object(const css::uno::Reference<css::uno::XInterface>& xInterface, const Runtime& rRuntime)
        : m_rRuntime(rRuntime)
        , m_xInterface(xInterface)
    {
    }

    static constexpr const char* CLASS_NAME = "Luno_Object";

    const Runtime& m_rRuntime;
    css::uno::Reference<css::uno::XInterface> m_xInterface;
    css::uno::Reference<css::beans::XIntrospectionAccess> m_xIntrospectionAccess;

    static void pushMetatable(lua_State* pLuaState);
    static Object* checkObject(lua_State* pLuaState, int nArg);
    static int gc(lua_State* pLuaState);
    int doIndexUncached(lua_State* pLuaState);
    int doIndex(lua_State* pLuaState);
    static int index(lua_State* pLuaState);
    int doEq(lua_State* pLuaState);
    static int eq(lua_State* pLuaState);
    int call(lua_State* pLuaState, Method* pMethod);
    static int call(lua_State* pLuaState);
};
}

/* vim:set shiftwidth=4 softtabstop=4 expandtab cinoptions=b1,g0,N-s cinkeys+=0=break: */
