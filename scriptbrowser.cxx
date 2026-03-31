/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4; fill-column: 100 -*- */
/*
 * This file is part of Luno.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "scriptbrowser.hxx"

#include <com/sun/star/script/browse/BrowseNodeTypes.hpp>
#include <com/sun/star/script/provider/XScriptURIHelper.hpp>
#include <com/sun/star/ucb/SimpleFileAccess.hpp>
#include <vector>

#include "scriptfile.hxx"

namespace uk::co::busydoingnothing::luno
{
ScriptBrowser::ScriptBrowser(
    const css::uno::Reference<css::uno::XComponentContext>& xContext,
    const css::uno::Reference<css::script::provider::XScriptURIHelper>& xUriHelper)
    : ScriptBrowser(xContext, xUriHelper, css::ucb::SimpleFileAccess::create(xContext), "Lua",
                    xUriHelper->getRootStorageURI())
{
}

ScriptBrowser::ScriptBrowser(
    const css::uno::Reference<css::uno::XComponentContext>& xContext,
    const css::uno::Reference<css::script::provider::XScriptURIHelper>& xUriHelper,
    const css::uno::Reference<css::ucb::XSimpleFileAccess3>& xFileAccess,
    const rtl::OUString& sName, const rtl::OUString& sBaseUri)
    : m_xContext(xContext)
    , m_xUriHelper(xUriHelper)
    , m_xFileAccess(xFileAccess)
    , m_sName(sName)
    , m_sBaseUri(sBaseUri)
{
}

rtl::OUString SAL_CALL ScriptBrowser::getName() { return m_sName; }

css::uno::Sequence<css::uno::Reference<css::script::browse::XBrowseNode>>
    SAL_CALL ScriptBrowser::getChildNodes()
{
    if (!m_xFileAccess->isFolder(m_sBaseUri) && m_xFileAccess->exists(m_sBaseUri))
    {
        // Each file is treated as a container with a single macro inside it
        css::uno::Sequence<css::uno::Reference<css::script::browse::XBrowseNode>> aChild(1);

        aChild.getArray()[0].set(new ScriptFile(m_xContext, m_xUriHelper, m_sBaseUri));

        return aChild;
    }

    try
    {
        css::uno::Sequence<rtl::OUString> aChildren
            = m_xFileAccess->getFolderContents(m_sBaseUri, true);

        std::vector<css::uno::Reference<css::script::browse::XBrowseNode>> aNodes;
        aNodes.reserve(aChildren.getLength());

        for (int i = 0, nCount = aChildren.getLength(); i < nCount; ++i)
        {
            if (m_xFileAccess->isFolder(aChildren[i]) || aChildren[i].endsWith(".lua"))
            {
                rtl::OUString sName;

                sal_Int32 nSlashPos = aChildren[i].lastIndexOf('/');
                sName = aChildren[i].copy(nSlashPos == -1 ? 0 : nSlashPos + 1);

                aNodes.emplace_back(new ScriptBrowser(m_xContext, m_xUriHelper, m_xFileAccess,
                                                      sName, aChildren[i]));
            }
        }

        css::uno::Sequence<css::uno::Reference<css::script::browse::XBrowseNode>> aNodesSequence(
            aNodes.size());
        css::uno::Reference<css::script::browse::XBrowseNode>* pNodesSequence
            = aNodesSequence.getArray();

        for (int i = 0, nCount = aNodes.size(); i < nCount; ++i)
            pNodesSequence[i] = std::move(aNodes[i]);

        return aNodesSequence;
    }
    catch (const css::uno::RuntimeException&)
    {
        throw;
    }
    catch (const css::uno::Exception&)
    {
        return css::uno::Sequence<css::uno::Reference<css::script::browse::XBrowseNode>>();
    }
}

sal_Bool SAL_CALL ScriptBrowser::hasChildNodes() { return true; }

sal_Int16 SAL_CALL ScriptBrowser::getType()
{
    return css::script::browse::BrowseNodeTypes::CONTAINER;
}
}

/* vim:set shiftwidth=4 softtabstop=4 expandtab cinoptions=b1,g0,N-s cinkeys+=0=break: */
