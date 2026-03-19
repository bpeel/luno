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
#include <vector>
#include <rtl/ustring.hxx>
#include <com/sun/star/uno/Reference.hxx>
#include <typelib/typedescription.hxx>

namespace com::sun::star::reflection
{
class XParameter;
class XServiceConstructorDescription;
}

namespace uk::co::busydoingnothing::luno
{
// A ServiceConstructor object caches the XServiceConstructorDescription for a constructor.
class ServiceConstructor
{
public:
    static void pushServiceConstructor(
        lua_State* pLuaState,
        const css::uno::Reference<css::reflection::XServiceConstructorDescription>& xConstructor);

    static ServiceConstructor* checkServiceConstructor(lua_State* pLuaState, int nArg);

    css::uno::Reference<css::reflection::XServiceConstructorDescription> getDescription() const
    {
        return m_xConstructor;
    }

    const std::vector<css::uno::TypeDescription>& getParameterTypes() const
    {
        return m_aParameterTypes;
    }

    bool hasRest() const { return m_bHasRest; }

private:
    ServiceConstructor(
        const css::uno::Reference<css::reflection::XServiceConstructorDescription>& xConstructor);

    static constexpr const char* CLASS_NAME = "Luno_ServiceConstructor";

    css::uno::Reference<css::reflection::XServiceConstructorDescription> m_xConstructor;

    static void pushMetatable(lua_State* pLuaState);
    static int gc(lua_State* pLuaState);

    std::vector<css::uno::TypeDescription> m_aParameterTypes;
    bool m_bHasRest;
};
}

/* vim:set shiftwidth=4 softtabstop=4 expandtab cinoptions=b1,g0,N-s cinkeys+=0=break: */
