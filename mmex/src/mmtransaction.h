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
 ********************************************************/

#ifndef _MM_EX_MMTRANSACTION_H_
#define _MM_EX_MMTRANSACTION_H_

#include "dbwrapper.h"
#include "mmcategory.h"
#include "mmpayee.h"
#include "mmcurrency.h"
#include <map>

class mmCoreDB;

class mmTransaction
{
public:
   mmTransaction() {}
   mmTransaction(int transactionID) : transactionID_(transactionID) {}
   virtual ~mmTransaction() {}

   int transactionID() const { return transactionID_; }
   void transactionID(int transactionID) { transactionID_ = transactionID; }

   virtual double value(int accountCtx) const = 0;

protected:
   int transactionID_;
};

class mmSplitTransactionEntry
{
public:
    int splitEntryID_;
    int categID_;
    int subCategID_;
    wxString sFullCategoryName_;

    double splitAmount_;
};

class mmSplitTransactionEntries
{
public:
    mmSplitTransactionEntries() : total_() {}

    size_t numEntries() const { return entries_.size(); }
    double getTotalSplits() const { return total_; }
    double getUpdatedTotalSplits();

    void addSplit(std::shared_ptr<mmSplitTransactionEntry> split);
    void removeSplit(int splitID);
    void removeSplitByIndex(int splitIndex);

    void updateToDB(std::shared_ptr<wxSQLite3Database>& db, int transID, bool edit);
    void loadFromBDDB(mmCoreDB* core, int bdID);

    std::vector<std::shared_ptr<mmSplitTransactionEntry> > entries_;
    double total_;
};

class mmBankTransaction : public mmTransaction
{
public:
    mmBankTransaction(mmCoreDB* core, wxSQLite3ResultSet& q1);
    mmBankTransaction(std::shared_ptr<wxSQLite3Database> db);
    virtual ~mmBankTransaction() {}
    bool operator < (const mmBankTransaction& tran) const;
    bool containsCategory(int categID, int subcategID, bool ignoreSubCateg = false) const;
    double getAmountForSplit(int categID, int subcategID) const;

    double value(int accountID) const;
    void updateAllData(mmCoreDB* core,
        int accountID,
        std::shared_ptr<mmCurrency> currencyPtr,
        bool forceUpdate=false);

    void getSplitTransactions(mmSplitTransactionEntries* splits) const;
    std::shared_ptr<wxSQLite3Database> db_;

    /* Core Data */
    wxDateTime date_;

    std::shared_ptr<mmCategory> category_;
    std::shared_ptr<mmPayee> payee_;

    wxString transNum_;
    wxString status_;
    wxString notes_;
    wxString transType_;

    double amt_;
    double toAmt_;

    int accountID_;
    int toAccountID_;

    std::shared_ptr<mmSplitTransactionEntries> splitEntries_;

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
    wxString fullCatStr_;

    double balance_;
    wxString balanceStr_;
    int payeeID_;
    int categID_;
    int subcategID_;

    bool updateRequired_;
    int followupID_;

    // used for transaction reports
    double reportCategAmount_;
    wxString reportCategAmountStr_;

private:
    bool isInited_;
};

class mmBankTransactionList
{
public:
    mmBankTransactionList(mmCoreDB* core);
    ~mmBankTransactionList() {}

    std::shared_ptr<mmBankTransaction> getBankTransactionPtr(int accountID, int transactionID) const;
    std::shared_ptr<mmBankTransaction> getBankTransactionPtr(int transactionID) const;
    int addTransaction(std::shared_ptr<mmBankTransaction> pTransaction);
    bool checkForExistingTransaction(std::shared_ptr<mmBankTransaction> pTransaction);
    std::shared_ptr<mmBankTransaction> copyTransaction(/*mmCoreDB* pCore,*/
       const long transactionID, const long accountID, const bool useOriginalDate);

    /// Loads database primary Transactions into memory.
    void LoadTransactions(mmCoreDB* core);

    /* Update Transactions */
    void UpdateTransaction(std::shared_ptr<mmBankTransaction> pTransaction);
    void UpdateAllTransactions();
    void UpdateAllTransactionsForCategory(int categID, int subCategID);
    int UpdateAllTransactionsForPayee(int payeeID);

    bool removeTransaction(int accountID, int transactionID);
    bool deleteTransaction(int accountID, int transactionID);
    void deleteTransactions(int accountID);
    int RelocatePayee(mmCoreDB* core, const int destPayeeID, const int sourcePayeeID, int& changedPayees_);
    int RelocateCategory(mmCoreDB* core,
        const int destCatID, const int destSubCatID, const int sourceCatID, const int sourceSubCatID,
        int& changedCats, int& changedSubCats);
    void ChangeDateFormat();

    /* Query Functions */
    void getExpensesIncome(const mmCoreDB* core, int accountID, double& expenses, double& income, bool ignoreDate, const wxDateTime &dtBegin, const wxDateTime &dtEnd, bool ignoreFuture = false) const;
    // The setting asDeposit is only valid if evaluateTransfer is true
    double getAmountForCategory(int categID, int subcategID, bool ignoreDate, const wxDateTime &dtBegin, const wxDateTime &dtEnd, bool evaluateTransfer = false, bool asDeposit = false, bool ignoreFuture = false) const;
    double getAmountForPayee(int payeeID, bool ignoreDate, const wxDateTime &dtbegin, const wxDateTime &dtEnd, bool ignoreFuture = false) const;
    void getTransactionStats(std::map<int, std::map<int, int> > &stats, int start_year) const;
    wxDateTime getLastDate(int accountID) const;

    double getBalance(int accountID, bool ignoreFuture = false) const;
    bool getDailyBalance(const mmCoreDB* core, int accountID, std::map<wxDateTime, double>& daily_balance, bool ignoreFuture = false) const;
    double getReconciledBalance(int accountID, bool ignoreFuture = false) const;
    int countFollowupTransactions() const;
    int getLastUsedCategoryID(const int accountID, const int payeeID, const wxString sType, int& subcategID) const;
    int getLastUsedPayeeID(const int accountID, wxString sType, int& categID, int& subcategID) const;
    wxArrayString getTransactionNumber(const int accountID, const wxDateTime transaction_date) const;
    bool IsCategoryUsed(const int iCatID, const int iSubCatID, bool& bIncome, bool bIgnor_subcat = true) const;
    bool IsPayeeUsed(const int iPayeeID) const;

    /* Data */
    typedef std::vector< std::shared_ptr<mmBankTransaction> >::const_iterator const_iterator;
    std::vector< std::shared_ptr<mmBankTransaction> > transactions_;

private:
    mmCoreDB* core_;
};

#endif
