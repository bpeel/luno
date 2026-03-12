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

#ifndef INCLUDED_LUNO_H
#define INCLUDED_LUNO_H

#include <lua.hpp>
#include <rtl/ustring.hxx>
#include <com/sun/star/uno/Reference.hxx>

namespace com::sun::star::uno
{
class XComponentContext;
}

namespace com::sun::star::lang
{
class XMultiComponentFactory;
}
namespace com::sun::star::beans
{
class XIntrospection;
}

namespace uk::co::busydoingnothing::luno
{
class Luno
{
public:
    Luno(const css::uno::Reference<css::uno::XComponentContext>& xContext);
    ~Luno();

    void executeCode(const rtl::OUString& sCode);

private:
    void throwLuaError();

    lua_State* m_pLuaState;
    css::uno::Reference<css::uno::XComponentContext> m_xContext;
    css::uno::Reference<css::lang::XMultiComponentFactory> m_xServiceManager;
    css::uno::Reference<css::beans::XIntrospection> m_xIntrospection;
};
}

#endif
