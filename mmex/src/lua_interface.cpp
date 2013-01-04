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
#include "htmlbuilder.h"

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
 (lua_, "lua_function_name", static_method_name_defined_in TLuaInterface);
 *****************************************************************************/
void TLuaInterface::Open_MMEX_Library()
{
    lua_register(lua_, "_",                  cpp2lua_GetTranslation);
    lua_register(lua_, "mmBell",             cpp2lua_Bell);
    lua_register(lua_, "mmMessageBox",       cpp2lua_MessageBox);
    lua_register(lua_, "mmSQLite3ResultSet", cpp2lua_SQLite3ResultSet);
    lua_register(lua_, "mmGetSingleChoice",  cpp2lua_GetSingleChoice);
    lua_register(lua_, "mmGetTextFromUser",  cpp2lua_GetTextFromUser);
    lua_register(lua_, "mmGetSiteContent",   cpp2lua_GetSiteContent);

    lua_register(lua_, "mmHTMLBuilder",      cpp2lua_HTMLBuilder);
}

/******************************************************************************
 Lua Syntax: See comment above the defined function.

 Note: Generally all functions require a variable to accept a return value.
       Some functions can return multiple values, shown as xx[, yy]
       [, yy] represents an optional value.
 *****************************************************************************/

/*****************************************************************************
 _"translation_string" or _("translation_string") similar to wxWidgets
 *****************************************************************************/
int TLuaInterface::cpp2lua_GetTranslation(lua_State *lua)
{
    wxString data = wxString::FromUTF8(lua_tostring(lua, -1));
    data = wxGetTranslation(data);
    lua_pushstring(lua, data.ToUTF8());

    return 1;
}

/******************************************************************************
 mmBell() - For Windows, ring the system bell when available. No return value.
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
 output[, error] = mmSQLite3ResultSet("sql_script")
 global var SQLQueryResult = { {x1.1 .... x1.n}, {x2.1 .... x2.n} }
 *****************************************************************************/
int TLuaInterface::cpp2lua_SQLite3ResultSet(lua_State *lua)
{
    wxString sScript = wxString::FromUTF8(lua_tostring(lua, -1));
    lua_pop(lua, 1); // remove error message
    int iError  = 0; //SQLITE_OK;
    wxString sOutput = wxT("");

    int iRowsCount = 0;
    try
    {
        iRowsCount = static_db_ptr().get()->ExecuteScalar(wxT("select count (*) from (\n") + sScript + wxT("\n)"));
    }
    catch(const wxSQLite3Exception& e)
    {
        iError = e.GetErrorCode();
        sOutput = e.GetMessage();
        if (sOutput.Contains(wxT("update")) ||
        sOutput.Contains(wxT("delete")) ||
        sOutput.Contains(wxT("insert")))
        {
            iError = 3; //SQLITE_PERM;
            sOutput = _("Please, use read only access to DB in this function");
        }
    }

    if (iError == 0) //SQLITE_OK
    {
        wxSQLite3ResultSet sqlQueryResult;
        try
        {
            sqlQueryResult = static_db_ptr().get()->ExecuteQuery(sScript);
        }
        catch(const wxSQLite3Exception& e)
        {
            iError = e.GetErrorCode();
            sOutput = wxString::Format(_("Error: %s"), e.GetMessage().c_str());
        }

        if (iError == 0)
        {
            lua_createtable(lua , iRowsCount, 0);  // push the main table T onto the stack

            int iColumnsCount = sqlQueryResult.GetColumnCount();
            for (int key = 1; key <= iRowsCount; key ++)
            {
                sqlQueryResult.NextRow();
                lua_createtable(lua , iColumnsCount, 0); // push a Row Table R onto the stack

                for (int i = 0; i < iColumnsCount; i++)
                {
                    wxString sData = sqlQueryResult.GetAsString(i);
                    // value is at -1, R is at -2
                    lua_pushstring(lua, sData.ToUTF8());
                    lua_rawseti(lua, -2, i+1);   // R[i] = Data

                    sOutput<<sData<<wxT("\t|\t");
                }
                // R is at -1, T is at -2
                lua_rawseti(lua, -2, key); // T[key] = R

                sOutput.RemoveLast(3);
                sOutput << wxT("<br>\n");
            }
            lua_setglobal(lua,"SQLQueryResult");
        }
    }

    lua_pushstring(lua, sOutput.ToUTF8());
    lua_pushinteger(lua, iError);

    return 2;
}

/******************************************************************************
 mmGetSingleChoice(lua_table, "dialog_message", "dialog_heading")
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

/******************************************************************************
 output[, error] = mmGetSiteContent("url_address")
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

/******************************************************************************
 mmHTMLBuilder("function[, value_1][, value_2][, value_3][, value_4]")
 *****************************************************************************/
int TLuaInterface::cpp2lua_HTMLBuilder(lua_State* lua)
{
    wxString value_4;
    if (lua_gettop(lua) > 4 )
    {
        value_4 = wxString::FromUTF8(lua_tostring(lua, -1));
        lua_pop(lua, 1);
    }

    wxString value_3;
    if (lua_gettop(lua) > 3 )
    {
        value_3 = wxString::FromUTF8(lua_tostring(lua, -1));
        lua_pop(lua, 1);
    }

    wxString value_2;
    if (lua_gettop(lua) > 2 )
    {
        value_2 = wxString::FromUTF8(lua_tostring(lua, -1));
        lua_pop(lua, 1);
    }

    wxString value_1;
    if (lua_gettop(lua) > 1 )
    {
        value_1 = wxString::FromUTF8(lua_tostring(lua, -1));
        lua_pop(lua, 1);
    }

    wxString fn_name = wxString::FromUTF8(lua_tostring(lua, -1));
    lua_pop(lua, 1);

    wxString html_error = _("HTML_BUILDER: Syntax error function: ");
    mmHTMLBuilder hb;
    try
    {
        if (fn_name == wxT("Init")) hb.init();
        else if (fn_name == wxT("StartTable")) hb.startTable(value_1, value_2);
        else if (fn_name == wxT("StartTableRow")) hb.startTableRow();
        else if (fn_name == wxT("AddTableHeaderCell")) hb.addTableHeaderCell(value_1, value_2 == wxT("") ? false: true);
        else if (fn_name == wxT("AddTableCell")) hb.addTableCell(value_1, value_2 == wxEmptyString ? false: true, value_3 == wxEmptyString ? false: true, value_4 == wxEmptyString ? false: true);
        else if (fn_name == wxT("AddRowSeparator")) hb.addRowSeparator(wxAtoi(value_1.c_str()));
        else if (fn_name == wxT("EndTableRow")) hb.endTableRow();
        else if (fn_name == wxT("EndTable")) hb.endTable();
        else if (fn_name == wxT("AddHorizontalLine")) hb.addHorizontalLine(wxAtoi(value_1.c_str()));
        else if (fn_name == wxT("End")) hb.end();
        else
        {
            hb.addParaText(html_error + fn_name);
        }
    }
    catch (...)
    {
        hb.addParaText(html_error + fn_name);
    }

    lua_pushstring(lua, hb.getHTMLText().ToUTF8());

    return 1;
}