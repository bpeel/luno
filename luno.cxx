/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4; fill-column: 100 -*- */
/*
 * This file is part of Luno.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "luno.hxx"

#include <rtl/string.h>
#include <com/sun/star/beans/theIntrospection.hpp>
#include <com/sun/star/container/XHierarchicalNameAccess.hpp>
#include <com/sun/star/reflection/InvocationTargetException.hpp>
#include <com/sun/star/reflection/theCoreReflection.hpp>
#include <com/sun/star/script/Converter.hpp>
#include <com/sun/star/uno/RuntimeException.hpp>
#include <com/sun/star/uno/XComponentContext.hpp>
#include <com/sun/star/lang/XMultiComponentFactory.hpp>
#include <com/sun/star/lang/XSingleServiceFactory.hpp>
#include <uk/co/busydoingnothing/luno/LuaException.hpp>

#include "object.hxx"
#include "lookup.hxx"
#include "conversions.hxx"
#include "lunotype.hxx"
#include "struct.hxx"

namespace uk::co::busydoingnothing::luno
{

Luno::Luno(const css::uno::Reference<css::uno::XComponentContext>& xContext)
    : m_pLuaState(luaL_newstate())
{
    m_aRuntime.m_xContext = xContext;
    m_aRuntime.m_xServiceManager = xContext->getServiceManager();
    m_aRuntime.m_xIntrospection = css::beans::theIntrospection::get(xContext);
    m_aRuntime.m_xIdlReflection = css::reflection::theCoreReflection::get(xContext);
    m_aRuntime.m_xInvocation.set(m_aRuntime.m_xServiceManager->createInstanceWithContext(
                                     "com.sun.star.script.Invocation", xContext),
                                 css::uno::UNO_QUERY);
    xContext->getValueByName("/singletons/com.sun.star.reflection.theTypeDescriptionManager")
        >>= m_aRuntime.m_xTypeManager;
    m_aRuntime.m_xTypeConverter = css::script::Converter::create(xContext);

    luaL_openlibs(m_pLuaState);

    // Set an __index on the global table to look up UNO modules
    lua_pushglobaltable(m_pLuaState);
    lua_newtable(m_pLuaState);
    lua_pushliteral(m_pLuaState, "__index");
    lua_pushliteral(m_pLuaState, "");
    pushLookupFunc(m_pLuaState, m_aRuntime);
    lua_rawset(m_pLuaState, -3);
    lua_setmetatable(m_pLuaState, -2);
    lua_pop(m_pLuaState, 1);

    // Set the component context as a global variable
    Object::pushObject(m_pLuaState, xContext, m_aRuntime);
    lua_setglobal(m_pLuaState, "XCONTEXT");

    // Set up the global “lunotype” function
    setUpLunoTypeFunction(m_pLuaState, m_aRuntime);
}

namespace
{
int tryToString(lua_State* pLuaState)
{
    luaL_tolstring(pLuaState, 1, nullptr);
    return 1;
}
}

void Luno::throwLuaError()
{
    // If the lua error object is already an UNO exception then throw it wrapped it an
    // InvocationTargetException
    if (Struct* pStruct = Struct::testStruct(m_pLuaState, -1))
    {
        css::uno::Any xStructValue = pStruct->getValue();

        css::reflection::InvocationTargetException aInvocationTargetException;

        if (xStructValue >>= aInvocationTargetException)
            throw aInvocationTargetException;

        css::uno::Exception aException;

        if (xStructValue >>= aException)
        {
            // Move the common parts from Exception
            static_cast<css::uno::Exception&>(aInvocationTargetException) = std::move(aException);
            aInvocationTargetException.TargetException = std::move(xStructValue);
            throw aInvocationTargetException;
        }
    }

    // Otherwise convert the object to a string and wrap it in a LuaException. Call luaL_tolstring
    // in protected mode in case the object has a __tostring metamethod which sets an error
    lua_pushcfunction(m_pLuaState, tryToString);
    lua_pushvalue(m_pLuaState, -2);

    rtl::OUString sMessage;

    if (lua_pcall(m_pLuaState, 1 /* nargs */, 1 /* nresults */, 0 /* msgh */) == LUA_OK)
    {
        size_t nMessageLength;
        const char* pMessage = lua_tolstring(m_pLuaState, -1, &nMessageLength);
        sMessage = rtl::OUString(pMessage, nMessageLength, RTL_TEXTENCODING_UTF8);
    }
    else
        sMessage = "A Lua error was encountered that couldn't be converted to a string";

    // Pop the error object and the string
    lua_pop(m_pLuaState, 2);
    throw LuaException(sMessage);
}

void SAL_CALL Luno::setCode(const rtl::OUString& sName, const css::uno::Sequence<sal_Int8>& aCode)
{
    if (!m_aRuntime.isValid())
        throw css::uno::RuntimeException("setCode called while Luno object is invalid state");

    int nRet = luaL_loadbuffer(m_pLuaState, reinterpret_cast<const char*>(aCode.getConstArray()),
                               aCode.getLength(),
                               rtl::OUStringToOString(sName, RTL_TEXTENCODING_UTF8).getStr());

    if (nRet != LUA_OK)
        throwLuaError();

    // Store the code in the registry for later execution
    lua_pushstring(m_pLuaState, CODE_NAME);
    lua_pushvalue(m_pLuaState, -2);
    lua_rawset(m_pLuaState, LUA_REGISTRYINDEX);
    lua_pop(m_pLuaState, 1);
}

css::uno::Any SAL_CALL Luno::execute()
{
    if (!m_aRuntime.isValid())
        throw css::uno::RuntimeException("execute called while Luno object is invalid state");

    // Get the compiled code that was previously stored in the registry
    lua_pushstring(m_pLuaState, CODE_NAME);
    lua_rawget(m_pLuaState, LUA_REGISTRYINDEX);

    if (lua_pcall(m_pLuaState, 0, 1, 0) != LUA_OK)
        throwLuaError();

    css::uno::Any xResult = getAny(m_pLuaState, -1);

    lua_pop(m_pLuaState, 1);

    return xResult;
}

css::uno::Any SAL_CALL Luno::invoke(const css::uno::Sequence<css::uno::Any>&,
                                    css::uno::Sequence<sal_Int16>&,
                                    css::uno::Sequence<css::uno::Any>&)
{
    try
    {
        return execute();
    }
    catch (const LuaException& e)
    {
        throw css::reflection::InvocationTargetException(
            e.Message, static_cast<css::uno::XWeak*>(this), css::uno::Any(e));
    }
}

Luno::~Luno() { lua_close(m_pLuaState); }

rtl::OUString SAL_CALL Luno::getImplementationName() { return getImplementationNameStatic(); }

sal_Bool SAL_CALL Luno::supportsService(rtl::OUString const& serviceName)
{
    css::uno::Sequence<rtl::OUString> names = getSupportedServiceNames();

    for (sal_Int32 i = 0, count = names.getLength(); i < count; i++)
    {
        if (names[i] == serviceName)
            return true;
    }

    return false;
}

css::uno::Sequence<rtl::OUString> SAL_CALL Luno::getSupportedServiceNames()
{
    return getSupportedServiceNamesStatic();
}

rtl::OUString Luno::getImplementationNameStatic() { return "uk.co.busydoingnothing.luno.Luno"; }

css::uno::Sequence<rtl::OUString> Luno::getSupportedServiceNamesStatic()
{
    css::uno::Sequence<rtl::OUString> names(1);
    names.getArray()[0] = rtl::OUString("uk.co.busydoingnothing.luno.Runner");
    return names;
}

css::uno::Reference<css::uno::XInterface>
Luno::create(const css::uno::Reference<css::uno::XComponentContext>& xContext)
{
    return static_cast<css::lang::XTypeProvider*>(new Luno(xContext));
}
}

/* vim:set shiftwidth=4 softtabstop=4 expandtab cinoptions=b1,g0,N-s cinkeys+=0=break: */
