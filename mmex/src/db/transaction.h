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

const wxString TRANS_STATE_DEF[] = {"N", "R", "V", "F", "D"};
const wxString TRANS_STATE_TRANSLATION[] = {
    "None",
    "Reconciled",
    "Void",
    "Follow up",
    "Duplicate"
};

const wxString TRANS_TYPE_DEF[] = {
    "Withdrawal",
    "Deposit",
    "Transfer"
};

/***********************************************************************************
 This class holds a single transaction entry
 **********************************************************************************/
class TTransactionEntry : public TEntryBase
{
private:
    friend class TTransactionList;

    int Add(wxSQLite3Database* db);
    void Delete(wxSQLite3Database* db);

protected:
    void SetDatabaseValues(wxSQLite3Statement& st, int& db_index);
    void GetDatabaseValues(wxSQLite3ResultSet& q1);

public:
    enum TRANS_STATE {
        STATE_NONE,
        STATE_RECONCILED,
        STATE_VOID,
        STATE_FOLLOWUP,
        STATE_DUPLICATE
    };

    enum TRANS_TYPE {
        TYPE_WITHDRAWAL,
        TYPE_DEPOSIT,
        TYPE_TRANSFER
    };

    int id_from_account;
    int id_to_account_;
    int id_payee_;
    int id_category_;
    int id_subcategory_;
    int id_followup_;

    double amount_from_;
    double amount_to_;

    wxDateTime trans_date_;
    wxString trans_num_;
    wxString trans_type_;   // transcode in database
    wxString trans_status_;
    wxString trans_notes_;

    virtual bool operator <(const TTransactionEntry& trans) const ;

    // Constructor for creating a new transaction entry
    TTransactionEntry();

    // Copy constructor using a pointer
    TTransactionEntry(TTransactionEntry* pEntry);

    // Constructor used to load a transaction from the database.
    TTransactionEntry(wxSQLite3ResultSet& q1);

    void Update(wxSQLite3Database* db);
    // Display transaction date according to required user format
    wxString DisplayTransactionDate();
};

/************************************************************************************
 Class TTransactionList
 ***********************************************************************************/
class TTransactionList : public TListBase
{
private:
    void LoadEntries(bool load_entries = true);

public:
    std::vector<std::shared_ptr<TTransactionEntry> > entrylist_;

    TTransactionList(std::shared_ptr<wxSQLite3Database> db, bool load_entries = true);

    // Allows specialised loads by providing the required SQL statement
    void LoadEntriesUsing(const wxString& sql_statement);

    int AddEntry(TTransactionEntry* pTransEntry);
    void DeleteEntry(int trans_id);

    TTransactionEntry* GetEntryPtr(int trans_id);
    TTransactionEntry* GetIndexedEntryPtr(unsigned int list_index);

    int CurrentListSize();
//    double GetBalance();
};
