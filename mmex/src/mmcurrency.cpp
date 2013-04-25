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
#include <map>

mmCurrency::mmCurrency(wxSQLite3ResultSet& q1)
{
   currencyID_       = q1.GetInt("CURRENCYID");
   currencyName_     = q1.GetString("CURRENCYNAME");
   pfxSymbol_        = q1.GetString("PFX_SYMBOL");
   sfxSymbol_        = q1.GetString("SFX_SYMBOL");
   dec_              = q1.GetString("DECIMAL_POINT");
   grp_              = q1.GetString("GROUP_SEPARATOR");
   unit_             = q1.GetString("UNIT_NAME");
   cent_             = q1.GetString("CENT_NAME");
   scaleDl_          = q1.GetInt("SCALE");
   baseConv_         = q1.GetDouble("BASECONVRATE", 1.0);
   if(q1.GetColumnCount() < 11) {
       /* no Currency symbol in the table yet */
       currencySymbol_ = wxEmptyString;
   } else {
       currencySymbol_ = q1.GetString("CURRENCY_SYMBOL");
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
, currencyName_("US Dollar")
, pfxSymbol_('$')
, sfxSymbol_()
, dec_('.')
, grp_(',')
, unit_("dollar")
, cent_("cent")
, scaleDl_(100)
, baseConv_(1)
, decChar_('\0')
, grpChar_('\0')
, currencySymbol_("USD")
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
mmCurrencyList::mmCurrencyList(wxSharedPtr<wxSQLite3Database> db)
: db_(db)
, info_table_()
{}

void mmCurrencyList::SetInfoTable(wxSharedPtr<MMEX_IniSettings> info_table)
{
    info_table_ = info_table;
}

void mmCurrencyList::LoadBaseCurrencySettings() const
{
    wxASSERT(info_table_);
    int currencyID = info_table_->GetIntSetting("BASECURRENCYID", -1);

    if (currencyID != -1)
    {
        SetCurrencySetting(getCurrencySharedPtr(currencyID));
    }
    else
    {
        mmex::CurrencyFormatter::instance().loadDefaultSettings();
    }
}

void mmCurrencyList::LoadCurrencySetting(const wxString& currencySymbol)
{
    SetCurrencySetting(getCurrencySharedPtr(currencySymbol, true));
}

void mmCurrencyList::SetCurrencySetting(wxSharedPtr<mmCurrency> pCurrency) const
{
    if (pCurrency)
    {
        mmex::CurrencyFormatter::instance().loadSettings(
            pCurrency->pfxSymbol_, pCurrency->sfxSymbol_, pCurrency->decChar_,
            pCurrency->grpChar_, pCurrency->unit_, pCurrency->cent_, pCurrency->scaleDl_);
    }
    else
    {
        mmex::CurrencyFormatter::instance().loadDefaultSettings();
    }
}

int mmCurrencyList::GetBaseCurrencySettings() const
{
    wxASSERT(info_table_);
    int iBaseCurrencyID = info_table_->GetIntSetting("BASECURRENCYID", -1);
    wxASSERT(iBaseCurrencyID > 0);
    return iBaseCurrencyID;

}

void mmCurrencyList::SetBaseCurrencySettings(int currencyID)
{
    wxASSERT(info_table_);
    info_table_->SetIntSetting("BASECURRENCYID", currencyID);
}

int mmCurrencyList::AddCurrency(wxSharedPtr<mmCurrency> pCurrency)
{
    const mmCurrency &r = *pCurrency;
    std::vector<wxString> data;
    data.push_back(r.currencyName_);
    data.push_back(r.pfxSymbol_);
    data.push_back(r.sfxSymbol_);
    data.push_back(r.dec_);
    data.push_back(r.grp_);
    data.push_back(r.unit_);
    data.push_back(r.cent_);
    data.push_back(wxString()<<r.scaleDl_);
    data.push_back(wxString()<<r.baseConv_);
    data.push_back(r.currencySymbol_);

    long currencyID;
    wxString sql = wxString::FromUTF8(INSERT_INTO_CURRENCYFORMATS_V1);
    int iError = mmDBWrapper::mmSQLiteExecuteUpdate(db_.get(), data, sql, currencyID);
    if ( iError == 0 && currencyID > 0)
    {
        pCurrency->currencyID_ = currencyID;
        currencies_.push_back(pCurrency);
    }
    return currencyID;
}

void mmCurrencyList::UpdateCurrency(wxSharedPtr<mmCurrency> pCurrency)
{
    wxASSERT(pCurrency->currencyID_ > 0);

    wxSQLite3Statement st = db_->PrepareStatement(UPDATE_CURRENCYFORMATS_V1);
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

void mmCurrencyList::DeleteCurrency(int currencyID)
{
    wxASSERT(currencyID > 0);

    if (mmDBWrapper::deleteCurrency(db_.get(), currencyID))
    {
        std::vector <wxSharedPtr<mmCurrency> >::iterator Iter;
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

int mmCurrencyList::getCurrencyID(const wxString& currencyName, bool symbol) const
{
    int currencyID = -1;
    wxSharedPtr<mmCurrency> pCurrency = getCurrencySharedPtr(currencyName, symbol);

    if (pCurrency)
    {
        currencyID = pCurrency->currencyID_;
    }
   
   return currencyID;
}

wxString mmCurrencyList::getCurrencyName(int currencyID, bool symbol) const
{
    for(const_iterator it = currencies_.begin(); it != currencies_.end(); ++ it)
    {
        if ((*it)->currencyID_ == currencyID)
        {
            return symbol ? (*it)->currencySymbol_ : (*it)->currencyName_;
        }
    }

    return wxEmptyString;
}

wxSharedPtr<mmCurrency> mmCurrencyList::getCurrencySharedPtr(int currencyID) const
{
    for (size_t i = 0; i < currencies_.size(); ++i)
    {
        if (currencies_[i]->currencyID_ == currencyID)
            return currencies_[i];
    }

    wxASSERT(false);
    return wxSharedPtr<mmCurrency>();
}

wxSharedPtr<mmCurrency> mmCurrencyList::getCurrencySharedPtr(const wxString& currencyName, bool symbol) const
{
    for (size_t i = 0; i < currencies_.size(); ++i)
    {
        if (symbol)
        {
            if (currencies_[i]->currencySymbol_ == currencyName)
            return currencies_[i];
        }
        else
        {
            if (currencies_[i]->currencyName_ == currencyName)
            return currencies_[i];
        }
    }

    return wxSharedPtr<mmCurrency>();
}
  
void mmCurrencyList::LoadCurrencies()
{
    wxSQLite3ResultSet q1 = db_->ExecuteQuery(SELECT_ALL_FROM_CURRENCYFORMATS_V1);

    while (q1.NextRow())
    {
        wxSharedPtr<mmCurrency> pCurrency(new mmCurrency(q1));
        currencies_.push_back(pCurrency);
    }

    q1.Finalize();
}

bool mmCurrencyList::OnlineUpdateCurRate(wxString& sError)
{
    LoadBaseCurrencySettings();
    int currencyID = GetBaseCurrencySettings();

    if(currencyID == -1)
    {
        sError = _("Could not find base currency symbol!");
        return false;
    }
    wxString base_symbol = getCurrencySharedPtr(currencyID)->currencySymbol_;

    wxString site;
    for (int idx = 0; idx < (int)currencies_.size(); idx++)
    {
        const wxString symbol = currencies_[idx]->currencySymbol_.Upper();

        site << symbol << base_symbol << "=X+";
    }
    if (site.Right(1).Contains("+")) site.RemoveLast(1);
    site = wxString::Format("http://download.finance.yahoo.com/d/quotes.csv?s=%s&f=sl1n&e=.csv", site);

    wxString sOutput;
    int err_code = site_content(site, sOutput);
    if (err_code != wxURL_NOERR)
    {
        sError = sOutput;
        return false;
    }

    wxString CurrencySymbol, dName;
    double dRate = 1;

    std::map<wxString, std::pair<double, wxString> > currency_data;

    // Break it up into lines
    wxStringTokenizer tkz(sOutput, "\r\n");

    while (tkz.HasMoreTokens())
    {
        wxString csvline = tkz.GetNextToken();

        wxStringTokenizer csvsimple(csvline, "\",",wxTOKEN_STRTOK);
        if (csvsimple.HasMoreTokens())
        {
            CurrencySymbol = csvsimple.GetNextToken();
            if (csvsimple.HasMoreTokens())
            {
                csvsimple.GetNextToken().ToDouble(&dRate);
                if (csvsimple.HasMoreTokens())
                    dName = csvsimple.GetNextToken();
            }
        }
        currency_data.insert(std::make_pair(CurrencySymbol, std::make_pair(dRate, dName)));
    }

    wxString msg = _("Currency rate updated");
    msg << "\n\n";

    db_->Begin();

    for (int idx = 0; idx < (int)currencies_.size(); idx++)
    {
        const wxString currency_symbol = currencies_[idx]->currencySymbol_.Upper();
        if (!currency_symbol.IsEmpty())
        {
            wxString currency_symbols_pair = currency_symbol + base_symbol + "=X";
            std::pair<double, wxString> data = currency_data[currency_symbols_pair];

            wxString valueStr, newValueStr;
            double new_rate = data.first;
            if (base_symbol == currency_symbol) new_rate = 1;

            double old_rate = currencies_[idx]->baseConv_;
            //mmex::formatDoubleToCurrencyEdit(old_rate, valueStr);
            //mmex::formatDoubleToCurrencyEdit(new_rate, newValueStr);
            valueStr = wxString::Format("%0.4f", old_rate);
            newValueStr = wxString::Format("%0.4f", new_rate);
            msg << wxString::Format(_("%s\t: %s -> %s\n"),
                currency_symbol, valueStr, newValueStr);
            currencies_[idx]->baseConv_ = new_rate;
            UpdateCurrency(currencies_[idx]);
        }
    }

    db_->Commit();
    sError = msg;
    return true;
}
