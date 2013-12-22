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

#ifndef MODEL_PAYEE_H
#define MODEL_PAYEE_H

#include "Model.h"
#include "db/DB_Table_Payee_V1.h"

class Model_Payee : public Model<DB_Table_PAYEE_V1>
{
public:
    using Model<DB_Table_PAYEE_V1>::get;

public:
    Model_Payee();
    ~Model_Payee();

public:
    /**
    * Initialize the global Model_Payee table.
    * Reset the Model_Payee table or create the table if it does not exist.
    */
    static Model_Payee& instance(wxSQLite3Database* db);

    /** Return the static instance of Model_Payee table */
    static Model_Payee& instance();

public:
    Data_Set FilterPayees(const wxString& payee_pattern);
    Data* get(const wxString& name);
    bool remove(int id);

    wxArrayString all_payee_names();

    static bool is_used(int id);
    static bool is_used(const Data* record);
    static bool is_used(const Data& record);
};

#endif // 
