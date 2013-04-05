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

enum TRANS_STATE {TRANS_NONE, TRANS_RECONCILED, TRANS_VOID, TRANS_FOLLOWUP, TRANS_DUPLICATE};
const wxString TRANS_STATE_DEF[] = {
    wxT("N"),
    wxT("R"),
    wxT("V"),
    wxT("F"),
    wxT("D")
};

enum TRANS_TYPE {TRANS_WITHDRAWAL, TRANS_DEPOSIT, TRANS_TRANSFER};
const wxString TRANS_TYPE_DEF[] = {
    wxT("Withdrawal"),
    wxT("Deposit"),
    wxT("Transfer")
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
    int id_from_account;
    int id_to_account_;
    int id_payee_;
    int id_category_;
    int id_subcategory_;
    int id_followup_;
    
    double amount_from_;
    double amount_to_;

    wxString trans_date_;
    wxString trans_num_;
    wxString trans_type_;   // transcode in database
    wxString trans_status_;
    wxString trans_notes_;

    /// Constructor for creating a new transaction entry
    TTransactionEntry();
    /// Copy constructor using a pointer
    TTransactionEntry(TTransactionEntry* pEntry);
    
    /// Constructor used to load a transaction from the database.
    TTransactionEntry(wxSQLite3ResultSet& q1);

    void Update(wxSQLite3Database* db);
};

/************************************************************************************
 Class TTransactionList
 ***********************************************************************************/
class TTransactionList : public TListBase
{
private:
    void LoadEntries(bool load_entries = true);

public:
    std::vector<boost::shared_ptr<TTransactionEntry> > entrylist_;

    TTransactionList(boost::shared_ptr<wxSQLite3Database> db, bool load_entries = true);

    /// Allows specialised loads by providing the required SQL statement
    void LoadEntriesUsing(const wxString& sql_statement);

    int AddEntry(TTransactionEntry* pTransEntry);
    void DeleteEntry(int trans_id);

    boost::shared_ptr<TTransactionEntry> GetEntryPtr(int trans_id);
    boost::shared_ptr<TTransactionEntry> GetIndexedEntryPtr(unsigned int list_index);

    int CurrentListSize();
//    double GetBalance();
};
