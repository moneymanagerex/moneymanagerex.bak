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
#ifndef _MM_EX_COREDB_H_
#define _MM_EX_COREDB_H_

#include <vector>
#include "boost/shared_ptr.hpp"
#include "mmaccount.h"
#include "mmpayee.h"
#include "mmcategory.h"
#include "dbwrapper.h"
#include "mmcurrency.h"

/** 
   mmCoreDB encapsulates most of the work in translating between
   the SQLite DB and the C++ datastructures used by MMEX
*/

class mmCoreDB
{
public: 
   mmCoreDB(boost::shared_ptr<wxSQLite3Database>);
   ~mmCoreDB();

    /* Utility Functions */

public:
   mmAccountList accountList_;
   mmPayeeList payeeList_;
   mmCategoryList categoryList_;
   mmCurrencyList currencyList_;
   mmBankTransactionList bTransactionList_;
   
   boost::shared_ptr<wxSQLite3Database> db_;
};

#endif