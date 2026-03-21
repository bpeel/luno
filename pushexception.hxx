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

namespace com::sun::star::uno
{
class Any;
}

namespace css = com::sun::star;

namespace uk::co::busydoingnothing::luno
{
struct Runtime;

// Pushes an exception onto the stack as a Lua object, or if the conversion didn’t work pushes a
// string describing the exception instead.
void pushExceptionFromAny(lua_State* pLuaState, const css::uno::Any& xAnyException,
                          const Runtime& rRuntime);
}

/* vim:set shiftwidth=4 softtabstop=4 expandtab cinoptions=b1,g0,N-s cinkeys+=0=break: */
