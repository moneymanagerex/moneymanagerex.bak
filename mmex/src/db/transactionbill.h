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

const wxString REPEAT_TYPE_DEF[] = {
    wxTRANSLATE("None"),
    wxTRANSLATE("Weekly"),
    wxTRANSLATE("Bi-Weekly"),
    wxTRANSLATE("Monthly"),
    wxTRANSLATE("Bi-Monthly"),
    wxTRANSLATE("Quarterly"),
    wxTRANSLATE("Half-Yearly"),
    wxTRANSLATE("Yearly"),
    wxTRANSLATE("Four Months"),
    wxTRANSLATE("Four Weeks"),
    wxTRANSLATE("Daily"),
    wxTRANSLATE("In %s Days"),
    wxTRANSLATE("In %s Months"),
    wxTRANSLATE("Every %s Days"),
    wxTRANSLATE("Every %s Months"),
    wxTRANSLATE("Monthly (last day)"),
    wxTRANSLATE("Monthly (last business day)")
};

/***********************************************************************************
 This class holds a single bill transaction entry
 **********************************************************************************/
class TTransactionBillEntry : public TTransactionEntry
{
private:
    friend class TTransactionBillList;

    int Add(wxSQLite3Database* db);
    void Delete(wxSQLite3Database* db);
    int MultiplexedRepeatType();

public:
    enum REPEAT_TYPE {
        INACTIVE = -1,
        NONE,
        WEEKLY,
        BI_WEEKLY,      // FORTNIGHTLY
        MONTHLY,
        BI_MONTHLY,
        QUARTERLY,      // TRI_MONTHLY
        HALF_YEARLY,
        YEARLY,
        FOUR_MONTHLY,   // QUAD_MONTHLY
        FOUR_WEEKLY,    // QUAD_WEEKLY
        DAILY,
        IN_X_DAYS,
        IN_X_MONTHS,
        EVERY_X_DAYS,
        EVERY_X_MONTHS,
        MONTHLY_LAST_DAY,
        MONTHLY_LAST_BUSINESS_DAY,
        REPEAT_TYPE_SIZE
    };

    bool autoExecuteManual_;
    bool autoExecuteSilent_;
    int repeat_type_;
    wxString nextOccurDate_;
    int num_repeats_;

    /// Constructor for creating a new transaction entry
    TTransactionBillEntry();
    
    /// Copy constructor using a pointer
    TTransactionBillEntry(TTransactionBillEntry* pEntry);

    /// Constructor used to load a transaction from the database.
    TTransactionBillEntry(wxSQLite3ResultSet& q1);

    void Update(wxSQLite3Database* db);
    /// Get the transaction entry from the bill transaction entry
    TTransactionEntry* GetTransaction();
    /// Set the bill transaction entry from a transaction
    void SetTransaction(wxSharedPtr<TTransactionEntry> pEntry);

    void AdjustNextOccuranceDate();
    bool RequiresExecution(int& remaining_days);
};

/************************************************************************************
 Class TTransactionBillsList
 ***********************************************************************************/
class TTransactionBillList : public TTransactionList
{
private:
    void LoadEntries(bool load_entries = true);

public:
    std::vector<wxSharedPtr<TTransactionBillEntry> > entrylist_;

    TTransactionBillList(wxSharedPtr<wxSQLite3Database> db, bool load_entries = true);

    /// Allows specialised loads by providing the required SQL statement
    void LoadEntriesUsing(const wxString& sql_statement);

    int AddEntry(TTransactionBillEntry* pTransBillsEntry);
    void DeleteEntry(int trans_bill_id);

    wxSharedPtr<TTransactionBillEntry> GetEntryPtr(int trans_bill_id);
    wxSharedPtr<TTransactionBillEntry> GetIndexedEntryPtr(unsigned int list_index);

    int CurrentListSize();
};
