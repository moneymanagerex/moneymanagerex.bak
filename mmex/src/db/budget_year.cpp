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

#include "budget_year.h"

/************************************************************************************
 TBudgetYearEntry Methods
 ***********************************************************************************/
/// Constructor used when loading assets from the database
TBudgetYearEntry::TBudgetYearEntry(wxSQLite3ResultSet& q1)
: TEntryBase()
{
    id_   = q1.GetInt(wxT("BudgetYearID"));
    name_ = q1.GetString(wxT("BudgetYearName"));
}

void TBudgetYearEntry::SetDatabaseValues(wxSQLite3Statement& st, int& db_index)
{
    st.Bind(++db_index, name_);
}

/// Constructor for creating a new budget year entry.
TBudgetYearEntry::TBudgetYearEntry()
: TEntryBase()
{}

int TBudgetYearEntry::Add(wxSQLite3Database* db)
{
    const char INSERT_INTO_BUDGETYEAR_V1[] =
    "insert into BUDGETYEAR_V1 "
    "(BudgetYearName) "
    "values (?)";

    wxSQLite3Statement st = db->PrepareStatement(INSERT_INTO_BUDGETYEAR_V1);
    int db_index = 0;
    SetDatabaseValues(st, db_index);
    FinaliseAdd(db, st);

    return id_;
}

void TBudgetYearEntry::Delete(wxSQLite3Database* db)
{
    DeleteEntry(db, wxT("delete from BUDGETYEAR_V1 where BudgetYearID = ?")); 
}

void TBudgetYearEntry::Update(wxSQLite3Database* db)
{
    const char UPDATE_BUDGETYEAR_V1[] =
    "update BUDGETYEAR_V1 set"
    " BudgetYearName = ?";
    try
    {
        wxSQLite3Statement st = db->PrepareStatement(UPDATE_BUDGETYEAR_V1);
        
        int db_index = 0;
        SetDatabaseValues(st, db_index);
        st.Bind(++db_index, id_);

        FinaliseStatement(st);
    }
    catch(const wxSQLite3Exception& e)
    {
        //wxLogDebug(wxT("TBudgetYearEntry:Update: %s"), e.GetMessage().c_str());
        wxLogError(wxT("TBudgetYearEntry:Update: %s"), e.GetMessage().c_str());
    }
}

/************************************************************************************
 TBudgetYearList Methods
 ***********************************************************************************/
/// Constructor
TBudgetYearList::TBudgetYearList(std::shared_ptr<wxSQLite3Database> db, bool load_entries)
: TListBase(db)
{
    LoadEntries(load_entries);
}

void TBudgetYearList::LoadEntries(bool load_entries)
{
    try
    {
        if (!db_->TableExists(wxT("BUDGETYEAR_V1")))
        {
            const char CREATE_TABLE_BUDGETYEAR_V1[] =
            "CREATE TABLE BUDGETYEAR_V1(BUDGETYEARID integer primary key, "
            "BUDGETYEARNAME TEXT NOT NULL UNIQUE)";

            db_->ExecuteUpdate(CREATE_TABLE_BUDGETYEAR_V1);
        }

        if (load_entries)
        {
            LoadEntriesUsing(wxT("select * from BUDGETYEAR_V1"));
        }
    }
    catch (const wxSQLite3Exception& e)
    {
        //wxLogDebug(wxT("TBudgetYearList::LoadEntries %s"), e.GetMessage().c_str());
        wxLogError(wxT("TBudgetYearList::LoadEntries %s"), e.GetMessage().c_str());
    }
}

void TBudgetYearList::LoadEntriesUsing(const wxString& sql_statement)
{
    entrylist_.clear();
    wxSQLite3ResultSet q1 = db_->ExecuteQuery(sql_statement);
    while (q1.NextRow())
    {
        std::shared_ptr<TBudgetYearEntry> pEntry(new TBudgetYearEntry(q1));
        entrylist_.push_back(pEntry);
    }
    q1.Finalize();
}

int TBudgetYearList::AddEntry(TBudgetYearEntry* pBudgetYearEntry)
{
    std::shared_ptr<TBudgetYearEntry> pEntry(pBudgetYearEntry);
    entrylist_.push_back(pEntry);
    pEntry->Add(db_.get());

    return pEntry->id_;
}

int TBudgetYearList::AddEntry(wxString budget_year_name)
{
    TBudgetYearEntry* pEntry = new TBudgetYearEntry();
    pEntry->name_ = budget_year_name;

    return AddEntry(pEntry);
}

void TBudgetYearList::DeleteEntry(int budget_year_id)
{
    std::shared_ptr<TBudgetYearEntry> pEntry = GetEntryPtr(budget_year_id);
    if (pEntry)
    {
        pEntry->Delete(db_.get());
        entrylist_.erase(entrylist_.begin() + current_index_);
    }
}

std::shared_ptr<TBudgetYearEntry> TBudgetYearList::GetEntryPtr(int budget_year_id)
{
    std::shared_ptr<TBudgetYearEntry> pEntry;
    size_t list_size = entrylist_.size();
    size_t index = 0;

    while (index < list_size)
    {
        if (entrylist_[index]->id_ == budget_year_id)
        {
            pEntry = entrylist_[index];
            current_index_ = index;
            break;
        }
        ++ index;
    }

    return pEntry;
}

std::shared_ptr<TBudgetYearEntry> TBudgetYearList::GetIndexedEntryPtr(unsigned int list_index)
{
    std::shared_ptr<TBudgetYearEntry> pEntry;
    if (list_index < entrylist_.size())
    {
        pEntry = entrylist_[list_index];
    }

    return pEntry;
}

int TBudgetYearList::CurrentListSize()
{
    return entrylist_.size();
}
