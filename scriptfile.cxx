/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4; fill-column: 100 -*- */
/*
 * This file is part of Luno.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "scriptfile.hxx"

#include <com/sun/star/script/browse/BrowseNodeTypes.hpp>
#include <com/sun/star/script/provider/XScriptURIHelper.hpp>
#include <com/sun/star/ucb/SimpleFileAccess.hpp>

namespace uk::co::busydoingnothing::luno
{
ScriptFile::ScriptFile(
    const css::uno::Reference<css::uno::XComponentContext>& xContext,
    const css::uno::Reference<css::script::provider::XScriptURIHelper>& xUriHelper,
    const rtl::OUString& sUri)
    : m_xContext(xContext)
    , m_xUriHelper(xUriHelper)
    , m_sUri(sUri)
{
    sal_Int32 nSlashPos = sUri.lastIndexOf('/');
    sal_Int32 nStart = nSlashPos == -1 ? 0 : nSlashPos + 1;

    sal_Int32 nCount = sUri.getLength() - nStart;

    if (sUri.endsWith(".lua"))
        nCount -= 4;

    m_sName = sUri.copy(nStart, nCount);
}

rtl::OUString SAL_CALL ScriptFile::getName() { return m_sName; }

css::uno::Sequence<css::uno::Reference<css::script::browse::XBrowseNode>>
    SAL_CALL ScriptFile::getChildNodes()
{
    return css::uno::Sequence<css::uno::Reference<css::script::browse::XBrowseNode>>();
}

sal_Bool SAL_CALL ScriptFile::hasChildNodes() { return false; }

sal_Int16 SAL_CALL ScriptFile::getType() { return css::script::browse::BrowseNodeTypes::SCRIPT; }

css::uno::Reference<css::beans::XPropertySetInfo> SAL_CALL ScriptFile::getPropertySetInfo()
{
    return this;
}

void SAL_CALL ScriptFile::setPropertyValue(const rtl::OUString&, const css::uno::Any&) {}

css::uno::Any SAL_CALL ScriptFile::getPropertyValue(const rtl::OUString& sPropertyName)
{
    css::uno::Any xRet;

    if (sPropertyName == "URI")
        xRet <<= m_xUriHelper->getScriptURI(m_sUri);

    return xRet;
}

void SAL_CALL ScriptFile::addPropertyChangeListener(
    const rtl::OUString&, const css::uno::Reference<css::beans::XPropertyChangeListener>&)
{
}

void SAL_CALL ScriptFile::removePropertyChangeListener(
    const rtl::OUString&, const css::uno::Reference<css::beans::XPropertyChangeListener>&)
{
}

void SAL_CALL ScriptFile::addVetoableChangeListener(
    const rtl::OUString&, const css::uno::Reference<css::beans::XVetoableChangeListener>&)
{
}

void SAL_CALL ScriptFile::removeVetoableChangeListener(
    const rtl::OUString&, const css::uno::Reference<css::beans::XVetoableChangeListener>&)
{
}

css::uno::Sequence<css::beans::Property> SAL_CALL ScriptFile::getProperties()
{
    css::uno::Sequence<css::beans::Property> aProperties(1);
    aProperties.getArray()[0] = getUriProperty();
    return aProperties;
}

css::beans::Property SAL_CALL ScriptFile::getPropertyByName(const rtl::OUString& sName)
{
    if (sName == "URI")
        return getUriProperty();
    else
    {
        throw css::beans::UnknownPropertyException("Tried to retrieve unknown property " + sName);
    }
}

sal_Bool SAL_CALL ScriptFile::hasPropertyByName(const rtl::OUString& sName)
{
    return sName == "URI";
}

css::beans::Property ScriptFile::getUriProperty()
{
    return css::beans::Property("URI", 0, cppu::UnoType<rtl::OUString>::get(), 0);
}
}

/* vim:set shiftwidth=4 softtabstop=4 expandtab cinoptions=b1,g0,N-s cinkeys+=0=break: */
