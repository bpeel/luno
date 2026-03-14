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

#include <cppuhelper/factory.hxx>
#include <cppuhelper/implementationentry.hxx>
#include <uno/lbnames.h>
#include <com/sun/star/lang/XMultiServiceFactory.hpp>

#include "protocolhandler.hxx"
#include "luno.hxx"

using namespace uk::co::busydoingnothing::luno;

namespace
{
const struct ::cppu::ImplementationEntry s_component_entries [] =
{
    { ProtocolHandler::create, ProtocolHandler::getImplementationNameStatic,
      ProtocolHandler::getSupportedServiceNamesStatic, cppu::createSingleComponentFactory,
      0, 0 },
    { Luno::create, Luno::getImplementationNameStatic,
      Luno::getSupportedServiceNamesStatic, cppu::createSingleComponentFactory,
      0, 0 },
    { 0, 0, 0, 0, 0, 0 }
};
}

extern "C"
{
SAL_DLLPUBLIC_EXPORT void* SAL_CALL component_getFactory(const char* implName, void* xMgr,
                                                         void* xRegistry)
{
    return cppu::component_getFactoryHelper(implName, xMgr, xRegistry, s_component_entries);
}

SAL_DLLPUBLIC_EXPORT void SAL_CALL
component_getImplementationEnvironment(char const** ppEnvTypeName, uno_Environment**)
{
    *ppEnvTypeName = CPPU_CURRENT_LANGUAGE_BINDING_NAME;
}

sal_Bool SAL_CALL component_writeInfo(css::lang::XMultiServiceFactory* xMgr,
                                      css::registry::XRegistryKey* xRegistry)
{
    return cppu::component_writeInfoHelper(xMgr, xRegistry, s_component_entries);
}
} // extern C
