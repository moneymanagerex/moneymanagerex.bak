/*******************************************************
 Copyright (C) 2013 Stefano Giorgio

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

#pragma once
#include "boost/shared_ptr.hpp"
#include <vector>
#include "../defs.h"
#include "../util.h"

/************************************************************************************
 Class TEntryBase
 This is the base class for all entries of the database.
 ***********************************************************************************/
class TEntryBase
{
protected:
    int id_;
    
    /// This will set the record id and finalise the sql statement.
    void FinaliseAdd(wxSQLite3Database* db, wxSQLite3Statement& st);

    /// This will finalise the sql statement.
    void FinaliseStatement(wxSQLite3Statement& st);

    void DeleteEntry(wxSQLite3Database* db, const wxString& sql_statement);

public:
    TEntryBase();

    int GetId();
};

/************************************************************************************
 Class TListBase
 This is the base class for main Lists of the database.
 ***********************************************************************************/
class TListBase
{
protected:
    boost::shared_ptr<wxSQLite3Database> db_;
    int current_index_;

public:
    TListBase(boost::shared_ptr<wxSQLite3Database> db);
    wxSQLite3Database* ListDatabase();
    int GetCurrentIndex();

};
