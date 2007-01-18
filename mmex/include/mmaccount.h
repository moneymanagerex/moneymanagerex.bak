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
#ifndef _MM_EX_MMACCOUNT_H_
#define _MM_EX_MMACCOUNT_H_

#include <vector>
#include <boost/shared_ptr.hpp>

#include "defs.h"
#include "dbwrapper.h"
#include "mmcurrency.h"
#include "mmtransaction.h"

class mmCoreDB;

class mmAccount
{
public: 
   mmAccount(boost::shared_ptr<wxSQLite3Database> db); 

   mmAccount(boost::shared_ptr<wxSQLite3Database> db, 
       wxSQLite3ResultSet& q1);
   virtual ~mmAccount() {}

    virtual double balance() = 0;

   /* Scoped Enums */
   enum AccountStatus
   {
      MMEX_Open,
      MMEX_Closed
   };

public:
   int      accountID_;
   wxString accountName_;
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

class mmCheckingAccount : public mmAccount
{
public: 
    mmCheckingAccount(boost::shared_ptr<wxSQLite3Database> db, 
        wxSQLite3ResultSet& q1);
    mmCheckingAccount(boost::shared_ptr<wxSQLite3Database> db)  : mmAccount(db) { }
    
    virtual ~mmCheckingAccount() {}

    
public:
    double balance();
};

class mmInvestmentAccount : public mmAccount
{
public: 
   mmInvestmentAccount(boost::shared_ptr<wxSQLite3Database> db, wxSQLite3ResultSet& q1)
       : mmAccount(db, q1) {}
   mmInvestmentAccount(boost::shared_ptr<wxSQLite3Database> db)  : mmAccount(db) { }
   virtual ~mmInvestmentAccount() {}

   double balance();
};


class mmAccountList
{
public:
    mmAccountList(boost::shared_ptr<wxSQLite3Database> db);
    ~mmAccountList() {}

    /* Account Functions */
    boost::shared_ptr<mmAccount> getAccountSharedPtr(int accountID);
    
    int addAccount(boost::shared_ptr<mmAccount> pAccount);
    wxString getAccountType(int accountID);
    int getAccountID(const wxString& accountName);
    bool deleteAccount(int accountID);
    void updateAccount(boost::shared_ptr<mmAccount> pAccount);
    bool accountExists(const wxString& accountName);
    boost::weak_ptr<mmCurrency> getCurrencyWeakPtr(int accountID);
    double getAccountBaseCurrencyConvRate(int accountID);

    std::vector< boost::shared_ptr<mmAccount> > accounts_;
    
    
private:
    boost::shared_ptr<wxSQLite3Database> db_;
};

#endif
