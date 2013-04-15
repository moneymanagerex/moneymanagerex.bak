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

#pragma once
#include "entry_base.h"

/************************************************************************************
 Class TBudgetTableEntry
 ***********************************************************************************/
class TBudgetTableEntry : public TEntryBase
{
private:
    friend class TBudgetTableList;    // Allows TBudgetTableList access to private members
    
    int Add(wxSQLite3Database* db);
    void Delete(wxSQLite3Database* db);
    void SetDatabaseValues(wxSQLite3Statement& st, int& db_index);

public:
    int id_budget_year_;
    int id_category_;
    int id_subcategory_;
    wxString period_;
    double amount_;

    /// Constructor used when loading assets from the database.
    TBudgetTableEntry(wxSQLite3ResultSet& q1);
    /// Copy constructor using a pointer
    TBudgetTableEntry(TBudgetTableEntry* pEntry);
    /// Constructor for creating a new asset entry.
    TBudgetTableEntry();

    void Update(wxSQLite3Database* db);
    wxString GetAmountCurrencyEditFormat();
};

/************************************************************************************
 Class TBudgetTableList
 ***********************************************************************************/
class TBudgetTableList : public TListBase
{
private:
    void LoadEntries(bool load_entries = true);

public:
    std::vector<wxSharedPtr<TBudgetTableEntry> >entrylist_;

    TBudgetTableList(wxSharedPtr<wxSQLite3Database> db, bool load_entries = true);

    /// Allows specialised loads by providing the required SQL statement
    void LoadEntriesUsing(const wxString& sql_statement);

    int AddEntry(TBudgetTableEntry* pAssetEntry);
    void DeleteEntry(int asset_id);

    wxSharedPtr<TBudgetTableEntry> GetEntryPtr(int asset_id);
    wxSharedPtr<TBudgetTableEntry> GetIndexedEntryPtr(unsigned int list_index);

    int CurrentListSize();
    //double GetAssetBalance(bool value_today = true);
    //wxString GetAssetBalanceCurrencyFormat(bool value_today = true);
    //wxString GetAssetBalanceCurrencyEditFormat(bool value_today = true);
};
