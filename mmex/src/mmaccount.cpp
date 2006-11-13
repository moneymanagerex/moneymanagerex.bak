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

#include "mmaccount.h"

mmAccount::mmAccount(boost::shared_ptr<wxSQLite3Database> db, wxSQLite3ResultSet& q1)
    : mmDBInterface(db)
{
    bool favoriteAcct_;
    boost::shared_ptr< mmCurrency*> currency_;

    accountID_ = q1.GetInt(wxT("ACCOUNTID"));
    accountName_ = q1.GetString(wxT("ACCOUNTNAME"));
    accountNum_  = q1.GetString(wxT("ACCOUNTNUM"));
    heldAt_ = q1.GetString(wxT("HELDAT"));
    website_ = q1.GetString(wxT("WEBSITE"));
    contactInfo_ = q1.GetString(wxT("CONTACTINFO"));
    accessInfo_ = q1.GetString(wxT("ACCESSINFO"));
    notes_ = q1.GetString(wxT("NOTES"));  
    acctType_ = q1.GetString(wxT("ACCOUNTTYPE"));

    status_ =  mmAccount::MMEX_Open;
    if (q1.GetString(wxT("STATUS")) == wxT("Closed"))
        status_ = mmAccount::MMEX_Closed;

    wxString retVal = q1.GetString(wxT("FAVORITEACCT"));
    if (retVal == wxT("TRUE"))
        favoriteAcct_ = true;
    else
        favoriteAcct_ = false;

    initialBalance_ = q1.GetDouble(wxT("INITIALBAL"));
}

void mmAccount::addDBRecord()
{


}
/* mmCheckingAccount */
std::vector<boost::shared_ptr<mmTransaction> > mmCheckingAccount::gTransactions_;

boost::shared_ptr<mmTransaction> mmCheckingAccount::findTransaction(int transactionID)
{
    int len = (int)mmCheckingAccount::gTransactions_.size();
    for (int i = 0; i < len; i++)
    {
        if (mmCheckingAccount::gTransactions_[i]->transactionID() == transactionID)
            return mmCheckingAccount::gTransactions_[i];
    }
    boost::shared_ptr<mmTransaction> ptr;    
    return ptr;
}

mmCheckingAccount::mmCheckingAccount(boost::shared_ptr<wxSQLite3Database> db, 
                 wxSQLite3ResultSet& q1) 
                 : mmAccount(db, q1) 
{
    mmBEGINSQL_LITE_EXCEPTION;

    wxSQLite3StatementBuffer bufSQL;
    bufSQL.Format("select * from CHECKINGACCOUNT_V1 where ACCOUNTID=%d OR TOACCOUNTID=%d;", 
        accountID_, accountID_);

    wxSQLite3ResultSet q2 = db_->ExecuteQuery(bufSQL);
    while (q2.NextRow())
    {
        bool addToGlobal = true;
        mmTransaction* ptrBase;
        if (q2.GetString(wxT("TRANSCODE")) == wxT("Withdrawal"))
        {
            ptrBase = new mmWithdrawalTransaction(db_, q2);
            boost::shared_ptr<mmTransaction> pTransaction(ptrBase);
            mmCheckingAccount::gTransactions_.push_back(pTransaction);
            transactions_.push_back(boost::weak_ptr<mmTransaction>(pTransaction));
        }
        else if (q2.GetString(wxT("TRANSCODE")) == wxT("Deposit"))
        {
            ptrBase = new mmDepositTransaction(db_, q2);
            boost::shared_ptr<mmTransaction> pTransaction(ptrBase);
            transactions_.push_back(boost::weak_ptr<mmTransaction>(pTransaction));
            mmCheckingAccount::gTransactions_.push_back(pTransaction);
        }
        else if (q2.GetString(wxT("TRANSCODE")) == wxT("Transfer"))
        {
           /* Check if transaction needs to tbe added to the global tranaction list */
           boost::shared_ptr<mmTransaction> pTrans = mmCheckingAccount::findTransaction(q2.GetInt(wxT("TRANSID")));
           if (pTrans)
           {
               transactions_.push_back(pTrans);
           }
           else
           {
               ptrBase = new mmTransferTransaction(db_, q2);
               boost::shared_ptr<mmTransaction> pTransaction(ptrBase);
               transactions_.push_back(boost::weak_ptr<mmTransaction>(pTransaction));
               mmCheckingAccount::gTransactions_.push_back(pTransaction);
           }
        }
    }
    q2.Finalize();

    mmENDSQL_LITE_EXCEPTION;
}

double mmCheckingAccount::balance()
{
    double balance = initialBalance_;
    int len = (int)transactions_.size();
    for (int idx = 0; idx < len; idx++)
    {
        boost::shared_ptr<mmTransaction> sptr = transactions_[idx].lock();
        if (sptr)
            balance += sptr->value(accountID_);
    }

    return balance;
}

/* mmAssetAccount */
double mmAssetAccount::balance()
{
    return 0.0;
}

/* mmInvestmentAccount */
double mmInvestmentAccount::balance()
{
    return 0.0;
}