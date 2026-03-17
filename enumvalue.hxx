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
#include <com/sun/star/uno/Type.hxx>
#include "runtime.hxx"

namespace uk::co::busydoingnothing::luno
{
class EnumValue
{
public:
    static void pushEnumValue(lua_State* pLuaState, const css::uno::Type& xType, sal_Int32 nValue,
                              const Runtime& rRuntime);

    static EnumValue* testEnumValue(lua_State* pLuaState, int nArg);

    css::uno::Type getType() const { return m_xType; }

    sal_Int32 getValue() const { return m_nValue; }

private:
    EnumValue(const css::uno::Type& xType, sal_Int32 nValue, const Runtime& rRuntime)
        : m_rRuntime(rRuntime)
        , m_xType(xType)
        , m_nValue(nValue)
    {
    }

    static constexpr const char* CLASS_NAME = "Luno_EnumValue";

    const Runtime& m_rRuntime;
    css::uno::Type m_xType;
    sal_Int32 m_nValue;

    static void pushMetatable(lua_State* pLuaState);
    static EnumValue* checkEnumValue(lua_State* pLuaState, int nArg);
    static int gc(lua_State* pLuaState);
    int doIndex(lua_State* pLuaState);
    static int index(lua_State* pLuaState);
    int doEq(lua_State* pLuaState);
    static int eq(lua_State* pLuaState);
};
}

/* vim:set shiftwidth=4 softtabstop=4 expandtab cinoptions=b1,g0,N-s cinkeys+=0=break: */
