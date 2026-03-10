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

#include "luno.hxx"

#include <rtl/string.h>
#include <iostream>
#include <com/sun/star/uno/XComponentContext.hpp>
#include <com/sun/star/lang/XSingleServiceFactory.hpp>
#include <com/sun/star/lang/XMultiComponentFactory.hpp>

#include "object.hxx"

namespace uk::co::busydoingnothing::luno
{

Luno::Luno(const css::uno::Reference<css::uno::XComponentContext>& xContext)
    : m_pLuaState(luaL_newstate())
    , m_xContext(xContext)
    , m_xServiceManager(xContext->getServiceManager())
    , m_xInvocationFactory(m_xServiceManager->createInstanceWithContext(
                               "com.sun.star.script.Invocation", xContext),
                           css::uno::UNO_QUERY)
{
    if (!m_xInvocationFactory.is())
        return;

    // Set the component context as a global variable
    Object::pushObject(m_pLuaState, xContext, m_xInvocationFactory);
    lua_setglobal(m_pLuaState, "XSCRIPTCONTEXT");
}

void Luno::executeCode(const rtl::OUString& sCode)
{
    if (!m_xContext.is() || !m_xServiceManager.is() || !m_xInvocationFactory.is())
        return;

    rtl::OString sCodeUtf8 = rtl::OUStringToOString(sCode, RTL_TEXTENCODING_UTF8);

    int nRet = luaL_loadbuffer(m_pLuaState, sCodeUtf8.getStr(), sCodeUtf8.getLength(),
                               "(uno text)");

    if (nRet != LUA_OK)
    {
        std::cout << "lua error: " << lua_tostring(m_pLuaState, -1) << std::endl;
        lua_pop(m_pLuaState, 1);
        return;
    }

    lua_pcall(m_pLuaState, 0, 1, 0);
    std::cout << "lua result: " << lua_tostring(m_pLuaState, -1) << std::endl;
    lua_pop(m_pLuaState, 1);
    return;
}

Luno::~Luno()
{
    lua_close(m_pLuaState);
}

}
