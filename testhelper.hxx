// TestHelper – LibreOffice bindings for Lua
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

#ifndef INCLUDED_TEST_HELPER_H
#define INCLUDED_TEST_HELPER_H

#include <com/sun/star/lang/XServiceInfo.hpp>
#include <cppuhelper/implbase2.hxx>
#include <uk/co/busydoingnothing/luno/qa/XTestHelper.hpp>

namespace com::sun::star::uno
{
class XComponentContext;
}

namespace uk::co::busydoingnothing::luno::qa
{
class TestHelper
    : public cppu::WeakImplHelper2<XTestHelper, css::lang::XServiceInfo>
{
public:
    // XServiceInfo
    rtl::OUString SAL_CALL getImplementationName() override;
    sal_Bool SAL_CALL supportsService(rtl::OUString const& serviceName) override;
    css::uno::Sequence<rtl::OUString> SAL_CALL getSupportedServiceNames() override;

    // XTestHelper
    void modifyStruct(sal_Int32 nSetLongValue, TestStruct& aSetLongStruct,
                      const rtl::OUString& sSetStringValue, TestStruct& aSetStringStruct) override;
    sal_Int32 multipleReturn(sal_Int32 mainReturnValue, sal_Int32 secondReturnValue,
                             sal_Int32& secondReturn, sal_Int32& fourthInputThirdOutput,
                             sal_Int32 thirdReturnValue, sal_Int32& fourthReturn) override;

    static rtl::OUString getImplementationNameStatic();
    static css::uno::Sequence<rtl::OUString> getSupportedServiceNamesStatic();
    static css::uno::Reference<css::uno::XInterface>
    create(const css::uno::Reference<css::uno::XComponentContext>& xContext);
};
}

#endif
