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
#include "mmcurrency.h"
#include "util.h"

mmCurrency::mmCurrency(boost::shared_ptr<wxSQLite3Database> db, 
                       wxSQLite3ResultSet& q1)
: db_ (db)
{
   currencyID_       = q1.GetInt(wxT("CURRENCYID"));
   currencyName_     = q1.GetString(wxT("CURRENCYNAME"));
   pfxSymbol_        = q1.GetString(wxT("PFX_SYMBOL"));
   sfxSymbol_        = q1.GetString(wxT("SFX_SYMBOL"));
   dec_              = q1.GetString(wxT("DECIMAL_POINT"));
   grp_              = q1.GetString(wxT("GROUP_SEPARATOR"));
   unit_             = q1.GetString(wxT("UNIT_NAME"));
   cent_             = q1.GetString(wxT("CENT_NAME"));
   scaleDl_          = q1.GetDouble(wxT("SCALE"));
   baseConv_         = q1.GetDouble(wxT("BASECONVRATE"), 1.0);
   decChar_ = 0;
   grpChar_ = 0;
   if (!dec_.IsEmpty())
   {
      decChar_ = dec_.GetChar(0);
   }

   if (!grp_.IsEmpty())
   {
      grpChar_ = grp_.GetChar(0);
   }
}

mmCurrency::mmCurrency()
{
   currencyID_   = -1;
   currencyName_ = wxT("US Dollar");
   pfxSymbol_    = wxT("$");
   sfxSymbol_    = wxT("");
   dec_          = wxT(".");
   grp_          = wxT(",");
   unit_         = wxT("dollar");
   cent_         = wxT("cent");
   scaleDl_      = 100.0;
   baseConv_     = 1.0;
   decChar_      = 0;
   grpChar_      = 0;
}

void mmCurrency::loadCurrencySettings()
{
   mmCurrencyFormatter::loadSettings
      (pfxSymbol_, 
       sfxSymbol_, 
       decChar_, 
       grpChar_, 
       unit_, 
       cent_, 
       scaleDl_);
}

//-----------------------------------------------------------------------------//
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
                                              pCurrency->currencyName_.c_str(),
                                              pCurrency->pfxSymbol_.c_str(), 
                                              pCurrency->sfxSymbol_.c_str(), 
                                              pCurrency->dec_.c_str(), 
					      pCurrency->grp_.c_str(), 
                                              pCurrency->unit_.c_str(), 
                                              pCurrency->cent_.c_str(), 
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
                                            pCurrency->pfxSymbol_.c_str(), 
                                            pCurrency->sfxSymbol_.c_str(), 
                                            pCurrency->dec_.c_str(),
                                            pCurrency->grp_.c_str(), 
                                            pCurrency->unit_.c_str(),
                                            pCurrency->cent_.c_str(), 
                                            pCurrency->scaleDl_, 
                                            pCurrency->baseConv_,
                                            pCurrency->currencyName_.c_str());

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
  
