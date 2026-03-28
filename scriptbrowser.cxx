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
