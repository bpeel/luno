/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4; fill-column: 100 -*- */
/*
 * This file is part of Luno.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef INCLUDED_LUNO_STRUCT_H
#define INCLUDED_LUNO_STRUCT_H

#include <lua.hpp>
#include <com/sun/star/uno/Any.hxx>
#include <com/sun/star/uno/Reference.hxx>
#include "runtime.hxx"

namespace com::sun::star::reflection
{
class XIdlClass;
}

namespace uk::co::busydoingnothing::luno
{
class Struct
{
public:
    static void pushStruct(lua_State* pLuaState,
                           const css::uno::Any& xValue,
                           const Runtime& rRuntime);

    static Struct* testStruct(lua_State* pLuaState, int nArg);
    css::uno::Any getValue() { return m_xValue; }
    // This should only be called with an Any containing the same struct type as the previous value
    void setValue(const css::uno::Any& xValue);
    css::uno::Reference<css::reflection::XIdlClass> getType();

private:
    Struct(const css::uno::Any& xValue, const Runtime& rRuntime)
        : m_rRuntime(rRuntime)
        , m_xValue(xValue)
    {
    }

    static constexpr const char* CLASS_NAME = "Luno_Struct";

    const Runtime& m_rRuntime;
    css::uno::Any m_xValue;
    css::uno::Reference<css::reflection::XIdlClass> m_xIdlClass;

    static void pushMetatable(lua_State* pLuaState);
    static Struct* checkStruct(lua_State* pLuaState, int nArg);
    static int gc(lua_State* pLuaState);
    bool ensureIdlClass();
    int doIndex(lua_State* pLuaState);
    static int index(lua_State* pLuaState);
    int doNewIndex(lua_State* pLuaState);
    static int newIndex(lua_State* pLuaState);
};
}

#endif

/* vim:set shiftwidth=4 softtabstop=4 expandtab cinoptions=b1,g0,N-s cinkeys+=0=break: */
