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
        static const char sql[] = "delete from SPLITTRANSACTIONS_V1 where TRANSID = ?";

        wxSQLite3Statement st = db->PrepareStatement(sql);
        st.Bind(1, transID);
        st.ExecuteUpdate();
        st.Finalize();
    }

    static const char sql[] = 
    "insert into SPLITTRANSACTIONS_V1 (TRANSID, CATEGID, SUBCATEGID, SPLITTRANSAMOUNT) "
    "values (?, ?, ?, ?)";

    wxSQLite3Statement st = db->PrepareStatement(sql);

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

void mmSplitTransactionEntries::loadFromBDDB(mmCoreDB* core,
                                            int bdID)
{
   entries_.clear();
   total_ = 0.0;

   static const char sql[] = 
    "select SPLITTRANSID, "
           "SPLITTRANSAMOUNT, "
           "CATEGID, "
           "SUBCATEGID "
    "from BUDGETSPLITTRANSACTIONS_V1 "
    "where TRANSID = ?";
    
   wxSQLite3Statement st = core->db_->PrepareStatement(sql);
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

      pSplitEntry->category_ = core->getCategorySharedPtr(catID, subID);
      wxASSERT(pSplitEntry->category_.lock());

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
    date_           = mmGetStorageStringAsDate(q1.GetString(wxT("TRANSDATE")));
    transNum_       = q1.GetString(wxT("TRANSACTIONNUMBER"));
    status_         = q1.GetString(wxT("STATUS"));
    notes_          = q1.GetString(wxT("NOTES"));
    transType_      = q1.GetString(wxT("TRANSCODE"));
    accountID_      = q1.GetInt(wxT("ACCOUNTID"));
    toAccountID_    = q1.GetInt(wxT("TOACCOUNTID"));
    payee_ = core->getPayeeSharedPtr(q1.GetInt(wxT("PAYEEID")));
    amt_            = q1.GetDouble(wxT("TRANSAMOUNT"));
    toAmt_          = q1.GetDouble(wxT("TOTRANSAMOUNT"));
    category_ = core->getCategorySharedPtr(q1.GetInt(wxT("CATEGID")), q1.GetInt(wxT("SUBCATEGID")));

    boost::shared_ptr<mmCurrency> pCurrencyPtr = core->accountList_.getCurrencyWeakPtr(accountID_).lock();
    wxASSERT(pCurrencyPtr);

    splitEntries_ = boost::shared_ptr<mmSplitTransactionEntries>(new mmSplitTransactionEntries());
    getSplitTransactions(core, splitEntries_.get());

    updateAllData(core, accountID_, pCurrencyPtr);
}

void mmBankTransaction::updateAllData(mmCoreDB* core, 
                                      int accountID, 
                                      boost::shared_ptr<mmCurrency> currencyPtr,
                                      bool forceUpdate
                                      )
{
   if ((isInited_) && (transType_ != TRANS_TYPE_TRANSFER_STR) && !forceUpdate)
   {
      return;
   }

   /* Load the Account Currency Settings for Formatting Strings */
   currencyPtr->loadCurrencySettings();

   dateStr_            = mmGetDateForDisplay(db_.get(), date_);

   wxString displayTransAmtString;
   mmex::formatDoubleToCurrencyEdit(amt_, displayTransAmtString);
   transAmtString_ = displayTransAmtString;

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
             errMsg << core->getAccountName(accountID_)
                    << wxT("\n\n")
                    << _("Subsequent errors not displayed.");
             wxMessageBox(errMsg,_("MMEX DATABASE ERROR"),wxICON_ERROR);
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
      wxString fromAccount = core->getAccountName(accountID_);
      wxString toAccount = core->getAccountName(toAccountID_);

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

   fromAccountStr_ = core->getAccountName(accountID_);

   boost::shared_ptr<mmCategory> pCategory = category_.lock();
   if (!pCategory && !splitEntries_->numEntries())
   {
      // If category is missing, we mark is as unknown
      int categID = core->getCategoryID(wxT("Unknown"));
      if (categID == -1)
      {
         categID =  core->addCategory(wxT("Unknown"));
      }

      category_ = core->getCategorySharedPtr(categID, -1);
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
         fullCatStr_ = catStr_ + wxT(":") +subCatStr_;
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
   else if (splitEntries_->numEntries() > 0)
   {
      fullCatStr_ = _("Split Category");
      categID_ = -1;
      subcategID_ = -1;
      catStr_= wxT("");
      subCatStr_ = wxT("");
   }
   isInited_ = true;
}

double mmBankTransaction::value(int accountID)
{
   double balance = 0.0;
   if (transType_ == TRANS_TYPE_DEPOSIT_STR)
   {
      balance = amt_;
   }
   else if (transType_== TRANS_TYPE_WITHDRAWAL_STR)
   {
      balance -= amt_;
   }
   else if (transType_ == TRANS_TYPE_TRANSFER_STR)
   {

      if (accountID_ == accountID)
      {
         balance -= amt_;
      }
      else
      {
         wxASSERT(toAccountID_ == accountID);
         balance += toAmt_;
      }
   }
   return balance;
}
void mmBankTransaction::getSplitTransactions(mmCoreDB* core, mmSplitTransactionEntries* splits) const
{
    splits->entries_.clear();
    splits->total_ = 0.0;

    static const char sql[] = 
    "select SPLITTRANSID, "
           "SPLITTRANSAMOUNT, "
           "CATEGID, "
           "SUBCATEGID "
    "from SPLITTRANSACTIONS_V1 "
    "where TRANSID = ?";
    
    wxSQLite3Statement st = db_->PrepareStatement(sql);
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

        boost::shared_ptr<mmCategory> p_cat = core->getCategorySharedPtr(catID, subID);
        wxASSERT(p_cat);
        pSplitEntry->category_ = p_cat;

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
                (splitEntries_->entries_[idx]->subCategID_ == subcategID))
            {
                return true;
            }
        }
    }
    else if (categID_ == categID)
    {
        if (ignoreSubCateg)
            return true;
       
        if (subcategID_ == subcategID)
        {
            return true;
        }

    }
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
mmBankTransactionList::mmBankTransactionList(boost::shared_ptr<wxSQLite3Database> db)
: db_(db)
{ 
   /* Allocate some empty space so loading transactions is faster */
   transactions_.reserve(5000);
}

int mmBankTransactionList::addTransaction(mmCoreDB* core, boost::shared_ptr<mmBankTransaction> pBankTransaction)
{
   if (checkForExistingTransaction(pBankTransaction))
   {
       pBankTransaction->status_ = wxT("D");
   }

   if(core->payeeExists(pBankTransaction->payeeID_) == false) 
   {
       pBankTransaction->payeeID_ = -1;
   }

   static const char sql[] = 
       "insert into CHECKINGACCOUNT_V1 ( "
       "ACCOUNTID, TOACCOUNTID, PAYEEID, TRANSCODE, "
       "TRANSAMOUNT, STATUS, TRANSACTIONNUMBER, NOTES, "
       "CATEGID, SUBCATEGID, TRANSDATE, FOLLOWUPID, TOTRANSAMOUNT "
       ") values (?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, -1, ?)";

   wxSQLite3Statement st = db_->PrepareStatement(sql);
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
   st.Bind(++i, r.toAmt_);

   wxASSERT(st.GetParamCount() == i);
   st.ExecuteUpdate();

   r.transactionID(db_->GetLastRowId().ToLong());
   st.Finalize();
   mmOptions::instance().databaseUpdated_ = true;

   r.splitEntries_->updateToDB(db_, r.transactionID(), false);
   transactions_.push_back(pBankTransaction);

   return pBankTransaction->transactionID();
}

bool mmBankTransactionList::checkForExistingTransaction(boost::shared_ptr<mmBankTransaction> pBankTransaction)
{
    static const char sql[] = 
    "select 1 "
    "from CHECKINGACCOUNT_V1 "
    "where ACCOUNTID = ? and "
          "TOACCOUNTID = ? and "
          "PAYEEID = ? and "
          "TRANSCODE = ? and "
          "TRANSAMOUNT = ? and "
          "TRANSACTIONNUMBER = ? and "
          "NOTES = ? and "
          "CATEGID = ? and "
          "SUBCATEGID = ? and "
          "TRANSDATE = ? and "
          "TOTRANSAMOUNT = ? and "
          "TRANSID > 0"; // is not null

   bool found = false;

   wxSQLite3Statement st = db_->PrepareStatement(sql);
   const mmBankTransaction &r = *pBankTransaction;

   int i = 0;
   st.Bind(++i, r.accountID_);
   st.Bind(++i, r.toAccountID_);
   st.Bind(++i, r.payeeID_);
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
   found = q1.NextRow(); // TODO: Need to check split entries
   st.Finalize();
    
    return found;
}

boost::shared_ptr<mmBankTransaction> mmBankTransactionList::copyTransaction(int transactionID, bool useOriginalDate)
{
   boost::shared_ptr<mmBankTransaction> pBankTransaction = getBankTransactionPtr(transactionID);
   if (!pBankTransaction)
       return boost::shared_ptr<mmBankTransaction>();

   boost::shared_ptr<mmBankTransaction> pCopyTransaction(new mmBankTransaction(db_));
   
   pCopyTransaction->accountID_ = pBankTransaction->accountID_; 
   pCopyTransaction->toAccountID_ = pBankTransaction->toAccountID_;
   pCopyTransaction->payee_       = pBankTransaction->payee_;
   pCopyTransaction->payeeID_=    pBankTransaction->payeeID_;
   pCopyTransaction->transType_=    pBankTransaction->transType_;
   pCopyTransaction->amt_ =   pBankTransaction->amt_;
   pCopyTransaction->status_ =    (useOriginalDate ? wxChar('D') : pBankTransaction->status_);
   pCopyTransaction->transNum_=   pBankTransaction->transNum_;
   pCopyTransaction->notes_=   pBankTransaction->notes_;
   pCopyTransaction->categID_  =   pBankTransaction->categID_;
   pCopyTransaction->subcategID_ =   pBankTransaction->subcategID_;
   pCopyTransaction->date_=    (useOriginalDate ? pBankTransaction->date_ : wxDateTime::Now()); 
   pCopyTransaction->toAmt_=   pBankTransaction->toAmt_;
   pCopyTransaction->category_ = pBankTransaction->category_;
   *pCopyTransaction->splitEntries_.get() = *pBankTransaction->splitEntries_.get();

   static const char sql[] = 
   "insert into CHECKINGACCOUNT_V1 ( "
     "ACCOUNTID, TOACCOUNTID, PAYEEID, TRANSCODE, "
     "TRANSAMOUNT, STATUS, TRANSACTIONNUMBER, NOTES, "
     "CATEGID, SUBCATEGID, TRANSDATE, FOLLOWUPID, TOTRANSAMOUNT "
   ") values ( ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, -1, ? )";

   wxSQLite3Statement st = db_->PrepareStatement(sql);
   const mmBankTransaction &r = *pBankTransaction;

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
   st.Bind(++i, pCopyTransaction->date_.FormatISODate());
   st.Bind(++i, r.toAmt_ );

   wxASSERT(st.GetParamCount() == i);
   st.ExecuteUpdate();

   pCopyTransaction->transactionID(db_->GetLastRowId().ToLong());
   st.Finalize();

   pCopyTransaction->splitEntries_->updateToDB(db_, pCopyTransaction->transactionID(), false);
   transactions_.push_back(pCopyTransaction);

   return pCopyTransaction;
}

void mmBankTransactionList::updateTransaction(boost::shared_ptr<mmBankTransaction> pBankTransaction)
{
    if (pBankTransaction->transType_ == TRANS_TYPE_TRANSFER_STR)
        pBankTransaction->payeeID_ = -1;

    static const char sql[] =    
    "update CHECKINGACCOUNT_V1 "
    "SET ACCOUNTID=?, TOACCOUNTID=?, PAYEEID=?, TRANSCODE=?, "
        "TRANSAMOUNT=?, STATUS=?, TRANSACTIONNUMBER=?, NOTES=?, "
        "CATEGID=?, SUBCATEGID=?, TRANSDATE=?, TOTRANSAMOUNT=? "
    "WHERE TRANSID = ?";
    
    wxSQLite3Statement st = db_->PrepareStatement(sql);
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
    st.Bind(++i, r.toAmt_);
    st.Bind(++i, r.transactionID());

    wxASSERT(st.GetParamCount() == i);
    st.ExecuteUpdate();

    r.splitEntries_->updateToDB(db_, r.transactionID(), true);
    st.Finalize();
    mmOptions::instance().databaseUpdated_ = true;
}

boost::shared_ptr<mmBankTransaction> mmBankTransactionList::getBankTransactionPtr(int accountID, int transactionID) const
{
    for (const_iterator i = transactions_.begin(); i!= transactions_.end(); ++ i)
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
    for (const_iterator i = transactions_.begin(); i!= transactions_.end(); ++ i)
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

void mmBankTransactionList::updateAllTransactions()
{
    // We need to update all transactions incase of errors when loading
    for (const_iterator i = transactions_.begin(); i != transactions_.end(); i++ )
    {
        boost::shared_ptr<mmBankTransaction> pBankTransaction = *i;
        if (pBankTransaction && pBankTransaction->updateRequired_)
        {
           updateTransaction(pBankTransaction);
           pBankTransaction->updateRequired_ = false;
        }
    }
}

void mmBankTransactionList::updateAllTransactionsForCategory(mmCoreDB* core, 
                                                             int categID, 
                                                             int subCategID)
{
    // We need to update all transactions incase of errors when loading
    for (const_iterator i = transactions_.begin(); i != transactions_.end(); i++ )
    {
        boost::shared_ptr<mmBankTransaction> pBankTransaction = *i;
        if (pBankTransaction && (pBankTransaction->categID_ == categID)
            && (pBankTransaction->subcategID_ == subCategID))
        {
            pBankTransaction->category_ = core->getCategorySharedPtr(categID, subCategID);
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

int mmBankTransactionList::updateAllTransactionsForPayee(mmCoreDB* core, int payeeID)
{
    // We need to update all transactions incase of errors when loading
    for (const_iterator i = transactions_.begin(); i != transactions_.end(); i++ )
    {
        boost::shared_ptr<mmBankTransaction> pBankTransaction = *i;
        if (pBankTransaction && (pBankTransaction->payeeID_ == payeeID))
        {
            pBankTransaction->payee_ = core->getPayeeSharedPtr(payeeID);
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

void mmBankTransactionList::getExpensesIncome(int accountID, double& expenses, double& income,
    bool ignoreDate, const wxDateTime &dtBegin, const wxDateTime &dtEnd, bool ignoreFuture) const
{
    for (const_iterator i = transactions_.begin(); i != transactions_.end(); i++ )
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
            double convRate = mmDBWrapper::getCurrencyBaseConvRate(db_.get(), pBankTransaction->accountID_);

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

            if (pBankTransaction->transType_ == TRANS_TYPE_DEPOSIT_STR)
                ++number;
            else if (pBankTransaction->transType_ == TRANS_TYPE_WITHDRAWAL_STR)
                ++number;
            else if (pBankTransaction->transType_ == TRANS_TYPE_TRANSFER_STR)
            {
                ++number;
            }
        }
    }
}

double mmBankTransactionList::getAmountForPayee(int payeeID, bool ignoreDate, 
    const wxDateTime &dtBegin, const wxDateTime &dtEnd, bool ignoreFuture) const
{
    double amt = 0.0;
    for (const_iterator i = transactions_.begin(); i != transactions_.end(); i++ )
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

                double convRate = mmDBWrapper::getCurrencyBaseConvRate(db_.get(), pBankTransaction->accountID_);

                if (pBankTransaction->transType_ == TRANS_TYPE_WITHDRAWAL_STR)
                {
                    amt -= pBankTransaction->amt_ * convRate;
                }
                else if (pBankTransaction->transType_ == TRANS_TYPE_DEPOSIT_STR)
                {
                    amt += pBankTransaction->amt_ * convRate;
                }
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

    for (const_iterator i = transactions_.begin(); i != transactions_.end(); i++)
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
            if (pBankTransaction->date_.IsLaterThan(wxDateTime::Now()))
                continue; //skip future dated transactions
        }
        if (!ignoreDate)
        {
            if (!pBankTransaction->date_.IsBetween(dtBegin, dtEnd))
            {
                continue; //skip
            }
        }

        double convRate = mmDBWrapper::getCurrencyBaseConvRate(db_.get(), pBankTransaction->accountID_);
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
    for (const_iterator i = transactions_.begin(); i != transactions_.end(); i++ )
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

            if (pBankTransaction->transType_ == TRANS_TYPE_DEPOSIT_STR)
                balance += pBankTransaction->amt_;
            else if (pBankTransaction->transType_ == TRANS_TYPE_WITHDRAWAL_STR)
                balance -= pBankTransaction->amt_;
            else if (pBankTransaction->transType_ == TRANS_TYPE_TRANSFER_STR)
            {
                if (pBankTransaction->accountID_ == accountID)
                {
                    balance -= pBankTransaction->amt_;
                }
                else if (pBankTransaction->toAccountID_ == accountID)
                {
                    balance += pBankTransaction->toAmt_;
                }
            }
            else
            {
                wxASSERT(false);
            }
        }
    }

    return balance;
}

bool mmBankTransactionList::getDailyBalance(int accountID, std::map<wxDateTime, double>& daily_balance, bool ignoreFuture) const
{
    wxDateTime now = wxDateTime::Now();
    double convRate = mmDBWrapper::getCurrencyBaseConvRate(db_.get(), accountID);
    for (const_iterator i = transactions_.begin(); i != transactions_.end(); i++ )
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

            double &balance = daily_balance[pBankTransaction->date_];

            if (pBankTransaction->transType_ == TRANS_TYPE_DEPOSIT_STR)
            {
                balance += pBankTransaction->amt_ * convRate;
            }
            else if (pBankTransaction->transType_ == TRANS_TYPE_WITHDRAWAL_STR)
            {
                balance -= pBankTransaction->amt_ * convRate;
            }
            else if (pBankTransaction->transType_ == TRANS_TYPE_TRANSFER_STR)
            {
                if (pBankTransaction->accountID_ == accountID)
                {
                    balance -= pBankTransaction->amt_ * convRate;
                }
                else if (pBankTransaction->toAccountID_ == accountID)
                {
                    balance += pBankTransaction->toAmt_ * convRate;
                }
            }
            else
            {
                wxASSERT(false);
            }
        }
    }

    return true;
}

double mmBankTransactionList::getReconciledBalance(int accountID, bool ignoreFuture) const
{
    double balance = 0.0;
    for (const_iterator i = transactions_.begin(); i != transactions_.end(); i++ )
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

            if (pBankTransaction->transType_ == TRANS_TYPE_DEPOSIT_STR)
                balance += pBankTransaction->amt_;
            else if (pBankTransaction->transType_ == TRANS_TYPE_WITHDRAWAL_STR)
                balance -= pBankTransaction->amt_;
            else if (pBankTransaction->transType_ == TRANS_TYPE_TRANSFER_STR)
            {
                if (pBankTransaction->accountID_ == accountID)
                {
                    balance -= pBankTransaction->amt_;
                }
                else if (pBankTransaction->toAccountID_ == accountID)
                {
                    balance += pBankTransaction->toAmt_;
                }
            }
            else
            {
                wxASSERT(false);
            }

        }
    }

    return balance;
}

int mmBankTransactionList::countFollowupTransactions() const
{
    int numFollowup = 0;
    for (const_iterator i = transactions_.begin(); i != transactions_.end(); i++ )
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
    if (mmDBWrapper::deleteTransaction(db_.get(), transactionID)) 
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
