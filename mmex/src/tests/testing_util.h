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

#pragma once
/****************************************************************************
 Revision of last commit: $Revision: 3245 $
 Author of last commit:   $Author: stef145g $
 ****************************************************************************/
#include <boost/scoped_ptr.hpp>
#include <wx/filename.h>
//----------------------------------------------------------------------------
#include "dbwrapper.h"
#include "mmcoredb.h"
#include "mmex_settings.h"
#include <iostream>
//----------------------------------------------------------------------------

const wxString g_BudgetYear   = wxT("2009");
const wxString g_CategName    = wxT("new category");
const wxString g_SubCategName = wxT("new subcategory");
const wxString g_CurrencyName = wxT("US Dollar");
const wxString g_PayeeName    = wxT("Payee #1");

const wxString g_TransType_deposit    = wxT("Deposit");
const wxString g_TransType_withdrawal = wxT("Withdrawal");
const wxString g_status_reconciled    = wxT("R");
const wxString g_status_void          = wxT("V");
//----------------------------------------------------------------------------

/*****************************************************************************************
 The test platform will create and test a new database.

 This class is used to remove the temporary database on completion.
 *****************************************************************************************/
class Cleanup
{
public:
    Cleanup(wxString filename, bool pause = false);
    ~Cleanup();
private:
    wxString dbFileName_;
    bool pause_;
};


/*****************************************************************************************
 Create a single access point for the ini_settings database
 *****************************************************************************************/
// Returns the user's current working directory for the new inidb database.
wxString getIniDbPpath();

boost::shared_ptr<wxSQLite3Database> get_pInidb();

// Single point access for the test database, stored in memory.
boost::shared_ptr<MMEX_IniSettings> pSettingsList();

/*****************************************************************************************
 Create a single access point for the database, Remove database on completion.
 *****************************************************************************************/
// Returns the user's current working directory for the new database.
wxString getDbPath();
boost::shared_ptr<wxSQLite3Database> get_pDb();

// Create a single access point for the main database, stored in memory.
boost::shared_ptr<mmCoreDB> pDb_core();
 
void const displayTimeTaken(const wxString msg, const wxDateTime start_time);

// End of file
