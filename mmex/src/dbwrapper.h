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
#include "mmcurrency.h"
#include "mmex_settings.h"
//----------------------------------------------------------------------------
#include <boost/shared_ptr.hpp>
#include <vector>
//----------------------------------------------------------------------------
static const char SELECT_ALL_FROM_ASSETS_V1[] =
    "select a.ASSETNAME as ASSETNAME, "
       "a.STARTDATE as STARTDATE, "
       "a.ASSETTYPE as ASSETTYPE, "
       "a.value + (a.value/100*VALUECHANGERATE) "
       "* (case VALUECHANGE when 'Appreciates' then 1 when 'Depreciates' then -1 else 0 end ) "
       "* (julianday('now', 'localtime') - julianday(a.STARTDATE, 'localtime')) / 365 as TODAY_VALUE, "
       "a.NOTES as NOTES, "
       "a.STARTDATE as STARTDATE, "
       "a.VALUECHANGE as VALUECHANGE, "
       "a.ASSETID as ASSETID, "
       "a.VALUECHANGERATE as VALUECHANGERATE, "
	   "a.value as VALUE "
    "from ASSETS_V1 a ";

static const char SELECT_ROW_FROM_ASSETS_V1[] =
    "SELECT ASSETNAME, "
           "NOTES, "
           "date(STARTDATE, 'localtime') as STARTDATE, "
           "VALUE, "
           "VALUECHANGERATE, "
           "VALUECHANGE, "
           "ASSETTYPE "
    "FROM ASSETS_V1 "
    "where ASSETID = ?";

static const char UPDATE_ASSETS_V1[] =
    "UPDATE ASSETS_V1 "
    "SET STARTDATE = ?, ASSETNAME = ?, "
        "VALUE = ?, VALUECHANGE = ?,"
        "NOTES = ?, VALUECHANGERATE = ?, "
        "ASSETTYPE = ? "
    "where ASSETID = ?";

static const char INSERT_INTO_ASSETS_V1[] =
    "INSERT INTO ASSETS_V1 ("
      "STARTDATE, ASSETNAME, VALUE, VALUECHANGE, NOTES, VALUECHANGERATE, ASSETTYPE "
    ") values (?, ?, ?, ?, ?, ?, ?)";

static const char SELECT_ALL_FROM_ACCOUNTLIST_V1[] =
    "SELECT * "
    "FROM ACCOUNTLIST_V1 "
    "ORDER BY ACCOUNTNAME";

static const char SELECT_ALL_INVESTMENT_FROM_ACCOUNTLIST_V1[] =
    "SELECT ACCOUNTNAME, ACCOUNTID "
    "FROM ACCOUNTLIST_V1 "
    "WHERE ACCOUNTTYPE = 'Investment' "
    "ORDER BY ACCOUNTNAME";

static const char SELECT_ALL_FROM_BILLSDEPOSITS_V1[] =
    "SELECT BDID, "
    "ACCOUNTID, "
    "TOACCOUNTID, "
    "PAYEEID, "
    "TRANSCODE, "
    "TRANSAMOUNT, "
    "STATUS, "
    "TRANSACTIONNUMBER, "
    "NOTES, "
    "CATEGID, "
    "SUBCATEGID, "
    "TRANSDATE, "
    "FOLLOWUPID, "
    "TOTRANSAMOUNT, "
    "REPEATS, "
    "NEXTOCCURRENCEDATE, "
    "NUMOCCURRENCES "
    "FROM BILLSDEPOSITS_V1";

static const char SELECT_ROW_FROM_BUDGETTABLE_V1[] =
    "SELECT PERIOD, "
           "AMOUNT "
    "FROM BUDGETTABLE_V1 "
    "WHERE BUDGETYEARID = ? AND "
          "CATEGID = ? AND "
          "SUBCATEGID = ?";

static const char SELECT_ALL_FROM_BUDGETYEAR_V1[] =
    "SELECT BUDGETYEARID, BUDGETYEARNAME "
    "FROM BUDGETYEAR_V1 "
    "ORDER BY BUDGETYEARNAME";

static const char SELECT_ALL_FROM_CATEGORY_V1[] =
    "SELECT CATEGID, CATEGNAME "
    "FROM CATEGORY_V1 "
    "ORDER BY CATEGNAME";

static const char SELECT_ALL_FROM_CHECKINGACCOUNT_V1[] =
    "SELECT * FROM CHECKINGACCOUNT_V1";

static const char SELECT_ROW_FROM_CURRENCYFORMATS_V1[] =
    "SELECT CURRENCYNAME, "
           "PFX_SYMBOL, "
           "SFX_SYMBOL, "
           "DECIMAL_POINT, "
           "GROUP_SEPARATOR, "
           "UNIT_NAME, "
           "CENT_NAME, "
           "SCALE, "
           "BASECONVRATE, "
           "CURRENCY_SYMBOL "
    "FROM CURRENCYFORMATS_V1 "
    "WHERE CURRENCYID = ?";

static const char SELECT_ALL_FROM_CURRENCYFORMATS_V1[] =
    "SELECT * "
    "FROM CURRENCYFORMATS_V1 "
    "ORDER BY CURRENCYNAME";

static const char DELETE_CURRENCYID_FROM_CURRENCYFORMATS_V1[] =
    "DELETE FROM CURRENCYFORMATS_V1 "
    "WHERE CURRENCYID = ?";

static const char INSERT_INTO_CURRENCYFORMATS_V1[] =
    "INSERT INTO CURRENCYFORMATS_V1 ( "
    "CURRENCYNAME, PFX_SYMBOL, SFX_SYMBOL, DECIMAL_POINT, "
    "GROUP_SEPARATOR, UNIT_NAME, CENT_NAME, SCALE, BASECONVRATE, CURRENCY_SYMBOL "
    " ) VALUES ( ?, ?, ?, ?, ?, ?, ?, ?, ?, ? )";

static const char SELECT_INFOVALUE_FROM_INFOTABLE_V1[] =
    "SELECT INFOVALUE FROM INFOTABLE_V1 "
    "where INFONAME = ?";

static const char SELECT_ALL_FROM_PAYEE_V1[] =
    "SELECT PAYEEID, PAYEENAME, CATEGID, SUBCATEGID "
    "FROM PAYEE_V1 "
    "ORDER BY PAYEENAME";

static const char SELECT_ROW_FROM_SETTING_V1[] =
    "SELECT SETTINGVALUE "
    "FROM SETTING_V1 "
    "WHERE SETTINGNAME = ?";

static const char SELECT_SUBCATEGNAME_FROM_SUBCATEGORY_V1[] =
    "SELECT SUBCATEGNAME, "
    "SUBCATEGID "
    "FROM SUBCATEGORY_V1 "
    "WHERE CATEGID = ? AND "
    "SUBCATEGID = ?";

static const char SELECT_SUBCATEGID_FROM_SUBCATEGORY_V1[] =
    "SELECT SUBCATEGID "
    "FROM SUBCATEGORY_V1 "
    "WHERE SUBCATEGNAME = ? AND "
          "CATEGID = ?";

static const char SELECT_SUBCATEGS_FROM_SUBCATEGORY_V1[] =
    "SELECT SUBCATEGID, SUBCATEGNAME "
    "FROM SUBCATEGORY_V1 "
    "WHERE CATEGID = ? "
    "ORDER BY SUBCATEGNAME";

static const char SELECT_ALL_CATEGORIES[] =
    "SELECT C.CATEGID, C.CATEGNAME, "
           "SC.SUBCATEGID, SC.SUBCATEGNAME "
    "FROM CATEGORY_V1 C "
    "LEFT JOIN SUBCATEGORY_V1 SC "
    "ON SC.CATEGID = C.CATEGID "
    "ORDER BY C.CATEGNAME, SC.SUBCATEGNAME";

//DELETE
static const char DELETE_BUDGETENTRYIDS_FROM_BUDGETTABLE_V1[] =
    "DELETE FROM BUDGETTABLE_V1 "
    "where BUDGETENTRYID in (select BUDGETENTRYID "
    "from BUDGETTABLE_V1 "
    "WHERE BUDGETYEARID = ? AND "
    "CATEGID = ? AND "
    "SUBCATEGID = ? "
    ")";

static const char DELETE_FROM_BUDGETTABLE_V1[] =
    "DELETE FROM BUDGETTABLE_V1 where BUDGETENTRYID = ?";

static const char DELETE_FROM_SPLITTRANSACTIONS_V1[] =
    "DELETE FROM SPLITTRANSACTIONS_V1 "
    "where TRANSID in( SELECT TRANSID "
    "from CHECKINGACCOUNT_V1 "
    "where ACCOUNTID = ? OR TOACCOUNTID = ? )";

static const char DELETE_FROM_BUDGETSPLITTRANSACTIONS_V1[] =
    "DELETE FROM BUDGETSPLITTRANSACTIONS_V1 "
    "WHERE TRANSID IN ( SELECT BDID "
    "FROM BILLSDEPOSITS_V1 "
    "WHERE ACCOUNTID = ? OR TOACCOUNTID = ? )";

static const char SELECT_ALL_SYMBOLS_FROM_STOCK_V1[] =
    "SELECT STOCKID, "
    "SYMBOL, "
    "CURRENTPRICE, "
    "NUMSHARES, "
    "STOCKNAME "
    "FROM STOCK_V1 ";

static const char SELECT_ROW_HELDAT_FROM_STOCK_V1[] =
    "select "
    "S.PURCHASEDATE as PURCHASEDATE, "
    "S.STOCKNAME, "
    "S.NUMSHARES as NUMSHARES, "
    "S.VALUE as VALUE, "
    "(S.VALUE - (S.NUMSHARES*S.PURCHASEPRICE + S.COMMISSION)) as GAIN_LOSS, "
    "S.CURRENTPRICE as CURRENTPRICE, "
    "S.NOTES, "
    "T.AVG_PURCHASEPRICE as AVG_PURCHASEPRICE, "
    "S.PURCHASEPRICE as PURCHASEPRICE, "
    "S.STOCKID, S.HELDAT, UPPER(S.SYMBOL) SYMBOL, "
    "S.COMMISSION as COMMISSION, "
    "julianday('now', 'localtime')-julianday (S.PURCHASEDATE, 'localtime') as DAYSOWN, "
    "t.TOTAL_NUMSHARES, T.PURCHASEDTIME, "
    "ifnull (strftime(INFOVALUE, S.PURCHASEDATE),strftime(replace (i.infovalue, '%y', SubStr (strftime('%Y', S.PURCHASEDATE),3,2)),S.PURCHASEDATE)) as PURCHDATE "
    "from STOCK_V1 S "
    "left join infotable_v1 i on i.INFONAME='DATEFORMAT' "
    "left join ( "
    "select count (UPPER (SYMBOL)) as PURCHASEDTIME, "
    "HELDAT, UPPER (SYMBOL) as SYMBOL, "
    "total (NUMSHARES) as TOTAL_NUMSHARES, "
    "total(PURCHASEPRICE*NUMSHARES)/total(NUMSHARES) as AVG_PURCHASEPRICE "
    "from STOCK_V1 "
    "left join infotable_v1 i on i.INFONAME='DATEFORMAT' "
    "group by HELDAT, UPPER (SYMBOL) "
    "order by julianday(min (PURCHASEDATE),'localtime'), SYMBOL, STOCKNAME "
    ") T on UPPER (T.SYMBOL)=UPPER (S.SYMBOL) and T.HELDAT=S.HELDAT "
    "where S.HELDAT = ? ";

static const char SELECT_ROW_FROM_STOCK_V1[] =
    "SELECT HELDAT, STOCKNAME, SYMBOL, NOTES, "
    "PURCHASEDATE, NUMSHARES, VALUE, "
    "PURCHASEPRICE, CURRENTPRICE, COMMISSION "
    "FROM STOCK_V1 "
    "WHERE STOCKID = ?";

//UPDATE
static const char UPDATE_BUDGETYEAR_V1[] =
    "UPDATE BUDGETYEAR_V1 "
    "SET BUDGETYEARNAME = ? "
    "WHERE BUDGETYEARID = ?";

static const char UPDATE_CATEGORY_V1[] =
    "UPDATE CATEGORY_V1 "
    "SET CATEGNAME = ? "
    "WHERE CATEGID = ?";

static const char SET_STATUS_CHECKINGACCOUNT_V1[] =
    "update CHECKINGACCOUNT_V1 set STATUS=? where TRANSID=?";
static const char SET_PAYEEID_CHECKINGACCOUNT_V1[] =
    "UPDATE CHECKINGACCOUNT_V1 SET PAYEEID = ? WHERE PAYEEID = ? ";


static const char UPDATE_INFOTABLE_V1[] =
    "UPDATE INFOTABLE_V1 SET INFOVALUE=? WHERE INFONAME=?";

static const char UPDATE_PAYEE_V1[] =
    "UPDATE PAYEE_V1 "
    "SET PAYEENAME = ?, "
    "CATEGID = ?, "
    "SUBCATEGID = ? "
    "WHERE PAYEEID = ?";

static const char UPDATE_STOCK_V1[] =
    "UPDATE STOCK_V1 "
    "SET CURRENTPRICE = ?, VALUE = ?, STOCKNAME = ? "
    "WHERE STOCKID = ?";

static const char UPDATE_ROW_STOCK_V1[]  =
    "UPDATE STOCK_V1 "
    "SET HELDAT=?, PURCHASEDATE=?, STOCKNAME=?, SYMBOL=?, "
        "NUMSHARES=?, PURCHASEPRICE=?, NOTES=?, CURRENTPRICE=?, "
        "VALUE=?, COMMISSION=? "
    "WHERE STOCKID = ?";

static const char UPDATE_SUBCATEGORY_V1[] =
    "UPDATE SUBCATEGORY_V1 "
    "SET SUBCATEGNAME = ? "
    "WHERE SUBCATEGID = ?";

//INSERT
static const char INSERT_INTO_BUDGETTABLE_V1[] =
    "INSERT INTO BUDGETTABLE_V1 ( "
    "BUDGETYEARID, "
    "CATEGID, "
    "SUBCATEGID, "
    "PERIOD, "
    "AMOUNT "
    ") values ( "
    "?, ?, ?, ?, ? "
    ")";

static const char INSERT_INTO_BUDGETYEARNAME_V1[] =
    "INSERT INTO BUDGETYEAR_V1 (BUDGETYEARNAME) VALUES (?)";

static const char INSERT_INTO_CATEGORY_V1[] =
    "INSERT INTO CATEGORY_V1 (CATEGNAME) VALUES(?)";

static const char INSERT_INTO_INFOTABLE_V1[] =
    "INSERT INTO INFOTABLE_V1 (INFONAME, INFOVALUE) VALUES (?, ?)";

static const char INSERT_ROW_INTO_STOCK_V1[]  =
    "INSERT INTO STOCK_V1 ( "
      "HELDAT, PURCHASEDATE, STOCKNAME, SYMBOL, "
      "NUMSHARES, PURCHASEPRICE, NOTES, CURRENTPRICE, "
      "VALUE, COMMISSION "
    " ) VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?, ?)";

static const char INSERT_ROW_INTO_SUBCATEGORY_V1[] =
    "INSERT INTO SUBCATEGORY_V1 "
    "( SUBCATEGNAME, CATEGID ) "
    "VALUES (:name, :id)";

//CREATE
static const char CREATE_TABLE_ACCOUNTLIST_V1[] =
    "CREATE TABLE ACCOUNTLIST_V1(ACCOUNTID integer primary key, "
    "ACCOUNTNAME TEXT NOT NULL UNIQUE, ACCOUNTTYPE TEXT NOT NULL, ACCOUNTNUM TEXT, "
    "STATUS TEXT NOT NULL, "
    "NOTES TEXT, HELDAT TEXT, WEBSITE TEXT, CONTACTINFO TEXT, "
    "ACCESSINFO TEXT, INITIALBAL numeric, FAVORITEACCT TEXT NOT NULL, "
    "CURRENCYID integer NOT NULL)";

static const char CREATE_TABLE_ASSETS_V1[] =
    "CREATE TABLE ASSETS_V1 (ASSETID integer primary key, "
    "STARTDATE TEXT NOT NULL, ASSETNAME TEXT, "
    "VALUE numeric, VALUECHANGE TEXT, NOTES TEXT, VALUECHANGERATE numeric, "
    "ASSETTYPE TEXT)";

static const char CREATE_TABLE_BILLSDEPOSITS_V1[] =
    "CREATE TABLE BILLSDEPOSITS_V1 (BDID INTEGER PRIMARY KEY, "
    "ACCOUNTID INTEGER NOT NULL, TOACCOUNTID INTEGER, PAYEEID integer NOT NULL, TRANSCODE TEXT NOT NULL, "
    "TRANSAMOUNT numeric NOT NULL, STATUS TEXT, TRANSACTIONNUMBER TEXT, NOTES TEXT, "
    "CATEGID integer, SUBCATEGID integer, TRANSDATE TEXT, FOLLOWUPID integer, TOTRANSAMOUNT numeric, "
    "REPEATS numeric, NEXTOCCURRENCEDATE TEXT, NUMOCCURRENCES numeric)";

static const char CREATE_TABLE_BUDGETTABLE_V1[] =
    "CREATE TABLE BUDGETTABLE_V1(BUDGETENTRYID integer primary key, "
    "BUDGETYEARID integer, CATEGID integer, SUBCATEGID integer, "
    "PERIOD TEXT NOT NULL, AMOUNT numeric NOT NULL)";

static const char CREATE_TABLE_BUDGETSPLITTRANSACTIONS_V1[]=
    "CREATE TABLE BUDGETSPLITTRANSACTIONS_V1(SPLITTRANSID integer primary key, "
    "TRANSID integer NOT NULL, CATEGID integer, SUBCATEGID integer, SPLITTRANSAMOUNT numeric)";

static const char CREATE_TABLE_BUDGETYEAR_V1[] =
    "CREATE TABLE BUDGETYEAR_V1(BUDGETYEARID integer primary key, "
    "BUDGETYEARNAME TEXT NOT NULL UNIQUE)";

static const char CREATE_TABLE_CATEGORY_V1[]=
    "CREATE TABLE CATEGORY_V1(CATEGID integer primary key, "
    "CATEGNAME TEXT NOT NULL)";

static const char CREATE_TABLE_CHECKINGACCOUNT_V1[]=
    "CREATE TABLE CHECKINGACCOUNT_V1(TRANSID integer primary key, "
    "ACCOUNTID integer NOT NULL, TOACCOUNTID integer, PAYEEID integer NOT NULL, TRANSCODE TEXT NOT NULL, "
    "TRANSAMOUNT numeric NOT NULL, STATUS TEXT, TRANSACTIONNUMBER TEXT, NOTES TEXT, "
    "CATEGID integer, SUBCATEGID integer, TRANSDATE TEXT, FOLLOWUPID integer, TOTRANSAMOUNT numeric)";

static const char CREATE_TABLE_CURRENCYFORMATS_V1[] =
    "CREATE TABLE CURRENCYFORMATS_V1 (CURRENCYID integer primary key, "
    "CURRENCYNAME TEXT NOT NULL UNIQUE, PFX_SYMBOL TEXT, SFX_SYMBOL TEXT, "
    "DECIMAL_POINT TEXT, GROUP_SEPARATOR TEXT, "
    "UNIT_NAME TEXT, CENT_NAME TEXT, SCALE numeric, BASECONVRATE numeric, CURRENCY_SYMBOL TEXT)";

static const char CREATE_TABLE_INFOTABLE_V1[] =
    "CREATE TABLE INFOTABLE_V1 "
    "( INFOID integer not null primary key, "
    "INFONAME TEXT NOT NULL UNIQUE, "
    "INFOVALUE TEXT NOT NULL )";

static const char CREATE_TABLE_PAYEE_V1[]=
    "CREATE TABLE PAYEE_V1(PAYEEID integer primary key, "
    "PAYEENAME TEXT NOT NULL UNIQUE, CATEGID integer, SUBCATEGID integer)";

static const char CREATE_TABLE_STOCK_V1[] =
    "CREATE TABLE STOCK_V1(STOCKID integer primary key, "
    "HELDAT numeric, PURCHASEDATE TEXT NOT NULL, STOCKNAME TEXT, SYMBOL TEXT, "
    "NUMSHARES numeric, PURCHASEPRICE numeric NOT NULL, NOTES TEXT, CURRENTPRICE numeric NOT NULL, "
    "VALUE numeric, COMMISSION numeric)";

static const char  CREATE_TABLE_SPLITTRANSACTIONS_V1[]=
    "CREATE TABLE SPLITTRANSACTIONS_V1(SPLITTRANSID integer primary key, "
    "TRANSID numeric NOT NULL, CATEGID integer, SUBCATEGID integer, SPLITTRANSAMOUNT numeric)";

static const char CREATE_TABLE_SUBCATEGORY_V1[]=
    "CREATE TABLE SUBCATEGORY_V1(SUBCATEGID integer primary key, "
    "SUBCATEGNAME TEXT NOT NULL, CATEGID integer NOT NULL)";


namespace mmDBWrapper
{

boost::shared_ptr<wxSQLite3Database> Open(const wxString &dbpath, const wxString &key = wxGetEmptyString());

bool ViewExists(wxSQLite3Database* db, const char *viewName);

/* Creating new DBs */
void initDB(wxSQLite3Database* db);

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

void addBudgetYear(wxSQLite3Database* db, const wxString &year);
bool copyBudgetYear(wxSQLite3Database* db, int newYear, int baseYear);
int getBudgetYearID(wxSQLite3Database* db, const wxString &year_name);
wxString getBudgetYearForID(wxSQLite3Database* db, const int &year_id);
void updateYearForID(wxSQLite3Database* db, const wxString& yearName, int yearid);
bool deleteBudgetYear(wxSQLite3Database* db, const wxString& yearName);

/* Budgeting BUDGETTABLE_V1 API */
void addBudgetEntry(wxSQLite3Database* db, int budgetYearID, int categID, int subCategID, const wxString& period, double amount);
void deleteBudgetEntry(wxSQLite3Database* db, int budgetEntryID);
void updateBudgetEntry(wxSQLite3Database* db, int budgetYearID, int categID, int subCategID, const wxString& period, double amout);
bool getBudgetEntry(wxSQLite3Database* db, int budgetYearID, int categID, int subCategID, wxString& period, double& amt);

/* Account API */
void removeSplitsForAccount(wxSQLite3Database* db, int accountID);

/* Payee Table API */
bool deletePayeeWithConstraints(wxSQLite3Database* db, int payeeID);

/* Category Table API */
bool deleteCategoryWithConstraints(wxSQLite3Database* db, int categID);
bool deleteSubCategoryWithConstraints(wxSQLite3Database* db, int categID, int subcategID);
bool updateCategory(wxSQLite3Database* db, int categID, int subcategID, const wxString &newName);
bool addCategory(wxSQLite3Database* db, const wxString &newName);
bool addSubCategory(wxSQLite3Database* db, int categID, const wxString &newName);

/* Transactions API */
bool updateTransactionWithStatus(wxSQLite3Database &db, int transID, const wxString& status);
bool deleteTransaction(wxSQLite3Database* db, int transID);
int relocatePayee(wxSQLite3Database* db, const int destPayeeID, const int sourcePayeeID);
int relocateCategory(wxSQLite3Database* db, const int destCatID, const int destSubCatID, const int sourceCatID, const int sourceSubCatID);
wxString getSplitTrxNotes(wxSQLite3Database* db_, int trxID);

/* Bills & Deposits API */
void deleteBDSeries(wxSQLite3Database* db, int bdID);
void completeBDInSeries(wxSQLite3Database* db, int bdID);

/* Operations on the CURRENCY */
void loadCurrencySettings(wxSQLite3Database* db, int currencyID);
double getCurrencyBaseConvRate(wxSQLite3Database* db, int accountID);
bool deleteCurrency(wxSQLite3Database* db, int currencyID);
int addCurrency(wxSQLite3Database* db, std::vector<wxString> data);

wxString getLastDbPath(boost::shared_ptr<MMEX_IniSettings> iniSettings, const wxString &defaultVal = wxGetEmptyString());

/* Stocks API */
void deleteStockInvestment(wxSQLite3Database* db, int stockID);
bool moveStockInvestment(wxSQLite3Database* db, int stockID, int toAccountID);
double getStockInvestmentBalance(wxSQLite3Database* db, double& invested);
double getStockInvestmentBalance(wxSQLite3Database* db, int accountID, bool convertToBase, double& originalVal);

/* Assets API */
void deleteAsset(wxSQLite3Database* db, int assetID);
double getAssetBalance(wxSQLite3Database* db);
double getAssetValue(wxSQLite3Database* db, int assetID);

//----------------------------------------------------------------------------

} // namespace mmDBWrapper

//----------------------------------------------------------------------------

#endif // _MM_EX_DBWRAPPER_H_
//----------------------------------------------------------------------------
