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

#ifndef INCLUDED_ENUM_TYPE_H
#define INCLUDED_ENUM_TYPE_H

#include <lua.hpp>
#include <com/sun/star/uno/Reference.hxx>
#include "runtime.hxx"
#include "enumvalue.hxx"

namespace com::sun::star::reflection
{
class XEnumTypeDescription;
}

namespace uk::co::busydoingnothing::luno
{
class EnumType
{
public:
    static void pushEnumType(
        lua_State* pLuaState,
        const css::uno::Reference<css::reflection::XEnumTypeDescription>& xTypeDescription,
        const Runtime& rRuntime);

    static EnumType* testEnumType(lua_State* pLuaState, int nArg);

private:
    EnumType(
        const css::uno::Reference<css::reflection::XEnumTypeDescription>& xTypeDescription,
        const Runtime& rRuntime)
        : m_rRuntime(rRuntime)
        , m_xTypeDescription(xTypeDescription)
    {
    }

    static constexpr const char* CLASS_NAME = "Luno_EnumType";

    const Runtime& m_rRuntime;
    css::uno::Reference<css::reflection::XEnumTypeDescription> m_xTypeDescription;

    static void pushMetatable(lua_State* pLuaState);
    static EnumType* checkEnumType(lua_State* pLuaState, int nArg);
    static int gc(lua_State* pLuaState);
    int doIndexUncached(lua_State* pLuaState);
    int doIndex(lua_State* pLuaState);
    static int index(lua_State* pLuaState);
};
}

#endif
