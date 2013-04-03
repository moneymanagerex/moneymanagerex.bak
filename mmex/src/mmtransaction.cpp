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

#include "constants.h"
#include "mmtransaction.h"
#include "util.h"
#include "mmcoredb.h"

void mmSplitTransactionEntries::addSplit(boost::shared_ptr<mmSplitTransactionEntry> split)
{
    total_ += split->splitAmount_;
    entries_.push_back(split);
}

double mmSplitTransactionEntries::getUpdatedTotalSplits()
{
    total_ = 0.0;
    for (size_t i = 0; i < entries_.size(); ++i)
    {
        total_ += entries_[i]->splitAmount_;
    }

    return total_;
}

void mmSplitTransactionEntries::removeSplit(int splitID)
{
    for (size_t i = 0; i < entries_.size(); ++i)
    {
        if (entries_[i]->splitEntryID_ == splitID)
        {
            entries_.erase(entries_.begin() + i);
            break;
        }
    }
}

void mmSplitTransactionEntries::removeSplitByIndex(int splitIndex)
{
    total_ -= entries_[splitIndex]->splitAmount_;
    entries_.erase(entries_.begin() + splitIndex);
}

void mmSplitTransactionEntries::updateToDB(boost::shared_ptr<wxSQLite3Database>& db,
                                           int transID,
                                           bool edit)
{
    if (edit)
    {
        wxSQLite3Statement st = db->PrepareStatement(DELETE_TRANSID_SPLITTRANSACTIONS_V1);
        st.Bind(1, transID);
        st.ExecuteUpdate();
        st.Finalize();
    }

    wxSQLite3Statement st = db->PrepareStatement(INSERT_INTO_SPLITTRANSACTIONS_V1);

    for (size_t i = 0; i < entries_.size(); ++i)
    {
        mmSplitTransactionEntry &r = *entries_[i];

        st.Bind(1, transID);
        st.Bind(2, r.categID_);
        st.Bind(3, r.subCategID_);
        st.Bind(4, r.splitAmount_);

        st.ExecuteUpdate();
        r.splitEntryID_ = db->GetLastRowId().ToLong();

        st.Reset();
    }

    st.Finalize();
}

void mmSplitTransactionEntries::loadFromBDDB(mmCoreDB* core, int bdID)
{
    entries_.clear();
    total_ = 0.0;

    wxSQLite3Statement st = core->db_->PrepareStatement(SELECT_ROW_FROM_BUDGETSPLITTRANSACTIONS_V1);
   st.Bind(1, bdID);

   wxSQLite3ResultSet q1 = st.ExecuteQuery();
   while (q1.NextRow())
   {
      boost::shared_ptr<mmSplitTransactionEntry> pSplitEntry(new mmSplitTransactionEntry());
      pSplitEntry->splitEntryID_ = q1.GetInt(wxT("SPLITTRANSID"));
      pSplitEntry->splitAmount_  = q1.GetDouble(wxT("SPLITTRANSAMOUNT"));

      int catID = q1.GetInt(wxT("CATEGID"));
      int subID = q1.GetInt(wxT("SUBCATEGID"));

      pSplitEntry->categID_ = catID;
      pSplitEntry->subCategID_ = subID;

      addSplit(pSplitEntry);
   }

    st.Finalize();
}
//-----------------------------------------------------------------------------//
mmBankTransaction::mmBankTransaction(boost::shared_ptr<wxSQLite3Database> db) :
    mmTransaction(-1),
    db_(db),
    isInited_(false),
    updateRequired_(false)
{
    splitEntries_ = boost::shared_ptr<mmSplitTransactionEntries>(new mmSplitTransactionEntries());
}

mmBankTransaction::mmBankTransaction(mmCoreDB* core, wxSQLite3ResultSet& q1)
: mmTransaction(q1.GetInt(wxT("TRANSID"))),
                db_(core->db_),
                isInited_(false),
                updateRequired_(false)
{
    date_        = mmGetStorageStringAsDate(q1.GetString(wxT("TRANSDATE")));
    transNum_    = q1.GetString(wxT("TRANSACTIONNUMBER"));
    status_      = q1.GetString(wxT("STATUS"));
    notes_       = q1.GetString(wxT("NOTES"));
    transType_   = q1.GetString(wxT("TRANSCODE"));
    accountID_   = q1.GetInt(wxT("ACCOUNTID"));
    toAccountID_ = q1.GetInt(wxT("TOACCOUNTID"));
    payee_       = core->payeeList_.GetPayeeSharedPtr(q1.GetInt(wxT("PAYEEID")));
    amt_         = q1.GetDouble(wxT("TRANSAMOUNT"));
    toAmt_       = q1.GetDouble(wxT("TOTRANSAMOUNT"));
    followupID_  = q1.GetInt(wxT("FOLLOWUPID"));
    category_    = core->categoryList_.GetCategorySharedPtr(q1.GetInt(wxT("CATEGID")), q1.GetInt(wxT("SUBCATEGID")));

    boost::shared_ptr<mmCurrency> pCurrencyPtr = core->accountList_.getCurrencyWeakPtr(accountID_).lock();
    wxASSERT(pCurrencyPtr);

    splitEntries_ = boost::shared_ptr<mmSplitTransactionEntries>(new mmSplitTransactionEntries());
    getSplitTransactions(splitEntries_.get());

    updateAllData(core, accountID_, pCurrencyPtr);
}

void mmBankTransaction::updateAllData(mmCoreDB* core,
                                      int accountID,
                                      boost::shared_ptr<mmCurrency> currencyPtr,
                                      bool forceUpdate
                                      )
{
    if ((isInited_) && (transType_ != TRANS_TYPE_TRANSFER_STR) && !forceUpdate) return;

    /* Load the Account Currency Settings for Formatting Strings */
    currencyPtr->loadCurrencySettings();

    dateStr_ = mmGetDateForDisplay(date_);

    wxString displayTransAmtString;
    mmex::formatDoubleToCurrencyEdit(amt_, displayTransAmtString);
    transAmtString_ = displayTransAmtString;

    wxASSERT(toAmt_ >= 0);
    wxASSERT(amt_ >= 0);
    if (toAmt_ < 0) toAmt_ = amt_;
    wxString displayToTransAmtString;
    mmex::formatDoubleToCurrencyEdit(toAmt_, displayToTransAmtString);
    transToAmtString_ = displayToTransAmtString;

    if (transType_ != TRANS_TYPE_TRANSFER_STR)
    {
        // needed to correct possible crash if database becomes corrupt.
        if (payee_.lock() == 0 )
        {
            if (core->displayDatabaseError_)
            {
                wxString errMsg = _("Payee not found in database for Account: ");
                errMsg << core->accountList_.GetAccountName(accountID_)
                    << wxT("\n\n")
                    << _("Subsequent errors not displayed.");
                wxMessageBox(errMsg,_("MMEX DATABASE ERROR"), wxOK|wxICON_ERROR);
                core->displayDatabaseError_ = false;
            }
            payeeID_  = -1;
            payeeStr_ = wxT("Payee Error");
            status_ = wxT("V");
        }
        else
        {
            boost::shared_ptr<mmPayee> pPayee = payee_.lock();
            wxASSERT(pPayee);
            payeeStr_ = pPayee->name_;
            payeeID_ = pPayee->id_;
        }
    }

    depositStr_ = wxT("");
    withdrawalStr_ = wxT("");
    if (transType_ == TRANS_TYPE_DEPOSIT_STR)
    {
        depositStr_ = displayTransAmtString;
    }
    else if (transType_== TRANS_TYPE_WITHDRAWAL_STR)
    {
        withdrawalStr_ = displayTransAmtString;
    }
    else if (transType_ == TRANS_TYPE_TRANSFER_STR)
    {
        wxString fromAccount = core->accountList_.GetAccountName(accountID_);
        wxString toAccount = core->accountList_.GetAccountName(toAccountID_);

        if (accountID_ == accountID)
        {
         withdrawalStr_ = displayTransAmtString;
         payeeStr_      = toAccount;
        }
        else if (toAccountID_ == accountID)
        {
         depositStr_ = displayToTransAmtString;
         payeeStr_   = fromAccount;
        }
    }

    fromAccountStr_ = core->accountList_.GetAccountName(accountID_);

    boost::shared_ptr<mmCategory> pCategory = category_.lock();
    if (!pCategory && !splitEntries_->numEntries())
    {
        // If category is missing, we mark is as unknown
        int categID = core->categoryList_.GetCategoryId(wxT("Unknown"));
        if (categID == -1) categID = core->categoryList_.AddCategory(wxT("Unknown"));

        category_ = core->categoryList_.GetCategorySharedPtr(categID, -1);
        pCategory = category_.lock();
        wxASSERT(pCategory);
        updateRequired_ = true;
    }

    if (pCategory)
    {
        boost::shared_ptr<mmCategory> parent = pCategory->parent_.lock();
        if (parent)
        {
            catStr_ = parent->categName_;
            subCatStr_ = pCategory->categName_;
            categID_ = parent->categID_;
            subcategID_ = pCategory->categID_;
            fullCatStr_ = core->categoryList_.GetFullCategoryString(categID_, subcategID_);
        }
        else
        {
            catStr_ = pCategory->categName_;
            subCatStr_ = wxT("");
            categID_ = pCategory->categID_;
            subcategID_ = -1;
            fullCatStr_ = catStr_;
        }
    }
    else if (splitEntries_->numEntries() == 1)
    {
        categID_ = -1;
        subcategID_ = -1;
        catStr_= wxT("");
        subCatStr_ = wxT("");
        fullCatStr_= core->categoryList_.GetFullCategoryString(
            splitEntries_->entries_[0]->categID_, splitEntries_->entries_[0]->subCategID_);
    }
    else if (splitEntries_->numEntries() > 1)
    {
        fullCatStr_ = _("Split Category");
        categID_ = -1;
        subcategID_ = -1;
        catStr_= wxT("");
        subCatStr_ = wxT("");
    }

   isInited_ = true;
}

double mmBankTransaction::value(int accountID) const
{
    double balance = 0.0;
    if (transType_ == TRANS_TYPE_DEPOSIT_STR)
       balance = amt_;
    else if (transType_== TRANS_TYPE_WITHDRAWAL_STR)
       balance -= amt_;
    else if (transType_ == TRANS_TYPE_TRANSFER_STR)
    {
      //Bug fix for brocken transactions (as result of wrong import) if account and to account the same
        if (accountID_ != toAccountID_)
        {
            if (accountID_ == accountID)
                 balance -= amt_;
            else if (toAccountID_ == accountID)
                 balance += toAmt_;
        }
    }

    return balance;
}
void mmBankTransaction::getSplitTransactions(mmSplitTransactionEntries* splits) const
{
    splits->entries_.clear();
    splits->total_ = 0.0;

    wxSQLite3Statement st = db_->PrepareStatement(SELECT_ROW_FROM_SPLITTRANSACTIONS_V1);
    st.Bind(1, transactionID());

    wxSQLite3ResultSet q1 = st.ExecuteQuery();

    while (q1.NextRow())
    {
        boost::shared_ptr<mmSplitTransactionEntry> pSplitEntry(new mmSplitTransactionEntry);

        pSplitEntry->splitEntryID_ = q1.GetInt(wxT("SPLITTRANSID"));
        pSplitEntry->splitAmount_  = q1.GetDouble(wxT("SPLITTRANSAMOUNT"));

        int catID = q1.GetInt(wxT("CATEGID"));
        int subID = q1.GetInt(wxT("SUBCATEGID"));

        pSplitEntry->categID_ = catID;
        pSplitEntry->subCategID_ = subID;


        splits->addSplit(pSplitEntry);
    }

    st.Finalize();
}

bool mmBankTransaction::containsCategory(int categID, int subcategID, bool ignoreSubCateg) const
{
    if (splitEntries_->numEntries())
    {
        for(size_t idx = 0; idx < splitEntries_->numEntries(); ++idx)
        {
            if ((splitEntries_->entries_[idx]->categID_ == categID) &&
                ((ignoreSubCateg) || splitEntries_->entries_[idx]->subCategID_ == subcategID))
            {
                return true;
            }
        }
    }
    else if (categID_ == categID)
        return (subcategID_ == subcategID || ignoreSubCateg);

    return false;
}

double mmBankTransaction::getAmountForSplit(int categID, int subcategID) const
{
    double splitAmount = 0.0;
    if (splitEntries_->numEntries())
    {
        for(size_t idx = 0; idx < splitEntries_->numEntries(); ++idx)
        {
            if ((splitEntries_->entries_[idx]->categID_ == categID) &&
                (splitEntries_->entries_[idx]->subCategID_ == subcategID))
            {
                splitAmount += splitEntries_->entries_[idx]->splitAmount_;
            }
        }

        return splitAmount;
    }
    else if ((categID_ == categID) &&
            (subcategID_ == subcategID))
    {
        return amt_;
    }

    return splitAmount;
}

//-----------------------------------------------------------------------------//
mmBankTransactionList::mmBankTransactionList(mmCoreDB* core)
: core_(core)
{
   /* Allocate some empty space so loading transactions is faster */
   transactions_.reserve(5000);
}

int mmBankTransactionList::addTransaction(boost::shared_ptr<mmBankTransaction> pBankTransaction)
{
    if (checkForExistingTransaction(pBankTransaction))
    {
       pBankTransaction->status_ = wxT("D");
    }

    if(core_->payeeList_.PayeeExists(pBankTransaction->payeeID_) == false)
    {
       pBankTransaction->payeeID_ = -1;
    }

    wxSQLite3Statement st = core_->db_.get()->PrepareStatement(INSERT_INTO_CHECKINGACCOUNT_V1);
    mmBankTransaction &r = *pBankTransaction;

    int i = 0;
    st.Bind(++i, r.accountID_);
    st.Bind(++i, r.toAccountID_);
    st.Bind(++i, r.payeeID_);
    st.Bind(++i, r.transType_);
    st.Bind(++i, r.amt_);
    st.Bind(++i, r.status_);
    st.Bind(++i, r.transNum_);
    st.Bind(++i, r.notes_);
    st.Bind(++i, r.categID_);
    st.Bind(++i, r.subcategID_);
    st.Bind(++i, r.date_.FormatISODate());
    st.Bind(++i, r.followupID_);
    st.Bind(++i, r.toAmt_);

    wxASSERT(st.GetParamCount() == i);
    st.ExecuteUpdate();

    r.transactionID(core_->db_.get()->GetLastRowId().ToLong());
    st.Finalize();
    mmOptions::instance().databaseUpdated_ = true;

    r.splitEntries_->updateToDB(core_->db_, r.transactionID(), false);
    transactions_.push_back(pBankTransaction);

    return pBankTransaction->transactionID();
}

bool mmBankTransactionList::checkForExistingTransaction(boost::shared_ptr<mmBankTransaction> pBankTransaction)
{
    bool found = false;

    wxSQLite3Statement st = core_->db_.get()->PrepareStatement(IS_TRX_IN_CHECKINGACCOUNT_V1);
    const mmBankTransaction &r = *pBankTransaction;

    int i = 0;
    st.Bind(++i, r.accountID_);
    st.Bind(++i, (r.transType_ == TRANS_TYPE_TRANSFER_STR ? r.toAccountID_ : -1));
    st.Bind(++i, (r.transType_ == TRANS_TYPE_TRANSFER_STR ? -1 : r.payeeID_));
    st.Bind(++i, r.transType_);
    st.Bind(++i, r.amt_);
    st.Bind(++i, r.transNum_);
    st.Bind(++i, r.notes_);
    st.Bind(++i, r.categID_);
    st.Bind(++i, r.subcategID_);
    st.Bind(++i, r.date_.FormatISODate());
    st.Bind(++i, r.toAmt_ );

    wxASSERT(st.GetParamCount() == i);

    wxSQLite3ResultSet q1 = st.ExecuteQuery();
    while (q1.NextRow() && !found)
    {
        int transactionID = q1.GetInt(wxT("TRANSID"));
        if (pBankTransaction->categID_ == -1)
        {
            mmSplitTransactionEntries* splits = pBankTransaction->splitEntries_.get();

            boost::shared_ptr<mmBankTransaction> pTempTransaction = getBankTransactionPtr(transactionID);
            mmSplitTransactionEntries* temp_splits = pTempTransaction->splitEntries_.get();

            if (splits->entries_.size() != temp_splits->entries_.size())
                continue;

            for (int i = 0; i < (int)splits->entries_.size(); ++i)
            {
                if (splits->entries_[i]->splitAmount_ != temp_splits->entries_[i]->splitAmount_)
                    continue;
                if (splits->entries_[i]->categID_ != temp_splits->entries_[i]->categID_)
                    continue;
                if (splits->entries_[i]->subCategID_ != temp_splits->entries_[i]->subCategID_)
                    continue;
            }
        }
    found = true;
    }
    st.Finalize();

    return found;
}

boost::shared_ptr<mmBankTransaction> mmBankTransactionList::copyTransaction(
   const long transactionID, const long accountID, const bool useOriginalDate)
{
    boost::shared_ptr<mmBankTransaction> pBankTransaction = getBankTransactionPtr(transactionID);
    if (!pBankTransaction)
       return boost::shared_ptr<mmBankTransaction>();

    boost::shared_ptr<mmBankTransaction> pCopyTransaction(new mmBankTransaction(core_->db_));

    if (pBankTransaction->transType_!=TRANS_TYPE_TRANSFER_STR)
        pCopyTransaction->accountID_ = accountID;
    else
    {
        if (accountID != pBankTransaction->accountID_ && accountID != pBankTransaction->toAccountID_)
            pCopyTransaction->accountID_ = accountID;
        else
            pCopyTransaction->accountID_ = pBankTransaction->accountID_;
    }
    pCopyTransaction->toAccountID_ = pBankTransaction->toAccountID_;
    pCopyTransaction->amt_         = pBankTransaction->amt_;
    pCopyTransaction->toAmt_       = pBankTransaction->toAmt_;
    pCopyTransaction->payee_       = pBankTransaction->payee_;
    pCopyTransaction->payeeID_     = pBankTransaction->payeeID_;
    pCopyTransaction->transType_   = pBankTransaction->transType_;
    pCopyTransaction->status_      = pBankTransaction->status_;
    pCopyTransaction->transNum_    = pBankTransaction->transNum_;
    pCopyTransaction->notes_       = pBankTransaction->notes_;
    pCopyTransaction->categID_     = pBankTransaction->categID_;
    pCopyTransaction->subcategID_  = pBankTransaction->subcategID_;
    pCopyTransaction->date_        = (useOriginalDate ? pBankTransaction->date_ : wxDateTime::Now());
    pCopyTransaction->category_    = pBankTransaction->category_;
    pCopyTransaction->followupID_  = pBankTransaction->followupID_;

    // we need to create a new pointer for Split transactions.
    boost::shared_ptr<mmSplitTransactionEntries> splitTransEntries(new mmSplitTransactionEntries());
    pBankTransaction->getSplitTransactions(splitTransEntries.get());
    pCopyTransaction->splitEntries_.get()->entries_ = splitTransEntries->entries_;

    if (checkForExistingTransaction(pCopyTransaction)) pCopyTransaction->status_ = wxT("D");

    wxSQLite3Statement st = core_->db_.get()->PrepareStatement(INSERT_INTO_CHECKINGACCOUNT_V1);
    const mmBankTransaction &r = *pBankTransaction;

    int i = 0;
    st.Bind(++i, pCopyTransaction->accountID_);
    st.Bind(++i, r.toAccountID_);
    st.Bind(++i, r.payeeID_);
    st.Bind(++i, r.transType_);
    st.Bind(++i, r.amt_);
    st.Bind(++i, r.status_);
    st.Bind(++i, r.transNum_);
    st.Bind(++i, r.notes_);
    st.Bind(++i, r.categID_);
    st.Bind(++i, r.subcategID_);
    st.Bind(++i, pCopyTransaction->date_.FormatISODate());
    st.Bind(++i, r.followupID_ );
    st.Bind(++i, r.toAmt_ );

    wxASSERT(st.GetParamCount() == i);
    st.ExecuteUpdate();

    pCopyTransaction->transactionID(core_->db_.get()->GetLastRowId().ToLong());
    st.Finalize();

    pCopyTransaction->splitEntries_->updateToDB(core_->db_, pCopyTransaction->transactionID(), false);
    transactions_.push_back(pCopyTransaction);

    return pCopyTransaction;
}

boost::shared_ptr<mmBankTransaction> mmBankTransactionList::getBankTransactionPtr(int accountID, int transactionID) const
{
    for (const_iterator i = transactions_.begin(); i!= transactions_.end(); ++i)
    {
        boost::shared_ptr<mmBankTransaction> pBankTransaction = *i;
        if (pBankTransaction)
        {
            if (((pBankTransaction->accountID_ == accountID) ||
               (pBankTransaction->toAccountID_ == accountID))
               && (pBankTransaction->transactionID() == transactionID))
            {
                return pBankTransaction;
            }
        }
    }
    // didn't find the transaction
    wxASSERT(false);
    return boost::shared_ptr<mmBankTransaction> ();
}

boost::shared_ptr<mmBankTransaction> mmBankTransactionList::getBankTransactionPtr(int transactionID) const
{
    for (const_iterator i = transactions_.begin(); i!= transactions_.end(); ++i)
    {
        boost::shared_ptr<mmBankTransaction> pBankTransaction = *i;
        if (pBankTransaction)
        {
            if (pBankTransaction->transactionID() == transactionID)
            {
                return pBankTransaction;
            }
        }
    }
    // didn't find the transaction
    wxASSERT(false);
    return boost::shared_ptr<mmBankTransaction> ();
}

void mmBankTransactionList::LoadTransactions(mmCoreDB* core)
{
    wxSQLite3ResultSet q1 = core_->db_.get()->ExecuteQuery(SELECT_ALL_FROM_CHECKINGACCOUNT_V1);

    while (q1.NextRow())
    {
        boost::shared_ptr<mmBankTransaction> pAccountTransaction(new mmBankTransaction(core, q1));
        transactions_.push_back(pAccountTransaction);
    }

    q1.Finalize();
}

void mmBankTransactionList::UpdateTransaction(boost::shared_ptr<mmBankTransaction> pBankTransaction)
{
    if (pBankTransaction->transType_ == TRANS_TYPE_TRANSFER_STR)
        pBankTransaction->payeeID_ = -1;

    wxSQLite3Statement st = core_->db_.get()->PrepareStatement(UPDATE_CHECKINGACCOUNT_V1);
    mmBankTransaction &r = *pBankTransaction;

    int i = 0;
    st.Bind(++i, r.accountID_);
    st.Bind(++i, r.toAccountID_);
    st.Bind(++i, r.payeeID_);
    st.Bind(++i, r.transType_);
    st.Bind(++i, r.amt_);
    st.Bind(++i, r.status_);
    st.Bind(++i, r.transNum_);
    st.Bind(++i, r.notes_);
    st.Bind(++i, r.categID_);
    st.Bind(++i, r.subcategID_);
    st.Bind(++i, r.date_.FormatISODate());
    st.Bind(++i, r.followupID_);
    st.Bind(++i, r.toAmt_);
    st.Bind(++i, r.transactionID());

    wxASSERT(st.GetParamCount() == i);
    st.ExecuteUpdate();

    r.splitEntries_->updateToDB(core_->db_, r.transactionID(), true);
    st.Finalize();
    mmOptions::instance().databaseUpdated_ = true;
}

void mmBankTransactionList::UpdateAllTransactions()
{
    // We need to update all transactions incase of errors when loading
    for (const_iterator i = transactions_.begin(); i != transactions_.end(); ++i)
    {
        boost::shared_ptr<mmBankTransaction> pBankTransaction = *i;
        if (pBankTransaction && pBankTransaction->updateRequired_)
        {
           UpdateTransaction(pBankTransaction);
           pBankTransaction->updateRequired_ = false;
        }
    }
}

void mmBankTransactionList::UpdateAllTransactionsForCategory(int categID,
                                                             int subCategID)
{
    // We need to update all transactions incase of errors when loading
    for (const_iterator i = transactions_.begin(); i != transactions_.end(); ++i)
    {
        boost::shared_ptr<mmBankTransaction> pBankTransaction = *i;
        if (pBankTransaction && (pBankTransaction->categID_ == categID)
            && (pBankTransaction->subcategID_ == subCategID))
        {
            pBankTransaction->category_ = core_->categoryList_.GetCategorySharedPtr(categID, subCategID);
            boost::shared_ptr<mmCategory> pCategory = pBankTransaction->category_.lock();

            boost::shared_ptr<mmCategory> parent = pCategory->parent_.lock();
            if (parent)
            {
                pBankTransaction->catStr_ = parent->categName_;
                pBankTransaction->subCatStr_ = pCategory->categName_;
                pBankTransaction->categID_ = parent->categID_;
                pBankTransaction->subcategID_ = pCategory->categID_;
            }
            else
            {
                pBankTransaction->catStr_ = pCategory->categName_;
                pBankTransaction->subCatStr_ = wxT("");
                pBankTransaction->categID_ = pCategory->categID_;
                pBankTransaction->subcategID_ = -1;
            }
        }
    }
}

int mmBankTransactionList::UpdateAllTransactionsForPayee(int payeeID)
{
    // We need to update all transactions incase of errors when loading
    for (const_iterator i = transactions_.begin(); i != transactions_.end(); ++i)
    {
        boost::shared_ptr<mmBankTransaction> pBankTransaction = *i;
        if (pBankTransaction && (pBankTransaction->payeeID_ == payeeID))
        {
            pBankTransaction->payee_ = core_->payeeList_.GetPayeeSharedPtr(payeeID);
            if (pBankTransaction->transType_ != TRANS_TYPE_TRANSFER_STR)
            {
                boost::shared_ptr<mmPayee> pPayee = pBankTransaction->payee_.lock();
                wxASSERT(pPayee);
                pBankTransaction->payeeStr_ = pPayee->name_;
                pBankTransaction->payeeID_ = pPayee->id_;
            }
        }
    }

    return 0;
}

void mmBankTransactionList::getExpensesIncome(const mmCoreDB* core, int accountID, double& expenses, double& income,
    bool ignoreDate, const wxDateTime &dtBegin, const wxDateTime &dtEnd, bool ignoreFuture) const
{
    for (const_iterator i = transactions_.begin(); i != transactions_.end(); ++i)
    {
        boost::shared_ptr<const mmBankTransaction> pBankTransaction = *i;
        if (pBankTransaction)
        {
            if (accountID != -1)
            {
                if (pBankTransaction->accountID_ != accountID && pBankTransaction->toAccountID_ != accountID)
                    continue; // skip
            }
            if (pBankTransaction->status_ == wxT("V"))
            {
                continue; // skip
            }
            if (ignoreFuture)
            {
                if (pBankTransaction->date_.IsLaterThan(wxDateTime::Now()))
                    continue; //skip future dated transactions
            }
            if (!ignoreDate)
            {
                if (!pBankTransaction->date_.IsBetween(dtBegin, dtEnd))
                    continue; //skip
            }

            // We got this far, get the currency conversion rate for this account
            double convRate = core->accountList_.getAccountBaseCurrencyConvRate(pBankTransaction->accountID_);

            if (pBankTransaction->transType_ == TRANS_TYPE_DEPOSIT_STR)
                income += pBankTransaction->amt_ * convRate;
            else if (pBankTransaction->transType_ == TRANS_TYPE_WITHDRAWAL_STR)
                expenses += pBankTransaction->amt_ * convRate;
            else if (pBankTransaction->transType_ == TRANS_TYPE_TRANSFER_STR)
            {
                // transfers are not considered in income/expenses calculations
            }
        }
    }
}

void mmBankTransactionList::getTransactionStats(int accountID, int& number,
    bool ignoreDate, const wxDateTime &dtBegin, const wxDateTime &dtEnd, bool ignoreFuture) const
{
    for (const_iterator i = transactions_.begin(); i != transactions_.end(); ++i)
    {
        boost::shared_ptr<const mmBankTransaction> pBankTransaction = *i;

        if (pBankTransaction)
        {
            if (accountID != -1)
            {
                if (pBankTransaction->accountID_ != accountID && pBankTransaction->toAccountID_ != accountID)
                    continue; // skip
            }
            if (pBankTransaction->status_ == wxT("V"))
            {
                continue; // skip
            }
            if (ignoreFuture)
            {
                if (pBankTransaction->date_.IsLaterThan(wxDateTime::Now()))
                    continue; //skip future dated transactions
            }
            if (!ignoreDate)
            {
                if (!pBankTransaction->date_.IsBetween(dtBegin, dtEnd))
                    continue; //skip
            }

            ++number;
        }
    }
}

wxDateTime mmBankTransactionList::getLastDate(int accountID) const
{
    // Set initial date a long time in past
    wxDateTime dt(wxDateTime::Now().Subtract(wxDateSpan::Years(20)));
    bool same_initial_date = true;

    for (const_iterator i = transactions_.begin(); i != transactions_.end(); ++i)
    {
        boost::shared_ptr<const mmBankTransaction> pBankTransaction = *i;

        if (pBankTransaction)
        {
            if (accountID != -1)
            {
                if (pBankTransaction->accountID_ != accountID && pBankTransaction->toAccountID_ != accountID)
                    continue; // skip

                if (pBankTransaction->date_.IsLaterThan(wxDateTime::Now()))
                    continue; //skip future dated transactions
            }

            if (!dt.IsLaterThan(pBankTransaction->date_))
            {
                dt = pBankTransaction->date_;
                same_initial_date = false;
            }
        }
    }

    if (same_initial_date) // then set date to present day.
    {
        dt = wxDateTime::Now();
    }

    return dt;
}

int mmBankTransactionList::getLastUsedCategoryID(const int accountID
    , const int payeeID, const wxString sType, int& subcategID) const
{
    int categ_id = -1;
    subcategID = -1;
    int index = transactions_.size() - 1;
    bool searching = true;

    while (searching && index >= 0)
    {
        boost::shared_ptr<const mmBankTransaction> pTransaction = transactions_[index];
        if (pTransaction)
        {
            if ((pTransaction->accountID_ == accountID || pTransaction->toAccountID_ == accountID)
                && pTransaction->transType_ == sType
                && pTransaction->payeeID_ == payeeID)
            {
                categ_id = pTransaction->categID_;
                if (categ_id > -1)
                {
                    subcategID = pTransaction->subcategID_;
                    searching = false;
                }
            }
        }
        index --;
    }
    return categ_id;
}

int mmBankTransactionList::getLastUsedPayeeID(const int accountID, wxString sType, int& categID, int& subcategID) const
{
    int payee_id = -1;
    int index = transactions_.size() - 1;
    bool searching = true;
    while (searching && index >= 0)
    {
        boost::shared_ptr<const mmBankTransaction> pBankTransaction = transactions_[index];
        if (pBankTransaction)
        {
            if (pBankTransaction->accountID_ == accountID
                && pBankTransaction->transType_ == sType)
            {
                payee_id   = pBankTransaction->payeeID_;
                categID    = pBankTransaction->categID_;
                subcategID = pBankTransaction->subcategID_;
                searching = false;
            }
        }
        index --;
    }

    return payee_id;
}

double mmBankTransactionList::getAmountForPayee(int payeeID, bool ignoreDate,
    const wxDateTime &dtBegin, const wxDateTime &dtEnd, bool ignoreFuture) const
{
    double amt = 0.0;
    for (const_iterator i = transactions_.begin(); i != transactions_.end(); ++i)
    {
        boost::shared_ptr<const mmBankTransaction> pBankTransaction = *i;
        if (pBankTransaction)
        {
            if (pBankTransaction->payeeID_ == payeeID)
            {
                if (pBankTransaction->status_ == wxT("V"))
                {
                    continue; // skip
                }
                if (ignoreFuture)
                {
                    if (pBankTransaction->date_.IsLaterThan(wxDateTime::Now()))
                        continue; //skip future dated transactions
                }
                if (!ignoreDate)
                {
                    if (!pBankTransaction->date_.IsBetween(dtBegin, dtEnd))
                        continue; //skip
                }

                if (pBankTransaction->transType_ == TRANS_TYPE_TRANSFER_STR)
                    continue;

                double convRate = core_->accountList_.getAccountBaseCurrencyConvRate(pBankTransaction->accountID_);

                amt += pBankTransaction->value(-1) * convRate;
            }
        }
    }

    return amt;
}

double mmBankTransactionList::getAmountForCategory(
    int categID,
    int subcategID,
    bool ignoreDate,
    const wxDateTime &dtBegin,
    const wxDateTime &dtEnd,
    bool evaluateTransfer,      // activates the asDeposit parameter.
    bool asDeposit,             // No effect when evaluateTransfer is false.
    bool ignoreFuture
) const
{
    double amt = 0.0;

    for (const_iterator i = transactions_.begin(); i != transactions_.end(); ++i)
    {
        boost::shared_ptr<const mmBankTransaction> pBankTransaction = *i;

        if (!pBankTransaction || !pBankTransaction->containsCategory(categID, subcategID))
        {
            continue;
        }
        if (pBankTransaction->status_ == wxT("V"))
        {
            continue; // skip
        }
        if (ignoreFuture)
        {
            if ( pBankTransaction->date_.GetDateOnly() > wxDateTime::Now().GetDateOnly())
                continue; //skip future dated transactions
        }
        if (!ignoreDate)
        {
            if ((pBankTransaction->date_.GetDateOnly() < dtBegin.GetDateOnly())
                ||  (pBankTransaction->date_.GetDateOnly() > dtEnd.GetDateOnly()))
            {
                continue; //skip
            }
        }

        double convRate = core_->accountList_.getAccountBaseCurrencyConvRate(pBankTransaction->accountID_);
        if (pBankTransaction->transType_ == TRANS_TYPE_TRANSFER_STR)
        {
            if (evaluateTransfer)
            {
                if (asDeposit)
                {
                    amt += pBankTransaction->getAmountForSplit(categID, subcategID) * convRate;
                }
                else
                {
                    amt -= pBankTransaction->getAmountForSplit(categID, subcategID) * convRate;
                }
            }
            continue;  //skip
        }
        if (pBankTransaction->transType_ == TRANS_TYPE_WITHDRAWAL_STR)
        {
            amt -= pBankTransaction->getAmountForSplit(categID, subcategID) * convRate;
        }
        else if (pBankTransaction->transType_ == TRANS_TYPE_DEPOSIT_STR)
        {
            amt += pBankTransaction->getAmountForSplit(categID, subcategID) * convRate;
        }
    }

    return amt;
}

double mmBankTransactionList::getBalance(int accountID, bool ignoreFuture) const
{
    double balance = 0.0;
    wxDateTime now = wxDateTime::Now();
    for (const_iterator i = transactions_.begin(); i != transactions_.end(); ++i)
    {
        boost::shared_ptr<mmBankTransaction> pBankTransaction = *i;
        if (pBankTransaction)
        {
            if (pBankTransaction->accountID_ != accountID && pBankTransaction->toAccountID_ != accountID)
                continue; // skip

            if (pBankTransaction->status_ == wxT("V"))
                continue; // skip

            if (ignoreFuture)
            {
                if (pBankTransaction->date_.IsLaterThan(now))
                    continue; //skip future dated transactions
            }

            balance += pBankTransaction->value(accountID);
        }
    }

    return balance;
}

bool mmBankTransactionList::getDailyBalance(const mmCoreDB* core, int accountID, std::map<wxDateTime, double>& daily_balance, bool ignoreFuture) const
{
    wxDateTime now = wxDateTime::Now();
    double convRate = core->accountList_.getAccountBaseCurrencyConvRate(accountID);
    for (const_iterator i = transactions_.begin(); i != transactions_.end(); ++i)
    {
        boost::shared_ptr<mmBankTransaction> pBankTransaction = *i;
        if (pBankTransaction)
        {
            if (pBankTransaction->accountID_ != accountID && pBankTransaction->toAccountID_ != accountID)
                continue; // skip

            if (pBankTransaction->status_ == wxT("V"))
                continue; // skip

            if (ignoreFuture)
            {
                if (pBankTransaction->date_.IsLaterThan(now))
                    continue; //skip future dated transactions
            }

            daily_balance[pBankTransaction->date_] += pBankTransaction->value(accountID) * convRate;
        }
    }

    return true;
}

double mmBankTransactionList::getReconciledBalance(int accountID, bool ignoreFuture) const
{
    double balance = 0.0;
    for (const_iterator i = transactions_.begin(); i != transactions_.end(); ++i)
    {
        boost::shared_ptr<mmBankTransaction> pBankTransaction = *i;
        if (pBankTransaction)
        {
            if (pBankTransaction->accountID_ != accountID && pBankTransaction->toAccountID_ != accountID)
                continue; // skip

            if (ignoreFuture)
            {
                if (pBankTransaction->date_.IsLaterThan(wxDateTime::Now()))
                    continue; //skip future dated transactions
            }

            if (pBankTransaction->status_ != wxT("R"))
                continue; // skip

            balance += pBankTransaction->value(accountID);
        }
    }

    return balance;
}

int mmBankTransactionList::countFollowupTransactions() const
{
    int numFollowup = 0;
    for (const_iterator i = transactions_.begin(); i != transactions_.end(); ++i)
    {
        boost::shared_ptr<mmBankTransaction> pBankTransaction = *i;
        if (pBankTransaction)
        {
            if (pBankTransaction->status_ != wxT("F"))
                continue; // skip

            numFollowup++;
        }
    }
    return numFollowup;
}

/** removes the transaction from memory */
bool mmBankTransactionList::removeTransaction(int accountID, int transactionID)
{
    std::vector< boost::shared_ptr<mmBankTransaction> >::iterator i;
    for (i = transactions_.begin(); i!= transactions_.end(); ++i)
    {
        boost::shared_ptr<mmBankTransaction> pBankTransaction = *i;
        if (pBankTransaction)
        {
            if ((pBankTransaction->accountID_ == accountID) || (pBankTransaction->toAccountID_ == accountID))
            {
                if (pBankTransaction->transactionID() == transactionID)
                {
                    i = transactions_.erase(i);
                    return true;
                }
            }
        }
    }
    // didn't find the transaction
    wxASSERT(false);
    return false;
}

/** removes the transaction from memory and the database */
bool mmBankTransactionList::deleteTransaction(int accountID, int transactionID)
{
    if (mmDBWrapper::deleteTransaction(core_->db_.get(), transactionID))
    {
        removeTransaction(accountID, transactionID);
        return true;
    }
    return false;
}

void mmBankTransactionList::deleteTransactions(int accountID)
{
    std::vector< boost::shared_ptr<mmBankTransaction> >::iterator i;
    for (i = transactions_.begin(); i!= transactions_.end(); )
    {
        boost::shared_ptr<mmBankTransaction> pBankTransaction = *i;
        if (pBankTransaction)
        {
            if ((pBankTransaction->accountID_ == accountID) ||
                (pBankTransaction->toAccountID_ == accountID))
            {
                i = transactions_.erase(i);
            }
            else
                ++i;
        }
    }
}

wxArrayString mmBankTransactionList::getTransactionNumber(const int accountID, const wxDateTime transaction_date) const
{
    double trx_number, today_number = 1, max_number = 1;
    wxArrayString number_strings;
    for (const_iterator i = transactions_.begin(); i != transactions_.end(); ++i)
    {
        boost::shared_ptr<mmBankTransaction> pBankTransaction = *i;
        if (pBankTransaction)
        {
            if (pBankTransaction->accountID_ != accountID && pBankTransaction->toAccountID_ != accountID)
                continue; // skip
            if ((pBankTransaction->transNum_).IsEmpty())
                continue;

            trx_number = 1;
            if ((pBankTransaction->transNum_).ToDouble(&trx_number))
            {
                if (max_number <= trx_number) max_number = trx_number+1;
                if (pBankTransaction->date_.FormatISODate() == transaction_date.FormatISODate())
                    if (today_number <= trx_number) today_number = trx_number+1;
            }
        }
    }
    number_strings.Add(wxString()<<(int)today_number);
    number_strings.Add(wxString()<<(int)max_number);

    return number_strings;
}

int mmBankTransactionList::RelocatePayee(mmCoreDB* core, const int destPayeeID, const int sourcePayeeID, int& changedPayees_)
{

    if (mmDBWrapper::relocatePayee(core_->db_.get(), destPayeeID, sourcePayeeID) == 0)
    {

        changedPayees_=0;
        for (const_iterator i = transactions_.begin(); i != transactions_.end(); ++i)
        {
            boost::shared_ptr<mmBankTransaction> pBankTransaction = *i;
            if (pBankTransaction && (pBankTransaction->payeeID_ == sourcePayeeID))
            {
                pBankTransaction->payee_ = core->payeeList_.GetPayeeSharedPtr(destPayeeID);
                pBankTransaction->payeeStr_ = core->payeeList_.GetPayeeName(destPayeeID);
                pBankTransaction->payeeID_ = destPayeeID;
                changedPayees_++;
            }
        }

    }
    return 0;
}

int mmBankTransactionList::RelocateCategory(mmCoreDB* core,
    const int destCatID, const int destSubCatID, const int sourceCatID, const int sourceSubCatID,
    int& changedCats, int& changedSubCats)
{
    int err = mmDBWrapper::relocateCategory(core_->db_.get(),
        destCatID, destSubCatID, sourceCatID, sourceSubCatID);
    if ( err == 0 )
    {

        changedCats=0;
        changedSubCats=0;
        for (const_iterator i = transactions_.begin(); i != transactions_.end(); ++i)
        {
            boost::shared_ptr<mmBankTransaction> pBankTransaction = *i;
            if (pBankTransaction && (pBankTransaction->categID_ == sourceCatID)
                && pBankTransaction->subcategID_== sourceSubCatID)
            {
                pBankTransaction->category_ = core->categoryList_.GetCategorySharedPtr(destCatID, destSubCatID);
                pBankTransaction->catStr_ = core->categoryList_.GetCategoryName(destCatID);
                pBankTransaction->subCatStr_ = core->categoryList_.GetSubCategoryName(destCatID, destSubCatID);
                pBankTransaction->categID_ = destCatID;
                pBankTransaction->subcategID_ = destSubCatID;
                pBankTransaction->fullCatStr_ = core->categoryList_.GetFullCategoryString(destCatID, destSubCatID);
                changedCats++;
            }
            else if (pBankTransaction && (pBankTransaction->categID_ == -1))
            {
                mmSplitTransactionEntries* splits = pBankTransaction->splitEntries_.get();
                pBankTransaction->getSplitTransactions(splits);

                for (int i = 0; i < (int)splits->entries_.size(); ++i)
                {
                    if (splits->entries_[i]->categID_==sourceCatID && splits->entries_[i]->subCategID_==sourceSubCatID)
                    {
                        splits->entries_[i]->categID_ = destCatID;
                        splits->entries_[i]->subCategID_ = destSubCatID;
                        changedSubCats++;
                    }
                }

            }
        }
    }
    return err;
}

void mmBankTransactionList::ChangeDateFormat()
{
    for (const_iterator i = transactions_.begin(); i != transactions_.end(); ++i)
    {
        boost::shared_ptr<mmBankTransaction> pBankTransaction = *i;
        if (pBankTransaction)
        {
            pBankTransaction->dateStr_ = (pBankTransaction->date_).Format(mmOptions::instance().dateFormat_);
        }
    }
}

bool mmBankTransactionList::IsCategoryUsed(const int iCatID, const int iSubCatID, bool& bIncome, bool bIgnor_subcat) const
{
    int index = transactions_.size() - 1;
    double sum = 0;
    bool bTrxUsed = false;

    boost::shared_ptr<mmBankTransaction> pBankTransaction;
    while (index >= 0)
    {
        pBankTransaction = transactions_[index];
        if (pBankTransaction)
        {
            if ((pBankTransaction->categID_ == iCatID)
                && (bIgnor_subcat ? true : pBankTransaction->subcategID_== iSubCatID))
            {
                bTrxUsed = true;
                if (pBankTransaction->transType_ == TRANS_TYPE_DEPOSIT_STR)
                    sum += pBankTransaction->amt_;
                else
                    sum -= pBankTransaction->amt_;
            }

            mmSplitTransactionEntries* splits = pBankTransaction->splitEntries_.get();

            for (int i = 0; i < (int)splits->entries_.size(); ++i)
            {
                if (splits->entries_[i]->categID_==iCatID && splits->entries_[i]->subCategID_==iSubCatID)
                {
                    bTrxUsed = true;
                    if ((pBankTransaction->transType_ == TRANS_TYPE_DEPOSIT_STR && splits->entries_[i]->splitAmount_ > 0)
                        || (pBankTransaction->transType_ == TRANS_TYPE_WITHDRAWAL_STR && splits->entries_[i]->splitAmount_ < 0))
                        sum += fabs(splits->entries_[i]->splitAmount_);
                    else
                        sum -= fabs(splits->entries_[i]->splitAmount_);
                }
            }
        }
        index --;
    }
    bIncome = sum > 0;
    return bTrxUsed;
}

bool mmBankTransactionList::IsPayeeUsed(const int iPayeeID) const
{
    int index = transactions_.size() - 1;
    bool searching = false;
    boost::shared_ptr<mmBankTransaction> pBankTransaction;
    while (!searching && index >= 0)
    {
        pBankTransaction = transactions_[index];
        if (pBankTransaction && (pBankTransaction->payeeID_ == iPayeeID))
        {
            searching = true;
        }
        index --;
    }
    return searching;
}
