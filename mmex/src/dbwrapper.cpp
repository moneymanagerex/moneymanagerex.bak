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

#include "dbwrapper.h"
#include "wx/datetime.h"
#include "util.h"
#include "guiid.h"

void  mmDBWrapper::createInfoV1Table(wxSQLite3Database* db)
{
    mmBEGINSQL_LITE_EXCEPTION;

    /* We always create new INFOTABLE_V1 */
    bool exists = db->TableExists(wxT("INFOTABLE_V1"));
    if (exists)
    {
        mmDBWrapper::setInfoSettingValue(db, wxT("MODIFIEDDATE"), 
            wxDateTime::Now().FormatISODate());
        mmDBWrapper::setInfoSettingValue(db, wxT("DATAVERSION"), MMDATAVERSION);
    }
    else
    {
        /* Create INFOTABLE_V1 Table */
        db->ExecuteUpdate(wxT("create table INFOTABLE_V1(INFOID integer primary key, \
                            INFONAME TEXT NOT NULL, INFOVALUE TEXT);"));
        bool valid = db->TableExists(wxT("INFOTABLE_V1"));
        wxASSERT(valid);

         /* Set DATAVERSION */
        mmDBWrapper::setInfoSettingValue(db, wxT("DATAVERSION"), MMDATAVERSION);

        wxSQLite3StatementBuffer bufSQL;
        bufSQL.Format("insert into INFOTABLE_V1 (INFONAME, INFOVALUE) values ('MMEXVERSION', %Q);", 
            MMEXVERSION);
        int retVal = db->ExecuteUpdate(bufSQL);

        /* Set Created Date */
        mmDBWrapper::setInfoSettingValue(db, wxT("CREATEDATE"), wxDateTime::Now().FormatISODate());

        /* Set Default Date Format */
        mmDBWrapper::setInfoSettingValue(db, wxT("DATEFORMAT"), DEFDATEFORMAT);
    }
    
    mmENDSQL_LITE_EXCEPTION;
}

void mmDBWrapper::createCurrencyV1Table(wxSQLite3Database* db, const wxString& fpath)
{
       mmBEGINSQL_LITE_EXCEPTION;
       bool valid = db->TableExists(wxT("CURRENCYFORMATS_V1"));
       if (!valid)
       {
           db->ExecuteUpdate(wxT("create table CURRENCYFORMATS_V1(CURRENCYID integer primary key, \
                                CURRENCYNAME TEXT NOT NULL, PFX_SYMBOL TEXT, SFX_SYMBOL TEXT,              \
                                DECIMAL_POINT TEXT, GROUP_SEPARATOR TEXT,               \
                                UNIT_NAME TEXT, CENT_NAME TEXT, SCALE numeric, BASECONVRATE numeric);"));
           valid = db->TableExists(wxT("CURRENCYFORMATS_V1"));
           wxASSERT(valid);

           /* Load Default US Currency */
           wxSQLite3StatementBuffer bufSQL;
           bufSQL.Format("insert into CURRENCYFORMATS_V1 (CURRENCYNAME, PFX_SYMBOL,               \
               SFX_SYMBOL, DECIMAL_POINT,                                                         \
               GROUP_SEPARATOR, UNIT_NAME, CENT_NAME, SCALE, BASECONVRATE) values                 \
               ('US DOLLAR', '$', '', '.', ',', 'dollar', 'cents', 100, 1.0);");
           int retVal = db->ExecuteUpdate(bufSQL);

           /* Load Currencies from iniDB */
           mmDBWrapper::loadCurrencies(db, fpath);
       }

       mmENDSQL_LITE_EXCEPTION;
}


void mmDBWrapper::createBudgetingV1Table(wxSQLite3Database* db)
{
    mmBEGINSQL_LITE_EXCEPTION;
    bool valid = db->TableExists(wxT("BUDGETYEAR_V1"));
    if (!valid)
    {
        db->ExecuteUpdate(wxT("create table BUDGETYEAR_V1(BUDGETYEARID integer primary key, \
                             BUDGETYEARNAME TEXT NOT NULL);"));
        valid = db->TableExists(wxT("BUDGETYEAR_V1"));
        wxASSERT(valid);
    }
    
    valid = db->TableExists(wxT("BUDGETTABLE_V1"));
    if (!valid)
    {
        db->ExecuteUpdate(wxT("create table BUDGETTABLE_V1(BUDGETENTRYID integer primary key, \
                             BUDGETYEARID numeric, CATEGID numeric, SUBCATEGID numeric,     \
                             PERIOD TEXT NOT NULL, AMOUNT numeric NOT NULL);"));
        valid = db->TableExists(wxT("BUDGETTABLE_V1"));
        wxASSERT(valid);
    }

    mmENDSQL_LITE_EXCEPTION;
}

void mmDBWrapper::createStockV1Table(wxSQLite3Database* db)
{
    mmBEGINSQL_LITE_EXCEPTION;
    bool valid = db->TableExists(wxT("STOCK_V1"));
    if (!valid)
    {
        db->ExecuteUpdate(wxT("create table STOCK_V1(STOCKID integer primary key, \
                             HELDAT numeric, PURCHASEDATE TEXT NOT NULL, STOCKNAME TEXT, SYMBOL TEXT, \
                             NUMSHARES numeric, PURCHASEPRICE numeric NOT NULL, NOTES TEXT, CURRENTPRICE numeric NOT NULL,\
                             VALUE numeric, COMMISSION numeric);"));
        valid = db->TableExists(wxT("STOCK_V1"));
        wxASSERT(valid);
    }
    
   mmENDSQL_LITE_EXCEPTION;
}

void mmDBWrapper::createBillsDepositsV1Table(wxSQLite3Database* db)
{
    mmBEGINSQL_LITE_EXCEPTION;
    bool valid = db->TableExists(wxT("BILLSDEPOSITS_V1"));
    if (!valid)
    {
        db->ExecuteUpdate(wxT("create table BILLSDEPOSITS_V1(BDID integer primary key, \
               ACCOUNTID numeric NOT NULL, TOACCOUNTID numeric, PAYEEID numeric NOT NULL, TRANSCODE TEXT NOT NULL, \
               TRANSAMOUNT numeric NOT NULL, STATUS TEXT, TRANSACTIONNUMBER TEXT, NOTES TEXT,       \
               CATEGID numeric, SUBCATEGID numeric, TRANSDATE TEXT, FOLLOWUPID numeric, TOTRANSAMOUNT numeric, \
               REPEATS numeric, NEXTOCCURRENCEDATE TEXT, NUMOCCURRENCES numeric);"));
        valid = db->TableExists(wxT("BILLSDEPOSITS_V1"));
        wxASSERT(valid);
    }
    
    mmENDSQL_LITE_EXCEPTION;
}

bool mmDBWrapper::checkDBVersion(wxSQLite3Database* db)
{
    mmBEGINSQL_LITE_EXCEPTION;

    bool valid = db->TableExists(wxT("INFOTABLE_V1"));
    if (!valid)
        return false;

    wxSQLite3StatementBuffer bufSQL;
    bufSQL.Format("select * from INFOTABLE_V1 where INFONAME = 'DATAVERSION'");
    wxSQLite3ResultSet q1 = db->ExecuteQuery(bufSQL);
    if (q1.NextRow())
    {
        int dataVersion = q1.GetInt(wxT("INFOVALUE"));
        q1.Finalize();
        if (dataVersion >= MM_MIN_DATAVERSION)
            return true;
    }
    else 
        return false;

    mmENDSQL_LITE_EXCEPTION;
    return false;
}

void mmDBWrapper::loadCurrencies(wxSQLite3Database* db, const wxString& fpath)
{
    wxSQLite3Database* inidb = new wxSQLite3Database();
    mmBEGINSQL_LITE_EXCEPTION;

    wxString fName = fpath + MMEX_CURRENCYDB_FNAME;
    inidb->Open(fName);

    wxSQLite3StatementBuffer bufSQL;
    bufSQL.Format("select * from CURRENCYFORMATS_V1");
    wxSQLite3ResultSet q1 = inidb->ExecuteQuery(bufSQL);
    
    while(q1.NextRow())
    {
        wxString currencyName = q1.GetString(wxT("CURRENCYNAME"));
        wxString pfxSymbol = q1.GetString(wxT("PFX_SYMBOL"));
        wxString sfxSymbol = q1.GetString(wxT("SFX_SYMBOL"));
        wxString dec = q1.GetString(wxT("DECIMAL_POINT"));
        wxString grp = q1.GetString(wxT("GROUP_SEPARATOR"));
        wxString unit = q1.GetString(wxT("UNIT_NAME"));
        wxString cent = q1.GetString(wxT("CENT_NAME"));
        double scaleDl = q1.GetDouble(wxT("SCALE"));
        double baseConv = q1.GetDouble(wxT("BASECONVRATE"), 1.0);

        wxString sqlStmt = wxString::Format(wxT("insert into CURRENCYFORMATS_V1 (CURRENCYNAME, PFX_SYMBOL, \
            SFX_SYMBOL, DECIMAL_POINT, GROUP_SEPARATOR, UNIT_NAME, CENT_NAME,  \
            SCALE, BASECONVRATE) values ('%s', '%s', '%s', '%s', '%s', '%s', '%s', '%f', '%f');"),
           currencyName.c_str(), pfxSymbol.c_str(), sfxSymbol.c_str(), dec.c_str(), grp.c_str(), unit.c_str(), cent.c_str(), scaleDl, baseConv);
        int retVal = db->ExecuteUpdate(sqlStmt);
    }
    q1.Finalize();
    mmENDSQL_LITE_EXCEPTION;
    
    inidb->Close();
    delete inidb;
}

void mmDBWrapper::createAccountListV1Table(wxSQLite3Database* db)
{
    mmBEGINSQL_LITE_EXCEPTION;
    bool exists = db->TableExists(wxT("ACCOUNTLIST_V1"));
    if (!exists)
    {
        db->ExecuteUpdate(wxT("create table ACCOUNTLIST_V1(ACCOUNTID integer primary key, \
                              ACCOUNTNAME TEXT NOT NULL, ACCOUNTTYPE TEXT NOT NULL, ACCOUNTNUM TEXT, STATUS TEXT NOT NULL, \
                              NOTES TEXT, HELDAT TEXT, WEBSITE TEXT, CONTACTINFO TEXT,       \
                              ACCESSINFO TEXT, INITIALBAL numeric, FAVORITEACCT TEXT NOT NULL, CURRENCYID numeric NOT NULL);"));
        exists = db->TableExists(wxT("ACCOUNTLIST_V1"));
        wxASSERT(exists);
    }
    else
    {
#if 0
        // if we add new fields, we need to alter table as follows
        mmBEGINSQL_LITE_EXCEPTION
            int ret = db->ExecuteUpdate(wxT("alter table ACCOUNTLIST_V1 ADD INFOID TEXT;"));
        mmENDSQL_LITE_IGNOREEXCEPTION;
#endif
    }
    mmENDSQL_LITE_EXCEPTION;
}

void mmDBWrapper::createCheckingAccountV1Table(wxSQLite3Database* db)
{
    mmBEGINSQL_LITE_EXCEPTION;
    bool exists = db->TableExists(wxT("CHECKINGACCOUNT_V1"));
    if (!exists)
    {
        db->ExecuteUpdate(wxT("create table CHECKINGACCOUNT_V1(TRANSID integer primary key, \
                              ACCOUNTID numeric NOT NULL, TOACCOUNTID numeric, PAYEEID numeric NOT NULL, TRANSCODE TEXT NOT NULL, \
                              TRANSAMOUNT numeric NOT NULL, STATUS TEXT, TRANSACTIONNUMBER TEXT, NOTES TEXT,       \
                              CATEGID numeric, SUBCATEGID numeric, TRANSDATE TEXT, FOLLOWUPID numeric, TOTRANSAMOUNT numeric);"));
        exists = db->TableExists(wxT("CHECKINGACCOUNT_V1"));
        wxASSERT(exists);
    }
    mmENDSQL_LITE_EXCEPTION;
}

void mmDBWrapper::createPayeeV1Table(wxSQLite3Database* db)
{
    mmBEGINSQL_LITE_EXCEPTION;
    bool exists = db->TableExists(wxT("PAYEE_V1"));
    if (!exists)
    {
        db->ExecuteUpdate(wxT("create table PAYEE_V1(PAYEEID integer primary key, \
                             PAYEENAME TEXT NOT NULL, CATEGID numeric, SUBCATEGID numeric);"));
        exists = db->TableExists(wxT("PAYEE_V1"));
        wxASSERT(exists);
    }
    mmENDSQL_LITE_EXCEPTION;
}

void mmDBWrapper::createCategoryV1Table(wxSQLite3Database* db)
{
    mmBEGINSQL_LITE_EXCEPTION;
    bool existsCat = db->TableExists(wxT("CATEGORY_V1"));
    if (!existsCat)
    {
        /* Create CATEGORY_V1 Tables */
        db->ExecuteUpdate(wxT("create table CATEGORY_V1(CATEGID integer primary key, \
                             CATEGNAME TEXT NOT NULL);"));
        bool exists = db->TableExists(wxT("CATEGORY_V1"));
        wxASSERT(exists);
    }

    bool existsSubCat = db->TableExists(wxT("SUBCATEGORY_V1"));
    if (!existsSubCat)
    {
        /* Create SUBCATEGORY_V1 Tables */
        db->ExecuteUpdate(wxT("create table SUBCATEGORY_V1(SUBCATEGID integer primary key, \
                             SUBCATEGNAME TEXT NOT NULL, CATEGID numeric NOT NULL);"));
        existsSubCat = db->TableExists(wxT("SUBCATEGORY_V1"));
        wxASSERT(existsSubCat);
    }

    if (!existsCat)
    {
        /* Update Category Tables with prebuilt categories */
        db->ExecuteUpdate("insert into CATEGORY_V1 (CATEGNAME) VALUES('Bills')");
        wxSQLite3StatementBuffer bufSQL;
        bufSQL.Format("insert into SUBCATEGORY_V1 (SUBCATEGNAME, CATEGID) values ('Telephone', %d);", 
            mmDBWrapper::getCategoryID(db, wxT("Bills")));
        db->ExecuteUpdate(bufSQL);
        bufSQL.Format("insert into SUBCATEGORY_V1 (SUBCATEGNAME, CATEGID) values ('Electricity', %d);", 
            mmDBWrapper::getCategoryID(db, wxT("Bills")));
        db->ExecuteUpdate(bufSQL);
        bufSQL.Format("insert into SUBCATEGORY_V1 (SUBCATEGNAME, CATEGID) values ('Gas', %d);", 
            mmDBWrapper::getCategoryID(db, wxT("Bills")));
        db->ExecuteUpdate(bufSQL);
        bufSQL.Format("insert into SUBCATEGORY_V1 (SUBCATEGNAME, CATEGID) values ('Internet', %d);", 
            mmDBWrapper::getCategoryID(db, wxT("Bills")));
        db->ExecuteUpdate(bufSQL);
        bufSQL.Format("insert into SUBCATEGORY_V1 (SUBCATEGNAME, CATEGID) values ('Rent', %d);", 
            mmDBWrapper::getCategoryID(db, wxT("Bills")));
        db->ExecuteUpdate(bufSQL);
        bufSQL.Format("insert into SUBCATEGORY_V1 (SUBCATEGNAME, CATEGID) values ('Cable TV', %d);", 
            mmDBWrapper::getCategoryID(db, wxT("Bills")));
        db->ExecuteUpdate(bufSQL);
        bufSQL.Format("insert into SUBCATEGORY_V1 (SUBCATEGNAME, CATEGID) values ('Water', %d);", 
            mmDBWrapper::getCategoryID(db, wxT("Bills")));
        db->ExecuteUpdate(bufSQL);

        db->ExecuteUpdate("insert into CATEGORY_V1 (CATEGNAME) VALUES('Food')");
        bufSQL.Format("insert into SUBCATEGORY_V1 (SUBCATEGNAME, CATEGID) values ('Groceries', %d);", 
            mmDBWrapper::getCategoryID(db, wxT("Food")));
        db->ExecuteUpdate(bufSQL);
        bufSQL.Format("insert into SUBCATEGORY_V1 (SUBCATEGNAME, CATEGID) values ('Dining out', %d);", 
            mmDBWrapper::getCategoryID(db, wxT("Food")));
        db->ExecuteUpdate(bufSQL);

        db->ExecuteUpdate("insert into CATEGORY_V1 (CATEGNAME) VALUES('Leisure')");
        bufSQL.Format("insert into SUBCATEGORY_V1 (SUBCATEGNAME, CATEGID) values ('Movies', %d);", 
            mmDBWrapper::getCategoryID(db, wxT("Leisure")));
        db->ExecuteUpdate(bufSQL);
        bufSQL.Format("insert into SUBCATEGORY_V1 (SUBCATEGNAME, CATEGID) values ('Video Rental', %d);", 
            mmDBWrapper::getCategoryID(db, wxT("Leisure")));
        db->ExecuteUpdate(bufSQL);
        bufSQL.Format("insert into SUBCATEGORY_V1 (SUBCATEGNAME, CATEGID) values ('Magazines', %d);", 
            mmDBWrapper::getCategoryID(db, wxT("Leisure")));
        db->ExecuteUpdate(bufSQL);

        db->ExecuteUpdate("insert into CATEGORY_V1 (CATEGNAME) VALUES('Automobile')");
        bufSQL.Format("insert into SUBCATEGORY_V1 (SUBCATEGNAME, CATEGID) values ('Maintenance', %d);", 
            mmDBWrapper::getCategoryID(db, wxT("Automobile")));
        db->ExecuteUpdate(bufSQL);
        bufSQL.Format("insert into SUBCATEGORY_V1 (SUBCATEGNAME, CATEGID) values ('Gas', %d);", 
            mmDBWrapper::getCategoryID(db, wxT("Automobile")));
        db->ExecuteUpdate(bufSQL);
        bufSQL.Format("insert into SUBCATEGORY_V1 (SUBCATEGNAME, CATEGID) values ('Parking', %d);", 
            mmDBWrapper::getCategoryID(db, wxT("Automobile")));
        db->ExecuteUpdate(bufSQL);
        bufSQL.Format("insert into SUBCATEGORY_V1 (SUBCATEGNAME, CATEGID) values ('Registration', %d);", 
            mmDBWrapper::getCategoryID(db, wxT("Automobile")));
        db->ExecuteUpdate(bufSQL);

        db->ExecuteUpdate(wxT("insert into CATEGORY_V1 (CATEGNAME) VALUES('Education')"));
        bufSQL.Format("insert into SUBCATEGORY_V1 (SUBCATEGNAME, CATEGID) values ('Books', %d);", 
            mmDBWrapper::getCategoryID(db, wxT("Education")));
        db->ExecuteUpdate(bufSQL);
        bufSQL.Format("insert into SUBCATEGORY_V1 (SUBCATEGNAME, CATEGID) values ('Tuition', %d);", 
            mmDBWrapper::getCategoryID(db, wxT("Education")));
        db->ExecuteUpdate(bufSQL);
        bufSQL.Format("insert into SUBCATEGORY_V1 (SUBCATEGNAME, CATEGID) values ('Other', %d);", 
            mmDBWrapper::getCategoryID(db, wxT("Education")));
        db->ExecuteUpdate(bufSQL);

        db->ExecuteUpdate("insert into CATEGORY_V1 (CATEGNAME) VALUES('Homeneeds')");
        bufSQL.Format("insert into SUBCATEGORY_V1 (SUBCATEGNAME, CATEGID) values ('Clothing', %d);", 
            mmDBWrapper::getCategoryID(db, wxT("Homeneeds")));
        db->ExecuteUpdate(bufSQL);
        bufSQL.Format("insert into SUBCATEGORY_V1 (SUBCATEGNAME, CATEGID) values ('Furnishing', %d);", 
            mmDBWrapper::getCategoryID(db, wxT("Homeneeds")));
        db->ExecuteUpdate(bufSQL);
        bufSQL.Format("insert into SUBCATEGORY_V1 (SUBCATEGNAME, CATEGID) values ('Other', %d);", 
            mmDBWrapper::getCategoryID(db, wxT("Homeneeds")));
        db->ExecuteUpdate(bufSQL);

        db->ExecuteUpdate("insert into CATEGORY_V1 (CATEGNAME) VALUES('Healthcare')");
        bufSQL.Format("insert into SUBCATEGORY_V1 (SUBCATEGNAME, CATEGID) values ('Health', %d);", 
            mmDBWrapper::getCategoryID(db, wxT("Healthcare")));
        db->ExecuteUpdate(bufSQL);
        bufSQL.Format("insert into SUBCATEGORY_V1 (SUBCATEGNAME, CATEGID) values ('Dental', %d);", 
            mmDBWrapper::getCategoryID(db, wxT("Healthcare")));
        db->ExecuteUpdate(bufSQL);
        bufSQL.Format("insert into SUBCATEGORY_V1 (SUBCATEGNAME, CATEGID) values ('Eyecare', %d);", 
            mmDBWrapper::getCategoryID(db, wxT("Healthcare")));
        db->ExecuteUpdate(bufSQL);
        bufSQL.Format("insert into SUBCATEGORY_V1 (SUBCATEGNAME, CATEGID) values ('Physician', %d);", 
            mmDBWrapper::getCategoryID(db, wxT("Healthcare")));
        db->ExecuteUpdate(bufSQL);
        bufSQL.Format("insert into SUBCATEGORY_V1 (SUBCATEGNAME, CATEGID) values ('Prescriptions', %d);", 
            mmDBWrapper::getCategoryID(db, wxT("Healthcare")));
        db->ExecuteUpdate(bufSQL);

        db->ExecuteUpdate("insert into CATEGORY_V1 (CATEGNAME) VALUES('Insurance')");
        bufSQL.Format("insert into SUBCATEGORY_V1 (SUBCATEGNAME, CATEGID) values ('Auto', %d);", 
            mmDBWrapper::getCategoryID(db, wxT("Insurance")));
        db->ExecuteUpdate(bufSQL);
        bufSQL.Format("insert into SUBCATEGORY_V1 (SUBCATEGNAME, CATEGID) values ('Life', %d);", 
            mmDBWrapper::getCategoryID(db, wxT("Insurance")));
        db->ExecuteUpdate(bufSQL);
        bufSQL.Format("insert into SUBCATEGORY_V1 (SUBCATEGNAME, CATEGID) values ('Home', %d);", 
            mmDBWrapper::getCategoryID(db, wxT("Insurance")));
        db->ExecuteUpdate(bufSQL);
        bufSQL.Format("insert into SUBCATEGORY_V1 (SUBCATEGNAME, CATEGID) values ('Health', %d);", 
            mmDBWrapper::getCategoryID(db, wxT("Insurance")));
        db->ExecuteUpdate(bufSQL);

        db->ExecuteUpdate("insert into CATEGORY_V1 (CATEGNAME) VALUES('Vacation')");
        bufSQL.Format("insert into SUBCATEGORY_V1 (SUBCATEGNAME, CATEGID) values ('Travel', %d);", 
            mmDBWrapper::getCategoryID(db, wxT("Vacation")));
        db->ExecuteUpdate(bufSQL);
        bufSQL.Format("insert into SUBCATEGORY_V1 (SUBCATEGNAME, CATEGID) values ('Lodging', %d);", 
            mmDBWrapper::getCategoryID(db, wxT("Vacation")));
        db->ExecuteUpdate(bufSQL);
        bufSQL.Format("insert into SUBCATEGORY_V1 (SUBCATEGNAME, CATEGID) values ('Sightseeing', %d);", 
            mmDBWrapper::getCategoryID(db, wxT("Vacation")));
        db->ExecuteUpdate(bufSQL);

        db->ExecuteUpdate("insert into CATEGORY_V1 (CATEGNAME) VALUES('Taxes')");
        bufSQL.Format("insert into SUBCATEGORY_V1 (SUBCATEGNAME, CATEGID) values ('Income Tax', %d);", 
            mmDBWrapper::getCategoryID(db, wxT("Taxes")));
        db->ExecuteUpdate(bufSQL);
        bufSQL.Format("insert into SUBCATEGORY_V1 (SUBCATEGNAME, CATEGID) values ('House Tax', %d);", 
            mmDBWrapper::getCategoryID(db, wxT("Taxes")));
        db->ExecuteUpdate(bufSQL);
        bufSQL.Format("insert into SUBCATEGORY_V1 (SUBCATEGNAME, CATEGID) values ('Water Tax', %d);", 
            mmDBWrapper::getCategoryID(db, wxT("Taxes")));
        db->ExecuteUpdate(bufSQL);
        bufSQL.Format("insert into SUBCATEGORY_V1 (SUBCATEGNAME, CATEGID) values ('Others', %d);", 
            mmDBWrapper::getCategoryID(db, wxT("Taxes")));
        db->ExecuteUpdate(bufSQL);

        db->ExecuteUpdate("insert into CATEGORY_V1 (CATEGNAME) VALUES('Miscellaneous')");
        db->ExecuteUpdate("insert into CATEGORY_V1 (CATEGNAME) VALUES('Gifts')");
        db->ExecuteUpdate("insert into CATEGORY_V1 (CATEGNAME) VALUES('Income')");
        bufSQL.Format("insert into SUBCATEGORY_V1 (SUBCATEGNAME, CATEGID) values ('Salary', %d);", 
            mmDBWrapper::getCategoryID(db, wxT("Income")));
        db->ExecuteUpdate(bufSQL);
        bufSQL.Format("insert into SUBCATEGORY_V1 (SUBCATEGNAME, CATEGID) values ('Reimbursement/Refunds', %d);", 
            mmDBWrapper::getCategoryID(db, wxT("Income")));
        db->ExecuteUpdate(bufSQL);
        bufSQL.Format("insert into SUBCATEGORY_V1 (SUBCATEGNAME, CATEGID) values ('Investment Income', %d);", 
            mmDBWrapper::getCategoryID(db, wxT("Income")));
        db->ExecuteUpdate(bufSQL);

        db->ExecuteUpdate("insert into CATEGORY_V1 (CATEGNAME) VALUES('Other Income')");
        db->ExecuteUpdate("insert into CATEGORY_V1 (CATEGNAME) VALUES('Other Expenses')");
        db->ExecuteUpdate("insert into CATEGORY_V1 (CATEGNAME) VALUES('Transfer')");
    }

    mmENDSQL_LITE_EXCEPTION;
}

/*
This routine sets up a new DB as well as update an old one
*/

void mmDBWrapper::initDB(wxSQLite3Database* db, wxProgressDialog* pgd, const wxString& fpath)
{
    mmBEGINSQL_LITE_EXCEPTION;

    /* Create INFOTABLE_V1 Tables */
    mmDBWrapper::createInfoV1Table(db);
    pgd->Update(10);

    /* Create Currency Settings */
    mmDBWrapper::createCurrencyV1Table(db, fpath);
    pgd->Update(20);

    /* Create ACCOUNTLIST_V1 Tables */
    mmDBWrapper::createAccountListV1Table(db);
    pgd->Update(30);

    /* Create CHECKINGACCOUNT_V1 Tables */
    mmDBWrapper::createCheckingAccountV1Table(db);
    pgd->Update(40);

    /* Create PAYEE_V1 Tables */
    mmDBWrapper::createPayeeV1Table(db);
    pgd->Update(50);
  
    /* Create CATEGORY_V1 Tables */
    mmDBWrapper::createCategoryV1Table(db);
    pgd->Update(60);

    /* Create Budgeting_V1 Tables */
    mmDBWrapper::createBudgetingV1Table(db);
    pgd->Update(70);

    /* Create Bills & Deposits V1 Table */
    mmDBWrapper::createBillsDepositsV1Table(db);
    pgd->Update(80);

    /* Create Stock V1 Table */
    mmDBWrapper::createStockV1Table(db);
    pgd->Update(90);

    mmENDSQL_LITE_EXCEPTION;
}

wxUint32 mmDBWrapper::getNumAccounts(wxSQLite3Database* db)
{
    mmBEGINSQL_LITE_EXCEPTION;

    wxUint32 num = db->ExecuteScalar("select count(*) from ACCOUNTLIST_V1;");
    return num;

    mmENDSQL_LITE_EXCEPTION;

    return 0;
}

wxString mmDBWrapper::getAccountName(wxSQLite3Database* db, int accountID)
{
    wxString name;
    mmBEGINSQL_LITE_EXCEPTION;

    wxSQLite3StatementBuffer bufSQL;
    bufSQL.Format("select ACCOUNTNAME from ACCOUNTLIST_V1 where ACCOUNTID = %d;", accountID);
    wxSQLite3ResultSet q1 = db->ExecuteQuery(bufSQL);
    if (q1.NextRow())
    {
        name = q1.GetString(wxT("ACCOUNTNAME"));
        q1.Finalize();
    }
    else
    {
        wxASSERT(true);
    }

    mmENDSQL_LITE_EXCEPTION;

    return name;
}

int mmDBWrapper::getAccountID(wxSQLite3Database* db, const wxString& accountName)
{
    int id = -1;
    mmBEGINSQL_LITE_EXCEPTION;

    wxString bufSQL = wxString::Format(wxT("select * from ACCOUNTLIST_V1 where ACCOUNTNAME = '%s';"), 
        mmCleanString(accountName).c_str());
    wxSQLite3ResultSet q1 = db->ExecuteQuery(bufSQL);
    if (q1.NextRow())
    {
        id = q1.GetInt(wxT("ACCOUNTID"));
        q1.Finalize();
    }

    mmENDSQL_LITE_EXCEPTION;
    return id;
}

void mmDBWrapper::loadSettings(int accountID, wxSQLite3Database* db)
{
    mmBEGINSQL_LITE_EXCEPTION;
    wxSQLite3StatementBuffer bufSQL;
    bufSQL.Format("select * from ACCOUNTLIST_V1 where ACCOUNTID = %d;", accountID);
    wxSQLite3ResultSet q1 = db->ExecuteQuery(bufSQL);
    if (q1.NextRow())
    {
        int currencyID = q1.GetInt(wxT("CURRENCYID"));
        mmDBWrapper::loadSettings(db, currencyID);
    }
    q1.Finalize();
    mmENDSQL_LITE_EXCEPTION;
}


int mmDBWrapper::getBaseCurrencySettings(wxSQLite3Database* db)
{
    int currencyID = -1;
    mmBEGINSQL_LITE_EXCEPTION;
    wxSQLite3StatementBuffer bufSQL;
    bufSQL.Format("select * from INFOTABLE_V1 where INFONAME = 'BASECURRENCYID'");
    wxSQLite3ResultSet q1 = db->ExecuteQuery(bufSQL);
    if (q1.NextRow())
    {
        currencyID = q1.GetInt(wxT("INFOVALUE"), -1);
    }
    q1.Finalize();
    mmENDSQL_LITE_EXCEPTION;
    return currencyID;
}

void mmDBWrapper::setBaseCurrencySettings(wxSQLite3Database* db, int currencyID)
{
    mmBEGINSQL_LITE_EXCEPTION;
    if (getBaseCurrencySettings(db) == -1)
    {
        wxString bufSQL = wxString::Format(wxT("insert into INFOTABLE_V1 (INFONAME, INFOVALUE) values ('BASECURRENCYID', %d);"), currencyID);
        int retVal = db->ExecuteUpdate(bufSQL);
    }
    else
    {
        wxString bufSQL = wxString::Format(wxT("update INFOTABLE_V1 SET INFOVALUE=%d WHERE INFONAME='BASECURRENCYID';"), currencyID);
        int retVal = db->ExecuteUpdate(bufSQL);
    }
    mmENDSQL_LITE_EXCEPTION;
}

void mmDBWrapper::loadBaseCurrencySettings(wxSQLite3Database* db)
{
    int currencyID = mmDBWrapper::getBaseCurrencySettings(db);
    if (currencyID != -1)
    {
        mmDBWrapper::loadSettings(db, currencyID);
    }
    else
    {
        mmCurrencyFormatter::loadDefaultSettings();
    }
}

void mmDBWrapper::loadSettings(wxSQLite3Database* db, int currencyID)
{
    mmBEGINSQL_LITE_EXCEPTION;
    wxSQLite3StatementBuffer bufSQL;
    bufSQL.Format("select * from CURRENCYFORMATS_V1 where CURRENCYID = %d;", currencyID);
    wxSQLite3ResultSet q1 = db->ExecuteQuery(bufSQL);
    
    if (q1.NextRow())
    {
        wxString pfxSymbol = q1.GetString(wxT("PFX_SYMBOL"));
        wxString sfxSymbol = q1.GetString(wxT("SFX_SYMBOL"));
        wxString dec = q1.GetString(wxT("DECIMAL_POINT"));
        wxString grp = q1.GetString(wxT("GROUP_SEPARATOR"));
        wxString unit = q1.GetString(wxT("UNIT_NAME"));
        wxString cent = q1.GetString(wxT("CENT_NAME"));
        double scaleDl = q1.GetDouble(wxT("SCALE"));
        int currencyID = q1.GetInt(wxT("CURRENCYID"));
        double convRate = q1.GetDouble(wxT("BASECONVRATE"));
        mmCurrencyFormatter::loadSettings(pfxSymbol, sfxSymbol, 
            dec.GetChar(0), grp.GetChar(0), unit, cent, scaleDl);

        q1.Finalize();
    }
    else
    {
        wxASSERT(true);
    }
    mmENDSQL_LITE_EXCEPTION;
}

double mmDBWrapper::getReconciledBalanceOnAccount(wxSQLite3Database* db, int accountID)
{
    double balance = 0.0;
    mmBEGINSQL_LITE_EXCEPTION;

    wxSQLite3StatementBuffer bufSQL;

    bufSQL.Format("select * from ACCOUNTLIST_V1 where ACCOUNTID=%d;", accountID);
    wxSQLite3ResultSet q1 = db->ExecuteQuery(bufSQL);
    balance = q1.GetDouble(wxT("INITIALBAL"));
    q1.Finalize();
    
    bufSQL.Format("select * from CHECKINGACCOUNT_V1 where ACCOUNTID=%d OR TOACCOUNTID=%d;", accountID, 
            accountID);
    q1 = db->ExecuteQuery(bufSQL);
    while (q1.NextRow())
    {
        wxString transTypeString = q1.GetString(wxT("TRANSCODE"));
        wxString transStatus     = q1.GetString(wxT("STATUS"));
        int transactionFromID = q1.GetInt(wxT("ACCOUNTID"));
        int transactionToID = q1.GetInt(wxT("TOACCOUNTID"));
        double transAmount = q1.GetDouble(wxT("TRANSAMOUNT"));
        double toTransAmount = q1.GetDouble(wxT("TOTRANSAMOUNT"));

        // we want only reconciled transactions
        if (transStatus != wxT("R"))
            continue; // skip

        if (transTypeString == wxT("Deposit"))
            balance += transAmount;
        else if (transTypeString == wxT("Withdrawal"))
            balance -= transAmount;
        else if (transTypeString == wxT("Transfer"))
        {
            if (transactionFromID == accountID)
            {
                balance -= transAmount;
            }
            else if (transactionToID == accountID)
            {
                balance += toTransAmount;
            }
        }
        else
        {
            wxASSERT(false);
        }
    }
    q1.Finalize();

    mmENDSQL_LITE_EXCEPTION;
    return balance;
}

double mmDBWrapper::getTotalBalanceOnAccount(wxSQLite3Database* db, int accountID, bool ignoreFuture)
{
    double balance = 0.0;
    mmBEGINSQL_LITE_EXCEPTION;

    wxSQLite3StatementBuffer bufSQL;

    bufSQL.Format("select * from ACCOUNTLIST_V1 where ACCOUNTID=%d;", accountID);
    wxSQLite3ResultSet q1 = db->ExecuteQuery(bufSQL);
    balance = q1.GetDouble(wxT("INITIALBAL"));
    q1.Finalize();
    
    bufSQL.Format("select * from CHECKINGACCOUNT_V1 where ACCOUNTID=%d OR TOACCOUNTID=%d;", accountID, 
            accountID);
    q1 = db->ExecuteQuery(bufSQL);
    while (q1.NextRow())
    {
        wxString transTypeString = q1.GetString(wxT("TRANSCODE"));
        wxString transStatus     = q1.GetString(wxT("STATUS"));
        int transactionFromID = q1.GetInt(wxT("ACCOUNTID"));
        int transactionToID = q1.GetInt(wxT("TOACCOUNTID"));
        double transAmount = q1.GetDouble(wxT("TRANSAMOUNT"));
        double toTransAmount = q1.GetDouble(wxT("TOTRANSAMOUNT"));
        wxString dateString = q1.GetString(wxT("TRANSDATE"));
        wxDateTime dtdt = mmGetStorageStringAsDate(dateString);

        if (transStatus == wxT("V"))
           continue; // skip

        if (ignoreFuture)
        {
            if (dtdt.IsLaterThan(wxDateTime::Now()))
                continue; //skip future dated transactions
        }

        if (transTypeString == wxT("Deposit"))
            balance += transAmount;
        else if (transTypeString == wxT("Withdrawal"))
            balance -= transAmount;
        else if (transTypeString == wxT("Transfer"))
        {
            if (transactionFromID == accountID)
            {
                balance -= transAmount;
            }
            else if (transactionToID == accountID)
            {
                balance += toTransAmount;
            }
        }
        else
        {
            wxASSERT(false);
        }
    }
    q1.Finalize();

    mmENDSQL_LITE_EXCEPTION;
    return balance;
}

bool mmDBWrapper::getExpensesIncome(wxSQLite3Database* db, int accountID, 
                                    double& expenses, double& income,  
                                    bool ignoreDate, wxDateTime dtBegin, wxDateTime dtEnd)
{
    mmBEGINSQL_LITE_EXCEPTION;

    wxString bufSQL;
    if (accountID != -1)
    {
        bufSQL = wxString::Format(wxT("select * from CHECKINGACCOUNT_V1 where ACCOUNTID=%d OR TOACCOUNTID=%d;"), accountID, 
                accountID);
    }
    else
    {
        bufSQL = wxString::Format(wxT("select * from CHECKINGACCOUNT_V1;"));
    }
    wxSQLite3ResultSet q1 = db->ExecuteQuery(bufSQL);
    while (q1.NextRow())
    {
        wxString transTypeString = q1.GetString(wxT("TRANSCODE"));
        int transactionFromID = q1.GetInt(wxT("ACCOUNTID"));
        int transactionToID = q1.GetInt(wxT("TOACCOUNTID"));
        double transAmount = q1.GetDouble(wxT("TRANSAMOUNT"));
        wxString transStatus = q1.GetString(wxT("STATUS"));
        wxString dateString = q1.GetString(wxT("TRANSDATE"));
        wxDateTime dtdt = mmGetStorageStringAsDate(dateString);

        if (transStatus == wxT("V"))
           continue; // skip

        if (!ignoreDate)
        {
            if (!dtdt.IsBetween(dtBegin, dtEnd))
                continue; //skip
        }


        if (transTypeString == wxT("Deposit"))
            income += transAmount;
        else if (transTypeString == wxT("Withdrawal"))
            expenses += transAmount;
        else if (transTypeString == wxT("Transfer"))
        {
            // transfers are not considered in income/expenses calculations
        }
    }
    q1.Finalize();

    mmENDSQL_LITE_EXCEPTION;
    return true;
}


void mmDBWrapper::addPayee(wxSQLite3Database* db, wxString payee, int categID, int subcategID)
{
    mmBEGINSQL_LITE_EXCEPTION;
    wxString bufSQL = wxString::Format(wxT("insert into PAYEE_V1 (PAYEENAME, CATEGID, SUBCATEGID) values ('%s', %d, %d);"), 
        mmCleanString(payee).c_str(), categID, subcategID);
    int retVal = db->ExecuteUpdate(bufSQL);
    mmENDSQL_LITE_EXCEPTION;
}

bool mmDBWrapper::getPayeeID(wxSQLite3Database* db, wxString payee, int& payeeID, 
                             int& categID, int& subcategID )
{
    mmBEGINSQL_LITE_EXCEPTION;
    wxString bufSQL = wxString::Format(wxT("select * from PAYEE_V1 where PAYEENAME='%s';"), 
        mmCleanString(payee).c_str());
    wxSQLite3ResultSet q1 = db->ExecuteQuery(bufSQL);
    if (q1.NextRow())
    {
        payeeID = q1.GetInt(wxT("PAYEEID"));
        categID = q1.GetInt(wxT("CATEGID"));
        subcategID = q1.GetInt(wxT("SUBCATEGID"));
        q1.Finalize();
        return true;
    }
    q1.Finalize();
    mmENDSQL_LITE_EXCEPTION;
    return false;
}

bool mmDBWrapper::deleteCategoryWithConstraints(wxSQLite3Database* db, int categID)
{
    mmBEGINSQL_LITE_EXCEPTION;

    wxSQLite3StatementBuffer bufSQL;
    bufSQL.Format("select * from CHECKINGACCOUNT_V1 where CATEGID=%d;", categID);
    wxSQLite3ResultSet q1 = db->ExecuteQuery(bufSQL);
    if (q1.NextRow())
    {
        /* Records exist */
        q1.Finalize();
        return false;
    }
    q1.Finalize();
    
    wxSQLite3StatementBuffer bufSQL1;
    bufSQL1.Format("delete from SUBCATEGORY_V1 where CATEGID=%d;", categID);
    db->ExecuteUpdate(bufSQL1);

    wxSQLite3StatementBuffer bufSQL2;
    bufSQL2.Format("delete from CATEGORY_V1 where CATEGID=%d;", categID);
    db->ExecuteUpdate(bufSQL2);

    wxSQLite3StatementBuffer bufSQL3;
    bufSQL.Format("delete from BUDGETTABLE_V1 WHERE CATEGID=%d;", 
         categID);
    db->ExecuteUpdate(bufSQL3);
    
    mmENDSQL_LITE_EXCEPTION;
    return true;
}

bool mmDBWrapper::deleteSubCategoryWithConstraints(wxSQLite3Database* db, int categID, int subcategID)
{
    mmBEGINSQL_LITE_EXCEPTION;
    wxSQLite3StatementBuffer bufSQL;
    bufSQL.Format("select * from CHECKINGACCOUNT_V1 where CATEGID=%d and SUBCATEGID=%d;",
        categID, subcategID);
    wxSQLite3ResultSet q1 = db->ExecuteQuery(bufSQL);
    if (q1.NextRow())
    {
        /* Records exist */
        q1.Finalize();
        return false;
    }
    q1.Finalize();
    
    wxSQLite3StatementBuffer bufSQL1;
    bufSQL1.Format("delete from SUBCATEGORY_V1 where CATEGID=%d and SUBCATEGID=%d;", 
        categID, subcategID);
    db->ExecuteUpdate(bufSQL1);

    wxSQLite3StatementBuffer bufSQL3;
    bufSQL.Format("delete from BUDGETTABLE_V1 WHERE CATEGID=%d AND SUBCATEGID=%d;", 
        categID, subcategID);
    db->ExecuteUpdate(bufSQL3);

    mmENDSQL_LITE_EXCEPTION;
    return true;
}

bool mmDBWrapper::updateCategory(wxSQLite3Database* db, int categID, 
                                 int subcategID, wxString newName)
{
    mmBEGINSQL_LITE_EXCEPTION;

    if (subcategID == -1)
    {
        wxString bufSQL = wxString::Format(wxT("update CATEGORY_V1 SET CATEGNAME='%s' WHERE CATEGID=%d;"), mmCleanString(newName).c_str(), categID);
        db->ExecuteUpdate(bufSQL);
    }
    else
    {
        wxString bufSQL1 = wxString::Format(wxT("update SUBCATEGORY_V1 SET SUBCATEGNAME='%s' WHERE SUBCATEGID=%d;"), mmCleanString(newName).c_str(), subcategID);
        db->ExecuteUpdate(bufSQL1);
    }
    
    mmENDSQL_LITE_EXCEPTION;
    return true;
}

bool mmDBWrapper::addCategory(wxSQLite3Database* db, wxString newName)
{
    mmBEGINSQL_LITE_EXCEPTION;
    wxString bufSQL = wxString::Format(wxT("insert into CATEGORY_V1 (CATEGNAME) values ('%s');"), 
        mmCleanString(newName).c_str());
    int retVal = db->ExecuteUpdate(bufSQL);
    mmENDSQL_LITE_EXCEPTION;
    return true;
}

bool mmDBWrapper::addSubCategory(wxSQLite3Database* db, int categID, wxString newName)
{
    mmBEGINSQL_LITE_EXCEPTION;
    wxString bufSQL = wxString::Format(wxT("insert into SUBCATEGORY_V1 (SUBCATEGNAME, CATEGID) values ('%s', %d);"), mmCleanString(newName).c_str(), categID);
    int retVal = db->ExecuteUpdate(bufSQL);
    mmENDSQL_LITE_EXCEPTION;
    return true;
}

int mmDBWrapper::getCategoryID(wxSQLite3Database* db, wxString name)
{
    int categID = -1;
    mmBEGINSQL_LITE_EXCEPTION
    wxString bufSQL = wxString::Format(wxT("select * from CATEGORY_V1 where CATEGNAME='%s';"), mmCleanString(name).c_str());
    wxSQLite3ResultSet q1 = db->ExecuteQuery(bufSQL);
    if (q1.NextRow())
    {
        categID = q1.GetInt(wxT("CATEGID"));
    }
    q1.Finalize();
    mmENDSQL_LITE_EXCEPTION
    return categID;
}

int mmDBWrapper::getSubCategoryID(wxSQLite3Database* db, int categID, wxString name)
{
    int subcategID = -1;
    mmBEGINSQL_LITE_EXCEPTION
    wxString bufSQL = wxString::Format(wxT("select * from SUBCATEGORY_V1 where SUBCATEGNAME='%s' and CATEGID=%d;"), mmCleanString(name).c_str(), categID);
    wxSQLite3ResultSet q1 = db->ExecuteQuery(bufSQL);
    if (q1.NextRow())
    {
        subcategID = q1.GetInt(wxT("SUBCATEGID"));
    }
    q1.Finalize();
    mmENDSQL_LITE_EXCEPTION
    return subcategID;
}

wxString mmDBWrapper::getCategoryName(wxSQLite3Database* db, int categID)
{
    wxString name = wxT("");
    mmBEGINSQL_LITE_EXCEPTION
    wxSQLite3StatementBuffer bufSQL;
    bufSQL.Format("select * from CATEGORY_V1 where CATEGID=%d;", categID);
    wxSQLite3ResultSet q1 = db->ExecuteQuery(bufSQL);
    if (q1.NextRow())
    {
        name = q1.GetString(wxT("CATEGNAME"));
    }
    q1.Finalize();
    mmENDSQL_LITE_EXCEPTION
    return name;
}

wxString mmDBWrapper::getSubCategoryName(wxSQLite3Database* db, int categID, int subcategID)
{
    wxString name = wxT("");
    mmBEGINSQL_LITE_EXCEPTION
    wxSQLite3StatementBuffer bufSQL;
    bufSQL.Format("select * from SUBCATEGORY_V1 where CATEGID=%d and SUBCATEGID=%d;", categID, subcategID);
    wxSQLite3ResultSet q1 = db->ExecuteQuery(bufSQL);
    if (q1.NextRow())
    {
        name = q1.GetString(wxT("SUBCATEGNAME"));
    }
    q1.Finalize();
    mmENDSQL_LITE_EXCEPTION
    return name;
}

bool mmDBWrapper::deleteTransaction(wxSQLite3Database* db, int transID)
{
    mmBEGINSQL_LITE_EXCEPTION;
    wxSQLite3StatementBuffer bufSQL;
    bufSQL.Format("delete from CHECKINGACCOUNT_V1 where TRANSID=%d;", transID);
    db->ExecuteUpdate(bufSQL);
    mmENDSQL_LITE_EXCEPTION;
    return true;
}


bool mmDBWrapper::deleteFlaggedTransactions(wxSQLite3Database* db, int accountID)
{
    mmBEGINSQL_LITE_EXCEPTION;
    wxString bufSQL = wxString::Format(wxT("delete from CHECKINGACCOUNT_V1 where (ACCOUNTID=%d OR TOACCOUNTID=%d) AND (STATUS='F');"), accountID, accountID);
    db->ExecuteUpdate(bufSQL);
    mmENDSQL_LITE_EXCEPTION;
    return true;
}

bool mmDBWrapper::updatePayee(wxSQLite3Database* db, const wxString& payeeName, 
                              int payeeID, int categID, int subcategID)
{
    mmBEGINSQL_LITE_EXCEPTION;
    wxString bufSQL = wxString::Format(wxT("update PAYEE_V1 SET PAYEENAME='%s', CATEGID=%d, SUBCATEGID=%d WHERE PAYEEID=%d;"), payeeName.c_str(), categID, subcategID, payeeID);
    db->ExecuteUpdate(bufSQL);
    mmENDSQL_LITE_EXCEPTION;
    return true;

}

bool mmDBWrapper::deletePayeeWithConstraints(wxSQLite3Database* db, int payeeID)
{
    mmBEGINSQL_LITE_EXCEPTION;
    wxSQLite3StatementBuffer bufSQL;
    bufSQL.Format("select * from CHECKINGACCOUNT_V1 where PAYEEID=%d;", payeeID);
    wxSQLite3ResultSet q1 = db->ExecuteQuery(bufSQL);
    if (q1.NextRow())
    {
        /* Records exist */
        q1.Finalize();
        return false;
    }
    q1.Finalize();

    wxSQLite3StatementBuffer bufSQL1;
    bufSQL1.Format("delete from PAYEE_V1 where PAYEEID=%d;", payeeID);
    db->ExecuteUpdate(bufSQL1);
    mmENDSQL_LITE_EXCEPTION;
    return true;
}

wxString mmDBWrapper::getPayee(wxSQLite3Database* db, int payeeID, int& categID, int& subcategID )
{
    wxString payeeName = wxT("");
    mmBEGINSQL_LITE_EXCEPTION;
    wxSQLite3StatementBuffer bufSQL;
    bufSQL.Format("select * from PAYEE_V1 where PAYEEID=%d;", payeeID);
    wxSQLite3ResultSet q1 = db->ExecuteQuery(bufSQL);
    if (q1.NextRow())
    {
        payeeName = q1.GetString(wxT("PAYEENAME"));
        categID = q1.GetInt(wxT("CATEGID"));
        subcategID = q1.GetInt(wxT("SUBCATEGID"));
    }
    q1.Finalize();
    mmENDSQL_LITE_EXCEPTION;
    return payeeName;

}

int mmDBWrapper::getCurrencyID(wxSQLite3Database* db, const wxString& currencyName)
{
    int currencyID = -1;
    mmBEGINSQL_LITE_EXCEPTION;
    wxString bufSQL = wxString::Format(wxT("select * from CURRENCYFORMATS_V1 where CURRENCYNAME='%s';"), 
        mmCleanString(currencyName).c_str());
    wxSQLite3ResultSet q1 = db->ExecuteQuery(bufSQL);
    if (q1.NextRow())
    {
        currencyID = q1.GetInt(wxT("CURRENCYID"));
    }
    q1.Finalize();
    mmENDSQL_LITE_EXCEPTION;
    return currencyID;
}

wxString mmDBWrapper::getCurrencyName(wxSQLite3Database* db, int currencyID)
{
    mmBEGINSQL_LITE_EXCEPTION;
    wxString bufSQL = wxString::Format(wxT("select * from CURRENCYFORMATS_V1 where CURRENCYID='%d';"), 
        currencyID);
    wxSQLite3ResultSet q1 = db->ExecuteQuery(bufSQL);
    if (q1.NextRow())
    {
        return q1.GetString(wxT("CURRENCYNAME"));
    }
    q1.Finalize();
    mmENDSQL_LITE_EXCEPTION;
    return wxT("");
}

double mmDBWrapper::getCurrencyBaseConvRateForId(wxSQLite3Database* db, int currencyID)
{
    mmBEGINSQL_LITE_EXCEPTION;
    wxString bufSQL = wxString::Format(wxT("select * from CURRENCYFORMATS_V1 where CURRENCYID='%d';"), 
        currencyID);
    wxSQLite3ResultSet q1 = db->ExecuteQuery(bufSQL);
    if (q1.NextRow())
    {
        return q1.GetDouble(wxT("BASECONVRATE"));
    }
    q1.Finalize();
    mmENDSQL_LITE_EXCEPTION;
    return 1.0;
}

double mmDBWrapper::getCurrencyBaseConvRate(wxSQLite3Database* db, int accountID)
{
    double rate = 1.0;
    mmBEGINSQL_LITE_EXCEPTION;
    wxSQLite3StatementBuffer bufSQL;
    bufSQL.Format("select * from ACCOUNTLIST_V1 where ACCOUNTID=%d;", accountID);
    wxSQLite3ResultSet q1 = db->ExecuteQuery(bufSQL);
    if (q1.NextRow())
    {
        wxSQLite3StatementBuffer bufSQL1;
        bufSQL1.Format("select * from CURRENCYFORMATS_V1 where CURRENCYID=%d;", 
            q1.GetInt(wxT("CURRENCYID")));
        wxSQLite3ResultSet q2 = db->ExecuteQuery(bufSQL1);
        if (q2.NextRow())
        {
            rate = q2.GetDouble(wxT("BASECONVRATE"), 1.0);
        }
        q2.Finalize();

        q1.Finalize();
    }
    else
    {
        /* cannot find accountid */
        wxASSERT(true);
    }
    mmENDSQL_LITE_EXCEPTION;
    return rate;

}


//--------------------------------------------------------------------
void mmDBWrapper::verifyINIDB(wxSQLite3Database* inidb)
{
    mmBEGINSQL_LITE_EXCEPTION;

    bool exists = inidb->TableExists(wxT("SETTING_V1"));
    if (!exists)
    {
        /* Create INFOTABLE_V1 Table */
        inidb->ExecuteUpdate(wxT("create table SETTING_V1(SETTINGID integer primary key, \
                              SETTINGNAME TEXT NOT NULL, SETTINGVALUE TEXT);"));
        bool valid = inidb->TableExists(wxT("SETTING_V1"));
        wxASSERT(valid);
    }
    mmENDSQL_LITE_EXCEPTION;
}

wxString mmDBWrapper::getINISettingValue(wxSQLite3Database* db, const wxString& settingName, 
                                            const wxString& defaultVal)
{
    wxString value = defaultVal;
    mmBEGINSQL_LITE_EXCEPTION;
    wxString bufSQL = wxString::Format(wxT("select * from SETTING_V1 where SETTINGNAME='%s';"), settingName.c_str());
    wxSQLite3ResultSet q1 = db->ExecuteQuery(bufSQL);
    if (q1.NextRow())
    {
        value = q1.GetString(wxT("SETTINGVALUE"));
    }
    q1.Finalize();
    mmENDSQL_LITE_EXCEPTION;
    return value;
}

bool mmDBWrapper::doesINISettingNameExist(wxSQLite3Database* db, const wxString& settingName)
{
    mmBEGINSQL_LITE_EXCEPTION;
    wxString bufSQL = wxString::Format(wxT("select * from SETTING_V1 where SETTINGNAME='%s';"), settingName.c_str());
    wxSQLite3ResultSet q1 = db->ExecuteQuery(bufSQL);
    if (q1.NextRow())
    {
        return true;
    }
    q1.Finalize();
    mmENDSQL_LITE_EXCEPTION;
    return false;
}

void mmDBWrapper::setINISettingValue(wxSQLite3Database* db, const wxString& settingName, 
                                     const wxString& settingValue)
{
    if (!doesINISettingNameExist(db, settingName))
    {
        mmBEGINSQL_LITE_EXCEPTION;
        wxString bufSQL = wxString::Format(wxT("insert into SETTING_V1 (SETTINGNAME, SETTINGVALUE) values ('%s', '%s');"), settingName.c_str(), settingValue.c_str());
        int retVal = db->ExecuteUpdate(bufSQL);
        mmENDSQL_LITE_EXCEPTION;
        return;
    }

    mmBEGINSQL_LITE_EXCEPTION;
    wxString bufSQL = wxString::Format(wxT("update SETTING_V1 set SETTINGVALUE='%s' where SETTINGNAME='%s';"), settingValue.c_str(), settingName.c_str());
    int retVal = db->ExecuteUpdate(bufSQL);
    mmENDSQL_LITE_EXCEPTION;
}

void mmDBWrapper::updateTransactionWithStatus(wxSQLite3Database* db, int transID, 
                                              const wxString& status)
{
    mmBEGINSQL_LITE_EXCEPTION;
    wxString bufSQL = wxString::Format(wxT("update CHECKINGACCOUNT_V1 set STATUS='%s' where TRANSID=%d;"), status.c_str(), transID);
    int res = db->ExecuteUpdate(bufSQL);
    mmENDSQL_LITE_EXCEPTION;
}

wxString mmDBWrapper::getInfoSettingValue(wxSQLite3Database* db, const wxString& settingName, 
                                            const wxString& defaultVal)
{
    wxString value = defaultVal;
    mmBEGINSQL_LITE_EXCEPTION;
    wxString bufSQL = wxString::Format(wxT("select * from INFOTABLE_V1 where INFONAME='%s';"), settingName.c_str());
    wxSQLite3ResultSet q1 = db->ExecuteQuery(bufSQL);
    if (q1.NextRow())
    {
        value = q1.GetString(wxT("INFOVALUE"));
    }
    q1.Finalize();
    mmENDSQL_LITE_EXCEPTION;
    return value;
}

bool mmDBWrapper::doesInfoSettingNameExist(wxSQLite3Database* db, const wxString& settingName)
{
    mmBEGINSQL_LITE_EXCEPTION;
    wxString bufSQL = wxString::Format(wxT("select * from INFOTABLE_V1 where INFONAME='%s';"), settingName.c_str());
    wxSQLite3ResultSet q1 = db->ExecuteQuery(bufSQL);
    if (q1.NextRow())
    {
        return true;
    }
    q1.Finalize();
    mmENDSQL_LITE_EXCEPTION;
    return false;
}

void mmDBWrapper::setInfoSettingValue(wxSQLite3Database* db, const wxString& settingName, 
                                     const wxString& settingValue)
{
    if (!doesInfoSettingNameExist(db, settingName))
    {
        mmBEGINSQL_LITE_EXCEPTION;
        wxString bufSQL = wxString::Format(wxT("insert into INFOTABLE_V1 (INFONAME, INFOVALUE) values ('%s', '%s');"), 
            settingName.c_str(), settingValue.c_str());
        int retVal = db->ExecuteUpdate(bufSQL);
        mmENDSQL_LITE_EXCEPTION;
        return;
    }

    mmBEGINSQL_LITE_EXCEPTION;
    wxString bufSQL = wxString::Format(wxT("update INFOTABLE_V1 set INFOVALUE='%s' where INFONAME='%s';"), 
        settingValue.c_str(), settingName.c_str());
    int retVal = db->ExecuteUpdate(bufSQL);
    mmENDSQL_LITE_EXCEPTION;
}

double mmDBWrapper::getAmountForCategory(wxSQLite3Database* db, 
                                         int categID, 
                                         int subcategID,
                                         bool ignoreDate,
                                         wxDateTime dtBegin,
                                         wxDateTime dtEnd)
{
    double amt = 0.0;
    mmBEGINSQL_LITE_EXCEPTION;
    wxString bufSQL;
#if 0
    if (subcategID == -1)
    {
        bufSQL = wxString::Format(wxT("select * from CHECKINGACCOUNT_V1 where CATEGID=%d;"), 
            categID);
    }
    else
#endif
    {
        bufSQL = wxString::Format(wxT("select * from CHECKINGACCOUNT_V1 where CATEGID=%d AND SUBCATEGID=%d;"), 
            categID, subcategID);

    }
    wxSQLite3ResultSet q1 = db->ExecuteQuery(bufSQL);
    while (q1.NextRow())
    {
        wxString code = q1.GetString(wxT("TRANSCODE"));
        double transAmount = q1.GetDouble(wxT("TRANSAMOUNT"));
        wxString transStatus = q1.GetString(wxT("STATUS"));
        wxString dateString = q1.GetString(wxT("TRANSDATE"));
        wxDateTime dtdt = mmGetStorageStringAsDate(dateString);

        if (transStatus == wxT("V"))
           continue; // skip

        if (!ignoreDate)
        {
            if (!dtdt.IsBetween(dtBegin, dtEnd))
                continue; //skip
        }

        if (code == wxT("Transfer"))
            continue;

        if (code == wxT("Withdrawal"))
            amt = amt - transAmount;
        else if (code == wxT("Deposit"))
            amt = amt + transAmount;
    }
    q1.Finalize();

    mmENDSQL_LITE_EXCEPTION;
    return amt;
}   

double mmDBWrapper::getAmountForPayee(wxSQLite3Database* db, int payeeID,
        bool ignoreDate, wxDateTime dtBegin, wxDateTime dtEnd)
{
    double amt = 0.0;
    mmBEGINSQL_LITE_EXCEPTION;
    wxString bufSQL;
    bufSQL = wxString::Format(wxT("select * from CHECKINGACCOUNT_V1 where PAYEEID=%d;"), 
            payeeID);
    wxSQLite3ResultSet q1 = db->ExecuteQuery(bufSQL);
    while (q1.NextRow())
    {
        wxString code = q1.GetString(wxT("TRANSCODE"));
        double transAmount = q1.GetDouble(wxT("TRANSAMOUNT"));
        wxString transStatus = q1.GetString(wxT("STATUS"));
        wxString dateString = q1.GetString(wxT("TRANSDATE"));
        wxDateTime dtdt = mmGetStorageStringAsDate(dateString);

        if (transStatus == wxT("V"))
           continue; // skip

        if (!ignoreDate)
        {
            if (!dtdt.IsBetween(dtBegin, dtEnd))
                continue; //skip
        }

        if (code == wxT("Transfer"))
            continue;

        if (code == wxT("Withdrawal"))
            amt = amt - transAmount;
        else if (code == wxT("Deposit"))
            amt = amt + transAmount;
    }
    q1.Finalize();

    mmENDSQL_LITE_EXCEPTION;
    return amt;
}


 void mmDBWrapper::addBudgetYear(wxSQLite3Database* db, wxString year)
 {
    mmBEGINSQL_LITE_EXCEPTION;
    wxString bufSQL = wxString::Format(wxT("insert into BUDGETYEAR_V1 (BUDGETYEARNAME) values ('%s');"), 
        year.c_str());
    int retVal = db->ExecuteUpdate(bufSQL);
    mmENDSQL_LITE_EXCEPTION;
 }

 int mmDBWrapper::getBudgetYearID(wxSQLite3Database* db, wxString year)
 {
    int budgetYearID = -1;
    mmBEGINSQL_LITE_EXCEPTION;
    wxString bufSQL = wxString::Format(wxT("select * from BUDGETYEAR_V1 where BUDGETYEARNAME='%s';"), 
        year.c_str());
    wxSQLite3ResultSet q1 = db->ExecuteQuery(bufSQL);
    if (q1.NextRow())
    {
        budgetYearID = q1.GetInt(wxT("BUDGETYEARID"));
    }
    q1.Finalize();
    mmENDSQL_LITE_EXCEPTION;
    return budgetYearID;
 }

 wxString mmDBWrapper::getBudgetYearForID(wxSQLite3Database* db, int yearid)
 {
     wxString budgetYear = wxT("");
     mmBEGINSQL_LITE_EXCEPTION;
     wxString bufSQL = wxString::Format(wxT("select * from BUDGETYEAR_V1 where BUDGETYEARID=%d;"), 
         yearid);
     wxSQLite3ResultSet q1 = db->ExecuteQuery(bufSQL);
     if (q1.NextRow())
     {
         budgetYear = q1.GetString(wxT("BUDGETYEARNAME"));
     }
     q1.Finalize();
     mmENDSQL_LITE_EXCEPTION;
     return budgetYear;
 }

 void mmDBWrapper::updateYearForID(wxSQLite3Database* db, 
     const wxString& yearName, int yearid)
 {
     mmBEGINSQL_LITE_EXCEPTION;
     wxString bufSQL = wxString::Format(wxT("update BUDGETYEAR_V1 SET BUDGETYEARNAME='%s' WHERE BUDGETYEARID=%d;"), 
         yearName, yearid);
     db->ExecuteUpdate(bufSQL);
     mmENDSQL_LITE_EXCEPTION;
 }

 void mmDBWrapper::copyBudgetYear(wxSQLite3Database* db, int newyear, int baseYear)
 {
     vector<bYearEntry> entries;
     mmBEGINSQL_LITE_EXCEPTION;
     wxString bufSQL = wxString::Format(wxT("select * from BUDGETTABLE_V1 WHERE BUDGETYEARID=%d;"), 
         baseYear);
     wxSQLite3ResultSet q1 = db->ExecuteQuery(bufSQL);
     while (q1.NextRow())
     {
         struct bYearEntry ye;
         ye.categID = q1.GetInt(wxT("CATEGID"));
         ye.subcategID = q1.GetInt(wxT("SUBCATEGID"));
         ye.period = q1.GetString(wxT("PERIOD"));
         ye.amt = q1.GetDouble(wxT("AMOUNT"));
         entries.push_back(ye);
     }
     q1.Finalize();

     for (int idx = 0; idx < (int)entries.size(); idx++)
     {
         mmDBWrapper::addBudgetEntry(db, newyear, 
             entries[idx].categID, entries[idx].subcategID, entries[idx].period, entries[idx].amt);
     }

     mmENDSQL_LITE_EXCEPTION;
 }

 void mmDBWrapper::deleteBudgetYear(wxSQLite3Database* db, const wxString& yearName)
 {
    int budgetYearID = getBudgetYearID(db, yearName);
    if (budgetYearID == -1)
        return;
    mmBEGINSQL_LITE_EXCEPTION;
    wxSQLite3StatementBuffer bufSQL;
    bufSQL.Format("delete from BUDGETTABLE_V1 where BUDGETYEARID=%d;", budgetYearID);
    db->ExecuteUpdate(bufSQL);

    wxSQLite3StatementBuffer bufSQL1;
    bufSQL1.Format("delete from BUDGETYEAR_V1 where BUDGETYEARID=%d;", budgetYearID);
    db->ExecuteUpdate(bufSQL1);
    mmENDSQL_LITE_EXCEPTION;
 }

 void mmDBWrapper::addBudgetEntry(wxSQLite3Database* db, int budgetYearID, 
     int categID, int subCategID, const wxString& period, double amount)
 {
    mmBEGINSQL_LITE_EXCEPTION;
    wxString bufSQL = wxString::Format(wxT("insert into BUDGETTABLE_V1 (BUDGETYEARID, CATEGID, \
        SUBCATEGID, PERIOD, AMOUNT) values (%d, %d, %d, '%s', %f);"), 
        budgetYearID, categID, subCategID, period.c_str(), amount);
    int retVal = db->ExecuteUpdate(bufSQL);
    mmENDSQL_LITE_EXCEPTION;
 }

 void mmDBWrapper::deleteBudgetEntry(wxSQLite3Database* db, int budgetEntryID)
 {
    mmBEGINSQL_LITE_EXCEPTION;
    wxSQLite3StatementBuffer bufSQL1;
    bufSQL1.Format("delete from BUDGETTABLE_V1 where BUDGETENTRYID=%d;", budgetEntryID);
    db->ExecuteUpdate(bufSQL1);
    mmENDSQL_LITE_EXCEPTION;
 }
 
 void mmDBWrapper::updateBudgetEntry(wxSQLite3Database* db, 
        int budgetYearID, int categID, int subCategID,
                                const wxString& period, double amount)
 {
     mmBEGINSQL_LITE_EXCEPTION;

     wxString bufSQL = wxString::Format(wxT("select * from BUDGETTABLE_V1 WHERE BUDGETYEARID=%d AND CATEGID=%d AND SUBCATEGID=%d;"), 
         budgetYearID, categID, subCategID);

     wxSQLite3ResultSet q1 = db->ExecuteQuery(bufSQL);
     if (q1.NextRow())
     {
         int budgetEntryID = q1.GetInt(wxT("BUDGETENTRYID"));
         q1.Finalize();
         mmDBWrapper::deleteBudgetEntry(db, budgetEntryID);
     }
     

    addBudgetEntry(db, budgetYearID, categID, subCategID, period, amount);

     mmENDSQL_LITE_EXCEPTION;
 }

 bool mmDBWrapper::getBudgetEntry(wxSQLite3Database* db, int budgetYearID, 
                                int categID, int subCategID, wxString& period, double& amount)
 {
      bool isPresent = false;
     mmBEGINSQL_LITE_EXCEPTION;
     wxString bufSQL = wxString::Format(wxT("select * from BUDGETTABLE_V1 WHERE BUDGETYEARID=%d AND CATEGID=%d AND SUBCATEGID=%d;"), 
                                            budgetYearID, categID, subCategID);

     wxSQLite3ResultSet q1 = db->ExecuteQuery(bufSQL);
    
     if (q1.NextRow())
     {
        period = q1.GetString(wxT("PERIOD"), wxT("None"));
        amount = q1.GetDouble(wxT("AMOUNT"));
        isPresent = true;
     }
     q1.Finalize();

     mmENDSQL_LITE_EXCEPTION;
    return isPresent;
 }

 void mmDBWrapper::deleteBDSeries(wxSQLite3Database* db, int bdID)
 {
     mmBEGINSQL_LITE_EXCEPTION;
     wxSQLite3StatementBuffer bufSQL;
     bufSQL.Format("delete from BILLSDEPOSITS_V1 where BDID=%d;", bdID);
     db->ExecuteUpdate(bufSQL);
     mmENDSQL_LITE_EXCEPTION;
 }

 void mmDBWrapper::completeBDInSeries(wxSQLite3Database* db, int bdID)
 {
     mmBEGINSQL_LITE_EXCEPTION;
     wxString bufSQLStr = wxString::Format(wxT("select * from BILLSDEPOSITS_V1 WHERE BDID=%d"), 
         bdID);
    
     wxSQLite3ResultSet q1 = db->ExecuteQuery(bufSQLStr);
     wxDateTime updateOccur = wxDateTime::Now();
     long numRepeats  = -1;

     if (q1.NextRow())
     {
        numRepeats = q1.GetInt(wxT("NUMOCCURRENCES"));
        if (numRepeats != -1)
        {
            numRepeats -= 1;
        }

        wxString nextOccurrString = q1.GetString(wxT("NEXTOCCURRENCEDATE"));
        wxDateTime dtno = mmGetStorageStringAsDate(nextOccurrString);

        int repeats = q1.GetInt(wxT("REPEATS"));
        updateOccur = dtno;
        
        if (repeats == 0)
        {
            numRepeats = 0;
        }
        else if (repeats == 1)
        {
            updateOccur = dtno.Add(wxTimeSpan::Week());
        }
        else if (repeats == 2)
        {
            updateOccur = dtno.Add(wxTimeSpan::Weeks(2));
        }
        else if (repeats == 3)
        {
            updateOccur = dtno.Add(wxDateSpan::Month());
        }
        else if (repeats == 4)
        {
            updateOccur = dtno.Add(wxDateSpan::Months(2));
        }
        else if (repeats == 5)
        {
            updateOccur = dtno.Add(wxDateSpan::Months(4));
        }
        else if (repeats == 6)
        {
            updateOccur = dtno.Add(wxDateSpan::Months(6));
        }
        else if (repeats == 7)
        {
            updateOccur = dtno.Add(wxDateSpan::Year());
        }
     }
     q1.Finalize();
     
     wxString updateStr = updateOccur.FormatISODate();

     bufSQLStr = wxString::Format(wxT("update BILLSDEPOSITS_V1 set NEXTOCCURRENCEDATE='%s', NUMOCCURRENCES=%d where BDID=%d;"), 
         updateStr.c_str(), numRepeats, bdID);
     int retVal = db->ExecuteUpdate(bufSQLStr);

     long delCount = 0;
     wxSQLite3StatementBuffer bufSQL;
     bufSQL.Format("delete from BILLSDEPOSITS_V1 where NUMOCCURRENCES=%d;", delCount);
     db->ExecuteUpdate(bufSQL);

     mmENDSQL_LITE_EXCEPTION;
 }

void mmDBWrapper::deleteStockInvestment(wxSQLite3Database* db, int stockID)
{
    mmBEGINSQL_LITE_EXCEPTION;
    wxSQLite3StatementBuffer bufSQL;
    bufSQL.Format("delete from STOCK_V1 where STOCKID=%d;", stockID);
    db->ExecuteUpdate(bufSQL);
    mmENDSQL_LITE_EXCEPTION; 
}

double mmDBWrapper::getStockInvestmentBalance(wxSQLite3Database* db)
{
    double balance = 0.0;
    mmBEGINSQL_LITE_EXCEPTION;
    wxString bufSQL = wxString::Format(wxT("select * from STOCK_V1;"));
    wxSQLite3ResultSet q1 = db->ExecuteQuery(bufSQL);
    if (q1.NextRow())
    {
        double value = q1.GetDouble(wxT("VALUE"));
        balance += value;
    }
    q1.Finalize();
    mmENDSQL_LITE_EXCEPTION;
    return balance;
}