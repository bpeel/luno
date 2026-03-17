/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4; fill-column: 100 -*- */
/*
 * This file is part of Luno.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "pushexception.hxx"

#include <com/sun/star/uno/Exception.hpp>
#include <com/sun/star/uno/Any.hxx>

#include "conversions.hxx"

namespace uk::co::busydoingnothing::luno
{
void pushExceptionFromAny(lua_State* pLuaState, const css::uno::Any& xAnyException,
                          const Runtime& rRuntime)
{
    try
    {
        pushAny(pLuaState, xAnyException, rRuntime);
    }
    catch (const css::uno::Exception&)
    {
        css::uno::Exception aException;
        xAnyException >>= aException;

        rtl::OString sMessage
            = rtl::OUStringToOString(rtl::OUString("UNO exception: ") + aException.Message,
                                     RTL_TEXTENCODING_UTF8);
        lua_pushlstring(pLuaState, sMessage.getStr(), sMessage.getLength());
    }
}
}

/* vim:set shiftwidth=4 softtabstop=4 expandtab cinoptions=b1,g0,N-s cinkeys+=0=break: */
