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

#include "subcategory.h"

/************************************************************************************
 TSubCategoryEntry Methods
 ***********************************************************************************/
TSubCategoryEntry::TSubCategoryEntry(wxSQLite3ResultSet& q1)
: TEntryBase()
{
    id_     = q1.GetInt(wxT("SUBCATEGID"));
    name_   = q1.GetString(wxT("SUBCATEGNAME"));
    cat_id_ = q1.GetInt(wxT("CATEGID"));
}

TSubCategoryEntry::TSubCategoryEntry(int cat_id, const wxString& name)
: TEntryBase()
, cat_id_(cat_id)
, name_(name)
{}

int TSubCategoryEntry::Add(wxSQLite3Database* db)
{
    const char INSERT_INTO_SUBCATEGORY_V1[] =
    "insert into SUBCATEGORY_V1 (SUBCATEGNAME, CATEGID) values(?, ?)";

    wxSQLite3Statement st = db->PrepareStatement(INSERT_INTO_SUBCATEGORY_V1);
    st.Bind(1, name_);
    st.Bind(2, cat_id_);

    FinaliseAdd(db, st);
    
    return id_;
}

void TSubCategoryEntry::Delete(wxSQLite3Database* db)
{
    DeleteEntry(db, wxT("delete from SUBCATEGORY_V1 where SUBCATEGID = ?"));
}

void TSubCategoryEntry::Update(wxSQLite3Database* db)
{
    const char UPDATE_ALL_SUBCATEGORY_V1[] =
    "update SUBCATEGORY_V1 set SUBCATEGNAME = ?, CATEGID = ? "
    "where SUBCATEGID = ?";
    try
    {
        wxSQLite3Statement st = db->PrepareStatement(UPDATE_ALL_SUBCATEGORY_V1);
        st.Bind(1, name_);
        st.Bind(2, cat_id_);
        st.Bind(3, id_);

        this->FinaliseStatement(st);
    }
    catch(const wxSQLite3Exception& e)
    {
        //wxLogDebug(wxT("TSubCategoryEntry:Update: %s "), e.GetMessage().c_str());
        wxLogError(wxT("TSubCategoryEntry:Update: %s "), e.GetMessage().c_str());
    }
}

/************************************************************************************
 TSubCategoryList Methods
 ***********************************************************************************/
/// Constructor
TSubCategoryList::TSubCategoryList(wxSharedPtr<wxSQLite3Database> db, int cat_id)
: TListBase(db)
{
    LoadEntries(cat_id);
}

void TSubCategoryList::LoadEntries(int cat_id)
{
    try
    {
        if (!db_->TableExists(wxT("SUBCATEGORY_V1")))
        {
            const char CREATE_TABLE_SUBCATEGORY_V1[]=
            "CREATE TABLE SUBCATEGORY_V1(SUBCATEGID integer primary key, "
            "SUBCATEGNAME TEXT NOT NULL, CATEGID integer NOT NULL)";

            db_->ExecuteUpdate(CREATE_TABLE_SUBCATEGORY_V1);
        }

        wxString sql_statement = wxT("select SUBCATEGID, SUBCATEGNAME, CATEGID from SUBCATEGORY_V1");
        if (cat_id > 0)
        {
            sql_statement << wxT(" where CATEGID = ") << cat_id;
        }

        wxSQLite3ResultSet q1 = db_->ExecuteQuery(sql_statement);
        while (q1.NextRow())
        {
            wxSharedPtr<TSubCategoryEntry> pSubCategory(new TSubCategoryEntry(q1));
            entrylist_.push_back(pSubCategory);
        }
        q1.Finalize();
    }
    catch (const wxSQLite3Exception& e)
    {
        wxString msg = wxT("LoadSubCategoryEntries ");
        wxLogDebug(msg, e.GetMessage().c_str());
        wxLogError(msg + wxString::Format(_("Error: %s"), e.GetMessage().c_str()));
    }
}

int TSubCategoryList::AddEntry(int cat_id, const wxString& name)
{
    int subcat_id;
    if (SubCategoryExists(cat_id, name))
    {
        subcat_id = entrylist_[current_index_]->id_;
    }
    else
    {
        wxSharedPtr<TSubCategoryEntry> pSubCategory(new TSubCategoryEntry(cat_id, name));
        entrylist_.push_back(pSubCategory);
        subcat_id = pSubCategory->Add(db_.get());
    }

    return subcat_id;
}

void TSubCategoryList::UpdateEntry(int cat_id, int subcat_id, const wxString& new_category)
{
    wxSharedPtr<TSubCategoryEntry> pSubCategory = GetEntryPtr(cat_id, subcat_id);
    pSubCategory->name_ = new_category;
    pSubCategory->Update(db_.get());
}

/// Note: At this level, no checking is done for usage in other tables.
void TSubCategoryList::DeleteEntry(int cat_id, int subcat_id)
{
    wxSharedPtr<TSubCategoryEntry> pSubCatEntry = GetEntryPtr(cat_id, subcat_id);
    pSubCatEntry->Delete(db_.get());
    entrylist_.erase(entrylist_.begin() + current_index_);    
}

wxSharedPtr<TSubCategoryEntry> TSubCategoryList::GetEntryPtr(int cat_id, int subcat_id)
{
    wxSharedPtr<TSubCategoryEntry> pSubCatEntry;
    size_t index = 0;
    bool searching = entrylist_.size() != 0;
    while (searching && index < entrylist_.size())
    {
        if (entrylist_[index]->id_ == subcat_id &&
            entrylist_[index]->cat_id_ == cat_id)
        {
            searching = false;
            pSubCatEntry = entrylist_[index];
            current_index_ = index;
        }
        ++ index;
    }

    return pSubCatEntry;
}

wxSharedPtr<TSubCategoryEntry> TSubCategoryList::GetEntryPtr(int cat_id, const wxString& name)
{
    wxSharedPtr<TSubCategoryEntry> pSubCatEntry;
    size_t index = 0;
    bool searching = entrylist_.size() != 0;
    while (searching && index < entrylist_.size())
    {
        if (entrylist_[index]->cat_id_ == cat_id &&
            entrylist_[index]->name_   == name)
        {
            searching = false;
            pSubCatEntry = entrylist_[index];
            current_index_ = index;
        }
        ++ index;
    }

    return pSubCatEntry;
}

int TSubCategoryList::GetSubCategoryId(int cat_id, const wxString& name)
{
    int subcat_id = -1;
    wxSharedPtr<TSubCategoryEntry> pSubCatEntry = GetEntryPtr(cat_id, name);
    if (pSubCatEntry)
    {
        subcat_id = pSubCatEntry->GetId();
    }

    return subcat_id;
}


wxString TSubCategoryList::GetSubCategoryName(int cat_id, int subcat_id)
{
    wxString subcat_name;
    wxSharedPtr<TSubCategoryEntry> pSubCatEntry = GetEntryPtr(cat_id, subcat_id);
    if (pSubCatEntry)
    {
        subcat_name = pSubCatEntry->name_;
    }

    return subcat_name;
}

bool TSubCategoryList::SubCategoryExists(int cat_id, const wxString& name)
{
    bool cat_exists = false;
    if (GetEntryPtr(cat_id, name))
    {
        cat_exists = true;
    }
    return cat_exists;
}
