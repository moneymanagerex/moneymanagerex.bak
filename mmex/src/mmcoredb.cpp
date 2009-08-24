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
#include "util.h"

mmCoreDB::mmCoreDB(boost::shared_ptr<wxSQLite3Database> db)
: db_ (db),
  payeeList_(db),
  categoryList_(db),
  accountList_(db),
  currencyList_(db),
  bTransactionList_(db)
{
    if (!db_)
        throw wxString(wxT("Database Handle is invalid!"));

    mmBEGINSQL_LITE_EXCEPTION;
    
    /* Load the DB into memory */

    /* Load the Options */
    mmOptions::loadOptions(db_.get());

    /* Load the Currencies */
    wxString sqlString =  wxT("select * from CURRENCYFORMATS_V1 order by CURRENCYNAME;");
    wxSQLite3ResultSet q1 = db_->ExecuteQuery(sqlString);
    while (q1.NextRow())
    {
       boost::shared_ptr<mmCurrency> pCurrency(new mmCurrency(db_, q1));
       currencyList_.currencies_.push_back(pCurrency);
    }
    q1.Finalize();

    /* Load the Categories */
    sqlString = wxT("select * from CATEGORY_V1 order by CATEGNAME;");
    wxSQLite3StatementBuffer bufSQL1;
    q1 = db_->ExecuteQuery(sqlString);
    while (q1.NextRow())
    {
       int categID          = q1.GetInt(wxT("CATEGID"));
       boost::shared_ptr<mmCategory> pCategory(
          new mmCategory(categID, q1.GetString(wxT("CATEGNAME"))));
    
       /* Load the SubCategories */
       bufSQL1.Format("select * from SUBCATEGORY_V1 where CATEGID=%d;", categID);
       wxSQLite3ResultSet q2 = db_->ExecuteQuery(bufSQL1); 
       while(q2.NextRow())
       {
           int subcategID             = q2.GetInt(wxT("SUBCATEGID"));
           boost::shared_ptr<mmCategory> pSubCategory(
              new mmCategory(subcategID, q2.GetString(wxT("SUBCATEGNAME"))));
           pSubCategory->parent_ = pCategory;

           pCategory->children_.push_back(pSubCategory);
       }
       q2.Finalize();
       categoryList_.categories_.push_back(pCategory);
    }
    q1.Finalize();

    /* Load the Payees */
    sqlString = wxT("select * from PAYEE_V1 order by PAYEENAME;");
    q1 = db_->ExecuteQuery(sqlString);
    while (q1.NextRow())
    {
        int payeeID = q1.GetInt(wxT("PAYEEID"));
        int categID = q1.GetInt(wxT("CATEGID"));
        int subCategID = q1.GetInt(wxT("SUBCATEGID"));

        boost::shared_ptr<mmPayee> pPayee(
           new mmPayee(payeeID, q1.GetString(wxT("PAYEENAME")), 
           categoryList_.getCategorySharedPtr(categID, subCategID)));
        payeeList_.payees_.push_back(pPayee);
    }
    q1.Finalize();
    payeeList_.sortPayeeList();

    /* Load the Accounts */
    sqlString = wxT("select * from ACCOUNTLIST_V1 order by STATUS desc, FAVORITEACCT desc, ACCOUNTNAME;");
    q1 = db_->ExecuteQuery(sqlString);
    while (q1.NextRow())
    {
        mmAccount* ptrBase;
        if (q1.GetString(wxT("ACCOUNTTYPE")) == wxT("Checking"))
            ptrBase = new mmCheckingAccount(db_, q1);
        else
            ptrBase = new mmInvestmentAccount(db_, q1);

        boost::weak_ptr<mmCurrency> pCurrency
           = currencyList_.getCurrencySharedPtr(q1.GetInt(wxT("CURRENCYID")));
        ptrBase->currency_ = pCurrency;
        boost::shared_ptr<mmAccount> pAccount(ptrBase);
        accountList_.accounts_.push_back(pAccount);
    }
    q1.Finalize();

    /* Load the Transactions */
    sqlString = wxT("select * from CHECKINGACCOUNT_V1;");
    q1 = db_->ExecuteQuery(sqlString);
    while (q1.NextRow())
    {
       boost::shared_ptr<mmBankTransaction> ptrBase(new mmBankTransaction(this, q1));
       bTransactionList_.transactions_.push_back(ptrBase);
    }
    q1.Finalize();
    mmENDSQL_LITE_EXCEPTION;

    // Update All transactions in case of errors
    bTransactionList_.updateAllTransactions();
}

mmCoreDB::~mmCoreDB()
{
}

