#include "mmtransaction.h"
#include "util.h"
#include "mmcoredb.h"

mmBankTransaction::mmBankTransaction(mmCoreDB* core, wxSQLite3ResultSet& q1)
      : mmTransaction(q1.GetInt(wxT("TRANSID"))),
      db_(core->db_), isInited_(false)
 {
     wxString dateString = q1.GetString(wxT("TRANSDATE"));
     date_               = mmGetStorageStringAsDate(dateString);
     
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

     updateAllData(core, accountID_);
 }

void mmBankTransaction::updateAllData(mmCoreDB* core, int accountID)
{
    if ((isInited_) && (transType_ != wxT("Transfer")))
    {
       return;
    }

     dateStr_            = mmGetDateForDisplay(db_.get(), date_);

     boost::shared_ptr<mmCategory> pCategory = category_.lock();
     wxASSERT(pCategory);
     boost::shared_ptr<mmCategory> parent = pCategory->parent_.lock();
     if (parent)
     {
        catStr_ = parent->categName_;
        subCatStr_ = pCategory->categName_;
        categID_ = parent->categID_;
        subcategID_ = pCategory->categID_;
     }
     else
     {
        catStr_ = pCategory->categName_;
        subCatStr_ = wxT("");
        categID_ = pCategory->categID_;
        subcategID_ = -1;
     }
     
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
           payeeStr_ = toAccount;
        }
        else if (toAccountID_ == accountID)
        {
           depositStr_ = displayToTransAmtString;
           payeeStr_ = fromAccount;
        }
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

int mmBankTransactionList::addTransaction(boost::shared_ptr<mmBankTransaction> pBankTransaction)
{
   mmBEGINSQL_LITE_EXCEPTION;
   
   wxString bufSQL = wxString::Format(wxT("insert into CHECKINGACCOUNT_V1 (ACCOUNTID, TOACCOUNTID, PAYEEID, TRANSCODE, \
                                          TRANSAMOUNT, STATUS, TRANSACTIONNUMBER, NOTES,                               \
                                          CATEGID, SUBCATEGID, TRANSDATE, FOLLOWUPID, TOTRANSAMOUNT)                                              \
                                          values (%d, %d, %d, '%s', %f, '%s', '%s', '%s', %d, %d, '%s', -1, %f);"),
                                          pBankTransaction->accountID_, 
                                          pBankTransaction->toAccountID_, 
                                          pBankTransaction->payeeID_, 
                                          pBankTransaction->transType_, 
                                          pBankTransaction->amt_,
                                          pBankTransaction->status_, 
                                          pBankTransaction->transNum_, 
                                          mmCleanString(pBankTransaction->notes_.c_str()), 
                                          pBankTransaction->categID_, 
                                          pBankTransaction->subcategID_, 
                                          pBankTransaction->date_.FormatISODate(), 
                                          pBankTransaction->toAmt_ );  

   int retVal = db_->ExecuteUpdate(bufSQL);

   pBankTransaction->transactionID(db_->GetLastRowId().ToLong());
   transactions_.push_back(pBankTransaction);

   mmENDSQL_LITE_EXCEPTION;

   return pBankTransaction->transactionID();
}

void mmBankTransactionList::updateTransaction(boost::shared_ptr<mmBankTransaction> pBankTransaction)
{
   mmBEGINSQL_LITE_EXCEPTION;
   
   wxString bufSQL = wxString::Format(wxT("update CHECKINGACCOUNT_V1 SET ACCOUNTID=%d, TOACCOUNTID=%d, PAYEEID=%d, TRANSCODE='%s', \
                                          TRANSAMOUNT=%f, STATUS='%s', TRANSACTIONNUMBER='%s', NOTES='%s',                               \
                                          CATEGID=%d, SUBCATEGID=%d, TRANSDATE='%s', TOTRANSAMOUNT=%f WHERE TRANSID=%d;"),
                                          pBankTransaction->accountID_, 
                                          pBankTransaction->toAccountID_, 
                                          pBankTransaction->payeeID_, 
                                          pBankTransaction->transType_, 
                                          pBankTransaction->amt_,
                                          pBankTransaction->status_, 
                                          pBankTransaction->transNum_, 
                                          mmCleanString(pBankTransaction->notes_.c_str()), 
                                          pBankTransaction->categID_, 
                                          pBankTransaction->subcategID_, 
                                          pBankTransaction->date_.FormatISODate(), 
                                          pBankTransaction->toAmt_, 
                                          pBankTransaction->transactionID());  

   int retVal = db_->ExecuteUpdate(bufSQL);

    mmENDSQL_LITE_EXCEPTION;
}


boost::shared_ptr<mmBankTransaction> mmBankTransactionList::getBankTransactionPtr(int accountID, int transactionID)
{
    std::vector< boost::shared_ptr<mmBankTransaction> >::iterator i;
    for (i = transactions_.begin(); i!= transactions_.end(); )
    {
        boost::shared_ptr<mmBankTransaction> pBankTransaction = *i;
        if (pBankTransaction)
        {
            if (((pBankTransaction->accountID_ == accountID) ||
               (pBankTransaction->toAccountID_ == accountID)) && (pBankTransaction->transactionID() == transactionID))
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


void mmBankTransactionList::deleteTransaction(int accountID, int transactionID)
{
    std::vector< boost::shared_ptr<mmBankTransaction> >::iterator i;
    for (i = transactions_.begin(); i!= transactions_.end(); )
    {
       boost::shared_ptr<mmBankTransaction> pBankTransaction = *i;
        if (pBankTransaction)
        {
            if ((pBankTransaction->accountID_ == accountID) ||
               (pBankTransaction->toAccountID_) && pBankTransaction->transactionID() == transactionID)
            {
                i = transactions_.erase(i);
                mmDBWrapper::deleteTransaction(db_.get(), transactionID);
                return;
            }
            else
                ++i;
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
                (pBankTransaction->toAccountID_))
            {
                i = transactions_.erase(i);
            }
            else
                ++i;
        }
    }
}
