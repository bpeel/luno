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

#include "struct.hxx"

#include <com/sun/star/reflection/XIdlClass.hpp>
#include <com/sun/star/reflection/XIdlField2.hpp>
#include <com/sun/star/reflection/XIdlReflection.hpp>

#include "conversions.hxx"

namespace uk::co::busydoingnothing::luno
{
void Struct::pushStruct(lua_State* pLuaState,
                        const css::uno::Any& xValue,
                        const Runtime& rRuntime)
{
    assert(xValue.getValueTypeClass() == css::uno::TypeClass_STRUCT);

    void *pUserData = lua_newuserdatauv(pLuaState, sizeof(Struct), 0);

    // Use placement new to initialize the struct in the memory that Lua allocated
    new(pUserData) Struct(xValue, rRuntime);

    pushMetatable(pLuaState);
    lua_setmetatable(pLuaState, -2);
}

void Struct::pushMetatable(lua_State* pLuaState)
{
    if (luaL_newmetatable(pLuaState, CLASS_NAME) == 0)
    {
        // The table has already been created so there’s nothing else to do
        return;
    }

    lua_pushliteral(pLuaState, "__gc");
    lua_pushcfunction(pLuaState, gc);
    lua_rawset(pLuaState, -3);

    lua_pushliteral(pLuaState, "__index");
    lua_pushcfunction(pLuaState, index);
    lua_rawset(pLuaState, -3);

    lua_pushliteral(pLuaState, "__newindex");
    lua_pushcfunction(pLuaState, newIndex);
    lua_rawset(pLuaState, -3);
}

Struct* Struct::checkStruct(lua_State* pLuaState, int nArg)
{
    return reinterpret_cast<Struct*>(luaL_checkudata(pLuaState, nArg, CLASS_NAME));
}

Struct* Struct::testStruct(lua_State* pLuaState, int nArg)
{
    return reinterpret_cast<Struct*>(luaL_testudata(pLuaState, nArg, CLASS_NAME));
}

int Struct::gc(lua_State* pLuaState)
{
    Struct* pStruct = checkStruct(pLuaState, 1);

    // The memory was allocated by Lua so we only need to call the destructor
    pStruct->~Struct();

    return 0;
}

bool Struct::ensureIdlClass()
{
    if (m_xIdlClass.is())
        return true;

    if (!m_rRuntime.isValid() || !m_xValue.hasValue())
        return false;

    m_xIdlClass = m_rRuntime.m_xIdlReflection->getType(m_xValue);

    return m_xIdlClass.is();
}

int Struct::doIndex(lua_State* pLuaState)
{
    size_t nKeyLength;
    const char* pKey = luaL_checklstring(pLuaState, 2, &nKeyLength);

    if (!ensureIdlClass())
    {
        luaL_error(pLuaState, "__index called on struct in an invalid state");
        return 0;
    }

    {
        rtl::OUString sKey(pKey, nKeyLength, RTL_TEXTENCODING_UTF8);
        css::uno::Reference<css::reflection::XIdlField2> xField(
            m_xIdlClass->getField(sKey), css::uno::UNO_QUERY);

        if (xField.is())
        {
            try
            {
                pushAny(pLuaState, xField->get(m_xValue), m_rRuntime);
            }
            catch (const css::uno::Exception& e)
            {
                rtl::OString sMessage = rtl::OUStringToOString(e.Message, RTL_TEXTENCODING_UTF8);
                lua_pushlstring(pLuaState, sMessage.getStr(), sMessage.getLength());
                goto set_lua_error;
            }
        }
        else
        {
            lua_pushnil(pLuaState);
        }

    }

    return 1;

    // The goto is to ensure that we call all of the destructors before letting Lua do a longjmp
 set_lua_error:
    lua_error(pLuaState);
    return 0;
}

int Struct::index(lua_State* pLuaState)
{
    Struct* pStruct = checkStruct(pLuaState, 1);

    return pStruct->doIndex(pLuaState);
}

int Struct::doNewIndex(lua_State* pLuaState)
{
    size_t nKeyLength;
    const char* pKey = luaL_checklstring(pLuaState, 2, &nKeyLength);

    if (!ensureIdlClass())
    {
        luaL_error(pLuaState, "__index called on struct in an invalid state");
        return 0;
    }

    {
        rtl::OUString sKey(pKey, nKeyLength, RTL_TEXTENCODING_UTF8);
        css::uno::Reference<css::reflection::XIdlField2> xField(
            m_xIdlClass->getField(sKey), css::uno::UNO_QUERY);

        if (xField.is())
        {
            try
            {
                xField->set(m_xValue, getAnyAsType(pLuaState, 3, xField->getType(), m_rRuntime));
            }
            catch (const css::uno::Exception& e)
            {
                rtl::OString sMessage = rtl::OUStringToOString(e.Message, RTL_TEXTENCODING_UTF8);
                lua_pushlstring(pLuaState, sMessage.getStr(), sMessage.getLength());
                goto set_lua_error;
            }
        }
        else
        {
            lua_pushliteral(pLuaState, "Tried to set unknown property \"");
            lua_pushvalue(pLuaState, 2);
            lua_pushliteral(pLuaState, "\" on instance of ");
            rtl::OString sTypeName = rtl::OUStringToOString(
                m_xIdlClass->getName(), RTL_TEXTENCODING_UTF8);
            lua_pushlstring(pLuaState, sTypeName.getStr(), sTypeName.getLength());
            lua_concat(pLuaState, 4);
            goto set_lua_error;
        }

    }

    return 0;

    // The goto is to ensure that we call all of the destructors before letting Lua do a longjmp
 set_lua_error:
    lua_error(pLuaState);
    return 0;
}

int Struct::newIndex(lua_State* pLuaState)
{
    Struct* pStruct = checkStruct(pLuaState, 1);

    return pStruct->doNewIndex(pLuaState);
}

void Struct::setValue(const css::uno::Any& xValue)
{
    assert(xValue.getValueTypeClass() == css::uno::TypeClass_STRUCT);

    if (ensureIdlClass())
    {
        css::uno::Reference<css::reflection::XIdlClass> xIdlClass
            = m_rRuntime.m_xIdlReflection->getType(xValue);
        assert(m_xIdlClass->equals(xIdlClass));
    }

    m_xValue = xValue;
}

}
