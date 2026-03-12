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

#ifndef INCLUDED_LUNO_TYPE_H
#define INCLUDED_LUNO_TYPE_H

#include <lua.hpp>
#include <com/sun/star/uno/Reference.hxx>
#include "runtime.hxx"

namespace com::sun::star::reflection
{
class XIdlClass;
}

namespace uk::co::busydoingnothing::luno
{
class Type
{
public:
    static void pushType(lua_State* pLuaState,
                         const css::uno::Reference<css::reflection::XIdlClass>& xIdlClass,
                         const Runtime& rRuntime);

    static Type* testType(lua_State* pLuaState, int nArg);
    css::uno::Reference<css::reflection::XIdlClass> getIdlClass() { return m_xIdlClass; }

private:
    Type(const css::uno::Reference<css::reflection::XIdlClass>& xIdlClass,
         const Runtime& rRuntime)
        : m_rRuntime(rRuntime)
        , m_xIdlClass(xIdlClass)
    {
    }

    static constexpr const char* CLASS_NAME = "Luno_Type";

    const Runtime& m_rRuntime;
    css::uno::Reference<css::reflection::XIdlClass> m_xIdlClass;

    static void pushMetatable(lua_State* pLuaState);
    static Type* checkType(lua_State* pLuaState, int nArg);
    static int gc(lua_State* pLuaState);
};
}

#endif
