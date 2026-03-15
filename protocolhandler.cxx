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

#include "protocolhandler.hxx"

#include <com/sun/star/frame/XFrame.hpp>
#include <com/sun/star/uno/XComponentContext.hpp>
#include <com/sun/star/text/XTextDocument.hpp>
#include <com/sun/star/view/XSelectionSupplier.hpp>
#include <com/sun/star/awt/MessageBoxButtons.hpp>
#include <com/sun/star/awt/XMessageBoxFactory.hpp>
#include <com/sun/star/awt/XMessageBox.hpp>
#include <uk/co/busydoingnothing/luno/LuaException.hpp>
#include <uk/co/busydoingnothing/luno/Runner.hpp>

using namespace rtl;

namespace uk::co::busydoingnothing::luno
{

ProtocolHandler::ProtocolHandler(const css::uno::Reference<css::uno::XComponentContext>& xContext)
    : m_xContext(xContext)
{
}

void SAL_CALL ProtocolHandler::initialize(css::uno::Sequence<css::uno::Any> const& args)
{
    if (args.getLength() > 0)
        args[0] >>= m_xFrame;
}

bool ProtocolHandler::canHandleUrl(const css::util::URL& url)
{
    return m_xFrame.is() && m_xContext.is() && url.Protocol == "uk.co.busydoingnothing.luno:"
        && url.Path == "RunCode";
}

css::uno::Sequence<css::uno::Reference<css::frame::XDispatch>>
    SAL_CALL ProtocolHandler::queryDispatches(
        const css::uno::Sequence<css::frame::DispatchDescriptor>& aRequests)
{
    sal_Int32 length = aRequests.getLength();
    css::uno::Sequence<css::uno::Reference<css::frame::XDispatch>> seq(length);

    for (sal_Int32 i = 0; i < length; i++)
    {
        seq[i] = queryDispatch(aRequests[i].FeatureURL, aRequests[i].FrameName,
                               aRequests[i].SearchFlags);
    }

    return seq;
}

css::uno::Reference<css::frame::XDispatch> SAL_CALL ProtocolHandler::queryDispatch(
    const css::util::URL& url, const rtl::OUString& sTargetFrameName, sal_Int32 nSearchFlags)
{
    if (!canHandleUrl(url))
        return css::uno::Reference<css::frame::XDispatch>();

    return this;
}

rtl::OUString SAL_CALL ProtocolHandler::getImplementationName()
{
    return getImplementationNameStatic();
}

sal_Bool SAL_CALL ProtocolHandler::supportsService(OUString const& serviceName)
{
    css::uno::Sequence<OUString> names = getSupportedServiceNames();

    for (sal_Int32 i = 0, count = names.getLength(); i < count; i++)
    {
        if (names[i] == serviceName)
            return true;
    }

    return false;
}

css::uno::Sequence<OUString> SAL_CALL ProtocolHandler::getSupportedServiceNames()
{
    return getSupportedServiceNamesStatic();
}

void SAL_CALL ProtocolHandler::addStatusListener(
    const css::uno::Reference<css::frame::XStatusListener>& xControl, const css::util::URL& aURL)
{
    css::frame::FeatureStateEvent aEvent;
    aEvent.FeatureURL = aURL;
    aEvent.Source = static_cast<css::frame::XDispatch*>(this);
    aEvent.IsEnabled = sal_False;
    aEvent.Requery = sal_False;
    aEvent.State = css::uno::Any();

    if (canHandleUrl(aURL))
    {
        css::uno::Reference<css::frame::XController> xController = m_xFrame->getController();

        if (xController.is())
        {
            css::uno::Reference<css::text::XTextDocument> xTextDocument(xController->getModel(),
                                                                        css::uno::UNO_QUERY);

            aEvent.IsEnabled = xTextDocument.is();
        }
    }

    xControl->statusChanged(aEvent);
}

void SAL_CALL ProtocolHandler::removeStatusListener(
    const css::uno::Reference<css::frame::XStatusListener>& xControl, const css::util::URL& aURL)
{
}

void ProtocolHandler::showErrorDialog(const OUString& sMessage)
{
    if (!m_xContext.is() || !m_xFrame.is())
        return;

    css::uno::Reference<css::awt::XWindowPeer> xWindowPeer(
        m_xFrame->getComponentWindow(), css::uno::UNO_QUERY);

    if (!xWindowPeer.is())
        return;

    css::uno::Reference<css::awt::XMessageBoxFactory> xMessageBoxFactory(
        m_xContext->getServiceManager()->createInstanceWithContext(
            "com.sun.star.awt.Toolkit", m_xContext), css::uno::UNO_QUERY);

    if (!xMessageBoxFactory.is())
        return;

    css::uno::Reference<css::awt::XMessageBox> xMessageBox
        = xMessageBoxFactory->createMessageBox(
            xWindowPeer,
            css::awt::MessageBoxType_ERRORBOX,
            css::awt::MessageBoxButtons::BUTTONS_OK,
            "Error",
            sMessage);

    if (xMessageBox.is())
    {
        xMessageBox->execute();

        css::uno::Reference<css::lang::XComponent> xComponent(xMessageBox, css::uno::UNO_QUERY);

        if (xComponent.is())
            xComponent->dispose();
    }
}

rtl::OUString ProtocolHandler::getSelection()
{
    css::uno::Reference<css::view::XSelectionSupplier> xSelectionSupplier(m_xFrame->getController(),
                                                                          css::uno::UNO_QUERY);

    if (!xSelectionSupplier.is())
        return rtl::OUString();

    css::uno::Any xSelection = xSelectionSupplier->getSelection();
    css::uno::Reference<css::container::XIndexAccess> xIndexAccess;
    xSelection >>= xIndexAccess;

    if (!xIndexAccess.is())
        return rtl::OUString();

    sal_Int32 nIndexCount = xIndexAccess->getCount();

    if (nIndexCount <= 0)
        return rtl::OUString();

    css::uno::Any xFirstSelectionAny = xIndexAccess->getByIndex(0);
    css::uno::Reference<css::text::XTextRange> xFirstSelection;
    xFirstSelectionAny >>= xFirstSelection;

    if (!xFirstSelection.is())
        return rtl::OUString();

    return xFirstSelection->getString();
}

void SAL_CALL ProtocolHandler::dispatch(const css::util::URL& aURL,
                                        const css::uno::Sequence<css::beans::PropertyValue>& lArgs)
{
    if (!canHandleUrl(aURL))
        return;

    rtl::OUString sSource = getSelection();

    if (sSource.getLength() <= 0)
    {
        showErrorDialog("Select some text to run as Lua code before using this function");
    }
    else
    {
        css::uno::Reference<XRunner> xRunner = Runner::create(m_xContext);

        try
        {
            xRunner->executeCode("(selected text)", sSource);
        }
        catch (const LuaException& e)
        {
            showErrorDialog(e.Message);
        }
    }
}

OUString ProtocolHandler::getImplementationNameStatic()
{
    return OUString("uk.co.busydoingnothing.luno.ProtocolHandler");
}

css::uno::Sequence<OUString> ProtocolHandler::getSupportedServiceNamesStatic()
{
    css::uno::Sequence<OUString> names(1);
    names[0] = OUString("com.sun.star.frame.ProtocolHandler");
    return names;
}

css::uno::Reference<css::uno::XInterface>
ProtocolHandler::create(const css::uno::Reference<css::uno::XComponentContext>& xContext)
{
    return static_cast<css::lang::XTypeProvider*>(new ProtocolHandler(xContext));
}

}
