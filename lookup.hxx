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
#include "runtime.hxx"

namespace uk::co::busydoingnothing::luno
{
// Pushes a function that is intended to be used as the __index of a
// metatable in order to look up the next step in the chain of UNO
// modules. Pops one argument off the stack to use as the prefix for
// lookups
void pushLookupFunc(lua_State* pLuaState, const Runtime& rRuntime);
}

/* vim:set shiftwidth=4 softtabstop=4 expandtab cinoptions=b1,g0,N-s cinkeys+=0=break: */
