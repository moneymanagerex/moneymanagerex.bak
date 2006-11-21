#include "mmtransaction.h"
#include "util.h"

mmBankTransaction::mmBankTransaction(boost::shared_ptr<wxSQLite3Database> db, 
       wxSQLite3ResultSet& q1): mmTransaction(q1.GetInt(wxT("TRANSID")))
 {
     wxString dateString = q1.GetString(wxT("TRANSDATE"));
     wxDateTime dtdt = mmGetStorageStringAsDate(dateString);
     dateStr_       = mmGetDateForDisplay(db.get(), dtdt);
     
     date_           = dtdt;
     
     //catID_          = q1.GetInt(wxT("CATEGID"));
     //subcategID_     = q1.GetInt(wxT("SUBCATEGID"));
     
     payeeID_        = q1.GetInt(wxT("PAYEEID"));
     transNum_    = q1.GetString(wxT("TRANSACTIONNUMBER"));
     status_         = q1.GetString(wxT("STATUS"));
     notes_          = q1.GetString(wxT("NOTES"));
     transType_      = q1.GetString(wxT("TRANSCODE"));
     accountID_      = q1.GetInt(wxT("ACCOUNTID"));
     toAccountID_    = q1.GetInt(wxT("TOACCOUNTID"));

     amt_            = q1.GetDouble(wxT("TRANSAMOUNT"));
     toAmt_          = q1.GetDouble(wxT("TOTRANSAMOUNT"));

     
     wxString displayTransAmtString;
     if (mmCurrencyFormatter::formatDoubleToCurrencyEdit(th.amt_, displayTransAmtString))
         transAmtString_ = displayTransAmtString;

     wxString displayToTransAmtString;
     if (mmCurrencyFormatter::formatDoubleToCurrencyEdit(th.toAmt_, displayToTransAmtString))
         transToAmtString_ = displayToTransAmtString;

     catStr_ = mmDBWrapper::getCategoryName(db_, th.catID_);
     subCatStr_ = mmDBWrapper::getSubCategoryName(db_, th.catID_, th.subcategID_);
     
     int cid = 0, sid = 0;
     th.payeeStr_ = mmDBWrapper::getPayee(db_, th.payeeID_, cid, sid);

  
 }

double mmTransferTransaction::value(int accountCtx)
{
    double balance = 0.0;
    if (accountID_ == accountCtx)
    {
        balance -= amt_;
    }
    else
    {
        wxASSERT(toAccountID_ == accountCtx);
        balance += toAmt_;
    }
    return balance;
}