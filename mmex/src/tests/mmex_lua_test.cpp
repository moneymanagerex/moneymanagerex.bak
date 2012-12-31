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

TEST(mmex_lua_test_start)
{
    // Separate from other tests
    printf("Lua Tests__________________________________________________________\n");
    printf("\n");
}

TEST(lua_interface_hello_world)
{
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

TEST(lua_interface_test_sql)
{
    const wxDateTime start_time(wxDateTime::UNow());
    TLuaInterface* lua_core = new TLuaInterface();

    wxString lua_program = wxString() <<
        wxT("sql_script = \"select * from category_v1\"       \n") <<
        wxT("var = mmSQLite3ResultSet(sql_script)             \n") <<
        wxT("return var                                       \n")
    ; // end of text script 

    wxString lua_code_result = lua_core->RunLuaCode(lua_program);
    printf(lua_code_result.char_str());

    displayTimeTaken(wxT("lua_interface_test_sql"), start_time);
}

TEST(mmex_lua_test_end)
{
    // Separate from other tests
    printf("___________________________________________________________________\n");
    printf("\n");
    printf("\n");
//#ifndef _DBWRAPPER_TEST_IS_INCLUDED_IN_BUILD_
//    std::cout << "Press ENTER to continue... ";
//    std::cin.get();
//#endif
}

} // SUITE end Inidb_test

//----------------------------------------------------------------------------
