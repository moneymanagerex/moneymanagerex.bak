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
#include "dbwrapper.h"

mmCurrency::mmCurrency(wxSQLite3ResultSet& q1)
{
   currencyID_       = q1.GetInt(wxT("CURRENCYID"));
   currencyName_     = q1.GetString(wxT("CURRENCYNAME"));
   pfxSymbol_        = q1.GetString(wxT("PFX_SYMBOL"));
   sfxSymbol_        = q1.GetString(wxT("SFX_SYMBOL"));
   dec_              = q1.GetString(wxT("DECIMAL_POINT"));
   grp_              = q1.GetString(wxT("GROUP_SEPARATOR"));
   unit_             = q1.GetString(wxT("UNIT_NAME"));
   cent_             = q1.GetString(wxT("CENT_NAME"));
   scaleDl_          = q1.GetInt(wxT("SCALE"));
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
}

mmCurrency::mmCurrency()
: currencyID_(-1)
, currencyName_(wxT("US Dollar"))
, pfxSymbol_(wxT('$'))
, sfxSymbol_()
, dec_(wxT('.'))
, grp_(wxT(','))
, unit_(wxT("dollar"))
, cent_(wxT("cent"))
, scaleDl_(100)
, baseConv_(1)
, decChar_(wxT('\0'))
, grpChar_(wxT('\0'))
, currencySymbol_(wxT("USD"))
{}

void mmCurrency::loadCurrencySettings()
{
    mmex::CurrencyFormatter::instance().loadSettings(
        pfxSymbol_, 
        sfxSymbol_, 
        decChar_, 
        grpChar_, 
        unit_, 
        cent_, 
        scaleDl_
    );
}

//-----------------------------------------------------------------------------//
mmCurrencyList::mmCurrencyList(boost::shared_ptr<wxSQLite3Database> db)
: db_(db)
{}

void mmCurrencyList::LoadBaseCurrencySettings(MMEX_IniSettings* info_table) const
{
    int currencyID = info_table->GetIntSetting(wxT("BASECURRENCYID"), -1);

    if (currencyID != -1)
    {
        mmDBWrapper::loadCurrencySettings(db_.get(), currencyID);
    }
    else
    {
        mmex::CurrencyFormatter::instance().loadDefaultSettings();
    }
}

int mmCurrencyList::getBaseCurrencySettings(MMEX_IniSettings* info_table) const
{
   return info_table->GetIntSetting(wxT("BASECURRENCYID"), 1);
}

void mmCurrencyList::setBaseCurrencySettings(MMEX_IniSettings* info_table, int currencyID)
{
   info_table->SetIntSetting(wxT("BASECURRENCYID"), currencyID);
}

int mmCurrencyList::AddCurrency(boost::shared_ptr<mmCurrency> pCurrency)
{
    const mmCurrency &r = *pCurrency;
    std::vector<wxString> data;
    data.push_back(r.currencyName_);
    data.push_back(r.pfxSymbol_);
    data.push_back(r.sfxSymbol_);
    data.push_back(r.dec_);
    data.push_back(r.grp_);
    data.push_back(r.unit_);
    data.push_back(wxString()<<r.cent_);
    data.push_back(wxString()<<r.scaleDl_);
    data.push_back(wxString()<<r.baseConv_);
    data.push_back(r.currencySymbol_);

    int currencyID = mmDBWrapper::addCurrency(db_.get(), data);
    if ( currencyID > 0)
    {
        pCurrency->currencyID_ = currencyID;
        currencies_.push_back(pCurrency);
    }
    return currencyID;

}

void mmCurrencyList::updateCurrency(boost::shared_ptr<mmCurrency> pCurrency)
{
    static const char sql[] = 
    "update CURRENCYFORMATS_V1 "
    "set PFX_SYMBOL=?, SFX_SYMBOL=?, DECIMAL_POINT=?,"
        "GROUP_SEPARATOR=?, UNIT_NAME=?, CENT_NAME=?, "
        "SCALE=?, BASECONVRATE=?, CURRENCY_SYMBOL=?, "
        "CURRENCYNAME=? "
    "where CURRENCYID = ?";

    wxASSERT(pCurrency->currencyID_ > 0);

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
    st.Bind(++i, r.currencyName_);
    st.Bind(++i, r.currencyID_);

    wxASSERT(st.GetParamCount() == i);

    st.ExecuteUpdate();
    st.Finalize();
}

void mmCurrencyList::deleteCurrency(int currencyID)
{
    wxASSERT(currencyID > 0);

    if (mmDBWrapper::deleteCurrency(db_.get(), currencyID))
    {
        std::vector <boost::shared_ptr<mmCurrency> >::iterator Iter;
        for ( Iter = currencies_.begin( ) ; Iter != currencies_.end( ) ; Iter++ )
        {
            if ((*Iter)->currencyID_ == currencyID)
            {
                currencies_.erase(Iter);
                break;
            }
        }
    }
}

int mmCurrencyList::getCurrencyID(const wxString& currencyName) const
{
    int currencyID = -1;
    std::vector <boost::shared_ptr<mmCurrency> >::const_iterator Iter;
    for ( Iter = currencies_.begin( ) ; Iter != currencies_.end( ) ; Iter++ )
    {
        if ((*Iter)->currencyName_ == currencyName)
        {
            currencyID = (*Iter)->currencyID_ ;
            break;
        }
    }
   
   return currencyID;
}

wxString mmCurrencyList::getCurrencyName(int currencyID) const
{
    for(const_iterator it = currencies_.begin(); it != currencies_.end(); ++ it)
    {
        if ((*it)->currencyID_ == currencyID) return (*it)->currencyName_;
    }

    return wxEmptyString;
}

boost::shared_ptr<mmCurrency> mmCurrencyList::getCurrencySharedPtr(int currencyID) const
{
    for (size_t i = 0; i < currencies_.size(); ++i) {
        if (currencies_[i]->currencyID_ == currencyID)
            return currencies_[i];
    }

    wxASSERT(false);
    return boost::shared_ptr<mmCurrency>();
}

boost::shared_ptr<mmCurrency> mmCurrencyList::getCurrencySharedPtr(const wxString& currencyName) const
{
    for (size_t i = 0; i < currencies_.size(); ++i) {
        if (currencies_[i]->currencyName_ == currencyName)
            return currencies_[i];
    }

    wxASSERT(false);
    return boost::shared_ptr<mmCurrency>();
}
  
void mmCurrencyList::LoadCurrencies()
{
    wxSQLite3ResultSet q1 = db_->ExecuteQuery(SELECT_ALL_FROM_CURRENCYFORMATS_V1);

    while (q1.NextRow())
    {
        boost::shared_ptr<mmCurrency> pCurrency(new mmCurrency(q1));
        currencies_.push_back(pCurrency);
    }

    q1.Finalize();
}
