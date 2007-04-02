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

#ifndef _MM_EX_DBWRAPPER_H_
#define _MM_EX_DBWRAPPER_H_

#include "defs.h"

#define mmBEGINSQL_LITE_EXCEPTION   \
    try                             \
    {                               \


#define mmENDSQL_LITE_EXCEPTION     \
    }                               \
    catch (wxSQLite3Exception& e)   \
    {                               \
        wxASSERT(false);            \
        wxLogError(e.GetMessage()); \
    }                               \

#define mmENDSQL_LITE_IGNOREEXCEPTION     \
    }                                     \
    catch (wxSQLite3Exception& e)         \
    {                                     \
                                          \
    }                                     \

class mmDBWrapper
{
public:
    mmDBWrapper()  {}
    ~mmDBWrapper() {}

    /* Creating new DBs */
    static void initDB(wxSQLite3Database* db, wxProgressDialog* pgd, const wxString& path);
    static void loadCurrencies(wxSQLite3Database* db, const wxString& fpath);
    static void createInfoV1Table(wxSQLite3Database* db);
    static void createCurrencyV1Table(wxSQLite3Database* db, const wxString& fpath);
    static void createAccountListV1Table(wxSQLite3Database* db);
    static void createCheckingAccountV1Table(wxSQLite3Database* db);
    static void createPayeeV1Table(wxSQLite3Database* db);
    static void createCategoryV1Table(wxSQLite3Database* db);
    static void createBudgetingV1Table(wxSQLite3Database* db);
    static void createBillsDepositsV1Table(wxSQLite3Database* db);
    static void createStockV1Table(wxSQLite3Database* db);
	static void createAssetsV1Table(wxSQLite3Database* db);
    
    /* Upgrading to new DBs */
    static bool checkDBVersion(wxSQLite3Database* db);

    /* Budgeting BUDGETYEAR_V1 API */
    struct bYearEntry
    {
        int categID;
        int subcategID;
        double amt;
        wxString period;
    };
    static void addBudgetYear(wxSQLite3Database* db, wxString year);
    static void copyBudgetYear(wxSQLite3Database* db, int newyear, int baseYear);
    static int getBudgetYearID(wxSQLite3Database* db, wxString year);
    static wxString getBudgetYearForID(wxSQLite3Database* db, int yearid);
    static void updateYearForID(wxSQLite3Database* db, const wxString& yearName, int yearid);
    static void deleteBudgetYear(wxSQLite3Database* db, const wxString& yearName);

    /* Budgeting BUDGETTABLE_V1 API */
    static void addBudgetEntry(wxSQLite3Database* db, int budgetYearID, 
                                int categID, int subCategID,
                                const wxString& period, double amount);
    static void deleteBudgetEntry(wxSQLite3Database* db, int budgetEntryID);
    static void updateBudgetEntry(wxSQLite3Database* db, int budgetYearID, 
        int categID, int subCategID,
                                const wxString& period, double amout);
    static bool getBudgetEntry(wxSQLite3Database* db, int budgetYearID, 
                                int categID, int subCategID, 
                                wxString& period, double& amt);
    

    /* Account API */
    static wxUint32 getNumAccounts(wxSQLite3Database* db);
    static wxString getAccountName(wxSQLite3Database* db, int accountID);
    static wxString getAccountType(wxSQLite3Database* db, int accountID);
    static int getAccountID(wxSQLite3Database* db, const wxString& accountName);
    static double getTotalBalanceOnAccount(wxSQLite3Database* db, int accountID, bool ignoreFuture=false);
    static double getReconciledBalanceOnAccount(wxSQLite3Database* db, int accountID);
    static bool getExpensesIncome(wxSQLite3Database* db, int accountID, 
        double& expenses, double& income, bool ignoreDate, 
        wxDateTime dtbegin, wxDateTime dtEnd);
    
    /* Payee Table API */
    static void addPayee(wxSQLite3Database* db, wxString payee, int categID, int subcategID);
    static wxString getPayee(wxSQLite3Database* db, int payeeID, int& categID, int& subcategID );
    static bool getPayeeID(wxSQLite3Database* db, wxString payee, int& payeeID, 
        int& categID, int& subcategID );
    static bool updatePayee(wxSQLite3Database* db, const wxString& payeeName, 
        int payeeID, int categID, int subcategID);
    static bool deletePayeeWithConstraints(wxSQLite3Database* db, int payeeID);
    static double getAmountForPayee(wxSQLite3Database* db, int payeeID,
        bool ignoreDate, wxDateTime dtbegin, wxDateTime dtEnd);
    
    /* Category Table API */
    static bool deleteCategoryWithConstraints(wxSQLite3Database* db, int categID);
    static bool deleteSubCategoryWithConstraints(wxSQLite3Database* db, 
        int categID, int subcategID);
    static bool updateCategory(wxSQLite3Database* db, int categID, 
        int subcategID, wxString newName);
    static bool addCategory(wxSQLite3Database* db, wxString newName);
    static bool addSubCategory(wxSQLite3Database* db, int categID, wxString newName);
    static int getCategoryID(wxSQLite3Database* db, wxString name);
    static int getSubCategoryID(wxSQLite3Database* db, int categID, wxString name);
    static wxString getCategoryName(wxSQLite3Database* db, int categID);
    static wxString getSubCategoryName(wxSQLite3Database* db, int categID, int subcategID);
    static double getAmountForCategory(wxSQLite3Database* db, int categID, int subcategID,
        bool ignoreDate, wxDateTime dtbegin, wxDateTime dtEnd);
    
    /* Transactions API */
    static void updateTransactionWithStatus(wxSQLite3Database* db, int transID, 
        const wxString& status);
    static bool deleteTransaction(wxSQLite3Database* db, int transID);
    static bool deleteFlaggedTransactions(wxSQLite3Database* db, int accountID);

    /* Bills & Deposits API */
    static void deleteBDSeries(wxSQLite3Database* db, int bdID);
    static void completeBDInSeries(wxSQLite3Database* db, int bdID);
    
    /* Operations on the CURRENCY */
    static int getBaseCurrencySettings(wxSQLite3Database* db);
    static void setBaseCurrencySettings(wxSQLite3Database* db, int currencyID);
    static void loadSettings(wxSQLite3Database* db, int currencyID);
    static void loadSettings(int accountID, wxSQLite3Database* db);
    static void loadBaseCurrencySettings(wxSQLite3Database* db);
    static int getCurrencyID(wxSQLite3Database* db, const wxString& currencyName);
    static wxString getCurrencyName(wxSQLite3Database* db, int currencyID);
    static double getCurrencyBaseConvRate(wxSQLite3Database* db, int accountID);
    static double getCurrencyBaseConvRateForId(wxSQLite3Database* db, int currencyID);

    /* Operations on INFOTABLE */
    static wxString getInfoSettingValue(wxSQLite3Database* db, const wxString& settingName, 
        const wxString& defaultVal);
    static void setInfoSettingValue(wxSQLite3Database* db, const wxString& settingName, 
        const wxString& settingValue);
    static bool doesInfoSettingNameExist(wxSQLite3Database* db, const wxString& settingName);

    /* Operations on the INI SETTINGS DB */
    static wxString getINISettingValue(wxSQLite3Database* db, const wxString& settingName, 
        const wxString& defaultVal);
    static void setINISettingValue(wxSQLite3Database* db, const wxString& settingName, 
        const wxString& settingValue);
    static bool doesINISettingNameExist(wxSQLite3Database* db, const wxString& settingName);
    static void verifyINIDB(wxSQLite3Database* inidb);

    /* Stocks API */
    static void deleteStockInvestment(wxSQLite3Database* db, int stockID);
    static double getStockInvestmentBalance(wxSQLite3Database* db);
	static double getStockInvestmentBalance(wxSQLite3Database* db, int accountID, bool convertToBase);

	/* Assets API */
	static void deleteAsset(wxSQLite3Database* db, int assetID);
    static double getAssetBalance(wxSQLite3Database* db);
	static double getAssetValue(wxSQLite3Database* db, int assetID);
};


#endif
