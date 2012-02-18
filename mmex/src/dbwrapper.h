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
//----------------------------------------------------------------------------
#include <boost/shared_ptr.hpp>
//----------------------------------------------------------------------------

const wxString LANGUAGE_PARAMETER = wxT("LANGUAGE");

const int BD_REPEATS_MULTIPLEX_BASE  = 100;

const wxString ACCOUNT_TYPE_BANK  = wxT("Checking");
const wxString ACCOUNT_TYPE_TERM  = wxT("Term");
const wxString ACCOUNT_TYPE_STOCK = wxT("Investment");

const wxString TRANS_TYPE_WITHDRAWAL_STR   = wxT("Withdrawal");   
const wxString TRANS_TYPE_DEPOSIT_STR      = wxT("Deposit");
const wxString TRANS_TYPE_TRANSFER_STR     = wxT("Transfer");

enum {DEF_STATUS_NONE , DEF_STATUS_RECONCILED , DEF_STATUS_VOID , DEF_STATUS_FOLLOWUP , DEF_STATUS_DUPLICATE };
enum {DEF_WITHDRAWAL , DEF_DEPOSIT , DEF_TRANSFER};

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
wxString getCurrencySymbol(wxSQLite3Database* db, int currencyID);

/* Upgrading to new DBs */
bool checkDBVersion(wxSQLite3Database* db);

void addBudgetYear(wxSQLite3Database* db, const wxString &year);
bool copyBudgetYear(wxSQLite3Database* db, int newYear, int baseYear);
int getBudgetYearID(wxSQLite3Database* db, const wxString &year);
int getTransactionNumber(wxSQLite3Database* db, int accountID);
wxString getBudgetYearForID(wxSQLite3Database* db, int yearid);
void updateYearForID(wxSQLite3Database* db, const wxString& yearName, int yearid);
bool deleteBudgetYear(wxSQLite3Database* db, const wxString& yearName);

/* Budgeting BUDGETTABLE_V1 API */
void addBudgetEntry(wxSQLite3Database* db, int budgetYearID, int categID, int subCategID, const wxString& period, double amount);
void deleteBudgetEntry(wxSQLite3Database* db, int budgetEntryID);
void updateBudgetEntry(wxSQLite3Database* db, int budgetYearID, int categID, int subCategID, const wxString& period, double amout);
bool getBudgetEntry(wxSQLite3Database* db, int budgetYearID, int categID, int subCategID, wxString& period, double& amt);
    

/* Account API */
int getNumBankAccounts(wxSQLite3Database* db);
wxString getAccountName(wxSQLite3Database* db, int accountID);
int getAccountID(wxSQLite3Database* db, const wxString& accountName);
double getTotalBalanceOnAccount(wxSQLite3Database* db, int accountID, bool ignoreFuture=false);
double getInitBalanceOnAccount(wxSQLite3Database* db, int accountID);

void removeSplitsForAccount(wxSQLite3Database* db, int accountID);

/* Payee Table API */
wxString getPayee(wxSQLite3Database* db, int payeeID, int& categID, int& subcategID);
wxString getPayee(wxSQLite3Database* db, int payeeID);
bool updatePayee(wxSQLite3Database* db, const wxString& payeeName, int payeeID, int categID, int subcategID);
bool deletePayeeWithConstraints(wxSQLite3Database* db, int payeeID);
wxArrayString filterPayees(wxSQLite3Database* db, const wxString& patt);
	
/* Category Table API */
bool deleteCategoryWithConstraints(wxSQLite3Database* db, int categID);
bool deleteSubCategoryWithConstraints(wxSQLite3Database* db, int categID, int subcategID);
bool updateCategory(wxSQLite3Database* db, int categID, int subcategID, const wxString &newName);
bool addCategory(wxSQLite3Database* db, const wxString &newName);
bool addSubCategory(wxSQLite3Database* db, int categID, const wxString &newName);
int getSubCategoryID(wxSQLite3Database* db, int categID, const wxString &name);
wxString getCategoryName(wxSQLite3Database* db, int categID);
wxString getSubCategoryName(wxSQLite3Database* db, int categID, int subcategID);
double getAmountForCategory(wxSQLite3Database* db, int categID, int subcategID, bool ignoreDate, const wxDateTime& dtbegin, const wxDateTime& dtEnd, bool evaluateTransfer = false, bool asDeposit = false, bool ignoreFuture = false);

/* Transactions API */
bool updateTransactionWithStatus(wxSQLite3Database &db, int transID, 
	const wxString& status);
bool deleteTransaction(wxSQLite3Database* db, int transID);
bool commit(wxSQLite3Database* db);
bool begin(wxSQLite3Database* db);
bool rollback(wxSQLite3Database* db);
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
void deleteStockInvestment(wxSQLite3Database* db, int stockID);
double getStockInvestmentBalance(wxSQLite3Database* db, double& invested);
double getStockInvestmentBalance(wxSQLite3Database* db, int accountID, bool convertToBase, double& originalVal);

/* Assets API */
void deleteAsset(wxSQLite3Database* db, int assetID);
double getAssetBalance(wxSQLite3Database* db);
double getAssetValue(wxSQLite3Database* db, int assetID);

/* Split Transaction API */
double getSplitTransactionValueForCategory(wxSQLite3Database* db, int transID, int categID, int subcategID);

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
