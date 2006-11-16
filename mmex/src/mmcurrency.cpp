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

void mmCurrencyList::addCurrency(boost::shared_ptr<mmCurrency> pCurrency)
{

}

bool mmCurrencyList::deleteCurrency(int currencyID)
{

   return true;
}

void mmCurrencyList::updateCurrency(int currencyID, boost::shared_ptr<mmCurrency> pCurrency)
{

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
   for (int idx = 0; idx < currencies_.size(); idx++)
   {
      if (currencies_[idx]->currencyID_ == currencyID)
      {
         return currencies_[idx];
      }
   }
   wxASSERT(false);
   return boost::shared_ptr<mmCurrency>();
}
    
