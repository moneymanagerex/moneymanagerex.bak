/*******************************************************
Copyright (C) 2012 Stefano Giorgio

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
 ********************************************************/

/****************************************************************************
 These tests can be run independantly of other tests by excluding
 dbwrapper_test.cpp from the project. Alternatively, this module can be
 prevented from running by excluding mmex_lua_test.cpp from the project.

 Note: Total test time is meaningless with this module included.

 ****************************************************************************/
//----------------------------------------------------------------------------
#include <UnitTest++.h>
#include "defs.h"
#include <iostream>
//----------------------------------------------------------------------------

#include "lua_interface.h"

void display_time_taken(const wxString msg, const wxDateTime start_time)
{
    const wxDateTime end_time(wxDateTime::UNow());
    const wxString time_dif = (end_time - start_time).Format(wxT("%S%l"));

    printf(wxString::Format(wxT("Time Taken: %s milliseconds - Test: %s \n"), time_dif.c_str(), msg.c_str()).char_str());
}

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

TEST(lua_interface_test)
{
    const wxDateTime start_time(wxDateTime::UNow());

    TLuaInterface* lua_core = new TLuaInterface();

    {// Lua test code block 1
        wxString lua_program = wxString() <<
            wxT("print \"\"                        \n") <<
            wxT("print \"Message from Lua.\"       \n") <<
            wxT("print \"Hello World.\"            \n") <<
            wxT("print \"\"                        \n")
        ; // end of text script 

        wxString lua_code_result = lua_core->RunLuaCode(lua_program);
        printf(lua_code_result.char_str());
    }

    printf("Test - Lua syntax with error\n");
    {// Lua test code block 2
        wxString lua_program = wxString() << 
            wxT("print \"\"                        \n")
            wxT("print \"Message from Lua.\"       \n")
            wxT("print \"Hello World.\"            \n")
            wxT("print                             \n") // missing empty quotes
        ; // end of text script 

        wxString lua_code_result = lua_core->RunLuaCode(lua_program);
        printf(lua_code_result.char_str());
    }

#if 0
    printf("Test - Lua CPP calls\n");
    {// Lua test code block 3

        wxString lua_program = wxString() <<
            wxT("var = \"Message from Lua.\"       \n") <<
            wxT("var = var .. \"Hello World.\"     \n") <<
            wxT("var = var .. \"\n\"               \n") <<
            wxT("var = var .. _\"Amount\"          \n") <<
            // Following commented out line crashes in test mode.
            //  wxT("var = mmGetUserText(_(\"Amount\"), _(\"Dialogue\"), 2012) ") <<
            wxT("var = _\"User \".. var            \n") <<
            wxT("print \"\"                        \n") <<
            wxT("return var                        \n")
        ; // end of text script 
        
        wxString lua_code_result = lua_core->RunLuaCode(lua_program);
        printf(lua_code_result.char_str());
    }
#endif

    display_time_taken(wxT("lua_interface_test"), start_time);
}
TEST(mmex_lua_test_end)
{
    // Separate from other tests
    printf("___________________________________________________________________\n");
    printf("\n");
    printf("\n");
#ifndef _DBWRAPPER_TEST_IS_INCLUDED_IN_BUILD_
    std::cout << "Press ENTER to continue... ";
    std::cin.get();
#endif
}

} // SUITE end Inidb_test

//----------------------------------------------------------------------------
