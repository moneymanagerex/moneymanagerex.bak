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

#ifndef _MM_EX_MMCURRENCY_H_
#define _MM_EX_MMCURRENCY_H_

#include "mmdbinterface.h"
#include "boost/shared_ptr.hpp"

class mmCurrency
{
public: 
   mmCurrency();
   mmCurrency(boost::shared_ptr<wxSQLite3Database> db, wxSQLite3ResultSet& q1);
   ~mmCurrency() {}

   void loadCurrencySettings();

   /* Data */
   int currencyID_; 
   wxString currencyName_;
   wxString pfxSymbol_;
   wxString sfxSymbol_;
   wxString dec_;
   wxString grp_;
   wxString unit_;
   wxString cent_;
   double scaleDl_;
   double baseConv_;
   wxChar decChar_;
   wxChar grpChar_;
   wxString currencySymbol_;

private:
    boost::shared_ptr<wxSQLite3Database> db_;
};

class mmCurrencyList
{
public:
    mmCurrencyList(boost::shared_ptr<wxSQLite3Database> db)
        : db_(db) {}
    ~mmCurrencyList() {}

    /* Currency Functions */
    int addCurrency(boost::shared_ptr<mmCurrency> pCurrency);
    //bool deleteCurrency(int currencyID);
    void updateCurrency(boost::shared_ptr<mmCurrency> pCurrency);
    //bool currencyExists(const wxString& currencyName);
    //int getCurrencyID(const wxString& currencyName);
    boost::shared_ptr<mmCurrency> getCurrencySharedPtr(int currencyID);
    boost::shared_ptr<mmCurrency> getCurrencySharedPtr(const wxString& currencyName);

    void loadBaseCurrencySettings();
    int getBaseCurrencySettings();
    void setBaseCurrencySettings(int currencyID);

    std::vector< boost::shared_ptr<mmCurrency> > currencies_;
    
private:
    boost::shared_ptr<wxSQLite3Database> db_;
};


#endif
