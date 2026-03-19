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
#include <rtl/ustring.hxx>
#include <com/sun/star/reflection/ParamInfo.hpp>
#include <com/sun/star/uno/Reference.hxx>

namespace com::sun::star::reflection
{
class XIdlMethod;
}

namespace uk::co::busydoingnothing::luno
{
// A Method object caches the XdlMethod for a method. It doesn’t need to track the interface that it
// will be called with because any time it is invoked the object will be passed in as a parameter
// anyway.
class Method
{
public:
    static void pushMethod(lua_State* pLuaState,
                           const css::uno::Reference<css::reflection::XIdlMethod>& xMethod,
                           lua_CFunction pFunc);

    static Method* checkMethod(lua_State* pLuaState, int nArg);
    css::uno::Reference<css::reflection::XIdlMethod> getIdlMethod() const { return m_xMethod; }
    const css::uno::Sequence<css::reflection::ParamInfo>& getParameterInfos() const
    {
        return m_aParamInfos;
    }

private:
    Method(const css::uno::Reference<css::reflection::XIdlMethod>& xMethod, lua_CFunction pFunc)
        : m_xMethod(xMethod)
        , m_aParamInfos(xMethod->getParameterInfos())
        , m_pFunc(pFunc)
    {
    }

    static constexpr const char* CLASS_NAME = "Luno_Method";
    static constexpr const char* METHOD_CACHE_NAME = "Luno_MethodCache";

    css::uno::Reference<css::reflection::XIdlMethod> m_xMethod;
    // Cache the parameter infos so they can be accessed without having to do an allocation.
    css::uno::Sequence<css::reflection::ParamInfo> m_aParamInfos;
    lua_CFunction m_pFunc;

    static void pushMetatable(lua_State* pLuaState, lua_CFunction pFunc);
    static int gc(lua_State* pLuaState);
};
}

/* vim:set shiftwidth=4 softtabstop=4 expandtab cinoptions=b1,g0,N-s cinkeys+=0=break: */
