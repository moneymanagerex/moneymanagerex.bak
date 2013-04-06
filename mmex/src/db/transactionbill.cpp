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

#include "transactionbill.h"

const int BD_REPEATS_MULTIPLEX_BASE  = 100;

/************************************************************************************
 TTransactionEntry Methods
 ***********************************************************************************/

    /// Constructor for creating a new transaction entry
TTransactionBillEntry::TTransactionBillEntry()
: TTransactionEntry()
, repeats_(0)
, num_repeats_(0)
, autoExecuteManual_(false)
, autoExecuteSilent_(false)
{}

/// Constructor used to load a transaction from the database.
TTransactionBillEntry::TTransactionBillEntry(wxSQLite3ResultSet& q1)
: TTransactionEntry()
, autoExecuteManual_(false)
, autoExecuteSilent_(false)
{
    id_ = q1.GetInt(wxT("BDID"));
    GetDatabaseValues(q1);
    repeats_       = q1.GetInt(wxT("REPEATS"));
    nextOccurDate_ = q1.GetString(wxT("NEXTOCCURRENCEDATE"));
    num_repeats_   = q1.GetInt(wxT("NUMOCCURRENCES"));

    // DeMultiplex the Auto Executable fields from the db entry: REPEATS
    if (repeats_ >= BD_REPEATS_MULTIPLEX_BASE)    // Auto Execute - User Acknowlegement
    {
        repeats_ -= BD_REPEATS_MULTIPLEX_BASE;
        autoExecuteManual_ = true;
    }

    if (repeats_ >= BD_REPEATS_MULTIPLEX_BASE)    // Auto Execute - Silent mode
    {
        repeats_ -= BD_REPEATS_MULTIPLEX_BASE;
        autoExecuteManual_ = false;               // Can only be manual or auto. Not both
        autoExecuteSilent_ = true;
    }
}

int TTransactionBillEntry::Add(wxSQLite3Database* db)
{
    const char ADD_BILLSDEPOSITS_V1_ROW[] =
    "insert into BILLSDEPOSITS_V1"
    " (ACCOUNTID, TOACCOUNTID, PAYEEID, TRANSCODE, TRANSAMOUNT,"
    " STATUS, TRANSACTIONNUMBER, NOTES, CATEGID, SUBCATEGID,"
    " TRANSDATE, FOLLOWUPID, TOTRANSAMOUNT,"
    " REPEATS, NEXTOCCURRENCEDATE, NUMOCCURRENCES)"
    " values (?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?)";

    wxSQLite3Statement st = db->PrepareStatement(ADD_BILLSDEPOSITS_V1_ROW);
    int db_index = 0;
    SetDatabaseValues(st, db_index);

    // Multiplex Auto executable onto the repeat field of the database.
    if (autoExecuteManual_) repeats_ += BD_REPEATS_MULTIPLEX_BASE;
    if (autoExecuteSilent_) repeats_ += BD_REPEATS_MULTIPLEX_BASE;
    st.Bind(++db_index, repeats_);

    st.Bind(++db_index, nextOccurDate_);
    st.Bind(++db_index, num_repeats_);
    
    FinaliseAdd(db, st);

    return id_;
}

void TTransactionBillEntry::Delete(wxSQLite3Database* db)
{
    DeleteEntry(db, wxT("delete from BILLSDEPOSITS_V1 where where BDID = ?"));
}

void TTransactionBillEntry::Update(wxSQLite3Database* db)
{
    const char UPDATE_BILLSDEPOSITS_V1_ROW[] =
    "update BILLSDEPOSITS_V1 set"
    " ACCOUNTID = ?, TOACCOUNTID = ?, PAYEEID = ?, TRANSCODE = ?, TRANSAMOUNT = ?,"
    " STATUS = ?, TRANSACTIONNUMBER = ?, NOTES = ?, CATEGID = ?, SUBCATEGID = ?,"
    " TRANSDATE = ?, FOLLOWUPID = ?, TOTRANSAMOUNT = ?,"
    " REPEATS = ?, NEXTOCCURRENCEDATE = ?, NUMOCCURRENCES = ? "
    "where BDID = ?";
    try
    {
        wxSQLite3Statement st = db->PrepareStatement(UPDATE_BILLSDEPOSITS_V1_ROW);
        int db_index = 0;
        SetDatabaseValues(st, db_index);

        // Multiplex Auto executable onto the repeat field of the database.
        if (autoExecuteManual_) repeats_ += BD_REPEATS_MULTIPLEX_BASE;
        if (autoExecuteSilent_) repeats_ += BD_REPEATS_MULTIPLEX_BASE;
        st.Bind(++db_index, repeats_);

        st.Bind(++db_index, repeats_);
        st.Bind(++db_index, nextOccurDate_);
        st.Bind(++db_index, num_repeats_);
        st.Bind(++db_index, id_);

        FinaliseStatement(st);
    }
    catch(const wxSQLite3Exception& e)
    {
        // wxLogDebug(wxT("TTransactionBillEntry:update: %s"), e.GetMessage().c_str());
        wxLogError(wxT("TTransactionBillEntry:update: %s"), e.GetMessage().c_str());
    }
}

TTransactionEntry* TTransactionBillEntry::GetTransaction()
{
    TTransactionEntry* pEntry(new TTransactionEntry());
    
    pEntry->id_from_account = id_from_account;
    pEntry->id_to_account_  = id_to_account_;
    pEntry->id_payee_       = id_payee_;
    pEntry->id_category_    = id_category_;
    pEntry->id_subcategory_ = id_subcategory_;
    pEntry->id_followup_    = id_followup_;
    
    pEntry->amount_from_    = amount_from_;
    pEntry->amount_to_      = amount_to_;

    pEntry->trans_date_     = trans_date_;
    pEntry->trans_num_      = trans_num_;
    pEntry->trans_type_     = trans_type_;   // transcode in database
    pEntry->trans_status_   = trans_status_;
    pEntry->trans_notes_    = trans_notes_;

    return pEntry;
}

void TTransactionBillEntry::SetTransaction(boost::shared_ptr<TTransactionEntry> pEntry)
{
    id_from_account = pEntry->id_from_account;
    id_to_account_  = pEntry->id_to_account_;
    id_payee_       = pEntry->id_payee_;
    id_category_    = pEntry->id_category_;
    id_subcategory_ = pEntry->id_subcategory_;
    id_followup_    = pEntry->id_followup_;
    
    amount_from_    = pEntry->amount_from_;
    amount_to_      = pEntry->amount_to_;

    trans_date_     = pEntry->trans_date_;
    trans_num_      = pEntry->trans_num_;
    trans_type_     = pEntry->trans_type_;   // transcode in database
    trans_status_   = pEntry->trans_status_;
    trans_notes_    = pEntry->trans_notes_;
}

/************************************************************************************
 TTransactionList Methods
 ***********************************************************************************/
/// Constructor
TTransactionBillList::TTransactionBillList(boost::shared_ptr<wxSQLite3Database> db, bool load_entries)
: TTransactionList(db, false)
{
    LoadEntries(load_entries);
}

void TTransactionBillList::LoadEntries(bool load_entries)
{
    try
    {
        if (!db_->TableExists(wxT("BILLSDEPOSITS_V1")))
        {
            const char CREATE_TABLE_BILLSDEPOSITS_V1[] =
            "CREATE TABLE BILLSDEPOSITS_V1 (BDID INTEGER PRIMARY KEY, "
            "ACCOUNTID INTEGER NOT NULL, TOACCOUNTID INTEGER, PAYEEID integer NOT NULL, TRANSCODE TEXT NOT NULL, "
            "TRANSAMOUNT numeric NOT NULL, STATUS TEXT, TRANSACTIONNUMBER TEXT, NOTES TEXT, "
            "CATEGID integer, SUBCATEGID integer, TRANSDATE TEXT, FOLLOWUPID integer, TOTRANSAMOUNT numeric, "
            "REPEATS numeric, NEXTOCCURRENCEDATE TEXT, NUMOCCURRENCES numeric)";

            db_->ExecuteUpdate(CREATE_TABLE_BILLSDEPOSITS_V1);
        }

        if (load_entries)
        {
            LoadEntriesUsing(wxT("select * from BILLSDEPOSITS_V1"));
        }
    }
    catch (const wxSQLite3Exception& e)
    {
        // wxLogDebug(wxT("TTransactionBillList:LoadEntries %s"), e.GetMessage().c_str());
        wxLogError(wxT("TTransactionBillList:LoadEntries %s"), e.GetMessage().c_str());
    }
}

void TTransactionBillList::LoadEntriesUsing(const wxString& sql_statement)
{
    entrylist_.clear();
    wxSQLite3ResultSet q1 = db_->ExecuteQuery(sql_statement);
    while (q1.NextRow())
    {
        boost::shared_ptr<TTransactionBillEntry> pEntry(new TTransactionBillEntry(q1));
        entrylist_.push_back(pEntry);
    }
    q1.Finalize();
}

int TTransactionBillList::AddEntry(TTransactionBillEntry* pTransBillEntry)
{
    boost::shared_ptr<TTransactionBillEntry> pEntry(pTransBillEntry);
    entrylist_.push_back(pEntry);
    pEntry->Add(db_.get());

    return pEntry->id_;
}

void TTransactionBillList::DeleteEntry(int trans_bill_id)
{
    boost::shared_ptr<TTransactionBillEntry> pEntry = GetEntryPtr(trans_bill_id);
    if (pEntry)
    {
        pEntry->Delete(db_.get());
        entrylist_.erase(entrylist_.begin() + current_index_);
    }
}

boost::shared_ptr<TTransactionBillEntry> TTransactionBillList::GetEntryPtr(int trans_bill_id)
{
    boost::shared_ptr<TTransactionBillEntry> pEntry;
    size_t list_size = entrylist_.size();
    size_t index = 0;

    while (index < list_size)
    {
        if (entrylist_[index]->id_ == trans_bill_id)
        {
            pEntry = entrylist_[index];
            current_index_ = index;
            break;
        }
        ++ index;
    }

    return pEntry;
}

boost::shared_ptr<TTransactionBillEntry> TTransactionBillList::GetIndexedEntryPtr(unsigned int list_index)
{
    boost::shared_ptr<TTransactionBillEntry> pEntry;
    if (list_index < entrylist_.size())
    {
        pEntry = entrylist_[list_index];
    }

    return pEntry;
}

int TTransactionBillList::CurrentListSize()
{
    return entrylist_.size();
}
