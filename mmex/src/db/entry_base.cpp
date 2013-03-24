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

#include "entry_base.h"

/************************************************************************************
 TEntryBase Methods
 ***********************************************************************************/
TEntryBase::TEntryBase()
: id_(-1)
{}

void TEntryBase::FinaliseAdd(wxSQLite3Database* db, wxSQLite3Statement& st)
{
    st.ExecuteUpdate();
    id_ = (db->GetLastRowId()).ToLong();
    st.Finalize();
    mmOptions::instance().databaseUpdated_ = true;
}

void TEntryBase::DeleteEntry(wxSQLite3Database* db, const wxString& sql_statement)
{
    wxSQLite3Statement st = db->PrepareStatement(sql_statement);
    st.Bind(1, id_);

    FinaliseStatement(st);
}

void TEntryBase::FinaliseStatement(wxSQLite3Statement& st)
{
    st.ExecuteUpdate();
    st.Finalize();
    mmOptions::instance().databaseUpdated_ = true;
}

int TEntryBase::GetId()
{
    return id_;
}

/************************************************************************************
 TListBase Methods
 ***********************************************************************************/
TListBase::TListBase(boost::shared_ptr<wxSQLite3Database> db)
: db_(db)
, current_index_(0)
{}

wxSQLite3Database* TListBase::ListDatabase()
{
    return db_.get();
}

int TListBase::GetCurrentIndex()
{
    return current_index_;
}
