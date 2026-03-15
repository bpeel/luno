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

#ifndef INCLUDED_LUNO_RUNTIME_H
#define INCLUDED_LUNO_RUNTIME_H

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

#endif
