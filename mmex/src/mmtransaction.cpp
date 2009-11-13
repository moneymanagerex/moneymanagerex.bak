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

    // --

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

      pSplitEntry->category_ = core->categoryList_.getCategorySharedPtr(catID, subID);
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

mmBankTransaction::mmBankTransaction(mmCoreDB* core, 
                                     wxSQLite3ResultSet& q1)
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
     payee_ = core->payeeList_.getPayeeSharedPtr(q1.GetInt(wxT("PAYEEID")));
     amt_            = q1.GetDouble(wxT("TRANSAMOUNT"));
     toAmt_          = q1.GetDouble(wxT("TOTRANSAMOUNT"));
     category_ = core->categoryList_.getCategorySharedPtr(q1.GetInt(wxT("CATEGID")), q1.GetInt(wxT("SUBCATEGID")));
   
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
   if ((isInited_) && (transType_ != wxT("Transfer")) && !forceUpdate)
   {
      return;
   }

   /* Load the Account Currency Settings for Formatting Strings */
   currencyPtr->loadCurrencySettings();

   dateStr_            = mmGetDateForDisplay(db_.get(), date_);

   wxString displayTransAmtString;
   if (mmCurrencyFormatter::formatDoubleToCurrencyEdit(amt_, displayTransAmtString))
      transAmtString_ = displayTransAmtString;

   wxString displayToTransAmtString;
   if (mmCurrencyFormatter::formatDoubleToCurrencyEdit(toAmt_, displayToTransAmtString))
      transToAmtString_ = displayToTransAmtString;

   if (transType_ != wxT("Transfer"))
   {
      boost::shared_ptr<mmPayee> pPayee = payee_.lock();
      wxASSERT(pPayee);
      payeeStr_ = pPayee->payeeName_;
      payeeID_ = pPayee->payeeID_;
   }

   depositStr_ = wxT("");
   withdrawalStr_ = wxT("");
   if (transType_ == wxT("Deposit"))
   {
      depositStr_ = displayTransAmtString;
   }
   else if (transType_== wxT("Withdrawal"))
   {
      withdrawalStr_ = displayTransAmtString;
   }
   else if (transType_ == wxT("Transfer"))
   {
      wxString fromAccount = core->accountList_.getAccountSharedPtr(accountID_)->accountName_;
      wxString toAccount = core->accountList_.getAccountSharedPtr(toAccountID_)->accountName_;

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

   fromAccountStr_ = core->accountList_.getAccountSharedPtr(accountID_)->accountName_;

   boost::shared_ptr<mmCategory> pCategory = category_.lock();
   if (!pCategory && !splitEntries_->numEntries())
   {
      // If category is missing, we mark is as unknown
      int categID = core->categoryList_.getCategoryID(wxT("Unknown"));
      if (categID == -1)
      {
         categID =  core->categoryList_.addCategory(wxT("Unknown"));
      }

      category_ = core->categoryList_.getCategorySharedPtr(categID, -1);
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
   if (transType_ == wxT("Deposit"))
   {
      balance = amt_;
   }
   else if (transType_== wxT("Withdrawal"))
   {
      balance -= amt_;
   }
   else if (transType_ == wxT("Transfer"))
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
void mmBankTransaction::getSplitTransactions(mmCoreDB* core, mmSplitTransactionEntries* splits)
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

        boost::shared_ptr<mmCategory> p_cat = core->categoryList_.getCategorySharedPtr(catID, subID);
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
   if (checkForExistingTransaction(pBankTransaction)){
	   pBankTransaction->status_ = wxT("D");
   }

   if(core->payeeList_.payeeExists(pBankTransaction->payeeID_) == false) {
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

void mmBankTransactionList::updateTransaction(
   boost::shared_ptr<mmBankTransaction> pBankTransaction)
{
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
}


boost::shared_ptr<mmBankTransaction> mmBankTransactionList::getBankTransactionPtr
(int accountID, int transactionID)
{
    std::vector< boost::shared_ptr<mmBankTransaction> >::iterator i;
    for (i = transactions_.begin(); i!= transactions_.end(); )
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
            else
                ++i;
        }
    }
    // didn't find the transaction
    wxASSERT(false);
    return boost::shared_ptr<mmBankTransaction> ();
}

boost::shared_ptr<mmBankTransaction> mmBankTransactionList::getBankTransactionPtr
(int transactionID)
{
    std::vector< boost::shared_ptr<mmBankTransaction> >::iterator i;
    for (i = transactions_.begin(); i!= transactions_.end(); )
    {
        boost::shared_ptr<mmBankTransaction> pBankTransaction = *i;
        if (pBankTransaction)
        {
            if (pBankTransaction->transactionID() == transactionID)
            {
                return pBankTransaction;
            }
            else
                ++i;
        }
    }
    // didn't find the transaction
    wxASSERT(false);
    return boost::shared_ptr<mmBankTransaction> ();
}

void mmBankTransactionList::updateAllTransactions()
{
    // We need to update all transactions incase of errors when loading
    std::vector< boost::shared_ptr<mmBankTransaction> >::iterator i;
    for (i = transactions_.begin(); i != transactions_.end(); i++ )
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
    std::vector< boost::shared_ptr<mmBankTransaction> >::iterator i;
    for (i = transactions_.begin(); i != transactions_.end(); i++ )
    {
        boost::shared_ptr<mmBankTransaction> pBankTransaction = *i;
        if (pBankTransaction && (pBankTransaction->categID_ == categID)
            && (pBankTransaction->subcategID_ == subCategID))
        {
            pBankTransaction->category_ = core->categoryList_.getCategorySharedPtr(categID, subCategID);
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

void mmBankTransactionList::updateAllTransactionsForPayee(mmCoreDB* core, 
                                                          int payeeID)
{
    // We need to update all transactions incase of errors when loading
    std::vector< boost::shared_ptr<mmBankTransaction> >::iterator i;
    for (i = transactions_.begin(); i != transactions_.end(); i++ )
    {
        boost::shared_ptr<mmBankTransaction> pBankTransaction = *i;
        if (pBankTransaction && (pBankTransaction->payeeID_ == payeeID))
        {
            pBankTransaction->payee_ = core->payeeList_.getPayeeSharedPtr(payeeID);
            if (pBankTransaction->transType_ != wxT("Transfer"))
            {
                boost::shared_ptr<mmPayee> pPayee = pBankTransaction->payee_.lock();
                wxASSERT(pPayee);
                pBankTransaction->payeeStr_ = pPayee->payeeName_;
                pBankTransaction->payeeID_ = pPayee->payeeID_;
            }
        }
    }
}

void mmBankTransactionList::getExpensesIncome(int accountID, double& expenses, double& income,  
                           bool ignoreDate, const wxDateTime &dtBegin, const wxDateTime &dtEnd) const
{
    std::vector< boost::shared_ptr<mmBankTransaction> >::const_iterator i;
    for (i = transactions_.begin(); i != transactions_.end(); i++ )
    {
        boost::shared_ptr<const mmBankTransaction> pBankTransaction = *i;
        if (pBankTransaction)
        {
            if (accountID != -1)
            {
                if ((pBankTransaction->accountID_ == accountID) ||
                    (pBankTransaction->toAccountID_ == accountID))
                {

                }
                else
                    continue; // skip
            }

            if (pBankTransaction->status_ == wxT("V"))
                continue; // skip

            if (!ignoreDate)
            {
                if (!pBankTransaction->date_.IsBetween(dtBegin, dtEnd))
                    continue; //skip
            }

            // We got this far, get the currency conversion rate for this account
            double convRate = mmDBWrapper::getCurrencyBaseConvRate(db_.get(), pBankTransaction->accountID_);

            if (pBankTransaction->transType_ == wxT("Deposit"))
                income += pBankTransaction->amt_ * convRate;
            else if (pBankTransaction->transType_ == wxT("Withdrawal"))
                expenses += pBankTransaction->amt_ * convRate;
            else if (pBankTransaction->transType_ == wxT("Transfer"))
            {
                // transfers are not considered in income/expenses calculations
            }

        }

    }
}


void mmBankTransactionList::getTransactionStats(int accountID, int& number,  
                           bool ignoreDate, const wxDateTime &dtBegin, const wxDateTime &dtEnd) const
{
    std::vector<boost::shared_ptr<mmBankTransaction> >::const_iterator i;

    for (i = transactions_.begin(); i != transactions_.end(); ++i)
    {
        boost::shared_ptr<const mmBankTransaction> pBankTransaction = *i;

        if (pBankTransaction)
        {
            if (accountID != -1)
            {
                if ((pBankTransaction->accountID_ == accountID) ||
                    (pBankTransaction->toAccountID_ == accountID))
                {

                }
                else
                    continue; // skip
            }

            if (pBankTransaction->status_ == wxT("V"))
                continue; // skip

            if (!ignoreDate)
            {
                if (!pBankTransaction->date_.IsBetween(dtBegin, dtEnd))
                    continue; //skip
            }

            if (pBankTransaction->transType_ == wxT("Deposit"))
                ++number;
            else if (pBankTransaction->transType_ == wxT("Withdrawal"))
                ++number;
            else if (pBankTransaction->transType_ == wxT("Transfer"))
            {
                ++number;
            }

        }
    }
}

double mmBankTransactionList::getAmountForPayee(int payeeID, bool ignoreDate, 
                                 const wxDateTime &dtBegin, const wxDateTime &dtEnd) const
{
    double amt = 0.0;
    std::vector< boost::shared_ptr<mmBankTransaction> >::const_iterator i;
    for (i = transactions_.begin(); i != transactions_.end(); i++ )
    {
        boost::shared_ptr<const mmBankTransaction> pBankTransaction = *i;
        if (pBankTransaction)
        {
           if (pBankTransaction->payeeID_ == payeeID)
           {
              if (pBankTransaction->status_ == wxT("V"))
                 continue; // skip

              if (!ignoreDate)
              {
                 if (!pBankTransaction->date_.IsBetween(dtBegin, dtEnd))
                    continue; //skip
              }

              if (pBankTransaction->transType_ == wxT("Transfer"))
                  continue;

              double convRate = mmDBWrapper::getCurrencyBaseConvRate(db_.get(), pBankTransaction->accountID_);

              if (pBankTransaction->transType_ == wxT("Withdrawal"))
                 amt -= pBankTransaction->amt_ * convRate;
              else if (pBankTransaction->transType_ == wxT("Deposit"))
                 amt += pBankTransaction->amt_ * convRate;
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
    const wxDateTime &dtEnd
) const
{
    double amt = 0;

    for (std::vector<boost::shared_ptr<mmBankTransaction> >::const_iterator i = transactions_.begin(); 
         i != transactions_.end(); 
         i++
        )
    {
        boost::shared_ptr<const mmBankTransaction> pBankTransaction = *i;

        if (!pBankTransaction || !pBankTransaction->containsCategory(categID, subcategID)) {
            continue;
        }

        if (pBankTransaction->status_ == wxT("V"))
          continue; // skip

        if (!ignoreDate)
        {
          if (!pBankTransaction->date_.IsBetween(dtBegin, dtEnd))
             continue; //skip
        }

        if (pBankTransaction->transType_ == wxT("Transfer"))
          continue;

        double convRate = mmDBWrapper::getCurrencyBaseConvRate(db_.get(), pBankTransaction->accountID_);

        if (pBankTransaction->transType_ == wxT("Withdrawal")) {
          amt -= pBankTransaction->getAmountForSplit(categID, subcategID) * convRate;
        } else if (pBankTransaction->transType_ == wxT("Deposit")) {
          amt += pBankTransaction->getAmountForSplit(categID, subcategID) * convRate;
        }
    }
    
    return amt;
}

double mmBankTransactionList::getBalance(int accountID, bool ignoreFuture)
{
    double balance = 0.0;
    std::vector< boost::shared_ptr<mmBankTransaction> >::iterator i;
    for (i = transactions_.begin(); i != transactions_.end(); i++ )
    {
        boost::shared_ptr<mmBankTransaction> pBankTransaction = *i;
        if (pBankTransaction)
        {
             if ((pBankTransaction->accountID_ == accountID) ||
                 (pBankTransaction->toAccountID_ == accountID))
                {

                }
                else
                    continue; // skip
            
            if (pBankTransaction->status_ == wxT("V"))
                continue; // skip

            if (ignoreFuture)
            {
                if (pBankTransaction->date_.IsLaterThan(wxDateTime::Now()))
                    continue; //skip future dated transactions
            }

            if (pBankTransaction->transType_ == wxT("Deposit"))
                balance += pBankTransaction->amt_;
            else if (pBankTransaction->transType_ == wxT("Withdrawal"))
                balance -= pBankTransaction->amt_;
            else if (pBankTransaction->transType_ == wxT("Transfer"))
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

double mmBankTransactionList::getReconciledBalance(int accountID)
{
    double balance = 0.0;
    std::vector< boost::shared_ptr<mmBankTransaction> >::iterator i;
    for (i = transactions_.begin(); i != transactions_.end(); i++ )
    {
        boost::shared_ptr<mmBankTransaction> pBankTransaction = *i;
        if (pBankTransaction)
        {
             if ((pBankTransaction->accountID_ == accountID) ||
                 (pBankTransaction->toAccountID_ == accountID))
                {

                }
                else
                    continue; // skip
            
            if (pBankTransaction->status_ != wxT("R"))
                continue; // skip

            if (pBankTransaction->transType_ == wxT("Deposit"))
                balance += pBankTransaction->amt_;
            else if (pBankTransaction->transType_ == wxT("Withdrawal"))
                balance -= pBankTransaction->amt_;
            else if (pBankTransaction->transType_ == wxT("Transfer"))
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

int mmBankTransactionList::countFollowupTransactions()
{
    int numFollowup = 0;
    std::vector< boost::shared_ptr<mmBankTransaction> >::iterator i;
    for (i = transactions_.begin(); i != transactions_.end(); i++ )
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

void mmBankTransactionList::deleteTransaction(int accountID, int transactionID)
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
                    mmDBWrapper::deleteTransaction(db_.get(), transactionID);
                    return;
                }
            }
        }
    }
    // didn't find the transaction
    wxASSERT(false);
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

