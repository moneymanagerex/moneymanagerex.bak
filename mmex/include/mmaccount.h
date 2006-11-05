/*******************************************************
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

class mmAccount
{
public: 
   mmAccount(wxSQLite3ResultSet& q1);
   virtual ~mmAccount() {}

   enum AccountStatus
   {
      MMEX_Open,
      MMEX_Closed
   };


private:
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
};

class mmCheckingAccount : public mmAccount
{
public: 
   mmCheckingAccount(wxSQLite3ResultSet& q1) : mmAccount(q1) {} 
   virtual ~mmCheckingAccount() {}
};

class mmAssetAccount : public mmAccount
{
public: 
   mmAssetAccount(wxSQLite3ResultSet& q1) : mmAccount(q1) {}
   virtual ~mmAssetAccount() {}
};

class mmInvestmentAccount : public mmAccount
{
public: 
   mmInvestmentAccount(wxSQLite3ResultSet& q1) : mmAccount(q1) {}
   virtual ~mmInvestmentAccount() {}
};

#endif