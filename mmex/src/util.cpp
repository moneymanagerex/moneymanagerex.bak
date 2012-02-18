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

#include "util.h"
#include "dbwrapper.h"
#include "guiid.h"
#include "fileviewerdialog.h"
#include "mmex.h"
#include "univcsvdialog.h"
#include "mmcoredb.h"
#include "paths.h"
#include "platfdep.h"
#include <sstream>
//----------------------------------------------------------------------------
#include <wx/sound.h>
#include <boost/shared_ptr.hpp>
//----------------------------------------------------------------------------

namespace
{

//----------------------------------------------------------------------------
const wxChar g_def_decimal_point = wxT('.');
const int g_def_scale = 100;

//----------------------------------------------------------------------------

int CaseInsensitiveCmp(const wxString &s1, const wxString &s2)
{
    return s1.CmpNoCase(s2);
}

//----------------------------------------------------------------------------
wxString selectLanguageDlg(wxWindow *parent, const wxString &langPath, bool verbose)
{
    wxString lang;

    wxArrayString lang_files;
    size_t cnt = wxDir::GetAllFiles(langPath, &lang_files, wxT("*.mo"));

    if (!cnt) 
    {
        if (verbose) 
        {
            wxString s = wxT("Can't find language files (.mo) at \"");
            s << langPath << wxT('\"');

            wxMessageDialog dlg(parent, s, wxT("Error"), wxICON_ERROR);
            dlg.ShowModal();
        }

        return lang;
    }

    for (size_t i = 0; i < cnt; ++i) 
    {
        wxFileName fname(lang_files[i]);
        lang_files[i] = fname.GetName();
    }

    lang_files.Sort(CaseInsensitiveCmp);
    lang = wxGetSingleChoice(wxT("Please choose language"), wxT("Languages"), lang_files, parent);

    return lang;
}
//----------------------------------------------------------------------------

double mmRound(double x)
{
    x += (x < 0 ? -0.5 : 0.5);

    double n = 0;
    modf(x, &n);

    return n;
}
//----------------------------------------------------------------------------

double mmMoneyInt(double x, int scale)
{
    double n = 0;
    modf(x/scale, &n);
    return n;
}
//----------------------------------------------------------------------------

int mmCents(double x, int scale)
{
    x += (x < 0 ? -0.5 : 0.5);

    double dummy = 0;
    double fract = modf(x/scale, &dummy);

    return static_cast<int>(fract*scale);
}
//----------------------------------------------------------------------------

wchar_t get_group_separator(wxChar sep)
{
    wxString s(sep);
    return *s.wc_str(*wxConvCurrent);
}
//----------------------------------------------------------------------------

/*
    Formats groups of 3 digits separated by group_separator.
    
    Empty separator means zero will be inserted. 
    Thus, if separator is not a printable char, it ignores.
    The same for decimal point char.
*/
wxString format_groups(const mmex::CurrencyFormatter &fmt, double x, size_t grp_sz)
{
    wxString val;
    val.Printf(wxT("%.0f"), x);

    wxChar grp_sep = fmt.getGroupSeparator();

    if (wxIsprint(grp_sep) && val.length() > grp_sz) 
    { 
        wchar_t sep = get_group_separator(grp_sep);

        std::wstring s;
        s.reserve(val.length() + val.length()/grp_sz);
        s.assign(val.wc_str(*wxConvCurrent), val.length());

        for (size_t i = 0, j = i + grp_sz*(i+1); j < s.length(); ++i, j = i + grp_sz*(i+1)) 
        {
            std::wstring::iterator it = s.begin();
            std::advance(it, s.length() - j);    
            s.insert(it, sep);
        }

        val = wxString(s.data(), *wxConvCurrent, s.length());
    }

    return val;
}
//----------------------------------------------------------------------------

wxString format_cents(const mmex::CurrencyFormatter &f, int cents)
{
    const wxChar* fmt[] = { wxT("%02d"), wxT("%01d"), wxT("%03d"), wxT("%04d") };

    wxASSERT(g_def_scale == 100);
    size_t i = 0; // "%02d" for g_def_scale

    int scale = f.getScale();

    if (scale >= 10000) 
    {
        i = 3;
    } 
    else if (scale >= 1000) 
    {
        i = 2;
    } 
    else if (scale < 100) 
    {
        i = 1;
    }

    wxASSERT(i < sizeof(fmt)/sizeof(*fmt));
    
    wxString s = f.getDecimalPoint();
    s += wxString::Format(fmt[i], cents);

    return s;
}
//----------------------------------------------------------------------------

void DoubleToCurrency(const mmex::CurrencyFormatter &fmt, double val, wxString& rdata, bool for_edit)
{
    wxString s;
    s.Alloc(32);

    if (!for_edit) 
    {
        wxString pfx = fmt.getPrefix();
        if (!pfx.empty()) 
        {
            s += pfx;
            s += wxT(' ');
        }
    }

    if (val < 0)
        s += wxT('-'); // "minus" sign

    int scale = fmt.getScale();
    double abs_val = fabs(mmRound(val*scale));

    s += format_groups(fmt, mmMoneyInt(abs_val, scale), 3);
    s += format_cents(fmt, mmCents(abs_val, scale));

    if (!for_edit)
        s += fmt.getSuffix();

    rdata = s;
}
//----------------------------------------------------------------------------

} // namespace

//----------------------------------------------------------------------------
mmIniOptions::mmIniOptions()
    : enableAssets_(true)
    , enableBudget_(true)
    , enableGraphs_(true)
    , enableCustomLogo_(false)
    , enableAddAccount_(true)
    , enableRepeatingTransactions_(true)
    , enableCheckForUpdates_(true)
    , enableReportIssues_ (true)
    , enableBeNotifiedForNewReleases_(true)
    , enableVisitWebsite_(true)
    , enableCustomAboutDialog_(false)
    , disableCategoryModify_(false)
    , enableDeleteAccount_(true)
    , enableCustomTemplateDB_(false)
    , fontSize_(wxT("3"))
    , expandBankHome_(true)
    , expandTermHome_(false)
    , expandStocksHome_(true)
    , expandBankTree_(true)
    , expandTermTree_(false)
    , budgetFinancialYears_(false)
    , budgetIncludeTransfers_(false)
    , budgetSetupWithoutSummaries_(false)
    , budgetSummaryWithoutCategories_(true)
    , ignoreFutureTransactions_(false)
    , transPayeeSelectionNone_(0)
    , transCategorySelectionNone_(0)
    , transStatusReconciled_(0)
    {}

mmIniOptions&
mmIniOptions::instance()
{
    return mmex::Singleton<mmIniOptions>::instance();
}
//----------------------------------------------------------------------------

void correctEmptyFileExt(wxString ext, wxString & fileName)
{
    wxFileName tempFileName(fileName);
    if (tempFileName.GetExt().IsEmpty())
        fileName << wxT(".") << ext;
}
//----------------------------------------------------------------------------
mmOptions::mmOptions()
        : dateFormat(mmex::DEFDATEFORMAT)
        , language(wxT("english"))
        , databaseUpdated_(false)
        {}
//----------------------------------------------------------------------------
mmOptions&
mmOptions::instance()
{
    return mmex::Singleton<mmOptions>::instance();
}
//----------------------------------------------------------------------------

void mmOptions::loadOptions(wxSQLite3Database* db)
{
    dateFormat = mmDBWrapper::getInfoSettingValue(db, wxT("DATEFORMAT"), mmex::DEFDATEFORMAT);

    financialYearStartDayString_   = mmDBWrapper::getInfoSettingValue(db, wxT("FINANCIAL_YEAR_START_DAY"), wxT("1"));
    financialYearStartMonthString_ = mmDBWrapper::getInfoSettingValue(db, wxT("FINANCIAL_YEAR_START_MONTH"), wxT("7"));

}
//----------------------------------------------------------------------------

void mmOptions::saveOptions(wxSQLite3Database* db)
{
    mmDBWrapper::setInfoSettingValue(db, wxT("DATEFORMAT"), dateFormat);
}
// --------------------------------------------------------------------------

void mmIniOptions::loadOptions(wxSQLite3Database* db)
{
    if (mmDBWrapper::getINISettingValue(db, wxT("ENABLESTOCKS"), wxT("TRUE")) != wxT("TRUE")) expandStocksHome_ = false;
    if (mmDBWrapper::getINISettingValue(db, wxT("ENABLEASSETS"), wxT("TRUE")) != wxT("TRUE")) enableAssets_ = false;
    if (mmDBWrapper::getINISettingValue(db, wxT("ENABLEBUDGET"), wxT("TRUE")) != wxT("TRUE")) enableBudget_ = false;
    if (mmDBWrapper::getINISettingValue(db, wxT("ENABLEGRAPHS"), wxT("TRUE")) != wxT("TRUE")) enableGraphs_ = false;

    fontSize_ = mmDBWrapper::getINISettingValue(db, wxT("HTMLFONTSIZE"), wxT("3"));

    if (mmDBWrapper::getINISettingValue(db, wxT("EXPAND_BANK_HOME"), wxT("TRUE")) != wxT("TRUE"))   expandBankHome_ = false;
    if (mmDBWrapper::getINISettingValue(db, wxT("EXPAND_TERM_HOME"), wxT("FALSE")) != wxT("FALSE")) expandTermHome_ = true;
    if (mmDBWrapper::getINISettingValue(db, wxT("EXPAND_BANK_TREE"), wxT("TRUE")) != wxT("TRUE"))   expandBankTree_ = false;
    if (mmDBWrapper::getINISettingValue(db, wxT("EXPAND_TERM_TREE"), wxT("FALSE")) != wxT("FALSE")) expandTermTree_ = true;

    if (mmDBWrapper::getINISettingValue(db, INIDB_BUDGET_FINANCIAL_YEARS, wxT("FALSE")) != wxT("FALSE")) budgetFinancialYears_ = true;
    if (mmDBWrapper::getINISettingValue(db, INIDB_BUDGET_INCLUDE_TRANSFERS, wxT("FALSE")) != wxT("FALSE")) budgetIncludeTransfers_ = true;
    if (mmDBWrapper::getINISettingValue(db, INIDB_BUDGET_SETUP_WITHOUT_SUMMARY, wxT("FALSE")) != wxT("FALSE")) budgetSetupWithoutSummaries_ = true;
    if (mmDBWrapper::getINISettingValue(db, INIDB_BUDGET_SUMMARY_WITHOUT_CATEG, wxT("TRUE")) != wxT("TRUE")) budgetSummaryWithoutCategories_ = false;
    if (mmDBWrapper::getINISettingValue(db, INIDB_IGNORE_FUTURE_TRANSACTIONS, wxT("FALSE")) != wxT("FALSE")) ignoreFutureTransactions_ = true;

    // Read the preference as a string and convert to int
    transPayeeSelectionNone_ = wxAtoi(mmDBWrapper::getINISettingValue(db, wxT("TRANSACTION_PAYEE_NONE"), wxT("0")));
    // For the category selection, default behavior should remain that the last category used for the payee is selected.
    //  This is item 1 (0-indexed) in the list.
    transCategorySelectionNone_ = wxAtoi(mmDBWrapper::getINISettingValue(db, wxT("TRANSACTION_CATEGORY_NONE"), wxT("1")));
    transStatusReconciled_ = wxAtoi(mmDBWrapper::getINISettingValue(db, wxT("TRANSACTION_STATUS_RECONCILED"), wxT("0")));
}

void mmIniOptions::loadInfoOptions(wxSQLite3Database* db)
{
    mmIniOptions::instance().userNameString_ = mmDBWrapper::getInfoSettingValue(db, wxT("USERNAME"), wxT(""));
}

void mmIniOptions::saveOptions(wxSQLite3Database* /*db*/)
{
}

// ---------------------------------------------------------------------------
void mmPlayTransactionSound(wxSQLite3Database* db_)
{
    wxString useSound = mmDBWrapper::getINISettingValue(db_, INIDB_USE_TRANSACTION_SOUND, wxT("TRUE"));

    if (useSound == wxT("TRUE")) 
    {
        wxSound registerSound(mmex::getPathResource(mmex::TRANS_SOUND));
        if (registerSound.IsOk())
            registerSound.Play(wxSOUND_ASYNC);
    }
}

/*
    locale.AddCatalog(lang) calls wxLogWarning and returns true for corrupted .mo file,
    so I should use locale.IsLoaded(lang) also.
*/
wxString mmSelectLanguage(wxWindow *parent, wxSQLite3Database* inidb, bool forced_show_dlg, bool save_setting)
{
    wxString lang;

    const wxString langPath = mmex::getPathShared(mmex::LANG_DIR);
    wxLocale &locale = wxGetApp().getLocale();
    bool verbose = forced_show_dlg;

    if (wxDir::Exists(langPath)) 
    {
        locale.AddCatalogLookupPathPrefix(langPath);
    } 
    else 
    {
        if (verbose) 
        {
            wxString s = wxT("Directory of language files does not exist:\n\"");
            s << langPath << wxT('\"');
            wxMessageDialog dlg(parent, s, wxT("Error"), wxICON_ERROR);
            dlg.ShowModal();
        }

        return lang;
    }

    if (!forced_show_dlg) 
    {
        lang = mmDBWrapper::getINISettingValue(inidb, LANGUAGE_PARAMETER);
        if (!lang.empty() && locale.AddCatalog(lang) && locale.IsLoaded(lang)) 
        {
            mmOptions::instance().language = lang;
            return lang;
        }
    }

    lang = selectLanguageDlg(parent, langPath, verbose);

    if (save_setting && !lang.empty()) 
    {
        bool ok = locale.AddCatalog(lang) && locale.IsLoaded(lang);
        if (!ok)  lang.clear(); // bad .mo file
        mmOptions::instance().language = lang;
        mmDBWrapper::setINISettingValue(inidb, LANGUAGE_PARAMETER, lang);
    }

    return lang;
}

wxString mmReadyDisplayString(const wxString& orig)
{
    wxString toReturn = orig;
    toReturn.Replace(wxT("&"), wxT("&&"));
    return toReturn;
}

wxString mmUnCleanString(const wxString& orig)
{
    wxString toReturn = orig;
    toReturn.Replace(wxT("''"), wxT("'"));
    return toReturn;
}

wxString mmGetNiceMonthName(int month)
{
    static const wxString mon[12] = 
    {
        _("January"), _("February"), _("March"), _("April"), _("May "), _("June"),
        _("July"), _("August"), _("September"), _("October"), _("November"), _("December")
    };

    wxASSERT(month >= 0 && month < 12);
    return mon[month];
}

wxString mmGetNiceShortMonthName(int month)
{
    static const wxString mon[12] = 
    {
        _("Jan"), _("Feb"), _("Mar"), _("Apr"), _("May"), _("Jun"),
        _("Jul"), _("Aug"), _("Sep"), _("Oct"), _("Nov"), _("Dec")
    };

    wxASSERT(month >= 0 && month < 12);
    return mon[month];
}

wxString mmGetNiceDateString(const wxDateTime &dt)
{
    static const wxString gDaysInWeek[7] = 
    {
        _("Sunday"), _("Monday"), _("Tuesday"), _("Wednesday"),
        _("Thursday"), _("Friday"), _("Saturday")
    };

    wxString dts(gDaysInWeek[dt.GetWeekDay()] + wxString(wxT(", ")));

//  Discover the date format set by the user
    wxString dateFmt = mmOptions::instance().dateFormat.Mid(1, 1).MakeUpper();

//  Format date as: DDD, DD MMM YYYY
    if (dateFmt == wxT("D")) 
    {
        dts += wxString::Format(wxT("%d"), dt.GetDay()) + wxString(wxT(" "));
        dts += mmGetNiceMonthName(dt.GetMonth()) + wxT(" ");
        dts += wxString::Format(wxT("%d"), dt.GetYear());

//  Format date as: DDD, YYYY MMM DD
    } 
    else if (dateFmt == wxT("Y")) 
    {
        dts += wxString::Format(wxT("%d"), dt.GetYear()) + wxString(wxT(" "));
        dts += mmGetNiceMonthName(dt.GetMonth()) + wxT(" ");
        dts += wxString::Format(wxT("%d"), dt.GetDay());

//  Format date as: DDD, MMM DD, YYYY
    } 
    else 
    {
        dts += mmGetNiceMonthName(dt.GetMonth()) + wxString(wxT(" "));
        dts += wxString::Format(wxT("%d"), dt.GetDay()) + wxT(", ")
            + wxString::Format(wxT("%d"), dt.GetYear());
    }

    return dts;
}

wxString mmGetNiceDateSimpleString(const wxDateTime &dt)
{
    wxString dts = mmGetNiceMonthName(dt.GetMonth()) + wxString(wxT(" "));

//  Discover the date format set by the user
    wxString dateFmt = mmOptions::instance().dateFormat.Mid(1,1).MakeUpper();
//  Format date as: DD MMM YYYY
    if (dateFmt == wxT("D")) 
    {
        dts = wxString::Format(wxT("%d"), dt.GetDay()) + wxString(wxT(" "))
            + dts
            + wxString::Format(wxT("%d"), dt.GetYear());

//  Format date as: YYYY MMM DD
    } 
    else if (dateFmt == wxT("Y")) 
    {
        dts = wxString::Format(wxT("%d"), dt.GetYear()) + wxString(wxT(" "))
            + dts
            + wxString::Format(wxT("%d"), dt.GetDay());

//  Format date as: MMM DD, YYYY
    } 
    else 
    {
        dts += wxString::Format(wxT("%d"), dt.GetDay()) + wxT(", ")
            + wxString::Format(wxT("%d"), dt.GetYear());
    }

    return dts;
}

void mmShowErrorMessage(wxWindow *parent, const wxString &message, const wxString &messageheader)
{
    wxMessageDialog msgDlg(parent, message, messageheader);
    msgDlg.ShowModal();
}

void mmShowErrorMessageInvalid(wxWindow *parent, const wxString &message)
{
    wxString msg = _("Entry ") + message + _(" is invalid.");
    mmShowErrorMessage(parent, msg, _("Invalid Entry"));
}

wxString inQuotes(wxString label)
{
    return wxString() << wxT("\"") << label << wxT("\"");
}

wxString withoutQuotes(wxString label)
{
    wxString result = label;
    result.Replace(wxT("\""), wxT(""));
    return result;
}

void mmExportQIF(mmCoreDB* core, wxSQLite3Database* db_)
{
    if (mmDBWrapper::getNumAccounts(db_) == 0) 
    {
        wxMessageBox(_("No Account available for export"), _("QIF Export"), wxICON_WARNING);
        return;
    }


    wxString delimit = mmDBWrapper::getInfoSettingValue(db_, wxT("DELIMITER"), mmex::DEFDELIMTER);
    wxString q =  wxT("\"");

    wxArrayString as = mmDBWrapper::getAccountsName(db_);
    wxSingleChoiceDialog scd(0, _("Choose Account to Export from:"),_("QIF Export"), as);

    wxString acctName;

    if (scd.ShowModal() == wxID_OK)  acctName = scd.GetStringSelection();
    if (acctName.IsEmpty())  return;

    wxString chooseExt;
    chooseExt << _("QIF Files") << wxT(" (*.qif)|*.qif;*.QIF");
    wxString fileName = wxFileSelector(_("Choose QIF data file to Export"),
                        wxEmptyString, wxEmptyString, wxEmptyString, chooseExt, wxFD_SAVE | wxFD_OVERWRITE_PROMPT);

    if (fileName.IsEmpty()) return;

    correctEmptyFileExt(wxT("qif"), fileName);

    wxFileOutputStream output(fileName);
    wxTextOutputStream text(output);
    int fromAccountID = mmDBWrapper::getAccountID(db_, acctName);
    wxString amtSeparator =  core->accountList_.getAccountCurrencyDecimalChar(fromAccountID);

    static const char sql[] =
            "SELECT transid, transdate as DATE, "
            "transcode as TRANSACTIONTYPE, transamount as AMOUNT, totransamount as TOAMOUNT, "
            " SUBCATEGID, "
            "CATEGID, PAYEEID, "
            "TRANSACTIONNUMBER, NOTES, TOACCOUNTID, ACCOUNTID "
            "FROM checkingaccount_v1 "
            "WHERE ACCOUNTID = ? OR TOACCOUNTID = ?"
            "ORDER BY transdate";

    wxSQLite3Statement st = db_->PrepareStatement(sql);
    st.Bind(1, fromAccountID);
    st.Bind(2, fromAccountID);

    wxSQLite3ResultSet q1 = st.ExecuteQuery();
    int numRecords = 0;

    text << wxT("!Account") << endl
         << wxT("N") << acctName <<  endl
         << wxT("TChecking") << endl
         << wxT("^") <<  endl
         << wxT("!Type:Cash") << endl;

    while (q1.NextRow()) 
    {
        wxString transid = q1.GetString(wxT("TRANSID"));
        wxString dateDBString = q1.GetString(wxT("DATE"));
        wxDateTime dtdt = mmGetStorageStringAsDate(dateDBString);
        wxString dateString = mmGetDateForDisplay(db_, dtdt);

        int sid, cid;
        wxString payee = mmDBWrapper::getPayee(db_, q1.GetInt(wxT("PAYEEID")), sid, cid);
        wxString type = q1.GetString(wxT("TRANSACTIONTYPE"));
                
        wxString amount = adjustedExportAmount(amtSeparator, q1.GetString(wxT("AMOUNT")));
        //Amount should be formated
        double value = 0.0;
        mmex::formatCurrencyToDouble(amount, value);
        mmex::formatDoubleToCurrencyEdit(value, amount);
                
        wxString toamount = q1.GetString(wxT("TOAMOUNT"));
        //Amount should be formated
        value = 0.0;
        mmex::formatCurrencyToDouble(toamount, value);
        mmex::formatDoubleToCurrencyEdit(value, toamount);
                
                
        wxString transNum = q1.GetString(wxT("TRANSACTIONNUMBER"));
        wxString categ = mmDBWrapper::getCategoryName(db_, q1.GetInt(wxT("CATEGID")));
        wxString subcateg = mmDBWrapper::getSubCategoryName(db_,
                            q1.GetInt(wxT("CATEGID")), q1.GetInt(wxT("SUBCATEGID")));
        wxString notes = mmUnCleanString(q1.GetString(wxT("NOTES")));
                
        //
        notes.Replace(wxT("\n"), wxT(" "));
        wxString subcategStr = wxT ("") ;

        if (type == wxT("Transfer")) 
        {
            subcategStr = type;
            int tAccountID = q1.GetInt(wxT("TOACCOUNTID"));
            int fAccountID = q1.GetInt(wxT("ACCOUNTID"));

            wxString fromAccount = mmDBWrapper::getAccountName(db_,  fAccountID);
            wxString toAccount = mmDBWrapper::getAccountName(db_,  tAccountID);

            if (tAccountID == fromAccountID) {
                payee = fromAccount;
                amount = toamount;
            } else if (fAccountID == fromAccountID) {
                payee = toAccount;
                amount = wxT ('-') + amount;
            }
        }
        else
        {
            subcategStr << categ << (subcateg != wxT("") ? wxT(":") : wxT("")) << subcateg;    
        }

        text << wxT('D') << dateString << endl
             << wxT('T') << (type == wxT("Withdrawal") ? wxT("-") : wxT("")) << amount << endl //FIXME: is T needed when Transfer?
             << wxT('P') << payee << endl
             << wxT('N') << transNum << endl
             //Category or Transfer
             << wxT('L') << subcategStr << endl
             << wxT('M') << notes << endl;
        if (type == wxT("Transfer")) 
        {
            text << wxT('$') << amount << endl;
        }
                
        //if categ id is empty the transaction has been splited
        if (categ.IsEmpty() && subcateg.IsEmpty()) 
        {
            static const char sql4splitedtrx[] =
                    "SELECT SUBCATEGID, CATEGID, SPLITTRANSAMOUNT "
                    "FROM splittransactions_v1 "
                    "WHERE TRANSID = ?";

            wxSQLite3Statement st2 = db_->PrepareStatement(sql4splitedtrx);
            st2.Bind(1, transid);

            wxSQLite3ResultSet q2 = st2.ExecuteQuery();

            while (q2.NextRow()) 
            {
                wxString splitamount = adjustedExportAmount(amtSeparator,q2.GetString(wxT("SPLITTRANSAMOUNT")));
                //Amount should be formated
                value = 0.0;
                mmex::formatCurrencyToDouble(splitamount, value);
                mmex::formatDoubleToCurrencyEdit(value, splitamount);
                wxString splitcateg = mmDBWrapper::getCategoryName(db_, q2.GetInt(wxT("CATEGID")));
                wxString splitsubcateg = mmDBWrapper::getSubCategoryName(db_,
                                            q2.GetInt(wxT("CATEGID")), q2.GetInt(wxT("SUBCATEGID")));
                text << wxT('S') << splitcateg << (splitsubcateg != wxT("") ? wxT(":") : wxT("")) << splitsubcateg << endl
                << wxT('$') << (type == wxT("Withdrawal") ? wxT("-") : wxT("")) << splitamount << endl
                // E Split memo — any text to go with this split item. I saggest Category:Subcategory = Amount for earch line
                << wxT('E') << splitcateg << (splitsubcateg != wxT("") ? wxT(":") : wxT("")) << splitsubcateg << (type == wxT("Withdrawal") ? wxT(" -") : wxT(" ")) << splitamount << endl;
            }

            q2.Finalize();
        }

        text << wxT('^') << endl;
        numRecords++;
    }

    q1.Finalize();

    wxString msg = wxString::Format(wxT("%d transactions exported"), numRecords);
    mmShowErrorMessage(0, msg, _("Export to QIF"));
}

wxString mmGetDateForDisplay(wxSQLite3Database* /*db*/, const wxDateTime &dt)
{
    return dt.Format(mmOptions::instance().dateFormat);
}

wxDateTime mmParseDisplayStringToDate(wxSQLite3Database* db, const wxString& dtstr)
{
    wxString selection = mmDBWrapper::getInfoSettingValue(db, wxT("DATEFORMAT"), mmex::DEFDATEFORMAT);
    wxDateTime dt;
    const wxChar* char1 = dt.ParseFormat(dtstr.GetData(), selection.GetData());

    if (char1 == NULL)
        return wxDateTime::Now();

    return dt;
}

wxString mmGetDateForStorage(const wxDateTime &dt)
{
    return dt.FormatISODate();
}

wxDateTime mmGetStorageStringAsDate(const wxString& str)
{
    wxDateTime dt;
    dt.ParseDate(str.GetData());
    return dt;
}

wxColour mmGetColourFromString(const wxString& str)
{
    wxStringTokenizer tkz(str, wxT(","), wxTOKEN_RET_EMPTY_ALL);
    unsigned char red = 0xFF;
    unsigned char blue = 0xFF;
    unsigned char green = 0xFF;

    if (tkz.HasMoreTokens()) 
    {
        long longVal;
        tkz.GetNextToken().ToLong(&longVal);
        red = longVal;

        if (tkz.HasMoreTokens()) 
        {
            tkz.GetNextToken().ToLong(&longVal);
            green = longVal;

            if (tkz.HasMoreTokens()) 
            {
                tkz.GetNextToken().ToLong(&longVal);
                blue = longVal;
            }
        }
    }

    return wxColour(red, green, blue);
}

wxString mmGetStringFromColour(wxColour color)
{
    return wxString::Format(wxT("%d,%d,%d"), color.Red(), color.Green(), color.Blue());
}

void mmLoadColorsFromDatabase(wxSQLite3Database* db_)
{
    mmColors::listAlternativeColor0 = mmGetColourFromString(mmDBWrapper::getINISettingValue(db_,
                                      wxT("LISTALT0"), wxT("225,237,251")));
    mmColors::listAlternativeColor1 = mmGetColourFromString(mmDBWrapper::getINISettingValue(db_,
                                      wxT("LISTALT1"), wxT("255,255,255")));
    mmColors::listBackColor = mmGetColourFromString(mmDBWrapper::getINISettingValue(db_,
                              wxT("LISTBACK"), wxT("255,255,255")));
    mmColors::navTreeBkColor = mmGetColourFromString(mmDBWrapper::getINISettingValue(db_,
                               wxT("NAVTREE"), wxT("255,255,255")));
    mmColors::listBorderColor = mmGetColourFromString(mmDBWrapper::getINISettingValue(db_,
                                wxT("LISTBORDER"), wxT("0,0,0")));
    mmColors::listDetailsPanelColor = mmGetColourFromString(mmDBWrapper::getINISettingValue(db_,
                                      wxT("LISTDETAILSPANEL"), wxT("244,247,251")));
    mmColors::listFutureDateColor = mmGetColourFromString(mmDBWrapper::getINISettingValue(db_,
                                    wxT("LISTFUTUREDATES"), wxT("116,134,168")));
}


/* Set the default colors */
wxColour mmColors::listAlternativeColor0 = wxColour(225, 237, 251);
wxColour mmColors::listAlternativeColor1 = wxColour(255, 255, 255);
wxColour mmColors::listBackColor = wxColour(255, 255, 255);
wxColour mmColors::navTreeBkColor = wxColour(255, 255, 255);
wxColour mmColors::listBorderColor = wxColour(0, 0, 0);
wxColour mmColors::listDetailsPanelColor = wxColour(244, 247, 251);
wxColour mmColors::listFutureDateColor = wxColour(116, 134, 168);

//----------------------------------------------------------------------------

mmex::CurrencyFormatter::CurrencyFormatter()
{
    loadDefaultSettings();
}
//----------------------------------------------------------------------------

void mmex::CurrencyFormatter::loadDefaultSettings()
{
    m_pfx_symbol = wxT("$");
    m_sfx_symbol.clear();

    m_decimal_point = g_def_decimal_point;
    m_group_separator = wxT(',');
        
    m_unit_name = wxT("dollar");
    m_cent_name = wxT("cent");
        
    m_scale = g_def_scale;
}
//----------------------------------------------------------------------------

void mmex::CurrencyFormatter::loadSettings(
    const wxString &pfx, 
    const wxString &sfx, 
    wxChar dec, 
    wxChar grp,
    const wxString &unit, 
    const wxString &cent, 
    int scale 
     )
{
    m_pfx_symbol = pfx;
    m_sfx_symbol = sfx;

    m_decimal_point = wxIsprint(dec) ? dec : g_def_decimal_point;
    m_group_separator = grp;
        
    m_unit_name = unit;
    m_cent_name = cent;
        
    m_scale = scale > 0 ? scale : g_def_scale;
}
//----------------------------------------------------------------------------

void mmex::CurrencyFormatter::loadSettings(const mmCurrency &cur)
{
    wxChar dec = cur.dec_.empty() ? wxT('\0') : cur.dec_.GetChar(0);
    wxChar grp = cur.grp_.empty() ? wxT('\0') : cur.grp_.GetChar(0);

    loadSettings(cur.pfxSymbol_, cur.sfxSymbol_, dec, grp, cur.unit_, cur.cent_, cur.scaleDl_);
}
//----------------------------------------------------------------------------

mmex::CurrencyFormatter& mmex::CurrencyFormatter::instance()
{
    return mmex::Singleton<mmex::CurrencyFormatter>::instance();
}
//----------------------------------------------------------------------------

void mmex::formatDoubleToCurrencyEdit(double val, wxString& rdata)
{
    const CurrencyFormatter &fmt = CurrencyFormatter::instance();
    DoubleToCurrency(fmt, val, rdata, true);
}
//----------------------------------------------------------------------------

void mmex::formatDoubleToCurrency(double val, wxString& rdata)
{
    const CurrencyFormatter &fmt = CurrencyFormatter::instance();
    DoubleToCurrency(fmt, val, rdata, false);
}
//----------------------------------------------------------------------------

bool mmex::formatCurrencyToDouble(const wxString& str, double& val)
{
    val = 0;
    const CurrencyFormatter &fmt = CurrencyFormatter::instance();
    wxString s = str;
    // remove separators from the amount.
    wxString gs = fmt.getGroupSeparator();
    s.Replace(gs, wxEmptyString);

    // adjust decimal point char to a decimal point.
    wxString gdp = fmt.getDecimalPoint();
    s.Replace(gdp, wxT("."));

    return !s.empty() && s.ToDouble(&val);
}
//----------------------------------------------------------------------------

wxString adjustedExportAmount(wxString amtSeparator, wxString strValue)
{
    // if number does not have a decimal point, add one to user requirements
    int dp = strValue.Find(wxT(".")); 
    if (dp < 0)
        strValue << amtSeparator << wxT("0");
    else 
        strValue.Replace(wxT("."),amtSeparator);

    return strValue;
}

int getTransformedTrxStatus(wxString in)
{
    int out;
    wxString statusStr = in;
    if (statusStr == wxT("R"))
    {
        out = (DEF_STATUS_RECONCILED);
    }
    else if (statusStr == wxT("V"))
    {
        out = (DEF_STATUS_VOID);
    }
    else if (statusStr == wxT("F"))
    {
        out = (DEF_STATUS_FOLLOWUP);
    }
    else if (statusStr == wxT("D"))
    {
        out = (DEF_STATUS_DUPLICATE);
    }
    else 
    {
        out = (DEF_STATUS_NONE);
    }
    return out;
}
wxString getTransformedTrxStatus(int in)
{
    wxString statusStr = wxEmptyString;
    if (in == DEF_STATUS_RECONCILED)
    {
        statusStr = wxT("R");
    }
    else if (in == DEF_STATUS_VOID)
    {
        statusStr = wxT("V");
    }
    else if (in == DEF_STATUS_FOLLOWUP)
    {
        statusStr = wxT("F");
    }
    else if (in == DEF_STATUS_DUPLICATE)
    {
        statusStr = wxT("D");
    }
    else 
    {
        statusStr = wxT("");
    }

    return statusStr;
}

wxString Tips(wxString type)
{
    wxString tipsStr = wxEmptyString;
    if (type == wxT("checkingpanel")) 
    {
        wxArrayString tips; 
        tips.Add(_("Recommendation: Always backup your .mmb database file regularly."));
        tips.Add(_("Recommendation: If upgrading to a new version of MMEX, make sure you backup your .mmb database file before doing so."));
        tips.Add(_("Recommendation: Use copy (Ctrl+С) and paste (Ctrl+V) for frequently used transactions."));
        tips.Add(_("Tip: Remember to make backups of your .mmb."));
        tips.Add(_("Tip: The .mmb file is not encrypted. That means anyone else having the proper know how can actually open the file and read the contents. So make sure that if you are storing any sensitive financial information it is properly guarded."));
        tips.Add(_("Tip: To mark a transaction as reconciled, just select the transaction and hit the 'r' or 'R' key. To mark a transaction as unreconciled, just select the transaction and hit the 'u' or 'U' key."));
        tips.Add(_("Tip: To mark a transaction as requiring followup, just select the transaction and hit the 'f' or 'F' key."));
        tips.Add(_("Tip: MMEX supports printing of all reports that can be viewed. The print options are available under the menu, File->Print."));
        tips.Add(_("Tip: You can modify some runtime behavior of MMEX by changing the options in the Options Dialog. "));
        tips.Add(_("Tip: To print a statement with transactions from any arbitary set of criteria, use the transaction filter to select the transactions you want and then do a print from the menu."));
        tips.Add(_("Tip: Set exchange rate for currencies in case if you have accounts with different currencies."));

        tips.Add(_("Organize Categories Dialog Tip: Pressing the h key will cycle through all categories starting with the letter h"));
        tips.Add(_("Organize Categories Dialog Tip: Pressing 2 key combination will cycle through all categories starting with that key combination. Example: Pressing ho will select Homeneeds, Home, House Tax, etc..."));
        tips.Add(_("Organize Payees Dialog Tip: Using the % key as a wildcard when using the filter. Example: %c shows Chemist and Doctor, %c%m shows Chemist only."));

        tips.Add(_("Tip to get out of debt: Pay yourself 10% first. Put this into an account that is hard to touch. Make sure it is a chore to get the money out (you have to drive to the bank), so you will only tap it consciously and for major expenses."));
        tips.Add(_("Tip to get out of debt: Establish an emergency fund.")); 
        tips.Add(_("Tip to get out of debt: Stop acquiring new debt."));
        tips.Add(_("Tip to get out of debt: Create a realistic budget for your expenses."));
        tips.Add(_("Tip to get out of debt: Spend less than you earn."));
        tips.Add(_("Tip to get out of debt: Pay more than the minimum."));
        tips.Add(_("Before going to a shop and buy something: take the time making a list of what you really need. In the shop buy what is in your list."));
        tipsStr = tips[rand() % tips.GetCount()];
    }    
    else if (type == wxT("assets")) 
    {
        tipsStr = _("MMEX allows you to track fixed assets like cars, houses, land and others. Each asset can have its value appreciate by a certain rate per year, depreciate by a certain rate per year, or not change in value. The total assets are added to your total financial worth.");
    }
    else if (type == wxT("stocks")) 
    {
        tipsStr = _("Using MMEX it is possible to track stocks/mutual funds investments.");
    }
    else if (type == wxT("billsdeposits")) 
    {
        wxArrayString tips;
        tips.Add(_("MMEX allows regular payments to be set up as transactions. These transactions can also be regular deposits, or transfers that will occur at some future time. These transactions act a reminder that an event is about to occur, and appears on the Home Page 14 days before the transaction is due. "));
        tips.Add(_("Tip: These transactions can be set up to activate – allowing the user to adjust any values on the due date."));
            
        tipsStr = tips[rand() % tips.GetCount()];
    }

    return tipsStr;
}
