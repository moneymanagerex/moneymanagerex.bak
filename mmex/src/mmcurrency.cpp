#include "mmcurrency.h"

mmCurrency::mmCurrency(boost::shared_ptr<wxSQLite3Database> db, wxSQLite3ResultSet& q1)
: db_ (db)
{
   currencyID_ = q1.GetInt(wxT("CURRENCYID"));
   currencyName_ = q1.GetString(wxT("CURRENCYNAME"));
   pfxSymbol_ = q1.GetString(wxT("PFX_SYMBOL"));
   sfxSymbol_ = q1.GetString(wxT("SFX_SYMBOL"));
   dec_ = q1.GetString(wxT("DECIMAL_POINT"));
   grp_ = q1.GetString(wxT("GROUP_SEPARATOR"));
   unit_ = q1.GetString(wxT("UNIT_NAME"));
   cent_ = q1.GetString(wxT("CENT_NAME"));
   scaleDl_ = q1.GetDouble(wxT("SCALE"));
   baseConv_ = q1.GetDouble(wxT("BASECONVRATE"), 1.0);
}

mmCurrency::mmCurrency()
{
   currencyID_ = -1;
   currencyName_ = wxT("US Dollar");
   pfxSymbol_ = wxT("$");
   sfxSymbol_ = wxT("");
   dec_ = wxT(".");
   grp_ = wxT(",");
   unit_ = wxT("dollar");
   cent_ = wxT("cent");
   scaleDl_ = 100.0;
   baseConv_ = 1.0;
}

void mmCurrencyList::loadBaseCurrencySettings()
{
   mmDBWrapper::loadBaseCurrencySettings(db_.get());
}

int mmCurrencyList::getBaseCurrencySettings()
{
   return mmDBWrapper::getBaseCurrencySettings(db_.get());
}

void mmCurrencyList::setBaseCurrencySettings(int currencyID)
{
   mmDBWrapper::setBaseCurrencySettings(db_.get(), currencyID);
}

int mmCurrencyList::addCurrency(boost::shared_ptr<mmCurrency> pCurrency)
{
    mmBEGINSQL_LITE_EXCEPTION;
    wxString bufSQLStr = wxString::Format(wxT("insert into CURRENCYFORMATS_V1 (CURRENCYNAME, PFX_SYMBOL, SFX_SYMBOL, DECIMAL_POINT,   \
                                              GROUP_SEPARATOR, UNIT_NAME, CENT_NAME, SCALE, BASECONVRATE) values ('%s', '%s', '%s', '%s', '%s', '%s', '%s', %f, %f);"), 
                                              pCurrency->currencyName_,
                                              pCurrency->pfxSymbol_, 
                                              pCurrency->sfxSymbol_, 
                                              pCurrency->dec_, pCurrency->grp_, 
                                              pCurrency->unit_, 
                                              pCurrency->cent_, 
                                              pCurrency->scaleDl_, 
                                              pCurrency->baseConv_);
                                              
    int retVal = db_->ExecuteUpdate(bufSQLStr);

    pCurrency->currencyID_ = db_->GetLastRowId().ToLong();
    currencies_.push_back(pCurrency);

    mmENDSQL_LITE_EXCEPTION;

    return pCurrency->currencyID_;
}

bool mmCurrencyList::deleteCurrency(int currencyID)
{

   return true;
}

void mmCurrencyList::updateCurrency(int currencyID, boost::shared_ptr<mmCurrency> pCurrency)
{
    mmBEGINSQL_LITE_EXCEPTION;
    wxString sqlStmt = wxString::Format(wxT("update CURRENCYFORMATS_V1 set PFX_SYMBOL='%s', SFX_SYMBOL='%s', DECIMAL_POINT='%s', \
                                            GROUP_SEPARATOR='%s', UNIT_NAME='%s', CENT_NAME='%s', SCALE='%f', BASECONVRATE='%f' where CURRENCYNAME='%s';"),
                                            pCurrency->pfxSymbol_, 
                                            pCurrency->sfxSymbol_, 
                                            pCurrency->dec_,
                                            pCurrency->grp_, 
                                            pCurrency->unit_,
                                            pCurrency->cent_, 
                                            pCurrency->scaleDl_, 
                                            pCurrency->baseConv_,
                                            pCurrency->currencyName_);

    int retVal = db_->ExecuteUpdate(sqlStmt);
    mmENDSQL_LITE_EXCEPTION;
}

bool mmCurrencyList::currencyExists(const wxString& currencyName)
{

   return true;
}

int mmCurrencyList::getCurrencyID(const wxString& currencyName)
{
   
   return -1;
}

boost::shared_ptr<mmCurrency> mmCurrencyList::getCurrencySharedPtr(int currencyID)
{
   for (int idx = 0; idx < (int)currencies_.size(); idx++)
   {
      if (currencies_[idx]->currencyID_ == currencyID)
      {
         return currencies_[idx];
      }
   }
   wxASSERT(false);
   return boost::shared_ptr<mmCurrency>();
}

boost::shared_ptr<mmCurrency> mmCurrencyList::getCurrencySharedPtr(const wxString& currencyName)
{
   for (int idx = 0; idx < (int)currencies_.size(); idx++)
   {
      if (currencies_[idx]->currencyName_ == currencyName)
      {
         return currencies_[idx];
      }
   }
   wxASSERT(false);
   return boost::shared_ptr<mmCurrency>();
}
  
