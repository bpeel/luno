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

#ifndef INCLUDED_LUNO_METHOD_H
#define INCLUDED_LUNO_METHOD_H

#include <lua.hpp>
#include <rtl/ustring.hxx>
#include <com/sun/star/uno/Reference.hxx>

namespace com::sun::star::reflection
{
class XIdlMethod;
}

namespace uk::co::busydoingnothing::luno
{
// A Method object caches the XdlMethod for a method. It doesn’t need to track the interface that it
// will be called with because any time it is invoked the object will be passed in as a parameter
// anyway.
class Method
{
public:
    static void pushMethod(lua_State* pLuaState,
                           const css::uno::Reference<css::reflection::XIdlMethod>& xMethod,
                           lua_CFunction pFunc);

    static Method* checkMethod(lua_State* pLuaState, int nArg);
    css::uno::Reference<css::reflection::XIdlMethod> getIdlMethod() const
    {
        return m_xMethod;
    }

private:
    Method(const css::uno::Reference<css::reflection::XIdlMethod>& xMethod,
           lua_CFunction pFunc)
        : m_xMethod(xMethod)
        , m_pFunc(pFunc)
    {
    }

    static constexpr const char* CLASS_NAME = "Luno_Method";
    static constexpr const char* METHOD_CACHE_NAME = "Luno_MethodCache";

    css::uno::Reference<css::reflection::XIdlMethod> m_xMethod;
    lua_CFunction m_pFunc;

    static void pushMetatable(lua_State* pLuaState, lua_CFunction pFunc);
    static int gc(lua_State* pLuaState);
};
}

#endif
