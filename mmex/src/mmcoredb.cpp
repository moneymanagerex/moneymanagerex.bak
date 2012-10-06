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
#include "util.h"

//----------------------------------------------------------------------------
mmCoreDB::mmCoreDB(boost::shared_ptr<wxSQLite3Database> db, boost::shared_ptr<MMEX_IniSettings> iniSettings)
: db_(db)
, iniSettings_(iniSettings)
, payeeList_(db)
, categoryList_(db)
, accountList_(db)
, currencyList_(db)
, bTransactionList_(db)
, displayDatabaseError_(true)
{
    if (!db_)
    {
        throw wxSQLite3Exception(WXSQLITE_ERROR, wxT("Null pointer to database"));
    }

    mmOptions::instance().loadOptions(db_.get());

    /* Load the DB into memory */
    currencyList_.LoadCurrencies();             // populate currencyList_
    LoadCategories();                           // populate categoryList_
    payeeList_.LoadPayees();                    // populate payeeList_
    accountList_.LoadAccounts(currencyList_);   // populate accountList_
    LoadTransactions();                         // populate bTransactionList_

    // Update All transactions in case of errors
    bTransactionList_.updateAllTransactions();
}
//----------------------------------------------------------------------------

void mmCoreDB::LoadCategories()
{
    boost::shared_ptr<mmCategory> pCat;
    wxSQLite3ResultSet q1 = db_->ExecuteQuery(SELECT_ALL_CATEGORIES);

    while (q1.NextRow())
    {
        int catID = q1.GetInt(wxT("CATEGID"));
        
        if (!pCat || pCat->categID_ != catID)
        {
            if (pCat)
            {
                categoryList_.entries_.push_back(pCat);
            }
            pCat.reset(new mmCategory(catID, q1.GetString(wxT("CATEGNAME"))));
        }

        int sub_idx = q1.FindColumnIndex(wxT("SUBCATEGID"));
        wxASSERT(sub_idx);

        if (!q1.IsNull(sub_idx))
        {
            int subcatID = q1.GetInt(sub_idx);
            boost::shared_ptr<mmCategory> pSubCat(new mmCategory(subcatID, q1.GetString(wxT("SUBCATEGNAME"))));

            pSubCat->parent_ = pCat;
            pCat->children_.push_back(pSubCat);
        }
    }

    q1.Finalize();

    if (pCat)
    {
        categoryList_.entries_.push_back(pCat);
    }
}
//----------------------------------------------------------------------------

void mmCoreDB::LoadTransactions()
{
    wxSQLite3ResultSet q1 = db_->ExecuteQuery(SELECT_ALL_FROM_CHECKINGACCOUNT_V1);

    while (q1.NextRow())
    {
        boost::shared_ptr<mmBankTransaction> ptrBase(new mmBankTransaction(this, q1));
        bTransactionList_.transactions_.push_back(ptrBase);
    }

    q1.Finalize();
}
//----------------------------------------------------------------------------
