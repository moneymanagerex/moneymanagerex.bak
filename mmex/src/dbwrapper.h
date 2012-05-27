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
#ifndef _MM_EX_DBWRAPPER_H_
#define _MM_EX_DBWRAPPER_H_
//----------------------------------------------------------------------------
#include "defs.h"
#include "mmex_db_view.h"
//----------------------------------------------------------------------------
#include <boost/shared_ptr.hpp>
//----------------------------------------------------------------------------

const wxString LANGUAGE_PARAMETER = wxT("LANGUAGE");

const int BD_REPEATS_MULTIPLEX_BASE  = 100;

const wxString ACCOUNT_TYPE_BANK  = wxT("Checking");
const wxString ACCOUNT_TYPE_TERM  = wxT("Term");
const wxString ACCOUNT_TYPE_STOCK = wxT("Investment");

const wxString TRANS_TYPE_WITHDRAWAL_STR = wxT("Withdrawal");
const wxString TRANS_TYPE_DEPOSIT_STR    = wxT("Deposit");
const wxString TRANS_TYPE_TRANSFER_STR   = wxT("Transfer");

enum {DEF_STATUS_NONE , DEF_STATUS_RECONCILED , DEF_STATUS_VOID , DEF_STATUS_FOLLOWUP , DEF_STATUS_DUPLICATE };
enum {DEF_WITHDRAWAL , DEF_DEPOSIT , DEF_TRANSFER};

/*****************************************************************************
Class BudgetYear_Table
******************************************************************************/
class BudgetYear_Table
{
public:
    BudgetYear_Table(wxSQLite3Database* db);

    /// Add the Year and return the ID
    int AddYear(const wxString& year);
    /// Get the Budget Year, for the given ID
    wxString GetYear(int id);
    /// Get the Budget Year ID, for the given Year
    int GetYearID(const wxString& year);
    
    /// Update the Budget Year, for the given ID
    bool UpdateYear(const wxString& year, int id);

    bool DeleteYear(int id);
    bool DeleteYear(const wxString& year);

private:
    wxSQLite3Database* db_;
};

/*****************************************************************************
Class BudgetEntry_Table
******************************************************************************/
class BudgetEntry_Table : public BudgetYear_Table
{
public:
    BudgetEntry_Table(wxSQLite3Database* db);

    /// Add the entry and return the Year ID
    int AddEntry(int year_id
        , int cat_id, int subcat_id
        , const wxString& period, double amount);

    // Get the entry ID given the year
    int GetEntryID(const wxString& year);
    
    /// Update the Budget Entry, for the given year ID
    bool UpdateEntry(int year_id
        , int cat_id, int subcat_id
        , const wxString& period, double amount);

    /// copy year of base ID to new ID
    bool CopyYear(int base_id, int new_id);

private:
    wxSQLite3Database* db_;
};

/*****************************************************************************
Class: Category_Table
******************************************************************************/
class Category_Table
{
public:
    Category_Table(wxSQLite3Database* db);

    /// Add the name and return the ID
    int AddName(const wxString& name);
    /// Get the category name, for the given ID
    wxString GetName(int id);
    /// Get the category ID, for the given name
    int GetID(const wxString& name);
    /// update the category name, for the given ID
    bool UpdateName(const wxString& name, int id);
    bool DeleteCategory(int id);
    bool DeleteCategory(const wxString& name);

private:
    wxSQLite3Database* db_;
};

/*****************************************************************************
Class SubCategory_Table
******************************************************************************/
class SubCategory_Table
{
public:
    SubCategory_Table(wxSQLite3Database* db);

    /// Add the name for the Category ID and return the Subcategory ID
    int AddName(const wxString& name, int cat_id);
    /// Get the subcategory name given the category ID and subcategory ID
    wxString GetName(int cat_id, int subcat_id);
    /// get the id given the name and category ID
    int GetID(const wxString& name, int cat_id);
    /// update the subcategory name, for the given cat ID and subcat ID
    bool UpdateName(const wxString& name, int cat_id, int subcat_id);

private:
    wxSQLite3Database* db_;
};

/*****************************************************************************
End Class Definitions
******************************************************************************/

namespace mmDBWrapper
{

//int isReadOnly(wxSQLite3Database &db);
boost::shared_ptr<wxSQLite3Database> Open(const wxString &dbpath, const wxString &key = wxGetEmptyString());

bool ViewExists(wxSQLite3Database* db, const char *viewName);

/* Creating new DBs */
void initDB(wxSQLite3Database* db, wxProgressDialog* pgd);
void createInfoV1Table(wxSQLite3Database* db);
void createCurrencyV1Table(wxSQLite3Database* db);
void createAccountListV1Table(wxSQLite3Database* db);
void createCheckingAccountV1Table(wxSQLite3Database* db);
void createPayeeV1Table(wxSQLite3Database* db);
void createCategoryV1Table(wxSQLite3Database* db);
void createBudgetingV1Table(wxSQLite3Database* db);
void createBillsDepositsV1Table(wxSQLite3Database* db);
void createStockV1Table(wxSQLite3Database* db);
void createAssetsV1Table(wxSQLite3Database* db);
void createSplitTransactionsV1Table(wxSQLite3Database* db);
void createAllDataView(wxSQLite3Database* db);

/* Upgrading to new DBs */
bool checkDBVersion(wxSQLite3Database* db);

bool copyBudgetYear(wxSQLite3Database* db, int newYear, int baseYear);
int getBudgetYearID(wxSQLite3Database* db, const wxString &year);
int getTransactionNumber(wxSQLite3Database* db, int accountID);
wxString getBudgetYearForID(wxSQLite3Database* db, int yearid);
void updateYearForID(wxSQLite3Database* db, const wxString& yearName, int yearid);
bool deleteBudgetYear(wxSQLite3Database* db, const wxString& yearName);

/* Budgeting BUDGETTABLE_V1 API */
void addBudgetEntry(wxSQLite3Database* db, int budgetYearID, int categID, int subCategID, const wxString& period, double amount);
void updateBudgetEntry(wxSQLite3Database* db, int budgetYearID, int categID, int subCategID, const wxString& period, double amout);
bool getBudgetEntry(wxSQLite3Database* db, int budgetYearID, int categID, int subCategID, wxString& period, double& amt);


/* Account API */
void removeSplitsForAccount(wxSQLite3Database* db, int accountID);

/* Payee Table API */
wxString getPayee(wxSQLite3Database* db, int payeeID, int& categID, int& subcategID);
bool updatePayee(wxSQLite3Database* db, const wxString& payeeName, int payeeID, int categID, int subcategID);
bool deletePayeeWithConstraints(wxSQLite3Database* db, int payeeID);
wxArrayString filterPayees(wxSQLite3Database* db, const wxString& patt);

/* Category Table API */
bool updateCategory(wxSQLite3Database* db, int categID, int subcategID, const wxString &newName);
wxString getSubCategoryName(wxSQLite3Database* db, int categID, int subcategID);

/* Transactions API */
bool updateTransactionWithStatus(wxSQLite3Database &db, int transID, const wxString& status);
bool deleteTransaction(wxSQLite3Database* db, int transID);
wxString getSplitTrxNotes(wxSQLite3Database* db_, int trxID);

/* Bills & Deposits API */
void deleteBDSeries(wxSQLite3Database* db, int bdID);
void completeBDInSeries(wxSQLite3Database* db, int bdID);

/* Operations on the CURRENCY */
int getBaseCurrencySettings(wxSQLite3Database* db);
void setBaseCurrencySettings(wxSQLite3Database* db, int currencyID);
void loadSettings(wxSQLite3Database* db, int currencyID);
void loadSettings(int accountID, wxSQLite3Database* db);
void loadBaseCurrencySettings(wxSQLite3Database* db);
double getCurrencyBaseConvRate(wxSQLite3Database* db, int accountID);
double getCurrencyBaseConvRateForId(wxSQLite3Database* db, int currencyID);

/* Operations on INFOTABLE */
wxString getInfoSettingValue(wxSQLite3Database* db, const wxString& settingName, const wxString& defaultVal = wxGetEmptyString());
void setInfoSettingValue(wxSQLite3Database* db, const wxString& settingName, const wxString& settingValue);

/* Operations on the INI SETTINGS DB */
wxString getINISettingValue(wxSQLite3Database* db, const wxString& settingName, const wxString& defaultVal = wxGetEmptyString());
void setINISettingValue(wxSQLite3Database* db, const wxString& settingName, const wxString& settingValue);
void verifyINIDB(wxSQLite3Database* inidb);

wxString getLastDbPath(wxSQLite3Database *db, const wxString &defaultVal = wxGetEmptyString());
void setLastDbPath(wxSQLite3Database *db, const wxString &path);

/* Stocks API */
double getStockInvestmentBalance(wxSQLite3Database* db, double& invested);
double getStockInvestmentBalance(wxSQLite3Database* db, int accountID, bool convertToBase, double& originalVal);

/* Assets API */
double getAssetBalance(wxSQLite3Database* db);
double getAssetValue(const DB_View_ASSETS_V1::Data& asset);


//----------------------------------------------------------------------------

} // namespace mmDBWrapper

//----------------------------------------------------------------------------

inline void mmDBWrapper::setLastDbPath(wxSQLite3Database *db, const wxString &path)
{
    setINISettingValue(db, wxT("LASTFILENAME"), path);
}
//----------------------------------------------------------------------------
#endif // _MM_EX_DBWRAPPER_H_
//----------------------------------------------------------------------------
