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
 ********************************************************/

#include "mmpayee.h"
#include "util.h"
#include "dbwrapper.h"

// Constructor used when loading a payee from the database
mmPayee::mmPayee(wxSQLite3ResultSet q1)
{
    id_        = q1.GetInt(wxT("PAYEEID"));
    name_      = q1.GetString(wxT("PAYEENAME"));
    categoryId_    = q1.GetInt(wxT("CATEGID"));
    subcategoryId_ = q1.GetInt(wxT("SUBCATEGID"));
}

// Constructor when adding a new payee to the database
mmPayee::mmPayee(int id, const wxString& name)
: id_(id)
, name_(name)
, categoryId_(-1)
, subcategoryId_(-1)
{}

bool mmPayee::UpdateDb(wxSQLite3Database* db)
{
    try
    {
        wxSQLite3Statement st = db->PrepareStatement(UPDATE_PAYEE_V1);
        st.Bind(1, name_);
        st.Bind(2, categoryId_);
        st.Bind(3, subcategoryId_);
        st.Bind(4, id_);

        st.ExecuteUpdate();
        st.Finalize();
        mmOptions::instance().databaseUpdated_ = true;
    }
    catch(const wxSQLite3Exception& e)
    {
        wxLogDebug(wxT("Database::updatePayee: Exception"), e.GetMessage().c_str());
        wxLogError(wxT("update PAYEE_V1. ") + wxString::Format(_("Error: %s"), e.GetMessage().c_str()));
        return false;
    }

    return true;
}

bool mmPayeeList::PayeeExists(const wxString& payeeName) const
{
    for (std::vector< boost::shared_ptr<mmPayee> >::const_iterator it = entries_.begin(); it != entries_.end(); ++ it)
    {
        if (! (*it)->name_.CmpNoCase(payeeName)) return true;
    }

    return false;
}

bool mmPayeeList::PayeeExists(const int payeeid) const
{
    for (std::vector< boost::shared_ptr<mmPayee> >::const_iterator it = entries_.begin(); it != entries_.end(); ++ it)
    {
        if ((*it)->id_ == payeeid) return true;
    }

    return false;
}

int mmPayeeList::AddPayee(const wxString &payeeName)
{
    int payeeID = -1;

    try {
	    char sql[] =
	    "insert into PAYEE_V1 (PAYEENAME, CATEGID, SUBCATEGID) values (?, ?, ?)";
	    wxSQLite3Statement st = db_->PrepareStatement(sql);
	    st.Bind(1, payeeName);
	    st.Bind(2, -1);
	    st.Bind(3, -1);
	
	    st.ExecuteUpdate();
	    payeeID = (int)db_->GetLastRowId().ToLong();
	    st.Finalize();
        mmOptions::instance().databaseUpdated_ = true;

    } catch(const wxSQLite3Exception& e) 
    { 
        wxLogDebug(wxT("Database::addPayee: Exception"), e.GetMessage().c_str());
        wxLogError(wxString::Format(_("Add Payee. Error: %s"), e.GetMessage().c_str()));
        return payeeID;
    }

    wxASSERT(payeeID > 0);

    boost::shared_ptr<mmPayee> pPayee(new mmPayee(payeeID, payeeName));
    entries_.push_back(pPayee);

    return payeeID;
}

int mmPayeeList::GetPayeeId(const wxString& payeeName) const
{
    for (std::vector< boost::shared_ptr<mmPayee> >::const_iterator it = entries_.begin(); it != entries_.end(); ++ it)
    {
        if (! (*it)->name_.CmpNoCase(payeeName)) return (*it)->id_;
    }

	return -1;
}

wxString mmPayeeList::GetPayeeName(int id) const
{
    for (std::vector< boost::shared_ptr<mmPayee> >::const_iterator it = entries_.begin(); it != entries_.end(); ++ it)
    {
        if ((*it)->id_ == id) return (*it)->name_;
    }

    return wxEmptyString;
}

bool mmPayeeList::RemovePayee(int payeeID)
{
    if (mmDBWrapper::deletePayeeWithConstraints(db_.get(), payeeID))
    {
        std::vector <boost::shared_ptr<mmPayee> >::iterator Iter;
        for ( Iter = entries_.begin( ) ; Iter != entries_.end( ) ; Iter++ )
        {
            if ((*Iter)->id_ == payeeID)
            {
                entries_.erase(Iter);
                return true;
            }
        }
    }
    return false;
}

bool sortPayees(const boost::shared_ptr<mmPayee>& elem1, const boost::shared_ptr<mmPayee>& elem2 )
{
    return elem1->name_ < elem2->name_;
}

void mmPayeeList::SortList(void)
{
    // sort the payee list alphabetically
    std::sort(entries_.begin(), entries_.end(), sortPayees);
}

boost::shared_ptr<mmPayee> mmPayeeList::GetPayeeSharedPtr(int payeeID)
{
    int numPayees = (int)entries_.size();
    for (int idx = 0; idx < numPayees; idx++)
    {
        if (entries_[idx]->id_ == payeeID)
        {
           return entries_[idx];
        }
    }
    return boost::shared_ptr<mmPayee>();
}

void mmPayeeList::UpdatePayee(int payeeID, const wxString& payeeName)
{
    boost::shared_ptr<mmPayee> pPayee = GetPayeeSharedPtr(payeeID);
    pPayee->name_ = payeeName;
    pPayee->UpdateDb(db_.get());
}

wxArrayString mmPayeeList::FilterPayees(const wxString& patt) const
{
    wxArrayString payee_list;

    for (size_t idx = 0; idx < (int)entries_.size(); idx++)
    {
        if (entries_[idx]->name_.Lower().Matches(patt.Lower().Append(wxT("*"))))
            payee_list.Add(entries_[idx]->name_);
    }
    return payee_list;
}

void mmPayeeList::LoadPayees()
{
    wxSQLite3ResultSet q1 = db_->ExecuteQuery(SELECT_ALL_FROM_PAYEE_V1);

    while (q1.NextRow())
    {
        boost::shared_ptr<mmPayee> pPayee(new mmPayee(q1));
        entries_.push_back(pPayee);
    }

    q1.Finalize();
    SortList();
}
