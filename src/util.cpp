﻿/*******************************************************
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

#include <wx/sstream.h>

#include "util.h"
#include "mmex.h"
#include "import_export/univcsvdialog.h"
#include "paths.h"
#include "constants.h"
#include "singleton.h"
#include "mmCurrencyFormatter.h"
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
    size_t cnt = wxDir::GetAllFiles(langPath, &lang_files, "*.mo");

    if (!cnt)
    {
        if (verbose)
        {
            wxString s = wxString::Format("Can't find language files (.mo) at \"%s\"", langPath);

            wxMessageDialog dlg(parent, s, "Error", wxOK|wxICON_ERROR);
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
    lang = wxGetSingleChoice("Please choose language", "Languages", lang_files, parent);

    return lang.Lower();
}


//----------------------------------------------------------------------------
void correctEmptyFileExt(const wxString& ext, wxString & fileName)
{
    wxFileName tempFileName(fileName);
    if (tempFileName.GetExt().IsEmpty())
        fileName << "." << ext;
}

// ---------------------------------------------------------------------------
//void mmPlayTransactionSound(wxSQLite3Database* db_)
//{
//    wxString useSound = mmDBWrapper::getINISettingValue(db_, INIDB_USE_TRANSACTION_SOUND, "TRUE");
//
//    if (useSound == "TRUE")
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
wxString mmSelectLanguage(wxWindow *parent, MMEX_IniSettings* pIniSettings, bool forced_show_dlg, bool save_setting)
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
            wxString s = "Directory of language files does not exist:\n\"";
            s << langPath << '\"';
            wxMessageDialog dlg(parent, s, "Error", wxOK|wxICON_ERROR);
            dlg.ShowModal();
        }

        return lang;
    }

    if (!forced_show_dlg)
    {
        lang = pIniSettings->GetStringSetting(LANGUAGE_PARAMETER, "english");
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
        pIniSettings->SetSetting(LANGUAGE_PARAMETER, lang);
    }

    return lang;
}

wxString mmGetNiceDateSimpleString(const wxDateTime &dt)
{
    wxString dateFmt = mmOptions::instance().dateFormat_;
    dateFmt.Replace("%Y%m%d", "%Y %m %d");
    dateFmt.Replace(".", " ");
    dateFmt.Replace(",", " ");
    dateFmt.Replace("/", " ");
    dateFmt.Replace("-", " ");
    dateFmt.Replace("%d", wxString::Format("%d", dt.GetDay()));
    dateFmt.Replace("%Y", wxString::Format("%d", dt.GetYear()));
    dateFmt.Replace("%y", wxString::Format("%d", dt.GetYear()).Mid(2,2));
    dateFmt.Replace("%m", wxGetTranslation(wxDateTime::GetEnglishMonthName(dt.GetMonth())));

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
    wxString msg = wxString::Format(_("Entry %s is invalid"), message);
    mmShowErrorMessage(parent, msg, _("Invalid Entry"));
}

wxString inQuotes(wxString label, wxString& delimiter)
{
    if (label.Contains(delimiter) || label.Contains("\""))
    {
        label.Replace("\"","\"\"", true);
        label = wxString() << "\"" << label << "\"";
    }

    label.Replace("\t","    ", true);
    label.Replace("\n"," ", true);
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
    wxString s = "/";

    //For correct date parsing, adjust separator format to: %x/%x/%x
    sDateMask.Replace("`", s);
    sDateMask.Replace("' ", s);
    sDateMask.Replace("/ ", s);
    sDateMask.Replace("'", s);
    sDateMask.Replace("-", s);
    sDateMask.Replace(".", s);
    sDateMask.Replace(",", s);
    sDateMask.Replace(" ", s);

    sDate.Replace("`", s);
    sDate.Replace("' ", s);
    sDate.Replace("/ ", s);
    sDate.Replace("'", s);
    sDate.Replace("-", s);
    sDate.Replace(".", s);
    sDate.Replace(",", s);
    sDate.Replace(" ", s);

    //Bad idea to change date mask here.
    //some dates may be wrong parsed, for example:
    // 1/1/2001 & 01/01/01
    /*if (sDate.Len()<9)
        sDateMask.Replace("%Y", "%y");
    else
        sDateMask.Replace("%y", "%Y");*/

    wxStringTokenizer token(sDate, s);
    double a,b,c;
    wxString t = token.GetNextToken().Trim();
    t.ToDouble(&a);
    t = token.GetNextToken().Trim();
    t.ToDouble(&b);
    t = token.GetNextToken().Trim();
    t.ToDouble(&c);

    bool bResult = true;

    if (((a>999) || (b>999) || (c>999)) && (sDateMask.Contains("%y")))
        return false;
    if ((a<100) && (b<100) && (c<100) && (sDateMask.Contains("%Y")))
        return false;

    sDate = wxString()<<a<<s<<b<<s<<c;
    if (!date.ParseFormat(sDate, sDateMask, wxDateTime::Now()))
        bResult = false;
    date = date.GetDateOnly();
    return bResult;
}

wxDateTime mmGetStorageStringAsDate(const wxString& str)
{
    wxDateTime dt = wxDateTime::Now();
    if (!str.IsEmpty()) dt.ParseDate(str);
    if (!dt.IsValid()) dt = wxDateTime::Now();
    if (dt.GetYear()<100) dt.Add(wxDateSpan::Years(2000));
    return dt;
}

wxColour mmGetColourFromString(const wxString& str)
{
    wxColor color = wxNullColour;
    wxRegEx pattern("([0-9]{1,3}),([0-9]{1,3}),([0-9]{1,3})");
    if (pattern.Matches(str))
    {
        wxString red = pattern.GetMatch(str, 1);
        wxString green = pattern.GetMatch(str, 2);
        wxString blue = pattern.GetMatch(str, 3);

        color = wxColour(wxAtoi(red), wxAtoi(green), wxAtoi(blue));
    }
    else
    {
        color = wxColor(str);
    }
    return color;
}

wxString mmGetStringFromColour(wxColour color)
{
    //TODO: In next releases color settings wtring may be changed
    //return color.GetAsString(wxC2S_HTML_SYNTAX);
    return wxString::Format("%d,%d,%d", color.Red(), color.Green(), color.Blue());

}

void mmLoadColorsFromDatabase(MMEX_IniSettings* pIniSettings)
{
    mmColors::listAlternativeColor0 = mmGetColourFromString(pIniSettings->GetStringSetting("LISTALT0", "225,237,251"));
    mmColors::listAlternativeColor1 = mmGetColourFromString(pIniSettings->GetStringSetting("LISTALT1", "255,255,255"));
    mmColors::listBackColor         = mmGetColourFromString(pIniSettings->GetStringSetting("LISTBACK", "255,255,255"));
    mmColors::navTreeBkColor        = mmGetColourFromString(pIniSettings->GetStringSetting("NAVTREE", "255,255,255"));
    mmColors::listBorderColor       = mmGetColourFromString(pIniSettings->GetStringSetting("LISTBORDER", "0,0,0"));
    mmColors::listDetailsPanelColor = mmGetColourFromString(pIniSettings->GetStringSetting("LISTDETAILSPANEL", "244,247,251"));
    mmColors::listFutureDateColor   = mmGetColourFromString(pIniSettings->GetStringSetting("LISTFUTUREDATES", "116,134,168"));
    mmColors::userDefColor1   = mmGetColourFromString(pIniSettings->GetStringSetting("USER_COLOR1", "255,0,0"));
    mmColors::userDefColor2   = mmGetColourFromString(pIniSettings->GetStringSetting("USER_COLOR2", "255,165,0"));
    mmColors::userDefColor3   = mmGetColourFromString(pIniSettings->GetStringSetting("USER_COLOR3", "255,255,0"));
    mmColors::userDefColor4   = mmGetColourFromString(pIniSettings->GetStringSetting("USER_COLOR4", "0,255,0"));
    mmColors::userDefColor5   = mmGetColourFromString(pIniSettings->GetStringSetting("USER_COLOR5", "0,255,255"));
    mmColors::userDefColor6   = mmGetColourFromString(pIniSettings->GetStringSetting("USER_COLOR6", "0,0,255"));
    mmColors::userDefColor7   = mmGetColourFromString(pIniSettings->GetStringSetting("USER_COLOR7", "0,0,128"));
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

//*-------------------------------------------------------------------------*//

wxString adjustedExportAmount(const wxString& amtSeparator, const wxString& strValue)
{
    // if number does not have a decimal point, add one to user requirements
    wxString value = strValue;
    int dp = value.Find(".");
    if (dp < 0)
        value<< amtSeparator << "0";
    else
        value.Replace(".",amtSeparator);

    return value;
}

wxString Tips(wxString type)
{
    return wxGetTranslation(TIPS[rand() % sizeof(TIPS)/sizeof(wxString)]);
}

//*--------------------------------------------------------------------------*//
std::map<wxString,wxString> date_formats_map()
{
    std::map<wxString, wxString> date_formats;
    date_formats["%d/%m/%y"]="DD/MM/YY";
    date_formats["%d/%m/%Y"]="DD/MM/YYYY";
    date_formats["%d-%m-%y"]="DD-MM-YY";
    date_formats["%d-%m-%Y"]="DD-MM-YYYY";
    date_formats["%d.%m.%y"]="DD.MM.YY";
    date_formats["%d.%m.%Y"]="DD.MM.YYYY";
    date_formats["%d,%m,%y"]="DD,MM,YY";
    date_formats["%d/%m'%Y"]="DD/MM'YYYY";
    date_formats["%d/%m %Y"]="DD/MM YYYY";
    date_formats["%m/%d/%y"]="MM/DD/YY";
    date_formats["%m/%d/%Y"]="MM/DD/YYYY";
    date_formats["%m-%d-%y"]="MM-DD-YY";
    date_formats["%m-%d-%Y"]="MM-DD-YYYY";
    date_formats["%m/%d'%y"]="MM/DD'YY";
    date_formats["%m/%d'%Y"]="MM/DD'YYYY";
    date_formats["%y/%m/%d"]="YY/MM/DD";
    date_formats["%y-%m-%d"]="YY-MM-DD";
    date_formats["%Y/%m/%d"]="YYYY/MM/DD";
    date_formats["%Y-%m-%d"]="YYYY-MM-DD";
    date_formats["%Y.%m.%d"]="YYYY.MM.DD";
    date_formats["%Y%m%d"]="YYYYMMDD";

    return date_formats;
}

//*--------------------------------------------------------------------------*//

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

std::shared_ptr<wxSQLite3Database> static_db_ptr()
{
    static std::shared_ptr<wxSQLite3Database> db(new wxSQLite3Database);

    return db;
}

bool mmCalculator(wxString sInput, wxString& sOutput)
{
    sInput.Replace(")(", ")*(");
    bool bResult = true;
    int a = sInput.Replace("(", "(");
    int b = sInput.Replace(")", ")");
    if (a != b) return false;
    if (a > 0)
    {
        for (size_t i = 0; i < sInput.Len(); i++)
        {
            if (sInput[i] == '(') a += i;
            if (sInput[i] == ')') b += i;
            if (sInput[i] == '(' && i > 0) bResult = bResult && (wxString("(+-*/").Contains(sInput[i-1]));
            if (sInput[i] == ')' && i < sInput.Len()-1) bResult = bResult && wxString(")+-*/").Contains(sInput[i+1]);
        }
        if (a >= b || !bResult) return false;
    }

    wxString sTemp = sInput.Trim().Prepend("(").Append(")");
    wxString midBrackets, sToCalc;
    double dAmount = 0;

    while (sTemp.Contains("(") && bResult)
    {
        dAmount = 0;
        size_t leftPos = sTemp.Find('(', true);
        size_t rightPos = sTemp.find(")", leftPos);
        midBrackets = sTemp.SubString(leftPos, rightPos);
        midBrackets.Replace("(-", "(N");
        sToCalc = midBrackets.SubString(1, midBrackets.Len()-2);
        if (sToCalc.IsEmpty()) bResult = false;
        double dTempAmount;
        sToCalc.Replace("*-", "M");
        sToCalc.Replace("/-", "D");
        sToCalc.Replace("+-", "-");
        sToCalc.Replace("-+", "-");
        sToCalc.Replace("+", "|");
        sToCalc.Replace("-", "|-");
        midBrackets.Replace("(N", "(-");

        wxStringTokenizer token(sToCalc, "|");

        while (token.HasMoreTokens() && bResult)
        {
            double dSubtotal = 1;
            wxString sToken = token.GetNextToken();
            sToken.Replace("M", "|M");
            sToken.Replace("D", "|D");
            sToken.Replace("*", "|*");
            sToken.Replace("/", "|/");
            sToken.Replace("N", "-");
            sToken.Prepend("*");

            wxStringTokenizer token2(sToken, "|");
            while (token2.HasMoreTokens() && bResult)
            {
                wxString sElement = token2.GetNextToken();
                wxString sSign = sElement.Mid(0,1);
                sElement.Remove(0,1);

                if (sElement.ToDouble(&dTempAmount) ||  CurrencyFormatter::formatCurrencyToDouble(sElement, dTempAmount))
                {
                    if (sSign == "*") dSubtotal = dSubtotal*dTempAmount;
                    else if (sSign == "M") dSubtotal = -dSubtotal*dTempAmount;
                    else if (sSign == "/" && dTempAmount != 0) dSubtotal = dSubtotal/dTempAmount;
                    else if (sSign == "D" && dTempAmount != 0) dSubtotal = -dSubtotal/dTempAmount;
                    else bResult = false;
                }
                else
                    bResult = false;
            }
            dAmount += dSubtotal;
        }
        sTemp.Replace(midBrackets, wxString()<<dAmount);
    }
    if (bResult)
    {
        if (sTemp.Contains("(")||sTemp.Contains(")")) bResult = false;
        else
        {
            sOutput = CurrencyFormatter::float2String(dAmount);
            bResult = !sOutput.IsEmpty() && bResult;
        }
    }
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
