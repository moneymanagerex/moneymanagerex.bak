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
 ********************************************************/

#include "mmcoredb.h"
#include "constants.h"
#include "guiid.h"
#include "util.h"
#include "mmOption.h"

//----------------------------------------------------------------------------
mmCoreDB::mmCoreDB(wxSharedPtr<wxSQLite3Database> db)
: db_(db)
, categoryList_(this)
, accountList_(this)
, currencyList_(db)
, bTransactionList_(this)
, displayDatabaseError_(true)
{
    if (!db_)
    {
        throw wxSQLite3Exception(WXSQLITE_ERROR, "Null pointer to database");
    }

    mmOptions::instance().loadOptions();

    /* Create the appropriate tables first if required */
    mmDBWrapper::initDB(db_.get());

    /* Load the DB into memory */
    currencyList_.LoadCurrencies();             // populate currencyList_
    categoryList_.LoadCategories();             // populate categoryList_
    accountList_.LoadAccounts(currencyList_);   // populate accountList_
    bTransactionList_.LoadTransactions();   // populate bTransactionList_
}

mmCoreDB::~mmCoreDB()
{
}
//----------------------------------------------------------------------------
