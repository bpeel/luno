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

#include "conversions.hxx"

#include <com/sun/star/beans/IllegalTypeException.hpp>
#include <com/sun/star/reflection/XIdlArray.hpp>
#include <com/sun/star/reflection/XIdlClass.hpp>
#include <com/sun/star/reflection/XIdlReflection.hpp>
#include <com/sun/star/uno/Sequence.hxx>
#include <sal/types.h>

#include "object.hxx"
#include "struct.hxx"

namespace uk::co::busydoingnothing::luno
{
namespace
{
bool pushSequence(lua_State* pLuaState,
                  const css::uno::Any& xAny,
                  const Runtime& rRuntime)
{
    css::uno::Reference<css::reflection::XIdlClass> xClass
        = rRuntime.m_xIdlReflection->getType(xAny);

    if (!xClass.is())
        return false;

    css::uno::Reference<css::reflection::XIdlArray> xIdlArray = xClass->getArray();

    if (!xIdlArray.is())
        return false;

    lua_newtable(pLuaState);

    for (int i = 0, nCount = xIdlArray->getLen(xAny); i < nCount; ++i)
    {
        try
        {
            pushAny(pLuaState, xIdlArray->get(xAny, i), rRuntime);
        }
        catch (css::uno::Exception&)
        {
            lua_pop(pLuaState, 1);
            throw;
        }

        lua_rawseti(pLuaState, -2, i + 1);
    }

    return true;
}

css::uno::Any convertInteger(lua_Integer nValue)
{
    css::uno::Any xAny;

    // Use the smallest possible type that can represent the value
    if (nValue >= SAL_MIN_INT8 && nValue <= SAL_MAX_INT8)
        xAny <<= sal_Int8(nValue);
    else if (nValue >= SAL_MIN_INT16 && nValue <= SAL_MAX_INT16)
        xAny <<= sal_Int16(nValue);
    else if (nValue >= SAL_MIN_INT32 && nValue <= SAL_MAX_INT32)
        xAny <<= sal_Int32(nValue);
    else
        xAny <<= sal_Int64(nValue);

    return xAny;
}
}

void pushAny(lua_State* pLuaState,
             const css::uno::Any& xAny,
             const Runtime& rRuntime)
{
    switch (xAny.getValueTypeClass())
    {
        case css::uno::TypeClass_VOID:
            lua_pushnil(pLuaState);
            return;

        case css::uno::TypeClass_BOOLEAN:
            {
                bool b;
                xAny >>= b;
                lua_pushboolean(pLuaState, b);
            }
            return;

        case css::uno::TypeClass_BYTE:
        case css::uno::TypeClass_SHORT:
        case css::uno::TypeClass_UNSIGNED_SHORT:
        case css::uno::TypeClass_LONG:
        case css::uno::TypeClass_UNSIGNED_LONG:
        case css::uno::TypeClass_HYPER:
        case css::uno::TypeClass_UNSIGNED_HYPER:
            {
                sal_Int64 nValue;
                if (xAny >>= nValue)
                {
                    lua_pushinteger(pLuaState, nValue);
                    return;
                }
            }
            break;

        case css::uno::TypeClass_FLOAT:
        case css::uno::TypeClass_DOUBLE:
            {
                lua_Number nValue;
                if (xAny >>= nValue)
                {
                    lua_pushnumber(pLuaState, nValue);
                    return;
                }
            }
            break;

        case css::uno::TypeClass_STRING:
            {
                rtl::OUString sValue;
                xAny >>= sValue;
                rtl::OString sUtf8Value = OUStringToOString(sValue, RTL_TEXTENCODING_UTF8);
                lua_pushlstring(pLuaState, sUtf8Value.getStr(), sUtf8Value.getLength());
            }
            return;

        case css::uno::TypeClass_INTERFACE:
            {
                css::uno::Reference<css::uno::XInterface> xInterface;
                if ((xAny >>= xInterface) && xInterface.is())
                    Object::pushObject(pLuaState, xInterface, rRuntime);
                else
                    lua_pushnil(pLuaState);
            }
            return;

        case css::uno::TypeClass_SEQUENCE:
            if (pushSequence(pLuaState, xAny, rRuntime))
                return;
            break;

        case css::uno::TypeClass_STRUCT:
            Struct::pushStruct(pLuaState, xAny, rRuntime);
            return;
    }

    rtl::OUString sMessage = "Unsupported conversion from type class "
        + rtl::OUString::number(xAny.getValueTypeClass());
    throw css::beans::IllegalTypeException(sMessage);
}

css::uno::Any getAny(lua_State* pLuaState, int nIndex)
{
    if (nIndex < 0)
        nIndex += lua_gettop(pLuaState) + 1;

    css::uno::Any xAny;

    switch (lua_type(pLuaState, nIndex))
    {
        case LUA_TNIL:
            // Leave the any void
            return xAny;

        case LUA_TNUMBER:
            if (lua_isinteger(pLuaState, nIndex))
                return convertInteger(lua_tointeger(pLuaState, nIndex));
            else
                xAny <<= double(lua_tonumber(pLuaState, nIndex));
            return xAny;

        case LUA_TBOOLEAN:
            xAny <<= bool(lua_toboolean(pLuaState, nIndex));
            return xAny;

        case LUA_TSTRING:
            {
                size_t nLen;
                const char* pString = lua_tolstring(pLuaState, nIndex, &nLen);
                xAny <<= rtl::OUString(pString, nLen, RTL_TEXTENCODING_UTF8);
            }
            return xAny;

        case LUA_TTABLE:
            {
                // We don’t want to use lua_len because we don’t want Lua to do a longjmp
                lua_Unsigned nLen = lua_rawlen(pLuaState, nIndex);
                css::uno::Sequence<css::uno::Any> aValues(nLen);
                for (lua_Unsigned i = 0; i < nLen; ++i)
                {
                    lua_rawgeti(pLuaState, nIndex, i);
                    aValues[i] = getAny(pLuaState, -1);
                    lua_pop(pLuaState, 1);
                }
                xAny <<= aValues;
            }
            return xAny;

        case LUA_TUSERDATA:
            if (Object* pObject = Object::testObject(pLuaState, nIndex))
            {
                xAny <<= pObject->getInterface();
                return xAny;
            }
            if (Struct* pStruct = Struct::testStruct(pLuaState, nIndex))
                return pStruct->getValue();
            break;
    }

    const char *sTypeName = luaL_typename(pLuaState, nIndex);
    rtl::OUString sMessage = rtl::OUString("Unsupported conversion from Lua type ")
        + rtl::OUString(sTypeName, strlen(sTypeName), RTL_TEXTENCODING_UTF8);
    throw css::beans::IllegalTypeException(sMessage);
}
}
