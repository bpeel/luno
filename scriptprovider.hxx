/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4; fill-column: 100 -*- */
/*
 * This file is part of Luno.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#pragma once

#include <com/sun/star/script/browse/XBrowseNode.hpp>
#include <com/sun/star/script/provider/XScriptProvider.hpp>
#include <com/sun/star/lang/XInitialization.hpp>
#include <com/sun/star/lang/XServiceInfo.hpp>
#include <com/sun/star/lang/XSingleComponentFactory.hpp>
#include <cppuhelper/implbase4.hxx>

namespace com::sun::star::script::provider
{
class XScriptURIHelper;
}

namespace uk::co::busydoingnothing::luno
{
class ScriptBrowser;

class ScriptProvider
    : public cppu::WeakImplHelper4<css::lang::XInitialization, css::script::browse::XBrowseNode,
                                   css::script::provider::XScriptProvider, css::lang::XServiceInfo>
{
public:
    // XInitialization
    void SAL_CALL initialize(const css::uno::Sequence<css::uno::Any>& aArguments) override;

    // XServiceInfo
    rtl::OUString SAL_CALL getImplementationName() override;
    sal_Bool SAL_CALL supportsService(rtl::OUString const& serviceName) override;
    css::uno::Sequence<rtl::OUString> SAL_CALL getSupportedServiceNames() override;

    // XBrowseNode
    rtl::OUString SAL_CALL getName() override;
    css::uno::Sequence<css::uno::Reference<css::script::browse::XBrowseNode>>
        SAL_CALL getChildNodes() override;
    sal_Bool SAL_CALL hasChildNodes() override;
    sal_Int16 SAL_CALL getType() override;

    // XScriptProvider
    css::uno::Reference<css::script::provider::XScript>
        SAL_CALL getScript(const rtl::OUString& sScriptUri) override;

    static rtl::OUString getImplementationNameStatic();
    static css::uno::Sequence<rtl::OUString> getSupportedServiceNamesStatic();
    static css::uno::Reference<css::uno::XInterface>
    create(const css::uno::Reference<css::uno::XComponentContext>& xContext);

private:
    ScriptProvider(const css::uno::Reference<css::uno::XComponentContext>& xContext);

    void setLocation(const rtl::OUString& sLocation);

    css::uno::Reference<css::uno::XComponentContext> m_xContext;
    css::uno::Reference<css::script::provider::XScriptURIHelper> m_xUriHelper;
    css::uno::Reference<css::script::browse::XBrowseNode> m_xRootBrowser;
};
}

/* vim:set shiftwidth=4 softtabstop=4 expandtab cinoptions=b1,g0,N-s cinkeys+=0=break: */
