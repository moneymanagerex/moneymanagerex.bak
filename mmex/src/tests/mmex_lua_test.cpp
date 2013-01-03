/*************************************************************************
 Copyright (C) 2011 Stefano Giorgio

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

/****************************************************************************
 These tests can be run independantly of other tests by excluding
 dbwrapper_test.cpp from the project. Alternatively, this module can be
 prevented from running by excluding mmex_lua_test.cpp from the project.

 Note: Total test time is meaningless with this module included.

 ****************************************************************************/
#include <UnitTest++.h>
#include "testing_util.h"
#include "lua_interface.h"

/*****************************************************************************************
 The tests for testing mmex_settings - in a database.
 *****************************************************************************************/
SUITE(mmex_lua_test)
{

TEST(lua_interface_hello_world)
{
    printf("\nLua Tests: START");
    display_STD_IO_separation_line();
    const wxDateTime start_time(wxDateTime::UNow());

    TLuaInterface* lua_core = new TLuaInterface();

    wxString lua_program = wxString() <<
        wxT("print \"\"                        \n") <<
        wxT("print \"Message from Lua.\"       \n") <<
        wxT("print \"Hello World.\"            \n") <<
        wxT("print \"\"                        \n")
    ; // end of text script 

    wxString lua_code_result = lua_core->RunLuaCode(lua_program);
    printf(lua_code_result.char_str());

    displayTimeTaken(wxT("lua_interface_hello_world"), start_time);
}

TEST(lua_interface_lua_syntax_error)
{
    const wxDateTime start_time(wxDateTime::UNow());
    TLuaInterface* lua_core = new TLuaInterface();

    wxString lua_program = wxString() << 
        wxT("print \"\"                        \n")
        wxT("print \"Message from Lua.\"       \n")
        wxT("print \"Hello World.\"            \n")
        wxT("print                             \n") // missing empty quotes
    ; // end of text script 

    wxString lua_code_result = lua_core->RunLuaCode(lua_program);
    printf(lua_code_result.char_str());

    displayTimeTaken(wxT("lua_interface_lua_syntax_error"), start_time);
}

TEST(lua_interface_test_sql_good)
{
    const wxDateTime start_time(wxDateTime::UNow());
    TLuaInterface* lua_core = new TLuaInterface();

    wxString lua_program = wxString() <<
        wxT("sql_1 = \"select * from category_v1\"            \n") <<
        wxT("sql_result = mmSQLite3ResultSet(sql_1)           \n") <<
        wxT("sql_2 = \"update category_v1 set categid = 1 \"  \n") <<
        wxT("sql_2 = sql_2 .. \"where categid=1\"             \n") <<
        wxT("sql_result = sql_result ..                       \n") <<
        wxT("             mmSQLite3ResultSet(sql_2)           \n") <<
        wxT("return sql_result                                \n")
    ; // end of text script 

    display_STD_IO_line();
    wxString lua_code_result = lua_core->RunLuaCode(lua_program);
    printf(lua_code_result.char_str());
    display_STD_IO_line();

    displayTimeTaken(wxT("lua_interface_test_sql_good"), start_time);
}

TEST(lua_interface_test_sql_bad)
{
    const wxDateTime start_time(wxDateTime::UNow());
    TLuaInterface* lua_core = new TLuaInterface();

    wxString lua_program = wxString() <<
        wxT("sql_script = \"select * from cat_v1\"            \n") <<
        wxT("result, error = mmSQLite3ResultSet(sql_script)   \n") <<
        wxT("return result                                    \n")
    ; // end of text script 

    display_STD_IO_line();
    wxString lua_code_result = lua_core->RunLuaCode(lua_program);
    printf(lua_code_result.char_str());
    display_STD_IO_line();

    displayTimeTaken(wxT("lua_interface_test_sql_bad"), start_time);
}

TEST(lua_interface_test_RunLuaFile)
{
    const wxDateTime start_time(wxDateTime::UNow());
    TLuaInterface* lua_core = new TLuaInterface();

    wxString lua_test_filename = wxT("./mmex_lua_test_file.lua");

    display_STD_IO_line();
    wxString lua_code_result = lua_core->RunLuaFile(lua_test_filename);
    printf(lua_code_result.char_str());
    display_STD_IO_line();

    displayTimeTaken(wxT("lua_interface_test_RunLuaFile"), start_time);
    display_STD_IO_separation_line();
    printf("Lua Tests: END\n\n");
}

} // SUITE end Inidb_test

//----------------------------------------------------------------------------
