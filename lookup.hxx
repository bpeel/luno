// Luno – LibreOffice bindings for Lua
// Copyright (C) 2026  Neil Roberts
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.

#ifndef INCLUDED_LUNO_LOOKUP_H
#define INCLUDED_LUNO_LOOKUP_H

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

#endif
