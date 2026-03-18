/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4; fill-column: 100 -*- */
/*
 * This file is part of Luno.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#pragma once

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
    void SAL_CALL setCode(const rtl::OUString& sName, const rtl::OUString& sCode) override;
    css::uno::Any SAL_CALL execute() override;

    static rtl::OUString getImplementationNameStatic();
    static css::uno::Sequence<rtl::OUString> getSupportedServiceNamesStatic();
    static css::uno::Reference<css::uno::XInterface>
    create(const css::uno::Reference<css::uno::XComponentContext>& xContext);

private:
    Luno(const css::uno::Reference<css::uno::XComponentContext>& xContext);
    ~Luno();

    static constexpr const char* CODE_NAME = "Luno_Code";

    void throwLuaError();

    lua_State* m_pLuaState;
    Runtime m_aRuntime;
};
}

/* vim:set shiftwidth=4 softtabstop=4 expandtab cinoptions=b1,g0,N-s cinkeys+=0=break: */
