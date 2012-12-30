/*************************************************************************
 Copyright (C) 2012 Stefano Giorgio & Nikolay

 This program is free software; you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation; either version 2 of the License, or
 (at your option) any later version.

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with this program; if not, write to the Free Software
 Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *************************************************************************/

#ifndef _MM_EX_LUA_INTERFACE_H_
#define _MM_EX_LUA_INTERFACE_H_

#include "defs.h"
#include <iostream>
#include "mmcoredb.h"

extern "C" {
#include "lualib.h"
#include "lauxlib.h"
}

class TLuaInterface
{
public:
    TLuaInterface(const mmCoreDB* core);
    ~TLuaInterface();
    
    wxString RunLuaCode(wxString lua_code);

private:
    const mmCoreDB*  core_;

    lua_State* lua_;
    int lua_result_;
    wxString LuaErrorResult();

    /**************************************************************************
    Lua function extensions provided by c++ code:

    All functions are to be defined in this class, and
    added to the Open_MMEX_Library() function, defined in this class.
    **************************************************************************/
    // List of mmex_library functions
    void Open_MMEX_Library();
    // All Lua functions provided by C++ for MMEX
    static int cpp2lua_Bell(lua_State* lua);
    static int cpp2lua_MessageBox(lua_State* lua);
	static int cpp2lua_GetSingleChoice(lua_State* lua);
    static int cpp2lua_GetTextFromUser(lua_State* lua);
    static int cpp2lua_GetTranslation(lua_State* lua);
};

#endif // _MM_EX_LUA_INTERFACE_H_
