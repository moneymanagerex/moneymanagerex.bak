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

#ifndef _MM_EX_MMACCOUNT_H_
#define _MM_EX_MMACCOUNT_H_

#include <vector>
#include <boost/shared_ptr.hpp>

#include "defs.h"
#include "dbwrapper.h"
#include "mmcurrency.h"
#include "mmtransaction.h"

class mmAccount
{
public: 
   mmAccount(boost::shared_ptr<wxSQLite3Database> db); 
   mmAccount(boost::shared_ptr<wxSQLite3Database> db, wxSQLite3ResultSet& q1);
   ~mmAccount() {}

   double balance();

   /* Scoped Enums */
   enum AccountStatus
   {
      MMEX_Open,
      MMEX_Closed
   };

public:
   int      id_;
   wxString name_;
   AccountStatus status_;
   wxString accountNum_;
   wxString notes_;
   wxString heldAt_;
   wxString website_;
   wxString contactInfo_;
   wxString accessInfo_;
   wxString acctType_;
   bool favoriteAcct_;
   double initialBalance_;
   boost::weak_ptr<mmCurrency> currency_;

   /* pointer to core */
   boost::shared_ptr<wxSQLite3Database> db_;
};

class mmAccountList
{
    typedef std::vector<boost::shared_ptr<mmAccount> > account_v;

public:
    mmAccountList(boost::shared_ptr<wxSQLite3Database> db);
    ~mmAccountList() {}

    /* Account Functions */
    boost::shared_ptr<mmAccount> getAccountSharedPtr(int accountID) const;
    
    int add(boost::shared_ptr<mmAccount> pAccount);
    /// returns the ACCOUNT_TYPE_xxxx String
    wxString getAccountType(int accountID) const;
    /// returns the status of the account (mmAccount::MMEX_Open or mmAccount::MMEX_Closed)\n
    /// to get a list of account names, use mmDBWrapper::getAccountsName(..)
    mmAccount::AccountStatus getAccountStatus(int accountID) const;
    int getID(const wxString& accountName) const;
    bool remove(int accountID);
    void update(boost::shared_ptr<mmAccount> pAccount);
    bool exists(const wxString& accountName) const;
    bool has_term_account() const;
    boost::weak_ptr<mmCurrency> getCurrencyWeakPtr(int accountID) const;
    double getAccountBaseCurrencyConvRate(int accountID) const;
    wxString getName(int accountID) const;
    wxString getAccountCurrencyDecimalChar(int accountID) const;
    wxString getAccountCurrencyGroupChar(int accountID) const;
    account_v accounts_;
    typedef std::vector<boost::shared_ptr<mmAccount> >::const_iterator const_iterator;
    std::pair<const_iterator, const_iterator> range() const; 
    
private:
    boost::shared_ptr<wxSQLite3Database> db_;
};

#endif
