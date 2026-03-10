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

#ifndef INCLUDED_LUNO_PROTOCOL_HANDLER_H
#define INCLUDED_LUNO_PROTOCOL_HANDLER_H

#include <cppuhelper/implbase4.hxx>
#include <com/sun/star/lang/XInitialization.hpp>
#include <com/sun/star/lang/XServiceInfo.hpp>
#include <com/sun/star/frame/XDispatch.hpp>
#include <com/sun/star/frame/XDispatchProvider.hpp>

namespace com::sun::star::frame
{
class XFrame;
class XStatusListener;
}
namespace com::sun::star::uno
{
class XComponentContext;
}
namespace com::sun::star::util
{
class URL;
}

namespace uk::co::busydoingnothing::luno
{
class ProtocolHandler
    : public cppu::WeakImplHelper4<css::frame::XDispatchProvider, css::lang::XServiceInfo,
                                   css::lang::XInitialization, css::frame::XDispatch>
{
public:
    // XInitialization will be called upon createInstanceWithArguments[AndContext]()
    void SAL_CALL initialize(css::uno::Sequence<css::uno::Any> const& args) override;
    // XServiceInfo
    rtl::OUString SAL_CALL getImplementationName() override;
    sal_Bool SAL_CALL supportsService(rtl::OUString const& serviceName) override;
    css::uno::Sequence<rtl::OUString> SAL_CALL getSupportedServiceNames() override;
    // XDispatchProvider
    css::uno::Sequence<css::uno::Reference<css::frame::XDispatch>> SAL_CALL
    queryDispatches(const css::uno::Sequence<css::frame::DispatchDescriptor>& aRequests) override;
    css::uno::Reference<css::frame::XDispatch>
        SAL_CALL queryDispatch(const css::util::URL& url, const rtl::OUString& sTargetFrameName,
                               sal_Int32 nSearchFlags) override;
    // XDispatch
    void SAL_CALL addStatusListener(const css::uno::Reference<css::frame::XStatusListener>& xControl,
                                    const css::util::URL& aURL) override;
    void SAL_CALL removeStatusListener(const css::uno::Reference<css::frame::XStatusListener>& xControl,
                                       const css::util::URL& aURL) override;
    void SAL_CALL dispatch(const css::util::URL& aURL,
                           const css::uno::Sequence<css::beans::PropertyValue>& lArgs) override;

    static rtl::OUString getImplementationNameStatic();
    static css::uno::Sequence<rtl::OUString> getSupportedServiceNamesStatic();
    static css::uno::Reference<css::uno::XInterface>
    create(const css::uno::Reference<css::uno::XComponentContext>& xContext);

private:
    ProtocolHandler(const css::uno::Reference<css::uno::XComponentContext>& xContext);
    bool canHandleUrl(const css::util::URL& url);
    css::uno::Reference<css::uno::XComponentContext> m_xContext;
    css::uno::Reference<css::frame::XFrame> m_xFrame;
};
}

#endif
