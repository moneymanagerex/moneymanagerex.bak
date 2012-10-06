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
    int id_;
    wxString name_; 
    int categoryId_;
    int subcategoryId_;

    /// Constructor used when loading a payee from the database
	mmPayee(wxSQLite3ResultSet q1);
    /// Constructor for adding a new payee to the database
    mmPayee(int id, const wxString& name);

    bool UpdateDb(wxSQLite3Database* db);
};

class mmPayeeList
{
public:
    mmPayeeList(boost::shared_ptr<wxSQLite3Database> db)
    : db_(db)
    {}

    /* Payee Functions */
    int AddPayee(const wxString& payeeName);
    bool RemovePayee(int payeeID);
    void UpdatePayee(int payeeID, const wxString& payeeName);
    bool PayeeExists(const wxString& payeeName) const;
    bool PayeeExists(const int payeeid) const;
    int GetPayeeId(const wxString& payeeName) const;
    wxString GetPayeeName(int id) const;
    boost::shared_ptr<mmPayee> GetPayeeSharedPtr(int payeeID);
    void SortList(void);
    wxArrayString FilterPayees(const wxString& patt) const;

    std::vector< boost::shared_ptr<mmPayee> > entries_;
    int Num() const
    {
        return entries_.size();
    }
    typedef std::vector< boost::shared_ptr<mmPayee> >::const_iterator const_iterator;
    std::pair<const_iterator, const_iterator> Range() const
    {
        return std::make_pair(entries_.begin(), entries_.end());
    }

    /// Loads database Payees into memory
    void LoadPayees();

private:
    boost::shared_ptr<wxSQLite3Database> db_;
};

#endif
