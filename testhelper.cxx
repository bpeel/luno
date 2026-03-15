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

#include "testhelper.hxx"

#include <uk/co/busydoingnothing/luno/qa/XTestHelper.hpp>

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

rtl::OUString SAL_CALL TestHelper::getImplementationName()
{
    return getImplementationNameStatic();
}

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
