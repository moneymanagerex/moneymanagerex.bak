/*******************************************************
 Copyright (C) 2013 Guan Lisheng (guanlisheng@gmail.com)

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

#ifndef MODEL_SUBCATEGORY_H
#define MODEL_SUBCATEGORY_H

#include "Model.h"
#include "db/DB_Table_Subcategory_V1.h"

class Model_Subcategory : public Model<DB_Table_SUBCATEGORY_V1>
{
public:
    using Model<DB_Table_SUBCATEGORY_V1>::get;

public:
    Model_Subcategory();
    ~Model_Subcategory();

public:
    /**
    * Initialize the global Model_Subcategory table.
    * Reset the Model_Subcategory table or create the table if it does not exist.
    */
    static Model_Subcategory& instance(wxSQLite3Database* db);

    /** Return the static instance of Model_Subcategory table */
    static Model_Subcategory& instance();

public:
    /** Return the Data record instance for the given subcategory name and category ID */
    Data* get(const wxString& name, int category_id = -1);

public:
    static bool is_used(int id);
};

#endif // 
