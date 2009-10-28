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

#ifndef _MM_EX_UTIL_H_
#define _MM_EX_UTIL_H_

#include "wx/wxprec.h"
#include "boost/shared_ptr.hpp"
#include "defs.h"
#include "mmcurrency.h"

class mmCoreDB;

struct ValuePair
{
    wxString label;
    double   amount;
};

wxString mmGetBaseWorkingPath(bool ignoreCommandLine = false);

void mmShowErrorMessageInvalid(wxWindow* parent, wxString message);
void mmShowErrorMessage(wxWindow* parent,  wxString message, wxString messageheader);

void mmSelectLanguage(wxSQLite3Database* inidb, bool showSelection=false);

wxString mmGetDateForStorage(wxDateTime dt);
wxDateTime mmGetStorageStringAsDate(const wxString& str);
wxString mmGetDateForDisplay(wxSQLite3Database* db, wxDateTime dt);
wxDateTime mmParseDisplayStringToDate(wxSQLite3Database* db, const wxString& dt);
wxString mmGetNiceDateString(wxDateTime dt);
wxString mmGetNiceDateSimpleString(wxDateTime dt);
wxString mmGetNiceMonthName(int month);
wxString mmGetNiceShortMonthName(int month);

void mmExportCSV(wxSQLite3Database* db_);
void mmExportQIF(wxSQLite3Database* db_);
wxString mmReadyDisplayString(const wxString& orig);

int mmImportCSV(mmCoreDB* core);
int mmImportCSVMMNET(mmCoreDB* core);
int mmImportQIF(mmCoreDB* core);

void mmLoadColorsFromDatabase(wxSQLite3Database* db_);
wxColour mmGetColourFromString(const wxString& str);
void mmRestoreDefaultColors();
void mmSaveColorsToDatabase(wxSQLite3Database* db_);

void mmPlayTransactionSound(wxSQLite3Database* db_);

wxString mmArrayIntToString(wxArrayInt& arrayint, char& symbol);

/* 
   mmOptions caches the options for MMEX
   so that we don't hit the DB that often 
   for data. 
*/
class mmOptions
{
public:
   static void loadOptions(wxSQLite3Database* db);
   static void saveOptions(wxSQLite3Database* db);

   static wxString dateFormat;
   static wxString language;
};

class mmIniOptions
{
public:
   static void loadOptions(wxSQLite3Database* db);
   static void loadInfoOptions(wxSQLite3Database* db);
   static void saveOptions(wxSQLite3Database* db);

   static bool enableStocks_;
   static bool enableAssets_;
   static bool enableBudget_;
   static bool enableGraphs_;
   static bool enableAddAccount_;
   static bool enableDeleteAccount_;
   static bool enableRepeatingTransactions_;
   static bool enableCustomLogo_;
   static bool enableCheckForUpdates_;
   static bool enableReportIssues_;
   static bool enableBeNotifiedForNewReleases_;
   static bool enableVisitWebsite_;
   static wxString logoName_;
   static wxString appName_;
   static wxString fontSize_;
   static bool enableImportMMNETCSV_;
   static bool enableImportMMCSV_;
   static bool enableCustomAboutDialog_;
   static wxString aboutCompanyName_;
   static bool disableCategoryModify_;
   static wxString userNameString_;
   static bool enableCustomTemplateDB_;
   static wxString customTemplateDB_;
};

class mmColors
{
public:
    static wxColour listAlternativeColor0;
    static wxColour listAlternativeColor1;
    static wxColour listBorderColor;
    static wxColour listBackColor;
    static wxColour navTreeBkColor;
    static wxColour listDetailsPanelColor;
    static wxColour listFutureDateColor;
};

class mmCurrencyFormatter
{
    public: mmCurrencyFormatter() {}
    public: ~mmCurrencyFormatter() {}

    public: static void loadSettings(wxString pfx, wxString sfx, wxChar dec, wxChar grp,
                                wxString unit, wxString cent, double scale);
    public: static void loadDefaultSettings();
    public: static bool formatDoubleToCurrency(double val, wxString& data);
    public: static bool formatCurrencyToDouble(const wxString& str, double& val);
    public: static bool formatDoubleToCurrencyEdit(double val, wxString& rdata);
    public: static void loadSettings(boost::shared_ptr<mmCurrency> pCurrencyPtr);

    public:
    static wxString pfx_symbol;   // Leading currency symbol (U.S.: '$')
    static wxString sfx_symbol;   // Trailing currency symbol
    static wxChar decimal_point;  // Character for 100ths  (U.S.:  period)
    static wxChar group_separator;// Character for 1000nds (U.S.:  comma)
    static wxString unit_name;    // Name of monetary unit (U.S.:  "dollar")
    static wxString cent_name;    // Name of fraction unit (U.S.:  "cent")
    static double  scale;         // Scale 100
};

#endif
