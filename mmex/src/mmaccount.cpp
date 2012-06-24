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
#include "dbwrapper.h"
#include "util.h"
#include "mmcurrency.h"

mmAccount::mmAccount(wxSQLite3ResultSet& q1)
{
    id_ = q1.GetInt("ACCOUNTID");
    name_ = q1.GetString("ACCOUNTNAME");
    accountNum_  = q1.GetString("ACCOUNTNUM");
    heldAt_ = q1.GetString("HELDAT");
    website_ = q1.GetString("WEBSITE");
    contactInfo_ = q1.GetString("CONTACTINFO");
    accessInfo_ = q1.GetString("ACCESSINFO");
    notes_ = q1.GetString("NOTES");  
    acctType_ = q1.GetString("ACCOUNTTYPE");

    status_ =  mmAccount::MMEX_Open;
    if (q1.GetString("STATUS") == ("Closed"))
        status_ = mmAccount::MMEX_Closed;

    wxString retVal = q1.GetString("FAVORITEACCT");
    if (retVal == "TRUE")
        favoriteAcct_ = true;
    else
        favoriteAcct_ = false;

    initialBalance_ = q1.GetDouble("INITIALBAL");
}

mmAccountList::mmAccountList(boost::shared_ptr<wxSQLite3Database> db)
       :db_(db) {}

bool mmAccountList::remove(int accountID)
{
    wxString acctType = getAccountType(accountID);
    if (acctType == ACCOUNT_TYPE_BANK || acctType == ACCOUNT_TYPE_TERM)
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
    else if (acctType == ACCOUNT_TYPE_STOCK)
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
    mmOptions::instance().databaseUpdated_ = true;

    std::vector<boost::shared_ptr<mmAccount> >::iterator iter;
    for (iter = accounts_.begin(); iter != accounts_.end(); )
    {
        boost::shared_ptr<mmAccount> pAccount = (*iter);
        if (pAccount->id_ == accountID)
        {
            iter = accounts_.erase(iter);
            break;
        }
        else
            ++iter;
    }

    return true;
}

double mmAccountList::getAccountBaseCurrencyConvRate(int accountID) const
{
   boost::weak_ptr<mmCurrency> wpCurrency = getCurrencyWeakPtr(accountID);
   boost::shared_ptr<mmCurrency> pCurrency = wpCurrency.lock();
   wxASSERT(pCurrency);

   if (pCurrency)
      return pCurrency->baseConv_;

   return 0.0;
}

void mmAccountList::update(boost::shared_ptr<mmAccount> pAccount)
{
   wxString statusStr = ("Open");
   if (pAccount->status_ == mmAccount::MMEX_Closed)
      statusStr = ("Closed");

   wxString favStr = ("TRUE");
   if (!pAccount->favoriteAcct_)
      favStr = ("FALSE");

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
   st.Bind(++i, r.name_);
   st.Bind(++i, r.acctType_);
   st.Bind(++i, r.accountNum_);
   st.Bind(++i, statusStr);
   st.Bind(++i, r.notes_);
   st.Bind(++i, r.heldAt_);
   st.Bind(++i, r.website_);
   st.Bind(++i, r.contactInfo_);
   st.Bind(++i, r.accessInfo_);
   st.Bind(++i, r.initialBalance_);
   st.Bind(++i, favStr);
   st.Bind(++i, currencyID);
   st.Bind(++i, r.id_);

   wxASSERT(st.GetParamCount() == i);

   st.ExecuteUpdate();
   st.Finalize();
   mmOptions::instance().databaseUpdated_ = true;
}

boost::shared_ptr<mmAccount> mmAccountList::getAccountSharedPtr(int accountID) const
{
    account_v::value_type res;

    for (account_v::const_iterator i = accounts_.begin(); i != accounts_.end(); ++i)
    {
        account_v::const_reference r = *i;
        if (r->id_ == accountID)
        {
            res = r;
            break;
        }
    }

    return res;
}
    
bool mmAccountList::exists(const wxString& accountName) const
{
    for (const_iterator it = accounts_.begin(); it != accounts_.end(); ++ it)
    {
        if ((*it)->name_ == accountName) return true;
    }

    return false;
}

bool mmAccountList::has_term_account() const
{
    for (const_iterator it = accounts_.begin(); it != accounts_.end(); ++ it)
    {
        if ((*it)->acctType_ == ACCOUNT_TYPE_TERM) return true;
    }

    return false;
}

bool mmAccountList::has_stock_account() const
{
    for (const_iterator it = accounts_.begin(); it != accounts_.end(); ++ it)
    {
        if ((*it)->acctType_ == ACCOUNT_TYPE_STOCK) return true;
    }

    return false;
}

int mmAccountList::getNumBankAccounts() const
{
    int num = 0;
    for (const_iterator it = accounts_.begin(); it != accounts_.end(); ++ it)
    {
        if ((*it)->acctType_ != ACCOUNT_TYPE_STOCK) ++ num; 
    }

    return num;
}

int mmAccountList::getID(const wxString& accountName) const
{
    for (const_iterator it = accounts_.begin(); it != accounts_.end(); ++ it)
    {
        if ((*it)->name_ == accountName) return (*it)->id_;
    }

   return -1;
}

wxString mmAccountList::getName(int accountID) const
{
    for (const_iterator it = accounts_.begin(); it != accounts_.end(); ++ it)
    {
        if ((*it)->id_ == accountID) return (*it)->name_;
    }

    return wxEmptyString;
}

wxArrayString mmAccountList::getAccountsName(int except_id) const
{
    wxArrayString as;
    for (const_iterator it = accounts_.begin(); it != accounts_.end(); ++ it)
    {
        const mmAccount* account = it->get();
        if ((account->acctType_ == ACCOUNT_TYPE_TERM || account->acctType_ == ACCOUNT_TYPE_BANK) 
            && account->status_ != mmAccount::MMEX_Closed && account->id_ != except_id)
        {
            as.Add(account->name_);
        }
    }

    return as;
}

wxString mmAccountList::getAccountType(int accountID) const
{
    for (account_v::const_iterator it = accounts_.begin(); it != accounts_.end(); ++ it)
    {
        if ((*it)->id_ == accountID) return (*it)->acctType_;
    }

    wxASSERT(false);
    return ("");
}

mmAccount::AccountStatus mmAccountList::getAccountStatus(int accountID) const
{
    for (account_v::const_iterator it = accounts_.begin(); it != accounts_.end(); ++ it)
    {
        if ((*it)->id_ == accountID) return (*it)->status_;
    }

    wxASSERT(false);
    return mmAccount::MMEX_Open;
}
    
wxString mmAccountList::getAccountCurrencyDecimalChar(int accountID) const
{
    boost::weak_ptr<mmCurrency> wpCurrency = getCurrencyWeakPtr(accountID);
    boost::shared_ptr<mmCurrency> pCurrency = wpCurrency.lock();
    wxASSERT(pCurrency);

    if (pCurrency)
        return pCurrency->decChar_;

    return (".");
}

wxString mmAccountList::getAccountCurrencyGroupChar(int accountID) const
{
    boost::weak_ptr<mmCurrency> wpCurrency = getCurrencyWeakPtr(accountID);
    boost::shared_ptr<mmCurrency> pCurrency = wpCurrency.lock();
    wxASSERT(pCurrency);

    if (pCurrency)
        return pCurrency->grpChar_;

    return (",");
}

wxString mmAccountList::getAccountCurrencyName(int accountID) const
{
    boost::weak_ptr<mmCurrency> wpCurrency = getCurrencyWeakPtr(accountID);
    boost::shared_ptr<mmCurrency> pCurrency = wpCurrency.lock();
    wxASSERT(pCurrency);

    if (pCurrency)
        return pCurrency->currencyName_;

    return ("");
}


boost::weak_ptr<mmCurrency> mmAccountList::getCurrencyWeakPtr(int accountID) const
{
    int len = (int)accounts_.size();
    for (int idx = 0; idx < len; idx++)
    {
        if (accounts_[idx]->id_ == accountID)
            return accounts_[idx]->currency_;
    }
    wxASSERT(false);
    return boost::weak_ptr<mmCurrency>();
}

int mmAccountList::add(boost::shared_ptr<mmAccount> pAccount)
{
      wxString statusStr = ("Open");
      if (pAccount->status_ == mmAccount::MMEX_Closed)
         statusStr = ("Closed");

      wxString favStr = ("TRUE");
      if (!pAccount->favoriteAcct_)
         favStr = ("FALSE");

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
    st.Bind(++i, r.name_);
    st.Bind(++i, r.acctType_);
    st.Bind(++i, r.accountNum_);
    st.Bind(++i, statusStr);
    st.Bind(++i, r.notes_);
    st.Bind(++i, r.heldAt_);
    st.Bind(++i, r.website_);
    st.Bind(++i, r.contactInfo_);
    st.Bind(++i, r.accessInfo_);
    st.Bind(++i, r.initialBalance_);
    st.Bind(++i, favStr);
    st.Bind(++i, currencyID);

    wxASSERT(st.GetParamCount() == i);
    st.ExecuteUpdate();

    pAccount->id_ = db_->GetLastRowId().ToLong();
    accounts_.push_back(pAccount);

    st.Finalize();
    mmOptions::instance().databaseUpdated_ = true;

    return pAccount->id_;
}

std::pair<mmAccountList::const_iterator, mmAccountList::const_iterator> 
mmAccountList::range() const
{
    return std::make_pair(accounts_.begin(), accounts_.end());
}
