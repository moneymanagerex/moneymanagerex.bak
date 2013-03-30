/*******************************************************
 Copyright (C) 2013 Stefano Giorgio

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

#include "assets.h"

/************************************************************************************
 TAssetEntry Methods
 ***********************************************************************************/
/// Constructor used when loading assets from the database
TAssetEntry::TAssetEntry(wxSQLite3ResultSet& q1)
: TEntryBase()
{
    id_         = q1.GetInt(wxT("ASSETID"));
    date_       = q1.GetString(wxT("STARTDATE"));
    name_       = q1.GetString(wxT("ASSETNAME"));
    value_      = q1.GetDouble(wxT("VALUE"));
    rate_type_  = q1.GetString(wxT("VALUECHANGE"));
    notes_      = q1.GetString(wxT("NOTES"));
    rate_value_ = q1.GetDouble(wxT("VALUECHANGERATE"));
    type_       = q1.GetString(wxT("ASSETTYPE"));
    // calculated value
    display_date_ = mmGetDateForDisplay(mmGetStorageStringAsDate(date_));
}

/// Copy constructor using a pointer
TAssetEntry::TAssetEntry(TAssetEntry* pEntry)
: TEntryBase()
{
    date_       = pEntry->date_;
    name_       = pEntry->name_;
    value_      = pEntry->value_;
    rate_type_  = pEntry->rate_type_;
    notes_      = pEntry->notes_;
    rate_value_ = pEntry->rate_value_;
    type_       = pEntry->type_;
}

void TAssetEntry::SetDatabaseValues(wxSQLite3Statement& st, int& db_index)
{
    st.Bind(++db_index, date_);      // q1.GetString(wxT("STARTDATE"));
    st.Bind(++db_index, name_);      // q1.GetString(wxT("ASSETNAME"));
    st.Bind(++db_index, value_);     // q1.GetDouble(wxT("VALUE"));
    st.Bind(++db_index, rate_type_); // q1.GetString(wxT("VALUECHANGE"));
    st.Bind(++db_index, notes_);     // q1.GetString(wxT("NOTES"));
    st.Bind(++db_index, rate_value_);// q1.GetDouble(wxT("VALUECHANGERATE"));
    st.Bind(++db_index, type_);      // q1.GetString(wxT("ASSETTYPE"));
}

/// Constructor for creating a new asset entry.
TAssetEntry::TAssetEntry()
: TEntryBase()
, value_(0.0)
, rate_value_(0.0)
{}

int TAssetEntry::Add(wxSQLite3Database* db)
{
    const char INSERT_INTO_ASSETS_V1[] =
    "INSERT INTO ASSETS_V1 "
    "(STARTDATE, ASSETNAME, VALUE, VALUECHANGE, NOTES, VALUECHANGERATE, ASSETTYPE) "
    "values (?, ?, ?, ?, ?, ?, ?)";

    wxSQLite3Statement st = db->PrepareStatement(INSERT_INTO_ASSETS_V1);
    int db_index = 0;
    SetDatabaseValues(st, db_index);
    FinaliseAdd(db, st);

    return id_;
}

void TAssetEntry::Delete(wxSQLite3Database* db)
{
    DeleteEntry(db, wxT("delete from ASSETS_V1 where ASSETID = ?")); 
}

void TAssetEntry::Update(wxSQLite3Database* db)
{
    const char UPDATE_ASSETS_V1[] =
    "update ASSETS_V1 set"
    " STARTDATE = ?, ASSETNAME = ?, VALUE = ?, "
    " VALUECHANGE = ?, NOTES = ?, VALUECHANGERATE = ?, ASSETTYPE = ? "
    "where ASSETID = ?";
    try
    {
        wxSQLite3Statement st = db->PrepareStatement(UPDATE_ASSETS_V1);
        
        int db_index = 0;
        SetDatabaseValues(st, db_index);
        st.Bind(++db_index, id_);

        FinaliseStatement(st);
    }
    catch(const wxSQLite3Exception& e)
    {
        //wxLogDebug(wxT("TAssetEntry:Update: %s"), e.GetMessage());
        wxLogError(wxT("TAssetEntry:Update: %s"), e.GetMessage().c_str());
    }
}

double TAssetEntry::GetAppreciatedValue(const wxDateTime& startDate, double value, double rate)
{
    double asset_value = 0.0;
    wxDateTime todayDate = wxDateTime::Now();

    if (todayDate > startDate)
    {
        int numYears = todayDate.GetYear() - startDate.GetYear();

        int numMonths = todayDate.GetMonth() - startDate.GetMonth();
        int numDays   = todayDate.GetDay() - startDate.GetDay();
        if ( (numMonths >= 0 ) && (numDays < 0) )   numMonths --;
        if ( (numYears > 0 )   && (numMonths < 0 )) numYears -- ;

        if (numYears > 0)
        {
            while (numYears > 0)
            {
                asset_value = value + (rate * value / 100);
                value = asset_value;
                -- numYears;
            }
        }
    }

    return value;
}

double TAssetEntry::GetDepreciatedValue(const wxDateTime& startDate, double value, double rate)
{
    double asset_value = 0.0;
    wxDateTime todayDate = wxDateTime::Now();

    if (todayDate > startDate)
    {
        int numYears = todayDate.GetYear() - startDate.GetYear();
        if (numYears > 0)
        {
            while (numYears > 0)
            {
                asset_value = value - (rate * value / 100);
                value = asset_value;
                numYears --;
            }
        }
    }

    return value;
}

double TAssetEntry::GetValue()
{
    wxDateTime start_date = mmGetStorageStringAsDate(date_);
    double asset_value = value_;

    if (rate_type_ == ASSET_RATE_DEF[APPRECIATE])
    {
        asset_value = GetAppreciatedValue(start_date, value_, rate_value_);
    }

    if (rate_type_ == ASSET_RATE_DEF[DEPRECIATE])
    {
        asset_value = GetDepreciatedValue(start_date, value_, rate_value_);
    }

    return asset_value;
}

wxString TAssetEntry::GetValueCurrencyEditFormat(bool initial_value)
{
    wxString formatted_value;
    if (initial_value) mmex::formatDoubleToCurrencyEdit(value_, formatted_value);
    else mmex::formatDoubleToCurrencyEdit(GetValue(), formatted_value);

    return formatted_value;
}

wxString TAssetEntry::DisplayDate()
{
    return mmGetDateForDisplay(mmGetStorageStringAsDate(date_));
}

/************************************************************************************
 TAssetList Methods
 ***********************************************************************************/
/// Constructor
TAssetList::TAssetList(boost::shared_ptr<wxSQLite3Database> db, bool load_entries)
: TListBase(db)
{
    LoadEntries(load_entries);
}

void TAssetList::LoadEntries(bool load_entries)
{
    try
    {
        if (!db_->TableExists(wxT("ASSETS_V1")))
        {
            const char CREATE_TABLE_ASSETS_V1[] =
            "CREATE TABLE ASSETS_V1 (ASSETID integer primary key, "
            "STARTDATE TEXT NOT NULL, ASSETNAME TEXT, "
            "VALUE numeric, VALUECHANGE TEXT, NOTES TEXT, VALUECHANGERATE numeric, "
            "ASSETTYPE TEXT)";

            db_->ExecuteUpdate(CREATE_TABLE_ASSETS_V1);
        }

        if (load_entries)
        {
            LoadAssetEntriesUsing(wxT("select * from assets_v1"));
        }
    }
    catch (const wxSQLite3Exception& e)
    {
        //wxLogDebug(wxT("TAssetList::LoadEntries %s"), e.GetMessage());
        wxLogError(wxT("TAssetList::LoadEntries %s"), e.GetMessage().c_str());
    }
}

void TAssetList::LoadAssetEntriesUsing(const wxString& sql_statement)
{
    entrylist_.clear();
    wxSQLite3ResultSet q1 = db_->ExecuteQuery(sql_statement);
    while (q1.NextRow())
    {
        boost::shared_ptr<TAssetEntry> pEntry(new TAssetEntry(q1));
        entrylist_.push_back(pEntry);
    }
    q1.Finalize();
}

int TAssetList::AddEntry(TAssetEntry* pAssetEntry)
{
    boost::shared_ptr<TAssetEntry> pEntry(pAssetEntry);
    entrylist_.push_back(pEntry);
    pEntry->Add(db_.get());

    return pEntry->id_;
}

void TAssetList::DeleteEntry(int asset_id)
{
    boost::shared_ptr<TAssetEntry> pEntry = GetEntryPtr(asset_id);
    if (pEntry)
    {
        pEntry->Delete(db_.get());
        entrylist_.erase(entrylist_.begin() + current_index_);
    }
}

boost::shared_ptr<TAssetEntry> TAssetList::GetEntryPtr(int asset_id)
{
    boost::shared_ptr<TAssetEntry> pEntry;
    size_t list_size = entrylist_.size();
    size_t index = 0;

    while (index < list_size)
    {
        if (entrylist_[index]->id_ == asset_id)
        {
            pEntry = entrylist_[index];
            current_index_ = index;
            break;
        }
        ++ index;
    }

    return pEntry;
}

boost::shared_ptr<TAssetEntry> TAssetList::GetIndexedEntryPtr(unsigned int list_index)
{
    boost::shared_ptr<TAssetEntry> pEntry;
    if (list_index < entrylist_.size())
    {
        pEntry = entrylist_[list_index];
    }

    return pEntry;
}

int TAssetList::CurrentListSize()
{
    return entrylist_.size();
}

double TAssetList::GetAssetBalance(bool value_today)
{
    double total_value = 0.0;
    for (size_t i = 0; i < entrylist_.size(); ++i)
    {
        if (value_today)
            total_value = total_value + entrylist_[i]->GetValue();
        else
            total_value = total_value + entrylist_[i]->value_;
    }

    return total_value;
}

wxString TAssetList::GetAssetBalanceCurrencyFormat(bool value_today)
{
    wxString balance_str;
    mmex::formatDoubleToCurrency(GetAssetBalance(value_today), balance_str);

    return balance_str;
}

wxString TAssetList::GetAssetBalanceCurrencyEditFormat(bool value_today)
{
    wxString balance_str;
    mmex::formatDoubleToCurrencyEdit(GetAssetBalance(value_today), balance_str);

    return balance_str;
}
