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
class XSingletonTypeDescription2;
}

namespace uk::co::busydoingnothing::luno
{
class Singleton
{
public:
    static void pushSingleton(
        lua_State* pLuaState,
        const css::uno::Reference<css::reflection::XSingletonTypeDescription2>& xTypeDescription,
        const Runtime& rRuntime);

    static Singleton* testSingleton(lua_State* pLuaState, int nArg);

private:
    Singleton(
        const css::uno::Reference<css::reflection::XSingletonTypeDescription2>& xTypeDescription,
        const Runtime& rRuntime)
        : m_rRuntime(rRuntime)
        , m_xTypeDescription(xTypeDescription)
    {
    }

    static constexpr const char* CLASS_NAME = "Luno_Singleton";

    const Runtime& m_rRuntime;
    css::uno::Reference<css::reflection::XSingletonTypeDescription2> m_xTypeDescription;

    static void pushMetatable(lua_State* pLuaState);
    static Singleton* checkSingleton(lua_State* pLuaState, int nArg);
    static int gc(lua_State* pLuaState);
    int doGet(lua_State* pLuaState);
    static int get(lua_State* pLuaState);
};
}

/* vim:set shiftwidth=4 softtabstop=4 expandtab cinoptions=b1,g0,N-s cinkeys+=0=break: */
