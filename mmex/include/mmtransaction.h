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
#include "mmpayee.h"
#include "mmcurrency.h"

class mmCoreDB;

class mmTransaction
{
public:
    mmTransaction() {}
    mmTransaction(int transactionID) : transactionID_(transactionID) {}
   virtual ~mmTransaction() {}

   int transactionID() { return transactionID_; }
   void transactionID(int transactionID) { transactionID_ = transactionID; }
   virtual double value(int accountCtx) = 0;

protected:
    int transactionID_;
};

class mmBankTransaction : public mmTransaction
{
public: 
    mmBankTransaction(mmCoreDB* core, 
       wxSQLite3ResultSet& q1);
    mmBankTransaction(boost::shared_ptr<wxSQLite3Database> db);
    virtual ~mmBankTransaction() {}

    double value(int accountID);
    void updateAllData(mmCoreDB* core, int accountID, boost::shared_ptr<mmCurrency> currencyPtr, 
        bool forceUpdate=false);

    boost::shared_ptr<wxSQLite3Database> db_;

    /* Core Data */
    wxDateTime date_;
    
    boost::weak_ptr<mmCategory> category_;
    boost::weak_ptr<mmPayee> payee_;

    wxString transNum_;
    wxString status_;
    wxString notes_;
    wxString transType_;
    
    double amt_;
    double toAmt_;
    
    int accountID_;
    int toAccountID_;

    /* Derived Data */
    wxString dateStr_;
    wxString catStr_;
    wxString subCatStr_;
    wxString payeeStr_;
    wxString transAmtString_;
    wxString transToAmtString_;
    wxString fromAccountStr_;
    wxString withdrawalStr_;
    wxString depositStr_;
    double balance_;
    wxString balanceStr_;
    int payeeID_;
    int categID_;
    int subcategID_;

private:
    bool isInited_;
};

class mmBankTransactionList
{
public:
   mmBankTransactionList(boost::shared_ptr<wxSQLite3Database> db) { db_ = db; }
    ~mmBankTransactionList() {}

    boost::shared_ptr<mmBankTransaction> getBankTransactionPtr(int accountID, int transactionID);
    int addTransaction(boost::shared_ptr<mmBankTransaction> pTransaction);
    void updateTransaction(boost::shared_ptr<mmBankTransaction> pTransaction);
    void deleteTransaction(int accountID, int transactionID);
    void deleteTransactions(int accountID);

    void getExpensesIncome(int accountID, double& expenses, double& income,  
                           bool ignoreDate, wxDateTime dtBegin, wxDateTime dtEnd);

    double getBalance(int accountID, bool ignoreFuture = false);
    double getReconciledBalance(int accountID);
    int countFollowupTransactions();

    /* Data */
    std::vector< boost::shared_ptr<mmBankTransaction> > transactions_;
    
private:
    boost::shared_ptr<wxSQLite3Database> db_;
};

#endif