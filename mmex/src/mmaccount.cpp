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
#include "util.h"

mmAccount::mmAccount(wxSQLite3ResultSet& q1)
{
    id_ = q1.GetInt(wxT("ACCOUNTID"));
    name_ = q1.GetString(wxT("ACCOUNTNAME"));
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
    currencyID_ = q1.GetDouble(wxT("CURRENCYID"));
}

mmAccountList::mmAccountList(boost::shared_ptr<wxSQLite3Database> db)
: db_(db)
{}

double mmAccountList::getAccountBaseCurrencyConvRate(int accountID) const
{
   boost::weak_ptr<mmCurrency> wpCurrency = getCurrencyWeakPtr(accountID);
   boost::shared_ptr<mmCurrency> pCurrency = wpCurrency.lock();
   wxASSERT(pCurrency);

   if (pCurrency)
      return pCurrency->baseConv_;

   return 0.0;
}

boost::shared_ptr<mmAccount> mmAccountList::GetAccountSharedPtr(int accountID) const
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
    
bool mmAccountList::AccountExists(const wxString& accountName) const
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

int mmAccountList::GetAccountId(const wxString& accountName) const
{
    for (const_iterator it = accounts_.begin(); it != accounts_.end(); ++ it)
    {
        if ((*it)->name_ == accountName) return (*it)->id_;
    }

   return -1;
}

wxString mmAccountList::GetAccountName(int accountID) const
{
    for (const_iterator it = accounts_.begin(); it != accounts_.end(); ++ it)
    {
        if ((*it)->id_ == accountID) return (*it)->name_;
    }

    return wxEmptyString;
}

wxArrayInt mmAccountList::getAccountsID(const wxArrayString accounts_type, const int except_id) const
{
    wxArrayInt accounts_id;
    for (const_iterator it = accounts_.begin(); it != accounts_.end(); ++ it)
    {
        const mmAccount* account = it->get();
        for (size_t i = 0; i < accounts_type.Count(); ++i)
        {
            if ((account->acctType_ == accounts_type[i]) 
                && account->status_ != mmAccount::MMEX_Closed && account->id_ != except_id)
            {
                accounts_id.Add(account->id_);
            }
        }
    }

    return accounts_id;
}
//TODO: Deprecated function. Use getAccountsID
wxArrayString mmAccountList::getAccountsName(const int except_id) const
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
    return wxT("");
}

bool mmAccountList::getAccountFavorite(int accountID) const
{
    for (account_v::const_iterator it = accounts_.begin(); it != accounts_.end(); ++ it)
    {
        if ((*it)->id_ == accountID) return (*it)->favoriteAcct_;
    }

    wxASSERT(false);
    return false;
}

mmAccount::Status mmAccountList::getAccountStatus(int accountID) const
{
    for (account_v::const_iterator it = accounts_.begin(); it != accounts_.end(); ++ it)
    {
        if ((*it)->id_ == accountID) return (*it)->status_;
    }

    wxASSERT(false);
    return mmAccount::MMEX_Open;
}

bool mmAccountList::currencyInUse(int currencyID) const
{
    for (account_v::const_iterator it = accounts_.begin(); it != accounts_.end(); ++ it)
    {
        if ((*it)->currencyID_ == currencyID) return true;
    }

    return false;
}

wxString mmAccountList::getAccountCurrencyDecimalChar(int accountID) const
{
    boost::weak_ptr<mmCurrency> wpCurrency = getCurrencyWeakPtr(accountID);
    boost::shared_ptr<mmCurrency> pCurrency = wpCurrency.lock();
    wxASSERT(pCurrency);

    if (pCurrency)
        return pCurrency->decChar_;

    return wxT(".");
}

wxString mmAccountList::getAccountCurrencyGroupChar(int accountID) const
{
    boost::weak_ptr<mmCurrency> wpCurrency = getCurrencyWeakPtr(accountID);
    boost::shared_ptr<mmCurrency> pCurrency = wpCurrency.lock();
    wxASSERT(pCurrency);

    if (pCurrency)
        return pCurrency->grpChar_;

    return wxT(",");
}

wxString mmAccountList::GetAccountCurrencyName(int accountID) const
{
    boost::weak_ptr<mmCurrency> wpCurrency = getCurrencyWeakPtr(accountID);
    boost::shared_ptr<mmCurrency> pCurrency = wpCurrency.lock();
    wxASSERT(pCurrency);

    if (pCurrency)
        return pCurrency->currencyName_;

    return wxT("");
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

std::pair<mmAccountList::const_iterator, mmAccountList::const_iterator> 
mmAccountList::range() const
{
    return std::make_pair(accounts_.begin(), accounts_.end());
}

void mmAccountList::UpdateAccount(boost::shared_ptr<mmAccount> pAccount)
{
   wxString statusStr = wxT("Open");
   if (pAccount->status_ == mmAccount::MMEX_Closed)
      statusStr = wxT("Closed");

   wxString favStr = wxT("TRUE");
   if (!pAccount->favoriteAcct_)
      favStr = wxT("FALSE");

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

int mmAccountList::AddAccount(boost::shared_ptr<mmAccount> pAccount)
{
    wxString statusStr = wxT("Open");
    if (pAccount->status_ == mmAccount::MMEX_Closed)
        statusStr = wxT("Closed");

    wxString favStr = wxT("TRUE");
    if (!pAccount->favoriteAcct_)
        favStr = wxT("FALSE");

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

bool mmAccountList::RemoveAccount(int accountID)
{
    wxString acctType = getAccountType(accountID);
    db_.get()->Begin();
    wxSQLite3Statement st;

    if (acctType != ACCOUNT_TYPE_STOCK)
    {
        mmDBWrapper::removeSplitsForAccount(db_.get(), accountID);

        st = db_->PrepareStatement("DELETE FROM CHECKINGACCOUNT_V1 WHERE ACCOUNTID = ? OR TOACCOUNTID = ?");
        st.Bind(1, accountID);
        st.Bind(2, accountID);
        st.ExecuteUpdate();
        st.Finalize();

        // --
        st = db_->PrepareStatement("DELETE FROM BILLSDEPOSITS_V1 WHERE ACCOUNTID=? OR TOACCOUNTID = ?");
        st.Bind(1, accountID);
        st.Bind(2, accountID);
        st.ExecuteUpdate();
        st.Finalize();
    }
    else
    {
        st = db_->PrepareStatement("DELETE FROM STOCK_V1 WHERE HELDAT = ?");
        st.Bind(1, accountID);
        st.ExecuteUpdate();
        st.Finalize();
    }

    st = db_->PrepareStatement("DELETE FROM INFOTABLE_V1 WHERE INFONAME = ?");
    st.Bind(1, wxString::Format(wxT("ACC_IMAGE_ID_%d"), accountID));
    st.ExecuteUpdate();
    st.Finalize();

    st = db_->PrepareStatement("DELETE FROM ACCOUNTLIST_V1 WHERE ACCOUNTID = ?");
    st.Bind(1, accountID);

    st.ExecuteUpdate();
    st.Finalize();

    db_.get()->Commit();

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

void mmAccountList::LoadAccounts(const mmCurrencyList& currencyList)
{
    wxSQLite3ResultSet q1 = db_->ExecuteQuery(SELECT_ALL_FROM_ACCOUNTLIST_V1);

    while (q1.NextRow())
    {
        boost::shared_ptr<mmAccount> pAccount(new mmAccount(q1));

        boost::weak_ptr<mmCurrency> pCurrency =
            currencyList.getCurrencySharedPtr(q1.GetInt(wxT("CURRENCYID")));
        pAccount->currency_ = pCurrency;

        accounts_.push_back(pAccount);
    }

    q1.Finalize();
}
//----------------------------------------------------------------------------
