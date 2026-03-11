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

#include <com/sun/star/lang/XSingleServiceFactory.hpp>
#include <com/sun/star/uno/Sequence.hxx>
#include <sal/types.h>

#include "object.hxx"

namespace uk::co::busydoingnothing::luno
{
void pushAny(lua_State* pLuaState,
             const css::uno::Any& xAny,
             const css::uno::Reference<css::lang::XSingleServiceFactory> xSingleServiceFactory)
{
    switch (xAny.getValueTypeClass())
    {
        case css::uno::TypeClass_VOID:
            lua_pushnil(pLuaState);
            break;

        case css::uno::TypeClass_BOOLEAN:
            {
                bool b;
                xAny >>= b;
                lua_pushboolean(pLuaState, b);
            }
            break;

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
                    lua_pushinteger(pLuaState, nValue);
                else
                    lua_pushnil(pLuaState); // FIXME
            }
            break;

        case css::uno::TypeClass_FLOAT:
        case css::uno::TypeClass_DOUBLE:
            {
                lua_Number nValue;
                if (xAny >>= nValue)
                    lua_pushnumber(pLuaState, nValue);
                else
                    lua_pushnil(pLuaState); // FIXME
            }
            break;

        case css::uno::TypeClass_INTERFACE:
            {
                css::uno::Reference<css::uno::XInterface> xInterface;
                if ((xAny >>= xInterface) && xInterface.is())
                    Object::pushObject(pLuaState, xInterface, xSingleServiceFactory);
                else
                    lua_pushnil(pLuaState);
            }
            break;

        default:
            // FIXME
            lua_pushnil(pLuaState);
    }
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
            break;

        case LUA_TNUMBER:
            if (lua_isinteger(pLuaState, nIndex))
                xAny <<= sal_uInt64(lua_tointeger(pLuaState, nIndex));
            else
                xAny <<= double(lua_tonumber(pLuaState, nIndex));
            break;

        case LUA_TBOOLEAN:
            xAny <<= bool(lua_toboolean(pLuaState, nIndex));
            break;

        case LUA_TSTRING:
            {
                size_t nLen;
                const char* pString = lua_tolstring(pLuaState, nIndex, &nLen);
                xAny <<= rtl::OUString(pString, nLen, RTL_TEXTENCODING_UTF8);
            }
            break;

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
            break;

        case LUA_TUSERDATA:
            if (Object* pObject = Object::testObject(pLuaState, nIndex))
            {
                xAny <<= pObject->getInterface();
            }
            else
            {
                // FIXME
            }
            break;

        default:
            // FIXME
            break;
    }

    return xAny;
}
}
