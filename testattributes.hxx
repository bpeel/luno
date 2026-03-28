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
#include <cppuhelper/implbase2.hxx>
#include <uk/co/busydoingnothing/luno/qa/XTestAttributes.hpp>

namespace uk::co::busydoingnothing::luno::qa
{
class TestAttributes : public cppu::WeakImplHelper2<XTestAttributes, css::beans::XPropertySet>
{
public:
    // XTestAttributes
    sal_Int32 SAL_CALL getLongAttribute() override;
    void SAL_CALL setLongAttribute(sal_Int32 nValue) override;
    float SAL_CALL getFloatAttribute() override;
    void SAL_CALL setFloatAttribute(float nValue) override;
    css::uno::Reference<css::uno::XInterface> SAL_CALL getInterfaceAttribute() override;
    void SAL_CALL
    setInterfaceAttribute(const css::uno::Reference<css::uno::XInterface>& xInterface) override;

    // XPropertySet
    css::uno::Reference<css::beans::XPropertySetInfo> SAL_CALL getPropertySetInfo() override;
    void SAL_CALL setPropertyValue(const rtl::OUString& sPropertyName,
                                   const css::uno::Any& aValue) override;
    css::uno::Any SAL_CALL getPropertyValue(const rtl::OUString& sPropertyName) override;
    void SAL_CALL addPropertyChangeListener(
        const rtl::OUString& sPropertyName,
        const css::uno::Reference<css::beans::XPropertyChangeListener>& xListener) override;
    void SAL_CALL removePropertyChangeListener(
        const rtl::OUString& sPropertyName,
        const css::uno::Reference<css::beans::XPropertyChangeListener>& aListener) override;
    void SAL_CALL addVetoableChangeListener(
        const rtl::OUString& sPropertyName,
        const css::uno::Reference<css::beans::XVetoableChangeListener>& aListener) override;
    void SAL_CALL removeVetoableChangeListener(
        const rtl::OUString& sPropertyName,
        const css::uno::Reference<css::beans::XVetoableChangeListener>& aListener) override;

private:
    sal_Int32 m_nLongAttribute = 42;
    float m_nFloatAttribute = 3;
    css::uno::Reference<css::uno::XInterface> m_xInterfaceAttribute;
    sal_Int32 m_nLongProperty = 78;
};
}

/* vim:set shiftwidth=4 softtabstop=4 expandtab cinoptions=b1,g0,N-s cinkeys+=0=break: */
