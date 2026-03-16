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

#ifndef INCLUDED_ENUM_VALUE_H
#define INCLUDED_ENUM_VALUE_H

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

#endif
