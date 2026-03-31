/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4; fill-column: 100 -*- */
/*
 * This file is part of Luno.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "scriptprovider.hxx"

#include <com/sun/star/script/provider/ScriptFrameworkErrorException.hpp>
#include <com/sun/star/script/provider/ScriptFrameworkErrorType.hpp>
#include <com/sun/star/script/provider/ScriptURIHelper.hpp>
#include <com/sun/star/ucb/SimpleFileAccess.hpp>
#include <uk/co/busydoingnothing/luno/Runner.hpp>
#include <vector>

#include "scriptbrowser.hxx"

namespace uk::co::busydoingnothing::luno
{
ScriptProvider::ScriptProvider(const css::uno::Reference<css::uno::XComponentContext>& xContext)
    : m_xContext(xContext)
{
}

void SAL_CALL ScriptProvider::initialize(const css::uno::Sequence<css::uno::Any>& aArguments)
{
    if (aArguments.getLength() > 0)
    {
        rtl::OUString sLocation;
        if (aArguments[0] >>= sLocation)
            setLocation(sLocation);
    }
}

void ScriptProvider::setLocation(const rtl::OUString& sLocation)
{
    m_xUriHelper = css::script::provider::ScriptURIHelper::create(m_xContext, "Lua", sLocation);
    m_xRootBrowser = css::uno::Reference<css::script::browse::XBrowseNode>(
        new ScriptBrowser(m_xContext, m_xUriHelper));
}

rtl::OUString SAL_CALL ScriptProvider::getName() { return "Lua"; }

css::uno::Sequence<css::uno::Reference<css::script::browse::XBrowseNode>>
    SAL_CALL ScriptProvider::getChildNodes()
{
    if (m_xRootBrowser.is())
        return m_xRootBrowser->getChildNodes();
    else
        return css::uno::Sequence<css::uno::Reference<css::script::browse::XBrowseNode>>();
}

sal_Bool SAL_CALL ScriptProvider::hasChildNodes()
{
    return m_xRootBrowser.is() && m_xRootBrowser->hasChildNodes();
}

sal_Int16 SAL_CALL ScriptProvider::getType()
{
    return m_xRootBrowser.is() ? m_xRootBrowser->getType() : 0;
}

css::uno::Reference<css::script::provider::XScript>
    SAL_CALL ScriptProvider::getScript(const rtl::OUString& sScriptUri)
{
    css::uno::Reference<css::ucb::XSimpleFileAccess3> xFileAccess
        = css::ucb::SimpleFileAccess::create(m_xContext);

    rtl::OUString sStorageUri = m_xUriHelper->getStorageURI(sScriptUri);

    sal_Int32 nSlashPos = sStorageUri.lastIndexOf('/');
    rtl::OUString sName = sStorageUri.copy(nSlashPos == -1 ? 0 : nSlashPos + 1);

    try
    {
        std::vector<sal_Int8> aSource;
        constexpr sal_Int32 nBufSize = 1024;
        css::uno::Sequence<sal_Int8> aBuf(nBufSize);

        css::uno::Reference<css::io::XInputStream> xIn = xFileAccess->openFileRead(sStorageUri);

        while (true)
        {
            sal_Int32 nBytesRead = xIn->readBytes(aBuf, aBuf.getLength());
            const sal_Int8* pBuf = aBuf.getConstArray();
            aSource.insert(aSource.end(), pBuf, pBuf + nBytesRead);

            if (nBytesRead < nBufSize)
                break;
        }

        css::uno::Sequence<sal_Int8> aSourceSequence(aSource.data(), aSource.size());

        css::uno::Reference<XRunner> xRunner = Runner::create(m_xContext);
        xRunner->setCode(sName, aSourceSequence);

        return css::uno::Reference<css::script::provider::XScript>(xRunner,
                                                                   css::uno::UNO_QUERY_THROW);
    }
    catch (const css::uno::Exception& e)
    {
        throw css::script::provider::ScriptFrameworkErrorException(
            e.Message, static_cast<css::uno::XWeak*>(this), sName, "Lua",
            css::script::provider::ScriptFrameworkErrorType::UNKNOWN);
    }
}

rtl::OUString SAL_CALL ScriptProvider::getImplementationName()
{
    return getImplementationNameStatic();
}

sal_Bool SAL_CALL ScriptProvider::supportsService(rtl::OUString const& serviceName)
{
    css::uno::Sequence<rtl::OUString> names = getSupportedServiceNames();
    rtl::OUString* pNames = names.getArray();

    for (sal_Int32 i = 0, count = names.getLength(); i < count; i++)
    {
        if (pNames[i] == serviceName)
            return true;
    }

    return false;
}

css::uno::Sequence<rtl::OUString> SAL_CALL ScriptProvider::getSupportedServiceNames()
{
    return getSupportedServiceNamesStatic();
}

rtl::OUString ScriptProvider::getImplementationNameStatic()
{
    return "uk.co.busydoingnothing.luno.ScriptProvider";
}

css::uno::Sequence<rtl::OUString> ScriptProvider::getSupportedServiceNamesStatic()
{
    css::uno::Sequence<rtl::OUString> names(2);
    rtl::OUString* pNames = names.getArray();
    pNames[0] = rtl::OUString("com.sun.star.script.provider.ScriptProviderForLua");
    pNames[1] = rtl::OUString("com.sun.star.script.provider.LanguageScriptProvider");
    return names;
}

css::uno::Reference<css::uno::XInterface>
ScriptProvider::create(const css::uno::Reference<css::uno::XComponentContext>& xContext)
{
    return static_cast<css::lang::XTypeProvider*>(new ScriptProvider(xContext));
}
}

/* vim:set shiftwidth=4 softtabstop=4 expandtab cinoptions=b1,g0,N-s cinkeys+=0=break: */
