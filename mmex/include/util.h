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
#ifndef _MM_EX_UTIL_H_
#define _MM_EX_UTIL_H_

#include "wx/wxprec.h"
#include "defs.h"

void mmShowErrorMessageInvalid(wxWindow* parent, wxString message);
void mmShowErrorMessage(wxWindow* parent,  wxString message, wxString messageheader);

void mmSelectLanguage(wxSQLite3Database* inidb, bool showSelection=false);

wxString mmGetDateForStorage(wxDateTime dt);
wxDateTime mmGetStorageStringAsDate(const wxString& str);
wxString mmGetDateForDisplay(wxSQLite3Database* db, wxDateTime dt);
wxDateTime mmParseDisplayStringToDate(wxSQLite3Database* db, const wxString& dt);
wxString mmGetNiceDateString(wxDateTime dt);
wxString mmGetNiceDateSimpleString(wxDateTime dt);

wxString mmCleanString(const wxString& orig);
wxString mmUnCleanString(const wxString& orig);

void mmExportCSV(wxSQLite3Database* db_);
wxString mmReadyDisplayString(const wxString& orig);

int mmImportCSV(wxSQLite3Database* db);
int mmImportCSVMMNET(wxSQLite3Database* db_);
int mmImportQIF(wxSQLite3Database* db_);

void mmLoadColorsFromDatabase(wxSQLite3Database* db_);
wxColour mmGetColourFromString(wxString& str);
void mmRestoreDefaultColors();
void mmSaveColorsToDatabase(wxSQLite3Database* db_);


class mmColors
{
public:
    static wxColour listAlternativeColor0;
    static wxColour listAlternativeColor1;
    static wxColour listBorderColor;
    static wxColour listBackColor;
    static wxColour navTreeBkColor;
    static wxColour listDetailsPanelColor;
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
