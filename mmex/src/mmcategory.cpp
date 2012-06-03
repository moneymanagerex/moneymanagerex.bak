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

#include "mmcategory.h"
#include "dbwrapper.h"
#include "mmex_db_view.h"
#include "util.h"

/*****************************************************************************
Class: Category_Table Methods
******************************************************************************/
Category_Table::Category_Table(wxSQLite3Database* db):db_(db)
{}

// Add the name and return the ID
int Category_Table::AddName(const wxString& name)
{
    DB_View_CATEGORY_V1::Data* category = CATEGORY_V1.create();
    category->CATEGNAME = name;
    category->save(db_);

    return GetID(name);
}

// Get the category name, for the given ID
wxString Category_Table::GetName(int id)
{
    DB_View_CATEGORY_V1::Data_Set record = CATEGORY_V1.find(db_
        , DB_View_CATEGORY_V1::COL_CATEGID, id);

    wxString name;
    if (record.size() > 0) name = record[0].CATEGNAME;
    
    return name;
}

// Get the category ID, for the given name
int Category_Table::GetID(const wxString& name)
{
    DB_View_CATEGORY_V1::Data_Set record = CATEGORY_V1.find(db_
        , DB_View_CATEGORY_V1::COL_CATEGNAME
        , name);

    int cat_id = -1;
    if (record.size() > 0) cat_id = record[0].CATEGID;

    return cat_id;
}

// update the category name, for the given ID
bool Category_Table::UpdateName(const wxString& name, int id)
{
    bool result = false;
    DB_View_CATEGORY_V1::Data_Set record
        = CATEGORY_V1.find(db_, DB_View_CATEGORY_V1::COL_CATEGID, id);

    if (record.size() > 0)
    {
        record[0].CATEGNAME = name;
        result = record[0].save(db_);
    }
    return result;
}

bool Category_Table::DeleteCategory(int id)
{
    // leave a marker for code to be completed
    int To_Do = id;
    //TODO: complete this

    return false;
}

bool Category_Table::DeleteCategory(const wxString& name)
{
    // leave a marker for code to be completed
    wxString dummy = name;
    int To_Do = 0;
    //TODO: complete this

    return false;
}

/*****************************************************************************
Class: SubCategory_Table Methods
******************************************************************************/
SubCategory_Table::SubCategory_Table(wxSQLite3Database* db):db_(db)
{}

// Add the name for the Category ID and return the Subcategory ID
int SubCategory_Table::AddName(const wxString& name, int cat_id)
{
    DB_View_SUBCATEGORY_V1::Data* subcategory = SUBCATEGORY_V1.create();
    subcategory->CATEGID = cat_id;
    subcategory->SUBCATEGNAME = name;
    subcategory->save(db_);

    return GetID(name, cat_id);
}

// Get the subcategory name given the category ID and subcategory ID
wxString SubCategory_Table::GetName(int cat_id, int subcat_id)
{
    DB_View_SUBCATEGORY_V1::Data_Set record 
        = SUBCATEGORY_V1.find(db_
        , DB_View_SUBCATEGORY_V1::COL_CATEGID, cat_id
        , DB_View_SUBCATEGORY_V1::COL_SUBCATEGID, subcat_id);

    wxString name;
    if (record.size() > 0) name = record[0].SUBCATEGNAME;
    
    return name;
}

// Get the id given the name
int SubCategory_Table::GetID(const wxString& name, int cat_id)
{
    DB_View_SUBCATEGORY_V1::Data_Set record 
        = SUBCATEGORY_V1.find(db_
        , DB_View_SUBCATEGORY_V1::COL_CATEGID, cat_id
        , DB_View_SUBCATEGORY_V1::COL_SUBCATEGNAME, name);

    int subcat_id = -1;
    if (record.size() > 0) subcat_id = record[0].SUBCATEGID;

    return subcat_id;
}

// update the subcategory name, for the given cat ID and subcat ID
bool SubCategory_Table::UpdateName(const wxString& name, int cat_id, int subcat_id)
{
    bool result = false;
    DB_View_SUBCATEGORY_V1::Data_Set record 
        = SUBCATEGORY_V1.find(db_
        , DB_View_SUBCATEGORY_V1::COL_CATEGID, cat_id
        , DB_View_SUBCATEGORY_V1::COL_SUBCATEGID, subcat_id);

    if (record.size() > 0)
    {
        record[0].SUBCATEGNAME = name;
        result = record[0].save(db_);
    }

    return result;
}


/*****************************************************************************
Class: mmCategoryList Methods
******************************************************************************/
bool mmCategoryList::categoryExists(const wxString& categoryName) const
{
    for (const_iterator it = entries_.begin(); it != entries_.end(); ++ it)
    {
        if (! (*it)->categName_.CmpNoCase(categoryName))
            return true;
    }

    return false;
}

int mmCategoryList::getID(const wxString& categoryName) const
{
    for (const_iterator it = entries_.begin(); it != entries_.end(); ++ it)
    {
        if ((*it)->categName_ == categoryName)
            return (*it)->categID_;
    }

    return -1;
}

wxString mmCategoryList::getCategoryName(int id) const
{
    for (const_iterator it = entries_.begin(); it != entries_.end(); ++ it)
    {
        if ((*it)->categID_ == id) return (*it)->categName_;
    }

    return wxEmptyString;
}

int mmCategoryList::addCategory(const wxString& name)
{
    DB_View_CATEGORY_V1::Data* category = CATEGORY_V1.create();
    category->CATEGNAME = name;
    if(!category->save(db_.get()))
        return -1;

    int cID = category->id();

    boost::shared_ptr<mmCategory> pCategory(new mmCategory(cID, name));
    entries_.push_back(pCategory);

    return cID;
}

int mmCategoryList::getSubCategoryID(int parentID, const wxString& subCategoryName) const
{
    DB_View_SUBCATEGORY_V1::Data_Set sub_category = SUBCATEGORY_V1.find(db_.get(), DB_View_SUBCATEGORY_V1::COL_CATEGID, parentID, DB_View_SUBCATEGORY_V1::COL_SUBCATEGNAME, subCategoryName, true);

    return sub_category.empty() ? -1: sub_category[0].SUBCATEGID;
}

int mmCategoryList::addSubCategory(int parentID, const wxString& text)
{
    DB_View_SUBCATEGORY_V1::Data* sub_category = SUBCATEGORY_V1.create();
    sub_category->CATEGID = parentID;
    sub_category->SUBCATEGNAME = text;

    if (!sub_category->save(db_.get()))
        return -1;

    int cID = sub_category->id();

    boost::shared_ptr<mmCategory> categ = getCategorySharedPtr(parentID, -1);

    boost::shared_ptr<mmCategory> subCateg(new mmCategory(cID, text));
    subCateg->parent_ = categ;
    categ->children_.push_back(subCateg);

    return cID;
}

boost::shared_ptr<mmCategory> mmCategoryList::getCategorySharedPtr(int category, int subcategory) const
{
    if (category != -1)
    {
        int numCategory = (int)entries_.size();
        for (int idx = 0; idx < numCategory; idx++)
        {
            if (entries_[idx]->categID_ == category)
            {
                if (subcategory == -1)
                    return entries_[idx];

                size_t numSubCategory = entries_[idx]->children_.size();
                for (size_t idxS = 0; idxS < numSubCategory; ++idxS)
                {
                    if (entries_[idx]->children_[idxS]->categID_ == subcategory)
                    {
                        return entries_[idx]->children_[idxS];
                    }
                }
            }
        }
    }
    boost::shared_ptr<mmCategory> categ;
    return categ;
}

int mmCategoryList::deleteCategory(int categID)
{
    int error_code = mmCategoryList::deleteCategoryWithConstraints(categID);
    if (error_code == wxID_OK)
    {
        std::vector <boost::shared_ptr<mmCategory> >::iterator Iter;
        for ( Iter = entries_.begin( ) ; Iter != entries_.end( ) ; Iter++ )
        {
            if ((*Iter)->categID_ == categID)
            {
                entries_.erase(Iter);
                return wxID_OK;
            }
        }
    }

    return error_code;
}

int mmCategoryList::deleteSubCategory(int categID, int subCategID)
{
    int error_code = mmCategoryList::deleteSubCategoryWithConstraints(categID, subCategID);
    if (error_code == wxID_OK)
    {
        boost::shared_ptr<mmCategory> categ = getCategorySharedPtr(categID, subCategID);
        boost::shared_ptr<mmCategory> parent = categ->parent_.lock();
        wxASSERT(parent);

        std::vector <boost::shared_ptr<mmCategory> >::iterator Iter;
        for ( Iter = parent->children_.begin( ) ; Iter != parent->children_.end( ) ; Iter++ )
        {
            if ((*Iter)->categID_ == subCategID)
            {
                parent->children_.erase(Iter);
                return wxID_OK;
            }
        }
    }

    return error_code;
}

bool mmCategoryList::updateCategory(int categID, int subCategID, const wxString& text)
{
    boost::shared_ptr<mmCategory> categ = getCategorySharedPtr(categID, subCategID);
    boost::shared_ptr<mmCategory> parent = categ->parent_.lock();

    if (categ->categName_ != text)
    {
        // check if the name already exists
        if (!parent)
        {
            if (categoryExists(text))
                return false;
        }
        else
        {
            // subcategory, check if siblings dont have this name already
            wxASSERT(parent);
            for (unsigned int idx = 0; idx < parent->children_.size(); idx++)
            {
                if (parent->children_[idx]->categName_ == text)
                    return false;
            }
        }
    }

    mmDBWrapper::updateCategory(db_.get(), categID, subCategID, text);
    categ->categName_ = text;
    return true;
}

wxString mmCategoryList::GetCategoryString(int categID) const
{
    wxString catName = this->getCategoryName(categID);
    catName.Replace (wxT("&"), wxT("&&"));

    return catName;
}

wxString mmCategoryList::GetSubCategoryString(int categID, int subCategID) const
{
    wxString subcatName = mmDBWrapper::getSubCategoryName(db_.get(), categID, subCategID);
    subcatName.Replace (wxT("&"), wxT("&&"));

    return subcatName;
}

wxString mmCategoryList::GetFullCategoryString(int categID, int subCategID) const
{
    wxString category    = GetCategoryString(categID);
    wxString subCategory = GetSubCategoryString(categID, subCategID);
    if (!subCategory.IsEmpty())
        category<< wxT(":") << subCategory;
    return category;
}

int mmCategoryList::deleteCategoryWithConstraints(int categID)
{
    DB_View_CHECKINGACCOUNT_V1::Data_Set all_trans =
        CHECKINGACCOUNT_V1.find(db_.get(), DB_View_CHECKINGACCOUNT_V1 ::COL_CATEGID, categID);
    if (!all_trans.empty()) // transactions existing
        return ERR_CAT_USED1; //error code more than zero

    DB_View_SPLITTRANSACTIONS_V1::Data_Set all_splittrans =
        SPLITTRANSACTIONS_V1.find(db_.get(), DB_View_SPLITTRANSACTIONS_V1::COL_CATEGID, categID);
    if (!all_splittrans.empty()) // split transactions existing
        return ERR_CAT_USED2;

    DB_View_BILLSDEPOSITS_V1::Data_Set all_bills =
        BILLSDEPOSITS_V1.find(db_.get(), DB_View_BILLSDEPOSITS_V1::COL_CATEGID, categID);
    if (!all_bills.empty()) // repeating transactions existing
        return ERR_CAT_USED3;

    DB_View_BUDGETSPLITTRANSACTIONS_V1::Data_Set all_budget_splittrans =
        BUDGETSPLITTRANSACTIONS_V1.find(db_.get(), DB_View_BUDGETSPLITTRANSACTIONS_V1::COL_CATEGID, categID);
    if (!all_budget_splittrans.empty()) // repeating transactions existing
        return ERR_CAT_USED4;

    try{
        static const char* sql_del[] =
        {
            "delete from SUBCATEGORY_V1     where CATEGID = ?",
            "delete from CATEGORY_V1        where CATEGID = ?",
            "delete from BUDGETTABLE_V1     WHERE CATEGID = ?",
            "update PAYEE_V1 set CATEGID=-1 WHERE CATEGID = ?",
            0
        };

        for (int i = 0; sql_del[i]; ++i)
        {
            wxSQLite3Statement st = db_.get()->PrepareStatement(sql_del[i]);
            st.Bind(1, categID);
            st.ExecuteUpdate();
            st.Finalize();
        }

        mmOptions::instance().databaseUpdated_ = true;
        return wxID_OK;

    } catch(const wxSQLite3Exception& e)
    {
        wxLogDebug(wxT("Database::deleteCategoryWithConstraints: Exception"), e.GetMessage().c_str());
        wxLogError(wxT("Delete Category with Constraints. ") + wxString::Format(_("Error: %s"), e.GetMessage().c_str()));
        return ERR_CAT_USED5;
    }
}

int mmCategoryList::deleteSubCategoryWithConstraints(int categID, int subcategID)
{
    DB_View_CHECKINGACCOUNT_V1::Data_Set all_trans =
        CHECKINGACCOUNT_V1.find(db_.get()
            , DB_View_CHECKINGACCOUNT_V1 ::COL_CATEGID, categID
            , DB_View_CHECKINGACCOUNT_V1 ::COL_SUBCATEGID, subcategID);
    if (!all_trans.empty()) // transactions existing
        return ERR_SUBCAT_USED1;

    DB_View_SPLITTRANSACTIONS_V1::Data_Set all_splittrans =
        SPLITTRANSACTIONS_V1.find(db_.get()
            , DB_View_SPLITTRANSACTIONS_V1::COL_CATEGID, categID
            , DB_View_SPLITTRANSACTIONS_V1::COL_SUBCATEGID, subcategID);
    if (!all_splittrans.empty()) // split transactions existing
        return ERR_SUBCAT_USED2;

    DB_View_BILLSDEPOSITS_V1::Data_Set all_bills =
        BILLSDEPOSITS_V1.find(db_.get()
            , DB_View_BILLSDEPOSITS_V1::COL_CATEGID, categID
            , DB_View_BILLSDEPOSITS_V1::COL_SUBCATEGID, subcategID);
    if (!all_bills.empty()) // repeating transactions existing
        return ERR_SUBCAT_USED3;

    DB_View_BUDGETSPLITTRANSACTIONS_V1::Data_Set all_budget_splittrans =
        BUDGETSPLITTRANSACTIONS_V1.find(db_.get()
            , DB_View_BUDGETSPLITTRANSACTIONS_V1::COL_CATEGID, categID
            , DB_View_BUDGETSPLITTRANSACTIONS_V1::COL_SUBCATEGID, subcategID);
    if (!all_budget_splittrans.empty()) // repeating transactions existing
        return ERR_SUBCAT_USED4;

        try {

        static const char* sql_del[] =
        {
            "delete from SUBCATEGORY_V1 where CATEGID=? AND SUBCATEGID=?",
            "delete from BUDGETTABLE_V1 WHERE CATEGID=? AND SUBCATEGID=?",

            "update PAYEE_V1 set CATEGID = -1, SUBCATEGID = -1 "
                                       "WHERE CATEGID=? AND SUBCATEGID=?",
            0
        };

        for (int i = 0; sql_del[i]; ++i)
        {
            wxSQLite3Statement st = db_.get()->PrepareStatement(sql_del[i]);
            st.Bind(1, categID);
            st.Bind(2, subcategID);

            st.ExecuteUpdate();
            st.Finalize();
        }

        mmOptions::instance().databaseUpdated_ = true;
        return wxID_OK;

    } catch(const wxSQLite3Exception& e)
    {
        wxLogDebug(wxT("Database::deleteSubCategoryWithConstraints: Exception"), e.GetMessage().c_str());
        wxLogError(wxT("Delete SubCategory with Constraints. ") + wxString::Format(_("Error: %s"), e.GetMessage().c_str()));
        return ERR_CAT_USED5;
    }
}


