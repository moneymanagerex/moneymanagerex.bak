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
   if(q1.GetColumnCount() < 11) {
       /* no Currency symbol in the table yet */
       currencySymbol_ = wxEmptyString;
   } else {
       currencySymbol_ = q1.GetString(wxT("CURRENCY_SYMBOL"));
   }
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
   
   if(currencySymbol_ == wxEmptyString) {
      currencySymbol_ = wxT("");
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
   currencySymbol_ = wxT("USD");
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
    static const char sql[] = 
    "insert into CURRENCYFORMATS_V1 ( "
      "CURRENCYNAME, PFX_SYMBOL, SFX_SYMBOL, DECIMAL_POINT, "
      "GROUP_SEPARATOR, UNIT_NAME, CENT_NAME, SCALE, BASECONVRATE, CURRENCY_SYMBOL "
    " ) values ( ?, ?, ?, ?, ?, ?, ?, ?, ?, ? )";

    mmBEGINSQL_LITE_EXCEPTION;

    wxSQLite3Statement st = db_->PrepareStatement(sql);
    const mmCurrency &r = *pCurrency;

    int i = 0;
    st.Bind(++i, r.currencyName_);
    st.Bind(++i, r.pfxSymbol_);
    st.Bind(++i, r.sfxSymbol_);
    st.Bind(++i, r.dec_);
    st.Bind(++i, r.grp_);
    st.Bind(++i, r.unit_);
    st.Bind(++i, r.cent_);
    st.Bind(++i, r.scaleDl_);
    st.Bind(++i, r.baseConv_);
    st.Bind(++i, r.currencySymbol_);

    wxASSERT(st.GetParamCount() == i);
    st.ExecuteUpdate();

    pCurrency->currencyID_ = db_->GetLastRowId().ToLong();
    currencies_.push_back(pCurrency);

    st.Finalize();

    mmENDSQL_LITE_EXCEPTION;

    return pCurrency->currencyID_;
}

//bool mmCurrencyList::deleteCurrency(int currencyID)
//{
//
//   return true;
//}

void mmCurrencyList::updateCurrency(boost::shared_ptr<mmCurrency> pCurrency)
{
    static const char sql[] = 
    "update CURRENCYFORMATS_V1 "
    "set PFX_SYMBOL=?, SFX_SYMBOL=?, DECIMAL_POINT=?,"
        "GROUP_SEPARATOR=?, UNIT_NAME=?, CENT_NAME=?, "
        "SCALE=?, BASECONVRATE=?, CURRENCY_SYMBOL=? "
    "where CURRENCYID = ?";

    wxASSERT(pCurrency->currencyID_ > 0);

    mmBEGINSQL_LITE_EXCEPTION;

    wxSQLite3Statement st = db_->PrepareStatement(sql);
    const mmCurrency &r = *pCurrency;

    int i = 0;
    st.Bind(++i, r.pfxSymbol_);
    st.Bind(++i, r.sfxSymbol_);
    st.Bind(++i, r.dec_);
    st.Bind(++i, r.grp_);
    st.Bind(++i, r.unit_);
    st.Bind(++i, r.cent_);
    st.Bind(++i, r.scaleDl_);
    st.Bind(++i, r.baseConv_);
    st.Bind(++i, r.currencySymbol_);
    st.Bind(++i, r.currencyID_);

    wxASSERT(st.GetParamCount() == i);

    st.ExecuteUpdate();
    st.Finalize();

    mmENDSQL_LITE_EXCEPTION;
}

//bool mmCurrencyList::currencyExists(const wxString& currencyName)
//{
//
//   return true;
//}

//int mmCurrencyList::getCurrencyID(const wxString& currencyName)
//{
//   
//   return -1;
//}

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
  
