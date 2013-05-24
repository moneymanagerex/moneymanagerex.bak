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
#include "../mmCurrencyFormatter.h"

/************************************************************************************
 TAssetEntry Methods
 ***********************************************************************************/
/// Constructor used when loading assets from the database
TAssetEntry::TAssetEntry(wxSQLite3ResultSet& q1)
: TEntryBase()
{
    id_         = q1.GetInt("ASSETID");
    date_       = q1.GetDate("STARTDATE");
    name_       = q1.GetString("ASSETNAME");
    value_      = q1.GetDouble("VALUE");
    rate_type_  = q1.GetString("VALUECHANGE");
    notes_      = q1.GetString("NOTES");
    rate_value_ = q1.GetDouble("VALUECHANGERATE");
    type_       = q1.GetString("ASSETTYPE");
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
    st.BindDate(++db_index, date_); // q1.GetString("STARTDATE");
    st.Bind(++db_index, name_);                 // q1.GetString("ASSETNAME");
    st.Bind(++db_index, value_);                // q1.GetDouble("VALUE");
    st.Bind(++db_index, rate_type_);            // q1.GetString("VALUECHANGE");
    st.Bind(++db_index, notes_);                // q1.GetString("NOTES");
    st.Bind(++db_index, rate_value_);           // q1.GetDouble("VALUECHANGERATE");
    st.Bind(++db_index, type_);                 // q1.GetString("ASSETTYPE");
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
    DeleteEntry(db, "delete from ASSETS_V1 where ASSETID = ?"); 
}

void TAssetEntry::Update(wxSQLite3Database* db)
{
    try
    {
        const char SQL_STATEMENT[] =
        "update ASSETS_V1 set"
        " STARTDATE = ?, ASSETNAME = ?, VALUE = ?, "
        " VALUECHANGE = ?, NOTES = ?, VALUECHANGERATE = ?, ASSETTYPE = ? "
        "where ASSETID = ?";

        wxSQLite3Statement st = db->PrepareStatement(SQL_STATEMENT);
        int db_index = 0;
        SetDatabaseValues(st, db_index);
        st.Bind(++db_index, id_);

        FinaliseStatement(st);
    }
    catch(const wxSQLite3Exception& e)
    {
        wxLogError("TAssetEntry:Update: %s", e.GetMessage().c_str());
    }
}

double TAssetEntry::PeriodValueAtRate(const wxDateTime startDate, double value, double rate)
{
    int diff_days = abs(startDate.Subtract(wxDateTime::Now()).GetDays());

    return ((value * (rate/100))/365.25) * diff_days;
}

double TAssetEntry::GetAppreciatedValue(const wxDateTime& startDate, double value, double rate)
{
    value = value + PeriodValueAtRate(startDate, value, rate);

    return value;
}

double TAssetEntry::GetDepreciatedValue(const wxDateTime& startDate, double value, double rate)
{
    value = value - PeriodValueAtRate(startDate, value, rate);
    if (value < 0)
    {
        value = 0;
    }

    return value;
}

bool TAssetEntry::operator < (const TAssetEntry& asset) const
{
    if (this->date_ < asset.date_) return true; else if (this->date_ > asset.date_) return false;
    return this->GetId() < asset.GetId();
}

double TAssetEntry::GetValue()
{
    double asset_value = value_;

    if (rate_type_ == ASSET_RATE_DEF[RATE_APPRECIATE])
    {
        asset_value = GetAppreciatedValue(date_, value_, rate_value_);
    }

    if (rate_type_ == ASSET_RATE_DEF[RATE_DEPRECIATE])
    {
        asset_value = GetDepreciatedValue(date_, value_, rate_value_);
    }

    return asset_value;
}

wxString TAssetEntry::GetValueCurrencyEditFormat(bool initial_value)
{
    wxString formatted_value;
    if (initial_value)  formatted_value = CurrencyFormatter::float2String(value_);
    else  formatted_value = CurrencyFormatter::float2String(GetValue());

    return formatted_value;
}

wxString TAssetEntry::DisplayDate()
{
    return mmGetDateForDisplay(date_);
}

/************************************************************************************
 TAssetList Methods
 ***********************************************************************************/
/// Constructor
TAssetList::TAssetList(std::shared_ptr<wxSQLite3Database> db, bool load_entries)
: TListBase(db)
{
    LoadEntries(load_entries);
}

void TAssetList::LoadEntries(bool load_entries)
{
    try
    {
        if (!db_->TableExists("ASSETS_V1"))
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
            LoadAssetEntriesUsing("select * from assets_v1");
        }
    }
    catch (const wxSQLite3Exception& e)
    {
        wxLogError("TAssetList::LoadEntries %s", e.GetMessage().c_str());
    }
}

void TAssetList::LoadAssetEntriesUsing(const wxString& sql_statement)
{
    entrylist_.clear();
    wxSQLite3ResultSet q1 = db_->ExecuteQuery(sql_statement);
    while (q1.NextRow())
    {
        std::shared_ptr<TAssetEntry> pEntry(new TAssetEntry(q1));
        entrylist_.push_back(pEntry);
    }
    q1.Finalize();
}

int TAssetList::AddEntry(TAssetEntry* pAssetEntry)
{
    std::shared_ptr<TAssetEntry> pEntry(pAssetEntry);
    entrylist_.push_back(pEntry);
    pEntry->Add(db_.get());

    return pEntry->id_;
}

void TAssetList::DeleteEntry(int asset_id)
{
    std::shared_ptr<TAssetEntry> pEntry = GetEntryPtr(asset_id);
    if (pEntry)
    {
        pEntry->Delete(db_.get());
        entrylist_.erase(entrylist_.begin() + current_index_);
    }
}

std::shared_ptr<TAssetEntry> TAssetList::GetEntryPtr(int asset_id)
{
    std::shared_ptr<TAssetEntry> pEntry;
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

std::shared_ptr<TAssetEntry> TAssetList::GetIndexedEntryPtr(unsigned int list_index)
{
    std::shared_ptr<TAssetEntry> pEntry;
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
    return CurrencyFormatter::float2Money(GetAssetBalance(value_today));
}
wxString TAssetList::GetAssetBalanceCurrencyEditFormat(bool value_today)
{
    return CurrencyFormatter::float2Money(GetAssetBalance(value_today));
} 
