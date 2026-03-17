/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4; fill-column: 100 -*- */
/*
 * This file is part of Luno.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#pragma once

#include <lua.hpp>
#include <com/sun/star/uno/Any.hxx>

namespace com::sun::star::uno
{
class XComponentContext;
}

namespace com::sun::star::lang
{
class XMultiComponentFactory;
}

namespace com::sun::star::beans
{
class XIntrospection;
}

namespace com::sun::star::reflection
{
class XIdlReflection;
}

namespace com::sun::star::container
{
class XHierarchicalNameAccess;
}

namespace com::sun::star::script
{
class XTypeConverter;
}

namespace uk::co::busydoingnothing::luno
{
struct Runtime
{
    css::uno::Reference<css::uno::XComponentContext> m_xContext;
    css::uno::Reference<css::lang::XMultiComponentFactory> m_xServiceManager;
    css::uno::Reference<css::beans::XIntrospection> m_xIntrospection;
    css::uno::Reference<css::reflection::XIdlReflection> m_xIdlReflection;
    css::uno::Reference<css::container::XHierarchicalNameAccess> m_xTypeManager;
    css::uno::Reference<css::script::XTypeConverter> m_xTypeConverter;

    bool isValid() const
    {
        return m_xContext.is() && m_xServiceManager.is() && m_xIntrospection.is() &&
            m_xIdlReflection.is() && m_xTypeManager.is() && m_xTypeConverter.is();
    }
};
}

/* vim:set shiftwidth=4 softtabstop=4 expandtab cinoptions=b1,g0,N-s cinkeys+=0=break: */
