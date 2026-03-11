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

#include "object.hxx"

#include <com/sun/star/lang/XSingleServiceFactory.hpp>
#include <com/sun/star/script/XInvocation.hpp>

#include "method.hxx"

namespace uk::co::busydoingnothing::luno
{
void Object::pushObject(lua_State* pLuaState,
                        const css::uno::Reference<css::uno::XInterface>& xInterface,
                        const css::uno::Reference<css::lang::XSingleServiceFactory>&
                        xInvocationFactory)
{
    void *pUserData = lua_newuserdatauv(pLuaState, sizeof(Object), 0);

    // Use placement new to initialize the object in the memory that Lua allocated
    new(pUserData) Object(xInterface, xInvocationFactory);

    pushMetatable(pLuaState);
    lua_setmetatable(pLuaState, -2);
}

void Object::pushMetatable(lua_State* pLuaState)
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
}

Object* Object::checkObject(lua_State* pLuaState, int nArg)
{
    return reinterpret_cast<Object*>(luaL_checkudata(pLuaState, nArg, CLASS_NAME));
}

int Object::gc(lua_State* pLuaState)
{
    Object* pObject = checkObject(pLuaState, 1);

    // The memory was allocated by Lua so we only need to call the destructor
    pObject->~Object();

    return 0;
}

int Object::doIndex(lua_State* pLuaState)
{
    size_t nKeyLength;
    const char* pKey = luaL_checklstring(pLuaState, 2, &nKeyLength);

    {
        if (!m_xInvocation.is())
        {
            if (!m_xInvocationFactory.is() || !m_xInterface.is())
                goto state_error;

            css::uno::Sequence<css::uno::Any> aArgs(1);
            aArgs[0] <<= m_xInterface;
            m_xInvocation.set(m_xInvocationFactory->createInstanceWithArguments(aArgs),
                              css::uno::UNO_QUERY);

            if (!m_xInvocation.is())
                goto state_error;
        }

        rtl::OUString sKey(pKey, nKeyLength, RTL_TEXTENCODING_UTF8);

        if (m_xInvocation->hasMethod(sKey))
            Method::pushMethod(pLuaState, 2, sKey, call);
        else
            lua_pushnil(pLuaState);
    }

    return 1;

    // The goto is to ensure that we call all of the destructors before letting Lua do a longjmp
 state_error:
    luaL_error(pLuaState, "__index called an object in an invalid state");
    return 0;
}

int Object::index(lua_State* pLuaState)
{
    Object* pObject = checkObject(pLuaState, 1);

    return pObject->doIndex(pLuaState);
}

int Object::call(lua_State* pLuaState, Method *pMethod)
{
    // STUB: just return a string regardless of the parameters
    lua_pushliteral(pLuaState, "STUB");

    return 1;
}

int Object::call(lua_State* pLuaState)
{
    Method *pMethod = Method::checkMethod(pLuaState, 1);
    Object *pObject = checkObject(pLuaState, 2);

    return pObject->call(pLuaState, pMethod);
}

}
