/*******************************************************
 Copyright (C) 2006 Madhan Kanagavel

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
 /*******************************************************/


#include "mmcoredb.h"

mmCoreDB::mmCoreDB(boost::shared_ptr<wxSQLite3Database> db)
: db_ (db)
{
   // Let's load the DB
   wxString sqlString = wxT("select * from ACCOUNTLIST_V1 order by ACCOUNTNAME;");
   
    wxSQLite3ResultSet q1 = db_->ExecuteQuery(sqlString);
    while (q1.NextRow())
    {
       mmAccount* ptrBase;
       if (q1.GetString(wxT("ACCOUNTTYPE")) == wxT("Checking"))
         ptrBase = new mmCheckingAccount(q1);
       else
         ptrBase = new mmInvestmentAccount(q1);

      boost::shared_ptr<mmAccount> pAccount(ptrBase);
       
       accounts_.push_back(pAccount);
    }
    q1.Finalize();
}

mmCoreDB::~mmCoreDB()
{

}

 