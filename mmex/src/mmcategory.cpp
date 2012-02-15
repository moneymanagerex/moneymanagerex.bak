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

int mmCategoryList::addCategory(const wxString& category)
{
    int cID = -1;

    mmDBWrapper::addCategory(db_.get(), category);
    cID = (db_->GetLastRowId()).ToLong();

    boost::shared_ptr<mmCategory> pCategory(new mmCategory(cID, category));
    entries_.push_back(pCategory);

    return cID;
}

int mmCategoryList::getSubCategoryID(int parentID, const wxString& subCategoryName) const
{
    return mmDBWrapper::getSubCategoryID(db_.get(), parentID, subCategoryName);
}

int mmCategoryList::addSubCategory(int parentID, const wxString& text)
{
    int cID = -1;

    mmDBWrapper::addSubCategory(db_.get(), parentID, text);
    cID = (db_->GetLastRowId()).ToLong();

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

bool mmCategoryList::deleteCategory(int categID)
{
    if (mmDBWrapper::deleteCategoryWithConstraints(db_.get(), categID))
    {
        std::vector <boost::shared_ptr<mmCategory> >::iterator Iter;
        for ( Iter = entries_.begin( ) ; Iter != entries_.end( ) ; Iter++ )
        {
            if ((*Iter)->categID_ == categID)
            {
                entries_.erase(Iter);
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
    wxString catName = mmDBWrapper::getCategoryName(db_.get(), categID);
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
