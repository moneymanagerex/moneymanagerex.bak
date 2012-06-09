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
//----------------------------------------------------------------------------
#include <wx/sound.h>
#if wxCHECK_VERSION(2,9,0)
#include <wx/unichar.h>
#endif
#include <boost/shared_ptr.hpp>
//----------------------------------------------------------------------------

namespace
{

//----------------------------------------------------------------------------
const wxChar g_def_decimal_point = ('.');
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
    size_t cnt = wxDir::GetAllFiles(langPath, &lang_files, ("*.mo"));

    if (!cnt)
    {
        if (verbose)
        {
            wxString s = ("Can't find language files (.mo) at \"");
            s << langPath << ('\"');

            wxMessageDialog dlg(parent, s, ("Error"), wxICON_ERROR);
            dlg.ShowModal();
        }

        return lang;
    }

    for (size_t i = 0; i < cnt; ++i)
    {
        wxFileName fname(lang_files[i]);
        lang_files[i] = fname.GetName().Left(1).Upper() + fname.GetName().SubString(1,fname.GetName().Len());
    }

    lang_files.Sort(CaseInsensitiveCmp);
    lang = wxGetSingleChoice(("Please choose language"), ("Languages"), lang_files, parent);

    return lang.Lower();
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
    val.Printf(("%.0f"), x);

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
    const wxString fmt[] = { ("%02d"), ("%01d"), ("%03d"), ("%04d") };

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
            s += (' ');
        }
    }

    if (val < 0)
        s += ('-'); // "minus" sign

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
    , enableAddAccount_(true)
    , enableDeleteAccount_(true)
    , enableRepeatingTransactions_(true)
    , enableCustomLogo_(false)
    , enableCheckForUpdates_(true)
    , enableReportIssues_ (true)
    , enableBeNotifiedForNewReleases_(true)
    , enableVisitWebsite_(true)
    , font_size_(3)
    , enableCustomAboutDialog_(false)
    , disableCategoryModify_(false)
    , enableCustomTemplateDB_(false)
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
    , transStatusReconciled_("None")
    , transDateDefault_(0)
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
        fileName << (".") << ext;
}
//----------------------------------------------------------------------------
mmOptions::mmOptions()
        : dateFormat(mmex::DEFDATEFORMAT)
        , language(("english"))
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
    dateFormat = mmDBWrapper::getInfoSettingValue(db, ("DATEFORMAT"), mmex::DEFDATEFORMAT);

    financialYearStartDayString_   = mmDBWrapper::getInfoSettingValue(db, ("FINANCIAL_YEAR_START_DAY"), ("1"));
    financialYearStartMonthString_ = mmDBWrapper::getInfoSettingValue(db, ("FINANCIAL_YEAR_START_MONTH"), ("7"));

}
//----------------------------------------------------------------------------

void mmOptions::saveOptions(wxSQLite3Database* db)
{
    mmDBWrapper::setInfoSettingValue(db, ("DATEFORMAT"), dateFormat);
}
// --------------------------------------------------------------------------

void mmIniOptions::loadOptions()
{
    wxConfigBase *config = wxConfigBase::Get();
    expandStocksHome_ = config->ReadBool("ENABLESTOCKS", true);
    enableAssets_ = config->ReadBool("ENABLEASSETS", true);
    enableBudget_ = config->ReadBool("ENABLEBUDGET", true);
    enableGraphs_ = config->ReadBool("ENABLEGRAPHS", true);

    font_size_ = config->ReadLong("HTMLFONTSIZE", 3);

    expandBankHome_ = config->ReadBool("EXPAND_BANK_HOME", true);
    expandTermHome_ = config->ReadBool("EXPAND_TERM_HOME", true);
    expandBankTree_ = config->ReadBool("EXPAND_BANK_TREE", true);
    expandTermTree_ = config->ReadBool("EXPAND_TERM_TREE", true);

    budgetFinancialYears_ = config->ReadBool(INIDB_BUDGET_FINANCIAL_YEARS, false);
    budgetIncludeTransfers_ = config->ReadBool(INIDB_BUDGET_INCLUDE_TRANSFERS, false);    
    budgetSetupWithoutSummaries_ = config->ReadBool(INIDB_BUDGET_SETUP_WITHOUT_SUMMARY, false);
    budgetSummaryWithoutCategories_ = config->ReadBool(INIDB_BUDGET_SUMMARY_WITHOUT_CATEG, true);
    ignoreFutureTransactions_ = config->ReadBool(INIDB_IGNORE_FUTURE_TRANSACTIONS, false);

    transPayeeSelectionNone_ = config->ReadLong("TRANSACTION_PAYEE_NONE", 0);
    // For the category selection, default behavior should remain that the last category used for the payee is selected.
    //  This is item 1 (0-indexed) in the list.
    transCategorySelectionNone_ = config->ReadLong("TRANSACTION_CATEGORY_NONE", 1);
    wxString t = config->Read("TRANSACTION_STATUS_RECONCILED", "None");
    if (t!= "None"|| t!="Reconciled" || t!="Void" || t!="Follow up" || t!="Duplicate")
        t = "None";
    transStatusReconciled_ = t;
    transDateDefault_ = config->ReadLong("TRANSACTION_DATE_DEFAULT", 0);
}

void mmIniOptions::loadInfoOptions(wxSQLite3Database* db)
{
    mmIniOptions::instance().userNameString_ = mmDBWrapper::getInfoSettingValue(db, ("USERNAME"), (""));
}

void mmIniOptions::saveOptions(wxSQLite3Database* /*db*/)
{
}

// ---------------------------------------------------------------------------
void mmPlayTransactionSound()
{
    wxConfigBase *config = wxConfigBase::Get();
    bool useSound = config->ReadBool(INIDB_USE_TRANSACTION_SOUND, true);

    if (useSound)
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
wxString mmSelectLanguage(wxWindow *parent, bool forced_show_dlg, bool save_setting)
{
    wxConfigBase *config = wxConfigBase::Get();
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
            wxString s = wxString::Format("Directory of language files does not exist:\n\"%s\"", langPath);
            wxMessageDialog dlg(parent, s, ("Error"), wxOK|wxICON_ERROR);
            dlg.ShowModal();
        }

        return lang;
    }

    if (!forced_show_dlg)
    {
        lang = config->Read(LANGUAGE_PARAMETER, "");
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
        config->Write(LANGUAGE_PARAMETER, lang);
    }

    return lang;
}

wxString mmReadyDisplayString(const wxString& orig)
{
    wxString toReturn = orig;
    toReturn.Replace(("&"), ("&&"));
    return toReturn;
}

//TODO this function used ones may be removed
wxString mmUnCleanString(const wxString& orig)
{
    wxString toReturn = orig;
    toReturn.Replace(("''"), ("'"));
    return toReturn;
}

void fixFileExt(wxFileName &f, const wxString &ext)
{
    if (f.GetExt().Lower() != ext) f.SetExt(ext);
}

wxString mmGetNiceMonthName(int month)
{
    static const wxString mon[12] =
    {
        wxTRANSLATE("January"), wxTRANSLATE("February"),
        wxTRANSLATE("March"), wxTRANSLATE("April"),
        wxTRANSLATE("May "), wxTRANSLATE("June"),
        wxTRANSLATE("July"), wxTRANSLATE("August"),
        wxTRANSLATE("September"), wxTRANSLATE("October"),
        wxTRANSLATE("November"), wxTRANSLATE("December")
    };

    wxASSERT(month >= 0 && month < 12);
    return wxGetTranslation(mon[month]);
}

wxString mmGetNiceShortMonthName(int month)
{
    static const wxString mon[12] =
    {
        wxTRANSLATE("Jan"), wxTRANSLATE("Feb"), wxTRANSLATE("Mar"),
        wxTRANSLATE("Apr"), wxTRANSLATE("May"), wxTRANSLATE("Jun"),
        wxTRANSLATE("Jul"), wxTRANSLATE("Aug"), wxTRANSLATE("Sep"),
        wxTRANSLATE("Oct"), wxTRANSLATE("Nov"), wxTRANSLATE("Dec")
    };

    wxASSERT(month >= 0 && month < 12);
    return wxGetTranslation(mon[month]);
}

wxString mmGetNiceWeekDayName(int week_day)
{
    static const wxString gDaysInWeek[7] =
    {
        wxTRANSLATE("Sunday"), wxTRANSLATE("Monday"),
        wxTRANSLATE("Tuesday"), wxTRANSLATE("Wednesday"),
        wxTRANSLATE("Thursday"), wxTRANSLATE("Friday"),
        wxTRANSLATE("Saturday")
    };
    wxASSERT(week_day >= 0 && week_day < 7);
    return wxGetTranslation(gDaysInWeek[week_day]);
}

wxString mmGetShortWeekDayName(const int week_day)
{
    static const wxString gDaysInWeek[7] =
    {
        wxTRANSLATE("Sun"), wxTRANSLATE("Mon"),
        wxTRANSLATE("Tue"), wxTRANSLATE("Wed"),
        wxTRANSLATE("Thu"), wxTRANSLATE("Fri"),
        wxTRANSLATE("Sat")
    };
    wxASSERT(week_day >= 0 && week_day < 7);
    return wxGetTranslation(gDaysInWeek[week_day]);
}

wxString mmGetNiceDateString(const wxDateTime &dt)
{
    wxString dts = wxString() << mmGetNiceWeekDayName(dt.GetWeekDay())
                              << (", ") << mmGetNiceDateSimpleString(dt);
    return dts;
}

wxString mmGetNiceDateSimpleString(const wxDateTime &dt)
{
    wxString dateFmt = mmOptions::instance().dateFormat;
    dateFmt.Replace("%Y%m%d", "%Y %m %d");
    dateFmt.Replace(".", " ");
    dateFmt.Replace(",", " ");
    dateFmt.Replace("/", " ");
    dateFmt.Replace("-", " ");
    dateFmt.Replace("%d", wxString::Format("%d", dt.GetDay()));
    dateFmt.Replace("%Y", wxString::Format("%d", dt.GetYear()));
    dateFmt.Replace("%y", wxString::Format("%d", dt.GetYear()).Mid(2,2));
    dateFmt.Replace("%m", mmGetNiceMonthName(dt.GetMonth()));

    return dateFmt;
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

wxString inQuotes(wxString label, wxString& delimiter)
{
    if (label.Contains(delimiter) || label.Contains(("\"")))
    {
        label.Replace(("\""),("\"\""), true);
        label = wxString() << ("\"") << label << ("\"");
    }

    label.Replace(("\t"),("    "), true);
    label.Replace(("\n"),(" "), true);
    return label;
}

void mmExportQIF(mmCoreDB* core, wxSQLite3Database* db_)
{
    if (core->getNumAccounts() == 0)
    {
        wxMessageBox(_("No Account available for export"), _("QIF Export"), wxICON_WARNING);
        return;
    }


    wxString delimit = mmDBWrapper::getInfoSettingValue(db_, ("DELIMITER"), mmex::DEFDELIMTER);
    wxString q =  ("\"");

    wxArrayString as = core->getAccountsName();
    wxSingleChoiceDialog scd(0, _("Choose Account to Export from:"),_("QIF Export"), as);

    wxString acctName;

    if (scd.ShowModal() == wxID_OK)  acctName = scd.GetStringSelection();
    if (acctName.IsEmpty())  return;

    wxString chooseExt;
    chooseExt << _("QIF Files") << (" (*.qif)|*.qif;*.QIF");
    wxString fileName = wxFileSelector(_("Choose QIF data file to Export"),
                        wxEmptyString, wxEmptyString, wxEmptyString, chooseExt, wxFD_SAVE | wxFD_OVERWRITE_PROMPT);

    if (fileName.IsEmpty()) return;

    correctEmptyFileExt(("qif"), fileName);

    wxFileOutputStream output(fileName);
    wxTextOutputStream text(output);
    int fromAccountID = core->getAccountID(acctName);
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

    text << ("!Account") << endl
         << ("N") << acctName <<  endl
         << ("TChecking") << endl
         << ("^") <<  endl
         << ("!Type:Cash") << endl;

    while (q1.NextRow())
    {
        wxString transid = q1.GetString(("TRANSID"));
        wxString dateDBString = q1.GetString(("DATE"));
        wxDateTime dtdt = mmGetStorageStringAsDate(dateDBString);
        wxString dateString = mmGetDateForDisplay(db_, dtdt);

        int sid, cid;
        wxString payee = mmDBWrapper::getPayee(db_, q1.GetInt(("PAYEEID")), sid, cid);
        wxString type = q1.GetString(("TRANSACTIONTYPE"));

        wxString amount = adjustedExportAmount(amtSeparator, q1.GetString(("AMOUNT")));
        //Amount should be formated
        double value = 0.0;
        mmex::formatCurrencyToDouble(amount, value);
        mmex::formatDoubleToCurrencyEdit(value, amount);

        wxString toamount = q1.GetString(("TOAMOUNT"));
        //Amount should be formated
        value = 0.0;
        mmex::formatCurrencyToDouble(toamount, value);
        mmex::formatDoubleToCurrencyEdit(value, toamount);


        wxString transNum = q1.GetString(("TRANSACTIONNUMBER"));
        wxString categ = core->getCategoryName(q1.GetInt(("CATEGID")));
        wxString subcateg = mmDBWrapper::getSubCategoryName(db_,
                            q1.GetInt(("CATEGID")), q1.GetInt(("SUBCATEGID")));
        wxString notes = mmUnCleanString(q1.GetString(("NOTES")));

        //
        notes.Replace(("\n"), (" "));
        wxString subcategStr = wxT ("") ;

        if (type == ("Transfer"))
        {
            subcategStr = type;
            int tAccountID = q1.GetInt(("TOACCOUNTID"));
            int fAccountID = q1.GetInt(("ACCOUNTID"));

            wxString fromAccount = core->getAccountName(fAccountID);
            wxString toAccount = core->getAccountName(tAccountID);

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
            subcategStr << categ << (subcateg != ("") ? (":") : ("")) << subcateg;
        }

        text << ('D') << dateString << endl
             << ('T') << (type == ("Withdrawal") ? ("-") : ("")) << amount << endl //FIXME: is T needed when Transfer?
             << ('P') << payee << endl
             << ('N') << transNum << endl
             //Category or Transfer
             << ('L') << subcategStr << endl
             << ('M') << notes << endl;
        if (type == ("Transfer"))
        {
            text << ('$') << amount << endl;
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
                wxString splitamount = adjustedExportAmount(amtSeparator,q2.GetString(("SPLITTRANSAMOUNT")));
                //Amount should be formated
                value = 0.0;
                mmex::formatCurrencyToDouble(splitamount, value);
                mmex::formatDoubleToCurrencyEdit(value, splitamount);
                wxString splitcateg = core->getCategoryName(q2.GetInt(("CATEGID")));
                wxString splitsubcateg = mmDBWrapper::getSubCategoryName(db_,
                                            q2.GetInt(("CATEGID")), q2.GetInt(("SUBCATEGID")));
                text << ('S') << splitcateg << (splitsubcateg != ("") ? (":") : ("")) << splitsubcateg << endl
                << ('$') << (type == ("Withdrawal") ? ("-") : ("")) << splitamount << endl
                // E Split memo — any text to go with this split item. I saggest Category:Subcategory = Amount for earch line
                << ('E') << splitcateg << (splitsubcateg != ("") ? (":") : ("")) << splitsubcateg << (type == ("Withdrawal") ? (" -") : (" ")) << splitamount << endl;
            }

            q2.Finalize();
        }

        text << ('^') << endl;
        numRecords++;
    }

    q1.Finalize();

    wxString msg = wxString::Format(("%d transactions exported"), numRecords);
    mmShowErrorMessage(0, msg, _("Export to QIF"));
}

wxString mmGetDateForDisplay(wxSQLite3Database* /*db*/, const wxDateTime &dt)
{
    return dt.Format(mmOptions::instance().dateFormat);
}

wxDateTime mmParseDisplayStringToDate(wxSQLite3Database* /*db*/, const wxString& dtstr, const wxString& date_format)
{
    //wxString date_format = mmDBWrapper::getInfoSettingValue(db, ("DATEFORMAT"), mmex::DEFDATEFORMAT);
    wxString date_mask = date_format;
    if (date_format.IsEmpty())
        wxString date_mask = mmOptions::instance().dateFormat;
    wxString date = dtstr;

    //For correct date parsing, adjust separator format to: %x/%x/%x
    date_mask.Replace(("`"), ("/"));
    date_mask.Replace(("'"), ("/"));
    date_mask.Replace(("-"), ("/"));
    date_mask.Replace(("."), ("/"));
    date_mask.Replace((","), ("/"));
    date.Replace(("`"), ("/"));
    date.Replace(("'"), ("/"));
    date.Replace(("-"), ("/"));
    date.Replace(("."), ("/"));
    date.Replace((","), ("/"));

    if (date.Len()<9)
        date_mask.Replace(("%Y"), ("%y"));
    else
        date_mask.Replace(("%y"), ("%Y"));

    wxDateTime dt;
    dt.ParseFormat(date, date_mask, wxDateTime::Now());
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

void mmLoadColorsFromDatabase()
{
    wxConfigBase *config = wxConfigBase::Get();
    mmColors::listAlternativeColor0 = wxColor(config->Read("LISTALT0", "WHITE"));
    mmColors::listAlternativeColor0 = wxColor(config->Read("LISTALT1", "rgb(225, 237, 251)"));
    mmColors::listBackColor = wxColor(config->Read("LISTBACK", "WHITE"));
    mmColors::navTreeBkColor = wxColor(config->Read("NAVTREE", "WHITE"));
    mmColors::listBorderColor = wxColor(config->Read("LISTBORDER", "BLACK"));
    mmColors::listDetailsPanelColor = wxColor(config->Read("LISTDETAILSPANEL", "rgb(244, 247, 251)"));
    mmColors::listFutureDateColor = wxColor(config->Read("LISTFUTUREDATES", "rgb(116, 134, 168)"));
}


/* Set the default colors */
wxColour mmColors::listAlternativeColor0 = wxColour("rgb(225, 237, 251)");
wxColour mmColors::listAlternativeColor1 = wxColour("rgb(255, 255, 255)");
wxColour mmColors::listBackColor = wxColour("rgb(255, 255, 255)");
wxColour mmColors::navTreeBkColor = wxColour("rgb(255, 255, 255)");
wxColour mmColors::listBorderColor = wxColour("rgb(0, 0, 0)");
wxColour mmColors::listDetailsPanelColor = wxColour("rgb(244, 247, 251)");
wxColour mmColors::listFutureDateColor = wxColour("rgb(116, 134, 168)");

//----------------------------------------------------------------------------

mmex::CurrencyFormatter::CurrencyFormatter()
{
    loadDefaultSettings();
}
//----------------------------------------------------------------------------

void mmex::CurrencyFormatter::loadDefaultSettings()
{
    m_pfx_symbol = ("$");
    m_sfx_symbol.clear();

    m_decimal_point = g_def_decimal_point;
    m_group_separator = (',');

    m_unit_name = ("dollar");
    m_cent_name = ("cent");

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
    int scale)

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
#if wxCHECK_VERSION(2,9,0)
    wxUniChar dec = cur.dec_.IsEmpty() ? wxUniChar('\0') : cur.dec_.GetChar(0);
    wxUniChar grp = cur.grp_.IsEmpty() ? wxUniChar('\0') : cur.grp_.GetChar(0);
#else
    wxChar dec = cur.dec_.IsEmpty() ? wxChar('\0') : cur.dec_.GetChar(0);
    wxChar grp = cur.grp_.IsEmpty() ? wxChar('\0') : cur.grp_.GetChar(0);
#endif

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
    if (s.Find(gs)) s.Replace(gs, wxEmptyString);

    // adjust decimal point char to a decimal point.
    wxString gdp = fmt.getDecimalPoint();
    if (s.Find(gdp)) s.Replace(gdp, ("."));

    return !s.empty() && s.ToDouble(&val);
}
//----------------------------------------------------------------------------

wxString adjustedExportAmount(wxString amtSeparator, wxString strValue)
{
    // if number does not have a decimal point, add one to user requirements
    int dp = strValue.Find(("."));
    if (dp < 0)
        strValue << amtSeparator << ("0");
    else
        strValue.Replace(("."),amtSeparator);

    return strValue;
}

int getTransformedTrxStatus(const wxString& in)
{
    int out;
    if (in == ("R"))
        out = (DEF_STATUS_RECONCILED);
    else if (in == ("V"))
        out = (DEF_STATUS_VOID);
    else if (in == ("F"))
        out = (DEF_STATUS_FOLLOWUP);
    else if (in == ("D"))
        out = (DEF_STATUS_DUPLICATE);
    else
        out = (DEF_STATUS_NONE);

    return out;
}

wxString getTransformedTrxStatus(int in)
{
    wxString statusStr = wxEmptyString;
    if (in == DEF_STATUS_RECONCILED)
        statusStr = ("R");
    else if (in == DEF_STATUS_VOID)
        statusStr = ("V");
    else if (in == DEF_STATUS_FOLLOWUP)
        statusStr = ("F");
    else if (in == DEF_STATUS_DUPLICATE)
        statusStr = ("D");
    else
        statusStr = ("");

    return statusStr;
}

wxString Tips(wxString type)
{
    wxString tipsStr = wxEmptyString;
    if (type == TIPS_ASSETS)
    {
        tipsStr = _("MMEX allows you to track fixed assets like cars, houses, land and others. Each asset can have its value appreciate by a certain rate per year, depreciate by a certain rate per year, or not change in value. The total assets are added to your total financial worth.");
    }
    else if (type == TIPS_STOCK)
    {
        tipsStr = _("Using MMEX it is possible to track stocks/mutual funds investments.");
    }
    else if (type == TIPS_BILLS)
    {
        wxArrayString tips;
        tips.Add(_("MMEX allows regular payments to be set up as transactions. These transactions can also be regular deposits, or transfers that will occur at some future time. These transactions act a reminder that an event is about to occur, and appears on the Home Page 14 days before the transaction is due. "));
        tips.Add(_("Tip: These transactions can be set up to activate – allowing the user to adjust any values on the due date."));

        tipsStr = tips[rand() % tips.GetCount()];
    }
    else
    {
        switch (1 + rand() % 21)
        {
            case 1 : tipsStr = _("Recommendation: Always backup your .mmb database file regularly."); break;
            case 2 : tipsStr = _("Recommendation: If upgrading to a new version of MMEX, make sure you backup your .mmb database file before doing so."); break ;
            case 3 : tipsStr = _("Recommendation: Use copy (Ctrl+С) and paste (Ctrl+V) for frequently used transactions."); break ; break ;
            case 4 : tipsStr = _("Tip: Remember to make backups of your .mmb."); break ;
            case 5 : tipsStr = _("Tip: The .mmb file is not encrypted. That means anyone else having the proper know how can actually open the file and read the contents. So make sure that if you are storing any sensitive financial information it is properly guarded."); break ;
            case 6 : tipsStr = _("Tip: To mark a transaction as reconciled, just select the transaction and hit the 'r' or 'R' key. To mark a transaction as unreconciled, just select the transaction and hit the 'u' or 'U' key."); break ;
            case 7 : tipsStr = _("Tip: To mark a transaction as requiring followup, just select the transaction and hit the 'f' or 'F' key."); break ;
            case 8 : tipsStr = _("Tip: MMEX supports printing of all reports that can be viewed. The print options are available under the menu, File->Print."); break ;
            case 9 : tipsStr = _("Tip: You can modify some runtime behavior of MMEX by changing the options in the Options Dialog. "); break ;
            case 10 : tipsStr = _("Tip: To print a statement with transactions from any arbitary set of criteria, use the transaction filter to select the transactions you want and then do a print from the menu."); break ;
            case 11 : tipsStr = _("Tip: Set exchange rate for currencies in case if you have accounts with different currencies."); break ;

            case 12 : tipsStr = _("Organize Categories Dialog Tip: Pressing the h key will cycle through all categories starting with the letter h"); break ;
            case 13 : tipsStr = _("Organize Categories Dialog Tip: Pressing 2 key combination will cycle through all categories starting with that key combination. Example: Pressing ho will select Homeneeds, Home, House Tax, etc..."); break ;
            case 14 : tipsStr = _("Organize Payees Dialog Tip: Using the % key as a wildcard when using the filter. Example: %c shows Chemist and Doctor, %c%m shows Chemist only."); break ;
    
            case 15 : tipsStr = _("Tip to get out of debt: Pay yourself 10% first. Put this into an account that is hard to touch. Make sure it is a chore to get the money out (you have to drive to the bank), so you will only tap it consciously and for major expenses."); break ;
            case 16 : tipsStr = _("Tip to get out of debt: Establish an emergency fund."); break ;
            case 17 : tipsStr = _("Tip to get out of debt: Stop acquiring new debt."); break ;
            case 18 : tipsStr = _("Tip to get out of debt: Create a realistic budget for your expenses."); break ;
            case 19 : tipsStr = _("Tip to get out of debt: Spend less than you earn."); break ;
            case 20 : tipsStr = _("Tip to get out of debt: Pay more than the minimum."); break ;
            case 21 : tipsStr = _("Before going to a shop and buy something: take the time making a list of what you really need. In the shop buy what is in your list.");
        }
    }
    return tipsStr;
}

wxString csv2tab_separated_values(wxString line, wxString& delimit)
{
    //csv line example:
    //12.02.2010,Payee,-1105.08,Category,Subcategory,,"Fuel ""95"", 42.31 l (24.20) 212366"
    int i=0;
    //Single quotes will be used instead double quotes
    //Replace all single quotes first
    line.Replace(("'"), ("SingleQuotesReplacer12345"));
    //Replace double quotes that used twice to replacer
    line.Replace(("\"\"\"")+delimit+("\"\"\""), ("DoubleQuotesReplacer12345\"")+delimit+("\"DoubleQuotesReplacer12345"));
    line.Replace(("\"\"\"")+delimit, ("DoubleQuotesReplacer12345\"")+delimit);
    line.Replace(delimit+("\"\"\""), delimit+("\"DoubleQuotesReplacer12345"));
    line.Replace(("\"\"")+delimit, ("DoubleQuotesReplacer12345")+delimit);
    line.Replace(delimit+("\"\""), delimit+("DoubleQuotesReplacer12345"));

    //replace delimiter to TAB and double quotes to single quotes
    line.Replace(("\"")+delimit+("\""), ("'\t'"));
    line.Replace(("\"")+delimit, ("'\t"));
    line.Replace(delimit+("\""), ("\t'"));
    line.Replace(("\"\""), ("DoubleQuotesReplacer12345"));
    line.Replace(("\""), ("'"));

    wxString temp_line = wxEmptyString;
    wxString token;
    wxStringTokenizer tkz1(line, ("'"));

    while (tkz1.HasMoreTokens())
    {
        token = tkz1.GetNextToken();
        if (0 == fmod((double)i,2))
            token.Replace(delimit,("\t"));
        temp_line << token;
        i++;
    };
    //Replace back all replacers to the original value
    temp_line.Replace(("DoubleQuotesReplacer12345"), ("\""));
    temp_line.Replace(("SingleQuotesReplacer12345"), ("'"));
    line = temp_line;

    return line;
}

wxString DisplayDate2FormatDate(wxString strDate)
{
    wxArrayString DateFormat = DateFormats();
    wxArrayString itemChoice7Strings = itemChoiceStrings();

    for(size_t i=0; i<DateFormat.Count(); i++)
    {
        if(strDate == itemChoice7Strings[i])
            return DateFormat[i];
    }

    return DateFormat[0];
}

wxString FormatDate2DisplayDate(wxString strDate)
{
    wxArrayString DateFormat = DateFormats();
    wxArrayString itemChoice7Strings = itemChoiceStrings();

    for(size_t i=0; i<DateFormat.Count(); i++)
    {
        if(strDate == DateFormat[i])
            return itemChoice7Strings[i];
    }

    return itemChoice7Strings[0];
}

wxArrayString DateFormats() {

    wxArrayString DateFormat;

    DateFormat.Add(("%d/%m/%y"));
    DateFormat.Add(("%d/%m/%Y"));
    DateFormat.Add(("%d-%m-%y"));
    DateFormat.Add(("%d-%m-%Y"));
    DateFormat.Add(("%d.%m.%y"));
    DateFormat.Add(("%d.%m.%Y"));
    DateFormat.Add(("%d,%m,%y"));
    DateFormat.Add(("%d/%m'%Y"));
    DateFormat.Add(("%d/%m %Y"));
    DateFormat.Add(("%m/%d/%y"));
    DateFormat.Add(("%m/%d/%Y"));
    DateFormat.Add(("%m-%d-%y"));
    DateFormat.Add(("%m-%d-%Y"));
    DateFormat.Add(("%m/%d'%Y"));
    DateFormat.Add(("%y/%m/%d"));
    DateFormat.Add(("%y-%m-%d"));
    DateFormat.Add(("%Y/%m/%d"));
    DateFormat.Add(("%Y-%m-%d"));
    DateFormat.Add(("%Y.%m.%d"));
    DateFormat.Add(("%Y%m%d"));

    return DateFormat;
}

wxArrayString itemChoiceStrings() {

    wxArrayString itemChoice7Strings;

    itemChoice7Strings.Add(("DD/MM/YY"));
    itemChoice7Strings.Add(("DD/MM/YYYY"));
    itemChoice7Strings.Add(("DD-MM-YY"));
    itemChoice7Strings.Add(("DD-MM-YYYY"));
    itemChoice7Strings.Add(("DD.MM.YY"));
    itemChoice7Strings.Add(("DD.MM.YYYY"));
    itemChoice7Strings.Add(("DD,MM,YY"));
    itemChoice7Strings.Add(("DD/MM'YYYY"));
    itemChoice7Strings.Add(("DD/MM YYYY"));
    itemChoice7Strings.Add(("MM/DD/YY"));
    itemChoice7Strings.Add(("MM/DD/YYYY"));
    itemChoice7Strings.Add(("MM-DD-YY"));
    itemChoice7Strings.Add(("MM-DD-YYYY"));
    itemChoice7Strings.Add(("MM/DD'YYYY"));
    itemChoice7Strings.Add(("YY/MM/DD"));
    itemChoice7Strings.Add(("YY-MM-DD"));
    itemChoice7Strings.Add(("YYYY/MM/DD"));
    itemChoice7Strings.Add(("YYYY-MM-DD"));
    itemChoice7Strings.Add(("YYYY.MM.DD"));
    itemChoice7Strings.Add(("YYYYMMDD"));

    return itemChoice7Strings;
}

int site_content(const wxString& site, wxString& output)
{
    int err_code = wxID_OK;
    wxURL url(site);
    if (url.GetError() == wxURL_NOERR) {
        url.GetProtocol().SetTimeout(10); // 10 secs
        unsigned char buf[16084];
        wxInputStream* in_stream = url.GetInputStream();
        if (in_stream) {
            in_stream->Read(buf, 16084);
            size_t bytes_read=in_stream->LastRead();
            delete in_stream;
            buf[bytes_read] = '\0';
            output = wxString::FromAscii((const char *)buf);
        }
        else
            err_code = 2; //Cannot get data from WWW!
    }
    else
        err_code = 1; //Unable to connect

    return err_code;
}
