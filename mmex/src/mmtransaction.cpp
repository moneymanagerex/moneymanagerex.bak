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

#include "mmtransaction.h"
#include "util.h"
#include "mmcoredb.h"

void mmSplitTransactionEntries::addSplit(boost::shared_ptr<mmSplitTransactionEntry>& split)
{
    total_ += split->splitAmount_;
    entries_.push_back(split);
}

void mmSplitTransactionEntries::removeSplit(int splitID)
{
    for (size_t idx = 0; idx < entries_.size(); idx++)
    {
        if (entries_[idx]->splitEntryID_ == splitID)
        {
            entries_.erase(entries_.begin() + idx);
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
    mmBEGINSQL_LITE_EXCEPTION;

    if (edit)
    {
        wxSQLite3StatementBuffer bufSQL;
        bufSQL.Format("delete from SPLITTRANSACTIONS_V1 where TRANSID = %d;", transID);
        db->ExecuteUpdate(bufSQL);
    }

    for (int idx = 0; idx < numEntries(); idx++)
    {
        wxString bufSQL = wxString::Format(wxT("insert into SPLITTRANSACTIONS_V1 (TRANSID, CATEGID, SUBCATEGID, SPLITTRANSAMOUNT) \
                                               values (%d, %d, %d, %f);"), transID, entries_[idx]->categID_,
                                               entries_[idx]->subCategID_,  entries_[idx]->splitAmount_);  
        int retVal = db->ExecuteUpdate(bufSQL);
        entries_[idx]->splitEntryID_ = db->GetLastRowId().ToLong();
    }

    mmENDSQL_LITE_EXCEPTION;
}

void mmSplitTransactionEntries::loadFromBDDB(mmCoreDB* core,
										    int bdID)
{
   mmBEGINSQL_LITE_EXCEPTION;

   entries_.clear();
   total_ = 0.0;

   wxSQLite3StatementBuffer bufSQL;
   bufSQL.Format("select * from BUDGETSPLITTRANSACTIONS_V1 where TRANSID = %d;", bdID);
   wxSQLite3ResultSet q1 = core->db_->ExecuteQuery(bufSQL);
   while (q1.NextRow())
   {
      boost::shared_ptr<mmSplitTransactionEntry> pSplitEntry(new mmSplitTransactionEntry());
      pSplitEntry->splitEntryID_ = q1.GetInt(wxT("SPLITTRANSID"));
      pSplitEntry->splitAmount_  = q1.GetDouble(wxT("SPLITTRANSAMOUNT"));

      pSplitEntry->categID_      = q1.GetInt(wxT("CATEGID"));
      pSplitEntry->subCategID_   = q1.GetInt(wxT("SUBCATEGID"));
      pSplitEntry->category_      = core->categoryList_.getCategorySharedPtr(q1.GetInt(wxT("CATEGID")), 
         q1.GetInt(wxT("SUBCATEGID")));
      wxASSERT(pSplitEntry->category_.lock());

      addSplit(pSplitEntry);
   }
   q1.Finalize();

    mmENDSQL_LITE_EXCEPTION;
}
//-----------------------------------------------------------------------------//
mmBankTransaction::mmBankTransaction(boost::shared_ptr<wxSQLite3Database> db)
: mmTransaction(-1),
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
    mmBEGINSQL_LITE_EXCEPTION;

    splits->entries_.clear();
    splits->total_ = 0.0;

    wxSQLite3StatementBuffer bufSQL;
    bufSQL.Format("select * from SPLITTRANSACTIONS_V1 where TRANSID = %d;", transactionID());
    wxSQLite3ResultSet q1 = db_->ExecuteQuery(bufSQL);
    while (q1.NextRow())
    {
        boost::shared_ptr<mmSplitTransactionEntry> pSplitEntry(new mmSplitTransactionEntry());
        pSplitEntry->splitEntryID_ = q1.GetInt(wxT("SPLITTRANSID"));
        pSplitEntry->splitAmount_  = q1.GetDouble(wxT("SPLITTRANSAMOUNT"));

        pSplitEntry->categID_      = q1.GetInt(wxT("CATEGID"));
        pSplitEntry->subCategID_   = q1.GetInt(wxT("SUBCATEGID"));
        pSplitEntry->category_      = core->categoryList_.getCategorySharedPtr(q1.GetInt(wxT("CATEGID")), 
                                                                               q1.GetInt(wxT("SUBCATEGID")));
        wxASSERT(pSplitEntry->category_.lock());

        splits->addSplit(pSplitEntry);
    }
    q1.Finalize();

    mmENDSQL_LITE_EXCEPTION;
}

bool mmBankTransaction::containsCategory(int categID, int subcategID)
{
    if (splitEntries_->numEntries() > 0)
    {
        for(int idx = 0; idx < splitEntries_->numEntries(); idx++)
        {
            if ((splitEntries_->entries_[idx]->categID_ == categID) &&
                (splitEntries_->entries_[idx]->subCategID_ == subcategID))
            {
                return true;
            }
        }
    }
    else if ((categID_ == categID) &&
            (subcategID_ == subcategID))
    {
        return true;

    }
    return false;
}
//-----------------------------------------------------------------------------//
mmBankTransactionList::mmBankTransactionList(boost::shared_ptr<wxSQLite3Database> db)
: db_(db)
{ 
   /* Allocate some empty space so loading transactions is faster */
   transactions_.reserve(5000);
}

int mmBankTransactionList::addTransaction(boost::shared_ptr<mmBankTransaction> pBankTransaction)
{
   mmBEGINSQL_LITE_EXCEPTION;
   
   if (checkForExistingTransaction(pBankTransaction)){
	   pBankTransaction->status_ = wxT("D");
   }

   wxString bufSQL = wxString::Format(wxT("insert into CHECKINGACCOUNT_V1 (ACCOUNTID, TOACCOUNTID, PAYEEID, TRANSCODE, \
                                          TRANSAMOUNT, STATUS, TRANSACTIONNUMBER, NOTES,                               \
                                          CATEGID, SUBCATEGID, TRANSDATE, FOLLOWUPID, TOTRANSAMOUNT)                                              \
                                          values (%d, %d, %d, '%s', %f, '%s', '%s', '%s', %d, %d, '%s', -1, %f);"),
                                          pBankTransaction->accountID_, 
                                          pBankTransaction->toAccountID_, 
                                          pBankTransaction->payeeID_, 
                                          pBankTransaction->transType_.c_str(), 
                                          pBankTransaction->amt_,
                                          pBankTransaction->status_.c_str(), 
                                          pBankTransaction->transNum_.c_str(), 
                                          pBankTransaction->notes_.c_str(), 
                                          pBankTransaction->categID_, 
                                          pBankTransaction->subcategID_, 
                                          pBankTransaction->date_.FormatISODate().c_str(), 
                                          pBankTransaction->toAmt_ );  

   int retVal = db_->ExecuteUpdate(bufSQL);

   pBankTransaction->transactionID(db_->GetLastRowId().ToLong());
   pBankTransaction->splitEntries_->updateToDB(db_, pBankTransaction->transactionID(), false);

   transactions_.push_back(pBankTransaction);

   mmENDSQL_LITE_EXCEPTION;

   return pBankTransaction->transactionID();
}

bool mmBankTransactionList::checkForExistingTransaction(boost::shared_ptr<mmBankTransaction> pBankTransaction)
{
   mmBEGINSQL_LITE_EXCEPTION;
   
   wxString bufSQL = wxString::Format(wxT("select TRANSID from CHECKINGACCOUNT_V1 where \
											ACCOUNTID = %d and TOACCOUNTID = %d and PAYEEID = %d and TRANSCODE = '%s' and \
											TRANSAMOUNT = %f and TRANSACTIONNUMBER = '%s' and NOTES = '%s' and \
											CATEGID = %d and SUBCATEGID = %d and TRANSDATE = '%s' and TOTRANSAMOUNT = %f"),
                                          pBankTransaction->accountID_, 
                                          pBankTransaction->toAccountID_, 
                                          pBankTransaction->payeeID_, 
                                          pBankTransaction->transType_.c_str(), 
                                          pBankTransaction->amt_,
                                          pBankTransaction->transNum_.c_str(), 
                                          pBankTransaction->notes_.c_str(), 
                                          pBankTransaction->categID_, 
                                          pBankTransaction->subcategID_, 
                                          pBankTransaction->date_.FormatISODate().c_str(), 
                                          pBankTransaction->toAmt_ );  

   wxSQLite3ResultSet q1 = db_->ExecuteQuery(bufSQL);

   if (q1.NextRow())
   {
       if ( q1.GetString(wxT("TRANSID")).Cmp(wxT("0")) > 0)
       {
           // TODO: Need to check split entries
           return true;
       }
       else
       {
           return false;
       }
   }
	
	mmENDSQL_LITE_EXCEPTION;

	return false;
}

boost::shared_ptr<mmBankTransaction> mmBankTransactionList::copyTransaction(int transactionID, bool useOriginalDate)
{
   boost::shared_ptr<mmBankTransaction> pBankTransaction = getBankTransactionPtr(transactionID);
   if (!pBankTransaction)
       return boost::shared_ptr<mmBankTransaction>();

   boost::shared_ptr<mmBankTransaction> pCopyTransaction(new mmBankTransaction(db_));
   mmBEGINSQL_LITE_EXCEPTION;
   
   pCopyTransaction->accountID_ = pBankTransaction->accountID_; 
   pCopyTransaction->toAccountID_ = pBankTransaction->toAccountID_;
   pCopyTransaction->payee_       = pBankTransaction->payee_;
   pCopyTransaction->payeeID_=    pBankTransaction->payeeID_;
   pCopyTransaction->transType_=    pBankTransaction->transType_;
   pCopyTransaction->amt_ =   pBankTransaction->amt_;
   pCopyTransaction->status_ =    pBankTransaction->status_;
   pCopyTransaction->transNum_=   pBankTransaction->transNum_;
   pCopyTransaction->notes_=   pBankTransaction->notes_;
   pCopyTransaction->categID_  =   pBankTransaction->categID_;
   pCopyTransaction->subcategID_ =   pBankTransaction->subcategID_;
   pCopyTransaction->date_=    (useOriginalDate ? pBankTransaction->date_ : wxDateTime::Now()); 
   pCopyTransaction->toAmt_=   pBankTransaction->toAmt_;
   pCopyTransaction->category_ = pBankTransaction->category_;
   *pCopyTransaction->splitEntries_.get() = *pBankTransaction->splitEntries_.get();

   wxString bufSQL = wxString::Format(wxT("insert into CHECKINGACCOUNT_V1 (ACCOUNTID, TOACCOUNTID, PAYEEID, TRANSCODE, \
                                          TRANSAMOUNT, STATUS, TRANSACTIONNUMBER, NOTES,                               \
                                          CATEGID, SUBCATEGID, TRANSDATE, FOLLOWUPID, TOTRANSAMOUNT)                                              \
                                          values (%d, %d, %d, '%s', %f, '%s', '%s', '%s', %d, %d, '%s', -1, %f);"),
                                          pBankTransaction->accountID_, 
                                          pBankTransaction->toAccountID_, 
                                          pBankTransaction->payeeID_, 
                                          pBankTransaction->transType_.c_str(), 
                                          pBankTransaction->amt_,
                                          pBankTransaction->status_.c_str(), 
                                          pBankTransaction->transNum_.c_str(), 
                                          pBankTransaction->notes_.c_str(), 
                                          pBankTransaction->categID_, 
                                          pBankTransaction->subcategID_, 
                                          pCopyTransaction->date_.FormatISODate().c_str(), 
                                          pBankTransaction->toAmt_ );  

   int retVal = db_->ExecuteUpdate(bufSQL);

   pCopyTransaction->transactionID(db_->GetLastRowId().ToLong());
   pCopyTransaction->splitEntries_->updateToDB(db_, pCopyTransaction->transactionID(), false);

   transactions_.push_back(pCopyTransaction);

   mmENDSQL_LITE_EXCEPTION;

   return pCopyTransaction;
}

void mmBankTransactionList::updateTransaction(
   boost::shared_ptr<mmBankTransaction> pBankTransaction)
{
   mmBEGINSQL_LITE_EXCEPTION;
   
   wxString bufSQL = wxString::Format(wxT("update CHECKINGACCOUNT_V1 SET ACCOUNTID=%d, TOACCOUNTID=%d, PAYEEID=%d, TRANSCODE='%s', \
                                          TRANSAMOUNT=%f, STATUS='%s', TRANSACTIONNUMBER='%s', NOTES='%s',                               \
                                          CATEGID=%d, SUBCATEGID=%d, TRANSDATE='%s', TOTRANSAMOUNT=%f WHERE TRANSID=%d;"),
                                          pBankTransaction->accountID_, 
                                          pBankTransaction->toAccountID_, 
                                          pBankTransaction->payeeID_, 
                                          pBankTransaction->transType_.c_str(), 
                                          pBankTransaction->amt_,
                                          pBankTransaction->status_.c_str(), 
                                          pBankTransaction->transNum_.c_str(), 
                                          pBankTransaction->notes_.c_str(), 
                                          pBankTransaction->categID_, 
                                          pBankTransaction->subcategID_, 
                                          pBankTransaction->date_.FormatISODate().c_str(), 
                                          pBankTransaction->toAmt_, 
                                          pBankTransaction->transactionID());  

   int retVal = db_->ExecuteUpdate(bufSQL);
   pBankTransaction->splitEntries_->updateToDB(db_, pBankTransaction->transactionID(), true);

    mmENDSQL_LITE_EXCEPTION;
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
                           bool ignoreDate, wxDateTime dtBegin, wxDateTime dtEnd)
{
    std::vector< boost::shared_ptr<mmBankTransaction> >::iterator i;
    for (i = transactions_.begin(); i != transactions_.end(); i++ )
    {
        boost::shared_ptr<mmBankTransaction> pBankTransaction = *i;
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
                           bool ignoreDate, wxDateTime dtBegin, wxDateTime dtEnd)
{
    std::vector< boost::shared_ptr<mmBankTransaction> >::iterator i;
    for (i = transactions_.begin(); i != transactions_.end(); i++ )
    {
        boost::shared_ptr<mmBankTransaction> pBankTransaction = *i;
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
                number++;
            else if (pBankTransaction->transType_ == wxT("Withdrawal"))
                number++;
            else if (pBankTransaction->transType_ == wxT("Transfer"))
            {
                number++;
            }

        }
    }
}

double mmBankTransactionList::getAmountForPayee(int payeeID, bool ignoreDate, 
                                 wxDateTime dtBegin, wxDateTime dtEnd)
{
    double amt = 0.0;
    std::vector< boost::shared_ptr<mmBankTransaction> >::iterator i;
    for (i = transactions_.begin(); i != transactions_.end(); i++ )
    {
        boost::shared_ptr<mmBankTransaction> pBankTransaction = *i;
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
                                         wxDateTime dtBegin,
                                         wxDateTime dtEnd)
{
    double amt = 0.0;
    std::vector< boost::shared_ptr<mmBankTransaction> >::iterator i;
    for (i = transactions_.begin(); i != transactions_.end(); i++ )
    {
        boost::shared_ptr<mmBankTransaction> pBankTransaction = *i;
        if (pBankTransaction)
        {
            if (pBankTransaction->containsCategory(categID, subcategID))
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
                    balance += pBankTransaction->amt_;
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

