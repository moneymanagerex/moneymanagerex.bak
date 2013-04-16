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
#include "mmex.h"
#include "univcsvdialog.h"
#include "paths.h"
#include "constants.h"
#include "singleton.h"

#include <wx/sstream.h>
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

            wxMessageDialog dlg(parent, s, wxT("Error"), wxOK|wxICON_ERROR);
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
    lang = wxGetSingleChoice(wxT("Please choose language"), wxT("Languages"), lang_files, parent);

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
, enableAddAccount_(true)
, enableDeleteAccount_(true)
, enableRepeatingTransactions_(true)
, enableCustomLogo_(false)
, enableCheckForUpdates_(true)
, enableReportIssues_ (true)
, enableBeNotifiedForNewReleases_(true)
, enableVisitWebsite_(true)
, html_font_size_(3)
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
, transStatusReconciled_(0)
, transDateDefault_(0)
{}

mmIniOptions& mmIniOptions::instance()
{
    return Singleton<mmIniOptions>::instance();
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
: dateFormat_(mmex::DEFDATEFORMAT)
, language_(wxT("english"))
, databaseUpdated_(false)
{}

//----------------------------------------------------------------------------
mmOptions& mmOptions::instance()
{
    return Singleton<mmOptions>::instance();
}

//----------------------------------------------------------------------------
void mmOptions::loadOptions(MMEX_IniSettings* info_table)
{
    dateFormat_     = info_table->GetStringSetting(wxT("DATEFORMAT"), mmex::DEFDATEFORMAT);
    userNameString_ = info_table->GetStringSetting(wxT("USERNAME"), wxT(""));

    financialYearStartDayString_   = info_table->GetStringSetting(wxT("FINANCIAL_YEAR_START_DAY"), wxT("1"));
    financialYearStartMonthString_ = info_table->GetStringSetting(wxT("FINANCIAL_YEAR_START_MONTH"), wxT("7"));
}

//----------------------------------------------------------------------------
void mmOptions::saveOptions(MMEX_IniSettings* info_table)
{
    info_table->SetStringSetting(wxT("DATEFORMAT"), dateFormat_);
}

// --------------------------------------------------------------------------
void mmIniOptions::loadOptions(wxSharedPtr<MMEX_IniSettings> pIniSettings)
{
    expandStocksHome_ = pIniSettings->GetBoolSetting(wxT("ENABLESTOCKS"), true);
    enableAssets_     = pIniSettings->GetBoolSetting(wxT("ENABLEASSETS"), true);
    enableBudget_     = pIniSettings->GetBoolSetting(wxT("ENABLEBUDGET"), true);
    enableGraphs_     = pIniSettings->GetBoolSetting(wxT("ENABLEGRAPHS"), true);

    html_font_size_   = pIniSettings->GetIntSetting(wxT("HTMLFONTSIZE"), 3);

    expandBankHome_   = pIniSettings->GetBoolSetting(wxT("EXPAND_BANK_HOME"), true);
    expandTermHome_   = pIniSettings->GetBoolSetting(wxT("EXPAND_TERM_HOME"), false);
    expandBankTree_   = pIniSettings->GetBoolSetting(wxT("EXPAND_BANK_TREE"), true);
    expandTermTree_   = pIniSettings->GetBoolSetting(wxT("EXPAND_TERM_TREE"), false);

    budgetFinancialYears_           = pIniSettings->GetBoolSetting(INIDB_BUDGET_FINANCIAL_YEARS, false);
    budgetIncludeTransfers_         = pIniSettings->GetBoolSetting(INIDB_BUDGET_INCLUDE_TRANSFERS, false);
    budgetSetupWithoutSummaries_    = pIniSettings->GetBoolSetting(INIDB_BUDGET_SETUP_WITHOUT_SUMMARY, false);
    budgetSummaryWithoutCategories_ = pIniSettings->GetBoolSetting(INIDB_BUDGET_SUMMARY_WITHOUT_CATEG, true);
    ignoreFutureTransactions_       = pIniSettings->GetBoolSetting(INIDB_IGNORE_FUTURE_TRANSACTIONS, false);

    // Read the preference as a string and convert to int
    transPayeeSelectionNone_ = pIniSettings->GetIntSetting(wxT("TRANSACTION_PAYEE_NONE"), 0);

    // For the category selection, default behavior should remain that the last category used for the payee is selected.
    //  This is item 1 (0-indexed) in the list.
    transCategorySelectionNone_ = pIniSettings->GetIntSetting(wxT("TRANSACTION_CATEGORY_NONE"), 1);
    transStatusReconciled_      = pIniSettings->GetIntSetting(wxT("TRANSACTION_STATUS_RECONCILED"), 0);
    transDateDefault_           = pIniSettings->GetIntSetting(wxT("TRANSACTION_DATE_DEFAULT"), 0);
}

// ---------------------------------------------------------------------------
//void mmPlayTransactionSound(wxSQLite3Database* db_)
//{
//    wxString useSound = mmDBWrapper::getINISettingValue(db_, INIDB_USE_TRANSACTION_SOUND, wxT("TRUE"));
//
//    if (useSound == wxT("TRUE"))
//    {
//        wxSound registerSound(mmex::getPathResource(mmex::TRANS_SOUND));
//        if (registerSound.IsOk())
//            registerSound.Play(wxSOUND_ASYNC);
//    }
//}

/*
    locale.AddCatalog(lang) calls wxLogWarning and returns true for corrupted .mo file,
    so I should use locale.IsLoaded(lang) also.
*/
wxString mmSelectLanguage(wxWindow *parent, wxSharedPtr<MMEX_IniSettings> pIniSettings, bool forced_show_dlg, bool save_setting)
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
            //TODO fix string for proper translation
            wxString s = wxT("Directory of language files does not exist:\n\"");
            s << langPath << wxT('\"');
            wxMessageDialog dlg(parent, s, wxT("Error"), wxOK|wxICON_ERROR);
            dlg.ShowModal();
        }

        return lang;
    }

    if (!forced_show_dlg)
    {
        lang = pIniSettings->GetStringSetting(LANGUAGE_PARAMETER, wxT("english"));
        if (!lang.empty() && locale.AddCatalog(lang) && locale.IsLoaded(lang))
        {
            mmOptions::instance().language_ = lang;
            return lang;
        }
    }

    lang = selectLanguageDlg(parent, langPath, verbose);

    if (save_setting && !lang.empty())
    {
        bool ok = locale.AddCatalog(lang) && locale.IsLoaded(lang);
        if (!ok)  lang.clear(); // bad .mo file
        mmOptions::instance().language_ = lang;
        pIniSettings->SetStringSetting(LANGUAGE_PARAMETER, lang);
    }

    return lang;
}

wxString mmReadyDisplayString(const wxString& orig)
{
    wxString toReturn = orig;
    toReturn.Replace(wxT("&"), wxT("&&"));
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

wxString mmGetNiceWeekDayName(int week_day)
{
    wxASSERT(week_day >= 0 && week_day < 7);
    return wxGetTranslation(gDaysInWeek[week_day]);
}

wxString mmGetNiceDateString(const wxDateTime &dt)
{
    wxString dts = wxString() << mmGetNiceWeekDayName(dt.GetWeekDay())
                              << wxT(", ") << mmGetNiceDateSimpleString(dt);
    return dts;
}

wxString mmGetNiceDateSimpleString(const wxDateTime &dt)
{
    wxString dateFmt = mmOptions::instance().dateFormat_;
    dateFmt.Replace(wxT("%Y%m%d"), wxT("%Y %m %d"));
    dateFmt.Replace(wxT("."), wxT(" "));
    dateFmt.Replace(wxT(","), wxT(" "));
    dateFmt.Replace(wxT("/"), wxT(" "));
    dateFmt.Replace(wxT("-"), wxT(" "));
    dateFmt.Replace(wxT("%d"), wxString::Format(wxT("%d"), dt.GetDay()));
    dateFmt.Replace(wxT("%Y"), wxString::Format(wxT("%d"), dt.GetYear()));
    dateFmt.Replace(wxT("%y"), wxString::Format(wxT("%d"), dt.GetYear()).Mid(2,2));
    dateFmt.Replace(wxT("%m"), mmGetNiceMonthName(dt.GetMonth()));

    return dateFmt;
}

void mmShowErrorMessage(wxWindow *parent
    , const wxString &message, const wxString &messageheader)
{
    wxMessageDialog msgDlg(parent, message, messageheader, wxOK|wxICON_ERROR);
    msgDlg.ShowModal();
}

void mmShowErrorMessageInvalid(wxWindow *parent, const wxString &message)
{
    wxString msg = wxString::Format(_("Entry %s is invalid"), message.c_str());
    mmShowErrorMessage(parent, msg, _("Invalid Entry"));
}

wxString inQuotes(wxString label, wxString& delimiter)
{
    if (label.Contains(delimiter) || label.Contains(wxT("\"")))
    {
        label.Replace(wxT("\""),wxT("\"\""), true);
        label = wxString() << wxT("\"") << label << wxT("\"");
    }

    label.Replace(wxT("\t"),wxT("    "), true);
    label.Replace(wxT("\n"),wxT(" "), true);
    return label;
}

wxString mmGetDateForDisplay(const wxDateTime &dt)
{
    return dt.Format(mmOptions::instance().dateFormat_);
}

bool mmParseDisplayStringToDate(wxDateTime& date, wxString sDate, wxString sDateMask)
{
    if (sDateMask.IsEmpty())
        sDateMask = mmOptions::instance().dateFormat_;
    wxString s = wxT("/");

    //For correct date parsing, adjust separator format to: %x/%x/%x
    sDateMask.Replace(wxT("`"), s);
    sDateMask.Replace(wxT("' "), s);
    sDateMask.Replace(wxT("/ "), s);
    sDateMask.Replace(wxT("'"), s);
    sDateMask.Replace(wxT("-"), s);
    sDateMask.Replace(wxT("."), s);
    sDateMask.Replace(wxT(","), s);
    sDateMask.Replace(wxT(" "), s);

    sDate.Replace(wxT("`"), s);
    sDate.Replace(wxT("' "), s);
    sDate.Replace(wxT("/ "), s);
    sDate.Replace(wxT("'"), s);
    sDate.Replace(wxT("-"), s);
    sDate.Replace(wxT("."), s);
    sDate.Replace(wxT(","), s);
    sDate.Replace(wxT(" "), s);

    //Bad idea to change date mask here.
    //some dates may be wrong parsed, for example:
    // 1/1/2001 & 01/01/01
    /*if (sDate.Len()<9)
        sDateMask.Replace(wxT("%Y"), wxT("%y"));
    else
        sDateMask.Replace(wxT("%y"), wxT("%Y"));*/

    wxStringTokenizer token(sDate, s);
    double a,b,c;
    wxString t = token.GetNextToken().Trim();
    t.ToDouble(&a);
    t = token.GetNextToken().Trim();
    t.ToDouble(&b);
    t = token.GetNextToken().Trim();
    t.ToDouble(&c);

    bool bResult = true;

    if (((a>999) || (b>999) || (c>999)) && (sDateMask.Contains(wxT("%y"))))
        return false;
    if ((a<100) && (b<100) && (c<100) && (sDateMask.Contains(wxT("%Y"))))
        return false;

    sDate = wxString()<<a<<s<<b<<s<<c;
    if (!date.ParseFormat(sDate, sDateMask, wxDateTime::Now()))
        bResult = false;
    date = date.GetDateOnly();
    return bResult;
}

wxString mmGetDateForStorage(const wxDateTime &dt)
{
    return dt.FormatISODate();
}

wxDateTime mmGetStorageStringAsDate(const wxString& str)
{
    wxDateTime dt = wxDateTime::Now();
    if (!str.IsEmpty()) dt.ParseDate(str.GetData());
    if (dt.IsValid()) return dt;
    return wxDateTime::Now();
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

void mmLoadColorsFromDatabase(wxSharedPtr<MMEX_IniSettings> pIniSettings)
{
    mmColors::listAlternativeColor0 = mmGetColourFromString(pIniSettings->GetStringSetting(wxT("LISTALT0"), wxT("225,237,251")));
    mmColors::listAlternativeColor1 = mmGetColourFromString(pIniSettings->GetStringSetting(wxT("LISTALT1"), wxT("255,255,255")));
    mmColors::listBackColor         = mmGetColourFromString(pIniSettings->GetStringSetting(wxT("LISTBACK"), wxT("255,255,255")));
    mmColors::navTreeBkColor        = mmGetColourFromString(pIniSettings->GetStringSetting(wxT("NAVTREE"), wxT("255,255,255")));
    mmColors::listBorderColor       = mmGetColourFromString(pIniSettings->GetStringSetting(wxT("LISTBORDER"), wxT("0,0,0")));
    mmColors::listDetailsPanelColor = mmGetColourFromString(pIniSettings->GetStringSetting(wxT("LISTDETAILSPANEL"), wxT("244,247,251")));
    mmColors::listFutureDateColor   = mmGetColourFromString(pIniSettings->GetStringSetting(wxT("LISTFUTUREDATES"), wxT("116,134,168")));
    mmColors::userDefColor1   = mmGetColourFromString(pIniSettings->GetStringSetting(wxT("USER_COLOR1"), wxT("255,0,0")));
    mmColors::userDefColor2   = mmGetColourFromString(pIniSettings->GetStringSetting(wxT("USER_COLOR2"), wxT("255,165,0")));
    mmColors::userDefColor3   = mmGetColourFromString(pIniSettings->GetStringSetting(wxT("USER_COLOR3"), wxT("255,255,0")));
    mmColors::userDefColor4   = mmGetColourFromString(pIniSettings->GetStringSetting(wxT("USER_COLOR4"), wxT("0,255,0")));
    mmColors::userDefColor5   = mmGetColourFromString(pIniSettings->GetStringSetting(wxT("USER_COLOR5"), wxT("0,255,255")));
    mmColors::userDefColor6   = mmGetColourFromString(pIniSettings->GetStringSetting(wxT("USER_COLOR6"), wxT("0,0,255")));
    mmColors::userDefColor7   = mmGetColourFromString(pIniSettings->GetStringSetting(wxT("USER_COLOR7"), wxT("0,0,128")));
}


/* Set the default colors */
wxColour mmColors::listAlternativeColor0 = wxColour(225, 237, 251);
wxColour mmColors::listAlternativeColor1 = wxColour(255, 255, 255);
wxColour mmColors::listBackColor = wxColour(255, 255, 255);
wxColour mmColors::navTreeBkColor = wxColour(255, 255, 255);
wxColour mmColors::listBorderColor = wxColour(0, 0, 0);
wxColour mmColors::listDetailsPanelColor = wxColour(244, 247, 251);
wxColour mmColors::listFutureDateColor = wxColour(116, 134, 168);

wxColour mmColors::userDefColor1 = wxColour(255,0,0);
wxColour mmColors::userDefColor2 = wxColour(255,165,0);
wxColour mmColors::userDefColor3 = wxColour(255,255,0);
wxColour mmColors::userDefColor4 = wxColour(0,255,0);
wxColour mmColors::userDefColor5 = wxColour(0,255,255);
wxColour mmColors::userDefColor6 = wxColour(0,0,255);
wxColour mmColors::userDefColor7 = wxColour(0,0,128);

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
    return Singleton<mmex::CurrencyFormatter>::instance();
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
    if (s.Find(gdp)) s.Replace(gdp, (wxT(".")));

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

int getTransformedTrxStatus(const wxString& in)
{
    int out;
    if (in == wxT("R"))
        out = (DEF_STATUS_RECONCILED);
    else if (in == wxT("V"))
        out = (DEF_STATUS_VOID);
    else if (in == wxT("F"))
        out = (DEF_STATUS_FOLLOWUP);
    else if (in == wxT("D"))
        out = (DEF_STATUS_DUPLICATE);
    else
        out = (DEF_STATUS_NONE);

    return out;
}

wxString getTransformedTrxStatus(int in)
{
    wxString statusStr = wxEmptyString;
    if (in == DEF_STATUS_RECONCILED)
        statusStr = wxT("R");
    else if (in == DEF_STATUS_VOID)
        statusStr = wxT("V");
    else if (in == DEF_STATUS_FOLLOWUP)
        statusStr = wxT("F");
    else if (in == DEF_STATUS_DUPLICATE)
        statusStr = wxT("D");
    else
        statusStr = wxT("");

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
        tipsStr = wxGetTranslation(TIPS[rand() % sizeof(TIPS)/sizeof(wxString)]);
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
    line.Replace(wxT("'"), wxT("SingleQuotesReplacer12345"));
    //Replace double quotes that used twice to replacer
    line.Replace(wxT("\"\"\"")+delimit+wxT("\"\"\""), wxT("DoubleQuotesReplacer12345\"")+delimit+wxT("\"DoubleQuotesReplacer12345"));
    line.Replace(wxT("\"\"\"")+delimit, wxT("DoubleQuotesReplacer12345\"")+delimit);
    line.Replace(delimit+wxT("\"\"\""), delimit+wxT("\"DoubleQuotesReplacer12345"));
    line.Replace(wxT("\"\"")+delimit, wxT("DoubleQuotesReplacer12345")+delimit);
    line.Replace(delimit+wxT("\"\""), delimit+wxT("DoubleQuotesReplacer12345"));

    //replace delimiter to TAB and double quotes to single quotes
    line.Replace(wxT("\"")+delimit+wxT("\""), wxT("'\t'"));
    line.Replace(wxT("\"")+delimit, wxT("'\t"));
    line.Replace(delimit+wxT("\""), wxT("\t'"));
    line.Replace(wxT("\"\""), wxT("DoubleQuotesReplacer12345"));
    line.Replace(wxT("\""), wxT("'"));

    wxString temp_line = wxEmptyString;
    wxString token;
    wxStringTokenizer tkz1(line, wxT("'"));

    while (tkz1.HasMoreTokens())
    {
        token = tkz1.GetNextToken();
        if (0 == fmod((double)i,2))
            token.Replace(delimit,wxT("\t"));
        temp_line << token;
        i++;
    };
    //Replace back all replacers to the original value
    temp_line.Replace(wxT("DoubleQuotesReplacer12345"), wxT("\""));
    temp_line.Replace(wxT("SingleQuotesReplacer12345"), wxT("'"));
    line = temp_line;

    return line;
}

wxString DisplayDate2FormatDate(wxString strDate)
{
    int i = date_format().Index(strDate.Upper());
    if (i == wxNOT_FOUND)
        return strDate;
    return date_format_mask()[i];
}

wxString FormatDate2DisplayDate(wxString strDate)
{
    int i = date_format_mask().Index(strDate);
    if (i == wxNOT_FOUND) i = 0;
    return date_format()[i];
}

const wxArrayString date_format_mask()
{
    wxArrayString mask;

    mask.Add(wxT("%d/%m/%y"));
    mask.Add(wxT("%d/%m/%Y"));
    mask.Add(wxT("%d-%m-%y"));
    mask.Add(wxT("%d-%m-%Y"));
    mask.Add(wxT("%d.%m.%y"));
    mask.Add(wxT("%d.%m.%Y"));
    mask.Add(wxT("%d,%m,%y"));
    mask.Add(wxT("%d/%m'%Y"));
    mask.Add(wxT("%d/%m %Y"));
    mask.Add(wxT("%m/%d/%y"));
    mask.Add(wxT("%m/%d/%Y"));
    mask.Add(wxT("%m-%d-%y"));
    mask.Add(wxT("%m-%d-%Y"));
    mask.Add(wxT("%m/%d'%y"));
    mask.Add(wxT("%m/%d'%Y"));
    mask.Add(wxT("%y/%m/%d"));
    mask.Add(wxT("%y-%m-%d"));
    mask.Add(wxT("%Y/%m/%d"));
    mask.Add(wxT("%Y-%m-%d"));
    mask.Add(wxT("%Y.%m.%d"));
    mask.Add(wxT("%Y%m%d"));

    return mask;
}

const wxArrayString date_format()
{
    wxArrayString date_format;

    date_format.Add(wxT("DD/MM/YY"));
    date_format.Add(wxT("DD/MM/YYYY"));
    date_format.Add(wxT("DD-MM-YY"));
    date_format.Add(wxT("DD-MM-YYYY"));
    date_format.Add(wxT("DD.MM.YY"));
    date_format.Add(wxT("DD.MM.YYYY"));
    date_format.Add(wxT("DD,MM,YY"));
    date_format.Add(wxT("DD/MM'YYYY"));
    date_format.Add(wxT("DD/MM YYYY"));
    date_format.Add(wxT("MM/DD/YY"));
    date_format.Add(wxT("MM/DD/YYYY"));
    date_format.Add(wxT("MM-DD-YY"));
    date_format.Add(wxT("MM-DD-YYYY"));
    date_format.Add(wxT("MM/DD'YY"));
    date_format.Add(wxT("MM/DD'YYYY"));
    date_format.Add(wxT("YY/MM/DD"));
    date_format.Add(wxT("YY-MM-DD"));
    date_format.Add(wxT("YYYY/MM/DD"));
    date_format.Add(wxT("YYYY-MM-DD"));
    date_format.Add(wxT("YYYY.MM.DD"));
    date_format.Add(wxT("YYYYMMDD"));

    return date_format;
}

// FIXME: Freeze - Thaw is ok for wx2.8.x but not for wx2.9.x
// Located here as a function to allow removal in WXGTK
void windowsFreezeThaw(wxWindow* pWindow)
{
#ifdef __WXGTK__
    return;
#endif

    if (pWindow->IsFrozen()) pWindow->Thaw();
    else pWindow->Freeze();
}

int site_content(const wxString& sSite, wxString& sOutput)
 {
    wxURL url(sSite);
    int err_code = url.GetError();
    if (err_code == wxURL_NOERR)
    {
        url.GetProtocol().SetTimeout(10); // 10 secs
        wxInputStream* in_stream = url.GetInputStream();
        if (in_stream)
        {
            wxStringOutputStream out_stream(&sOutput);
            in_stream->Read(out_stream);
        }
        else
            err_code = -1; //Cannot get data from WWW!
        delete in_stream;
    }

    if (err_code != wxURL_NOERR)
    {
        if      (err_code == wxURL_SNTXERR ) sOutput = _("Syntax error in the URL string");
        else if (err_code == wxURL_NOPROTO ) sOutput = _("Found no protocol which can get this URL");
        else if (err_code == wxURL_NOHOST  ) sOutput = _("A host name is required for this protocol");
        else if (err_code == wxURL_NOPATH  ) sOutput = _("A path is required for this protocol");
        else if (err_code == wxURL_CONNERR ) sOutput = _("Connection error");
        else if (err_code == wxURL_PROTOERR) sOutput = _("An error occurred during negotiation");
        else if (err_code == -1) sOutput = _("Cannot get data from WWW!");
        else sOutput = _("Unknown error");
    }
    return err_code;
}

int mmIniOptions::account_image_id(mmCoreDB* core, int account_id)
{
    double selectedImage = 9;
    wxString image_num_str = core->dbInfoSettings_->GetStringSetting(
        wxString::Format(wxT("ACC_IMAGE_ID_%d"), account_id), wxT(""));
    if (mmex::formatCurrencyToDouble(image_num_str, selectedImage))
    {
        if (selectedImage > 0)
            return selectedImage;
    }

    selectedImage = 9;
    int t = 0, s = 0;
    const wxString acctType = core->accountList_.getAccountType(account_id);
    const int acctStatus = core->accountList_.getAccountStatus(account_id);
    bool favorite = core->accountList_.getAccountFavorite(account_id);

    if (acctStatus == mmAccount::MMEX_Closed)
        s = 2;
    else if (favorite)
        s = 1;

    if (acctType == wxT("Term"))
        t = 3;
    else if (acctType == wxT("Investment"))
        t = 6;

    selectedImage += t + s;

    return selectedImage;
}


wxImageList* navtree_images_list_()
{
    wxImageList* imageList_ = new wxImageList(16, 16);

    imageList_->Add(wxBitmap(wxImage(house_xpm).Scale(16, 16)));
    imageList_->Add(wxBitmap(wxImage(moneyaccount_xpm).Scale(16, 16)));  //TODO: remove
    imageList_->Add(wxBitmap(wxImage(schedule_xpm).Scale(16, 16)));
    imageList_->Add(wxBitmap(wxImage(calendar_xpm).Scale(16, 16)));
    imageList_->Add(wxBitmap(wxImage(chartpiereport_xpm).Scale(16, 16)));
    imageList_->Add(wxBitmap(wxImage(help_xpm).Scale(16, 16)));
    imageList_->Add(wxBitmap(wxImage(stock_curve_xpm).Scale(16, 16))); //TODO: remove
    imageList_->Add(wxBitmap(wxImage(car_xpm).Scale(16, 16)));
    imageList_->Add(wxBitmap(wxImage(customsql_xpm).Scale(16, 16)));
    imageList_->Add(wxBitmap(wxImage(moneyaccount_xpm).Scale(16, 16))); // used for: savings_account
    imageList_->Add(wxBitmap(wxImage(savings_acc_favorite_xpm).Scale(16, 16))); //10
    imageList_->Add(wxBitmap(wxImage(savings_acc_closed_xpm).Scale(16, 16)));
    imageList_->Add(wxBitmap(wxImage(termaccount_xpm).Scale(16, 16))); // used for: term_account
    imageList_->Add(wxBitmap(wxImage(term_acc_favorite_xpm).Scale(16, 16)));
    imageList_->Add(wxBitmap(wxImage(term_acc_closed_xpm).Scale(16, 16)));
    imageList_->Add(wxBitmap(wxImage(stock_acc_xpm).Scale(16, 16))); // used for: invest_account
    imageList_->Add(wxBitmap(wxImage(stock_acc_favorite_xpm).Scale(16, 16))); //TODO: more icons
    imageList_->Add(wxBitmap(wxImage(stock_acc_closed_xpm).Scale(16, 16)));
    imageList_->Add(wxBitmap(wxImage(money_dollar_xpm).Scale(16, 16)));
    imageList_->Add(wxBitmap(wxImage(money_euro_xpm).Scale(16, 16))); //custom icons
    imageList_->Add(wxBitmap(wxImage(flag_xpm).Scale(16, 16)));
    imageList_->Add(wxBitmap(wxImage(accounttree_xpm).Scale(16, 16)));
    imageList_->Add(wxBitmap(wxImage(about_xpm).Scale(16, 16)));
    imageList_->Add(wxBitmap(wxImage(clock_xpm).Scale(16, 16)));
    imageList_->Add(wxBitmap(wxImage(cat_xpm).Scale(16, 16)));
    imageList_->Add(wxBitmap(wxImage(dog_xpm).Scale(16, 16)));
    imageList_->Add(wxBitmap(wxImage(trees_xpm).Scale(16, 16)));
    imageList_->Add(wxBitmap(wxImage(hourglass_xpm).Scale(16, 16)));
    imageList_->Add(wxBitmap(wxImage(work_xpm).Scale(16, 16)));
    imageList_->Add(wxBitmap(wxImage(yandex_money_xpm).Scale(16, 16)));
    imageList_->Add(wxBitmap(wxImage(web_money_xpm).Scale(16, 16)));
    imageList_->Add(wxBitmap(wxImage(rubik_cube_xpm).Scale(16, 16)));

    return imageList_;
}

wxSharedPtr<wxSQLite3Database> static_db_ptr()
{
    static wxSharedPtr<wxSQLite3Database> db(new wxSQLite3Database);

    return db;
}

bool mmCalculator(wxString sInput, wxString& sOutput)
{
    sInput.Replace(wxT(")("), wxT(")*("));
    bool bResult = true;
    int a = sInput.Replace(wxT("("), wxT("("));
    int b = sInput.Replace(wxT(")"), wxT(")"));
    if (a != b) return false;
    if (a > 0)
    {
        for (size_t i = 0; i < sInput.Len(); i++)
        {
            if (sInput[i] == '(') a += i;
            if (sInput[i] == ')') b += i;
            if (sInput[i] == '(' && i > 0) bResult = bResult && (wxString(wxT("(+-*/")).Contains(sInput[i-1]));
            if (sInput[i] == ')' && i < sInput.Len()-1) bResult = bResult && (wxString(wxT(")+-*/")).Contains(sInput[i+1]));
        }
        if (a >= b || !bResult) return false;
    }

    wxString sTemp = sInput.Trim().Prepend(wxT("(")).Append(wxT(")"));
    wxString midBrackets, sToCalc;
    double dAmount = 0;

    while (sTemp.Contains(wxT("(")) && bResult)
    {
        dAmount = 0;
        size_t leftPos = sTemp.Find('(', true);
        size_t rightPos = sTemp.find(wxT(")"), leftPos);
        midBrackets = sTemp.SubString(leftPos, rightPos);
        midBrackets.Replace(wxT("(-"), wxT("(N"));
        sToCalc = midBrackets.SubString(1, midBrackets.Len()-2);
        if (sToCalc.IsEmpty()) bResult = false;
        double dTempAmount;
        sToCalc.Replace(wxT("*-"), wxT("M"));
        sToCalc.Replace(wxT("/-"), wxT("D"));
        sToCalc.Replace(wxT("+-"), wxT("-"));
        sToCalc.Replace(wxT("-+"), wxT("-"));
        sToCalc.Replace(wxT("+"), wxT("|"));
        sToCalc.Replace(wxT("-"), wxT("|-"));
        midBrackets.Replace(wxT("(N"), wxT("(-"));

        wxStringTokenizer token(sToCalc, wxT("|"));

        while (token.HasMoreTokens() && bResult)
        {
            double dSubtotal = 1;
            wxString sToken = token.GetNextToken();
            sToken.Replace(wxT("M"), wxT("|M"));
            sToken.Replace(wxT("D"), wxT("|D"));
            sToken.Replace(wxT("*"), wxT("|*"));
            sToken.Replace(wxT("/"), wxT("|/"));
            sToken.Replace(wxT("N"), wxT("-"));
            sToken.Prepend(wxT("*"));

            wxStringTokenizer token2(sToken, wxT("|"));
            while (token2.HasMoreTokens() && bResult)
            {
                wxString sElement = token2.GetNextToken();
                wxString sSign = sElement.Mid(0,1);
                sElement.Remove(0,1);

                if (sElement.ToDouble(&dTempAmount) || mmex::formatCurrencyToDouble(sElement, dTempAmount))
                {
                    if (sSign == wxT("*")) dSubtotal = dSubtotal*dTempAmount;
                    else if (sSign == wxT("M")) dSubtotal = -dSubtotal*dTempAmount;
                    else if (sSign == wxT("/") && dTempAmount != 0) dSubtotal = dSubtotal/dTempAmount;
                    else if (sSign == wxT("D") && dTempAmount != 0) dSubtotal = -dSubtotal/dTempAmount;
                    else bResult = false;
                }
                else
                    bResult = false;
            }
            dAmount += dSubtotal;
        }
        sTemp.Replace(midBrackets, wxString()<<dAmount);
    }
    if (sTemp.Contains(wxT("("))||sTemp.Contains(wxT(")"))) bResult = false;
    if (bResult) mmex::formatDoubleToCurrencyEdit(dAmount, sOutput);

    return bResult;
}

wxDateTime getUserDefinedFinancialYear(bool prevDayRequired)
{
    long monthNum;
    mmOptions::instance().financialYearStartMonthString_.ToLong(&monthNum);

    if (monthNum > 0) //Test required for compatability with previous version
        monthNum --;

    wxDateTime today = wxDateTime::Now();
    int year = today.GetYear();
    if (today.GetMonth() < monthNum) year -- ;

    long dayNum;
    wxString dayNumStr = mmOptions::instance().financialYearStartDayString_;
    dayNumStr.ToLong(&dayNum);
    if ((dayNum < 1) || (dayNum > 31 )) {
        dayNum = 1;
    } else if (((monthNum == wxDateTime::Feb) && (dayNum > 28)) ||
        (((monthNum == wxDateTime::Sep) || (monthNum == wxDateTime::Apr) ||
           (monthNum == wxDateTime::Jun) || (monthNum == wxDateTime::Nov)) && (dayNum > 29)))
    {
        dayNum = 1;
    }

    wxDateTime financialYear = wxDateTime(today);
    financialYear.SetDay(dayNum);
    financialYear.SetMonth((wxDateTime::Month)monthNum);
    financialYear.SetYear(year);
    if (prevDayRequired)
        financialYear.Subtract(wxDateSpan::Day());
    return financialYear;
}

#if 0
void GetDateRange(wxDateTime &dtBegin, wxDateTime &dtEnd, const wxString sData)
{
    wxDateTime today = wxDateTime::Now();

    wxStringTokenizer token(sData, wxT("-"));
    token.GetNextToken();
    wxString sRange = token.GetNextToken();
    sRange = sRange.SubString(1, sRange.Length());

    if (sRange == wxT("Last Calendar Month"))
    {
        wxDateTime::Month cm = today.GetMonth();
        int numDays = 0;
        if (cm == wxDateTime::Jan)
            numDays = wxDateTime::GetNumberOfDays((wxDateTime::Month)(wxDateTime::Dec));
        else
            numDays = wxDateTime::GetNumberOfDays((wxDateTime::Month)(cm-1));

        wxDateTime prevMonthEnd = today.Subtract(wxDateSpan::Days(today.GetDay()));
        dtEnd = prevMonthEnd;
        dtBegin = prevMonthEnd.Subtract(wxDateSpan::Days(numDays)).Add(wxDateSpan::Day());
    }
    else if (sRange == wxT("30 Days") || sRange == wxT("Last 30 Days"))
    {
        wxDateTime prevMonthEnd = today;
        dtEnd = today;
        dtBegin = today.Subtract(wxDateSpan::Month()).Add(wxDateSpan::Day());
    }
    else if (sRange == wxT("Current Month"))
    {
        wxDateTime prevMonthEnd = today.Subtract(wxDateSpan::Days(today.GetDay()-1));
        dtBegin = prevMonthEnd;
        dtEnd = wxDateTime::Now().GetLastMonthDay();
        if (mmIniOptions::instance().ignoreFutureTransactions_)
        {
            dtEnd = wxDateTime::Now();
        }
    }
    else if (sRange == wxT("Last Year"))
    {
        int year = today.GetYear() - 1;
        wxDateTime prevYearEnd = wxDateTime(today);
        wxDateTime prevYearStart = wxDateTime(today);
        prevYearEnd.SetYear(year);
        prevYearStart.SetYear(year);
        prevYearEnd.SetMonth(wxDateTime::Dec);
        prevYearStart.SetMonth(wxDateTime::Jan);
        prevYearEnd.SetDay(31);
        prevYearStart.SetDay(1);
        dtEnd = prevYearEnd;
        dtBegin = prevYearStart;
    }
    else if (sRange == wxT("Current Year"))
    {
        int year = today.GetYear() - 1;
        wxDateTime yearBegin = wxDateTime(today);
        yearBegin.SetYear(year);
        yearBegin.SetMonth(wxDateTime::Jan);
        yearBegin.SetDay(1);
        dtEnd = today;
        dtBegin = yearBegin;
    }
    else if (sRange == wxT("Last Financial Year"))
    {
        wxDateTime refDate = wxDateTime(getUserDefinedFinancialYear(true));
        dtEnd = refDate;
        dtBegin = refDate.Subtract(wxDateSpan::Year()).Add(wxDateSpan::Day());

    }
    else if (sRange == wxT("Current Financial Year"))
    {
        dtBegin = wxDateTime(getUserDefinedFinancialYear(true)).Add(wxDateSpan::Day());
        dtEnd   = wxDateTime::Now();
    }
    else
        wxASSERT(false);
}
#endif
