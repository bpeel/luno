/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4; fill-column: 100 -*- */
/*
 * This file is part of Luno.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "testhelper.hxx"

#include <uk/co/busydoingnothing/luno/qa/XTestHelper.hpp>
#include <com/sun/star/lang/IllegalArgumentException.hpp>

namespace uk::co::busydoingnothing::luno::qa
{
void TestHelper::modifyStruct(sal_Int32 nSetLongValue, TestStruct& aSetLongStruct,
                              const rtl::OUString& sSetStringValue, TestStruct& aSetStringStruct)
{
    aSetLongStruct.LongValue = nSetLongValue;
    aSetStringStruct.StringValue = sSetStringValue;
}

void TestHelper::modifySequence(sal_Int32 nFirstValue, sal_Int32 nSecondValue,
                                sal_Int32 nThirdValue, css::uno::Sequence<sal_Int32>& aValues)
{
    aValues[0] = nFirstValue;
    aValues[1] = nSecondValue;
    aValues[2] = nThirdValue;
}

sal_Int32 TestHelper::multipleReturn(sal_Int32 mainReturnValue, sal_Int32 secondReturnValue,
                                     sal_Int32& secondReturn, sal_Int32& fourthInputThirdOutput,
                                     sal_Int32 thirdReturnValue, sal_Int32& fourthReturn)
{
    secondReturn = secondReturnValue;
    fourthReturn = fourthInputThirdOutput;
    fourthInputThirdOutput = thirdReturnValue;

    return mainReturnValue;
}

TestEnum TestHelper::getFourEnum() { return TestEnum::TestEnum_FOUR; }

sal_Int32 TestHelper::getEnumValue(TestEnum nEnum) { return nEnum; }

void TestHelper::throwException()
{
    throw css::lang::IllegalArgumentException(rtl::OUString("Your argument is illegal"),
                                              static_cast<XTestHelper*>(this), 0);
}

rtl::OUString SAL_CALL TestHelper::getImplementationName() { return getImplementationNameStatic(); }

sal_Bool SAL_CALL TestHelper::supportsService(rtl::OUString const& serviceName)
{
    css::uno::Sequence<rtl::OUString> names = getSupportedServiceNames();

    for (sal_Int32 i = 0, count = names.getLength(); i < count; i++)
    {
        if (names[i] == serviceName)
            return true;
    }

    return false;
}

css::uno::Sequence<rtl::OUString> SAL_CALL TestHelper::getSupportedServiceNames()
{
    return getSupportedServiceNamesStatic();
}

rtl::OUString TestHelper::getImplementationNameStatic()
{
    return rtl::OUString("uk.co.busydoingnothing.luno.qa.TestHelperImpl");
}

css::uno::Sequence<rtl::OUString> TestHelper::getSupportedServiceNamesStatic()
{
    css::uno::Sequence<rtl::OUString> names(1);
    names[0] = rtl::OUString("uk.co.busydoingnothing.luno.qa.TestHelper");
    return names;
}

css::uno::Reference<css::uno::XInterface>
TestHelper::create(const css::uno::Reference<css::uno::XComponentContext>& xContext)
{
    return static_cast<css::lang::XTypeProvider*>(new TestHelper);
}
}

/* vim:set shiftwidth=4 softtabstop=4 expandtab cinoptions=b1,g0,N-s cinkeys+=0=break: */
