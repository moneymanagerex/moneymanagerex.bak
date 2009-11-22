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

#include "dbwrapper.h"
#include "wx/datetime.h"
#include "util.h"
#include "guiid.h"
#include "paths.h"
#include "constants.h"
//----------------------------------------------------------------------------
#include <boost/scoped_ptr.hpp>
#include <string>
//----------------------------------------------------------------------------

namespace
{

const double g_defBASECONVRATE = 1.0;
const char g_BaseCurrencyIdName[] = "BASECURRENCYID";
//----------------------------------------------------------------------------

/*
    Appends "ACCOUNTLIST acl" & "CURRENCYFORMATS <table_alias>" tables to query.
*/
std::string joinCURRENCYFORMATS(const char *table_alias, const char *col_accountid)
{
    wxASSERT(col_accountid);

    std::string sql = " join ACCOUNTLIST_V1 acl "
                       "on acl.ACCOUNTID = ";

    sql += col_accountid;

    sql += " join CURRENCYFORMATS_V1 ";
    sql += table_alias;
    sql +=  " on cf.CURRENCYID = acl.CURRENCYID ";

    return sql;
}
//----------------------------------------------------------------------------

void loadCurrencies(wxSQLite3Database* db)
{
    static const char sql[] = 
    "select CURRENCYNAME, "
           "PFX_SYMBOL, "
           "SFX_SYMBOL, "
           "DECIMAL_POINT, "
           "GROUP_SEPARATOR, "
           "UNIT_NAME, "
           "CENT_NAME, "
           "SCALE, "
           "BASECONVRATE " // CURRENCY_SYMBOL not exists in this table
    "from CURRENCYFORMATS_V1";

    static const char sql2[] = 
    "insert into CURRENCYFORMATS_V1 ("
      "CURRENCYNAME, PFX_SYMBOL, SFX_SYMBOL, DECIMAL_POINT, GROUP_SEPARATOR, "
      "UNIT_NAME, CENT_NAME, SCALE, BASECONVRATE, CURRENCY_SYMBOL "
    " ) values ( ?, ?, ?, ?, ?, ?, ?, ?, ?, '' )";

    wxString fName = mmex::getPathShared(mmex::CURRENCY_DB_SEED);
    wxASSERT(wxFileName::FileExists(fName));
    
    wxSQLite3Database currencies;
    currencies.Open(fName);

    {
        bool ok = currencies.TableExists(wxT("CURRENCYFORMATS_V1"));
        wxASSERT(ok);
        ok = ok; // removes compiler's warning
    }

    wxSQLite3ResultSet rs = currencies.ExecuteQuery(sql);
    wxSQLite3Statement st = db->PrepareStatement(sql2);
    
    while(rs.NextRow())
    {
        int i = 0;
        st.Bind(++i, rs.GetString(wxT("CURRENCYNAME")));
        st.Bind(++i, rs.GetString(wxT("PFX_SYMBOL")));
        st.Bind(++i, rs.GetString(wxT("SFX_SYMBOL")));
        st.Bind(++i, rs.GetString(wxT("DECIMAL_POINT")));
        st.Bind(++i, rs.GetString(wxT("GROUP_SEPARATOR")));
        st.Bind(++i, rs.GetString(wxT("UNIT_NAME")));
        st.Bind(++i, rs.GetString(wxT("CENT_NAME")));
        st.Bind(++i, rs.GetDouble(wxT("SCALE")));
        st.Bind(++i, rs.GetDouble(wxT("BASECONVRATE"), g_defBASECONVRATE));

        wxASSERT(st.GetParamCount() == i);
        
        st.ExecuteUpdate();
        st.Reset();
    }

    st.Finalize();
    rs.Finalize();

    currencies.Close();
}
//----------------------------------------------------------------------------

void
insertCategoryTree(wxSQLite3Database* db,
                   wxSQLite3Statement &cat,
                   wxSQLite3Statement &subcat,
                   const wxString &categoryName,
                   const wxChar* subcats[] // must be NULL or ends with NULL
                  )
{
    wxASSERT(db);

    bool ok = cat.GetParamCount() == 1;
    wxASSERT(ok);

    cat.Bind(1, categoryName);
    cat.ExecuteUpdate();
    cat.Reset();

    if (!subcats)
        return;

    wxLongLong catId = db->GetLastRowId(); // id of newly inserted categoryName

    ok = subcat.GetParamCount() == 2;
    wxASSERT(ok);
    
    int name_idx = subcat.GetParamIndex(wxT(":name"));
    wxASSERT(name_idx);

    int id_idx = subcat.GetParamIndex(wxT(":id"));
    wxASSERT(id_idx);

    subcat.Bind(id_idx, catId);

    for (size_t i = 0; subcats[i]; ++i)
    {
        subcat.Bind(name_idx, subcats[i]);
        subcat.ExecuteUpdate();
        subcat.Reset();

        wxASSERT(i < 50); // subcats must ends with 0
    }
}
//----------------------------------------------------------------------------

void createDefaultCategories(wxSQLite3Database* db)
{
    wxASSERT(db);

    wxSQLite3Statement st_cat = db->PrepareStatement("insert into CATEGORY_V1 (CATEGNAME) VALUES(?)");

    wxSQLite3Statement st_subcat = db->PrepareStatement("insert into SUBCATEGORY_V1 "
      "( SUBCATEGNAME, CATEGID ) "
      "values (:name, :id)"
      );

    const wxChar* BillsCategories[] = {
      _("Telephone"),
      _("Electricity"),
      _("Gas"),
      _("Internet"),
      _("Rent"),
      _("Cable TV"),
      _("Water"),
      0
    };

    insertCategoryTree(db, st_cat, st_subcat, _("Bills"), BillsCategories);

    // --

    const wxChar* FoodCategories[] = {
      _("Groceries"),
      _("Dining out"),
      0
    };

    insertCategoryTree(db, st_cat, st_subcat, _("Food"), FoodCategories);

    // --

    const wxChar* LeisureCategories[] = {
      _("Movies"),
      _("Video Rental"),
      _("Magazines"),
      0
    };

    insertCategoryTree(db, st_cat, st_subcat, _("Leisure"), LeisureCategories);

    // --

    const wxChar* AutomobileCategories[] = {
      _("Maintenance"),
      _("Gas"),
      _("Parking"),
      _("Registration"),
      0
    };

    insertCategoryTree(db, st_cat, st_subcat, _("Automobile"), AutomobileCategories);

    // --

    const wxChar* EducationCategories[] = {
      _("Books"),
      _("Tuition"),
      _("Others"),
      0
    };

    insertCategoryTree(db, st_cat, st_subcat, _("Education"), EducationCategories);

    // --

    const wxChar* HomeneedsCategories[] = {
      _("Clothing"),
      _("Furnishing"),
      _("Others"),
      0
    };

    insertCategoryTree(db, st_cat, st_subcat, _("Homeneeds"), HomeneedsCategories);

    // --

    const wxChar* HealthcareCategories[] = {
      _("Health"),
      _("Dental"),
      _("Eyecare"),
      _("Physician"),
      _("Prescriptions"),
      0
    };

    insertCategoryTree(db, st_cat, st_subcat, _("Healthcare"), HealthcareCategories);

    // --

    const wxChar* InsuranceCategories[] = {
      _("Auto"),
      _("Life"),
      _("Home"),
      _("Health"),
      0
    };

    insertCategoryTree(db, st_cat, st_subcat, _("Insurance"), InsuranceCategories);

    // --

    const wxChar* VacationCategories[] = {
      _("Travel"),
      _("Lodging"),
      _("Sightseeing"),
      0
    };

    insertCategoryTree(db, st_cat, st_subcat, _("Vacation"), VacationCategories);

    // --

    const wxChar* TaxesCategories[] = {
      _("Income Tax"),
      _("House Tax"),
      _("Water Tax"),
      _("Others"),
      0
    };

    insertCategoryTree(db, st_cat, st_subcat, _("Taxes"), TaxesCategories);

    // --

    insertCategoryTree(db, st_cat, st_subcat, _("Miscellaneous"), 0);
    insertCategoryTree(db, st_cat, st_subcat, _("Gifts"), 0);

    // --

    const wxChar* IncomeCategories[] = {
      _("Salary"),
      _("Reimbursement/Refunds"),
      _("Investment Income"),
      0
    };

    insertCategoryTree(db, st_cat, st_subcat, _("Income"), IncomeCategories);

    // --

    insertCategoryTree(db, st_cat, st_subcat, _("Other Income"), 0);
    insertCategoryTree(db, st_cat, st_subcat, _("Other Expenses"), 0);
    insertCategoryTree(db, st_cat, st_subcat, _("Transfer"), 0);

    // cleanup

    st_subcat.Finalize();
    st_cat.Finalize();
}
//----------------------------------------------------------------------------

} // namespace

//----------------------------------------------------------------------------

void mmDBWrapper::createInfoV1Table(wxSQLite3Database* db)
{
    /* We always create new INFOTABLE_V1 */
    bool exists = db->TableExists(wxT("INFOTABLE_V1"));
    if (exists)
    {
        setInfoSettingValue(db, wxT("MODIFIEDDATE"), wxDateTime::Now().FormatISODate());
        setInfoSettingValue(db, wxT("DATAVERSION"), mmex::DATAVERSION);
    }
    else
    {
        static const char sql[] =
        "create table INFOTABLE_V1 "
        "( "
         "INFOID integer not null primary key, "
         "INFONAME TEXT NOT NULL, "
         "INFOVALUE TEXT NOT NULL "
        ")";

        db->ExecuteUpdate(sql);

        {
            bool ok = db->TableExists(wxT("INFOTABLE_V1"));
            wxASSERT(ok);
            ok = ok; // removes compiler's warning
        }

        // --    

        struct Rec
        {
            const char *name;
            wxString value;
        };

        const Rec data[] = 
        {
            { "MMEXVERSION", mmex::getProgramVersion()},
            { "DATAVERSION", mmex::DATAVERSION },
            { "CREATEDATE", wxDateTime::Now().FormatISODate() },
            { "DATEFORMAT", mmex::DEFDATEFORMAT },
            { 0, wxGetEmptyString() }
        };

        static const char sql_ins[] = 
        "insert into INFOTABLE_V1 (INFONAME, INFOVALUE) values (?, ?)";

        wxSQLite3Statement st = db->PrepareStatement(sql_ins);

        for (int i = 0; data[i].name; ++i)
        {
            st.Bind(1, data[i].name);
            st.Bind(2, data[i].value);

            st.ExecuteUpdate();
            st.Reset();
        }

        st.Finalize();
    }
}

void mmDBWrapper::createCurrencyV1Table(wxSQLite3Database* db)
{
       bool exists = db->TableExists(wxT("CURRENCYFORMATS_V1"));

       if (exists) { 

           /* Check whether the column "CURRENCY_SYMBOL" exists or not */
           wxSQLite3ResultSet q1 = db->ExecuteQuery(wxT("select * from CURRENCYFORMATS_V1 limit 1"));
           
           if(q1.GetColumnCount() < 11) {
                /* not exist, create the column */
                db->ExecuteUpdate(wxT("alter table CURRENCYFORMATS_V1 add CURRENCY_SYMBOL TEXT"));
           }

       } else {

           db->ExecuteUpdate(wxT("create table CURRENCYFORMATS_V1(CURRENCYID integer primary key, \
                                CURRENCYNAME TEXT NOT NULL, PFX_SYMBOL TEXT, SFX_SYMBOL TEXT,              \
                                DECIMAL_POINT TEXT, GROUP_SEPARATOR TEXT,               \
                                UNIT_NAME TEXT, CENT_NAME TEXT, SCALE numeric, BASECONVRATE numeric, CURRENCY_SYMBOL TEXT);"));

           exists = db->TableExists(wxT("CURRENCYFORMATS_V1"));
           wxASSERT(exists);

           /* Load Default US Currency */

           static const char sql[] = 
            "insert into CURRENCYFORMATS_V1 ("
              "CURRENCYNAME, PFX_SYMBOL, SFX_SYMBOL, DECIMAL_POINT,GROUP_SEPARATOR, UNIT_NAME, "
              "CENT_NAME, SCALE, BASECONVRATE, CURRENCY_SYMBOL "
            ") values ("
              "'US DOLLAR', '$', '', '.', ',', 'dollar', 'cents', 100, 1.0, 'USD' "
            ")";
           
           db->ExecuteUpdate(sql);
           loadCurrencies(db);
       }
}


void mmDBWrapper::createBudgetingV1Table(wxSQLite3Database* db)
{
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
}

void mmDBWrapper::createStockV1Table(wxSQLite3Database* db)
{
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
}


void mmDBWrapper::createAssetsV1Table(wxSQLite3Database* db)
{
    bool valid = db->TableExists(wxT("ASSETS_V1"));
    if (!valid)
    {
        db->ExecuteUpdate(wxT("create table ASSETS_V1(ASSETID integer primary key, \
                             STARTDATE TEXT NOT NULL, ASSETNAME TEXT, \
                             VALUE numeric, VALUECHANGE TEXT, NOTES TEXT, VALUECHANGERATE numeric,\
                             ASSETTYPE TEXT);"));
        valid = db->TableExists(wxT("ASSETS_V1"));
        wxASSERT(valid);
    }
}

void mmDBWrapper::createBillsDepositsV1Table(wxSQLite3Database* db)
{
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
}

bool mmDBWrapper::checkDBVersion(wxSQLite3Database* db)
{
    bool ok = false;

    bool valid = db->TableExists(wxT("INFOTABLE_V1"));
    if (!valid)
        return false;

    wxSQLite3ResultSet q1 = db->ExecuteQuery("select INFOVALUE "
                                             "from INFOTABLE_V1 "
                                             "where INFONAME = 'DATAVERSION'");

    if (q1.NextRow())
    {
        int dataVersion = q1.GetInt(wxT("INFOVALUE"));
        ok = dataVersion >= mmex::MIN_DATAVERSION;
    }
    q1.Finalize();

    return ok;
}

void mmDBWrapper::createAccountListV1Table(wxSQLite3Database* db)
{
    bool exists = db->TableExists(wxT("ACCOUNTLIST_V1"));
    if (!exists)
    {
        db->ExecuteUpdate(wxT("create table ACCOUNTLIST_V1(ACCOUNTID integer primary key, \
                              ACCOUNTNAME TEXT NOT NULL, ACCOUNTTYPE TEXT NOT NULL, ACCOUNTNUM TEXT, \
                              STATUS TEXT NOT NULL, \
                              NOTES TEXT, HELDAT TEXT, WEBSITE TEXT, CONTACTINFO TEXT,       \
                              ACCESSINFO TEXT, INITIALBAL numeric, FAVORITEACCT TEXT NOT NULL, \
                              CURRENCYID numeric NOT NULL);"));
        exists = db->TableExists(wxT("ACCOUNTLIST_V1"));
        wxASSERT(exists);
    }
    else
    {
#if 0
        // if we add new fields, we need to alter table as follows
            int ret = db->ExecuteUpdate(wxT("alter table ACCOUNTLIST_V1 ADD INFOID TEXT;"));
#endif
    }
}

void mmDBWrapper::createCheckingAccountV1Table(wxSQLite3Database* db)
{
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
}

void mmDBWrapper::createSplitTransactionsV1Table(wxSQLite3Database* db)
{
    bool exists = db->TableExists(wxT("SPLITTRANSACTIONS_V1"));
    if (!exists)
    {
        db->ExecuteUpdate(wxT("create table SPLITTRANSACTIONS_V1(SPLITTRANSID integer primary key, \
                              TRANSID numeric NOT NULL, CATEGID numeric, SUBCATEGID numeric, SPLITTRANSAMOUNT numeric);"));
        exists = db->TableExists(wxT("SPLITTRANSACTIONS_V1"));
        wxASSERT(exists);
    }

    exists = db->TableExists(wxT("BUDGETSPLITTRANSACTIONS_V1"));
    if (!exists)
    {
        db->ExecuteUpdate(wxT("create table BUDGETSPLITTRANSACTIONS_V1(SPLITTRANSID integer primary key, \
                              TRANSID numeric NOT NULL, CATEGID numeric, SUBCATEGID numeric, SPLITTRANSAMOUNT numeric);"));
        exists = db->TableExists(wxT("BUDGETSPLITTRANSACTIONS_V1"));
        wxASSERT(exists);
    }
}

void mmDBWrapper::createPayeeV1Table(wxSQLite3Database* db)
{
    bool exists = db->TableExists(wxT("PAYEE_V1"));
    if (!exists)
    {
        db->ExecuteUpdate(wxT("create table PAYEE_V1(PAYEEID integer primary key, \
                             PAYEENAME TEXT NOT NULL, CATEGID numeric, SUBCATEGID numeric);"));
        exists = db->TableExists(wxT("PAYEE_V1"));
        wxASSERT(exists);
    }
}

void mmDBWrapper::createCategoryV1Table(wxSQLite3Database* db)
{
    bool existsCat = db->TableExists(wxT("CATEGORY_V1"));
    if (!existsCat)
    {
        /* Create CATEGORY_V1 Tables */
        db->ExecuteUpdate(wxT("create table CATEGORY_V1(CATEGID integer primary key, \
                             CATEGNAME TEXT NOT NULL);"));

        {
            bool ok = db->TableExists(wxT("CATEGORY_V1"));
            wxASSERT(ok);
            ok = ok; // removes compiler's warning
        }
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
        createDefaultCategories(db);
    }
}

/*
    wxSQLite3Database::ViewExists was removed.
*/
bool mmDBWrapper::ViewExists(wxSQLite3Database* db, const char *viewName)
{
    static const char sql[] = 
    "select 1 "
    "from sqlite_master "
    "where type = 'view' and "
          "name like ?";

    wxASSERT(viewName);

    bool exists = false;
    
    wxSQLite3Statement st = db->PrepareStatement(sql);
    st.Bind(1, viewName);

    wxSQLite3ResultSet rs = st.ExecuteQuery();
    exists = rs.NextRow();

    st.Finalize();
    
    return exists;
}

void mmDBWrapper::createAllDataView(wxSQLite3Database* db)
{
    static const char view_name[] = "ALLDATA";

    static const char sql[] = 
"create view alldata as "
"select CANS.TransID as ID, "
       "CANS.TransCode as TransactionType, "
       "CANS.TransDate as Date, "
       "round(strftime('%Y', CANS.TransDate)) as Year, "
       "round(strftime('%m', CANS.TransDate)) as Month, "
       "round(strftime('%d', CANS.TransDate)) as Day, "
       "CAT.CategName as Category, "
       "SUBCAT.SubCategName as Subcategory, "
       "ROUND(case CANS.TRANSCODE when 'Withdrawal' then -1*CANS.TRANSAMOUNT  else CANS.TRANSAMOUNT end,2) as Amount, "

       "( select cf.BaseConvRate "
         "from currencyformats_v1 cf "
         "where cf.currencyid=FROMACC.CURRENCYID "
       ") as BaseConvRate, "

    "FROMACC.CurrencyID as CurrencyID, "
    "FROMACC.AccountName as AccountName,  "
    "FROMACC.AccountID as AccountID, "
    "TOACC.AccountName as ToAccountName, "
    "TOACC.ACCOUNTId as ToAccountID, "
    "CANS.ToTransAmount ToTransAmount, "
    "TOACC.CURRENCYID as ToCurrencyID, "
    "0 as Splitted, "
    "CAT.CategID as CategID, "
    "SUBCAT.SubCategID as SubCategID, "
    "PAYEE.PayeeName as Payee, "
    "PAYEE.PayeeID as PayeeID, "
    "CANS.TRANSACTIONNUMBER as TransactionNumber, "
    "CANS.Status as Status, "
    "CANS.NOTES as Notes "

"from  CHECKINGACCOUNT_V1 CANS "

"join CATEGORY_V1 CAT "
"on CAT.CATEGID = CANS.CATEGID "

"left join SUBCATEGORY_V1 SUBCAT "
"on SUBCAT.SUBCATEGID = CANS.SUBCATEGID "

"left join PAYEE_V1 PAYEE "
"on PAYEE.PAYEEID = CANS.PAYEEID "

"left join ACCOUNTLIST_V1 FROMACC "
"on FROMACC.ACCOUNTID = CANS.ACCOUNTID "

"left join ACCOUNTLIST_V1 TOACC "
"on TOACC.ACCOUNTID = CANS.TOACCOUNTID "

"UNION ALL "

"SELECT CASS.TRANSID, "
       "CASS.TRANSCODE, "
       "CASS.TransDate, "
       "round(strftime('%Y', CASS.TransDate)) as Year, "
       "round(strftime('%m', CASS.TransDate)) as Month, "
       "round(strftime('%d', CASS.TransDate)) as Day,"
       "CAT.CATEGNAME, "
       "SUBCAT.SUBCATEGNAME,"
       "ROUND(SPLIT.SPLITTRANSAMOUNT, 2)*(case when CASS.TRANSCODE='Withdrawal' then -1 else 1 end) as Amount, "

       "( select cf.BaseConvRate "
         "from currencyformats_v1 cf "
         "where cf.currencyid = FROMACC.CURRENCYID "
       ") as BaseConvRate, "

       "FROMACC.CURRENCYID, "
       "FROMACC.ACCOUNTNAME, "
       "FROMACC.ACCOUNTId,"
       "TOACC.ACCOUNTNAME, "
       "TOACC.ACCOUNTId, "
       "CASS.totransamount, "
       "TOACC.CURRENCYID, "
       "1 as Splitted, "
       "CAT.CATEGId, "
       "SUBCAT.SUBCATEGId, "
       "PAYEE.PAYEENAME, "
       "PAYEE.PAYEEID, "
       "CASS.TRANSACTIONNUMBER, "
       "CASS.Status, "
       "CASS.NOTES "

"from CHECKINGACCOUNT_V1 CASS "

"join SPLITTRANSACTIONS_V1 SPLIT "
"on SPLIT.TRANSID = CASS.TRANSID "

"join CATEGORY_V1 CAT "
"on CAT.CATEGID = SPLIT.CATEGID "

"left join SUBCATEGORY_V1 SUBCAT "
"on SUBCAT.SUBCATEGID = SPLIT.SUBCATEGID "

"left join PAYEE_V1 PAYEE "
"on PAYEE.PAYEEID = CASS.PAYEEID "

"left join ACCOUNTLIST_V1 FROMACC "
"on FROMACC.ACCOUNTID = CASS.ACCOUNTID "

"left join ACCOUNTLIST_V1 TOACC "
"on TOACC.ACCOUNTID = CASS.TOACCOUNTID "

"order by CANS.transid";

    bool exists = ViewExists(db, view_name);
    
    if (!exists) {
        db->ExecuteUpdate(sql);
        exists = ViewExists(db, view_name);
        wxASSERT(exists);   
    }
}

void removeCruft(wxSQLite3Database* db)
{
    {
        if (db->TableExists(wxT("SPLITTRANSACTIONS_V1")))
        {
            db->ExecuteUpdate("DELETE FROM SPLITTRANSACTIONS_V1 WHERE SPLITTRANSACTIONS_V1.TRANSID NOT IN (SELECT TRANSID FROM CHECKINGACCOUNT_V1)");
        }
    }

    {
        if (db->TableExists(wxT("BUDGETSPLITTRANSACTIONS_V1")))
        {
            db->ExecuteUpdate("DELETE FROM BUDGETSPLITTRANSACTIONS_V1 WHERE BUDGETSPLITTRANSACTIONS_V1.TRANSID NOT IN (SELECT BDID FROM BILLSDEPOSITS_V1)");
        }
    }

#if 0
    {
        wxSQLite3StatementBuffer bufSQL3;
        bufSQL3.Format("UPDATE CHECKINGACCOUNT_V1 SET CATEGID = -1, SUBCATEGID=-1  WHERE CHECKINGACCOUNT_V1.TRANSID IN (SELECT SPLITTRANSACTIONS_V1.TRANSID FROM SPLITTRANSACTIONS_V1);");
        db->ExecuteUpdate(bufSQL3);
    }

   {
        wxSQLite3StatementBuffer bufSQL3;
        bufSQL3.Format("UPDATE BUDGETSPLITTRANSACTIONS_V1 SET CATEGID = -1, SUBCATEGID=-1  WHERE BUDGETSPLITTRANSACTIONS_V1.TRANSID IN (SELECT BUDGETSPLITTRANSACTIONS_V1.TRANSID FROM BUDGETSPLITTRANSACTIONS_V1);");
        db->ExecuteUpdate(bufSQL3);
    }
#endif
}

/*
    This routine sets up a new DB as well as update an old one
*/

void mmDBWrapper::initDB(wxSQLite3Database* db, wxProgressDialog* pgd)
{
    /* Create INFOTABLE_V1 Tables */
    createInfoV1Table(db);
    if (pgd)
        pgd->Update(10);

    /* Create Currency Settings */
    createCurrencyV1Table(db);
    if (pgd)
        pgd->Update(20);

    /* Create ACCOUNTLIST_V1 Tables */
    createAccountListV1Table(db);
    if (pgd)
        pgd->Update(30);

    /* Create CHECKINGACCOUNT_V1 Tables */
    createCheckingAccountV1Table(db);
    if (pgd)
        pgd->Update(40);

    /* Create PAYEE_V1 Tables */
    createPayeeV1Table(db);
    if (pgd)
        pgd->Update(50);
  
    /* Create CATEGORY_V1 Tables */
    createCategoryV1Table(db);
    if (pgd)
        pgd->Update(60);

    /* Create Budgeting_V1 Tables */
    createBudgetingV1Table(db);
    if (pgd)
        pgd->Update(75);

    /* Create Bills & Deposits V1 Table */
    createBillsDepositsV1Table(db);
    if (pgd)
        pgd->Update(80);

    /* Create Stock V1 Table */
    createStockV1Table(db);
    if (pgd)
        pgd->Update(85);

	/* Create Asset V1 Table */
	createAssetsV1Table(db);
    if (pgd)
    	pgd->Update(90);

	/* Create SplitTransactions V1 Table */
	createSplitTransactionsV1Table(db);
    if (pgd)
    	pgd->Update(95);

	/* Create AllData view */
   createAllDataView(db);
   if (pgd)
       pgd->Update(99);

    /* Remove Any cruft */
    removeCruft(db);
}

int mmDBWrapper::getNumAccounts(wxSQLite3Database* db)
{
    int num = db->ExecuteScalar("select count(*) from ACCOUNTLIST_V1");
    return num;
}

wxString mmDBWrapper::getAccountName(wxSQLite3Database* db, int accountID)
{
    wxString name;

    wxSQLite3Statement st = db->PrepareStatement("select ACCOUNTNAME "
                                                 "from ACCOUNTLIST_V1 "
                                                 "where ACCOUNTID = ?"
                                                );
    
    st.Bind(1, accountID);

    wxSQLite3ResultSet q1 = st.ExecuteQuery();
    if (q1.NextRow())
    {
        name = q1.GetString(wxT("ACCOUNTNAME"));
    }
    else
    {
        wxASSERT(true);
    }

    st.Finalize();

    return name;
}

int mmDBWrapper::getAccountID(wxSQLite3Database* db, const wxString& accountName)
{
    int id = -1;

    wxSQLite3Statement st = db->PrepareStatement("select ACCOUNTID "
                                                 "from ACCOUNTLIST_V1 "
                                                 "where ACCOUNTNAME = ?"
                                                );

    st.Bind(1, accountName);

    wxSQLite3ResultSet q1 = st.ExecuteQuery();
    if (q1.NextRow())
    {
        id = q1.GetInt(wxT("ACCOUNTID"));
    }

    st.Finalize();

    return id;
}

void mmDBWrapper::loadSettings(int accountID, wxSQLite3Database* db)
{
    wxSQLite3Statement st = db->PrepareStatement("select CURRENCYID "
                                                 "from ACCOUNTLIST_V1 "
                                                 "where ACCOUNTID = ?"
                                                );
    
    st.Bind(1, accountID);

    wxSQLite3ResultSet q1 = st.ExecuteQuery();
    if (q1.NextRow())
    {
        int currencyID = q1.GetInt(wxT("CURRENCYID"));
        loadSettings(db, currencyID);
    }

    st.Finalize();
}

int mmDBWrapper::getBaseCurrencySettings(wxSQLite3Database* db)
{
    static const char sql[] = 
    "select INFOVALUE "
    "from INFOTABLE_V1 "
    "where INFONAME = ?";
    
    const int defID = -1;
    int currencyID = defID;

    wxSQLite3Statement st = db->PrepareStatement(sql);
    st.Bind(1, g_BaseCurrencyIdName);
    
    wxSQLite3ResultSet q1 = st.ExecuteQuery();
    if (q1.NextRow())
    {
        currencyID = q1.GetInt(wxT("INFOVALUE"), defID);
    }
    st.Finalize();
    
    return currencyID;
}

void mmDBWrapper::setBaseCurrencySettings(wxSQLite3Database* db, int currencyID)
{
    wxSQLite3Statement st = db->PrepareStatement("update INFOTABLE_V1 "
                                                 "SET INFOVALUE = ? "
                                                 "WHERE INFONAME = ?"
                                                );

    st.Bind(1, currencyID);
    st.Bind(2, g_BaseCurrencyIdName);

    int rows_affected = st.ExecuteUpdate();
    st.Finalize();

    if (!rows_affected)
    {
        st = db->PrepareStatement("insert into INFOTABLE_V1 (INFONAME, INFOVALUE) values (?, ?)");
        st.Bind(1, g_BaseCurrencyIdName);
        st.Bind(2, currencyID);

        rows_affected = st.ExecuteUpdate();
        st.Finalize();
    }
    
    wxASSERT(rows_affected == 1);
}

void mmDBWrapper::loadBaseCurrencySettings(wxSQLite3Database* db)
{
    int currencyID = getBaseCurrencySettings(db);
    if (currencyID != -1)
    {
        loadSettings(db, currencyID);
    }
    else
    {
        mmCurrencyFormatter::loadDefaultSettings();
    }
}

void mmDBWrapper::loadSettings(wxSQLite3Database* db, int currencyID)
{
    static const char sql[] = 
    "select PFX_SYMBOL, "
           "SFX_SYMBOL, "
           "DECIMAL_POINT, "
           "GROUP_SEPARATOR, "
           "UNIT_NAME, "
           "CENT_NAME, "
           "SCALE, "
           "CURRENCY_SYMBOL "
    "from CURRENCYFORMATS_V1 "
    "where CURRENCYID = ?";
    
    wxSQLite3Statement st = db->PrepareStatement(sql);
    st.Bind(1, currencyID);

    wxSQLite3ResultSet q1 = st.ExecuteQuery();
    
    if (q1.NextRow())
    {
        wxString pfxSymbol = q1.GetString(wxT("PFX_SYMBOL"));
        wxString sfxSymbol = q1.GetString(wxT("SFX_SYMBOL"));
        wxString dec = q1.GetString(wxT("DECIMAL_POINT"));
        wxString grp = q1.GetString(wxT("GROUP_SEPARATOR"));
        wxString unit = q1.GetString(wxT("UNIT_NAME"));
        wxString cent = q1.GetString(wxT("CENT_NAME"));
        double scaleDl = q1.GetDouble(wxT("SCALE"));
        wxString currencySymbol = q1.GetString(wxT("CURRENCY_SYMBOL"));
        
        wxChar decChar = 0;
        wxChar grpChar = 0;
        
        if (!dec.IsEmpty())
        {
            decChar = dec.GetChar(0);
        }

        if (!grp.IsEmpty())
        {
            grpChar = grp.GetChar(0);
        }

        if(currencySymbol == wxEmptyString) {
            currencySymbol = wxT("");
        }

        mmCurrencyFormatter::loadSettings(pfxSymbol, sfxSymbol, 
            decChar, grpChar, unit, cent, scaleDl);
    }
    else
    {
        wxASSERT(true);
    }

    st.Finalize();
}

double mmDBWrapper::getReconciledBalanceOnAccount(wxSQLite3Database* db, int accountID)
{
    double balance = 0.0;

    wxSQLite3Statement st = db->PrepareStatement("select INITIALBAL "
                                                 "from ACCOUNTLIST_V1 "
                                                 "where ACCOUNTID = ?"
                                                );

    st.Bind(1, accountID);
    
    wxSQLite3ResultSet q1 = st.ExecuteQuery();
    if (q1.NextRow()) 
    {
        balance = q1.GetDouble(wxT("INITIALBAL"));
    }

    st.Finalize();
    
    // --

    static const char sql[] =
    "select TRANSCODE, "
           "STATUS, "  
           "ACCOUNTID, "
           "TOACCOUNTID, "
           "TRANSAMOUNT, "
           "TOTRANSAMOUNT "
    "from CHECKINGACCOUNT_V1 "
    "where ACCOUNTID = ? OR TOACCOUNTID = ?";

    st = db->PrepareStatement(sql);
    st.Bind(1, accountID);
    st.Bind(2, accountID);

    q1 = st.ExecuteQuery();
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
    st.Finalize();

    return balance;
}

double mmDBWrapper::getTotalBalanceOnAccount(wxSQLite3Database* db, int accountID, bool ignoreFuture)
{
    static const char sql[] =
    "select INITIALBAL "
    "from ACCOUNTLIST_V1 "
    "where ACCOUNTID = ?";
        
    double balance = 0.0;

    wxSQLite3Statement st = db->PrepareStatement(sql);
    st.Bind(1, accountID);

    wxSQLite3ResultSet q1 = st.ExecuteQuery();
    if (q1.NextRow()) 
    {
        balance = q1.GetDouble(wxT("INITIALBAL"));
    }

    st.Finalize();

    // --

    static const char sql2[] =
    "select TRANSCODE, "
           "STATUS, " 
           "ACCOUNTID, "
           "TOACCOUNTID, "
           "TRANSAMOUNT, "
           "TOTRANSAMOUNT, "
           "TRANSDATE "
    "from CHECKINGACCOUNT_V1 "
    "where ACCOUNTID = ? OR TOACCOUNTID = ?";

    st = db->PrepareStatement(sql2);
    st.Bind(1, accountID);
    st.Bind(2, accountID);

    q1 = st.ExecuteQuery();
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
    st.Finalize();

    return balance;
}

bool mmDBWrapper::getExpensesIncome(wxSQLite3Database* db, 
                                    int accountID, 
                                    double& expenses, 
                                    double& income,  
                                    bool ignoreDate, 
                                    wxDateTime dtBegin, 
                                    wxDateTime dtEnd)
{
    static const std::string sql_base =
    "select ca.TRANSCODE, "
           "ca.TRANSAMOUNT, "
           "ca.STATUS, "
           "ca.TRANSDATE, "
           "cf.BASECONVRATE "
    "from CHECKINGACCOUNT_V1 ca " + joinCURRENCYFORMATS("cf", "ca.ACCOUNTID");

    wxSQLite3Statement st;

    if (accountID != -1)
    {
        std::string sql(sql_base);
        sql += " where ca.ACCOUNTID = ? OR ca.TOACCOUNTID = ?";

        st = db->PrepareStatement(sql.c_str());
        st.Bind(1, accountID);
        st.Bind(2, accountID);
    }
    else
    {
        st = db->PrepareStatement(sql_base.c_str());
    }
    
    wxSQLite3ResultSet q1 = st.ExecuteQuery();
    while (q1.NextRow())
    {
        wxString transTypeString = q1.GetString(wxT("TRANSCODE"));
        double transAmount = q1.GetDouble(wxT("TRANSAMOUNT"));
        wxString transStatus = q1.GetString(wxT("STATUS"));
        wxString dateString = q1.GetString(wxT("TRANSDATE"));
        wxDateTime dtdt = mmGetStorageStringAsDate(dateString);

        double dbRate = q1.GetDouble(wxT("BASECONVRATE"), g_defBASECONVRATE);
        transAmount = transAmount * dbRate;

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
    st.Finalize();

    return true;
}


void mmDBWrapper::addPayee(wxSQLite3Database* db, const wxString &payee, int categID, int subcategID)
{
    static const char sql[] = 
    "insert into PAYEE_V1 (PAYEENAME, CATEGID, SUBCATEGID) values (?, ?, ?)";

    wxSQLite3Statement st = db->PrepareStatement(sql);
    st.Bind(1, payee);
    st.Bind(2, categID);
    st.Bind(3, subcategID);

    st.ExecuteUpdate();
    st.Finalize();
}

bool mmDBWrapper::getPayeeID(wxSQLite3Database* db, const wxString &payee, int& payeeID, 
                             int& categID, int& subcategID )
{
    static const char sql[] = 
    "select PAYEEID, "
           "CATEGID, "
           "SUBCATEGID "
    "from PAYEE_V1 "
    "where PAYEENAME = ?";

    bool found = false;

    wxSQLite3Statement st = db->PrepareStatement(sql);
    st.Bind(1, payee);

    wxSQLite3ResultSet q1 = st.ExecuteQuery();
    found = q1.NextRow(); 

    if (found)
    {
        payeeID = q1.GetInt(wxT("PAYEEID"));
        categID = q1.GetInt(wxT("CATEGID"));
        subcategID = q1.GetInt(wxT("SUBCATEGID"));
    }
    
    st.Finalize();

    return found;
}

bool mmDBWrapper::deleteCategoryWithConstraints(wxSQLite3Database* db, int categID)
{
    static const char sql[] = 
    "select 1 "
    "from ( select CATEGID from CHECKINGACCOUNT_V1 "
           "union all " 
           "select CATEGID from SPLITTRANSACTIONS_V1 "
           "union all " 
           "select CATEGID from BUDGETSPLITTRANSACTIONS_V1 "
         ") "
    "where CATEGID = ? "
    "limit 1"; // return only one row

    {
        wxSQLite3Statement st = db->PrepareStatement(sql);
        st.Bind(1, categID);

        wxSQLite3ResultSet q1 = st.ExecuteQuery();
        bool found = q1.NextRow();
        st.Finalize();

        if (found)
        {
            return false;
        }
    }

    // --

    static const char* sql_del[] = 
    {
        "delete from SUBCATEGORY_V1     where CATEGID = ?",
        "delete from CATEGORY_V1        where CATEGID = ?",
        "delete from BUDGETTABLE_V1     WHERE CATEGID = ?",
        "update PAYEE_V1 set CATEGID=-1 WHERE CATEGID = ?",
        0
    };

    for (int i = 0; sql_del[i]; ++i)
    {
        wxSQLite3Statement st = db->PrepareStatement(sql_del[i]);
        st.Bind(1, categID);
        st.ExecuteUpdate();
        st.Finalize();
    }
    
    return true;
}

bool mmDBWrapper::deleteSubCategoryWithConstraints(wxSQLite3Database* db, int categID, int subcategID)
{
    static const char sql[] = 
    "select 1 "
    "from ( select CATEGID, SUBCATEGID from CHECKINGACCOUNT_V1 "
           "union all " 
           "select CATEGID, SUBCATEGID from SPLITTRANSACTIONS_V1 "
           "union all " 
           "select CATEGID, SUBCATEGID from BUDGETSPLITTRANSACTIONS_V1 "
         ") "
    "where CATEGID = ? and SUBCATEGID = ? "
    "limit 1"; // return only one row

    {
        wxSQLite3Statement st = db->PrepareStatement(sql);
        st.Bind(1, categID);
        st.Bind(2, subcategID);

        wxSQLite3ResultSet q1 = st.ExecuteQuery();
        bool found = q1.NextRow();
        st.Finalize();

        if (found)
        {
            return false;
        }
    }

    // --

    static const char* sql_del[] = 
    {
        "delete from SUBCATEGORY_V1 where CATEGID=? AND SUBCATEGID=?",
        "delete from BUDGETTABLE_V1 WHERE CATEGID=? AND SUBCATEGID=?",
        
        "update PAYEE_V1 set CATEGID = -1, SUBCATEGID = -1 "
                                   "WHERE CATEGID=? AND SUBCATEGID=?",
        0
    };

    for (int i = 0; sql_del[i]; ++i)
    {
        wxSQLite3Statement st = db->PrepareStatement(sql_del[i]);
        st.Bind(1, categID);
        st.Bind(2, subcategID);

        st.ExecuteUpdate();
        st.Finalize();
    }

    return true;
}

bool mmDBWrapper::updateCategory(wxSQLite3Database* db, int categID, 
                                 int subcategID, const wxString &newName)
{
    if (subcategID == -1)
    {
        wxSQLite3Statement st = db->PrepareStatement("update CATEGORY_V1 "
                                                     "SET CATEGNAME = ? "
                                                     "WHERE CATEGID = ?"
                                                    );
        
        st.Bind(1, newName);
        st.Bind(2, categID);

        st.ExecuteUpdate();
        st.Finalize();
    }
    else
    {
        wxSQLite3Statement st = db->PrepareStatement("update SUBCATEGORY_V1 "
                                                     "SET SUBCATEGNAME = ? "
                                                     "WHERE SUBCATEGID = ?"
                                                    );

        st.Bind(1, newName);
        st.Bind(2, subcategID);

        st.ExecuteUpdate();
        st.Finalize();
    }

    return true;
}

bool mmDBWrapper::addCategory(wxSQLite3Database* db, const wxString &newName)
{
    int rows_affected = 0;
    
    wxSQLite3Statement st = db->PrepareStatement("insert into CATEGORY_V1 (CATEGNAME) values (?)");
    st.Bind(1, newName);
    rows_affected = st.ExecuteUpdate();
    st.Finalize();

    return rows_affected == 1;
}

bool mmDBWrapper::addSubCategory(wxSQLite3Database* db, int categID, const wxString &newName)
{
    static const char sql[] = 
    "insert into SUBCATEGORY_V1 (SUBCATEGNAME, CATEGID) values (?, ?)";

    int rows_affected = 0;

    wxSQLite3Statement st = db->PrepareStatement(sql);
    st.Bind(1, newName);
    st.Bind(2, categID);

    rows_affected = st.ExecuteUpdate();
    st.Finalize();

    return rows_affected == 1;
}

int mmDBWrapper::getCategoryID(wxSQLite3Database* db, const wxString &name)
{
    static const char sql[] = 
    "select CATEGID "
    "from CATEGORY_V1 "
    "where CATEGNAME = ?";
    
    int categID = -1;
    
    wxSQLite3Statement st = db->PrepareStatement(sql);
    st.Bind(1, name);

    wxSQLite3ResultSet q1 = st.ExecuteQuery();
    if (q1.NextRow())
    {
        categID = q1.GetInt(wxT("CATEGID"));
    }
    st.Finalize();

    return categID;
}

int mmDBWrapper::getSubCategoryID(wxSQLite3Database* db, int categID, const wxString &name)
{
    static const char sql[] = 
    "select SUBCATEGID "
    "from SUBCATEGORY_V1 "
    "where SUBCATEGNAME = ? and "
          "CATEGID = ?";

    int subcategID = -1;
    
    wxSQLite3Statement st = db->PrepareStatement(sql);
    st.Bind(1, name);
    st.Bind(2, categID);

    wxSQLite3ResultSet q1 = st.ExecuteQuery();
    if (q1.NextRow())
    {
        subcategID = q1.GetInt(wxT("SUBCATEGID"));
    }
    st.Finalize();

    return subcategID;
}

wxString mmDBWrapper::getCategoryName(wxSQLite3Database* db, int categID)
{
    static const char sql[] = 
    "select CATEGNAME "
    "from CATEGORY_V1 "
    "where CATEGID = ?";

    wxString name;

    wxSQLite3Statement st = db->PrepareStatement(sql);
    st.Bind(1, categID);
    
    wxSQLite3ResultSet q1 = st.ExecuteQuery();
    if (q1.NextRow())
    {
        name = q1.GetString(wxT("CATEGNAME"));
    }
    
    st.Finalize();

    return name;
}

wxString mmDBWrapper::getSubCategoryName(wxSQLite3Database* db, int categID, int subcategID)
{
    static const char sql[] = 
    "select SUBCATEGNAME "
    "from SUBCATEGORY_V1 "
    "where CATEGID = ? and "
          "SUBCATEGID = ?";
    
    wxString name;
    
    wxSQLite3Statement st = db->PrepareStatement(sql);
    st.Bind(1, categID);
    st.Bind(2, subcategID);
    
    wxSQLite3ResultSet q1 = st.ExecuteQuery();
    if (q1.NextRow())
    {
        name = q1.GetString(wxT("SUBCATEGNAME"));
    }
    
    st.Finalize();
    
    return name;
}

bool mmDBWrapper::deleteTransaction(wxSQLite3Database* db, int transID)
{
    static const char* sql[] = 
    {
        "delete from CHECKINGACCOUNT_V1   where TRANSID = ?",
        "delete from SPLITTRANSACTIONS_V1 where TRANSID = ?",
        0
    };

    for (int i = 0; sql[i]; ++i)
    {
        wxSQLite3Statement st = db->PrepareStatement(sql[i]);
        st.Bind(1, transID);
        st.ExecuteUpdate();
        st.Finalize();
    }

    return true;
}


bool mmDBWrapper::deleteFlaggedTransactions(wxSQLite3Database* db, int accountID)
{
    static const char sql[] = 
    "delete from CHECKINGACCOUNT_V1 "
    "where STATUS = 'F' and "
         "(ACCOUNTID = ? OR TOACCOUNTID = ?)";

    wxSQLite3Statement st = db->PrepareStatement(sql);
    st.Bind(1, accountID);
    st.Bind(2, accountID);
    
    st.ExecuteUpdate();
    st.Finalize();

    return true;
}

bool mmDBWrapper::updatePayee(wxSQLite3Database* db, const wxString& payeeName, 
                              int payeeID, int categID, int subcategID)
{
    static const char sql[] = 
    "update PAYEE_V1 "
    "SET PAYEENAME = ?, "
        "CATEGID = ?, "
        "SUBCATEGID = ? "
    "WHERE PAYEEID = ?";

    wxSQLite3Statement st = db->PrepareStatement(sql);
    st.Bind(1, payeeName);
    st.Bind(2, categID);
    st.Bind(3, subcategID);
    st.Bind(4, payeeID);
    
    st.ExecuteUpdate();
    st.Finalize();

    return true;
}

bool mmDBWrapper::deletePayeeWithConstraints(wxSQLite3Database* db, int payeeID)
{
    wxSQLite3Statement st = db->PrepareStatement("select 1 "
                                                 "from CHECKINGACCOUNT_V1 "
                                                 "where PAYEEID = ? "
                                                 "limit 1" // return just one row
                                                );

    st.Bind(1, payeeID);

    wxSQLite3ResultSet q1 = st.ExecuteQuery();
    bool found = q1.NextRow();
    st.Finalize();

    if (found)
    {
        return false;
    }
    
    // --

    st = db->PrepareStatement("delete from PAYEE_V1 where PAYEEID = ?");
    st.Bind(1, payeeID);
    
    st.ExecuteUpdate();
    st.Finalize();

    return true;
}

wxString mmDBWrapper::getPayee(wxSQLite3Database* db, int payeeID, int& categID, int& subcategID )
{
    static const char sql[] = 
    "select PAYEENAME, "
           "CATEGID, "   
           "SUBCATEGID "
    "from PAYEE_V1 "
    "where PAYEEID = ?";
    
    wxString payeeName;

    wxSQLite3Statement st = db->PrepareStatement(sql);
    st.Bind(1, payeeID);

    wxSQLite3ResultSet q1 = st.ExecuteQuery();
    if (q1.NextRow())
    {
        payeeName = q1.GetString(wxT("PAYEENAME"));
        categID = q1.GetInt(wxT("CATEGID"));
        subcategID = q1.GetInt(wxT("SUBCATEGID"));
    }
    st.Finalize();

    return payeeName;
}

int mmDBWrapper::getCurrencyID(wxSQLite3Database* db, const wxString& currencyName)
{
    int currencyID = -1;
    
    wxSQLite3Statement st = db->PrepareStatement("select CURRENCYID "
                                                 "from CURRENCYFORMATS_V1 "
                                                 "where CURRENCYNAME = ?"
                                                );
    
    st.Bind(1, currencyName);

    wxSQLite3ResultSet q1 = st.ExecuteQuery();
    if (q1.NextRow())
    {
        currencyID = q1.GetInt(wxT("CURRENCYID"));
    }

    st.Finalize();

    return currencyID;
}

wxString mmDBWrapper::getCurrencyName(wxSQLite3Database* db, int currencyID)
{
    wxString name;
    
    wxSQLite3Statement st = db->PrepareStatement("select CURRENCYNAME "
                                                 "from CURRENCYFORMATS_V1 "
                                                 "where CURRENCYID = ?"
                                                );

    st.Bind(1, currencyID);

    wxSQLite3ResultSet q1 = st.ExecuteQuery();
    if (q1.NextRow())
    {
        name = q1.GetString(wxT("CURRENCYNAME"));
    }

    st.Finalize();

    return name;
}

wxString mmDBWrapper::getCurrencySymbol(wxSQLite3Database* db, int currencyID)
{
    wxString symbol;

    wxSQLite3Statement st = db->PrepareStatement("select CURRENCY_SYMBOL "
                                                 "from CURRENCYFORMATS_V1 "
                                                 "where CURRENCYID = ?"
                                                );

    st.Bind(1, currencyID);

    wxSQLite3ResultSet q1 = st.ExecuteQuery();
    if (q1.NextRow())
    {
        symbol = q1.GetString(wxT("CURRENCY_SYMBOL"));
    }
    
    st.Finalize();
    
    return symbol;
}

double mmDBWrapper::getCurrencyBaseConvRateForId(wxSQLite3Database* db, int currencyID)
{
    double rate = g_defBASECONVRATE;

    wxSQLite3Statement st = db->PrepareStatement("select BASECONVRATE "
                                                 "from CURRENCYFORMATS_V1 "
                                                 "where CURRENCYID = ?"
                                                );
    st.Bind(1, currencyID);
    
    wxSQLite3ResultSet q1 = st.ExecuteQuery();
    if (q1.NextRow())
    {
        rate = q1.GetDouble(wxT("BASECONVRATE"), g_defBASECONVRATE);
    }
    
    st.Finalize();
    
    return rate;
}

double mmDBWrapper::getCurrencyBaseConvRate(wxSQLite3Database* db, int accountID)
{
    static const char sql[] = 
    "select f.BASECONVRATE "
    "from ACCOUNTLIST_V1 acl "
    
    "join CURRENCYFORMATS_V1 f "
    "on f.CURRENCYID = acl.CURRENCYID "

    "where acl.ACCOUNTID = ?";

    double rate = g_defBASECONVRATE;

    wxSQLite3Statement st = db->PrepareStatement(sql);
    st.Bind(1, accountID);

    wxSQLite3ResultSet q1 = st.ExecuteQuery();
    if (q1.NextRow())
    {
        rate = q1.GetDouble(wxT("BASECONVRATE"), g_defBASECONVRATE);
    }
    else
    {
        /* cannot find accountid */
        wxASSERT(true);
    }

    st.Finalize();

    return rate;
}
//--------------------------------------------------------------------

void mmDBWrapper::verifyINIDB(wxSQLite3Database* inidb)
{
    wxASSERT(inidb);

    bool exists = inidb->TableExists(wxT("SETTING_V1"));
    if (!exists)
    {
        /* Create INFOTABLE_V1 Table */
        inidb->ExecuteUpdate(wxT("create table SETTING_V1(SETTINGID integer primary key, \
                              SETTINGNAME TEXT NOT NULL, SETTINGVALUE TEXT)"));
        
        {
            bool ok = inidb->TableExists(wxT("SETTING_V1"));
            wxASSERT(ok);
            ok = ok; // removes compiler's warning
        }
    }
}

wxString mmDBWrapper::getINISettingValue(wxSQLite3Database* db, const wxString& settingName, 
                                            const wxString& defaultVal)
{
    wxString value = defaultVal;

    wxSQLite3Statement st = db->PrepareStatement("select SETTINGVALUE "
                                                 "from SETTING_V1 "
                                                 "where SETTINGNAME = ?"
                                                );
    
    st.Bind(1, settingName);

    wxSQLite3ResultSet q1 = st.ExecuteQuery();
    if (q1.NextRow())
    {
        value = q1.GetString(wxT("SETTINGVALUE"));
    }

    st.Finalize();

    return value;
}

void mmDBWrapper::setINISettingValue(wxSQLite3Database* db, const wxString& settingName, 
                                     const wxString& settingValue)
{
    wxSQLite3Statement st = db->PrepareStatement("update SETTING_V1 "
                                                 "set SETTINGVALUE = ? "
                                                 "where SETTINGNAME = ?"
                                                );

    st.Bind(1, settingValue);
    st.Bind(2, settingName);

    int rows_affected = st.ExecuteUpdate();
    st.Finalize();

    if (!rows_affected)
    {
        st = db->PrepareStatement("insert into SETTING_V1 (SETTINGNAME, SETTINGVALUE) values (?, ?)");
        st.Bind(1, settingName);
        st.Bind(2, settingValue);

        rows_affected = st.ExecuteUpdate();
        st.Finalize();
    }

    wxASSERT(rows_affected == 1);
}

void mmDBWrapper::updateTransactionWithStatus(wxSQLite3Database &db, int transID, 
                                              const wxString& status)
{
    wxSQLite3Statement st = db.PrepareStatement("update CHECKINGACCOUNT_V1 set STATUS=? where TRANSID=?");
    st.Bind(1, status);
    st.Bind(2, transID);

    st.ExecuteUpdate();
    st.Finalize();
}

wxString mmDBWrapper::getInfoSettingValue(wxSQLite3Database* db, const wxString& settingName, 
                                            const wxString& defaultVal)
{
    wxString value = defaultVal;

    wxSQLite3Statement st = db->PrepareStatement("select INFOVALUE from INFOTABLE_V1 where INFONAME = ?");
    st.Bind(1, settingName);

    wxSQLite3ResultSet q1 = st.ExecuteQuery();
    if (q1.NextRow())
    {
        value = q1.GetString(wxT("INFOVALUE"));
    }
    st.Finalize();

    return value;
}

void mmDBWrapper::setInfoSettingValue(wxSQLite3Database* db, const wxString& settingName, 
                                     const wxString& settingValue)
{
    wxSQLite3Statement st = db->PrepareStatement("update INFOTABLE_V1 set INFOVALUE=? where INFONAME=?");
    st.Bind(1, settingValue);
    st.Bind(2, settingName);

    int rows_affected = st.ExecuteUpdate();
    st.Finalize();

    if (!rows_affected)
    {
        st = db->PrepareStatement("insert into INFOTABLE_V1 (INFONAME, INFOVALUE) values (?, ?)");
        st.Bind(1, settingName);
        st.Bind(2, settingValue);

        rows_affected = st.ExecuteUpdate();
        st.Finalize();
    }

    wxASSERT(rows_affected == 1);
}

double mmDBWrapper::getAmountForCategory(wxSQLite3Database* db, 
                                         int categID, 
                                         int subcategID,
                                         bool ignoreDate,
                                         wxDateTime dtBegin,
                                         wxDateTime dtEnd)
{
    static const std::string sql = 
    "select ca.TRANSCODE, "
           "ca.TRANSAMOUNT, "
           "ca.STATUS, "
           "ca.TRANSDATE, "
           "cf.BASECONVRATE "
    "from CHECKINGACCOUNT_V1 ca " + joinCURRENCYFORMATS("cf", "ca.ACCOUNTID") +
   " where ca.CATEGID = ? AND "
          "ca.SUBCATEGID = ?";

#if 0
    if (subcategID == -1)
    {
        const char sql[] = "select * from CHECKINGACCOUNT_V1 where CATEGID=?";
    }
#endif

    double amt = 0.0;

    wxSQLite3Statement st = db->PrepareStatement(sql.c_str());
    st.Bind(1, categID);
    st.Bind(2, subcategID);

    wxSQLite3ResultSet q1 = st.ExecuteQuery();
    while (q1.NextRow())
    {
        wxString code = q1.GetString(wxT("TRANSCODE"));
        double transAmount = q1.GetDouble(wxT("TRANSAMOUNT"));
        wxString transStatus = q1.GetString(wxT("STATUS"));
        wxString dateString = q1.GetString(wxT("TRANSDATE"));
        wxDateTime dtdt = mmGetStorageStringAsDate(dateString);

        double dbRate = q1.GetDouble(wxT("BASECONVRATE"), g_defBASECONVRATE);
        transAmount = transAmount * dbRate;

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
    
    st.Finalize();

    // --

    static const std::string sql2 = 
    "select ca.TRANSID, "
           "ca.TRANSCODE, "
           "ca.STATUS, "
           "ca.TRANSDATE, "
           "cf.BASECONVRATE "
    "from CHECKINGACCOUNT_V1 ca " + joinCURRENCYFORMATS("cf", "ca.ACCOUNTID") +
   " where ca.TRANSID in( select TRANSID "
                         "from SPLITTRANSACTIONS_V1 "
                         "where CATEGID = ? and "
                               "SUBCATEGID = ? "
                       ")";


    st = db->PrepareStatement(sql2.c_str());
    st.Bind(1, categID);
    st.Bind(2, subcategID);

    q1 = st.ExecuteQuery();

    while (q1.NextRow())
    {
         int transID = q1.GetInt(wxT("TRANSID"));
         wxString code = q1.GetString(wxT("TRANSCODE"));
         wxString transStatus = q1.GetString(wxT("STATUS"));
         wxString dateString = q1.GetString(wxT("TRANSDATE"));
         wxDateTime dtdt = mmGetStorageStringAsDate(dateString);
         double dbRate = q1.GetDouble(wxT("BASECONVRATE"), g_defBASECONVRATE);

         if (transStatus == wxT("V"))
             continue; // skip

         if (!ignoreDate)
         {
             if (!dtdt.IsBetween(dtBegin, dtEnd))
                 continue; //skip
         }

         if (code == wxT("Transfer"))
             continue;

         // This is a valid transaction get amount for
         // this transaction
         double val = getSplitTransactionValueForCategory(db, transID, categID, subcategID);
         val = val * dbRate;
         if (code == wxT("Withdrawal"))
             amt = amt - val;
         else if (code == wxT("Deposit"))
             amt = amt + val;
    }

    st.Finalize();

    return amt;
}   

double mmDBWrapper::getSplitTransactionValueForCategory(wxSQLite3Database* db, int transID, 
                                                      int categID, int subcategID)
{
    static const char sql[] = 
    "select SPLITTRANSAMOUNT "
    "from SPLITTRANSACTIONS_V1 "
    "where CATEGID = ? and "
          "SUBCATEGID = ? and "
          "TRANSID = ?";

    double amt = 0.0;

    wxSQLite3Statement st = db->PrepareStatement(sql);
    st.Bind(1, categID);
    st.Bind(2, subcategID);
    st.Bind(3, transID);

    wxSQLite3ResultSet q1 = st.ExecuteQuery();
    if (q1.NextRow())
    {
        amt = q1.GetDouble(wxT("SPLITTRANSAMOUNT"));
    }

    st.Finalize();

    return amt;
}

double mmDBWrapper::getAmountForPayee(wxSQLite3Database* db, int payeeID,
        bool ignoreDate, wxDateTime dtBegin, wxDateTime dtEnd)
{
    static const std::string sql = 
    "select ca.TRANSCODE, "
           "ca.TRANSAMOUNT, " 
           "ca.STATUS, "
           "ca.TRANSDATE, "
           "cf.BASECONVRATE "
    "from CHECKINGACCOUNT_V1 ca " + joinCURRENCYFORMATS("cf", "ca.ACCOUNTID") +
   " where ca.PAYEEID = ?";

    double amt = 0.0;

    wxSQLite3Statement st = db->PrepareStatement(sql.c_str());
    st.Bind(1, payeeID);

    wxSQLite3ResultSet q1 = st.ExecuteQuery();

    while (q1.NextRow())
    {
        wxString code = q1.GetString(wxT("TRANSCODE"));
        double transAmount = q1.GetDouble(wxT("TRANSAMOUNT"));
        wxString transStatus = q1.GetString(wxT("STATUS"));
        wxString dateString = q1.GetString(wxT("TRANSDATE"));
        wxDateTime dtdt = mmGetStorageStringAsDate(dateString);

        double dbRate = q1.GetDouble(wxT("BASECONVRATE"), g_defBASECONVRATE);
        transAmount = transAmount * dbRate;

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
    st.Finalize();

    return amt;
}


 void mmDBWrapper::addBudgetYear(wxSQLite3Database* db, const wxString &year)
 {
	wxSQLite3Statement st = db->PrepareStatement("insert into BUDGETYEAR_V1 (BUDGETYEARNAME) values (?)");
	st.Bind(1, year);
    st.ExecuteUpdate();
	st.Finalize();
 }

 int mmDBWrapper::getBudgetYearID(wxSQLite3Database* db, const wxString &year)
 {
    int budgetYearID = -1;
    
	wxSQLite3Statement st = db->PrepareStatement("select BUDGETYEARID "
                                                 "from BUDGETYEAR_V1 "
                                                 "where BUDGETYEARNAME = ?"
                                                );

    st.Bind(1, year);
    
	wxSQLite3ResultSet q1 = st.ExecuteQuery();
    if (q1.NextRow())
    {
        budgetYearID = q1.GetInt(wxT("BUDGETYEARID"));
    }
    st.Finalize();
    
    return budgetYearID;
 }

 int mmDBWrapper::getTransIDByDate(wxSQLite3Database* db, const wxString &byDate, int accountID)
 {
	static const char sql[] = 
	"select max(cast(TRANSACTIONNUMBER as integer)) as MaxTransID "
	"from CHECKINGACCOUNT_V1 "
	"where TRANSDATE = ? and "
	      "ACCOUNTID = ?";

	int transID = 1;
    
	wxSQLite3Statement st = db->PrepareStatement(sql);
	st.Bind(1, byDate);
	st.Bind(2, accountID);

	wxSQLite3ResultSet q1 = st.ExecuteQuery();
    if (q1.NextRow())
    {
		transID = q1.GetInt(wxT("MaxTransID")) + 1;
    }
    st.Finalize();
    
	return transID;
 }

 wxString mmDBWrapper::getBudgetYearForID(wxSQLite3Database* db, int yearid)
 {
     wxString budgetYear;

	 wxSQLite3Statement st = db->PrepareStatement("select BUDGETYEARNAME "
                                                  "from BUDGETYEAR_V1 "
                                                  "where BUDGETYEARID = ?"
                                                 );

     st.Bind(1, yearid);
     
     wxSQLite3ResultSet q1 = st.ExecuteQuery();
     if (q1.NextRow())
     {
         budgetYear = q1.GetString(wxT("BUDGETYEARNAME"));
     }
     st.Finalize();
     
     return budgetYear;
}

void mmDBWrapper::updateYearForID(wxSQLite3Database* db, 
     const wxString& yearName, int yearid)
 {
	 wxSQLite3Statement st = db->PrepareStatement("update BUDGETYEAR_V1 "
		                                          "SET BUDGETYEARNAME = ? "
												  "WHERE BUDGETYEARID = ?"
												 );

	 st.Bind(1, yearName);	
	 st.Bind(2, yearid);	

     st.ExecuteUpdate();
	 st.Finalize();
}

bool mmDBWrapper::copyBudgetYear(wxSQLite3Database* db, int newYear, int baseYear)
{
	static const char sql[] = 
    "insert into BUDGETTABLE_V1 ( "
      "BUDGETYEARID, "
      "CATEGID, "
      "SUBCATEGID, "
      "PERIOD, "
      "AMOUNT "
    ") "
    "select ?, " 
           "CATEGID, "
	       "SUBCATEGID, "
		   "PERIOD, "
		   "AMOUNT "
	"from BUDGETTABLE_V1 "
	"WHERE BUDGETYEARID = ?";
	 
     if (newYear == baseYear)
         return false;
    
	 wxSQLite3Statement st = db->PrepareStatement(sql);
     st.Bind(1, newYear);
	 st.Bind(2, baseYear);

     st.ExecuteUpdate();
     st.Finalize();

     return true;
}

bool mmDBWrapper::deleteBudgetYear(wxSQLite3Database* db, const wxString& yearName)
 {
    int budgetYearID = getBudgetYearID(db, yearName);
    if (budgetYearID == -1)
        return false;

    static const char* sql[] = 
    {
        "delete from BUDGETTABLE_V1 where BUDGETYEARID = ?",
        "delete from BUDGETYEAR_V1  where BUDGETYEARID = ?",
        0
    };

    for (int i = 0; sql[i]; ++i)
    {
        wxSQLite3Statement st = db->PrepareStatement(sql[i]);
        st.Bind(1, budgetYearID);
        st.ExecuteUpdate();
        st.Finalize();
    }

    return true;
 }

 void mmDBWrapper::addBudgetEntry(wxSQLite3Database* db, int budgetYearID, 
     int categID, int subCategID, const wxString& period, double amount)
 {
	 static const char sql[] = 
	"insert into BUDGETTABLE_V1 ( "
	  "BUDGETYEARID, "
	  "CATEGID, "
      "SUBCATEGID, "
	  "PERIOD, "
	  "AMOUNT "
	") values ( "
	  "?, ?, ?, ?, ? "
	")";
	 
	 wxSQLite3Statement st = db->PrepareStatement(sql);
	
	int i = 0;
	st.Bind(++i, budgetYearID);
	st.Bind(++i, categID);
	st.Bind(++i, subCategID);
	st.Bind(++i, period);
	st.Bind(++i, amount);

    wxASSERT(st.GetParamCount() == i);

    st.ExecuteUpdate();
	st.Finalize();
 }

 void mmDBWrapper::deleteBudgetEntry(wxSQLite3Database* db, int budgetEntryID)
 {
    wxSQLite3Statement st = db->PrepareStatement("delete from BUDGETTABLE_V1 where BUDGETENTRYID = ?");
    st.Bind(1, budgetEntryID);
    
    st.ExecuteUpdate();
	st.Finalize();
 }
 
 void mmDBWrapper::updateBudgetEntry(wxSQLite3Database* db, 
        int budgetYearID, int categID, int subCategID,
                                const wxString& period, double amount)
{
	 static const char sql[] = 
	 "delete from BUDGETTABLE_V1 "
     "where BUDGETENTRYID in( select BUDGETENTRYID "
	                         "from BUDGETTABLE_V1 "
	                         "WHERE BUDGETYEARID = ? AND "
	                               "CATEGID = ? AND "
		                           "SUBCATEGID = ? "
                           ")";
	
	 wxSQLite3Statement st = db->PrepareStatement(sql);
	 st.Bind(1, budgetYearID);
	 st.Bind(2, categID);
	 st.Bind(3, subCategID);

     st.ExecuteUpdate();
     st.Finalize();

     addBudgetEntry(db, budgetYearID, categID, subCategID, period, amount);
 }

 bool mmDBWrapper::getBudgetEntry(wxSQLite3Database* db, int budgetYearID, 
                                int categID, int subCategID, wxString& period, double& amount)
 {
	static const char sql[] = 
	"select PERIOD, "
	       "AMOUNT "
	"from BUDGETTABLE_V1 "
	"WHERE BUDGETYEARID = ? AND "
	      "CATEGID = ? AND "
	      "SUBCATEGID = ?";
	 
	 bool found = false;

	 wxSQLite3Statement st = db->PrepareStatement(sql);
	 st.Bind(1, budgetYearID);
	 st.Bind(2, categID);
	 st.Bind(3, subCategID);

     wxSQLite3ResultSet q1 = st.ExecuteQuery();
    
     if (q1.NextRow())
     {
        found = true;
        period = q1.GetString(wxT("PERIOD"), wxT("None"));
        amount = q1.GetDouble(wxT("AMOUNT"));
     }

	 st.Finalize();

	 return found;
 }

 void mmDBWrapper::deleteBDSeries(wxSQLite3Database* db, int bdID)
 {
     static const char* sql[] = 
     {
        "delete from BILLSDEPOSITS_V1 where BDID = ?",
        "delete from BUDGETSPLITTRANSACTIONS_V1 where TRANSID = ?",
        0
     };
     
     for (int i = 0; sql[i]; ++i)
     {
         wxSQLite3Statement st = db->PrepareStatement(sql[i]);
         st.Bind(1, bdID);
         st.ExecuteUpdate();
         st.Finalize();
     }
 }

 void mmDBWrapper::completeBDInSeries(wxSQLite3Database* db, int bdID)
 {
	static const char sql[] = 
	"select NUMOCCURRENCES, "
		   "NEXTOCCURRENCEDATE, "
		   "REPEATS "
	"from BILLSDEPOSITS_V1 "
	"WHERE BDID = ?";
	 
     wxDateTime updateOccur = wxDateTime::Now();
     int numRepeats  = -1;

	 wxSQLite3Statement st = db->PrepareStatement(sql);
	 st.Bind(1, bdID);

	 wxSQLite3ResultSet q1 = st.ExecuteQuery();

     if (q1.NextRow())
     {
        numRepeats = q1.GetInt(wxT("NUMOCCURRENCES"));
        if (numRepeats != -1)
        {
            --numRepeats;
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
            updateOccur = dtno.Add(wxDateSpan::Months(3));
        }
        else if (repeats == 6)
        {
            updateOccur = dtno.Add(wxDateSpan::Months(6));
        }
        else if (repeats == 7)
        {
            updateOccur = dtno.Add(wxDateSpan::Year());
        }
        else if (repeats == 8)
        {
            updateOccur = dtno.Add(wxDateSpan::Months(4));
        }
        else if (repeats == 9)
        {
            updateOccur = dtno.Add(wxDateSpan::Weeks(4));
        }
        else if (repeats == 10)
        {
            updateOccur = dtno.Add(wxDateSpan::Days(1));
        }
     }

	 st.Finalize();
     
	{
		static const char sql[] = 
		"update BILLSDEPOSITS_V1 "
		"set NEXTOCCURRENCEDATE = ?, "
		    "NUMOCCURRENCES = ? "
		"where BDID = ?";

		wxSQLite3Statement st = db->PrepareStatement(sql);

		st.Bind(1, updateOccur.FormatISODate());
		st.Bind(2, numRepeats);
		st.Bind(3, bdID);

		st.ExecuteUpdate();
		st.Finalize();
	 }

	db->ExecuteUpdate("delete from BILLSDEPOSITS_V1 where NUMOCCURRENCES = 0");
 }

void mmDBWrapper::deleteStockInvestment(wxSQLite3Database* db, int stockID)
{
	wxSQLite3Statement st = db->PrepareStatement("delete from STOCK_V1 where STOCKID = ?");
    st.Bind(1, stockID);
    st.ExecuteUpdate();
	st.Finalize();
}

double mmDBWrapper::getStockInvestmentBalance(wxSQLite3Database* db, double& invested)
{
    static const char sql[] = 
    "select ACCOUNTID "
    "from ACCOUNTLIST_V1 "
    "where ACCOUNTTYPE = 'Investment'";

    double balance = 0.0;
    invested = 0.0;
    
    wxSQLite3ResultSet q1 = db->ExecuteQuery(sql);
    while (q1.NextRow())
    {
       double originalVal = 0; 
       balance += getStockInvestmentBalance(db, q1.GetInt(wxT("ACCOUNTID")), true, originalVal);
       invested += originalVal;
    }

    q1.Finalize();
    
    return balance;
}

double mmDBWrapper::getStockInvestmentBalance(wxSQLite3Database* db, int accountID, 
                                              bool convertToBase, double& originalVal)
{
   wxASSERT(accountID != -1);

   double balance = 0.0;
   originalVal = 0.0;

   {
	   wxSQLite3Statement st = db->PrepareStatement("select INITIALBAL "
                                                    "from ACCOUNTLIST_V1 "
                                                    "where ACCOUNTID = ?"
                                                   );

	   st.Bind(1, accountID);

       wxSQLite3ResultSet q1 = st.ExecuteQuery();
	   if (q1.NextRow())
       {
            balance = q1.GetDouble(wxT("INITIALBAL"));
       }
	   
       st.Finalize();
   }

	static const char sql[] = 
	"select VALUE, "
		   "NUMSHARES, "
		   "PURCHASEPRICE, "
		   "COMMISSION "
	"from STOCK_V1 "
	"where HELDAT = ?";

   wxSQLite3Statement st = db->PrepareStatement(sql);
   st.Bind(1, accountID);
   
   wxSQLite3ResultSet q1 = st.ExecuteQuery();
   
   while (q1.NextRow())
   {
      double value = q1.GetDouble(wxT("VALUE"));
      double numShares = q1.GetDouble(wxT("NUMSHARES"));
      double purchasePrice = q1.GetDouble(wxT("PURCHASEPRICE"));
      double commission = q1.GetDouble(wxT("COMMISSION"));

      double investedAmt = (numShares * purchasePrice) + commission;
      originalVal += investedAmt;
      balance += value;
   }

   st.Finalize();

   double convRate = g_defBASECONVRATE;
   if (convertToBase)
       convRate = getCurrencyBaseConvRate(db, accountID);
   
   originalVal = originalVal * convRate;
   return balance * convRate;
}

void mmDBWrapper::deleteAsset(wxSQLite3Database* db, int assetID)
{
	wxSQLite3Statement st = db->PrepareStatement("delete from ASSETS_V1 where ASSETID = ?");
	st.Bind(1, assetID);
	st.ExecuteUpdate();
	st.Finalize();
}

double mmDBWrapper::getAssetBalance(wxSQLite3Database* db)
{
	double balance = 0.0;

    wxSQLite3ResultSet q1 = db->ExecuteQuery("select ASSETID from ASSETS_V1");

    while (q1.NextRow())
    {
        int assetID = q1.GetInt(wxT("ASSETID"));
		balance += getAssetValue(db, assetID);
    }
    q1.Finalize();

    return balance;
}

double mmDBWrapper::getAssetValue(wxSQLite3Database* db, int assetID)
{
	static const char sql[] = 
	"select VALUECHANGE, "
		   "VALUE, "
		   "VALUECHANGERATE, "
		   "STARTDATE "
	"from ASSETS_V1 "
	"where ASSETID = ?";

	double assetValue = 0.0;
	
	wxSQLite3Statement st = db->PrepareStatement(sql);
    st.Bind(1, assetID);
    
	wxSQLite3ResultSet q1 = st.ExecuteQuery();
    
	if (q1.NextRow())
    {
		wxString valChange = q1.GetString(wxT("VALUECHANGE"));
		double value = q1.GetDouble(wxT("VALUE"));
		double valueChangeRate = q1.GetDouble(wxT("VALUECHANGERATE"));
        wxString startDateStr = q1.GetString(wxT("STARTDATE"));
        wxDateTime startDate = mmGetStorageStringAsDate(startDateStr);
		wxDateTime todayDate = wxDateTime::Now();

		if (valChange == wxT("None"))
			assetValue = value;
		else if (valChange == wxT("Appreciates"))
		{
			if (todayDate > startDate)
			{
				int numYears = todayDate.GetYear() - startDate.GetYear();
				if (numYears > 0)
				{
					double appreciation = numYears * valueChangeRate * value / 100;
					assetValue = value + appreciation;
				}
                else
                     assetValue = value;
			}
			else
				assetValue = value;
		}
		else if (valChange == wxT("Depreciates"))
		{
			if (todayDate > startDate)
			{
				int numYears = todayDate.GetYear() - startDate.GetYear();
				if (numYears > 0)
				{
					double depreciation = numYears * valueChangeRate * value / 100;
					assetValue = value - depreciation;
				}
                else
                    assetValue = value;
			}
			else
				assetValue = value;
		}
		else
		{
			wxASSERT(false);
		}
    }
    
	st.Finalize();
    
	return assetValue;
}

wxString mmDBWrapper::getAccountType(wxSQLite3Database* db_, int accountID)
{
	wxString acctType;

	wxSQLite3Statement st = db_->PrepareStatement("select ACCOUNTTYPE "
                                                  "from ACCOUNTLIST_V1 "
                                                  "where ACCOUNTID = ?"
                                                 );
	
    st.Bind(1, accountID);
	
	wxSQLite3ResultSet rs = st.ExecuteQuery();
	if (rs.NextRow())
	{
		acctType = rs.GetString(wxT("ACCOUNTTYPE"));
	}

	st.Finalize();

	return acctType;
}

void mmDBWrapper::removeSplitsForAccount(wxSQLite3Database* db, int accountID)
{
    static const char sql[] = 
    "delete from SPLITTRANSACTIONS_V1 "
    "where TRANSID in( SELECT TRANSID "
                      "from CHECKINGACCOUNT_V1 "
                      "where ACCOUNTID = ? OR TOACCOUNTID = ? "
                    ")";

	wxSQLite3Statement st = db->PrepareStatement(sql);
	st.Bind(1, accountID);
	st.Bind(2, accountID);

    st.ExecuteUpdate();
	st.Finalize();

    // --

    static const char sql2[] = 
    "delete from BUDGETSPLITTRANSACTIONS_V1 "
    "where TRANSID in( SELECT BDID "
                      "from BILLSDEPOSITS_V1 "
                      "where ACCOUNTID = ? OR TOACCOUNTID = ? "
                    ")";

	st = db->PrepareStatement(sql2);
    st.Bind(1, accountID);
    st.Bind(2, accountID);

    st.ExecuteUpdate();
	st.Finalize();
}
