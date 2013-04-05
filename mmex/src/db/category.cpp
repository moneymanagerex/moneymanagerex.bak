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

#include "category.h"

/************************************************************************************
 TCategoryEntry Methods
 ***********************************************************************************/
/// Constructor used when loading categories from the database
TCategoryEntry::TCategoryEntry(wxSQLite3ResultSet& q1)
: TEntryBase()
{
    id_   = q1.GetInt(wxT("CATEGID"));
    name_ = q1.GetString(wxT("CATEGNAME"));
}

/// Constructor for creating a new category entry
TCategoryEntry::TCategoryEntry(const wxString& name)
: TEntryBase()
, name_(name)
{}

int TCategoryEntry::Add(wxSQLite3Database* db)
{
    const char INSERT_INTO_CATEGORY_V1[] =
    "INSERT INTO CATEGORY_V1 (CATEGNAME) VALUES(?)";
    
    wxSQLite3Statement st = db->PrepareStatement(INSERT_INTO_CATEGORY_V1);
    st.Bind(1, name_);

    this->FinaliseAdd(db, st);

    return id_;
}

void TCategoryEntry::Delete(wxSQLite3Database* db)
{
    DeleteEntry(db, wxT("delete from CATEGORY_V1 where CATEGID = ?"));
}

void TCategoryEntry::Update(wxSQLite3Database* db)
{
    const char UPDATE_CATEGORY_V1[] =
    "UPDATE CATEGORY_V1 SET CATEGNAME = ? "
    "WHERE CATEGID = ?";
    try
    {
        wxSQLite3Statement st = db->PrepareStatement(UPDATE_CATEGORY_V1);
        st.Bind(1, name_);
        st.Bind(2, id_);

        this->FinaliseStatement(st);
    }
    catch(const wxSQLite3Exception& e)
    {
        //wxLogDebug(wxT("TCategoryEntry::Update %s"), e.GetMessage().c_str());
        wxLogError(wxT("TCategoryEntry:Update: %s"), e.GetMessage().c_str());
    }
}

/************************************************************************************
 TCategoryList Methods
 ***********************************************************************************/
/// Constructor
TCategoryList::TCategoryList(boost::shared_ptr<wxSQLite3Database> db)
: TListBase(db)
{
    LoadEntries();
}

void TCategoryList::LoadEntries()
{
    try
    {
        if (!db_->TableExists(wxT("CATEGORY_V1")))
        {
            const char CREATE_TABLE_CATEGORY_V1[]=
            "CREATE TABLE CATEGORY_V1(CATEGID integer primary key, "
            "CATEGNAME TEXT NOT NULL)";

            db_->ExecuteUpdate(CREATE_TABLE_CATEGORY_V1);
        }

        wxString sql_statement = wxT("select CATEGID, CATEGNAME from CATEGORY_V1");
        wxSQLite3ResultSet q1 = db_->ExecuteQuery(sql_statement);
        while (q1.NextRow())
        {
            boost::shared_ptr<TCategoryEntry> pCategory(new TCategoryEntry(q1));
            entrylist_.push_back(pCategory);
        }
        q1.Finalize();
    }
    catch (const wxSQLite3Exception& e)
    {
        //wxLogDebug(wxT("TCategoryList:LoadEntries: %s"), e.GetMessage().c_str());
        wxLogError(wxT("TCategoryList:LoadEntries: %s"), e.GetMessage().c_str());
    }
}

int TCategoryList::AddEntry(const wxString& name)
{
    int cat_id = -1;
    if (CategoryExists(name))
    {
        cat_id = entrylist_[current_index_]->id_;
    }
    else
    {
        boost::shared_ptr<TCategoryEntry> pEntry(new TCategoryEntry(name));
        entrylist_.push_back(pEntry);
        cat_id = pEntry->Add(db_.get());
    }

    return cat_id;
}

void TCategoryList::UpdateEntry(int cat_id, const wxString& new_category)
{
    boost::shared_ptr<TCategoryEntry> pEntry = GetEntryPtr(cat_id);
    pEntry->name_ = new_category;
    pEntry->Update(db_.get());
}

/// Note: At this level, no checking is done for usage in other tables.
void TCategoryList::DeleteEntry(int cat_id)
{
    boost::shared_ptr<TCategoryEntry> pEntry = GetEntryPtr(cat_id);
    pEntry->Delete(db_.get());
    entrylist_.erase(entrylist_.begin() + current_index_);
}

//-----------------------------------------------------------------------------

boost::shared_ptr<TCategoryEntry> TCategoryList::GetEntryPtr(int cat_id)
{
    boost::shared_ptr<TCategoryEntry> pEntry;
    size_t index = 0;
    bool searching = entrylist_.size() != 0;
    while (searching && index < entrylist_.size())
    {
        if (entrylist_[index]->id_ == cat_id)
        {
            searching = false;
            pEntry = entrylist_[index];
            current_index_ = index;
        }
        ++ index;
    }

    return pEntry;
}

boost::shared_ptr<TCategoryEntry> TCategoryList::GetEntryPtr(const wxString& name)
{
    boost::shared_ptr<TCategoryEntry> pEntry;
    size_t index = 0;
    bool searching = entrylist_.size() != 0;
    while (searching && index < entrylist_.size())
    {
        if (entrylist_[index]->name_ == name)
        {
            searching = false;
            pEntry = entrylist_[index];
            current_index_ = index;
        }
        ++ index;
    }

    return pEntry;
}

int TCategoryList::GetCategoryId(const wxString& name)
{
    int cat_id = -1;
    boost::shared_ptr<TCategoryEntry> pEntry = GetEntryPtr(name);
    if (pEntry)
    {
        cat_id = pEntry->GetId();
    }

    return cat_id;
}

wxString TCategoryList::GetCategoryName(int cat_id)
{
    wxString cat_name;
    boost::shared_ptr<TCategoryEntry> pEntry = GetEntryPtr(cat_id);
    if (pEntry)
    {
        cat_name = pEntry->name_;
    }

    return cat_name;
}

bool TCategoryList::CategoryExists(const wxString& name)
{
    bool cat_exists = false;
    if (GetEntryPtr(name))
    {
        cat_exists = true;
    }

    return cat_exists;
}
