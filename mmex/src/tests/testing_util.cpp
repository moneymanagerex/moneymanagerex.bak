/*************************************************************************
 Copyright (C) 2009 VaDiM
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

#include "testing_util.h"

/*****************************************************************************************
 This class is used to remove the temporary database on completion.
 *****************************************************************************************/
Cleanup::Cleanup(wxString filename, bool pause)
: dbFileName_(filename)
, pause_(pause)
{
    printf("\nTest file created at location:\n");
    printf(wxString::Format(wxT("%s \n\n"), dbFileName_.c_str()).char_str());
}

// Cleanup class destructor - Called on test completion.
Cleanup::~Cleanup()
{
    try
    {
        if (pause_)
        {
            printf("\n");
            std::cout << "Test files will be deleted. ";
            std::cout << "Press ENTER to continue... ";
            std::cin.get();
        }
        wxRemoveFile(dbFileName_);
    }
    catch (...)
    {
        wxASSERT(false);
    }
}

/*****************************************************************************************
 Create a single access point for the ini_settings database
 *****************************************************************************************/
// Returns the user's current working directory for the new inidb database.
wxString getIniDbPpath()
{
    wxFileName fn(wxFileName::GetCwd(), wxT("mmexinidb_test.db3"));
    return fn.GetFullPath();
}

boost::shared_ptr<wxSQLite3Database> get_pInidb()
{
    static Cleanup temp_IniDatabase(getIniDbPpath(), true);
    static boost::shared_ptr<wxSQLite3Database> pInidb(new wxSQLite3Database);

    if (!pInidb->IsOpen())
    {
        wxString path = getIniDbPpath();
        wxRemoveFile(path);
        
        pInidb->Open(path);
    }
    return pInidb;
}

// Single point access for the test database, stored in memory.
boost::shared_ptr<MMEX_IniSettings> pSettingsList()
{
    static boost::shared_ptr<MMEX_IniSettings> pIniList(new MMEX_IniSettings(get_pInidb()));

    return pIniList;
}
/*****************************************************************************************/

/*****************************************************************************************
 Create a single access point for the database, Remove database on completion.
 *****************************************************************************************/
// Returns the user's current working directory for the new database.
wxString getDbPath()
{
    wxFileName fn(wxFileName::GetCwd(), wxT("mmex_tests.db3"));
    return fn.GetFullPath();
}

boost::shared_ptr<wxSQLite3Database> get_pDb()
{
    static Cleanup temp_database(getDbPath());
    static boost::shared_ptr<wxSQLite3Database> pDb(new wxSQLite3Database);

    if (!pDb->IsOpen())
    {
        wxString path = getDbPath();
        wxRemoveFile(path);

        pDb->Open(path);
    }

    return pDb;
}

// Create a single access point for the main database, stored in memory.
boost::shared_ptr<mmCoreDB> pDb_core()
{
    static boost::shared_ptr<mmCoreDB> pCore(new mmCoreDB(get_pDb(), pSettingsList()));

    return pCore;
}
/*****************************************************************************************/
 
void const displayTimeTaken(const wxString msg, const wxDateTime start_time)
{
    const wxDateTime end_time(wxDateTime::UNow());
    const wxString time_dif = (end_time - start_time).Format(wxT("%S%l"));

    printf(wxString::Format(wxT("Time Taken: %s milliseconds - Test: %s \n"), time_dif.c_str(), msg.c_str()).char_str());
}
/*****************************************************************************************/

void display_STD_IO_separation_line()
{
    printf("\n___________________________________________________________________\n");
}

void display_STD_IO_line()
{
    printf("\n----------------------\n");
}
