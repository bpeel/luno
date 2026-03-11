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

namespace uk::co::busydoingnothing::luno
{
// The method class just caches the OUString for the method name. It doesn’t need to track the
// interface that it will be called with because any time it is invoked the object will be passed in
// as a parameter anyway.
class Method
{
public:
    static void pushMethod(lua_State* pLuaState,
                           int nMethodNamePos,
                           const rtl::OUString& sMethodName,
                           lua_CFunction pFunc);

    static Method* checkMethod(lua_State* pLuaState, int nArg);
    rtl::OUString getMethodName() const { return m_sMethodName; }

private:
    Method(const rtl::OUString& sMethodName,
           lua_CFunction pFunc)
        : m_sMethodName(sMethodName)
        , m_pFunc(pFunc)
    {
    }

    static constexpr const char* CLASS_NAME = "Luno_Method";
    static constexpr const char* METHOD_CACHE_NAME = "Luno_MethodCache";

    rtl::OUString m_sMethodName;
    lua_CFunction m_pFunc;

    static void pushMetatable(lua_State* pLuaState, lua_CFunction pFunc);
    static int gc(lua_State* pLuaState);
};
}

#endif
