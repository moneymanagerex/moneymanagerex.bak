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

#include "account.h"

/************************************************************************************
 TAccountEntry Methods
 ***********************************************************************************/
/// Constructor used when loading accounts from the database
TAccountEntry::TAccountEntry(wxSQLite3ResultSet& q1)
: TEntryBase()
{
    id_             = q1.GetInt("ACCOUNTID");
    acc_name_       = q1.GetString("ACCOUNTNAME");
    acc_type_       = q1.GetString("ACCOUNTTYPE");
    acc_number_     = q1.GetString("ACCOUNTNUM");
    acc_state_      = q1.GetString("STATUS");
    notes_          = q1.GetString("NOTES");  
    held_at_        = q1.GetString("HELDAT");
    website_        = q1.GetString("WEBSITE");
    contact_info_   = q1.GetString("CONTACTINFO");
    access_info_    = q1.GetString("ACCESSINFO");
    init_balance_   = q1.GetDouble("INITIALBAL");

    favoriteAcct_ = false;
    if (q1.GetString("FAVORITEACCT") == "TRUE") favoriteAcct_ = true;

    currency_id_    = q1.GetDouble("CURRENCYID");
}

void TAccountEntry::SetDatabaseValues(wxSQLite3Statement& st, int& db_index)
{
    st.Bind(++db_index, acc_name_);
    st.Bind(++db_index, acc_type_);
    st.Bind(++db_index, acc_number_);
    st.Bind(++db_index, acc_state_);
    st.Bind(++db_index, notes_);
    st.Bind(++db_index, held_at_);
    st.Bind(++db_index, website_);
    st.Bind(++db_index, contact_info_);
    st.Bind(++db_index, access_info_);
    st.Bind(++db_index, init_balance_);

    if (favoriteAcct_) st.Bind(++db_index, "TRUE");
    else st.Bind(++db_index, "FALSE");

    st.Bind(++db_index, currency_id_);
}

/// Constructor for creating a new account entry
TAccountEntry::TAccountEntry()
: TEntryBase()
, init_balance_(0.0)
, currency_id_(-1)
, favoriteAcct_(false)
{}

int TAccountEntry::Add(wxSQLite3Database* db)
{
    try
    {
        const char SQL_STATEMENT[] =
        "insert into ACCOUNTLIST_V1 "
        "(ACCOUNTNAME, ACCOUNTTYPE, ACCOUNTNUM, STATUS, NOTES,"
        " HELDAT, WEBSITE, CONTACTINFO, ACCESSINFO, "
        " INITIALBAL, FAVORITEACCT, CURRENCYID) "
        "values (?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ? )";

        wxSQLite3Statement st = db->PrepareStatement(SQL_STATEMENT);
        int db_index = 0;
        SetDatabaseValues(st, db_index);
        this->FinaliseAdd(db, st);
    }
    catch(const wxSQLite3Exception& e)
    {
        wxLogError("TAccountEntry:Add: %s", e.GetMessage().c_str());
    }

    return id_;
}

void TAccountEntry::Delete(wxSQLite3Database* db)
{
    DeleteEntry(db, "delete from ACCOUNTLIST_V1 where ACCOUNTID = ?");
}

void TAccountEntry::Update(wxSQLite3Database* db)
{
    try
    {
        const char SQL_STATEMENT[] =
        "update ACCOUNTLIST_V1 set "
        " ACCOUNTNAME = ?, ACCOUNTTYPE = ?, ACCOUNTNUM = ?, STATUS = ?, NOTES = ?, "
        " HELDAT = ?, WEBSITE = ?, CONTACTINFO=?,  ACCESSINFO = ?, INITIALBAL = ?, "
        " FAVORITEACCT = ?, CURRENCYID = ? "
        "where ACCOUNTID = ?";
    
        wxSQLite3Statement st = db->PrepareStatement(SQL_STATEMENT);
        int db_index = 0;
        SetDatabaseValues(st, db_index);
        st.Bind(++db_index, id_);

        FinaliseStatement(st);
    }
    catch(const wxSQLite3Exception& e)
    {
        wxLogError("TAccountEntry:Update: %s", e.GetMessage().c_str());
    }
}

/************************************************************************************
 TAccountList Methods
 ***********************************************************************************/
/// Constructor
TAccountList::TAccountList(std::shared_ptr<wxSQLite3Database> db
, TCurrencyList& currency_list, bool load_entries)
: TListBase(db)
, currency_list_(currency_list)
{
    LoadEntries(load_entries);
}

void TAccountList::LoadEntries(bool load_entries)
{
    try
    {
        if (!db_->TableExists("ACCOUNTLIST_V1"))
        {
            const char CREATE_TABLE_ACCOUNTLIST_V1[] =
            "CREATE TABLE ACCOUNTLIST_V1(ACCOUNTID integer primary key, "
            "ACCOUNTNAME TEXT NOT NULL UNIQUE, ACCOUNTTYPE TEXT NOT NULL, ACCOUNTNUM TEXT, "
            "STATUS TEXT NOT NULL, "
            "NOTES TEXT, HELDAT TEXT, WEBSITE TEXT, CONTACTINFO TEXT, "
            "ACCESSINFO TEXT, INITIALBAL numeric, FAVORITEACCT TEXT NOT NULL, "
            "CURRENCYID integer NOT NULL)";

            db_->ExecuteUpdate(CREATE_TABLE_ACCOUNTLIST_V1);
        }

        if (load_entries)
        {
            LoadEntriesUsing("select * from ACCOUNTLIST_V1 order by ACCOUNTNAME");
        }
    }
    catch (const wxSQLite3Exception& e)
    {
        wxLogError("TAccountList:LoadEntries: %s", e.GetMessage().c_str());
    }
}

/// Allows specialised list loading provided by SQL statement
void TAccountList::LoadEntriesUsing(const wxString& sql_statement)
{
    try
    {
        entrylist_.clear();
        wxSQLite3ResultSet q1 = db_->ExecuteQuery(sql_statement);
        while (q1.NextRow())
        {
            std::shared_ptr<TAccountEntry> pEntry(new TAccountEntry(q1));
            entrylist_.push_back(pEntry);
        }
        q1.Finalize();
    }
    catch (const wxSQLite3Exception& e)
    {
        wxLogError("TAccountList:LoadEntriesUsing: %s", e.GetMessage().c_str());
    }
}

int TAccountList::AddEntry(TAccountEntry* pAccountEntry)
{
    std::shared_ptr<TAccountEntry> pEntry(pAccountEntry);
    entrylist_.push_back(pEntry);
    pEntry->Add(ListDatabase());

    return pEntry->id_;
}

void TAccountList::DeleteEntry(int account_id)
{
    std::shared_ptr<TAccountEntry> pEntry = GetEntryPtr(account_id);
    if (pEntry)
    {
        pEntry->Delete(db_.get());
        entrylist_.erase(entrylist_.begin() + current_index_);
    }
}

void TAccountList::DeleteEntry(const wxString& account_name)
{
    DeleteEntry(GetAccountId(account_name));
}

//-----------------------------------------------------------------------------

std::shared_ptr<TAccountEntry> TAccountList::GetEntryPtr(int account_id)
{
    std::shared_ptr<TAccountEntry> pEntry;
    size_t list_size = entrylist_.size();
    size_t index = 0;

    while (index < list_size)
    {
        if (entrylist_[index]->id_ == account_id)
        {
            pEntry = entrylist_[index];
            current_index_ = index;
            break;
        }
        ++ index;
    }

    return pEntry;
}

std::shared_ptr<TAccountEntry> TAccountList::GetEntryPtr(const wxString& name)
{
    std::shared_ptr<TAccountEntry> pEntry;
    size_t list_size = entrylist_.size();
    size_t index = 0;

    while (index < list_size)
    {
        if (entrylist_[index]->acc_name_ == name)
        {
            pEntry = entrylist_[index];
            current_index_ = index;
            break;
        }
        ++ index;
    }

    return pEntry;
}

std::shared_ptr<TAccountEntry> TAccountList::GetIndexedEntryPtr(unsigned int list_index)
{
    std::shared_ptr<TAccountEntry> pEntry;
    if (list_index < entrylist_.size())
    {
        pEntry = entrylist_[list_index];
    }

    return pEntry;
}

int TAccountList::GetAccountId(const wxString& account_name)
{
    int account_id = -1;
    std::shared_ptr<TAccountEntry> pEntry = GetEntryPtr(account_name);
    if (pEntry)
    {
        account_id = pEntry->GetId();
    }

    return account_id;
}

wxString TAccountList::GetAccountName(int account_id)
{
    wxString account_name;
    std::shared_ptr<TAccountEntry> pEntry = GetEntryPtr(account_id);
    if (pEntry)
    {
        account_name = pEntry->acc_name_;
    }

    return account_name;
}

bool TAccountList::AccountExists(const wxString& account_name)
{
    bool result = false;
    if (GetEntryPtr(account_name))
    {
        result = true;
    }
    return result;
}

int TAccountList::NumberOfAccounts(int account_type)
{
    size_t list_size = entrylist_.size();
    size_t index = 0;
    int count = 0;

    while (index < list_size)
    {
        if (entrylist_[index]->acc_type_ == ACCOUNT_TYPE_DEF[account_type])
        {
            ++ count;
        }
        ++ index;
    }

    return count;
}

int TAccountList::NumberOfBankAccounts()
{
    return NumberOfAccounts(TAccountEntry::TYPE_BANK);
}

int TAccountList::NumberOfTermAccounts()
{
    return NumberOfAccounts(TAccountEntry::TYPE_TERM);
}

int TAccountList::NumberOfStockAccounts()
{
    return NumberOfAccounts(TAccountEntry::TYPE_STOCK);
}
