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

#ifndef INCLUDED_LUNO_CONVERSIONS_H
#define INCLUDED_LUNO_CONVERSIONS_H

#include <lua.hpp>
#include <com/sun/star/uno/Any.hxx>

namespace com::sun::star::lang
{
class XSingleServiceFactory;
}

namespace uk::co::busydoingnothing::luno
{
void pushAny(lua_State* pLuaState,
             const css::uno::Any& xAny,
             const css::uno::Reference<css::lang::XSingleServiceFactory> xSingleServiceFactory);
css::uno::Any getAny(lua_State* pLuaState, int nIndex);
}

#endif
