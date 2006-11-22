/*******************************************************
 Copyright (C) 2006 Madhan Kanagavel

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
 /*******************************************************/

#include "mmcategory.h"
#include "dbwrapper.h"

mmCategory::mmCategory(int id, const wxString& name) 
: categID_(id), categName_(name)
{
}

bool mmCategoryList::categoryExists(const wxString& categoryName)
{
    int numCategs = (int)categories_.size();
    for (int idx = 0; idx < numCategs; idx++)
    {
        if (categories_[idx]->categName_ == categoryName)
            return true;
    }
    return false;
}

int mmCategoryList::getCategoryID(const wxString& categoryName)
{
    int numCategs = (int)categories_.size();
    for (int idx = 0; idx < numCategs; idx++)
    {
        if (categories_[idx]->categName_ == categoryName)
            return categories_[idx]->categID_;
    }
    return -1;
}

int mmCategoryList::addCategory(const wxString& category)
{
    int cID = -1;

    mmBEGINSQL_LITE_EXCEPTION;

    mmDBWrapper::addCategory(db_.get(), category);
    cID = (db_->GetLastRowId()).ToLong();

    boost::shared_ptr<mmCategory> pCategory(new mmCategory(cID, category));
    categories_.push_back(pCategory);

    mmENDSQL_LITE_EXCEPTION;

    return cID;
}

int mmCategoryList::getSubCategoryID(int parentID, const wxString& subCategoryName)
{
    return mmDBWrapper::getSubCategoryID(db_.get(), parentID, subCategoryName);
}

int mmCategoryList::addSubCategory(int parentID, const wxString& text)
{
    int cID = -1;

    mmBEGINSQL_LITE_EXCEPTION;

    mmDBWrapper::addSubCategory(db_.get(), parentID, text);
    cID = (db_->GetLastRowId()).ToLong();

    boost::shared_ptr<mmCategory> categ = getCategorySharedPtr(parentID, -1);
    
    boost::shared_ptr<mmCategory> subCateg(new mmCategory(cID, text));
    subCateg->parent_ = categ;
    categ->children_.push_back(subCateg);
    
    mmENDSQL_LITE_EXCEPTION;

    return cID;
}

boost::shared_ptr<mmCategory> mmCategoryList::getCategorySharedPtr(int category, int subcategory)
{
    if (category != -1)
    {
        int numCategory = (int)categories_.size();
        for (int idx = 0; idx < numCategory; idx++)
        {
            if (categories_[idx]->categID_ == category)
            {
                if (subcategory == -1)
                    return categories_[idx];

                int numSubCategory = (int)categories_[idx]->children_.size();
                for (int idxS = 0; idxS < numSubCategory; idxS++)
                {
                    if (categories_[idx]->children_[idxS]->categID_ == subcategory)
                    {
                        return categories_[idx]->children_[idxS];
                    }
                }
            }
        }
    }
    boost::shared_ptr<mmCategory> categ;
    return categ;
}

bool mmCategoryList::deleteCategory(int categID)
{
    if (mmDBWrapper::deleteCategoryWithConstraints(db_.get(), categID))
    {
        std::vector <boost::shared_ptr<mmCategory> >::iterator Iter;
        for ( Iter = categories_.begin( ) ; Iter != categories_.end( ) ; Iter++ )
        {
            if ((*Iter)->categID_ == categID)
            {
                categories_.erase(Iter);
                return true;
            }
        }
    }
    return false;
}

bool mmCategoryList::deleteSubCategory(int categID, int subCategID)
{
    if (mmDBWrapper::deleteSubCategoryWithConstraints(db_.get(), categID, subCategID))
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
                return true;
            }
        }
    }
    return false;
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
            for (int idx = 0; idx < parent->children_.size(); idx++)
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