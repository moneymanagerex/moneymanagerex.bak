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

mmAccount::mmAccount(boost::shared_ptr<wxSQLite3Database> db)
: mmDBInterface(db)
{

}

mmAccount::mmAccount(boost::shared_ptr<wxSQLite3Database> db, 
                     wxSQLite3ResultSet& q1)
    : mmDBInterface(db)
{
    bool favoriteAcct_;
   
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

void mmCheckingAccount::deleteTransactions(int accountID)
{
    std::vector<boost::weak_ptr<mmTransaction> >::iterator iter;

    for (iter = transactions_.begin(); iter != transactions_.end(); )
    {
         boost::shared_ptr<mmTransaction> sptr = (*iter).lock();
         if (!sptr)
         {
            // transaction has gone away, delete it
             iter = transactions_.erase(iter);
         }
         else
         {
             mmBankTransaction* pBankTransaction = dynamic_cast<mmBankTransaction*>(sptr.get());
             if (pBankTransaction)
             {
                 if ((pBankTransaction->accountID_ == accountID) ||
                     (pBankTransaction->toAccountID_))
                 {
                     wxASSERT(false); // we should never have to be here
                 }
             }
             ++iter;
         }
    }
}

void mmCheckingAccount::deleteGlobalTransactions(int accountID)
{
    std::vector< boost::shared_ptr<mmTransaction> >::iterator i;
    for (i = gTransactions_.begin(); i!= gTransactions_.end(); )
    {
        boost::shared_ptr<mmTransaction> sptr = *i;
        mmBankTransaction* pBankTransaction = dynamic_cast<mmBankTransaction*>(sptr.get());
        if (pBankTransaction)
        {
            if ((pBankTransaction->accountID_ == accountID) ||
                (pBankTransaction->toAccountID_))
            {
                i = gTransactions_.erase(i);
            }
            else
                ++i;
        }
    }
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
        else
            wxASSERT(false);
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

// --------------------------------------------------------

bool mmAccountList::deleteAccount(int accountID)
{
    mmBEGINSQL_LITE_EXCEPTION;
    wxString acctType = getAccountType(accountID);
    if (acctType = wxT("Checking"))
    {
        mmCheckingAccount::deleteGlobalTransactions(accountID);
        for (int i = 0; i < accounts_.size(); i++)
        {
            mmCheckingAccount* pChk = dynamic_cast<mmCheckingAccount*>(accounts_[i].get());
            if (pChk)
            {
                pChk->deleteTransactions(accountID);
            }
        }

        wxSQLite3StatementBuffer bufSQL;
        bufSQL.Format("delete from CHECKINGACCOUNT_V1 where ACCOUNTID=%d OR TOACCOUNTID=%d;", accountID, accountID);
        int nTransDeleted = db_.get()->ExecuteUpdate(bufSQL);

        bufSQL.Format("delete from ACCOUNTLIST_V1 where ACCOUNTID=%d;", accountID);
        int nRows = db_.get()->ExecuteUpdate(bufSQL);
        wxASSERT(nRows);
    }
    else if (acctType == wxT("Investment"))
    {
        wxSQLite3StatementBuffer bufSQL;
        bufSQL.Format("delete from STOCK_V1 where HELDAT=%d;", accountID);
        int nTransDeleted = db_.get()->ExecuteUpdate(bufSQL);

        bufSQL.Format("delete from ACCOUNTLIST_V1 where ACCOUNTID=%d;", accountID);
        int nRows = db_.get()->ExecuteUpdate(bufSQL);
        wxASSERT(nRows);
    }

    std::vector<boost::shared_ptr<mmAccount> >::iterator iter;
    for (iter = accounts_.begin(); iter != accounts_.end(); )
    {
        boost::shared_ptr<mmAccount> pAccount = (*iter);
        if (pAccount->accountID_ == accountID)
        {
            iter = accounts_.erase(iter);
            break;
        }
        else
            ++iter;
    }

    mmENDSQL_LITE_EXCEPTION;



    return true;
}

void mmAccountList::updateAccount(boost::shared_ptr<mmAccount> pAccount)
{
   mmBEGINSQL_LITE_EXCEPTION;

   wxString statusStr = wxT("Open");
   if (pAccount->status_ == mmAccount::MMEX_Closed)
      statusStr = wxT("Closed");

   wxString favStr = wxT("TRUE");
   if (!pAccount->favoriteAcct_)
      favStr = wxT("FALSE");

   boost::shared_ptr<mmCurrency> pCurrency = pAccount->currency_.lock();
   wxASSERT(pCurrency);
   int currencyID = pCurrency->currencyID_;


   wxString bufSQL = wxString::Format(wxT("update ACCOUNTLIST_V1 SET ACCOUNTNAME='%s', ACCOUNTTYPE='%s', ACCOUNTNUM='%s', \
                                          STATUS='%s', NOTES='%s', HELDAT='%s', WEBSITE='%s', CONTACTINFO='%s',  ACCESSINFO='%s',                               \
                                          INITIALBAL=%f, FAVORITEACCT='%s', CURRENCYID=%d                     \
                                          where ACCOUNTID=%d;"), 
                                          pAccount->accountName_.c_str(), pAccount->acctType_.c_str(), 
                                          pAccount->accountNum_.c_str(),  
                                          statusStr, 
                                          pAccount->notes_.c_str(), 
                                          pAccount->heldAt_.c_str(), 
                                          pAccount->website_.c_str(),
                                          pAccount->contactInfo_.c_str(), 
                                          pAccount->accessInfo_.c_str(),
                                          pAccount->initialBalance_, 
                                          favStr.c_str(), 
                                          currencyID, 
                                          pAccount->accountID_);

   int retVal = db_->ExecuteUpdate(bufSQL);

   mmENDSQL_LITE_EXCEPTION;
}

boost::shared_ptr<mmAccount> mmAccountList::getAccountSharedPtr(int accountID)
{
    std::vector<boost::shared_ptr<mmAccount> >::iterator iter;
    for (iter = accounts_.begin(); iter != accounts_.end(); )
    {
        boost::shared_ptr<mmAccount> pAccount = (*iter);
        if (pAccount->accountID_ == accountID)
        {
           return (*iter);
        }
    }
    return boost::shared_ptr<mmAccount>();
}
    
bool mmAccountList::accountExists(const wxString& accountName)
{
   int checkAcctID = mmDBWrapper::getAccountID(db_.get(), accountName);
   if (checkAcctID != -1)
      return true;

    return false;
}

int mmAccountList::getAccountID(const wxString& accountName)
{
   return mmDBWrapper::getAccountID(db_.get(), accountName);
}

wxString mmAccountList::getAccountType(int accountID)
{
    int len = (int)accounts_.size();
    for (int idx = 0; idx < len; idx++)
    {
        if (accounts_[idx]->accountID_ == accountID)
            return accounts_[idx]->acctType_;
    }
    wxASSERT(false);
    return wxT("");
}

void mmAccountList::addAccount(boost::shared_ptr<mmAccount> pAccount)
{
      mmBEGINSQL_LITE_EXCEPTION;
    
      wxString statusStr = wxT("Open");
      if (pAccount->status_ == mmAccount::MMEX_Closed)
         statusStr = wxT("Closed");

      wxString favStr = wxT("TRUE");
      if (!pAccount->favoriteAcct_)
         favStr = wxT("FALSE");

      boost::shared_ptr<mmCurrency> pCurrency = pAccount->currency_.lock();
      wxASSERT(pCurrency);
      int currencyID = pCurrency->currencyID_;
      
      wxString bufSQL = wxString::Format(wxT("insert into ACCOUNTLIST_V1 (ACCOUNTNAME, ACCOUNTTYPE, ACCOUNTNUM, \
                                               STATUS, NOTES, HELDAT, WEBSITE, CONTACTINFO, ACCESSINFO,                                 \
                                               INITIALBAL, FAVORITEACCT, CURRENCYID)                      \
                                               values ('%s', '%s', '%s', '%s', '%s', '%s', '%s', '%s', '%s', %f, '%s', %d );"), 
                                               pAccount->accountName_.c_str(), pAccount->acctType_.c_str(), 
                                               pAccount->accountNum_.c_str(),  
                                               statusStr, 
                                               pAccount->notes_.c_str(), 
                                               pAccount->heldAt_.c_str(), 
                                               pAccount->website_.c_str(),
                                               pAccount->contactInfo_.c_str(), 
                                               pAccount->accessInfo_.c_str(),
                                               pAccount->initialBalance_, 
                                               favStr.c_str(), 
                                               currencyID
                                               );

        int retVal = db_->ExecuteUpdate(bufSQL);

        pAccount->accountID_ = db_->GetLastRowId().ToLong();
        accounts_.push_back(pAccount);

        mmENDSQL_LITE_EXCEPTION;
    

}