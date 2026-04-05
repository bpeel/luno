/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4; fill-column: 100 -*- */
/*
 * This file is part of Luno.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include <cppuhelper/factory.hxx>
#include <cppuhelper/implementationentry.hxx>
#include <uno/lbnames.h>
#include <com/sun/star/lang/XMultiServiceFactory.hpp>

#include "scriptprovider.hxx"
#include "luno.hxx"

using namespace uk::co::busydoingnothing::luno;

namespace
{
const struct ::cppu::ImplementationEntry s_component_entries[]
    = { { Luno::create, Luno::getImplementationNameStatic, Luno::getSupportedServiceNamesStatic,
          cppu::createSingleComponentFactory, 0, 0 },
        { ScriptProvider::create, ScriptProvider::getImplementationNameStatic,
          ScriptProvider::getSupportedServiceNamesStatic, cppu::createSingleComponentFactory, 0,
          0 },
        { 0, 0, 0, 0, 0, 0 } };
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
} // extern C

/* vim:set shiftwidth=4 softtabstop=4 expandtab cinoptions=b1,g0,N-s cinkeys+=0=break: */
