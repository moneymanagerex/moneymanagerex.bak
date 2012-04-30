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
#include "mmex_db_view.h"
//----------------------------------------------------------------------------
#include <boost/scoped_ptr.hpp>
#include <string>
//----------------------------------------------------------------------------
#include <sqlite3.h>
//----------------------------------------------------------------------------

namespace
{

const double g_defBASECONVRATE = 1.0;
const char g_BaseCurrencyIdName[] = "BASECURRENCYID";

//----------------------------------------------------------------------------
void loadCurrencies(wxSQLite3Database* db)
{
    try{
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

        wxString fName = mmex::getPathResource(mmex::CURRENCY_DB_SEED);
        wxASSERT(wxFileName::FileExists(fName));

        wxSQLite3Database currencies;
        currencies.Open(fName, wxGetEmptyString(), WXSQLITE_OPEN_READONLY);

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
    } catch(const wxSQLite3Exception& e)
    {
        wxLogDebug(wxT("Database::loadCurrencies: Exception"), e.GetMessage().c_str());
        wxLogError(wxT("insert into CURRENCYFORMATS_V1. ") + wxString::Format(_("Error: %s"), e.GetMessage().c_str()));
    }
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
    try{
        wxASSERT(db);

        wxSQLite3Statement st_cat = db->PrepareStatement("insert into CATEGORY_V1 (CATEGNAME) VALUES(?)");

        wxSQLite3Statement st_subcat = db->PrepareStatement("insert into SUBCATEGORY_V1 "
          "( SUBCATEGNAME, CATEGID ) "
          "values (:name, :id)"
        );

        const wxChar* BillsCategories[] = {_("Telephone"), _("Electricity"), _("Gas"), _("Internet"), _("Rent"), _("Cable TV"), _("Water"), 0};
        insertCategoryTree(db, st_cat, st_subcat, _("Bills"), BillsCategories);

        const wxChar* FoodCategories[] = {_("Groceries"), _("Dining out"), 0};
        insertCategoryTree(db, st_cat, st_subcat, _("Food"), FoodCategories);

        const wxChar* LeisureCategories[] = {_("Movies"), _("Video Rental"), _("Magazines"), 0};
        insertCategoryTree(db, st_cat, st_subcat, _("Leisure"), LeisureCategories);

        const wxChar* AutomobileCategories[] = {_("Maintenance"), _("Gas"), _("Parking"), _("Registration"), 0};
        insertCategoryTree(db, st_cat, st_subcat, _("Automobile"), AutomobileCategories);

        const wxChar* EducationCategories[] = {_("Books"), _("Tuition"), _("Others"), 0};
        insertCategoryTree(db, st_cat, st_subcat, _("Education"), EducationCategories);

        const wxChar* HomeneedsCategories[] = {_("Clothing"), _("Furnishing"), _("Others"), 0};
        insertCategoryTree(db, st_cat, st_subcat, _("Homeneeds"), HomeneedsCategories);

        const wxChar* HealthcareCategories[] = {_("Health"), _("Dental"), _("Eyecare"), _("Physician"), _("Prescriptions"), 0};
        insertCategoryTree(db, st_cat, st_subcat, _("Healthcare"), HealthcareCategories);

        const wxChar* InsuranceCategories[] = {_("Auto"), _("Life"), _("Home"), _("Health"), 0};
        insertCategoryTree(db, st_cat, st_subcat, _("Insurance"), InsuranceCategories);

        const wxChar* VacationCategories[] = {_("Travel"), _("Lodging"), _("Sightseeing"), 0};
        insertCategoryTree(db, st_cat, st_subcat, _("Vacation"), VacationCategories);

        const wxChar* TaxesCategories[] = {_("Income Tax"), _("House Tax"), _("Water Tax"), _("Others"), 0};
        insertCategoryTree(db, st_cat, st_subcat, _("Taxes"), TaxesCategories);

        insertCategoryTree(db, st_cat, st_subcat, _("Miscellaneous"), 0);
        insertCategoryTree(db, st_cat, st_subcat, _("Gifts"), 0);

        const wxChar* IncomeCategories[] = {_("Salary"), _("Reimbursement/Refunds"),  _("Investment Income"), 0};
        insertCategoryTree(db, st_cat, st_subcat, _("Income"), IncomeCategories);

        insertCategoryTree(db, st_cat, st_subcat, _("Other Income"), 0);
        insertCategoryTree(db, st_cat, st_subcat, _("Other Expenses"), 0);
        insertCategoryTree(db, st_cat, st_subcat, _("Transfer"), 0);

        // cleanup
        st_subcat.Finalize();
        st_cat.Finalize();
    } catch(const wxSQLite3Exception& e)
    {
        wxLogDebug(wxT("Database::createDefaultCategories: Exception"), e.GetMessage().c_str());
        wxLogError(wxT("insert into [SUB]CATEGORY_V1. ") + wxString::Format(_("Error: %s"), e.GetMessage().c_str()));
    }
}
//----------------------------------------------------------------------------

} // namespace

//----------------------------------------------------------------------------

void mmDBWrapper::createInfoV1Table(wxSQLite3Database* db)
{
    try{
        /* We always create new INFOTABLE_V1 */
        if (!db->TableExists(wxT("INFOTABLE_V1")))
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
    } catch(const wxSQLite3Exception& e)
    {
        wxLogDebug(wxT("Database::createInfoV1Table: Exception"), e.GetMessage().c_str());
        wxLogError(wxT("create table INFOTABLE_V1. ") + wxString::Format(_("Error: %s"), e.GetMessage().c_str()));
    }
}

void mmDBWrapper::createCurrencyV1Table(wxSQLite3Database* db)
{
    try{
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
    } catch(const wxSQLite3Exception& e)
    {
        wxLogDebug(wxT("Database::createCurrencyV1Table: Exception"), e.GetMessage().c_str());
        wxLogError(wxT("create CURRENCYFORMATS_V1. ") + wxString::Format(_("Error: %s"), e.GetMessage().c_str()));
    }
}


void mmDBWrapper::createBudgetingV1Table(wxSQLite3Database* db)
{
    BUDGETYEAR_V1.ensure(db);
    BUDGETTABLE_V1.ensure(db);
}

void mmDBWrapper::createStockV1Table(wxSQLite3Database* db)
{
    STOCK_V1.ensure(db);
}

void mmDBWrapper::createAssetsV1Table(wxSQLite3Database* db)
{
    ASSETS_V1.ensure(db);
}

void mmDBWrapper::createBillsDepositsV1Table(wxSQLite3Database* db)
{
    BILLSDEPOSITS_V1.ensure(db);
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
    ACCOUNTLIST_V1.ensure(db);
}

void mmDBWrapper::createCheckingAccountV1Table(wxSQLite3Database* db)
{
    CHECKINGACCOUNT_V1.ensure(db);
}

void mmDBWrapper::createSplitTransactionsV1Table(wxSQLite3Database* db)
{
    SPLITTRANSACTIONS_V1.ensure(db);
    BUDGETSPLITTRANSACTIONS_V1.ensure(db);
}

void mmDBWrapper::createPayeeV1Table(wxSQLite3Database* db)
{
    PAYEE_V1.ensure(db);
}

void mmDBWrapper::createCategoryV1Table(wxSQLite3Database* db)
{
    CATEGORY_V1.ensure(db);
    SUBCATEGORY_V1.ensure(db);
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
    try{
    static const char view_name[] = "ALLDATA";

    static const char sql[] =
    "create  view alldata as "
    "select \n"
    "       CANS.TransID as ID, CANS.TransCode as TransactionType, \n"
    "       date(CANS.TransDate, 'localtime') as Date, \n"
    "       d.userdate as UserDate \n"
    "       ,coalesce(CAT.CategName, SCAT.CategName) as Category, \n"
    "       coalesce(SUBCAT.SUBCategName, SSCAT.SUBCategName, '') as Subcategory, \n"
    "       ROUND((case CANS.TRANSCODE when 'Withdrawal' then -1 else 1 end)*(case CANS.CATEGID when -1 then st.splittransamount else CANS.TRANSAMOUNT end),2) as Amount, \n"
    "       cf.currency_symbol as currency, \n"
    "       CANS.Status as Status, CANS.NOTES as Notes, \n"
    "       cf.BaseConvRate as BaseConvRate, \n"
    "       FROMACC.CurrencyID as CurrencyID, \n"
    "       FROMACC.AccountName as AccountName,  FROMACC.AccountID as AccountID, \n"
    "       ifnull (TOACC.AccountName,'') as ToAccountName, \n"
    "       ifnull (TOACC.ACCOUNTId,-1) as ToAccountID, CANS.ToTransAmount ToTransAmount, \n"
    "       ifnull (TOACC.CURRENCYID, -1) as ToCurrencyID, \n"
    "       (case ifnull(CANS.CATEGID,-1) when -1 then 1 else 0 end) as Splitted, \n"
    "       ifnull(CAT.CategId,st.CategId) as CategID, \n"
    "       ifnull (ifnull(SUBCAT.SubCategID,st.subCategId),-1) as SubCategID, \n"
    "       ifnull (PAYEE.PayeeName,'') as Payee, \n"
    "       ifnull (PAYEE.PayeeID,-1) as PayeeID, \n"
    "            CANS.TRANSACTIONNUMBER as TransactionNumber, \n"
    "            d.year as Year, \n"
    "            d.month as Month, \n"
    "            d.day as Day, \n"
    "            d.finyear as FinYear \n"
    "            from  CHECKINGACCOUNT_V1 CANS \n"
    "            left join CATEGORY_V1 CAT on CAT.CATEGID = CANS.CATEGID \n"
    "            left join SUBCATEGORY_V1 SUBCAT on SUBCAT.SUBCATEGID = CANS.SUBCATEGID and SUBCAT.CATEGID = CANS.CATEGID \n"
    "            left join PAYEE_V1 PAYEE on PAYEE.PAYEEID = CANS.PAYEEID \n"
    "            left join ACCOUNTLIST_V1 FROMACC on FROMACC.ACCOUNTID = CANS.ACCOUNTID \n"
    "            left join ACCOUNTLIST_V1 TOACC on TOACC.ACCOUNTID = CANS.TOACCOUNTID \n"
    "            left join splittransactions_v1 st on CANS.transid=st.transid \n"
    "            left join CATEGORY_V1 SCAT on SCAT.CATEGID = st.CATEGID and CANS.TransId=st.transid \n"
    "            left join SUBCATEGORY_V1 SSCAT on SSCAT.SUBCATEGID = st.SUBCATEGID and SSCAT.CATEGID = st.CATEGID and CANS.TransId=st.transid \n"
    "            left join currencyformats_v1 cf on cf.currencyid=FROMACC .currencyid \n"
    "           left join ( select transid as id , date(transdate, 'localtime') as transdate \n"
    ",round (strftime('%d', transdate, 'localtime'))  as day \n"
    ",round (strftime('%m', transdate, 'localtime'))  as month \n"
    ",round (strftime('%Y', transdate, 'localtime'))  as year \n"
    ",round(strftime('%Y', transdate, 'localtime' ,'start of month', ((case when fd.infovalue<=round(strftime('%d', transdate , 'localtime')) then 1  else 0 end)-fm.infovalue)||' month')) as finyear \n"
    ",ifnull (ifnull (strftime(df.infovalue, TransDate, 'localtime'), \n"
    "       (strftime(replace (df.infovalue, '%y', SubStr (strftime('%Y',TransDate, 'localtime'),3,2)),TransDate, 'localtime')) \n"
    "       ), date(TransDate, 'localtime')) as UserDate \n"
    "from  CHECKINGACCOUNT_V1 \n"
    "left join infotable_v1 df on df.infoname='DATEFORMAT' \n"
    "left join infotable_v1 fm on fm.infoname='FINANCIAL_YEAR_START_MONTH' \n"
    "left join infotable_v1 fd on fd.infoname='FINANCIAL_YEAR_START_DAY') d on d.id=CANS.TRANSID \n"
    "order by CANS.transid";

    bool exists = ViewExists(db, view_name);

    if (!exists) {
        db->ExecuteUpdate(sql);
        exists = ViewExists(db, view_name);
        wxASSERT(exists);
    }
    } catch(const wxSQLite3Exception& e)
    {
        wxLogDebug(wxT("Database::createAllDataView: Exception"), e.GetMessage().c_str());
        wxLogError(wxT("create AllData view. ") + wxString::Format(_("Error: %s"), e.GetMessage().c_str()));
    }
}

void removeCruft(wxSQLite3Database* db)
{
    try{
        {
            if (db->TableExists(wxT("SPLITTRANSACTIONS_V1")))
            {
                if (db->ExecuteScalar("SELECT COUNT(*) FROM SPLITTRANSACTIONS_V1 WHERE SPLITTRANSACTIONS_V1.TRANSID NOT IN (SELECT TRANSID FROM CHECKINGACCOUNT_V1)") > 0)
                    db->ExecuteUpdate("DELETE FROM SPLITTRANSACTIONS_V1 WHERE SPLITTRANSACTIONS_V1.TRANSID NOT IN (SELECT TRANSID FROM CHECKINGACCOUNT_V1)");
            }
        }

        {
            if (db->TableExists(wxT("BUDGETSPLITTRANSACTIONS_V1")))
            {
                if (db->ExecuteScalar("SELECT COUNT(*) FROM BUDGETSPLITTRANSACTIONS_V1 WHERE BUDGETSPLITTRANSACTIONS_V1.TRANSID NOT IN (SELECT BDID FROM BILLSDEPOSITS_V1)") > 0)
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
    } catch(const wxSQLite3Exception& e)
    {
        wxLogDebug(wxT("Database::removeCruft: Exception"), e.GetMessage().c_str());
        wxLogError(wxT("Remove Cruft. ") + wxString::Format(_("Error: %s"), e.GetMessage().c_str()));
    }
}

/*
    This routine sets up a new DB as well as update an old one
*/

void mmDBWrapper::initDB(wxSQLite3Database* db, wxProgressDialog* pgd)
{
    /* Create INFOTABLE_V1 Tables */
    db->Begin();

    createInfoV1Table(db);
    if (pgd) pgd->Update(10);

    /* Create Currency Settings */
    createCurrencyV1Table(db);
    if (pgd) pgd->Update(20);

    /* Create ACCOUNTLIST_V1 Tables */
    createAccountListV1Table(db);
    if (pgd) pgd->Update(30);

    /* Create CHECKINGACCOUNT_V1 Tables */
    createCheckingAccountV1Table(db);
    if (pgd) pgd->Update(40);

    /* Create PAYEE_V1 Tables */
    createPayeeV1Table(db);
    if (pgd) pgd->Update(50);

    /* Create CATEGORY_V1 Tables */
    createCategoryV1Table(db);
    if (pgd) pgd->Update(60);

    /* Create Budgeting_V1 Tables */
    createBudgetingV1Table(db);
    if (pgd) pgd->Update(75);

    /* Create Bills & Deposits V1 Table */
    createBillsDepositsV1Table(db);
    if (pgd) pgd->Update(80);

    /* Create Stock V1 Table */
    createStockV1Table(db);
    if (pgd) pgd->Update(85);

    /* Create Asset V1 Table */
    createAssetsV1Table(db);
    if (pgd) pgd->Update(90);

    /* Create SplitTransactions V1 Table */
    createSplitTransactionsV1Table(db);
    if (pgd) pgd->Update(95);

    /* Create AllData view */
    createAllDataView(db);
    if (pgd) pgd->Update(99);

    /* Remove Any cruft */
    removeCruft(db);
    db->Commit();
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
    try{
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
    } catch(const wxSQLite3Exception& e)
    {
        wxLogDebug(wxT("Database::setBaseCurrencySettings: Exception"), e.GetMessage().c_str());
        wxLogError(wxT("insert or update INFOTABLE_V1. ") + wxString::Format(_("Error: %s"), e.GetMessage().c_str()));
    }
}

void mmDBWrapper::loadBaseCurrencySettings(wxSQLite3Database* db)
{
    int currencyID = getBaseCurrencySettings(db);

    if (currencyID != -1)
        loadSettings(db, currencyID);
    else
        mmex::CurrencyFormatter::instance().loadDefaultSettings();
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

    if (q1.NextRow()) {

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

        if (!dec.empty()) {
            decChar = dec.GetChar(0);
        }

        if (!grp.empty()) {
            grpChar = grp.GetChar(0);
        }

        mmex::CurrencyFormatter::instance().loadSettings(pfxSymbol, sfxSymbol, decChar, grpChar, unit, cent, scaleDl);

    } else {
        wxASSERT(true);
    }

    st.Finalize();
}

bool mmDBWrapper::deleteCategoryWithConstraints(wxSQLite3Database* db, int categID)
{
    try{
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

        mmOptions::instance().databaseUpdated_ = true;
        return true;
    } catch(const wxSQLite3Exception& e)
    {
        wxLogDebug(wxT("Database::deleteCategoryWithConstraints: Exception"), e.GetMessage().c_str());
        wxLogError(wxT("Delete Category with Constraints. ") + wxString::Format(_("Error: %s"), e.GetMessage().c_str()));
        return false;
    }
}

bool mmDBWrapper::deleteSubCategoryWithConstraints(wxSQLite3Database* db, int categID, int subcategID)
{
    try {
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

        mmOptions::instance().databaseUpdated_ = true;
        return true;
    } catch(const wxSQLite3Exception& e)
    {
        wxLogDebug(wxT("Database::deleteSubCategoryWithConstraints: Exception"), e.GetMessage().c_str());
        wxLogError(wxT("Delete SubCategory with Constraints. ") + wxString::Format(_("Error: %s"), e.GetMessage().c_str()));
        return false;
    }
}

bool mmDBWrapper::updateCategory(wxSQLite3Database* db, int categID,
                                 int subcategID, const wxString &newName)
{
    try {
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

        mmOptions::instance().databaseUpdated_ = true;
    } catch(const wxSQLite3Exception& e)
    {
        wxLogDebug(wxT("Database::updateCategory: Exception"), e.GetMessage().c_str());
        wxLogError(wxT("update [SUB]CATEGORY_V1. ") + wxString::Format(_("Error: %s"), e.GetMessage().c_str()));
    }
    return true;

}

bool mmDBWrapper::addCategory(wxSQLite3Database* db, const wxString &newName)
{
    int rows_affected = 0;

    try {
        wxSQLite3Statement st = db->PrepareStatement("insert into CATEGORY_V1 (CATEGNAME) values (?)");
        st.Bind(1, newName);
        rows_affected = st.ExecuteUpdate();
        st.Finalize();
        mmOptions::instance().databaseUpdated_ = true;

    } catch(const wxSQLite3Exception& e)
    {
        wxLogDebug(wxT("Database::addCategory: Exception"), e.GetMessage().c_str());
        wxLogError(wxT("insert into CATEGORY_V1. ") + wxString::Format(_("Error: %s"), e.GetMessage().c_str()));
    }
    return rows_affected == 1;
}

bool mmDBWrapper::addSubCategory(wxSQLite3Database* db, int categID, const wxString &newName)
{
    static const char sql[] =
    "insert into SUBCATEGORY_V1 (SUBCATEGNAME, CATEGID) values (?, ?)";

    int rows_affected = 0;

    try {
    wxSQLite3Statement st = db->PrepareStatement(sql);
    st.Bind(1, newName);
    st.Bind(2, categID);

    rows_affected = st.ExecuteUpdate();
    st.Finalize();

    } catch(const wxSQLite3Exception& e)
    {
        wxLogDebug(wxT("Database::addSubCategory: Exception"), e.GetMessage().c_str());
        wxLogError(wxT("insert into SUBCATEGORY_V1. ") + wxString::Format(_("Error: %s"), e.GetMessage().c_str()));
    }
    mmOptions::instance().databaseUpdated_ = true;
    return rows_affected == 1;
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
    try {
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
        mmOptions::instance().databaseUpdated_ = true;
        return true;
    } catch(const wxSQLite3Exception& e)
    {
        wxLogDebug(wxT("Database::deleteTransaction: Exception"), e.GetMessage().c_str());
        wxLogError(wxT("Delete Transaction. ") + wxString::Format(_("Error: %s"), e.GetMessage().c_str()));
        return false;
    }
}

bool mmDBWrapper::updatePayee(wxSQLite3Database* db, const wxString& payeeName,
                              int payeeID, int categID, int subcategID)
{
    try {
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
        mmOptions::instance().databaseUpdated_ = true;

    } catch(const wxSQLite3Exception& e)
    {
        wxLogDebug(wxT("Database::updatePayee: Exception"), e.GetMessage().c_str());
        wxLogError(wxT("update PAYEE_V1. ") + wxString::Format(_("Error: %s"), e.GetMessage().c_str()));
        return false;
    }

    return true;
}

bool mmDBWrapper::deletePayeeWithConstraints(wxSQLite3Database* db, int payeeID)
{
    try {
        char sql[] =
        "select 1 "
        "from CHECKINGACCOUNT_V1 "
        "where transcode <> 'Transfer' " //for transfer transactions payee id should be null. if not null jast ignore it
        "and PAYEEID = ? "
        "limit 1";
        wxSQLite3Statement st = db->PrepareStatement(sql);
        st.Bind(1, payeeID);
        wxSQLite3ResultSet q1 = st.ExecuteQuery();

        bool found = q1.NextRow();
        st.Finalize();

        if (found)
            return false;

        // --
        st = db->PrepareStatement("delete from PAYEE_V1 where PAYEEID = ?");
        st.Bind(1, payeeID);

        st.ExecuteUpdate();
        st.Finalize();
        mmOptions::instance().databaseUpdated_ = true;

    } catch(const wxSQLite3Exception& e) {
        wxLogDebug(wxT("Database::deletePayeeWithConstraints: Exception"), e.GetMessage().c_str());
        wxLogError(wxT("delete from PAYEE_V1. ") + wxString::Format(_("Error: %s"), e.GetMessage().c_str()));
        return false;
    }

    return true;
}

wxString mmDBWrapper::getPayee(wxSQLite3Database* db, int payeeID, int& categID, int& subcategID)
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
        payeeName  = q1.GetString(wxT("PAYEENAME"));
        categID    = q1.GetInt(wxT("CATEGID"));
        subcategID = q1.GetInt(wxT("SUBCATEGID"));
    }
    st.Finalize();

    return payeeName;
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
    bool ok = false;
    try
    {
        ok = inidb->TableExists(wxT("SETTING_V1"));
    }
    catch (const wxSQLite3Exception& e)
    {
        wxLogDebug(wxT("Database::TableExists: Exception"), e.GetMessage().c_str());
        wxLogError(wxString::Format(_("Error: %s"), e.GetMessage().c_str()));
    }

    if (ok) return;

    // mmexini.db3 database not set correctly so recreate it.
    static const char sql[] =
        "create table SETTING_V1"
        "(SETTINGID integer not null primary key, "
          "SETTINGNAME TEXT NOT NULL, "
          "SETTINGVALUE TEXT)";
    try
    {
        inidb->ExecuteUpdate(sql);
    }
    catch (const wxSQLite3Exception& e)
    {
        wxLogDebug(wxT("Database::Create table SETTING_V1: Exception: %s"), e.GetMessage().c_str());
        wxLogError(wxT("create table SETTING_V1. ") + wxString::Format(_("Error: %s"), e.GetMessage().c_str()));
    }

    try
    {
        ok = inidb->TableExists(wxT("SETTING_V1"));
    }
    catch (const wxSQLite3Exception& e)
    {
        wxLogDebug(wxT("Database::TableExists: Exception: %s"), e.GetMessage().c_str());
        wxLogError(wxString::Format(_("Error: %s"), e.GetMessage().c_str()));
    }

    wxASSERT(ok);
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
    try {
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
    } catch(const wxSQLite3Exception& e)
    {
        wxString error_str;
        wxLogDebug(wxT("Database::setINISettingValue: Exception"), e.GetMessage().c_str());
        error_str << wxT("Set INI Setting Value. ");
        error_str << wxString::Format(wxT("%s = %s \t"), settingName.c_str(), settingValue.c_str());
        error_str << wxString::Format(_("Error: %s"), e.GetMessage().c_str());
        wxLogError(error_str);
    }
}

bool mmDBWrapper::updateTransactionWithStatus(wxSQLite3Database &db, int transID,
                                              const wxString& status)
{
    try {
    wxSQLite3Statement st = db.PrepareStatement("update CHECKINGACCOUNT_V1 set STATUS=? where TRANSID=?");
    st.Bind(1, status);
    st.Bind(2, transID);

    st.ExecuteUpdate();
    st.Finalize();
    mmOptions::instance().databaseUpdated_ = true;

    } catch(const wxSQLite3Exception& e)
    {
        wxLogDebug(wxT("Database::updateTransactionWithStatus: Exception"), e.GetMessage().c_str());
        wxLogError(wxT("Update Transaction With Status. ") + wxString::Format(_("Error: %s"), e.GetMessage().c_str()));
        return false;
    }
    return true;
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
    try {
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
    } catch(const wxSQLite3Exception& e)
    {
        wxLogDebug(wxT("Function::setInfoSettingValue: Exception"), e.GetMessage().c_str());
        wxLogError(wxString::Format(_("Set Info Setting Value %s = %s. Error: %s"), settingName.c_str(), settingValue.c_str(), e.GetMessage().c_str()));
    }
}


wxArrayString mmDBWrapper::filterPayees(wxSQLite3Database* db, const wxString& patt)
{
    wxSQLite3Statement st = db->PrepareStatement("select PAYEENAME from PAYEE_V1 where PAYEENAME LIKE ? ORDER BY PAYEENAME");
    st.Bind(1, patt + wxT("%"));
    wxSQLite3ResultSet q1 = st.ExecuteQuery();

    wxArrayString flist;

    while (q1.NextRow())
    {
        flist.Add(q1.GetString(wxT("PAYEENAME")));
    }
    st.Finalize();

    return flist;
}


void mmDBWrapper::addBudgetYear(wxSQLite3Database* db, const wxString &year)
{
    try {
        wxSQLite3Statement st = db->PrepareStatement("insert into BUDGETYEAR_V1 (BUDGETYEARNAME) values (?)");
        st.Bind(1, year);
        st.ExecuteUpdate();
        st.Finalize();
        mmOptions::instance().databaseUpdated_ = true;

    } catch(const wxSQLite3Exception& e)
    {
        wxLogDebug(wxT("Function::addBudgetYear: Exception"), e.GetMessage().c_str());
        wxLogError(wxT("Add Budget Year. ") + wxString::Format(_("Error: %s"), e.GetMessage().c_str()));
    }
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

int mmDBWrapper::getTransactionNumber(wxSQLite3Database* db, int accountID)
{
    static const char sql[] =
    "select ifnull(max(cast(TRANSACTIONNUMBER as integer)),0)+1 as MaxTransID "
    "from CHECKINGACCOUNT_V1 "
    "where ACCOUNTID = ? ";

    int transID = 1;

    wxSQLite3Statement st = db->PrepareStatement(sql);
    st.Bind(1, accountID);

    wxSQLite3ResultSet q1 = st.ExecuteQuery();
    if (q1.NextRow())
    {
        transID = q1.GetInt(wxT("MaxTransID"));
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
    try {
        wxSQLite3Statement st = db->PrepareStatement("update BUDGETYEAR_V1 "
                                                      "SET BUDGETYEARNAME = ? "
                                                      "WHERE BUDGETYEARID = ?"
                                                     );

        st.Bind(1, yearName);
        st.Bind(2, yearid);

        st.ExecuteUpdate();
        st.Finalize();
        mmOptions::instance().databaseUpdated_ = true;

    } catch(const wxSQLite3Exception& e)
    {
        wxLogDebug(wxT("Function::updateYearForID: Exception"), e.GetMessage().c_str());
        wxLogError(wxT("Update Year For ID. ") + wxString::Format(_("Error: %s"), e.GetMessage().c_str()));
    }
}

bool mmDBWrapper::copyBudgetYear(wxSQLite3Database* db, int newYear, int baseYear)
{
    try {
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
         mmOptions::instance().databaseUpdated_ = true;

    } catch(const wxSQLite3Exception& e)
    {
        wxLogDebug(wxT("Function::copyBudgetYear: Exception"), e.GetMessage().c_str());
        wxLogError(wxT("Copy Budget Year. ") + wxString::Format(_("Error: %s"), e.GetMessage().c_str()));
    }
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

    try {
        for (int i = 0; sql[i]; ++i)
        {
            wxSQLite3Statement st = db->PrepareStatement(sql[i]);
            st.Bind(1, budgetYearID);
            st.ExecuteUpdate();
            st.Finalize();
        }
        mmOptions::instance().databaseUpdated_ = true;

    } catch(const wxSQLite3Exception& e)
    {
        wxLogDebug(wxT("Function::deleteBudgetYear: Exception"), e.GetMessage().c_str());
        wxLogError(wxT("Delete Budget Year. ") + wxString::Format(_("Error: %s"), e.GetMessage().c_str()));
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

    try {
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
        mmOptions::instance().databaseUpdated_ = true;

    } catch(const wxSQLite3Exception& e)
    {
        wxLogDebug(wxT("Function::addBudgetEntry: Exception"), e.GetMessage().c_str());
        wxLogError(wxT("Add Budget Entry. ") + wxString::Format(_("Error: %s"), e.GetMessage().c_str()));
    }
}

void mmDBWrapper::deleteBudgetEntry(wxSQLite3Database* db, int budgetEntryID)
{
    try {
        wxSQLite3Statement st = db->PrepareStatement("delete from BUDGETTABLE_V1 where BUDGETENTRYID = ?");
        st.Bind(1, budgetEntryID);

        st.ExecuteUpdate();
        st.Finalize();
        mmOptions::instance().databaseUpdated_ = true;

    } catch(const wxSQLite3Exception& e)
    {
        wxLogDebug(wxT("Function::deleteBudgetEntry: Exception"), e.GetMessage().c_str());
        wxLogError(wxT("delete from BUDGETTABLE_V1. ") + wxString::Format(_("Error: %s"), e.GetMessage().c_str()));
    }
}

void mmDBWrapper::updateBudgetEntry(wxSQLite3Database* db,
        int budgetYearID, int categID, int subCategID,
                                const wxString& period, double amount)
{
    try {
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
    } catch(const wxSQLite3Exception& e)
    {
        wxLogDebug(wxT("Function::updateBudgetEntry: Exception"), e.GetMessage().c_str());
        wxLogError(wxT("delete from BUDGETTABLE_V1. ") + wxString::Format(_("Error: %s"), e.GetMessage().c_str()));
    }
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
    try {
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
        mmOptions::instance().databaseUpdated_ = true;

    } catch(const wxSQLite3Exception& e)
    {
        wxLogDebug(wxT("Function::deleteBDSeries: Exception"), e.GetMessage().c_str());
        wxLogError(wxString::Format(_("Error: %s"), e.GetMessage().c_str()));
    }
}

void mmDBWrapper::completeBDInSeries(wxSQLite3Database* db, int bdID)
{
    try {
        static const char sql[] =
        "select NUMOCCURRENCES, "
               "NEXTOCCURRENCEDATE, "
               "REPEATS "
        "from BILLSDEPOSITS_V1 "
        "WHERE BDID = ?";
        // Removed "date(NEXTOCCURRENCEDATE, 'localtime') as NEXTOCCURRENCEDATE, "
        // because causing problems with some systems and in different time zones

        wxDateTime updateOccur = wxDateTime::Now();
        int numRepeats  = -1;

        wxSQLite3Statement st = db->PrepareStatement(sql);
        st.Bind(1, bdID);

        wxSQLite3ResultSet q1 = st.ExecuteQuery();

        if (q1.NextRow())
        {
            wxString nextOccurrString = q1.GetString(wxT("NEXTOCCURRENCEDATE"));
            wxDateTime dtno = mmGetStorageStringAsDate(nextOccurrString);
            updateOccur = dtno;

            int repeats = q1.GetInt(wxT("REPEATS"));

            // DeMultiplex the Auto Executable fields.
            if (repeats >= BD_REPEATS_MULTIPLEX_BASE)    // Auto Execute User Acknowlegement required
                repeats -= BD_REPEATS_MULTIPLEX_BASE;
            if (repeats >= BD_REPEATS_MULTIPLEX_BASE)    // Auto Execute Silent mode
                repeats -= BD_REPEATS_MULTIPLEX_BASE;

            numRepeats = q1.GetInt(wxT("NUMOCCURRENCES"));
            if (numRepeats != -1)
            {
                if (repeats < 11) --numRepeats;
            }

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
            else if ((repeats == 11) || (repeats == 12))
            {
                if (numRepeats != -1) numRepeats = -1;
            }
            else if (repeats == 13)
            {
                if (numRepeats > 0) updateOccur = dtno.Add(wxDateSpan::Days(numRepeats));
            }
            else if (repeats == 14)
            {
                if (numRepeats > 0) updateOccur = dtno.Add(wxDateSpan::Months(numRepeats));
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
        mmOptions::instance().databaseUpdated_ = true;

    } catch(const wxSQLite3Exception& e)
    {
        wxLogDebug(wxT("Function::completeBDInSeries: Exception"), e.GetMessage().c_str());
        wxLogError(wxT("Complete BD In Series. ") + wxString::Format(_("Error: %s"), e.GetMessage().c_str()));
    }
}

void mmDBWrapper::deleteStockInvestment(wxSQLite3Database* db, int stockID)
{
    try {
        wxSQLite3Statement st = db->PrepareStatement("delete from STOCK_V1 where STOCKID = ?");
        st.Bind(1, stockID);
        st.ExecuteUpdate();
        st.Finalize();
        mmOptions::instance().databaseUpdated_ = true;

    } catch(const wxSQLite3Exception& e)
    {
        wxLogDebug(wxT("Function::deleteStockInvestment: Exception"), e.GetMessage().c_str());
        wxLogError(wxT("Delete Stock Investment. ") + wxString::Format(_("Error: %s"), e.GetMessage().c_str()));
    }
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
    try {
        wxSQLite3Statement st = db->PrepareStatement("delete from ASSETS_V1 where ASSETID = ?");
        st.Bind(1, assetID);
        st.ExecuteUpdate();
        st.Finalize();
        mmOptions::instance().databaseUpdated_ = true;

    } catch(const wxSQLite3Exception& e)
    {
        wxLogDebug(wxT("Function::deleteAsset: Exception"), e.GetMessage().c_str());
        wxLogError(wxT("delete from ASSETS_V1. ") + wxString::Format(_("Error: %s"), e.GetMessage().c_str()));
    }
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
           "date(STARTDATE, 'localtime') as STARTDATE "
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

//              improve the accuracy of number of years calculation
                int numMonths = todayDate.GetMonth() - startDate.GetMonth();
                int numDays   = todayDate.GetDay() - startDate.GetDay();
                if ( (numMonths >= 0 ) && (numDays < 0) )   numMonths --;
                if ( (numYears > 0 )   && (numMonths < 0 )) numYears -- ;

                if (numYears > 0)
                {
//                  double appreciation = numYears * valueChangeRate * value / 100;
//                  assetValue = value + appreciation;

//                  Calculation changed to compound interest.
                    while (numYears > 0)
                    {
                        double appreciation = valueChangeRate * value / 100;
                        assetValue = value + appreciation;
                        value = assetValue;
                        numYears --;
                    }
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
//                  double depreciation = numYears * valueChangeRate * value / 100;
//                  assetValue = value - depreciation;

//                  Calculation changed to compound interest.
                    while (numYears > 0)
                    {
                        double depreciation = valueChangeRate * value / 100;
                        assetValue = value - depreciation;
                        value = assetValue;
                        numYears --;
                    }
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

wxString mmDBWrapper::getSplitTrxNotes(wxSQLite3Database* db_, int trxID)
{
    wxString infoStr =  wxEmptyString;
    double amount;
    wxString amountStr;

    char sql[]=
        "select c.categname || case when sc.subcategname not null then ' : '||sc.subcategname else ''end as CATEG "
        ", -st.splittransamount as SPLITTRANSAMOUNT "
        "from splittransactions_v1 st "
        "left join category_v1 c on st.categid=c.categid "
        "left join subcategory_v1 sc on st.subcategid=sc.subcategid "
        "where st.transid = ?  "
        "order by c.categname, sc.subcategname";

    wxSQLite3Statement st = db_->PrepareStatement(sql);
    st.Bind(1, trxID);

    wxSQLite3ResultSet q1 = st.ExecuteQuery();
    while (q1.NextRow())
    {
        infoStr << q1.GetString(wxT("CATEG"));
        infoStr << wxT(" = ");
        amount = q1.GetDouble(wxT("SPLITTRANSAMOUNT"));
        mmex::formatDoubleToCurrencyEdit(amount, amountStr);
        infoStr << amountStr << wxT("\n");
    }
    st.Finalize();
    return infoStr;
}

void mmDBWrapper::removeSplitsForAccount(wxSQLite3Database* db, int accountID)
{
    try {
        static const char sql[] =
        "delete from SPLITTRANSACTIONS_V1 "
        "where TRANSID in( SELECT TRANSID "
                          "from CHECKINGACCOUNT_V1 "
                          "where ACCOUNTID = ? OR TOACCOUNTID = ? "
                        ")";

        static const char sql2[] =
        "delete from BUDGETSPLITTRANSACTIONS_V1 "
        "where TRANSID in( SELECT BDID "
                          "from BILLSDEPOSITS_V1 "
                          "where ACCOUNTID = ? OR TOACCOUNTID = ? "
                        ")";

        wxSQLite3Statement st = db->PrepareStatement(sql);
        st.Bind(1, accountID);
        st.Bind(2, accountID);

        db->Begin();
        st.ExecuteUpdate();
        st.Finalize();

        st = db->PrepareStatement(sql2);
        st.Bind(1, accountID);
        st.Bind(2, accountID);

        st.ExecuteUpdate();
        st.Finalize();
        db->Commit();
        mmOptions::instance().databaseUpdated_ = true;
    } catch(const wxSQLite3Exception& e)
    {
        wxLogDebug(wxT("Function::removeSplitsForAccount: Exception"), e.GetMessage().c_str());
        wxLogError(wxT("Remove Splits For Account. ") + wxString::Format(_("Error: %s"), e.GetMessage().c_str()));
    }
}
//----------------------------------------------------------------------------

wxString mmDBWrapper::getLastDbPath(wxSQLite3Database *db, const wxString &defaultVal)
{
    wxString path = getINISettingValue(db, wxT("LASTFILENAME"), defaultVal);

    if (!mmex::isPortableMode()) {
        return path;
    }

    wxString vol = wxFileName(wxStandardPaths::Get().GetExecutablePath()).GetVolume();

    if (!vol.IsEmpty()) {

        wxFileName fname(path);
        fname.SetVolume(vol); // database should be on portable device

        if (fname.FileExists()) {
            path = fname.GetFullPath();
        }
    }

    return path;
}
//----------------------------------------------------------------------------

/*
    SQLITE_OPEN_READWRITE
    The database is opened for reading and writing if possible, or reading
    only if the file is write protected by the operating system.  In either
    case the database must already exist, otherwise an error is returned.
*/
boost::shared_ptr<wxSQLite3Database> mmDBWrapper::Open(const wxString &dbpath, const wxString &key)
{
    boost::shared_ptr<wxSQLite3Database> db(new wxSQLite3Database);

    int err = SQLITE_OK;
    wxString errStr=wxEmptyString;
    try
    {
        db->Open(dbpath, key);
    } catch (const wxSQLite3Exception& e)
    {
        //wxLogError(wxT("Database::open: Exception: %s"), e.GetMessage().c_str());
        //wxLogDebug(wxT("Database::open: Exception: %s"), e.GetMessage().c_str());
        err = e.GetErrorCode();
        errStr << e.GetMessage();
    }
    if (err==SQLITE_OK)
    {
        //Try if db selectable
        try {
            db->ExecuteScalar("select count(*) from  sqlite_master where type='table'");
        } catch (const wxSQLite3Exception &e)
        {
            err = e.GetExtendedErrorCode();
            errStr = e.GetMessage();
        }

        if (err==SQLITE_OK)
        {
            //timeout 2 sec
            db->SetBusyTimeout(2000);

            //TODO oblolete code
            if (err!=SQLITE_OK)
            {
                wxLogError(wxString::Format(_("Write error: %s"), errStr.c_str()));
            }
        return (db);
        }
    }
    db->Close();
    db.reset();

    wxString s = _("When database file opening:");
    s << wxT("\n") << wxString::Format(wxT("\n%s\n\n"), dbpath.c_str());
    if (err == SQLITE_CANTOPEN)
    {
        s << _("Can't open file") <<wxT("\n") << _("You must specify path to another database file") << wxT("\n");
    }
    else if (err == SQLITE_NOTADB)
    {
        s << _("An incorrect password given for an encrypted file")
        << wxT("\n\n") << _("or") << wxT("\n\n")
        << _("Attempt to open a File that is not a database file") << wxT("\n");
    }
    else
    {
        s << wxT("\n") << _("Error") << err << wxT("\n");
    }

    wxSafeShowMessage(_("Database::open: Exception"), s);

    s << errStr << wxT("\n\n") << _("Continue ?");

    wxMessageDialog msgDlg(NULL, s, _("Error"), wxYES_NO|wxICON_ERROR);
    if (msgDlg.ShowModal() == wxID_NO)
        exit(err);

    return db; // return a NULL database pointer
}

// #define SQLITE_OK           0   /* Successful result */
/* beginning-of-error-codes */
//  SQLITE_ERROR        1   /* SQL error or missing database */
//  SQLITE_INTERNAL     2   /* Internal logic error in SQLite */
//  SQLITE_PERM         3   /* Access permission denied */
//  SQLITE_ABORT        4   /* Callback routine requested an abort */
//  SQLITE_BUSY         5   /* The database file is locked */
//  SQLITE_LOCKED       6   /* A table in the database is locked */
//  SQLITE_NOMEM        7   /* A malloc() failed */
//  SQLITE_READONLY     8   /* Attempt to write a readonly database */
//  SQLITE_INTERRUPT    9   /* Operation terminated by sqlite3_interrupt()*/
//  SQLITE_IOERR       10   /* Some kind of disk I/O error occurred */
//  SQLITE_CORRUPT     11   /* The database disk image is malformed */
//  SQLITE_NOTFOUND    12   /* Unknown opcode in sqlite3_file_control() */
//  SQLITE_FULL        13   /* Insertion failed because database is full */
//  SQLITE_CANTOPEN    14   /* Unable to open the database file */
//  SQLITE_PROTOCOL    15   /* Database lock protocol error */
//  SQLITE_EMPTY       16   /* Database is empty */
//  SQLITE_SCHEMA      17   /* The database schema changed */
//  SQLITE_TOOBIG      18   /* String or BLOB exceeds size limit */
//  SQLITE_CONSTRAINT  19   /* Abort due to constraint violation */
//  SQLITE_MISMATCH    20   /* Data type mismatch */
//  SQLITE_MISUSE      21   /* Library used incorrectly */
//  SQLITE_NOLFS       22   /* Uses OS features not supported on host */
//  SQLITE_AUTH        23   /* Authorization denied */
//  SQLITE_FORMAT      24   /* Auxiliary database format error */
//  SQLITE_RANGE       25   /* 2nd parameter to sqlite3_bind out of range */
//  SQLITE_NOTADB      26   /* File opened that is not a database file */
//  SQLITE_ROW         100  /* sqlite3_step() has another row ready */
//  SQLITE_DONE        101  /* sqlite3_step() has finished executing */

//----------------------------------------------------------------------------
