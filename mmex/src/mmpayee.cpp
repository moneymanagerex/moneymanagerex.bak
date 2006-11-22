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

#include "mmpayee.h"
#include "util.h"

mmPayee::mmPayee(int payeeID, const wxString& payeeString, boost::shared_ptr<mmCategory> category)
    : payeeID_(payeeID), payeeName_(payeeString), category_(category)
{

}

bool mmPayeeList::payeeExists(const wxString& payeeName)
{
    int numPayees = (int)payees_.size();
    for (int idx = 0; idx < numPayees; idx++)
    {
        if (payees_[idx]->payeeName_ == payeeName)
            return true;
    }
    return false;
}

int mmPayeeList::addPayee(const wxString &payeeName)
{
    int payeeID = -1;
    mmBEGINSQL_LITE_EXCEPTION;

    int categID = -1;
    int subcategID = -1;
    wxString bufSQL = wxString::Format(wxT("insert into PAYEE_V1 (PAYEENAME, CATEGID, SUBCATEGID) values ('%s', %d, %d);"), 
        mmCleanString(payeeName).c_str(), categID, subcategID);
    int retVal = db_->ExecuteUpdate(bufSQL);
    payeeID = (db_->GetLastRowId()).ToLong();
    boost::shared_ptr<mmCategory> pNullCategory;
    boost::shared_ptr<mmPayee> pPayee(new mmPayee(payeeID, payeeName, pNullCategory));
    payees_.push_back(pPayee);
    mmENDSQL_LITE_EXCEPTION;
    return payeeID;
}

int mmPayeeList::getPayeeID(const wxString& payeeName)
{
    int categID = -1;
    int subcategID = -1;
    int payeeID = -1;

    mmDBWrapper::getPayeeID(db_.get(), payeeName, payeeID, categID, subcategID);
    return payeeID;
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
        }
    }
}
