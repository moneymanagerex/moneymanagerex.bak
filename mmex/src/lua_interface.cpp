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

#include "lua_interface.h"
#include "util.h"

// Constructor: Initialises Lua when an instant is created.
TLuaInterface::TLuaInterface()
{
    lua_ = luaL_newstate();
    wxASSERT(lua_);

    luaopen_string(lua_);
    luaL_openlibs(lua_);
    Open_MMEX_Library();
}

// Destructor: Shuts down Lua on completion
TLuaInterface::~TLuaInterface()
{
    lua_close(lua_);
}

// Passes Lua code in a string, to be run by Lua.
wxString TLuaInterface::RunLuaCode(wxString lua_code)
{
    lua_result_ = luaL_loadstring(lua_, lua_code.ToUTF8());
    if (lua_result_)
    {
        return LuaErrorResult();
    }

    lua_result_ = lua_pcall(lua_, 0, LUA_MULTRET, 0);
    if (lua_result_)
    {
        return LuaErrorResult();
    }

    return wxString::FromUTF8(lua_tostring(lua_, -1));
}

// Passes a filename containing Lua code, to be run by Lua.
wxString TLuaInterface::RunLuaFile(wxString lua_filename)
{
    lua_result_ = luaL_loadfile(lua_, lua_filename.ToUTF8());
    if (lua_result_)
    {
        return LuaErrorResult();
    }

    lua_result_ = lua_pcall(lua_, 0, LUA_MULTRET, 0);
    if (lua_result_)
    {
        return LuaErrorResult();
    }

    return wxString::FromUTF8(lua_tostring(lua_, -1));
}

// Decode the error into a literal string
wxString TLuaInterface::LuaErrorResult()
{
    wxString error_result;
    if (lua_result_ != 0)
    {
        error_result = wxString::Format(_("Error: %s\n"), wxString::FromUTF8(lua_tostring(lua_, -1)).c_str());
        lua_pop(lua_, 1); // remove error message from stack
    }
    return error_result;
}

/******************************************************************************
 Lua extended functions, provided by c++ code:

 New functions needed to be registered with Lua using following values:
 ..., "name_of_lua_function", name_of_static_method_defined_in TLuaInterface);

 Lua syntax: See comment above the defined function 
 *****************************************************************************/
void TLuaInterface::Open_MMEX_Library()
{
    lua_register(lua_, "_",                 cpp2lua_GetTranslation);
    lua_register(lua_, "mmBell",            cpp2lua_Bell);
    lua_register(lua_, "mmMessageBox",      cpp2lua_MessageBox);
	lua_register(lua_, "mmSQLite3ResultSet", cpp2lua_SQLite3ResultSet);
    lua_register(lua_, "mmGetTextFromUser", cpp2lua_GetTextFromUser);
    lua_register(lua_, "mmGetSingleChoice", cpp2lua_GetSingleChoice);
    lua_register(lua_, "mmGetSiteContent",  cpp2lua_GetSiteContent);
}

/******************************************************************************
 mmBell() - Does nothing except ring the stystem bell
 *****************************************************************************/
int TLuaInterface::cpp2lua_Bell(lua_State* lua)
{
    int stack_pos;                  // prevent compiler
    stack_pos = lua_gettop(lua);    // warnings
    wxBell();

    return 0;
}

/******************************************************************************
 mmMessageBox(_("Message"), _("Caption"), Style)
 *****************************************************************************/
int TLuaInterface::cpp2lua_MessageBox(lua_State* lua)
{
    int mm_style = lua_tointeger(lua, -1);
    lua_pop(lua, 1); // remove error message

    wxString caption = wxString::FromUTF8(lua_tostring(lua, -1));
    lua_pop(lua, 1); // remove error message

    wxString message = wxString::FromUTF8(lua_tostring(lua, -1));
    lua_pop(lua, 1); // remove error message

    double win_style;

    if      (mm_style == 1) win_style = wxOK;
    else if (mm_style == 2) win_style = wxYES_NO;
    else if (mm_style == 3) win_style = wxCANCEL;
    else if (mm_style == 13 || mm_style == 31) win_style = wxCANCEL + wxOK;
    else if (mm_style == 23 || mm_style == 32) win_style = wxCANCEL + wxYES_NO;
    else if (mm_style == 4) win_style = wxICON_EXCLAMATION;
    else if (mm_style == 5) win_style = wxICON_HAND;
    else if (mm_style == 6) win_style = wxICON_ERROR;
    else if (mm_style == 7) win_style = wxICON_QUESTION;
    else win_style = wxICON_INFORMATION;

    int ans = wxMessageBox(message, caption, win_style);

    if (ans == 2 || ans == 4) mm_style = 1; // OK or Yes
    else if (ans == 8)        mm_style = 2; // No
    else if (ans == 16)       mm_style = 3; // Cancel
    else mm_style = ans;

    lua_pushinteger(lua, mm_style);
    return 1;
}

/******************************************************************************
 output, error = mmSQLite3ResultSet("sql_script")  
 *****************************************************************************/
int TLuaInterface::cpp2lua_SQLite3ResultSet(lua_State *lua)
{
    wxString sScript = wxString::FromUTF8(lua_tostring(lua, -1));
    lua_pop(lua, 1); // remove error message
    wxString sError  = wxT("");
    wxString sOutput = wxT("");
 
    wxSQLite3ResultSet sqlQueryResult;
    try
    {
        sqlQueryResult = static_db_ptr().get()->ExecuteQuery(sScript);
    }
    catch(const wxSQLite3Exception& e)
    {
       sError = wxString::Format(_("Error: %s"), e.GetMessage().c_str());
    }

    if (sError.IsEmpty())
    {
        int columnCount = sqlQueryResult.GetColumnCount();
        while (sqlQueryResult.NextRow())
        {
            for (int index = 0; index < columnCount; index ++)
            {
                wxString sData = sqlQueryResult.GetAsString(index);
                sOutput << sData << wxT(" | ");  //it should be Lua table not string
            }
            sOutput << wxT("\n");
        }
    }
 
    lua_pushstring(lua, sOutput.ToUTF8());
    lua_pushstring(lua, sError.ToUTF8());
    
    return 2;
}

/******************************************************************************
 mmGetSingleChoice(lua_table, _("Choose Accounts"), _("Dialogue"))
 *****************************************************************************/
int TLuaInterface::cpp2lua_GetSingleChoice(lua_State* lua)
{
    luaL_checktype(lua, 1, LUA_TTABLE);
    size_t len = lua_rawlen(lua, 1);

    wxArrayString data;
    for(unsigned int i = 1; i <= len; i++)
    {
        lua_rawgeti(lua, 1, i);
        data.Add(wxString::FromUTF8(luaL_checkstring(lua, -1)));  
        lua_pop(lua, 1);
    }

    wxString header = wxString::FromUTF8(lua_tostring(lua, -1));
    lua_pop(lua, 1); // remove error message

    wxString message = wxString::FromUTF8(lua_tostring(lua, -1));
    lua_pop(lua, 1); // remove error message

    wxString value;
    value = wxGetSingleChoice(message, header, data);
    lua_pushstring(lua, value.ToUTF8());

    return 1;
}

/******************************************************************************
 mmGetTextFromUser("field_name", "dialog_heading", "default_value")
 *****************************************************************************/
int TLuaInterface::cpp2lua_GetTextFromUser(lua_State* lua)
{
    wxString value = wxString::FromUTF8(lua_tostring(lua, -1));
    lua_pop(lua, 1); // remove error message

    wxString header = wxString::FromUTF8(lua_tostring(lua, -1));
    lua_pop(lua, 1); // remove error message

    wxString message = wxString::FromUTF8(lua_tostring(lua, -1));
    lua_pop(lua, 1); // remove error message

    value = wxGetTextFromUser(message, header, value);
    lua_pushstring(lua, value.ToUTF8());

    return 1;
}

/*****************************************************************************
 _("translation_string")
 *****************************************************************************/
int TLuaInterface::cpp2lua_GetTranslation(lua_State *lua)
{
    wxString data = wxString::FromUTF8(lua_tostring(lua, -1));
    data = wxGetTranslation(data);
    lua_pushstring(lua, data.ToUTF8());

    return 1;
}

/******************************************************************************
 mmGetSiteContent("address")
 *****************************************************************************/
int TLuaInterface::cpp2lua_GetSiteContent(lua_State* lua)
{
    wxString sSite_address = wxString::FromUTF8(lua_tostring(lua, -1));
    lua_pop(lua, 1); // remove error message

    wxString sOutput;
    int error = site_content(sSite_address, sOutput);

    lua_pushstring(lua, sOutput.ToUTF8());
    lua_pushinteger(lua, error);

    return 2;
}
