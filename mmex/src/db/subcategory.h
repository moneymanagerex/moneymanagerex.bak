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

/************************************************************************************
 Class TSubCategoryEntry
 ***********************************************************************************/
class TSubCategoryEntry : public TEntryBase
{
private:
    friend class TSubCategoryList;

    int Add(wxSQLite3Database* db);
    void Delete(wxSQLite3Database* db);

public:
    int cat_id_;
    wxString name_;

    /// Constructor used when loading subcategories from the database.
    TSubCategoryEntry(wxSQLite3ResultSet& q1);

    /// Constructor for creating a new subcategory entry
    TSubCategoryEntry(int cat_id, const wxString& name);

    void Update(wxSQLite3Database* db);
};

/************************************************************************************
 Class TSubCategoryList
 ***********************************************************************************/
class TSubCategoryList : public TListBase
{
private:
    void LoadEntries(int subcat_id = -1);

public:
    std::vector<wxSharedPtr<TSubCategoryEntry> > entrylist_;

    TSubCategoryList(wxSharedPtr<wxSQLite3Database> db, int cat_id = -1);

    int AddEntry(int cat_id, const wxString& name);
    void UpdateEntry(int cat_id, int subcat_id, const wxString& new_category);

    /// Note: At this level, no checking is done for usage in other tables.
    void DeleteEntry(int cat_id, int subcat_id);

    wxSharedPtr<TSubCategoryEntry> GetEntryPtr(int cat_id, const wxString& name);
    wxSharedPtr<TSubCategoryEntry> GetEntryPtr(int cat_id, int subcat_id);
    int GetSubCategoryId(int cat_id, const wxString& name);
    wxString GetSubCategoryName(int cat_id, int subcat_id);
    bool SubCategoryExists(int cat_id, const wxString& name);
};
