/*******************************************************
 Copyright (C) 2013,2014 James Higley

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

#include "Model_Budgetsplittransaction.h"

Model_Budgetsplittransaction::Model_Budgetsplittransaction()
: Model<DB_Table_BUDGETSPLITTRANSACTIONS_V1>()
{
}

Model_Budgetsplittransaction::~Model_Budgetsplittransaction()
{
}

/**
* Initialize the global Model_Budgetsplittransaction table.
* Reset the Model_Budgetsplittransaction table or create the table if it does not exist.
*/
Model_Budgetsplittransaction& Model_Budgetsplittransaction::instance(wxSQLite3Database* db)
{
    Model_Budgetsplittransaction& ins = Singleton<Model_Budgetsplittransaction>::instance();
    ins.db_ = db;
    ins.destroy_cache();
    ins.ensure(db);

    return ins;
}

/** Return the static instance of Model_Budgetsplittransaction table */
Model_Budgetsplittransaction& Model_Budgetsplittransaction::instance()
{
    return Singleton<Model_Budgetsplittransaction>::instance();
}

double Model_Budgetsplittransaction::get_total(const Data_Set& rows)
{
    double total = 0.0;
    for (auto& r : rows) total += r.SPLITTRANSAMOUNT;

    return total;
}
