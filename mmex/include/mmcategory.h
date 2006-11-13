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
#ifndef _MM_EX_MMCATEGORY_H_
#define _MM_EX_MMCATEGORY_H_

#include "defs.h"
#include "boost/weak_ptr.hpp"

class mmCategory
{
public: 
    mmCategory(int id, const wxString& name);
   ~mmCategory() {}

   /* Public Data */
   int categID_;
   wxString categName_;
   boost::weak_ptr<mmCategory> parent_;
   std::vector<boost::shared_ptr<mmCategory> > children_;
};

class mmCategoryList
{
public:
    mmCategoryList(boost::shared_ptr<wxSQLite3Database> db)
        : db_(db) {}
    ~mmCategoryList() {}

    /* Category Functions */
    bool categoryExists(const wxString& categoryName);
    boost::shared_ptr<mmCategory> getCategorySharedPtr(int category, int subcategory);
    int getSubCategoryID(int parentID, const wxString& subCategoryName);

    int addCategory(const wxString& category);
    int addSubCategory(int parentID, const wxString& text);
    
    bool deleteCategory(int categID);
    bool deleteSubCategory(int categID, int subCategID);
    bool updateCategory(int categID, int subCategID, const wxString& text);
    
    /* Public Data */
    std::vector< boost::shared_ptr<mmCategory> > categories_;
    
private:
    boost::shared_ptr<wxSQLite3Database> db_;
};


#endif