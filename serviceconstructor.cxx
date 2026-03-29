/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4; fill-column: 100 -*- */
/*
 * This file is part of Luno.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "serviceconstructor.hxx"

#include <com/sun/star/reflection/XServiceConstructorDescription.hpp>
#include <com/sun/star/uno/Sequence.hxx>

namespace uk::co::busydoingnothing::luno
{
ServiceConstructor::ServiceConstructor(
    const css::uno::Reference<css::reflection::XServiceConstructorDescription>& xConstructor)
    : m_xConstructor(xConstructor)
{
    const css::uno::Sequence<css::uno::Reference<css::reflection::XParameter>> xParameters
        = xConstructor->getParameters();

    m_aParameterTypes.reserve(xParameters.getLength());

    for (const auto& xParameter : xParameters)
    {
        css::uno::Reference<css::reflection::XTypeDescription> xTypeDescription
            = xParameter->getType();
        m_aParameterTypes.emplace_back(xTypeDescription->getName());
    }

    m_bHasRest = xParameters.getLength() > 0 && xParameters[0]->isRestParameter();
}

void ServiceConstructor::pushServiceConstructor(
    lua_State* pLuaState,
    const css::uno::Reference<css::reflection::XServiceConstructorDescription>& xConstructor)
{
    void* pUserData = lua_newuserdatauv(pLuaState, sizeof(ServiceConstructor), 0);

    // Use placement new to initialize the method in the memory that Lua allocated
    new (pUserData) ServiceConstructor(xConstructor);
    pushMetatable(pLuaState);
    lua_setmetatable(pLuaState, -2);
}

void ServiceConstructor::pushMetatable(lua_State* pLuaState)
{
    if (luaL_newmetatable(pLuaState, CLASS_NAME) == 0)
    {
        // The table has already been created so there’s nothing else to do
        return;
    }

    lua_pushliteral(pLuaState, "__gc");
    lua_pushcfunction(pLuaState, gc);
    lua_rawset(pLuaState, -3);
}

ServiceConstructor* ServiceConstructor::checkServiceConstructor(lua_State* pLuaState, int nArg)
{
    return static_cast<ServiceConstructor*>(luaL_checkudata(pLuaState, nArg, CLASS_NAME));
}

int ServiceConstructor::gc(lua_State* pLuaState)
{
    ServiceConstructor* pServiceConstructor = checkServiceConstructor(pLuaState, 1);

    // The memory was allocated by Lua so we only need to call the destructor
    pServiceConstructor->~ServiceConstructor();

    return 0;
}
}

/* vim:set shiftwidth=4 softtabstop=4 expandtab cinoptions=b1,g0,N-s cinkeys+=0=break: */
