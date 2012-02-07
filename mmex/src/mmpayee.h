/*******************************************************
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

#ifndef _MM_EX_MMPAYEE_H_
#define _MM_EX_MMPAYEE_H_

#include "mmcategory.h"
#include "boost/shared_ptr.hpp"

class mmPayee
{
public: 
    mmPayee() {}
    mmPayee(int id, const wxString& name, boost::shared_ptr<mmCategory> category);
    ~mmPayee() {}

    int id_;
    wxString name_;
    boost::weak_ptr<mmCategory> category_;
};

class mmPayeeList
{
public:
    mmPayeeList(boost::shared_ptr<wxSQLite3Database> db)
        : db_(db) {}
    ~mmPayeeList() {}

    /* Payee Functions */
    int add(const wxString& payeeName);
    bool remove(int payeeID);
    void update(int payeeID, const wxString& payeeName);
    bool exists(const wxString& payeeName) const;
    bool exists(const int payeeid) const;
    int getID(const wxString& payeeName) const;
    boost::shared_ptr<mmPayee> getSharedPtr(int payeeID);
    void sortPayeeList(void);

    std::vector< boost::shared_ptr<mmPayee> > entities_;

private:
    boost::shared_ptr<wxSQLite3Database> db_;
};

#endif
