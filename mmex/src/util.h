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

#include "defs.h"
#include "mmcurrency.h"
#include "reportbase.h"

class mmCoreDB;
class MMEX_IniSettings;

struct ValuePair
{
    wxString label;
    double   amount;
};

class mmListBoxItem: public wxClientData
{
public:
    mmListBoxItem(int index, const wxString& name)
        : index_(index), name_(name)
    {}

    int getIndex() const { return index_; }
    wxString getName() const { return name_; }

private:
    int index_;
    wxString name_;
};

class mmTreeItemData : public wxTreeItemData
{
public:
    mmTreeItemData(int id, bool isBudget)
        : id_(id)
        , isString_(false)
        , isBudgetingNode_(isBudget)
        , report_(0)
    {}
    mmTreeItemData(const wxString& string, mmPrintableBase* report = 0)
        : isString_(true)
        , isBudgetingNode_(false)
        , stringData_(string)
        , report_(report)
    {}
    ~mmTreeItemData()
    {
        if (report_) delete report_;
    }

    int getData() const { return id_; }
    wxString getString() const { return stringData_; }
    mmPrintableBase* get_report() const { return report_; }
    bool isStringData() const { return isString_; }
    bool isBudgetingNode() const { return isBudgetingNode_; }

private:
    int id_;
    bool isString_;
    bool isBudgetingNode_;
    wxString stringData_;
    mmPrintableBase* report_;
};

void mmShowErrorMessageInvalid( wxWindow *parent, const wxString &message );
void mmShowErrorMessage( wxWindow *parent, const wxString &message, const wxString &messageheader );

wxString mmSelectLanguage(wxWindow *parent, std::shared_ptr<MMEX_IniSettings> pIniSettings, bool forced_show_dlg, bool save_setting = true);

wxString mmGetDateForStorage( const wxDateTime &dt );
wxDateTime mmGetStorageStringAsDate( const wxString& str );
wxString mmGetDateForDisplay( const wxDateTime &dt );
bool mmParseDisplayStringToDate(wxDateTime& date, const wxString sDate, wxString sDateMask );
wxString mmGetNiceDateString( const wxDateTime &dt );
wxString mmGetNiceDateSimpleString( const wxDateTime &dt );
wxString mmGetNiceMonthName( int month );
wxString mmGetNiceShortMonthName( int month );
wxString mmGetNiceWeekDayName( int week_day );
wxString DisplayDate2FormatDate(const wxString strDate);
wxString FormatDate2DisplayDate(const wxString strDate);
const wxArrayString date_format();
const wxArrayString date_format_mask();

wxString inQuotes(wxString label, wxString& delimiter);
wxString csv2tab_separated_values(wxString line, wxString& delimit);
wxString mmReadyDisplayString( const wxString& orig );
wxString adjustedExportAmount(wxString amtSeparator, wxString strValue);
void fixFileExt(wxFileName &f, const wxString &ext);

void correctEmptyFileExt(wxString ext, wxString & fileName );

void mmLoadColorsFromDatabase(std::shared_ptr<MMEX_IniSettings> pIniSettings);
wxColour mmGetColourFromString( const wxString& str );
wxString mmGetStringFromColour( wxColour color );

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
    static wxColour userDefColor1;
    static wxColour userDefColor2;
    static wxColour userDefColor3;
    static wxColour userDefColor4;
    static wxColour userDefColor5;
    static wxColour userDefColor6;
    static wxColour userDefColor7;
};
//----------------------------------------------------------------------------


class CurrencyFormatter
{
public:
    CurrencyFormatter();
    static CurrencyFormatter& instance();

    void loadDefaultSettings();
    void loadSettings(const mmCurrency &cur);

    void loadSettings(const wxString &pfx, const wxString &sfx,
                wxChar dec, wxChar grp,
                const wxString &unit, const wxString &cent,
                int scale
                );

    wxString getPrefix() const { return m_pfx_symbol; }
    wxString getSuffix() const { return m_sfx_symbol; }

    wxChar getDecimalPoint() const { return m_decimal_point; }
    wxChar getGroupSeparator() const { return m_group_separator; }

    int getScale() const { return m_scale; }
    int getDec() const { return log10(m_scale); }

private:
    wxString m_pfx_symbol;   // Leading currency symbol
    wxString m_sfx_symbol;   // Trailing currency symbol
    wxChar m_decimal_point;  // Character for 100ths
    wxChar m_group_separator;// Character for 1000nds
    wxString m_unit_name;    // Name of monetary unit
    wxString m_cent_name;    // Name of fraction unit
    int m_scale;
};
//----------------------------------------------------------------------------

namespace mmex
{
void formatDoubleToCurrency( double val, wxString& rdata );
void formatDoubleToCurrencyEdit( double val, wxString& rdata );
bool formatCurrencyToDouble( const wxString& str, double& val );

//----------------------------------------------------------------------------

} // namespace mmex

//----------------------------------------------------------------------------

int getTransformedTrxStatus(const wxString& in);
wxString getTransformedTrxStatus(int in);
wxString Tips(wxString type);
int site_content(const wxString& site, wxString& output);
bool mmCalculator(wxString sInput, wxString& sOutput);

wxDateTime getUserDefinedFinancialYear(bool prevDayRequired = false);

wxImageList* navtree_images_list_();

/// Located here as a function to allow removal in GTK
void windowsFreezeThaw(wxWindow* pWindow);

std::shared_ptr<wxSQLite3Database> static_db_ptr();

#endif // _MM_EX_UTIL_H_
//----------------------------------------------------------------------------
