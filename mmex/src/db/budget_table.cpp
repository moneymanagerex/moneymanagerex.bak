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

#include "budget_table.h"

/************************************************************************************
 TBudgetTableEntry Methods
 ***********************************************************************************/
/// Constructor used when loading assets from the database
TBudgetTableEntry::TBudgetTableEntry(wxSQLite3ResultSet& q1)
: TEntryBase()
{
    id_             = q1.GetInt(wxT("BudgetEntryID"));
    id_budget_year_ = q1.GetInt(wxT("BudgetYearID"));
    id_category_    = q1.GetInt(wxT("CategID"));
    id_subcategory_ = q1.GetInt(wxT("SubCategID"));
    period_         = q1.GetString(wxT("Period"));
    amount_         = q1.GetDouble(wxT("Amount"));
}

/// Copy constructor using a pointer
TBudgetTableEntry::TBudgetTableEntry(TBudgetTableEntry* pEntry)
: TEntryBase()
{
    id_budget_year_ = pEntry->id_budget_year_;
    id_category_    = pEntry->id_category_;
    id_subcategory_ = pEntry->id_subcategory_;
    period_         = pEntry->period_;
    amount_         = pEntry->amount_;
}

void TBudgetTableEntry::SetDatabaseValues(wxSQLite3Statement& st, int& db_index)
{
    st.Bind(++db_index, id_budget_year_);
    st.Bind(++db_index, id_category_);
    st.Bind(++db_index, id_subcategory_);
    st.Bind(++db_index, period_);
    st.Bind(++db_index, amount_);
}

/// Constructor for creating a new asset entry.
TBudgetTableEntry::TBudgetTableEntry()
: TEntryBase()
, id_budget_year_(-1)
, id_category_(-1)
, id_subcategory_(-1)
, amount_(0.0)
{}

int TBudgetTableEntry::Add(wxSQLite3Database* db)
{
    const char INSERT_INTO_BUDGETTABLE_V1[] =
    "INSERT INTO BUDGETTABLE_V1 "
    "(BudgetYearID, CategID, SubCategID, Period, Amount) "
    "values (?, ?, ?, ?, ?)";

    wxSQLite3Statement st = db->PrepareStatement(INSERT_INTO_BUDGETTABLE_V1);
    int db_index = 0;
    SetDatabaseValues(st, db_index);
    FinaliseAdd(db, st);

    return id_;
}

void TBudgetTableEntry::Delete(wxSQLite3Database* db)
{
    DeleteEntry(db, wxT("delete from BUDGETTABLE_V1 where BUDGETENTRYID = ?")); 
}

void TBudgetTableEntry::Update(wxSQLite3Database* db)
{
    const char UPDATE_BUDGETTABLE_V1[] =
    "update BUDGETTABLE_V1 set"
    " BudgetYearID = ?, CategID = ?, SubCategID = ?, "
    " Period = ?, Amount = ? "
    "where BUDGETENTRYID = ?";
    try
    {
        wxSQLite3Statement st = db->PrepareStatement(UPDATE_BUDGETTABLE_V1);
        
        int db_index = 0;
        SetDatabaseValues(st, db_index);
        st.Bind(++db_index, id_);

        FinaliseStatement(st);
    }
    catch(const wxSQLite3Exception& e)
    {
        //wxLogDebug(wxT("TBudgetTableEntry:Update: %s"), e.GetMessage().c_str());
        wxLogError(wxT("TBudgetTableEntry:Update: %s"), e.GetMessage().c_str());
    }
}

wxString TBudgetTableEntry::GetAmountCurrencyEditFormat()
{
    wxString formatted_value;
    mmex::formatDoubleToCurrencyEdit(amount_, formatted_value);

    return formatted_value;
}

/************************************************************************************
 TBudgetTableList Methods
 ***********************************************************************************/
/// Constructor
TBudgetTableList::TBudgetTableList(wxSharedPtr<wxSQLite3Database> db, bool load_entries)
: TListBase(db)
{
    LoadEntries(load_entries);
}

void TBudgetTableList::LoadEntries(bool load_entries)
{
    try
    {
        if (!db_->TableExists(wxT("BUDGETTABLE_V1")))
        {
            const char CREATE_TABLE_BUDGETTABLE_V1[] =
            "CREATE TABLE BUDGETTABLE_V1(BUDGETENTRYID integer primary key, "
            "BUDGETYEARID integer, CATEGID integer, SUBCATEGID integer, "
            "PERIOD TEXT NOT NULL, AMOUNT numeric NOT NULL)";

            db_->ExecuteUpdate(CREATE_TABLE_BUDGETTABLE_V1);
        }

        if (load_entries)
        {
            LoadEntriesUsing(wxT("select * from BUDGETTABLE_V1"));
        }
    }
    catch (const wxSQLite3Exception& e)
    {
        //wxLogDebug(wxT("TBudgetTableList::LoadEntries %s"), e.GetMessage().c_str());
        wxLogError(wxT("TBudgetTableList::LoadEntries %s"), e.GetMessage().c_str());
    }
}

void TBudgetTableList::LoadEntriesUsing(const wxString& sql_statement)
{
    entrylist_.clear();
    wxSQLite3ResultSet q1 = db_->ExecuteQuery(sql_statement);
    while (q1.NextRow())
    {
        wxSharedPtr<TBudgetTableEntry> pEntry(new TBudgetTableEntry(q1));
        entrylist_.push_back(pEntry);
    }
    q1.Finalize();
}

int TBudgetTableList::AddEntry(TBudgetTableEntry* pBudgetTableEntry)
{
    wxSharedPtr<TBudgetTableEntry> pEntry(pBudgetTableEntry);
    entrylist_.push_back(pEntry);
    pEntry->Add(db_.get());

    return pEntry->id_;
}

void TBudgetTableList::DeleteEntry(int asset_id)
{
    wxSharedPtr<TBudgetTableEntry> pEntry = GetEntryPtr(asset_id);
    if (pEntry)
    {
        pEntry->Delete(db_.get());
        entrylist_.erase(entrylist_.begin() + current_index_);
    }
}

wxSharedPtr<TBudgetTableEntry> TBudgetTableList::GetEntryPtr(int asset_id)
{
    wxSharedPtr<TBudgetTableEntry> pEntry;
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

wxSharedPtr<TBudgetTableEntry> TBudgetTableList::GetIndexedEntryPtr(unsigned int list_index)
{
    wxSharedPtr<TBudgetTableEntry> pEntry;
    if (list_index < entrylist_.size())
    {
        pEntry = entrylist_[list_index];
    }

    return pEntry;
}

int TBudgetTableList::CurrentListSize()
{
    return entrylist_.size();
}

//double TBudgetTableList::GetAssetBalance(bool value_today)
//{
//    double total_value = 0.0;
//    for (size_t i = 0; i < entrylist_.size(); ++i)
//    {
//        if (value_today)
//            total_value = total_value + entrylist_[i]->GetValue();
//        else
//            total_value = total_value + entrylist_[i]->value_;
//    }
//
//    return total_value;
//}
//
//wxString TBudgetTableList::GetAssetBalanceCurrencyFormat(bool value_today)
//{
//    wxString balance_str;
//    mmex::formatDoubleToCurrency(GetAssetBalance(value_today), balance_str);
//
//    return balance_str;
//}
//
//wxString TBudgetTableList::GetAssetBalanceCurrencyEditFormat(bool value_today)
//{
//    wxString balance_str;
//    mmex::formatDoubleToCurrencyEdit(GetAssetBalance(value_today), balance_str);
//
//    return balance_str;
//}
