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

mmPayee::mmPayee(int payeeID, 
                 const wxString& payeeString, 
                 boost::shared_ptr<mmCategory> category)
    : payeeID_(payeeID), 
      payeeName_(payeeString), 
      category_(category)
{
}

bool mmPayeeList::payeeExists(const wxString& payeeName) const
{
    for (std::vector< boost::shared_ptr<mmPayee> >::const_iterator it = payees_.begin(); it != payees_.end(); ++ it)
    {
        if (! (*it)->payeeName_.CmpNoCase(payeeName)) return true;
    }

    return false;
}

bool mmPayeeList::payeeExists(const int payeeid) const
{
    for (std::vector< boost::shared_ptr<mmPayee> >::const_iterator it = payees_.begin(); it != payees_.end(); ++ it)
    {
        if ((*it)->payeeID_ == payeeid) return true;
    }

    return false;
}

int mmPayeeList::addPayee(const wxString &payeeName)
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
        mmOptions::databaseUpdated_ = true;

    } catch(wxSQLite3Exception e) 
    { 
        wxLogDebug(wxT("Database::addPayee: Exception"), e.GetMessage().c_str());
        wxLogError(wxString::Format(_("Add Payee. Error: %s"), e.GetMessage().c_str()));
        return payeeID;
    }

    wxASSERT(payeeID > 0);

    boost::shared_ptr<mmCategory> pNullCategory;
    boost::shared_ptr<mmPayee> pPayee(new mmPayee(payeeID, payeeName, pNullCategory));
    payees_.push_back(pPayee);

    return payeeID;
}

int mmPayeeList::getPayeeID(const wxString& payeeName) const
{
    for (std::vector< boost::shared_ptr<mmPayee> >::const_iterator it = payees_.begin(); it != payees_.end(); ++ it)
    {
        if (! (*it)->payeeName_.CmpNoCase(payeeName)) return (*it)->payeeID_;
    }

	return -1;
}

bool mmPayeeList::deletePayee(int payeeID)
{
    if (mmDBWrapper::deletePayeeWithConstraints(db_.get(), payeeID))
    {
        std::vector <boost::shared_ptr<mmPayee> >::iterator Iter;
        for ( Iter = payees_.begin( ) ; Iter != payees_.end( ) ; Iter++ )
        {
            if ((*Iter)->payeeID_ == payeeID)
            {
                payees_.erase(Iter);
                return true;
            }
        }
    }
    return false;
}

bool sortPayees( boost::shared_ptr<mmPayee> elem1, boost::shared_ptr<mmPayee> elem2 )
{
    return elem1->payeeName_ < elem2->payeeName_;
}

void mmPayeeList::sortPayeeList(void)
{
    // sort the payee list alphabetically
    std::sort(payees_.begin(), payees_.end(), sortPayees);
}

boost::shared_ptr<mmPayee> mmPayeeList::getPayeeSharedPtr(int payeeID)
{
    int numPayees = (int)payees_.size();
    for (int idx = 0; idx < numPayees; idx++)
    {
        if (payees_[idx]->payeeID_ == payeeID)
        {
           return payees_[idx];
        }
    }
    return boost::shared_ptr<mmPayee>();
}

void mmPayeeList::updatePayee(int payeeID, const wxString& payeeName)
{
    int numPayees = (int)payees_.size();
    for (int idx = 0; idx < numPayees; idx++)
    {
        if (payees_[idx]->payeeID_ == payeeID)
        {
            int categID = -1;
            int subcategID = -1;
            boost::shared_ptr<mmCategory> category = payees_[idx]->category_.lock(); 
            if (category)  
            {
                boost::shared_ptr<mmCategory> pCategory = category->parent_.lock();
                if (pCategory)
                {
                    categID = pCategory->categID_;        
                    subcategID = category->categID_;
                }
                else
                {
                    categID = category->categID_;        
                }
            }
            mmDBWrapper::updatePayee(db_.get(), payeeName, payeeID, categID, subcategID);
            payees_[idx]->payeeName_ = payeeName;
            break;
        }
    }
}

