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

#include <wx/wxprec.h>
#include <boost/shared_ptr.hpp>
#include <boost/static_assert.hpp>

class mmCoreDB;

struct ValuePair {
        wxString label;
        double   amount;
};

void mmShowErrorMessageInvalid( wxWindow *parent, const wxString &message );
void mmShowErrorMessage( wxWindow *parent, const wxString &message, const wxString &messageheader );

wxString mmSelectLanguage( wxWindow *parent, wxSQLite3Database *inidb, bool forced_show_dlg );

wxString mmGetDateForStorage( const wxDateTime &dt );
wxDateTime mmGetStorageStringAsDate( const wxString& str );
wxString mmGetDateForDisplay( wxSQLite3Database* db, const wxDateTime &dt );
wxDateTime mmParseDisplayStringToDate( wxSQLite3Database* db, const wxString& dt );
wxString mmGetNiceDateString( const wxDateTime &dt );
wxString mmGetNiceDateSimpleString( const wxDateTime &dt );
wxString mmGetNiceMonthName( int month );
wxString mmGetNiceShortMonthName( int month );

void mmExportCSV( mmCoreDB* core, wxSQLite3Database* db_ );
void mmExportQIF( mmCoreDB* core, wxSQLite3Database* db_ );
wxString mmReadyDisplayString( const wxString& orig );
wxString adjustedExportAmount(wxString amtSeparator, wxString strValue);

int mmImportCSV( mmCoreDB* core );
int mmImportCSVMMNET( mmCoreDB* core );
int mmImportQIF( mmCoreDB* core, wxString destinationAccountName = wxEmptyString );

void correctEmptyFileExt(wxString ext, wxString & fileName );

void mmLoadColorsFromDatabase( wxSQLite3Database* db_ );
wxColour mmGetColourFromString( const wxString& str );
void mmRestoreDefaultColors();
void mmSaveColorsToDatabase( wxSQLite3Database* db_ );

void mmPlayTransactionSound( wxSQLite3Database* db_ );

wxString mmArrayIntToString( wxArrayInt& arrayint, char& symbol );

/*
   mmOptions caches the options for MMEX
   so that we don't hit the DB that often
   for data.
*/
class mmOptions
{
public:
        static void loadOptions( wxSQLite3Database* db );
        static void saveOptions( wxSQLite3Database* db );

        static wxString dateFormat;
        static wxString language;
        static wxString financialYearStartDayString_;
        static wxString financialYearStartMonthString_;
};

class mmIniOptions
{
public:
        static void loadOptions( wxSQLite3Database* db );
        static void loadInfoOptions( wxSQLite3Database* db );
        static void saveOptions( wxSQLite3Database* db );

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
        static wxString fontSize_;
        static bool enableImportMMNETCSV_;
        static bool enableImportMMCSV_;
        static bool enableCustomAboutDialog_;
        static wxString aboutCompanyName_;
        static bool disableCategoryModify_;
        static wxString userNameString_;
        static bool enableCustomTemplateDB_;
        static wxString customTemplateDB_;

        static bool expandBankHome_;
        static bool expandTermHome_;
        static bool expandStocksHome_;
        static bool expandBankTree_;
        static bool expandTermTree_;

        static int transPayeeSelectionNone_;
        static int transCategorySelectionNone_;
        static int transStatusReconciled_;
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
//----------------------------------------------------------------------------

namespace mmex
{

class CurrencyFormatter
{
public:
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

private:
        wxString m_pfx_symbol;   // Leading currency symbol
        wxString m_sfx_symbol;   // Trailing currency symbol
        wxChar m_decimal_point;  // Character for 100ths
        wxChar m_group_separator;// Character for 1000nds
        wxString m_unit_name;    // Name of monetary unit
        wxString m_cent_name;    // Name of fraction unit
        int m_scale;

        CurrencyFormatter();
};
//----------------------------------------------------------------------------

void formatDoubleToCurrency( double val, wxString& rdata );
void formatDoubleToCurrencyEdit( double val, wxString& rdata );
bool formatCurrencyToDouble( const wxString& str, double& val );

//----------------------------------------------------------------------------

} // namespace mmex

namespace boost
{
 	inline std::size_t hash_value(const wxChar* string)
	{
		long int_key = 0;

		while (*string) {
			int_key += static_cast<wxUChar>(*string++);
	        }

		return int_key;
	}

   /* problems using wxWidgets 2.9.2
	inline std::size_t hash_value(const wxString& value)
	{
		return boost::hash_value(value.c_str());
	}
    */

} // namespace boost

//----------------------------------------------------------------------------

namespace DateTimeProviders {

struct Today
{
	inline static wxDateTime StartRange()
	{
		return wxDateTime::Now().GetDateOnly();
	}
	inline static wxDateTime EndRange()
	{
		return StartRange() + wxTimeSpan(23, 59, 59, 999);
	}
};

template <int year, wxDateTime::Month month, int day>
struct CustomDate
{
	inline static wxDateTime StartRange()
	{
		return wxDateTime(day, month, year, 0, 0, 0, 0);
	}
	inline static wxDateTime EndRange()
	{
		return wxDateTime(day, month, year, 23, 59, 59, 999);
	}
};

template <int Period, typename CurrentData = Today>
struct LastDays
{
	inline static wxDateTime StartRange()
	{
		wxDateTime today = CurrentData::StartRange();
		return today.Subtract((Period - 1) * wxDateSpan::Day());;
	}
	inline static wxDateTime EndRange()
	{
		return CurrentData::EndRange();
	}
};

template <int MonthsAgoStart, int MonthsAgoEnd = 0, typename CurrentData = Today>
struct LastMonths
{
	BOOST_STATIC_ASSERT(MonthsAgoStart >= MonthsAgoEnd);

	inline static wxDateTime StartRange()
	{
		wxDateTime datePast = CurrentData::StartRange().Subtract(MonthsAgoStart * wxDateSpan::Month());
		wxDateTime result(1, datePast.GetMonth(), datePast.GetYear());
        	return result;
	}
	inline static wxDateTime EndRange()
	{
		return StartRange().Add((MonthsAgoStart-MonthsAgoEnd+1) * wxDateSpan::Month()).Subtract(wxTimeSpan(0,0,0,1));
	}
};

template <typename CurrentData = Today>
struct CurrentMonth: public LastMonths<0, 0, CurrentData> { };

}; // namespace DateTimeProviders

int getTransformedTrxStatus(wxString in);
wxString getTransformedTrxStatus(int in);
wxString Tips(wxString type);

// Validators -----------------------------------------------------

class doubleValidator : public wxTextValidator {
public:
    doubleValidator() : wxTextValidator(wxFILTER_INCLUDE_CHAR_LIST)
    {
        wxArrayString list;
        wxString valid_chars(wxT(" 0123456789.,"));
        size_t len = valid_chars.Length(); 
        for (size_t i=0; i<len; i++) {
            list.Add(wxString(valid_chars.GetChar(i)));
        } 
        SetIncludes(list);
    }
};

#endif // _MM_EX_UTIL_H_
//----------------------------------------------------------------------------
