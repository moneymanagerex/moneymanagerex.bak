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

void mmExportCSV( wxSQLite3Database* db_ );
void mmExportQIF( wxSQLite3Database* db_ );
wxString mmReadyDisplayString( const wxString& orig );

int mmImportCSV( mmCoreDB* core );
int mmImportCSVMMNET( mmCoreDB* core );
int mmImportQIF( mmCoreDB* core );

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
};

class mmIniOptions
{
public:
        static void loadOptions( wxSQLite3Database* db );
        static void loadInfoOptions( wxSQLite3Database* db );
        static void saveOptions( wxSQLite3Database* db );

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

namespace collections {

template <class T>
class Matcher
{
public:
	virtual bool Match(const T& t) = 0;
};

template<typename T>
struct CompareTraits_Equal
{
	static bool Equal(const T& t1, const T& t2) { return t1 == t2; }
};

template<typename T>
struct CompareTraits_NotEqual
{
	static bool Equal(const T& t1, const T& t2) { return t1 != t2; }
};

} // namespace collections

namespace boost
{
	inline std::size_t hash_value(const wxChar* string)
	{
		long int_key = 0;

		while (*string)
			int_key += (wxUChar) *string++;
		return int_key;
	}

	inline std::size_t hash_value(const wxString& value)
	{
		return boost::hash_value(value.c_str());
	}
}
//----------------------------------------------------------------------------
#endif // _MM_EX_UTIL_H_
//----------------------------------------------------------------------------

