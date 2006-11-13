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
#ifndef _MM_EX_MMTRANSACTION_H_
#define _MM_EX_MMTRANSACTION_H_

#include "boost/shared_ptr.hpp"
#include "dbwrapper.h"
#include "mmcategory.h"

class mmTransaction
{
public:
    mmTransaction() {}
    mmTransaction(int transactionID) : transactionID_(transactionID) {}
   virtual ~mmTransaction() {}

   virtual int transactionID() { return transactionID_; }
   virtual double value(int accountCtx) = 0;

protected:
    int transactionID_;
};

class mmBankTransaction : public mmTransaction
{
public: 
    mmBankTransaction(boost::shared_ptr<wxSQLite3Database> db, wxSQLite3ResultSet& q1);
    virtual ~mmBankTransaction() {}

    virtual double value(int accountCtx) = 0;

    wxDateTime date_;
    wxString dateStr_;

    boost::weak_ptr<mmCategory> category_;
    wxString catStr_;
    wxString subCatStr_;

    int payeeID_;
    wxString payeeStr_;

    wxString transNum_;
    wxString status_;
    wxString notes_;
    wxString transType_;
    
    wxString transAmtString_;
    double amt_;

    wxString transToAmtString_;
    double toAmt_;
    
    int accountID_;
    int toAccountID_;

    wxString fromAccountStr_;
    
    wxString withdrawalStr_;
    wxString depositStr_;

    double balance_;
    wxString balanceStr_;
};

class mmDepositTransaction : public mmBankTransaction
{
public: 
   mmDepositTransaction(boost::shared_ptr<wxSQLite3Database> db, 
       wxSQLite3ResultSet& q1) : mmBankTransaction(db,q1) {}
   virtual ~mmDepositTransaction() {}

   virtual double value(int accountCtx) { return amt_; }

};

class mmWithdrawalTransaction : public mmBankTransaction
{
public: 
   mmWithdrawalTransaction(boost::shared_ptr<wxSQLite3Database> db, 
       wxSQLite3ResultSet& q1) : mmBankTransaction(db, q1)  {}
   virtual ~mmWithdrawalTransaction() {}

   virtual double value(int accountCtx) { return -amt_; }
};

class mmTransferTransaction : public mmBankTransaction
{
public: 
   mmTransferTransaction(boost::shared_ptr<wxSQLite3Database> db, 
                 wxSQLite3ResultSet& q1) : mmBankTransaction(db, q1) {}
   virtual ~mmTransferTransaction() {}

   virtual double value(int accountCtx);
};


class mmAssetTransaction : public mmTransaction
{
public:
   mmAssetTransaction(boost::shared_ptr<wxSQLite3Database> db, 
                 wxSQLite3ResultSet& q1) : mmTransaction(q1.GetInt(wxT("TRANSID"))) {}
   virtual ~mmAssetTransaction() {}

   virtual double value(int accountCtx) { return 0.0; }
};


#endif