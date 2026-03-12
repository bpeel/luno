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

#ifndef INCLUDED_LUNO_OBJECT_H
#define INCLUDED_LUNO_OBJECT_H

#include <lua.hpp>
#include <com/sun/star/uno/Reference.hxx>

namespace com::sun::star::uno
{
class XInterface;
}

namespace com::sun::star::beans
{
class XIntrospection;
class XIntrospectionAccess;
}

namespace uk::co::busydoingnothing::luno
{
class Method;

class Object
{
public:
    static void pushObject(lua_State* pLuaState,
                           const css::uno::Reference<css::uno::XInterface>& xInterface,
                           const css::uno::Reference<css::beans::XIntrospection>& xIntrospection);

    static Object* testObject(lua_State* pLuaState, int nArg);
    css::uno::Reference<css::uno::XInterface> getInterface() { return m_xInterface; }

private:
    Object(const css::uno::Reference<css::uno::XInterface>& xInterface,
           const css::uno::Reference<css::beans::XIntrospection>& xIntrospection)
        : m_xInterface(xInterface)
        , m_xIntrospection(xIntrospection)
    {
    }

    static constexpr const char* CLASS_NAME = "Luno_Object";

    css::uno::Reference<css::uno::XInterface> m_xInterface;
    css::uno::Reference<css::beans::XIntrospection> m_xIntrospection;
    css::uno::Reference<css::beans::XIntrospectionAccess> m_xIntrospectionAccess;

    static void pushMetatable(lua_State* pLuaState);
    static Object* checkObject(lua_State* pLuaState, int nArg);
    static int gc(lua_State* pLuaState);
    int doIndexUncached(lua_State* pLuaState);
    int doIndex(lua_State* pLuaState);
    static int index(lua_State* pLuaState);
    int call(lua_State* pLuaState, Method* pMethod);
    static int call(lua_State* pLuaState);
};
}

#endif
