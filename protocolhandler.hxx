/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4; fill-column: 100 -*- */
/*
 * This file is part of Luno.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#pragma once

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
    void showErrorDialog(const rtl::OUString& sMessage);
    rtl::OUString getSelection();

    css::uno::Reference<css::uno::XComponentContext> m_xContext;
    css::uno::Reference<css::frame::XFrame> m_xFrame;
};
}

/* vim:set shiftwidth=4 softtabstop=4 expandtab cinoptions=b1,g0,N-s cinkeys+=0=break: */
