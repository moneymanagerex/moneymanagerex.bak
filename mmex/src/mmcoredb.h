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

//----------------------------------------------------------------------------
#ifndef _MM_EX_COREDB_H_
#define _MM_EX_COREDB_H_
//----------------------------------------------------------------------------
#include "mmpayee.h"
#include "mmcategory.h"
#include "mmaccount.h"
#include "mmcurrency.h"
#include "mmtransaction.h"
//----------------------------------------------------------------------------
#include "boost/shared_ptr.hpp"
//----------------------------------------------------------------------------
class wxSQLite3Database;
//----------------------------------------------------------------------------

/** 
   mmCoreDB encapsulates most of the work in translating between
   the SQLite DB and the C++ datastructures used by MMEX
*/

class mmCoreDB
{
public:
    int getPayeeID(const wxString& name) const { return payeeList_.getID(name); }
    int getCategoryID(const wxString& name) const { return categoryList_.getID(name); }
    int getSubCategoryID(int parentID, const wxString& name) const { return categoryList_.getSubCategoryID(parentID, name); }
    int getAccountID(const wxString& name) const { return accountList_.getID(name); }
    int getCurrencyID(const wxString& name) const { return currencyList_.getCurrencyID(name); }

    bool payeeExists(int id) const { return payeeList_.exists(id); }
    bool payeeExists(const wxString& name) const { return payeeList_.exists(name); }
    wxString getPayeeName(int id) const {return payeeList_.getName(id); }
    int addPayee(const wxString& name) { return payeeList_.add(name); }
    bool removePayee(int id) { return payeeList_.remove(id); }
    void updatePayee(int id, const wxString& name) { return payeeList_.update(id, name); }
    boost::shared_ptr<mmPayee> getPayeeSharedPtr(int id) { return payeeList_.getSharedPtr(id); }
    int numPayee() const { return payeeList_.num(); }
    std::pair<mmPayeeList::const_iterator, mmPayeeList::const_iterator> rangePayee() const { return payeeList_.range(); }

    wxString getAccountName(int accountID) const { return accountList_.getName(accountID); }
    wxArrayString getAccountsName(int except_id = -1) const { return accountList_.getAccountsName(except_id); }
    int getNumAccounts() const { return accountList_.getNumAccounts(); }
    int getNumBankAccounts() const { return accountList_.getNumBankAccounts(); }
    int addAccount(boost::shared_ptr<mmAccount> pAccount) { return accountList_.add(pAccount); };
    void updateAccount(boost::shared_ptr<mmAccount> pAccount) { return accountList_.update(pAccount); };
    bool accountExists(const wxString& name) const { return accountList_.exists(name); };
    bool deleteAccount(int accountID) { return accountList_.remove(accountID); }

    bool categoryExists(const wxString& categoryName) const { return categoryList_.categoryExists(categoryName); }
    boost::shared_ptr<mmCategory> getCategorySharedPtr(int category, int subcategory) const { return categoryList_.getCategorySharedPtr(category, subcategory); }
    int addCategory(const wxString& category) { return categoryList_.addCategory(category); }
    int addSubCategory(int parentID, const wxString& text) { return categoryList_.addSubCategory(parentID, text); }
    bool deleteCategory(int categID) { return categoryList_.deleteCategory(categID); }
    bool deleteSubCategory(int categID, int subCategID) { return categoryList_.deleteSubCategory(categID, subCategID); }
    bool updateCategory(int categID, int subCategID, const wxString& text) { return categoryList_.updateCategory(categID, subCategID, text); }
    wxString GetCategoryString(int categID) const { return categoryList_.GetCategoryString(categID); }
    wxString GetSubCategoryString(int categID, int subCategID) const { return categoryList_.GetSubCategoryString(categID, subCategID); }
    wxString GetFullCategoryString(int categID, int subCategID) const { return categoryList_.GetFullCategoryString(categID, subCategID); }
    std::pair<mmCategoryList::const_iterator, mmCategoryList::const_iterator> rangeCategory() const { return categoryList_.range(); }

    /* Currency Functions */
    int addCurrency(boost::shared_ptr<mmCurrency> pCurrency) { return currencyList_.addCurrency(pCurrency); }
    void deleteCurrency(int currencyID) { return currencyList_.deleteCurrency(currencyID); }
    bool currencyInUse(int currencyID) { return currencyList_.currencyInUse(currencyID); }
    void updateCurrency(boost::shared_ptr<mmCurrency> pCurrency) { return currencyList_.updateCurrency(pCurrency); }
    wxString getCurrencyName(int currencyID) const { return currencyList_.getCurrencyName(currencyID); }
    boost::shared_ptr<mmCurrency> getCurrencySharedPtr(int currencyID) const { return currencyList_.getCurrencySharedPtr(currencyID); }
    boost::shared_ptr<mmCurrency> getCurrencySharedPtr(const wxString& currencyName) const { return currencyList_.getCurrencySharedPtr(currencyName); }
    void loadBaseCurrencySettings() const { return currencyList_.loadBaseCurrencySettings(); }
    int getBaseCurrencySettings() const { return currencyList_.getBaseCurrencySettings(); }
    void setBaseCurrencySettings(int currencyID) { return currencyList_.setBaseCurrencySettings(currencyID); }
    std::pair<mmCurrencyList::const_iterator, mmCurrencyList::const_iterator> rangeCurrency() const { return currencyList_.range(); }

    /*Account Functions*/
    bool has_term_account() const { return accountList_.has_term_account(); }
    std::pair<mmAccountList::const_iterator, mmAccountList::const_iterator> rangeAccount() const { return accountList_.range(); }

public: 
   mmCoreDB(boost::shared_ptr<wxSQLite3Database>);

   boost::shared_ptr<wxSQLite3Database> db_;

   mmCurrencyList currencyList_;
   mmAccountList accountList_;
   mmBankTransactionList bTransactionList_;

   bool displayDatabaseError_;

private:
   mmPayeeList payeeList_;
   mmCategoryList categoryList_;

};
//----------------------------------------------------------------------------
#endif // _MM_EX_COREDB_H_
//----------------------------------------------------------------------------

