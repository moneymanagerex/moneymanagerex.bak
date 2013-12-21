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

#ifndef MODEL_BUDGETYEAR_H
#define MODEL_BUDGETYEAR_H

#include "Model.h"
#include "db/DB_Table_Budgetyear_V1.h"

class Model_Budgetyear : public Model<DB_Table_BUDGETYEAR_V1>
{
public:
    using Model<DB_Table_BUDGETYEAR_V1>::remove;

public:
    Model_Budgetyear();
    ~Model_Budgetyear();

public:
    /**
    * Initialize the global Model_Budgetyear table.
    * Reset the Model_Budgetyear table or create the table if it does not exist.
    */
    static Model_Budgetyear& instance(wxSQLite3Database* db);

    /** Return the static instance of Model_Budgetyear table */
    static Model_Budgetyear& instance();

    bool remove(int id);

public:
    void Set(int year_id, const wxString& value);
    int Add(const wxString& value);

    wxString Get(int year_id);
    int Get(const wxString& year_name);

    bool Exists(int year_id);
    bool Exists(const wxString& year_name);
};

#endif // 
