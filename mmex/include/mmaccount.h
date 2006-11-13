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
#include "mmdbinterface.h"
#include "mmtransaction.h"

class mmAccount : public mmDBInterface
{
public: 
   mmAccount(boost::shared_ptr<wxSQLite3Database> db, 
       wxSQLite3ResultSet& q1);
   virtual ~mmAccount() {}

    virtual double balance() = 0;

   /* Overrides of mmDBInterface */
   virtual void addDBRecord();

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
   boost::shared_ptr< mmCurrency*> currency_;

   /* List of associated transactions */
   std::vector<boost::weak_ptr<mmTransaction> > transactions_;

   
};

class mmCheckingAccount : public mmAccount
{
public: 
    mmCheckingAccount(boost::shared_ptr<wxSQLite3Database> db, 
        wxSQLite3ResultSet& q1);
    virtual ~mmCheckingAccount() {}

public:
    /* List of global transactions */
    static std::vector<boost::shared_ptr<mmTransaction> > gTransactions_;
    static boost::shared_ptr<mmTransaction> findTransaction(int transactionID);

    double balance();
};

class mmAssetAccount : public mmAccount
{
public: 
   mmAssetAccount(boost::shared_ptr<wxSQLite3Database> db, wxSQLite3ResultSet& q1) 
       : mmAccount(db, q1) {}
   virtual ~mmAssetAccount() {}

   double balance();
};

class mmInvestmentAccount : public mmAccount
{
public: 
   mmInvestmentAccount(boost::shared_ptr<wxSQLite3Database> db, wxSQLite3ResultSet& q1)
       : mmAccount(db, q1) {}
   virtual ~mmInvestmentAccount() {}

   double balance();
};

#endif