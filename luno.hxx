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
#include <com/sun/star/lang/XServiceInfo.hpp>
#include <com/sun/star/uno/Reference.hxx>
#include <cppuhelper/implbase2.hxx>
#include <uk/co/busydoingnothing/luno/XRunner.hpp>

#include "runtime.hxx"

namespace uk::co::busydoingnothing::luno
{
class Luno
    : public cppu::WeakImplHelper2<XRunner, css::lang::XServiceInfo>
{
public:
    // XServiceInfo
    rtl::OUString SAL_CALL getImplementationName() override;
    sal_Bool SAL_CALL supportsService(rtl::OUString const& serviceName) override;
    css::uno::Sequence<rtl::OUString> SAL_CALL getSupportedServiceNames() override;
    // XRunner
    css::uno::Any SAL_CALL executeCode(
        const rtl::OUString& sName, const rtl::OUString& sCode) override;

    static rtl::OUString getImplementationNameStatic();
    static css::uno::Sequence<rtl::OUString> getSupportedServiceNamesStatic();
    static css::uno::Reference<css::uno::XInterface>
    create(const css::uno::Reference<css::uno::XComponentContext>& xContext);

private:
    Luno(const css::uno::Reference<css::uno::XComponentContext>& xContext);
    ~Luno();

    void throwLuaError();

    lua_State* m_pLuaState;
    Runtime m_aRuntime;
};
}

#endif
