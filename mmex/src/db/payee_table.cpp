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

#include "payee_table.h"

/************************************************************************************
 TPayeeEntry Methods
 ***********************************************************************************/
/// Constructor used when loading payees from the database
TPayeeEntry::TPayeeEntry(wxSQLite3ResultSet& q1)
: TEntryBase()
{
    id_        = q1.GetInt(wxT("PAYEEID"));
    name_      = q1.GetString(wxT("PAYEENAME"));
    cat_id_    = q1.GetInt(wxT("CATEGID"));
    subcat_id_ = q1.GetInt(wxT("SUBCATEGID"));
}

/// Constructor for creating a new category entry
TPayeeEntry::TPayeeEntry(const wxString& name)
: TEntryBase()
, name_(name)
, cat_id_(-1)
, subcat_id_(-1)
{}

int TPayeeEntry::Add(wxSQLite3Database* db)
{
    const char INSERT_INTO_PAYEE_V1[] =
    "INSERT INTO PAYEE_V1 (PAYEENAME, CATEGID, SUBCATEGID) VALUES(?, ?, ?)";
    
    wxSQLite3Statement st = db->PrepareStatement(INSERT_INTO_PAYEE_V1);
        st.Bind(1, name_);
        st.Bind(2, cat_id_);
        st.Bind(3, subcat_id_);

    this->FinaliseAdd(db, st);

    return id_;
}

void TPayeeEntry::Delete(wxSQLite3Database* db)
{
    DeleteEntry(db, wxT("delete from PAYEE_V1 where PAYEEID = ?"));
}

void TPayeeEntry::Update(wxSQLite3Database* db)
{
    static const char UPDATE_PAYEE_V1[] =
    "UPDATE PAYEE_V1 SET PAYEENAME = ?, "
        "CATEGID = ?, SUBCATEGID = ? "
    "WHERE PAYEEID = ?";
    try
    {
        wxSQLite3Statement st = db->PrepareStatement(UPDATE_PAYEE_V1);
        st.Bind(1, name_);
        st.Bind(2, cat_id_);
        st.Bind(3, subcat_id_);
        st.Bind(4, id_);

        this->FinaliseStatement(st);
    }
    catch(const wxSQLite3Exception& e)
    {
        //wxLogDebug(wxT("TPayeeEntry:Update: %s"), e.GetMessage().c_str());
        wxLogError(wxT("TPayeeEntry:Update: %s"), e.GetMessage().c_str());
    }
}

/************************************************************************************
 TPayeeList Methods
 ***********************************************************************************/
/// Constructor
TPayeeList::TPayeeList(boost::shared_ptr<wxSQLite3Database> db)
: TListBase(db)
{
    LoadEntries();
}

void TPayeeList::LoadEntries()
{
    try
    {
        if (!db_->TableExists(wxT("PAYEE_V1")))
        {
            const char CREATE_TABLE_PAYEE_V1[]=
            "CREATE TABLE PAYEE_V1(PAYEEID integer primary key, "
            "PAYEENAME TEXT NOT NULL UNIQUE, CATEGID integer, SUBCATEGID integer)";

            db_->ExecuteUpdate(CREATE_TABLE_PAYEE_V1);
        }

        const char SELECT_ALL_FROM_PAYEE_V1[] =
        "SELECT PAYEEID, PAYEENAME, CATEGID, SUBCATEGID "
        "FROM PAYEE_V1 "
        "ORDER BY PAYEENAME";

        wxSQLite3ResultSet q1 = db_->ExecuteQuery(SELECT_ALL_FROM_PAYEE_V1);
        while (q1.NextRow())
        {
            boost::shared_ptr<TPayeeEntry> pEntry(new TPayeeEntry(q1));
            entrylist_.push_back(pEntry);
        }
        q1.Finalize();
    }
    catch (const wxSQLite3Exception& e)
    {
        //wxLogDebug(wxT("TPayeeList:LoadEntries: %s"), e.GetMessage().c_str());
        wxLogError(wxT("TPayeeList:LoadEntries: %s"), e.GetMessage().c_str());
    }
}

int TPayeeList::AddEntry(const wxString& name, wxString category, wxString subcategory)
{
    int payee_id = -1;
    if (PayeeExists(name))
    {
        payee_id = entrylist_[current_index_]->id_;
    }
    else
    {
        boost::shared_ptr<TPayeeEntry> pEntry(new TPayeeEntry(name));
        entrylist_.push_back(pEntry);
        payee_id = pEntry->Add(db_.get());
    }

    return payee_id;
}

void TPayeeList::UpdateEntry(int payee_id, const wxString& new_payee_name, int cat_id, int subcat_id)
{
    boost::shared_ptr<TPayeeEntry> pEntry = GetEntryPtr(payee_id);
    pEntry->name_ = new_payee_name;
    if (cat_id > 0) pEntry->cat_id_ = cat_id;
    if (subcat_id > 0) pEntry->subcat_id_ = subcat_id;
    pEntry->Update(db_.get());
}

void TPayeeList::UpdateEntry(const wxString& payee_name, int cat_id, int subcat_id)
{
    boost::shared_ptr<TPayeeEntry> pEntry = GetEntryPtr(payee_name);
    pEntry->cat_id_ = cat_id;
    pEntry->subcat_id_ = subcat_id;
    pEntry->Update(db_.get());
}

void TPayeeList::DeleteEntry(int payee_id)
{
    boost::shared_ptr<TPayeeEntry> pEntry = GetEntryPtr(payee_id);
    if (pEntry)
    {
        pEntry->Delete(db_.get());
        entrylist_.erase(entrylist_.begin() + current_index_);
    }
}

void TPayeeList::DeleteEntry(wxString payee_name)
{
    DeleteEntry(GetPayeeId(payee_name));
}

//-----------------------------------------------------------------------------

boost::shared_ptr<TPayeeEntry> TPayeeList::GetEntryPtr(int payee_id)
{
    boost::shared_ptr<TPayeeEntry> pEntry;
    size_t list_size = entrylist_.size();
    size_t index = 0;

    while (index < list_size)
    {
        if (entrylist_[index]->id_ == payee_id)
        {
            pEntry = entrylist_[index];
            current_index_ = index;
            break;
        }
        ++ index;
    }

    return pEntry;
}

boost::shared_ptr<TPayeeEntry> TPayeeList::GetEntryPtr(const wxString& name)
{
    boost::shared_ptr<TPayeeEntry> pEntry;
    size_t list_size = entrylist_.size();
    size_t index = 0;

    while (index < list_size)
    {
        if (entrylist_[index]->name_ == name)
        {
            pEntry = entrylist_[index];
            current_index_ = index;
            break;
        }
        ++ index;
    }

    return pEntry;
}

int TPayeeList::GetPayeeId(const wxString& payee_name)
{
    int payee_id = -1;
    boost::shared_ptr<TPayeeEntry> pEntry = GetEntryPtr(payee_name);
    if (pEntry)
    {
        payee_id = pEntry->GetId();
    }

    return payee_id;
}

wxString TPayeeList::GetPayeeName(int payee_id)
{
    wxString payee_name;
    boost::shared_ptr<TPayeeEntry> pEntry = GetEntryPtr(payee_id);
    if (pEntry)
    {
        payee_name = pEntry->name_;
    }

    return payee_name;
}

bool TPayeeList::PayeeExists(const wxString& payeeName)
{
    bool payee_result = false;
    if (GetEntryPtr(payeeName))
    {
        payee_result = true;
    }
    return payee_result;
}

