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

#include "lookup.hxx"

#include <rtl/string.hxx>
#include <rtl/ustring.hxx>
#include <com/sun/star/container/XHierarchicalNameAccess.hpp>
#include <com/sun/star/reflection/XIdlReflection.hpp>
#include <com/sun/star/reflection/XConstantTypeDescription.hpp>
#include <com/sun/star/reflection/XTypeDescription.hpp>

#include "runtime.hxx"
#include "type.hxx"
#include "conversions.hxx"

namespace com::sun::star::container
{
class NoSuchElementException;
}

namespace
{
constexpr int PREFIX_UPVALUE = 1;
constexpr int RUNTIME_UPVALUE = 2;
}

namespace uk::co::busydoingnothing::luno
{
namespace
{
void createModule(lua_State *pLuaState, const rtl::OUString& sFullNameUtf16, const Runtime& rRuntime)
{
    rtl::OString sFullName = OUStringToOString(sFullNameUtf16, RTL_TEXTENCODING_UTF8);
    sal_Int32 nPartStart = 0;

    // Start from the global table
    lua_pushglobaltable(pLuaState);

    while (true)
    {
        sal_Int32 nPartEnd = sFullName.indexOf('.', nPartStart);

        if (nPartEnd == -1)
            nPartEnd = sFullName.getLength();

        lua_pushlstring(pLuaState, sFullName.getStr() + nPartStart, nPartEnd - nPartStart);
        if (lua_rawget(pLuaState, -2) == LUA_TNIL)
        {
            lua_pop(pLuaState, 1);

            // Lazily create the table
            lua_newtable(pLuaState);
            // And its metatable
            lua_newtable(pLuaState);
            lua_pushliteral(pLuaState, "__index");
            // Create the new prefix by concatenating "." onto the prefix up to this point
            lua_pushlstring(pLuaState, sFullName.getStr(), nPartEnd);
            lua_pushliteral(pLuaState, ".");
            lua_concat(pLuaState, 2);
            pushLookupFunc(pLuaState, rRuntime);
            lua_rawset(pLuaState, -3);
            lua_setmetatable(pLuaState, -2);

            // Store the new table directly in the parent table to avoid looking it up again
            lua_pushlstring(pLuaState, sFullName.getStr() + nPartStart, nPartEnd - nPartStart);
            lua_pushvalue(pLuaState, -2);
            lua_rawset(pLuaState, -4);
        }

        // Replace the previous table with the new one in the stack and continue the search from
        // there
        lua_remove(pLuaState, -2);

        if (nPartEnd >= sFullName.getLength())
            break;

        nPartStart = nPartEnd + 1;
    }

    // The last table is left on the stack
}

// Stores the item at the top of the stack in the table for the module of the item. Does not pop the
// item
void storeInParentModule(lua_State *pLuaState, const rtl::OUString& sFullName,
                         const Runtime& rRuntime)
{
    // Add the type to the module dictionary
    int nLastDot = sFullName.lastIndexOf('.');

    if (nLastDot == -1)
        return;

    createModule(pLuaState, rtl::OUString(sFullName.getStr(), nLastDot), rRuntime);
    rtl::OString sLastPart(
        sFullName.getStr() + nLastDot + 1, sFullName.getLength() - nLastDot - 1,
        RTL_TEXTENCODING_UTF8);
    lua_pushlstring(pLuaState, sLastPart.getStr(), sLastPart.getLength());
    lua_pushvalue(pLuaState, -3);
    lua_rawset(pLuaState, -3);
    lua_pop(pLuaState, 1);
}

bool createType(lua_State *pLuaState, const rtl::OUString& sFullName, const Runtime& rRuntime)
{
    css::uno::Reference<css::reflection::XIdlClass> xIdlClass
        = rRuntime.m_xIdlReflection->forName(sFullName);

    if (!xIdlClass.is())
        return false;

    Type::pushType(pLuaState, xIdlClass, rRuntime);

    storeInParentModule(pLuaState, sFullName, rRuntime);

    return true;
}

bool createConstant(lua_State *pLuaState, const rtl::OUString& sFullName,
                    const css::uno::Reference<css::reflection::XTypeDescription>& xType,
                    const Runtime& rRuntime)
{
    css::uno::Reference<css::reflection::XConstantTypeDescription> xConstantType(
        xType, css::uno::UNO_QUERY);

    if (!xConstantType.is())
    {
        lua_pushliteral(pLuaState,
                        "internal error: contant type found without implementing "
                        "XContantTypeDescription");
        return false;
    }

    try
    {
        pushAny(pLuaState, xConstantType->getConstantValue(), rRuntime);
    }
    catch (const css::uno::Exception& e)
    {
        rtl::OString sMessage = rtl::OUStringToOString(e.Message, RTL_TEXTENCODING_UTF8);
        lua_pushlstring(pLuaState, sMessage.getStr(), sMessage.getLength());
        return false;
    }

    storeInParentModule(pLuaState, sFullName, rRuntime);

    return true;
}

int lookup(lua_State* pLuaState)
{
    size_t nKeyLength;
    const char *sKey = luaL_checklstring(pLuaState, 2, &nKeyLength);
    size_t nPrefixLength;
    const char* sPrefix = luaL_checklstring(
        pLuaState, lua_upvalueindex(PREFIX_UPVALUE), &nPrefixLength);
    const Runtime* pRuntime = reinterpret_cast<const Runtime*>(
        lua_touserdata(pLuaState, lua_upvalueindex(RUNTIME_UPVALUE)));

    if (!pRuntime->isValid())
        luaL_error(pLuaState, "global called with Luno runtime in invalid state");

    {
        rtl::OUString sFullName = rtl::OUString(sPrefix, nPrefixLength, RTL_TEXTENCODING_UTF8) +
            rtl::OUString(sKey, nKeyLength, RTL_TEXTENCODING_UTF8);

        css::uno::Any xTypeAny;

        try
        {
            xTypeAny = pRuntime->m_xTypeManager->getByHierarchicalName(sFullName);
        }
        catch (css::container::NoSuchElementException&)
        {
            return 0;
        }

        css::uno::Reference<css::reflection::XTypeDescription> xType;

        if (!(xTypeAny >>= xType) || !xType.is())
            return 0;

        switch (xType->getTypeClass())
        {
            case css::uno::TypeClass_MODULE:
            case css::uno::TypeClass_CONSTANTS:
                createModule(pLuaState, sFullName, *pRuntime);
                return 1;

            case css::uno::TypeClass_INTERFACE:
            case css::uno::TypeClass_STRUCT:
                return createType(pLuaState, sFullName, *pRuntime) ? 1 : 0;

            case css::uno::TypeClass_CONSTANT:
                if (!createConstant(pLuaState, sFullName, xType, *pRuntime))
                    goto set_lua_error;
                return 1;

            default:
                return 0;
        }
    }

    // The goto is to make sure the destructors are all called before letting Lua do a longjmp
 set_lua_error:
    lua_error(pLuaState);
    return 0;
}
}

void pushLookupFunc(lua_State* pLuaState, const Runtime& rRuntime)
{
    // Store a pointer to the runtime as light user data
    lua_pushlightuserdata(pLuaState,
                          const_cast<void*>(reinterpret_cast<const void*>(&rRuntime)));
    // Store two upvalues. The first one has already been pushed onto the stack by the caller to
    // this function.
    lua_pushcclosure(pLuaState, lookup, 2);
}
}
