/*******************************************************
 Copyright (C) 2006 Madhan Kanagavel
 Copyright (C) 2011 Stefano Giorgio

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

#ifndef _MM_EX_MMCATEGORY_H_
#define _MM_EX_MMCATEGORY_H_

#define ERR_CAT_USED1 101
#define ERR_CAT_USED2 102
#define ERR_CAT_USED3 103
#define ERR_CAT_USED4 104
#define ERR_CAT_USED5 105
#define ERR_SUBCAT_USED1 111
#define ERR_SUBCAT_USED2 112
#define ERR_SUBCAT_USED3 113
#define ERR_SUBCAT_USED4 114
#include "defs.h"

#include <boost/weak_ptr.hpp>
#include <vector>

/*****************************************************************************
Class: Category_Table
******************************************************************************/
class Category_Table
{
public:
    Category_Table(wxSQLite3Database* db);

    /// Add the name and return the ID
    int AddName(const wxString& name);
    /// Get the category name, for the given ID
    wxString GetName(int id);
    /// Get the category ID, for the given name
    int GetID(const wxString& name);
    /// update the category name, for the given ID
    bool UpdateName(const wxString& name, int id);
    bool DeleteCategory(int id);
    bool DeleteCategory(const wxString& name);

private:
    wxSQLite3Database* db_;
};

/*****************************************************************************
Class SubCategory_Table
******************************************************************************/
class SubCategory_Table
{
public:
    SubCategory_Table(wxSQLite3Database* db);

    /// Add the name for the Category ID and return the Subcategory ID
    int AddName(const wxString& name, int cat_id);
    /// Get the subcategory name given the category ID and subcategory ID
    wxString GetName(int cat_id, int subcat_id);
    /// get the id given the name and category ID
    int GetID(const wxString& name, int cat_id);
    /// update the subcategory name, for the given cat ID and subcat ID
    bool UpdateName(const wxString& name, int cat_id, int subcat_id);

private:
    wxSQLite3Database* db_;
};

/*****************************************************************************
Class mmCategory
******************************************************************************/
class mmCategory
{
public:
    mmCategory(int id, const wxString& name): categID_(id), categName_(name) {}
    ~mmCategory() {}

    /* Public Data */
    int categID_;
    wxString categName_;
    boost::weak_ptr<mmCategory> parent_;
    std::vector<boost::shared_ptr<mmCategory> > children_;
};

/*****************************************************************************
Class mmCategoryList
******************************************************************************/
class mmCategoryList
{
public:
    mmCategoryList(boost::shared_ptr<wxSQLite3Database> db);
//    ~mmCategoryList() {};

    /* Category Functions */
    int getID(const wxString& categoryName) const;
    wxString getCategoryName(int id) const;
    bool categoryExists(const wxString& categoryName) const;
    boost::shared_ptr<mmCategory> getCategorySharedPtr(int category, int subcategory) const;
    int getSubCategoryID(int parentID, const wxString& subCategoryName) const;

    int addCategory(const wxString& category);
    int addSubCategory(int parentID, const wxString& text);

    int deleteCategory(int categID);
    int deleteSubCategory(int categID, int subCategID);
    int deleteCategoryWithConstraints(int categID);
    int deleteSubCategoryWithConstraints(int categID, int subcategID);

    bool updateCategory(int categID, int subCategID, const wxString& text);
    wxString GetCategoryString(int categID) const;
    wxString GetSubCategoryString(int categID, int subCategID) const;
    wxString GetFullCategoryString(int categID, int subCategID) const;

    /* Public Data */
    std::vector< boost::shared_ptr<mmCategory> > entries_;
    typedef std::vector< boost::shared_ptr<mmCategory> >::const_iterator const_iterator;
    std::pair<const_iterator, const_iterator> range() const { return std::make_pair(entries_.begin(), entries_.end()); }

private:
    boost::shared_ptr<wxSQLite3Database> db_;
    Category_Table category_table_;
    SubCategory_Table subCategory_table_;
};
#endif
