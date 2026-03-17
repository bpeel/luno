/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4; fill-column: 100 -*- */
/*
 * This file is part of Luno.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "lunotype.hxx"

#include "struct.hxx"
#include "type.hxx"

namespace uk::co::busydoingnothing::luno
{
namespace
{
int lunotype(lua_State *pLuaState)
{
    Struct* pStruct = Struct::testStruct(pLuaState, 1);

    if (pStruct == nullptr)
        return 0;

    css::uno::Reference<css::reflection::XIdlClass> xIdlClass = pStruct->getType();

    if (!xIdlClass.is())
        return 0;

    const Runtime* pRuntime = static_cast<Runtime*>(lua_touserdata(pLuaState, lua_upvalueindex(1)));

    Type::pushType(pLuaState, xIdlClass, *pRuntime);
    return 1;
}
}

void setUpLunoTypeFunction(lua_State* pLuaState, const Runtime& rRuntime)
{
    // Store a pointer to the runtime as an upvalue
    lua_pushlightuserdata(pLuaState, const_cast<void*>(static_cast<const void*>(&rRuntime)));
    lua_pushcclosure(pLuaState, lunotype, 1);

    // Store the function in the global “lunotype” variable
    lua_setglobal(pLuaState, "lunotype");
}
}

/* vim:set shiftwidth=4 softtabstop=4 expandtab cinoptions=b1,g0,N-s cinkeys+=0=break: */
