/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4; fill-column: 100 -*- */
/*
 * This file is part of Luno.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#pragma once

#include <com/sun/star/beans/XPropertySet.hpp>
#include <com/sun/star/beans/XPropertySetInfo.hpp>
#include <com/sun/star/script/browse/XBrowseNode.hpp>
#include <cppuhelper/implbase3.hxx>

namespace com::sun::star::script::provider
{
class XScriptURIHelper;
}

namespace com::sun::star::uno
{
class XComponentContext;
}

namespace uk::co::busydoingnothing::luno
{
class ScriptFile
    : public cppu::WeakImplHelper3<css::script::browse::XBrowseNode, css::beans::XPropertySet,
                                   css::beans::XPropertySetInfo>
{
public:
    ScriptFile(const css::uno::Reference<css::uno::XComponentContext>& xContext,
               const css::uno::Reference<css::script::provider::XScriptURIHelper>& xUriHelper,
               const rtl::OUString& sUri);

    // XBrowseNode
    rtl::OUString SAL_CALL getName() override;
    css::uno::Sequence<css::uno::Reference<css::script::browse::XBrowseNode>>
        SAL_CALL getChildNodes() override;
    sal_Bool SAL_CALL hasChildNodes() override;
    sal_Int16 SAL_CALL getType() override;

    // XPropertySet
    css::uno::Reference<css::beans::XPropertySetInfo> SAL_CALL getPropertySetInfo() override;
    void SAL_CALL setPropertyValue(const rtl::OUString& aPropertyName,
                                   const css::uno::Any& aValue) override;
    css::uno::Any SAL_CALL getPropertyValue(const rtl::OUString& sPropertyName) override;
    void SAL_CALL addPropertyChangeListener(
        const rtl::OUString& aPropertyName,
        const css::uno::Reference<css::beans::XPropertyChangeListener>& xListener) override;
    void SAL_CALL removePropertyChangeListener(
        const rtl::OUString& aPropertyName,
        const css::uno::Reference<css::beans::XPropertyChangeListener>& aListener) override;
    void SAL_CALL addVetoableChangeListener(
        const rtl::OUString& PropertyName,
        const css::uno::Reference<css::beans::XVetoableChangeListener>& aListener) override;
    void SAL_CALL removeVetoableChangeListener(
        const rtl::OUString& PropertyName,
        const css::uno::Reference<css::beans::XVetoableChangeListener>& aListener) override;

    // XPropertySetInfo
    css::uno::Sequence<css::beans::Property> SAL_CALL getProperties() override;
    css::beans::Property SAL_CALL getPropertyByName(const rtl::OUString& sName) override;
    sal_Bool SAL_CALL hasPropertyByName(const rtl::OUString& sName) override;

private:
    static css::beans::Property getUriProperty();

    css::uno::Reference<css::uno::XComponentContext> m_xContext;

    css::uno::Reference<css::script::provider::XScriptURIHelper> m_xUriHelper;

    rtl::OUString m_sName;
    rtl::OUString m_sUri;
};
}

/* vim:set shiftwidth=4 softtabstop=4 expandtab cinoptions=b1,g0,N-s cinkeys+=0=break: */
