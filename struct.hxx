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
