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
#include <sqlite3.h>
//----------------------------------------------------------------------------

namespace
{

const double g_defBASECONVRATE = 1.0;

//----------------------------------------------------------------------------

void insertCategoryTree(wxSQLite3Database* db,
                   wxSQLite3Statement &cat,
                   wxSQLite3Statement &subcat,
                   const wxString &categoryName,
                   const char* subcats[] // must be NULL or ends with NULL
                  )
{
    wxASSERT(db);

    bool ok = cat.GetParamCount() == 1;
    wxASSERT(ok);

    cat.Bind(1, wxGetTranslation(categoryName));
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
        wxString subcateg = wxString::Format(wxT("%s"), (subcats[i]));
        subcat.Bind(name_idx, wxGetTranslation(subcateg));
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

        wxSQLite3Statement st_cat = db->PrepareStatement(INSERT_INTO_CATEGORY_V1);

        wxSQLite3Statement st_subcat = db->PrepareStatement(INSERT_ROW_INTO_SUBCATEGORY_V1);

        const char* BillsCategories[] = {
            (char *)wxTRANSLATE("Telephone"), (char *)wxTRANSLATE("Electricity"),
            (char *)wxTRANSLATE("Gas"), (char *)wxTRANSLATE("Internet"),
            (char *)wxTRANSLATE("Rent"), (char *)wxTRANSLATE("Cable TV"),
            (char *)wxTRANSLATE("Water"), 0
        };
        insertCategoryTree(db, st_cat, st_subcat, wxTRANSLATE("Bills"), BillsCategories);

        const char* FoodCategories[] = {
        (char *)wxTRANSLATE("Groceries"), (char *)wxTRANSLATE("Dining out"), 0};
        insertCategoryTree(db, st_cat, st_subcat, wxTRANSLATE("Food"), FoodCategories);

        const char* LeisureCategories[] = {
            (char *)wxTRANSLATE("Movies"), (char *)wxTRANSLATE("Video Rental"),
            (char *)wxTRANSLATE("Magazines"), 0};
        insertCategoryTree(db, st_cat, st_subcat, wxTRANSLATE("Leisure"), LeisureCategories);

        const char* AutomobileCategories[] = {
            (char *)wxTRANSLATE("Maintenance"), (char *)wxTRANSLATE("Gas"),
            (char *)wxTRANSLATE("Parking"), (char *)wxTRANSLATE("Registration"), 0};
        insertCategoryTree(db, st_cat, st_subcat, wxTRANSLATE("Automobile"), AutomobileCategories);

        const char* EducationCategories[] = {
            (char *)wxTRANSLATE("Books"),
            (char *)wxTRANSLATE("Tuition"), (char *)wxTRANSLATE("Others"), 0};
        insertCategoryTree(db, st_cat, st_subcat, wxTRANSLATE("Education"), EducationCategories);

        const char* HomeneedsCategories[] = {
            (char *)wxTRANSLATE("Clothing"),
            (char *)wxTRANSLATE("Furnishing"), (char *)wxTRANSLATE("Others"), 0};
        insertCategoryTree(db, st_cat, st_subcat, wxTRANSLATE("Homeneeds"), HomeneedsCategories);

        const char* HealthcareCategories[] = {
            (char *)wxTRANSLATE("Health"),
            (char *)wxTRANSLATE("Dental"), (char *)wxTRANSLATE("Eyecare"),
            (char *)wxTRANSLATE("Physician"), (char *)wxTRANSLATE("Prescriptions"), 0};
        insertCategoryTree(db, st_cat, st_subcat, wxTRANSLATE("Healthcare"), HealthcareCategories);

        const char* InsuranceCategories[] = {
            (char *)wxTRANSLATE("Auto"), (char *)wxTRANSLATE("Life"),
            (char *)wxTRANSLATE("Home"), (char *)wxTRANSLATE("Health"), 0};
        insertCategoryTree(db, st_cat, st_subcat, wxTRANSLATE("Insurance"), InsuranceCategories);

        const char* VacationCategories[] = {
            (char *)wxTRANSLATE("Travel"), (char *)wxTRANSLATE("Lodging"),
            (char *)wxTRANSLATE("Sightseeing"), 0};
        insertCategoryTree(db, st_cat, st_subcat, wxTRANSLATE("Vacation"), VacationCategories);

        const char* TaxesCategories[] = {
            (char *)wxTRANSLATE("Income Tax"), (char *)wxTRANSLATE("House Tax"),
            (char *)wxTRANSLATE("Water Tax"), (char *)wxTRANSLATE("Others"), 0};
        insertCategoryTree(db, st_cat, st_subcat, wxTRANSLATE("Taxes"), TaxesCategories);

        insertCategoryTree(db, st_cat, st_subcat, wxTRANSLATE("Miscellaneous"), 0);
        insertCategoryTree(db, st_cat, st_subcat, wxTRANSLATE("Gifts"), 0);

        const char* IncomeCategories[] = {
            (char *)wxTRANSLATE("Salary"), (char *)wxTRANSLATE("Reimbursement/Refunds"),
            (char *)wxTRANSLATE("Investment Income"), 0};
        insertCategoryTree(db, st_cat, st_subcat, wxTRANSLATE("Income"), IncomeCategories);

        insertCategoryTree(db, st_cat, st_subcat, wxTRANSLATE("Other Income"), 0);
        insertCategoryTree(db, st_cat, st_subcat, wxTRANSLATE("Other Expenses"), 0);
        insertCategoryTree(db, st_cat, st_subcat, wxTRANSLATE("Transfer"), 0);

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

#if 0
void mmDBWrapper::createInfoV1Table(wxSQLite3Database* db)
{
    try{
        /* We always create new INFOTABLE_V1 */
        if (!db->TableExists(wxT("INFOTABLE_V1")))
        {

            db->ExecuteUpdate(CREATE_TABLE_INFOTABLE_V1);

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
#endif

void mmDBWrapper::createCurrencyV1Table(wxSQLite3Database* db)
{
    if (db->TableExists(wxT("CURRENCYFORMATS_V1"))) return;

    try
    {
        db->ExecuteUpdate(CREATE_TABLE_CURRENCYFORMATS_V1);
        /* Load Default Currencies */
        wxSortedArrayString currencies;
        currencies.Add(wxT("US Dollar  ;$;;.;,;dollar;cents;100;1;USD"));
        // currencies.Add(wxT("EURO;€;;.;,;euro;cent;100;1;EUR"));
	    // Euro symbol € incorrectly displayed in windows. Correct when using \u20ac equivalent.
        // MS-VC++ 2010: Ignore warning C4428: universal-character-name encountered in source
#pragma warning( push )
#pragma warning( disable : 4428 )
        currencies.Add(wxT("EURO;\u20ac;;.;,;euro;cent;100;1;EUR"));
#pragma warning( pop )
        wxString fileName = mmex::getPathResource(mmex::CURRENCY_DB_SEED);
        wxASSERT(wxFileName::FileExists(fileName));

        if (!fileName.empty())
        {
            wxTextFile tFile(fileName);
            if (tFile.Open())
            {
                wxString str;
                for (str = tFile.GetFirstLine(); !tFile.Eof(); str = tFile.GetNextLine())
                {
                    currencies.Add(str);
                }
            }
            else
                wxMessageBox(_("Unable to open file."), _("Currency Manager"), wxOK|wxICON_WARNING);
        }

        //wxSQLite3Statement st;

        for (size_t i = 0; i < currencies.Count(); ++i)
        {
            wxStringTokenizer tk(currencies[i], wxT(";"));

            std::vector<wxString> data;
            data.push_back(tk.GetNextToken().Trim());
            data.push_back(tk.GetNextToken());
            data.push_back(tk.GetNextToken());
            data.push_back(tk.GetNextToken());
            data.push_back(tk.GetNextToken());
            data.push_back(tk.GetNextToken());
            data.push_back(tk.GetNextToken());
            data.push_back(tk.GetNextToken());
            data.push_back(tk.GetNextToken());
            data.push_back(tk.GetNextToken());

            addCurrency(db, data);
        }
    }
    catch(const wxSQLite3Exception& e)
    {
        wxLogDebug(wxT("Database::createCurrencyV1Table: Exception"), e.GetMessage().c_str());
        wxLogError(wxT("create CURRENCYFORMATS_V1. ") + wxString::Format(_("Error: %s"), e.GetMessage().c_str()));
    }
}


void mmDBWrapper::createBudgetingV1Table(wxSQLite3Database* db)
{
    try{
        bool valid = db->TableExists(wxT("BUDGETYEAR_V1"));
        if (!valid)
        {
            db->ExecuteUpdate(CREATE_TABLE_BUDGETYEAR_V1);
            valid = db->TableExists(wxT("BUDGETYEAR_V1"));
            wxASSERT(valid);
        }

        valid = db->TableExists(wxT("BUDGETTABLE_V1"));
        if (!valid)
        {
            db->ExecuteUpdate(CREATE_TABLE_BUDGETTABLE_V1);
            valid = db->TableExists(wxT("BUDGETTABLE_V1"));
            wxASSERT(valid);
        }
    } catch(const wxSQLite3Exception& e)
    {
        wxLogDebug(wxT("Database::createBudgetingV1Table: Exception"), e.GetMessage().c_str());
        wxLogError(wxT("create table BUDGETTABLE_V1. ") + wxString::Format(_("Error: %s"), e.GetMessage().c_str()));
    }
}

void mmDBWrapper::createStockV1Table(wxSQLite3Database* db)
{
    try{
        bool valid = db->TableExists(wxT("STOCK_V1"));
        if (!valid)
        {
            db->ExecuteUpdate(CREATE_TABLE_STOCK_V1);
            valid = db->TableExists(wxT("STOCK_V1"));
            wxASSERT(valid);
        }
    } catch(const wxSQLite3Exception& e)
    {
        wxLogDebug(wxT("Database::createStockV1Table: Exception"), e.GetMessage().c_str());
        wxLogError(wxT("create table STOCK_V1. ") + wxString::Format(_("Error: %s"), e.GetMessage().c_str()));
    }
}

void mmDBWrapper::createAssetsV1Table(wxSQLite3Database* db)
{
    try{
        bool valid = db->TableExists(wxT("ASSETS_V1"));
        if (!valid)
        {
            db->ExecuteUpdate(CREATE_TABLE_ASSETS_V1);
            valid = db->TableExists(wxT("ASSETS_V1"));
            wxASSERT(valid);
        }
    } catch(const wxSQLite3Exception& e)
    {
        wxLogDebug(wxT("Database::createAssetsV1Table: Exception"), e.GetMessage().c_str());
        wxLogError(wxT("create table ASSETS_V1. ") + wxString::Format(_("Error: %s"), e.GetMessage().c_str()));
    }
}

void mmDBWrapper::createBillsDepositsV1Table(wxSQLite3Database* db)
{
    try {
        bool valid = db->TableExists(wxT("BILLSDEPOSITS_V1"));
        if (!valid)
        {
            db->ExecuteUpdate(CREATE_TABLE_BILLSDEPOSITS_V1);
            valid = db->TableExists(wxT("BILLSDEPOSITS_V1"));
            wxASSERT(valid);
        }
    } catch(const wxSQLite3Exception& e)
    {
        wxLogDebug(wxT("Database::createBillsDepositsV1Table: Exception"), e.GetMessage().c_str());
        wxLogError(wxT("create table BILLSDEPOSITS_V1. ") + wxString::Format(_("Error: %s"), e.GetMessage().c_str()));
    }
}

bool mmDBWrapper::checkDBVersion(wxSQLite3Database* db)
{
    bool result = false;
    if (db->TableExists(wxT("INFOTABLE_V1")))
    {
        wxSQLite3Statement st = db->PrepareStatement(SELECT_INFOVALUE_FROM_INFOTABLE_V1);
        st.Bind(1, wxT("DATAVERSION"));
        wxSQLite3ResultSet q1 = st.ExecuteQuery();
        if (q1.NextRow())
        {
            int dataVersion = q1.GetInt(wxT("INFOVALUE"));
            result = dataVersion >= mmex::MIN_DATAVERSION;
        }
        st.Finalize();
    }
    return result;
}

void mmDBWrapper::createAccountListV1Table(wxSQLite3Database* db)
{
    try {
        bool exists = db->TableExists(wxT("ACCOUNTLIST_V1"));
        if (!exists)
        {
            db->ExecuteUpdate(CREATE_TABLE_ACCOUNTLIST_V1);
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
    } catch(const wxSQLite3Exception& e)
    {
        wxLogDebug(wxT("Database::createAccountListV1Table: Exception"), e.GetMessage().c_str());
        wxLogError(wxT("create table ACCOUNTLIST_V1. ") + wxString::Format(_("Error: %s"), e.GetMessage().c_str()));
    }
}

void mmDBWrapper::createCheckingAccountV1Table(wxSQLite3Database* db)
{
    try {
        bool exists = db->TableExists(wxT("CHECKINGACCOUNT_V1"));
        if (!exists)
        {
            db->ExecuteUpdate(CREATE_TABLE_CHECKINGACCOUNT_V1);
            exists = db->TableExists(wxT("CHECKINGACCOUNT_V1"));
            wxASSERT(exists);
        }
    } catch(const wxSQLite3Exception& e)
    {
        wxLogDebug(wxT("Database::createCheckingAccountV1Table: Exception"), e.GetMessage().c_str());
        wxLogError(wxT("create table CHECKINGACCOUNT_V1. ") + wxString::Format(_("Error: %s"), e.GetMessage().c_str()));
    }
}

void mmDBWrapper::createSplitTransactionsV1Table(wxSQLite3Database* db)
{
    try {
        bool exists = db->TableExists(wxT("SPLITTRANSACTIONS_V1"));
        if (!exists)
        {
            db->ExecuteUpdate(CREATE_TABLE_SPLITTRANSACTIONS_V1);
            exists = db->TableExists(wxT("SPLITTRANSACTIONS_V1"));
            wxASSERT(exists);
        }

        exists = db->TableExists(wxT("BUDGETSPLITTRANSACTIONS_V1"));
        if (!exists)
        {
            db->ExecuteUpdate(CREATE_TABLE_BUDGETSPLITTRANSACTIONS_V1);
            exists = db->TableExists(wxT("BUDGETSPLITTRANSACTIONS_V1"));
            wxASSERT(exists);
        }
    } catch(const wxSQLite3Exception& e)
    {
        wxLogDebug(wxT("Database::createSplitTransactionsV1Table: Exception"), e.GetMessage().c_str());
        wxLogError(wxT("create table BUDGETSPLITTRANSACTIONS_V1. ") + wxString::Format(_("Error: %s"), e.GetMessage().c_str()));
    }
}

void mmDBWrapper::createPayeeV1Table(wxSQLite3Database* db)
{
    try {
        bool exists = db->TableExists(wxT("PAYEE_V1"));
        if (!exists)
        {
            db->ExecuteUpdate(CREATE_TABLE_PAYEE_V1);
            exists = db->TableExists(wxT("PAYEE_V1"));
            wxASSERT(exists);
        }
    } catch(const wxSQLite3Exception& e)
    {
        wxLogDebug(wxT("Database::createPayeeV1Table: Exception"), e.GetMessage().c_str());
        wxLogError(wxT("create table PAYEE_V1. ") + wxString::Format(_("Error: %s"), e.GetMessage().c_str()));
    }
}

void mmDBWrapper::createCategoryV1Table(wxSQLite3Database* db)
{
    try {
        bool existsCat = db->TableExists(wxT("CATEGORY_V1"));
        if (!existsCat)
        {
            /* Create CATEGORY_V1 Tables */
            db->ExecuteUpdate(CREATE_TABLE_CATEGORY_V1);
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
            db->ExecuteUpdate(CREATE_TABLE_SUBCATEGORY_V1);
            existsSubCat = db->TableExists(wxT("SUBCATEGORY_V1"));
            wxASSERT(existsSubCat);
        }

        if (!existsCat)
        {
            createDefaultCategories(db);
        }
    } catch(const wxSQLite3Exception& e)
    {
        wxLogDebug(wxT("Database::createCategoryV1Table: Exception"), e.GetMessage().c_str());
        wxLogError(wxT("create table CATEGORY_V1. ") + wxString::Format(_("Error: %s"), e.GetMessage().c_str()));
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
    try {
        if (db->TableExists(wxT("SPLITTRANSACTIONS_V1")))
        {
            if (db->ExecuteScalar("SELECT COUNT(*) FROM SPLITTRANSACTIONS_V1 WHERE SPLITTRANSACTIONS_V1.TRANSID NOT IN (SELECT TRANSID FROM CHECKINGACCOUNT_V1)") > 0)
                db->ExecuteUpdate("DELETE FROM SPLITTRANSACTIONS_V1 WHERE SPLITTRANSACTIONS_V1.TRANSID NOT IN (SELECT TRANSID FROM CHECKINGACCOUNT_V1)");
        }

        if (db->TableExists(wxT("BUDGETSPLITTRANSACTIONS_V1")))
        {
            if (db->ExecuteScalar("SELECT COUNT(*) FROM BUDGETSPLITTRANSACTIONS_V1 WHERE BUDGETSPLITTRANSACTIONS_V1.TRANSID NOT IN (SELECT BDID FROM BILLSDEPOSITS_V1)") > 0)
                db->ExecuteUpdate("DELETE FROM BUDGETSPLITTRANSACTIONS_V1 WHERE BUDGETSPLITTRANSACTIONS_V1.TRANSID NOT IN (SELECT BDID FROM BILLSDEPOSITS_V1)");
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

void mmDBWrapper::initDB(wxSQLite3Database* db)
{
    db->Begin();

    /* Create INFOTABLE_V1 Tables */
//    createInfoV1Table(db);

    /* Create Currency Settings */
    createCurrencyV1Table(db);

    /* Create ACCOUNTLIST_V1 Tables */
    createAccountListV1Table(db);

    /* Create CHECKINGACCOUNT_V1 Tables */
    createCheckingAccountV1Table(db);

    /* Create PAYEE_V1 Tables */
    createPayeeV1Table(db);

    /* Create CATEGORY_V1 Tables */
    createCategoryV1Table(db);

    /* Create Budgeting_V1 Tables */
    createBudgetingV1Table(db);

    /* Create Bills & Deposits V1 Table */
    createBillsDepositsV1Table(db);

    /* Create Stock V1 Table */
    createStockV1Table(db);

    /* Create Asset V1 Table */
    createAssetsV1Table(db);

    /* Create SplitTransactions V1 Table */
    createSplitTransactionsV1Table(db);

    /* Create AllData view */
    createAllDataView(db);

    /* Remove Any cruft */
    removeCruft(db);
    db->Commit();
}

void mmDBWrapper::loadCurrencySettings(wxSQLite3Database* db, int currencyID)
{
    wxSQLite3Statement st = db->PrepareStatement(SELECT_ROW_FROM_CURRENCYFORMATS_V1);
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
            "DELETE FROM SUBCATEGORY_V1     WHERE CATEGID = ?",
            "DELETE FROM CATEGORY_V1        WHERE CATEGID = ?",
            "DELETE FROM BUDGETTABLE_V1     WHERE CATEGID = ?",
            "UPDATE PAYEE_V1 SET CATEGID=-1 WHERE CATEGID = ?",
            0
        };

        db->Begin();
        for (int i = 0; sql_del[i]; ++i)
        {
            wxSQLite3Statement st = db->PrepareStatement(sql_del[i]);
            st.Bind(1, categID);
            st.ExecuteUpdate();
            st.Finalize();
        }
        db->Commit();

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
            "DELETE FROM SUBCATEGORY_V1 WHERE CATEGID=? AND SUBCATEGID=?",
            "DELETE FROM BUDGETTABLE_V1 WHERE CATEGID=? AND SUBCATEGID=?",

            "UPDATE PAYEE_V1 SET CATEGID = -1, SUBCATEGID = -1 "
                                       "WHERE CATEGID=? AND SUBCATEGID=?",
            0
        };

        db->Begin();
        for (int i = 0; sql_del[i]; ++i)
        {
            wxSQLite3Statement st = db->PrepareStatement(sql_del[i]);
            st.Bind(1, categID);
            st.Bind(2, subcategID);

            st.ExecuteUpdate();
            st.Finalize();
        }
        db->Commit();

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
            wxSQLite3Statement st = db->PrepareStatement(UPDATE_CATEGORY_V1);

            st.Bind(1, newName);
            st.Bind(2, categID);

            st.ExecuteUpdate();
            st.Finalize();
        }
        else
        {
            wxSQLite3Statement st = db->PrepareStatement(UPDATE_SUBCATEGORY_V1);

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
        wxSQLite3Statement st = db->PrepareStatement(INSERT_INTO_CATEGORY_V1);
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

int mmDBWrapper::relocatePayee(wxSQLite3Database* db,
    const int destPayeeID, const int sourcePayeeID)
{
    int changedPayees_, err = SQLITE_OK;
    //static const char SET_PAYEEID_CHECKINGACCOUNT_V1[] = "UPDATE CHECKINGACCOUNT_V1 SET PAYEEID = ? WHERE PAYEEID = ? ";
    wxSQLite3Statement st = db->PrepareStatement(SET_PAYEEID_CHECKINGACCOUNT_V1);
    st.Bind(1, destPayeeID);
    st.Bind(2, sourcePayeeID);
    try {
        changedPayees_ = st.ExecuteUpdate();
        st.Finalize();
        //db_.get()->Commit();
    } catch(const wxSQLite3Exception& e)
    {
        err = e.GetExtendedErrorCode();
        wxLogDebug(wxT("update checkingaccount_v1 : Exception"), e.GetMessage().c_str());
        wxLogError(wxString::Format(_("Error: %s"), e.GetMessage().c_str()));
    }
    return err;
}

int mmDBWrapper::relocateCategory(wxSQLite3Database* db,
    const int destCatID, const int destSubCatID, const int sourceCatID, const int sourceSubCatID)
{
    int err = SQLITE_OK;
	static const char sqlCat[] = "update checkingaccount_v1 set categid= ?, subcategid= ? "
								 "where categid= ? and subcategid= ?";
	wxSQLite3Statement stCat = db->PrepareStatement(sqlCat);
	stCat.Bind(1, destCatID);
	stCat.Bind(2, destSubCatID);
	stCat.Bind(3, sourceCatID);
	stCat.Bind(4, sourceSubCatID);

	static const char sqlSubCat[] = "update splittransactions_v1 set categid= ?, subcategid= ? "
									"where categid= ? and subcategid= ?";
	wxSQLite3Statement stSubCat = db->PrepareStatement(sqlSubCat);
	stSubCat.Bind(1, destCatID);
	stSubCat.Bind(2, destSubCatID);
	stSubCat.Bind(3, sourceCatID);
	stSubCat.Bind(4, sourceSubCatID);
    try {
		stCat.ExecuteUpdate();
		stSubCat.ExecuteUpdate();
		stCat.Finalize();
		stSubCat.Finalize();
        //db_.get()->Commit();
    } catch(const wxSQLite3Exception& e)
    {
        err = e.GetExtendedErrorCode();
        wxLogDebug(wxT("update checkingaccount_v1 : Exception"), e.GetMessage().c_str());
        wxLogError(wxString::Format(_("Error: %s"), e.GetMessage().c_str()));
    }
    return err;
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

bool mmDBWrapper::updateTransactionWithStatus(wxSQLite3Database &db, int transID,
                                              const wxString& status)
{
    try {
    wxSQLite3Statement st = db.PrepareStatement(SET_STATUS_CHECKINGACCOUNT_V1);
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

#if 0
wxString mmDBWrapper::getInfoSettingValue(wxSQLite3Database* db, const wxString& settingName,
                                            const wxString& defaultVal)
{
    wxString value = defaultVal;

    wxSQLite3Statement st = db->PrepareStatement(SELECT_INFOVALUE_FROM_INFOTABLE_V1);
    st.Bind(1, settingName);

    wxSQLite3ResultSet q1 = st.ExecuteQuery();
    if (q1.NextRow())
    {
        value = q1.GetString(wxT("INFOVALUE"));
    }
    st.Finalize();

    return value;
}
#endif

#if 0
void mmDBWrapper::setInfoSettingValue(wxSQLite3Database* db, const wxString& settingName,
                                     const wxString& settingValue)
{
    try {
        wxSQLite3Statement st = db->PrepareStatement(UPDATE_INFOTABLE_V1);
        st.Bind(1, settingValue);
        st.Bind(2, settingName);

        int rows_affected = st.ExecuteUpdate();
        st.Finalize();

        if (!rows_affected)
        {
            st = db->PrepareStatement(INSERT_INTO_INFOTABLE_V1);
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
#endif

void mmDBWrapper::addBudgetYear(wxSQLite3Database* db, const wxString &year)
{
    try {
        wxSQLite3Statement st = db->PrepareStatement(INSERT_INTO_BUDGETYEARNAME_V1);
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

int mmDBWrapper::getBudgetYearID(wxSQLite3Database* db, const wxString &year_name)
{
    int budgetYearID = -1;

    wxSQLite3Statement st = db->PrepareStatement(SELECT_ALL_FROM_BUDGETYEAR_V1);
    wxSQLite3ResultSet q1 = st.ExecuteQuery();
    while (q1.NextRow())
    {
        if (q1.GetString(wxT("BUDGETYEARNAME")) == year_name)
        {
            budgetYearID = q1.GetInt(wxT("BUDGETYEARID"));
            break;
        }
    }
    st.Finalize();

    return budgetYearID;
}

wxString mmDBWrapper::getBudgetYearForID(wxSQLite3Database* db, const int &year_id)
{
     wxString year_name = wxT("");

     wxSQLite3Statement st = db->PrepareStatement(SELECT_ALL_FROM_BUDGETYEAR_V1);
     wxSQLite3ResultSet q1 = st.ExecuteQuery();
     while (q1.NextRow())
     {
         if (q1.GetInt(wxT("BUDGETYEARID")) == year_id)
         {
             year_name = q1.GetString(wxT("BUDGETYEARNAME"));
             break;
         }
     }
     st.Finalize();

     return year_name;
}

void mmDBWrapper::updateYearForID(wxSQLite3Database* db,
     const wxString& yearName, int yearid)
{
    try {
        wxSQLite3Statement st = db->PrepareStatement(UPDATE_BUDGETYEAR_V1);

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
    static const char INSERT_INTO_BUDGETTABLE_V1[] =
    "INSERT INTO BUDGETTABLE_V1 ( "
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

    try {
         if (newYear == baseYear)
             return false;

         wxSQLite3Statement st = db->PrepareStatement(INSERT_INTO_BUDGETTABLE_V1);
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
    try {
        wxSQLite3Statement st = db->PrepareStatement(INSERT_INTO_BUDGETTABLE_V1);

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
        wxSQLite3Statement st = db->PrepareStatement(DELETE_FROM_BUDGETTABLE_V1);
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

        wxSQLite3Statement st = db->PrepareStatement(DELETE_BUDGETENTRYIDS_FROM_BUDGETTABLE_V1);
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
    bool found = false;

    wxSQLite3Statement st = db->PrepareStatement(SELECT_ROW_FROM_BUDGETTABLE_V1);
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
            "DELETE FROM BILLSDEPOSITS_V1 where BDID = ?",
            "DELETE FROM BUDGETSPLITTRANSACTIONS_V1 where TRANSID = ?",
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
        "SELECT NUMOCCURRENCES, "
               "NEXTOCCURRENCEDATE, "
               "REPEATS "
        "FROM BILLSDEPOSITS_V1 "
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
            static const char UPDATE_BILLSDEPOSITS_V1[] =
            "UPDATE BILLSDEPOSITS_V1 "
            "set NEXTOCCURRENCEDATE = ?, "
                "NUMOCCURRENCES = ? "
            "where BDID = ?";

            wxSQLite3Statement st = db->PrepareStatement(UPDATE_BILLSDEPOSITS_V1);

            st.Bind(1, updateOccur.FormatISODate());
            st.Bind(2, numRepeats);
            st.Bind(3, bdID);

            st.ExecuteUpdate();
            st.Finalize();
        }

        db->ExecuteUpdate("DELETE FROM BILLSDEPOSITS_V1 where NUMOCCURRENCES = 0");
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
        wxSQLite3Statement st = db->PrepareStatement("DELETE FROM STOCK_V1 where STOCKID = ?");
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

bool mmDBWrapper::moveStockInvestment(wxSQLite3Database* db, int stockID, int toAccountID)
{
    try {
        wxSQLite3Statement st = db->PrepareStatement("UPDATE STOCK_V1 SET HELDAT = ? where STOCKID = ?");
        st.Bind(1, toAccountID);
        st.Bind(2, stockID);
        st.ExecuteUpdate();
        st.Finalize();
        mmOptions::instance().databaseUpdated_ = true;

    } catch(const wxSQLite3Exception& e)
    {
        wxLogDebug(wxT("Function::moveStockInvestment: Exception"), e.GetMessage().c_str());
        wxLogError(wxT("Delete Stock Investment. ") + wxString::Format(_("Error: %s"), e.GetMessage().c_str()));
        return false;
    }
    return true;
}

double mmDBWrapper::getStockInvestmentBalance(wxSQLite3Database* db, double& invested)
{
    double balance = 0.0;
    invested = 0.0;

    wxSQLite3ResultSet q1 = db->ExecuteQuery(SELECT_ALL_INVESTMENT_FROM_ACCOUNTLIST_V1);
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

    wxSQLite3ResultSet q1 = db->ExecuteQuery(SELECT_ALL_FROM_ASSETS_V1);

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
    double assetValue = 0.0;

    wxSQLite3Statement st = db->PrepareStatement(SELECT_ROW_FROM_ASSETS_V1);
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

    static const char sql[]=
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
        wxSQLite3Statement st = db->PrepareStatement(DELETE_FROM_SPLITTRANSACTIONS_V1);
        st.Bind(1, accountID);
        st.Bind(2, accountID);

        st.ExecuteUpdate();
        st.Finalize();

        st = db->PrepareStatement(DELETE_FROM_BUDGETSPLITTRANSACTIONS_V1);
        st.Bind(1, accountID);
        st.Bind(2, accountID);

        st.ExecuteUpdate();
        st.Finalize();

        mmOptions::instance().databaseUpdated_ = true;
    } catch(const wxSQLite3Exception& e)
    {
        wxLogDebug(wxT("Function::removeSplitsForAccount: Exception"), e.GetMessage().c_str());
        wxLogError(wxT("Remove Splits For Account. ") + wxString::Format(_("Error: %s"), e.GetMessage().c_str()));
    }
}

bool mmDBWrapper::deleteCurrency(wxSQLite3Database* db, int currencyID)
{
    try {
        wxSQLite3Statement st = db->PrepareStatement(DELETE_CURRENCYID_FROM_CURRENCYFORMATS_V1);
        st.Bind(1, currencyID);
        st.ExecuteUpdate();
        st.Finalize();

        mmOptions::instance().databaseUpdated_ = true;
    } catch(const wxSQLite3Exception& e)
    {
        wxLogDebug(wxT("Function::deleteCurrency: Exception"), e.GetMessage().c_str());
        wxLogError(wxT("Remove currency. ") + wxString::Format(_("Error: %s"), e.GetMessage().c_str()));
        return false;
    }
    return true;
}
int mmDBWrapper::addCurrency(wxSQLite3Database* db, std::vector<wxString> data)
{
    try {
        wxSQLite3Statement st = db->PrepareStatement(INSERT_INTO_CURRENCYFORMATS_V1);
        int i = 0;
        for (std::vector<wxString>::const_iterator d = data.begin(); d != data.end(); ++d)
            st.Bind(++i, *d);

        st.ExecuteUpdate();

        st.Finalize();
    } catch(const wxSQLite3Exception& e)
    {
        wxLogDebug(wxT("Function::addCurrency: Exception"), e.GetMessage().c_str());
        wxLogError(wxT("Add currency. ") + wxString::Format(_("Error: %s"), e.GetMessage().c_str()));
        return -1;
    }

    return db->GetLastRowId().ToLong();;
}
//----------------------------------------------------------------------------

wxString mmDBWrapper::getLastDbPath(boost::shared_ptr<MMEX_IniSettings> iniSettings, const wxString &defaultVal)
{
    wxString path = iniSettings->GetStringSetting(wxT("LASTFILENAME"), defaultVal);

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
