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

enum ACCOUNT_STATE {OPEN, CLOSED};
const wxString ACCOUNT_STATE_DEF[] = {
    wxT("Open"),
    wxT("Closed")
};

enum ACCOUNT_TYPE {BANK, TERM, STOCK};
const wxString ACCOUNT_TYPE_DEF[] = {
    wxT("Checking"),
    wxT("Term"),
    wxT("Investment")
};

class TCurrencyList;

/************************************************************************************
 Class TAccountEntry
 ***********************************************************************************/
class TAccountEntry : public TEntryBase
{
private:
    friend class TAccountList;    // Allows the class, access to private members.
    
    int Add(wxSQLite3Database* db);
    void Delete(wxSQLite3Database* db);
    void SetDatabaseValues(wxSQLite3Statement& st, int& db_index);

public:
    wxString acc_name_;
    wxString acc_type_;
    wxString acc_number_;
    wxString acc_state_;
    wxString notes_;
    wxString held_at_;
    wxString website_;
    wxString contact_info_;
    wxString access_info_;
    bool favoriteAcct_;
    double init_balance_;
    int currency_id_;

    /// Constructor used when loading payees from the database
    TAccountEntry(wxSQLite3ResultSet& q1);
    
    /// Constructor for creating a new account entry
    TAccountEntry();

    void Update(wxSQLite3Database* db);
};

/************************************************************************************
 Class TAccountList
 ***********************************************************************************/
class TAccountList : public TListBase
{
private:
    TCurrencyList& currency_list_;
    void LoadEntries(bool load_entries = true);

    int NumberOfAccounts(int account_type);

public:
    std::vector<wxSharedPtr<TAccountEntry> > entrylist_;

    TAccountList(wxSharedPtr<wxSQLite3Database> db,
    TCurrencyList& currency_list, bool load_entries = true);

    /// Allows specialised list loading provided by SQL statement
    void LoadEntriesUsing(const wxString& sql_statement);

    int AddEntry(TAccountEntry* pAccountEntry);

    /// Note: At this level, no checking is done for usage in other tables.
    void DeleteEntry(int account_id);
    void DeleteEntry(const wxString& account_name);

    wxSharedPtr<TAccountEntry> GetEntryPtr(int account_id);
    wxSharedPtr<TAccountEntry> GetEntryPtr(const wxString& name);
    wxSharedPtr<TAccountEntry> GetIndexedEntryPtr(unsigned int list_index);

    int GetAccountId(const wxString& account_name);
    wxString GetAccountName(int account_id);
    bool AccountExists(const wxString& account_name);

    int NumberOfBankAccounts();
    int NumberOfTermAccounts();
    int NumberOfStockAccounts();
};
