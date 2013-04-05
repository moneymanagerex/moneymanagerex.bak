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
#include "transaction.h"

/***********************************************************************************
 This class holds a single bill transaction entry
 **********************************************************************************/
class TTransactionBillEntry : public TTransactionEntry
{
private:
    friend class TTransactionBillList;

    int Add(wxSQLite3Database* db);
    void Delete(wxSQLite3Database* db);

public:
    int repeats_;
    wxString nextOccurDate_;
    int num_repeats_;
    ///Extra field multiplexed onto repeats field
    bool autoExecuteManual_;
    ///Extra field multiplexed onto repeats field
    bool autoExecuteSilent_;

    /// Constructor for creating a new transaction entry
    TTransactionBillEntry();
    
    /// Constructor used to load a transaction from the database.
    TTransactionBillEntry(wxSQLite3ResultSet& q1);

    void Update(wxSQLite3Database* db);
    /// Get the transaction entry from the bill transaction
    TTransactionEntry* GetTransaction();
    void SetTransaction(boost::shared_ptr<TTransactionEntry> pEntry);
};

/************************************************************************************
 Class TTransactionBillsList
 ***********************************************************************************/
class TTransactionBillList : public TTransactionList
{
private:
    void LoadEntries(bool load_entries = true);

public:
    std::vector<boost::shared_ptr<TTransactionBillEntry> > entrylist_;

    TTransactionBillList(boost::shared_ptr<wxSQLite3Database> db, bool load_entries = true);

    /// Allows specialised loads by providing the required SQL statement
    void LoadEntriesUsing(const wxString& sql_statement);

    int AddEntry(TTransactionBillEntry* pTransBillsEntry);
    void DeleteEntry(int trans_bill_id);

    boost::shared_ptr<TTransactionBillEntry> GetEntryPtr(int trans_bill_id);
    boost::shared_ptr<TTransactionBillEntry> GetIndexedEntryPtr(unsigned int list_index);

    int CurrentListSize();
};
