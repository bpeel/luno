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
#include <cppuhelper/implbase1.hxx>

namespace com::sun::star::script::provider
{
class XScriptURIHelper;
}

namespace com::sun::star::ucb
{
class XSimpleFileAccess3;
}

namespace com::sun::star::uno
{
class XComponentContext;
}

namespace uk::co::busydoingnothing::luno
{
class ScriptBrowser : public cppu::WeakImplHelper1<css::script::browse::XBrowseNode>
{
public:
    ScriptBrowser(const css::uno::Reference<css::uno::XComponentContext>& xContext,
                  const css::uno::Reference<css::script::provider::XScriptURIHelper>& xUriHelper);
    ScriptBrowser(const css::uno::Reference<css::uno::XComponentContext>& xContext,
                  const css::uno::Reference<css::script::provider::XScriptURIHelper>& xUriHelper,
                  const css::uno::Reference<css::ucb::XSimpleFileAccess3>& xFileAccess,
                  const rtl::OUString& sName, const rtl::OUString& sBaseUri);

    // XBrowseNode
    rtl::OUString SAL_CALL getName() override;
    css::uno::Sequence<css::uno::Reference<css::script::browse::XBrowseNode>>
        SAL_CALL getChildNodes() override;
    sal_Bool SAL_CALL hasChildNodes() override;
    sal_Int16 SAL_CALL getType() override;

private:
    css::uno::Reference<css::uno::XComponentContext> m_xContext;
    css::uno::Reference<css::script::provider::XScriptURIHelper> m_xUriHelper;
    css::uno::Reference<css::ucb::XSimpleFileAccess3> m_xFileAccess;

    rtl::OUString m_sName;
    rtl::OUString m_sBaseUri;
};
}

/* vim:set shiftwidth=4 softtabstop=4 expandtab cinoptions=b1,g0,N-s cinkeys+=0=break: */
