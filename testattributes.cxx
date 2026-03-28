/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4; fill-column: 100 -*- */
/*
 * This file is part of Luno.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "testattributes.hxx"

#include <com/sun/star/beans/XPropertySetInfo.hpp>
#include <cppuhelper/implbase1.hxx>
#include <uk/co/busydoingnothing/luno/qa/XTestAttributes.hpp>

namespace uk::co::busydoingnothing::luno::qa
{
namespace
{
class PropertySetInfo : public cppu::WeakImplHelper1<css::beans::XPropertySetInfo>
{
public:
    // XPropertySetInfo
    css::uno::Sequence<css::beans::Property> SAL_CALL getProperties() override;
    css::beans::Property SAL_CALL getPropertyByName(const rtl::OUString& aName) override;
    sal_Bool SAL_CALL hasPropertyByName(const rtl::OUString& aName) override;

private:
    css::beans::Property getProperty() const;
};

css::uno::Sequence<css::beans::Property> SAL_CALL PropertySetInfo::getProperties()
{
    css::uno::Sequence<css::beans::Property> aProps(1);
    aProps[0] = getProperty();
    return aProps;
}

css::beans::Property SAL_CALL PropertySetInfo::getPropertyByName(const rtl::OUString& aName)
{
    if (aName == "LongProperty")
        return getProperty();
    else
        throw css::beans::UnknownPropertyException("Only LongProperty is supported");
}

sal_Bool SAL_CALL PropertySetInfo::hasPropertyByName(const rtl::OUString& aName)
{
    return aName == "LongProperty";
}

css::beans::Property PropertySetInfo::getProperty() const
{
    css::beans::Property aProperty = {
        /* .Name = */ "LongProperty",
        /* .Handle = */ -1,
        /* .Type = */ cppu::UnoType<sal_Int32>::get(),
        /* .Attributes = */ 0,
    };

    return aProperty;
}
}

sal_Int32 SAL_CALL TestAttributes::getLongAttribute() { return m_nLongAttribute; }

void SAL_CALL TestAttributes::setLongAttribute(sal_Int32 nLongAttribute)
{
    m_nLongAttribute = nLongAttribute;
}

float SAL_CALL TestAttributes::getFloatAttribute() { return m_nFloatAttribute; }

void SAL_CALL TestAttributes::setFloatAttribute(float nFloatAttribute)
{
    m_nFloatAttribute = nFloatAttribute;
}

css::uno::Reference<css::uno::XInterface> SAL_CALL TestAttributes::getInterfaceAttribute()
{
    return m_xInterfaceAttribute;
}

void SAL_CALL
TestAttributes::setInterfaceAttribute(const css::uno::Reference<css::uno::XInterface>& xInterface)
{
    m_xInterfaceAttribute = xInterface;
}

css::uno::Reference<css::beans::XPropertySetInfo> SAL_CALL TestAttributes::getPropertySetInfo()
{
    return new PropertySetInfo;
}

void SAL_CALL TestAttributes::setPropertyValue(const rtl::OUString& sPropertyName,
                                               const css::uno::Any& aValue)
{
    if (sPropertyName == "LongProperty")
        aValue >>= m_nLongProperty;
    else
        throw css::beans::UnknownPropertyException("Only LongProperty is supported");
}

css::uno::Any SAL_CALL TestAttributes::getPropertyValue(const rtl::OUString& sPropertyName)
{
    if (sPropertyName == "LongProperty")
    {
        css::uno::Any xAny;
        xAny <<= m_nLongProperty;
        return xAny;
    }
    else
        throw css::beans::UnknownPropertyException("Only LongProperty is supported");
}

void SAL_CALL TestAttributes::addPropertyChangeListener(
    const rtl::OUString& sPropertyName,
    const css::uno::Reference<css::beans::XPropertyChangeListener>& xListener)
{
}

void SAL_CALL TestAttributes::removePropertyChangeListener(
    const rtl::OUString& sPropertyName,
    const css::uno::Reference<css::beans::XPropertyChangeListener>& aListener)
{
}

void SAL_CALL TestAttributes::addVetoableChangeListener(
    const rtl::OUString& sPropertyName,
    const css::uno::Reference<css::beans::XVetoableChangeListener>& aListener)
{
}

void SAL_CALL TestAttributes::removeVetoableChangeListener(
    const rtl::OUString& sPropertyName,
    const css::uno::Reference<css::beans::XVetoableChangeListener>& aListener)
{
}
}

/* vim:set shiftwidth=4 softtabstop=4 expandtab cinoptions=b1,g0,N-s cinkeys+=0=break: */
