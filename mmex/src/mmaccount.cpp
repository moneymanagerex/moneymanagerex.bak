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

#include "mmaccount.h"
#include "mmcoredb.h"
#include "util.h"

mmAccount::mmAccount(boost::shared_ptr<wxSQLite3Database> db)
:db_(db)
{

}

mmAccount::mmAccount(boost::shared_ptr<wxSQLite3Database> db, 
                     wxSQLite3ResultSet& q1)
    : db_(db)
{
    accountID_ = q1.GetInt(wxT("ACCOUNTID"));
    accountName_ = q1.GetString(wxT("ACCOUNTNAME"));
    accountNum_  = q1.GetString(wxT("ACCOUNTNUM"));
    heldAt_ = q1.GetString(wxT("HELDAT"));
    website_ = q1.GetString(wxT("WEBSITE"));
    contactInfo_ = q1.GetString(wxT("CONTACTINFO"));
    accessInfo_ = q1.GetString(wxT("ACCESSINFO"));
    notes_ = q1.GetString(wxT("NOTES"));  
	favorite_ = q1.GetString(wxT("FAVORITEACCT"));
    acctType_ = q1.GetString(wxT("ACCOUNTTYPE"));

    status_ =  mmAccount::MMEX_Open;
    if (q1.GetString(wxT("STATUS")) == wxT("Closed"))
        status_ = mmAccount::MMEX_Closed;

    wxString favorite_ = q1.GetString(wxT("FAVORITEACCT"));

    initialBalance_ = q1.GetDouble(wxT("INITIALBAL"));
}

#if 0
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
#endif

mmCheckingAccount::mmCheckingAccount(
                 boost::shared_ptr<wxSQLite3Database> db, 
                 wxSQLite3ResultSet& q1) 
                 : mmAccount(db, q1) 
{
    
}

#if 0
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

void mmCheckingAccount::deleteTransaction(int transactionID)
{
   std::vector< boost::weak_ptr<mmTransaction> >::iterator i;
   for (i = transactions_.begin(); i!= transactions_.end(); )
   {
      boost::shared_ptr<mmTransaction> sptr = (*i).lock();
      wxASSERT(sptr);
      mmBankTransaction* pBankTransaction = dynamic_cast<mmBankTransaction*>(sptr.get());
      if (pBankTransaction)
      {
         if (pBankTransaction->transactionID() == transactionID)
         {
            i = transactions_.erase(i);
            break;
         }
         else
            ++i;
      }
   }
   mmCheckingAccount::deleteGlobalTransaction(this->accountID_, transactionID);
   mmDBWrapper::deleteTransaction(core_->db_.get(), transactionID);
}
#endif

double mmCheckingAccount::balance()
{
   return mmDBWrapper::getTotalBalanceOnAccount(db_.get(), this->accountID_);
}

/* mmInvestmentAccount */
double mmInvestmentAccount::balance()
{
    return 0.0;
}

// --------------------------------------------------------

mmAccountList::mmAccountList(boost::shared_ptr<wxSQLite3Database> db)
       :db_(db) {}

bool mmAccountList::deleteAccount(int accountID)
{
    wxString acctType = getAccountType(accountID);
    if (acctType == wxT("Checking"))
    {
        mmDBWrapper::removeSplitsForAccount(db_.get(), accountID);

        wxSQLite3Statement st = db_->PrepareStatement("delete from CHECKINGACCOUNT_V1 where ACCOUNTID=? OR TOACCOUNTID=?");
        st.Bind(1, accountID);
        st.Bind(2, accountID);

        st.ExecuteUpdate();
        st.Finalize();

        // --

        st = db_->PrepareStatement("delete from BILLSDEPOSITS_V1 where ACCOUNTID=? OR TOACCOUNTID=?");
        st.Bind(1, accountID);
        st.Bind(2, accountID);

        st.ExecuteUpdate();
        st.Finalize();

        // --

        st = db_->PrepareStatement("delete from ACCOUNTLIST_V1 where ACCOUNTID=?");
        st.Bind(1, accountID);

        st.ExecuteUpdate();
        st.Finalize();
    }
    else if (acctType == wxT("Investment"))
    {
        wxSQLite3Statement st = db_->PrepareStatement("delete from STOCK_V1 where HELDAT=?");
        st.Bind(1, accountID);
        st.ExecuteUpdate();
        st.Finalize();

        // --

        st = db_->PrepareStatement("delete from ACCOUNTLIST_V1 where ACCOUNTID=?");
        st.Bind(1, accountID);

        st.ExecuteUpdate();
        st.Finalize();
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

    return true;
}

double mmAccountList::getAccountBaseCurrencyConvRate(int accountID)
{
   boost::weak_ptr<mmCurrency> wpCurrency = getCurrencyWeakPtr(accountID);
   boost::shared_ptr<mmCurrency> pCurrency = wpCurrency.lock();
   wxASSERT(pCurrency);

   if (pCurrency)
      return pCurrency->baseConv_;


   return 0.0;
}

void mmAccountList::updateAccount(boost::shared_ptr<mmAccount> pAccount)
{
   wxString statusStr = wxT("Open");
   if (pAccount->status_ == mmAccount::MMEX_Closed)
      statusStr = wxT("Closed");

   boost::shared_ptr<mmCurrency> pCurrency = pAccount->currency_.lock();
   wxASSERT(pCurrency);
   int currencyID = pCurrency->currencyID_;

    static const char sql[] = 
    "update ACCOUNTLIST_V1 "
    "SET ACCOUNTNAME=?, ACCOUNTTYPE=?, ACCOUNTNUM=?,"
        "STATUS=?, NOTES=?, HELDAT=?, WEBSITE=?, CONTACTINFO=?,  ACCESSINFO=?,"
        "INITIALBAL=?, FAVORITEACCT=?, CURRENCYID=? "
    "where ACCOUNTID = ?";

   wxSQLite3Statement st = db_->PrepareStatement(sql);
   const mmAccount &r = *pAccount;

   int i = 0;
   st.Bind(++i, r.accountName_);
   st.Bind(++i, r.acctType_);
   st.Bind(++i, r.accountNum_);
   st.Bind(++i, statusStr);
   st.Bind(++i, r.notes_);
   st.Bind(++i, r.heldAt_);
   st.Bind(++i, r.website_);
   st.Bind(++i, r.contactInfo_);
   st.Bind(++i, r.accessInfo_);
   st.Bind(++i, r.initialBalance_);
   st.Bind(++i, r.favorite_);
   st.Bind(++i, currencyID);
   st.Bind(++i, r.accountID_);

   wxASSERT(st.GetParamCount() == i);

   st.ExecuteUpdate();
   st.Finalize();
}

boost::shared_ptr<mmAccount> mmAccountList::getAccountSharedPtr(int accountID)
{
    account_v::value_type res;

    for (account_v::const_iterator i = accounts_.begin(); i != accounts_.end(); ++i)
    {
        account_v::const_reference r = *i;
        if (r->accountID_ == accountID)
        {
            res = r;
            break;
        }
    }

    return res;
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

boost::weak_ptr<mmCurrency> mmAccountList::getCurrencyWeakPtr(int accountID)
{
    int len = (int)accounts_.size();
    for (int idx = 0; idx < len; idx++)
    {
        if (accounts_[idx]->accountID_ == accountID)
            return accounts_[idx]->currency_;
    }
    wxASSERT(false);
    return boost::weak_ptr<mmCurrency>();
}

int mmAccountList::addAccount(boost::shared_ptr<mmAccount> pAccount)
{
      wxString statusStr = wxT("Open");
      if (pAccount->status_ == mmAccount::MMEX_Closed)
         statusStr = wxT("Closed");

	  wxString favorite_ = wxT("2");

      boost::shared_ptr<mmCurrency> pCurrency = pAccount->currency_.lock();
      wxASSERT(pCurrency);
      int currencyID = pCurrency->currencyID_;
      
    static const char sql[] =       
    "insert into ACCOUNTLIST_V1 ( "
      "ACCOUNTNAME, ACCOUNTTYPE, ACCOUNTNUM, "
      "STATUS, NOTES, HELDAT, WEBSITE, CONTACTINFO, ACCESSINFO, "
      "INITIALBAL, FAVORITEACCT, CURRENCYID "
    " ) values (?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ? )";

    wxSQLite3Statement st = db_->PrepareStatement(sql);
    const mmAccount &r = *pAccount;

    int i = 0;
    st.Bind(++i, r.accountName_);
    st.Bind(++i, r.acctType_);
    st.Bind(++i, r.accountNum_);
    st.Bind(++i, statusStr);
    st.Bind(++i, r.notes_);
    st.Bind(++i, r.heldAt_);
    st.Bind(++i, r.website_);
    st.Bind(++i, r.contactInfo_);
    st.Bind(++i, r.accessInfo_);
    st.Bind(++i, r.initialBalance_);
	st.Bind(++i, r.favorite_);
    st.Bind(++i, currencyID);

    wxASSERT(st.GetParamCount() == i);
    st.ExecuteUpdate();

    pAccount->accountID_ = db_->GetLastRowId().ToLong();
    accounts_.push_back(pAccount);

    st.Finalize();

    return pAccount->accountID_;
}
