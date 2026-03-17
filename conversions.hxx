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
#include <com/sun/star/uno/Any.hxx>
#include "runtime.hxx"

namespace com::sun::star::reflection
{
class XIdlClass;
}

namespace uk::co::busydoingnothing::luno
{
void pushAny(lua_State* pLuaState,
             const css::uno::Any& xAny,
             const Runtime& rRuntime);
css::uno::Any getAny(lua_State* pLuaState, int nIndex);
css::uno::Any getAnyAsType(lua_State* pLuaState, int nIndex,
                           const css::uno::Reference<css::reflection::XIdlClass>& xDestType,
                           const Runtime& rRuntime);
}

/* vim:set shiftwidth=4 softtabstop=4 expandtab cinoptions=b1,g0,N-s cinkeys+=0=break: */
