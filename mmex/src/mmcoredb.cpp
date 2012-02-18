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

namespace
{

void loadCurrencies(boost::shared_ptr<wxSQLite3Database> db_, mmCurrencyList &cur_list)
{
    wxSQLite3ResultSet q1 = db_->ExecuteQuery("select * "
                                              "from CURRENCYFORMATS_V1 "
                                              "order by CURRENCYNAME"
                                             );

    while (q1.NextRow())
    {
        boost::shared_ptr<mmCurrency> pCurrency(new mmCurrency(db_, q1));
        cur_list.currencies_.push_back(pCurrency);
    }

    q1.Finalize();
}
//----------------------------------------------------------------------------

void loadCategories(boost::shared_ptr<wxSQLite3Database> db_, mmCategoryList &cat_list)
{
    static const char sql[] = 
    "select c.CATEGID, c.CATEGNAME, "
           "sc.SUBCATEGID, sc.SUBCATEGNAME "
    "from CATEGORY_V1 c "
    "left join SUBCATEGORY_V1 sc "
    "on sc.CATEGID = c.CATEGID "
    "order by c.CATEGNAME, sc.SUBCATEGNAME";

    boost::shared_ptr<mmCategory> pCat;
    wxSQLite3ResultSet q1 = db_->ExecuteQuery(sql);

    while (q1.NextRow())
    {
        int catID = q1.GetInt(wxT("CATEGID"));
        
        if (!pCat || pCat->categID_ != catID)
        {
            if (pCat) cat_list.entries_.push_back(pCat);

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
        cat_list.entries_.push_back(pCat);
    }
}
//----------------------------------------------------------------------------

void loadPayees(boost::shared_ptr<wxSQLite3Database> db_, 
                mmCategoryList &cat_list, 
                mmPayeeList &payee_list
               )
{
    static const char sql[] = 
    "select PAYEEID, PAYEENAME, CATEGID, SUBCATEGID "
    "from PAYEE_V1 "
    "order by PAYEENAME";

    wxSQLite3ResultSet q1 = db_->ExecuteQuery(sql);

    while (q1.NextRow())
    {
        int payeeID = q1.GetInt(wxT("PAYEEID"));
        int categID = q1.GetInt(wxT("CATEGID"));
        int subCategID = q1.GetInt(wxT("SUBCATEGID"));

        boost::shared_ptr<mmPayee> pPayee(new mmPayee(payeeID, q1.GetString(wxT("PAYEENAME")), 
                                                      cat_list.getCategorySharedPtr(categID, subCategID)
                                                     )
                                         );

        payee_list.entries_.push_back(pPayee);
    }

    q1.Finalize();
    payee_list.sortPayeeList();
}
//----------------------------------------------------------------------------

void loadAccounts(boost::shared_ptr<wxSQLite3Database> db_, 
                  mmCurrencyList &cur_list,
                  mmAccountList &acc_list
                 )
{
    static const char sql[] = 
    "select * "
    "from ACCOUNTLIST_V1 "
    "order by ACCOUNTNAME";

    wxSQLite3ResultSet q1 = db_->ExecuteQuery(sql);

    while (q1.NextRow())
    {
        mmAccount* ptrBase = new mmAccount(q1);

        boost::weak_ptr<mmCurrency> pCurrency = cur_list.getCurrencySharedPtr(q1.GetInt(wxT("CURRENCYID")));
        ptrBase->currency_ = pCurrency;

        boost::shared_ptr<mmAccount> pAccount(ptrBase);
        acc_list.accounts_.push_back(pAccount);
    }

    q1.Finalize();
}
//----------------------------------------------------------------------------

void loadTransactions(boost::shared_ptr<wxSQLite3Database> db_, 
                      mmCoreDB *theCore,
                      mmBankTransactionList &trans_list
                     )
{
    wxSQLite3ResultSet q1 = db_->ExecuteQuery("select * from CHECKINGACCOUNT_V1");

    while (q1.NextRow())
    {
        boost::shared_ptr<mmBankTransaction> ptrBase(new mmBankTransaction(theCore, q1));
        trans_list.transactions_.push_back(ptrBase);
    }

    q1.Finalize();
}
//----------------------------------------------------------------------------

} // namespace

//----------------------------------------------------------------------------

mmCoreDB::mmCoreDB(boost::shared_ptr<wxSQLite3Database> db) : 
    db_(db),
    payeeList_(db),
    categoryList_(db),
    accountList_(db),
    currencyList_(db),
    bTransactionList_(db),
    displayDatabaseError_(true)
{
    if (!db_)
        throw wxSQLite3Exception(WXSQLITE_ERROR, wxT("Null pointer to database"));

    /* Load the DB into memory */

    mmOptions::instance().loadOptions(db_.get());
    loadCurrencies(db_, currencyList_);
    loadCategories(db_, categoryList_);
    loadPayees(db_, categoryList_, payeeList_);
    loadAccounts(db_, currencyList_, accountList_);
    loadTransactions(db_, this, bTransactionList_);

    // Update All transactions in case of errors
    bTransactionList_.updateAllTransactions();
}
//----------------------------------------------------------------------------
