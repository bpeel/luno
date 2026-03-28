/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4; fill-column: 100 -*- */
/*
 * This file is part of Luno.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#pragma once

#include <com/sun/star/lang/XInitialization.hpp>
#include <com/sun/star/lang/XServiceInfo.hpp>
#include <cppuhelper/implbase3.hxx>
#include <uk/co/busydoingnothing/luno/qa/XTestHelper.hpp>

namespace com::sun::star::uno
{
class XComponentContext;
}

namespace uk::co::busydoingnothing::luno::qa
{
class TestHelper
    : public cppu::WeakImplHelper3<css::lang::XInitialization, XTestHelper, css::lang::XServiceInfo>
{
public:
    // XInitialization
    void SAL_CALL initialize(const css::uno::Sequence<css::uno::Any>& aArguments) override;

    // XServiceInfo
    rtl::OUString SAL_CALL getImplementationName() override;
    sal_Bool SAL_CALL supportsService(rtl::OUString const& serviceName) override;
    css::uno::Sequence<rtl::OUString> SAL_CALL getSupportedServiceNames() override;

    // XTestHelper
    void SAL_CALL modifyStruct(sal_Int32 nSetLongValue, TestStruct& aSetLongStruct,
                               const rtl::OUString& sSetStringValue,
                               TestStruct& aSetStringStruct) override;
    void SAL_CALL modifySequence(sal_Int32 nFirstValue, sal_Int32 nSecondValue,
                                 sal_Int32 nThirdValue,
                                 css::uno::Sequence<sal_Int32>& aValues) override;
    sal_Int32 SAL_CALL multipleReturn(sal_Int32 mainReturnValue, sal_Int32 secondReturnValue,
                                      sal_Int32& secondReturn, sal_Int32& fourthInputThirdOutput,
                                      sal_Int32 thirdReturnValue, sal_Int32& fourthReturn) override;
    TestEnum SAL_CALL getFourEnum() override;
    sal_Int32 SAL_CALL getEnumValue(TestEnum nEnum) override;
    void SAL_CALL throwException() override;
    css::uno::Sequence<css::uno::Any> SAL_CALL getArguments() override;
    css::uno::Reference<XTestAttributes> SAL_CALL getAttributes() override;

    static rtl::OUString getImplementationNameStatic();
    static css::uno::Sequence<rtl::OUString> getSupportedServiceNamesStatic();
    static css::uno::Reference<css::uno::XInterface>
    create(const css::uno::Reference<css::uno::XComponentContext>& xContext);

private:
    css::uno::Sequence<css::uno::Any> m_aArgs;
};
}

/* vim:set shiftwidth=4 softtabstop=4 expandtab cinoptions=b1,g0,N-s cinkeys+=0=break: */
