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
#include <boost/shared_ptr.hpp>
//----------------------------------------------------------------------------

namespace
{

const char g_AccountNameSQL[] =
"select ACCOUNTNAME "
"from ACCOUNTLIST_V1 "
"where (ACCOUNTTYPE = 'Checking' or ACCOUNTTYPE = 'Term') and STATUS != 'Closed' "
"order by ACCOUNTNAME";

//----------------------------------------------------------------------------
const wxChar g_def_decimal_point = wxT('.');
const int g_def_scale = 100;
//----------------------------------------------------------------------------

wxString mmCleanString( const wxString& orig )
{
        wxString toReturn = orig;
        toReturn.Replace( wxT( "'" ), wxT( "''" ) );
        return toReturn;
}
//----------------------------------------------------------------------------

int CaseInsensitiveCmp( const wxString &s1, const wxString &s2 )
{
        return s1.CmpNoCase( s2 );
}
//----------------------------------------------------------------------------

wxString selectLanguageDlg( wxWindow *parent, const wxString &langPath, bool verbose )
{
        wxString lang;

        wxArrayString lang_files;
        size_t cnt = wxDir::GetAllFiles( langPath, &lang_files, wxT( "*.mo" ) );

        if ( !cnt ) {

                if ( verbose ) {
                        wxString s = wxT( "Can't find language files (.mo) at \"" );
                        s << langPath << wxT( '\"' );

                        wxMessageDialog dlg( parent, s, wxT( "Error" ), wxICON_ERROR );
                        dlg.ShowModal();
                }

                return lang;
        }

        for ( size_t i = 0; i < cnt; ++i ) {
                wxFileName fname( lang_files[i] );
                lang_files[i] = fname.GetName();
        }

        lang_files.Sort( CaseInsensitiveCmp );
        lang = wxGetSingleChoice( wxT( "Please choose language" ), wxT( "Languages" ), lang_files, parent );

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

	if (wxIsprint(grp_sep) && val.length() > grp_sz) { // there will be groups

		wchar_t sep = get_group_separator(grp_sep);

		std::wstring s;
		s.reserve(val.length() + val.length()/grp_sz);
		s.assign(val.wc_str(*wxConvCurrent), val.length());

		for (size_t i = 0, j = i + grp_sz*(i+1); j < s.length(); ++i, j = i + grp_sz*(i+1)) {
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

        if (scale >= 10000) {
        	i = 3;
        } else if (scale >= 1000) {
        	i = 2;
        } else if (scale < 100) {
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

        if (!for_edit) {
		wxString pfx = fmt.getPrefix();
	        if (!pfx.empty()) {
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
bool mmIniOptions::enableAssets_ = true;
bool mmIniOptions::enableBudget_ = true;
bool mmIniOptions::enableStocks_ = true;
bool mmIniOptions::enableGraphs_ = true;
bool mmIniOptions::enableCustomLogo_ = false;
wxString mmIniOptions::logoName_;
bool mmIniOptions::enableAddAccount_ = true;
bool mmIniOptions::enableRepeatingTransactions_ = true;
bool mmIniOptions::enableImportMMNETCSV_ = true;
bool mmIniOptions::enableImportMMCSV_ = true;
bool mmIniOptions::enableCheckForUpdates_ = true;
bool mmIniOptions::enableReportIssues_  = true;
bool mmIniOptions::enableBeNotifiedForNewReleases_ = true;
bool mmIniOptions::enableVisitWebsite_ = true;
bool mmIniOptions::enableCustomAboutDialog_ = false;
wxString mmIniOptions::aboutCompanyName_;
bool mmIniOptions::disableCategoryModify_ = false;
bool mmIniOptions::enableDeleteAccount_ = true;
wxString mmIniOptions::userNameString_;
bool mmIniOptions::enableCustomTemplateDB_ = false;
wxString mmIniOptions::customTemplateDB_;
wxString mmIniOptions::fontSize_ = wxT( "3" );

bool mmIniOptions::expandBankHome_ = true;
bool mmIniOptions::expandTermHome_ = false;
bool mmIniOptions::expandBankTree_ = true;
bool mmIniOptions::expandTermTree_ = false;
bool mmIniOptions::transactionStatusReconciled_ = false;

//----------------------------------------------------------------------------
wxString mmOptions::dateFormat = mmex::DEFDATEFORMAT;
wxString mmOptions::language = wxT( "english" );
wxString mmOptions::financialYearStartDayString_;
wxString mmOptions::financialYearStartMonthString_;
//----------------------------------------------------------------------------

void mmOptions::loadOptions( wxSQLite3Database* db )
{
    dateFormat = mmDBWrapper::getInfoSettingValue( db, wxT( "DATEFORMAT" ), mmex::DEFDATEFORMAT );

    financialYearStartDayString_   = mmDBWrapper::getInfoSettingValue(db, wxT("FINANCIAL_YEAR_START_DAY"), wxT("1"));
    financialYearStartMonthString_ = mmDBWrapper::getInfoSettingValue(db, wxT("FINANCIAL_YEAR_START_MONTH"), wxT("Jul"));

}
//----------------------------------------------------------------------------

void mmOptions::saveOptions( wxSQLite3Database* db )
{
    mmDBWrapper::setInfoSettingValue( db, wxT( "DATEFORMAT" ), dateFormat );
}
// --------------------------------------------------------------------------

void mmIniOptions::loadOptions( wxSQLite3Database* db )
{
    if ( mmDBWrapper::getINISettingValue( db, wxT( "ENABLESTOCKS" ), wxT( "TRUE" ) ) != wxT( "TRUE" ) )
        enableStocks_ = false;

    if ( mmDBWrapper::getINISettingValue( db, wxT( "ENABLEASSETS" ), wxT( "TRUE" ) ) != wxT( "TRUE" ) )
        enableAssets_ = false;

    if ( mmDBWrapper::getINISettingValue( db, wxT( "ENABLEBUDGET" ), wxT( "TRUE" ) ) != wxT( "TRUE" ) )
        enableBudget_ = false;

    if ( mmDBWrapper::getINISettingValue( db, wxT( "ENABLEGRAPHS" ), wxT( "TRUE" ) ) != wxT( "TRUE" ) )
        enableGraphs_ = false;

    mmIniOptions::fontSize_ = mmDBWrapper::getINISettingValue( db, wxT( "HTMLFONTSIZE" ), wxT( "3" ) );

    if ( mmDBWrapper::getINISettingValue( db, wxT("EXPAND_BANK_HOME"), wxT( "TRUE" ) ) != wxT( "TRUE" ) )
        expandBankHome_ = false;
    if ( mmDBWrapper::getINISettingValue( db, wxT("EXPAND_TERM_HOME"), wxT( "FALSE" ) ) != wxT( "FALSE" ) )
        expandTermHome_ = true;
    if ( mmDBWrapper::getINISettingValue( db, wxT("EXPAND_BANK_TREE"), wxT( "TRUE" ) ) != wxT( "TRUE" ) )
        expandBankTree_ = false;
    if ( mmDBWrapper::getINISettingValue( db, wxT("EXPAND_TERM_TREE"), wxT( "FALSE" ) ) != wxT( "FALSE" ) )
        expandTermTree_ = true;

    if ( mmDBWrapper::getINISettingValue( db, wxT("TRANSACTION_STATUS_RECONCILED"), wxT("FALSE") ) != wxT( "FALSE" ) )
        transactionStatusReconciled_ = true;
}

void mmIniOptions::loadInfoOptions( wxSQLite3Database* db )
{
    mmIniOptions::userNameString_ = mmDBWrapper::getInfoSettingValue( db, wxT( "USERNAME" ), wxT( "" ) );
}

void mmIniOptions::saveOptions( wxSQLite3Database* /*db*/ )
{

}

// ---------------------------------------------------------------------------
void mmPlayTransactionSound( wxSQLite3Database* db_ )
{
        wxString useSound = mmDBWrapper::getINISettingValue( db_, wxT( "USETRANSSOUND" ), wxT( "TRUE" ) );

        if ( useSound == wxT( "TRUE" ) ) {
                wxSound registerSound( mmex::getPathResource( mmex::TRANS_SOUND ) );

                if ( registerSound.IsOk() )
                        registerSound.Play( wxSOUND_ASYNC );
        }
}

/*
        locale.AddCatalog(lang) calls wxLogWarning and returns true for corrupted .mo file,
        so I should use locale.IsLoaded(lang) also.
*/
wxString mmSelectLanguage( wxWindow *parent, wxSQLite3Database* inidb, bool forced_show_dlg )
{
        wxString lang;

        const wxString langPath = mmex::getPathShared( mmex::LANG_DIR );
        wxLocale &locale = wxGetApp().getLocale();
        bool verbose = forced_show_dlg;

        if ( wxDir::Exists( langPath ) ) {
                locale.AddCatalogLookupPathPrefix( langPath );
        } else {
                if ( verbose ) {
                        wxString s = wxT( "Directory of language files does not exist:\n\"" );
                        s << langPath << wxT( '\"' );

                        wxMessageDialog dlg( parent, s, wxT( "Error" ), wxICON_ERROR );
                        dlg.ShowModal();
                }

                return lang;
        }

        const wxString param_lang( wxT( "LANGUAGE" ) );

        if ( !forced_show_dlg ) {

                lang = mmDBWrapper::getINISettingValue( inidb, param_lang );

                if ( !lang.empty() && locale.AddCatalog( lang ) && locale.IsLoaded( lang ) ) {
                        mmOptions::language = lang;
                        return lang;
                }
        }

        lang = selectLanguageDlg( parent, langPath, verbose );

        if ( !lang.empty() ) {

                bool ok = locale.AddCatalog( lang ) && locale.IsLoaded( lang );

                if ( !ok ) {
                        lang.clear(); // bad .mo file
                }

                mmOptions::language = lang;
                mmDBWrapper::setINISettingValue( inidb, param_lang, lang );
        }

        return lang;
}

wxString mmReadyDisplayString( const wxString& orig )
{
        wxString toReturn = orig;
        toReturn.Replace( wxT( "&" ), wxT( "&&" ) );
        return toReturn;
}

wxString mmUnCleanString( const wxString& orig )
{
        wxString toReturn = orig;
        toReturn.Replace( wxT( "''" ), wxT( "'" ) );
        return toReturn;
}

wxString mmGetNiceMonthName( int month )
{
        static const wxString mon[12] = {
                _( "January" ), _( "February" ), _( "March" ), _( "April" ), _( "May " ), _( "June" ),
                _( "July" ), _( "August" ), _( "September" ), _( "October" ), _( "November" ), _( "December" )
        };

        wxASSERT( month >= 0 && month < 12 );
        return mon[month];
}

wxString mmGetNiceShortMonthName( int month )
{
        static const wxString mon[12] = {
                _( "Jan" ), _( "Feb" ), _( "Mar" ), _( "Apr" ), _( "May" ), _( "Jun" ),
                _( "Jul" ), _( "Aug" ), _( "Sep" ), _( "Oct" ), _( "Nov" ), _( "Dec" )
        };

        wxASSERT( month >= 0 && month < 12 );
        return mon[month];
}

wxString mmGetNiceDateString( const wxDateTime &dt )
{
        static const wxString gDaysInWeek[7] = {
                _( "Sunday" ), _( "Monday" ), _( "Tuesday" ), _( "Wednesday" ),
                _( "Thursday" ), _( "Friday" ), _( "Saturday" )
        };

        wxString dts( gDaysInWeek[dt.GetWeekDay()] + wxString( wxT( ", " ) ) );

//      Discover the date format set by the user
        wxString dateFmt = mmOptions::dateFormat.Mid(1,1).MakeUpper();

//      Format date as: DDD, DD MMM YYYY
        if ( dateFmt == wxT("D")) {
            dts += wxString::Format( wxT( "%d" ), dt.GetDay() ) + wxString( wxT( " " ) );
            dts += mmGetNiceMonthName( dt.GetMonth() ) + wxT( " " );
            dts += wxString::Format( wxT( "%d" ), dt.GetYear() );

//      Format date as: DDD, YYYY MMM DD
        } else if ( dateFmt == wxT("Y")) {
            dts += wxString::Format( wxT( "%d" ), dt.GetYear() ) + wxString( wxT( " " ) );
            dts += mmGetNiceMonthName( dt.GetMonth() ) + wxT( " " );
            dts += wxString::Format( wxT( "%d" ), dt.GetDay() );

//      Format date as: DDD, MMM DD, YYYY
        } else {
            dts += mmGetNiceMonthName( dt.GetMonth() ) + wxString( wxT( " " ) );
            dts += wxString::Format( wxT( "%d" ), dt.GetDay() ) + wxT( ", " )
                 + wxString::Format( wxT( "%d" ), dt.GetYear() );
        }

        return dts;
}

wxString mmGetNiceDateSimpleString( const wxDateTime &dt )
{
        wxString dts = mmGetNiceMonthName( dt.GetMonth() ) + wxString( wxT( " " ) );

//      Discover the date format set by the user
        wxString dateFmt = mmOptions::dateFormat.Mid(1,1).MakeUpper();
//      Format date as: DD MMM YYYY
        if ( dateFmt == wxT("D")) {
            dts = wxString::Format( wxT( "%d" ), dt.GetDay() ) + wxString( wxT( " " ) )
            + dts
            + wxString::Format( wxT( "%d" ), dt.GetYear() );

//      Format date as: YYYY MMM DD
        } else if ( dateFmt == wxT("Y")) {
            dts = wxString::Format( wxT( "%d" ), dt.GetYear() ) + wxString( wxT( " " ) )
                + dts
                + wxString::Format( wxT( "%d" ), dt.GetDay() );

//      Format date as: MMM DD, YYYY
        } else {
            dts += wxString::Format( wxT( "%d" ), dt.GetDay() ) + wxT( ", " )
                 + wxString::Format( wxT( "%d" ), dt.GetYear() );
        }

        return dts;
}

void mmShowErrorMessage( wxWindow *parent, const wxString &message, const wxString &messageheader )
{
        wxMessageDialog msgDlg( parent, message, messageheader );
        msgDlg.ShowModal();
}

void mmShowErrorMessageInvalid( wxWindow *parent, const wxString &message )
{
        wxString msg = _( "Entry " ) + message + _( " is invalid." );
        mmShowErrorMessage( parent, msg, _( "Invalid Entry" ) );
}

wxString mmNotes4ExportString( const wxString& orig )
{
        wxString toReturn = orig;

        if ( !toReturn.IsEmpty() ) {
                //It's should be 1 line for each transaction in the exported file
                toReturn.Replace( wxT( "\n" ), wxT( " " ) );
                toReturn.Trim();
                //Double quotas should be doubled in the notes.
                toReturn.Replace( wxT( "\"" ), wxT( "\"\"" ) );
                wxString text = wxT( "\"" ) + toReturn + wxT( "\"" );
                toReturn = text;
        }

        return toReturn;
}

void mmExportCSV( wxSQLite3Database* db_ )
{
        if ( mmDBWrapper::getNumAccounts( db_ ) == 0 ) {
                mmShowErrorMessage( 0, _( "No Account available! Cannot Export!" ), _( "Error" ) );
                return;
        }

        wxArrayString as;

        {
                wxSQLite3ResultSet q1 = db_->ExecuteQuery( g_AccountNameSQL );

                while ( q1.NextRow() ) {
                        as.Add( q1.GetString( wxT( "ACCOUNTNAME" ) ) );
                }

                q1.Finalize();
        }

        wxString delimit = mmDBWrapper::getInfoSettingValue( db_, wxT( "DELIMITER" ), mmex::DEFDELIMTER );

        wxSingleChoiceDialog scd( 0, _( "Choose Account to Export from:" ), _( "CSV Export" ), as);

        if ( scd.ShowModal() != wxID_OK )
                return;

        wxString acctName = scd.GetStringSelection();
        int fromAccountID = mmDBWrapper::getAccountID( db_, acctName );

        wxString fileName = wxFileSelector( _( "Choose CSV data file to Export" ),
                                            wxT( "" ), wxT( "" ), wxT( "" ), wxT( "*.csv" ), wxSAVE | wxOVERWRITE_PROMPT );

        if ( fileName.empty() )
                return;

        wxFileOutputStream output( fileName );
        wxTextOutputStream text( output );

        static const char sql[] =
                "SELECT TRANSID, TRANSDATE, "
                "TRANSCODE, TRANSAMOUNT,  SUBCATEGID, "
                "CATEGID, PAYEEID, "
                "TRANSACTIONNUMBER, NOTES, TOACCOUNTID, ACCOUNTID "
                "FROM CHECKINGACCOUNT_V1 "
                "where ACCOUNTID = ? OR TOACCOUNTID = ?"
                "ORDER BY TRANSDATE";

        wxSQLite3Statement st = db_->PrepareStatement( sql );
        st.Bind( 1, fromAccountID );
        st.Bind( 2, fromAccountID );

        wxSQLite3ResultSet q1 = st.ExecuteQuery();
        int numRecords = 0;

        while ( q1.NextRow() ) {
                wxString transid = q1.GetString( wxT( "TRANSID" ) );
                wxString dateDBString = q1.GetString( wxT( "TRANSDATE" ) );
                wxDateTime dtdt = mmGetStorageStringAsDate( dateDBString );
                wxString dateString = mmGetDateForDisplay( db_, dtdt );

                int sid, cid;
                wxString payee = mmNotes4ExportString(mmDBWrapper::getPayee( db_, q1.GetInt( wxT( "PAYEEID" ) ), sid, cid ));
                wxString type = q1.GetString( wxT( "TRANSCODE" ) );
                wxString sign = wxT( "" );
                wxString amount = q1.GetString( wxT( "TRANSAMOUNT" ) );

                //Amount should be formated if delimiters is not "." or ","
                if ( delimit != wxT( "." ) && delimit != wxT( "," ) ) {
                        double value = 0;
                        mmex::formatCurrencyToDouble( amount, value );
                        mmex::formatDoubleToCurrencyEdit( value, amount );
                }

                wxString categ = mmNotes4ExportString(mmDBWrapper::getCategoryName( db_, q1.GetInt( wxT( "CATEGID" ))));
                wxString subcateg = mmNotes4ExportString(mmDBWrapper::getSubCategoryName( db_,
                                    q1.GetInt( wxT( "CATEGID" ) ), q1.GetInt( wxT( "SUBCATEGID" ))));
                wxString transNum = mmNotes4ExportString( q1.GetString( wxT( "TRANSACTIONNUMBER" ) ) );
                wxString notes = mmNotes4ExportString( q1.GetString( wxT( "NOTES" ) ) );
                wxString origtype = type;

                if ( type == wxT( "Transfer" ) ) 
                {
                        int tAccountID = q1.GetInt( wxT( "TOACCOUNTID" ) );
                        int fAccountID = q1.GetInt( wxT( "ACCOUNTID" ) );

                        wxString fromAccount = mmDBWrapper::getAccountName( db_,  fAccountID );
                        wxString toAccount = mmDBWrapper::getAccountName( db_,  tAccountID );

                        if ( tAccountID == fromAccountID ) {
                                type = wxT( "Deposit" );
                                payee = fromAccount;
                        } else if ( fAccountID == fromAccountID ) {
                                type = wxT( "Withdrawal" );
                                payee = toAccount;
                                //transfer = wxT("T");
                        }
                }

                //It should be negative amounts for withdrwal
                if ( type == wxT( "Withdrawal" ) ) {
                        sign = wxT( "-" );
                }

                if ( categ.IsEmpty() && subcateg.IsEmpty() ) {
                        static const char sql4splitedtrx[] =
                                "SELECT SUBCATEGID, CATEGID, SPLITTRANSAMOUNT "
                                "FROM splittransactions_v1 "
                                "WHERE TRANSID = ?";

                        wxSQLite3Statement st2 = db_->PrepareStatement( sql4splitedtrx );
                        st2.Bind( 1, transid );

                        wxSQLite3ResultSet q2 = st2.ExecuteQuery();

                        while ( q2.NextRow() ) {
                                wxString splitamount = q2.GetString( wxT( "SPLITTRANSAMOUNT" ) );

                                //Amount should be formated if delimiters is not "." or ","
                                if ( delimit != wxT( "." ) && delimit != wxT( "," ) ) {
                                        double value = 0;
                                        mmex::formatCurrencyToDouble( splitamount, value );
                                        mmex::formatDoubleToCurrencyEdit( value, splitamount );
                                }

                                wxString splitcateg = mmNotes4ExportString(mmDBWrapper::getCategoryName( db_, q2.GetInt( wxT( "CATEGID" ) ) ));
                                wxString splitsubcateg = mmNotes4ExportString(mmDBWrapper::getSubCategoryName( db_, q2.GetInt( wxT( "CATEGID" ) ), q2.GetInt( wxT( "SUBCATEGID" ) ) ));

                                text << wxT("\"") << dateString << wxT("\"") << delimit << payee << delimit << wxT("\"") << sign << splitamount << wxT("\"") << delimit
                                << splitcateg << delimit << splitsubcateg << delimit << transNum << delimit
                                << notes << delimit << wxT("\"") << origtype << wxT("\"") << endl;

                        }

                        st2.Finalize();
                } else {
                        text << wxT("\"") << dateString << wxT("\"") << delimit << payee << delimit
                        << wxT("\"") << sign << amount << wxT("\"") << delimit << categ << delimit << subcateg << delimit 
						<< transNum << delimit << notes << delimit << wxT("\"") << origtype << wxT("\"") << endl;
                }

                numRecords++;
        }

        st.Finalize();

        wxString msg = wxString::Format( wxT( "%d transactions exported" ), numRecords );
        mmShowErrorMessage( 0, msg, _( "Export to CSV" ) );
}

void mmExportQIF( wxSQLite3Database* db_ )
{
        if ( mmDBWrapper::getNumAccounts( db_ ) == 0 ) {
                mmShowErrorMessage( 0, _( "No Account available! Cannot Export!" ), _( "Error" ) );
                return;
        }

        wxArrayString as;

        wxSQLite3ResultSet q3 = db_->ExecuteQuery( g_AccountNameSQL );

        while ( q3.NextRow() ) {
                as.Add( q3.GetString( wxT( "ACCOUNTNAME" ) ) );
        }

        q3.Finalize();

        wxString delimit = mmDBWrapper::getInfoSettingValue( db_, wxT( "DELIMITER" ), mmex::DEFDELIMTER );

        wxSingleChoiceDialog scd( 0, _( "Choose Account to Export from:" ),_( "QIF Export" ), as );

        wxString acctName;

        if ( scd.ShowModal() == wxID_OK ) {
                acctName = scd.GetStringSelection();
        }

        if ( acctName.IsEmpty() ) {
                return;
        }

        wxString fileName = wxFileSelector( _( "Choose QIF data file to Export" ), wxT( "" ), wxT( "" ), wxT( "" ), wxT( "*.qif" ), wxSAVE | wxOVERWRITE_PROMPT );

        if ( fileName.IsEmpty() ) {
                return;
        }

        wxFileOutputStream output( fileName );
        wxTextOutputStream text( output );
        int fromAccountID = mmDBWrapper::getAccountID( db_, acctName );

        static const char sql[] =
                "SELECT transid, transdate as DATE, "
                "transcode as TRANSACTIONTYPE, transamount as AMOUNT, totransamount as TOAMOUNT, "
                " SUBCATEGID, "
                "CATEGID, PAYEEID, "
                "TRANSACTIONNUMBER, NOTES, TOACCOUNTID, ACCOUNTID "
                "FROM checkingaccount_v1 "
                "WHERE ACCOUNTID = ? OR TOACCOUNTID = ?"
                "ORDER BY transdate";

        wxSQLite3Statement st = db_->PrepareStatement( sql );
        st.Bind( 1, fromAccountID );
        st.Bind( 2, fromAccountID );

        wxSQLite3ResultSet q1 = st.ExecuteQuery();
        int numRecords = 0;

        text
        << wxT( "!Account" ) << endl
        << wxT( "N" ) << acctName <<  endl
        << wxT( "TChecking" ) << endl
        << wxT( "^" ) <<  endl
        << wxT( "!Type:Cash" ) << endl;

        while ( q1.NextRow() ) {
                wxString transid = q1.GetString( wxT( "TRANSID" ) );
                wxString dateDBString = q1.GetString( wxT( "DATE" ) );
                wxDateTime dtdt = mmGetStorageStringAsDate( dateDBString );
                wxString dateString = mmGetDateForDisplay( db_, dtdt );

                int sid, cid;
                wxString payee = mmDBWrapper::getPayee( db_, q1.GetInt( wxT( "PAYEEID" ) ), sid, cid );
                wxString type = q1.GetString( wxT( "TRANSACTIONTYPE" ) );
                
                wxString amount = q1.GetString( wxT( "AMOUNT" ) );
                //Amount should be formated
                double value = 0.0;
                mmex::formatCurrencyToDouble( amount, value );
                mmex::formatDoubleToCurrencyEdit( value, amount );
                
                wxString toamount = q1.GetString( wxT( "TOAMOUNT" ) );
                //Amount should be formated
                value = 0.0;
                mmex::formatCurrencyToDouble( toamount, value );
                mmex::formatDoubleToCurrencyEdit( value, toamount );
                
                
                wxString transNum = q1.GetString( wxT( "TRANSACTIONNUMBER" ) );
                wxString categ = mmDBWrapper::getCategoryName( db_, q1.GetInt( wxT( "CATEGID" ) ) );
                wxString subcateg = mmDBWrapper::getSubCategoryName( db_,
                                    q1.GetInt( wxT( "CATEGID" ) ), q1.GetInt( wxT( "SUBCATEGID" ) ) );
                wxString notes = mmUnCleanString( q1.GetString( wxT( "NOTES" ) ) );
                
                //
                notes.Replace( wxT( "\n" ), wxT( " " ) );
                wxString subcategStr = wxT ("") ;

                if ( type == wxT( "Transfer" ) ) 
                {
                        subcategStr = type;
                        int tAccountID = q1.GetInt( wxT( "TOACCOUNTID" ) );
                        int fAccountID = q1.GetInt( wxT( "ACCOUNTID" ) );

                        wxString fromAccount = mmDBWrapper::getAccountName( db_,  fAccountID );
                        wxString toAccount = mmDBWrapper::getAccountName( db_,  tAccountID );

                        if ( tAccountID == fromAccountID ) {
                                 payee = fromAccount;
                                 amount = toamount;
                        } else if ( fAccountID == fromAccountID ) {
                                payee = toAccount;
                                amount = wxT ('-') + amount;
                        }
                }
                else
                {
	             subcategStr << categ << ( subcateg != wxT( "" ) ? wxT( ":" ) : wxT( "" ) ) << subcateg;    
                }

                text << wxT( 'D' ) << dateString << endl
                << wxT( 'T' ) << ( type == wxT( "Withdrawal" ) ? wxT( "-" ) : wxT( "" ) ) << amount << endl //FIXME: is T needed when Transfer?
                << wxT( 'P' ) << payee << endl
                << wxT( 'N' ) << transNum << endl
                //Category or Transfer
                << wxT( 'L' ) << subcategStr << endl
                << wxT( 'M' ) << notes << endl;
                if ( type == wxT( "Transfer" ) ) 
                {
	                text << wxT('$') << amount << endl;
                }
                

                //if categ id is empty the transaction has been splited
                if ( categ.IsEmpty() && subcateg.IsEmpty() ) {
                        static const char sql4splitedtrx[] =
                                "SELECT SUBCATEGID, CATEGID, SPLITTRANSAMOUNT "
                                "FROM splittransactions_v1 "
                                "WHERE TRANSID = ?";

                        wxSQLite3Statement st2 = db_->PrepareStatement( sql4splitedtrx );
                        st2.Bind( 1, transid );

                        wxSQLite3ResultSet q2 = st2.ExecuteQuery();

                        while ( q2.NextRow() ) {
                                wxString splitamount = q2.GetString( wxT( "SPLITTRANSAMOUNT" ) );
                                //Amount should be formated
                                value = 0.0;
                                mmex::formatCurrencyToDouble( splitamount, value );
                                mmex::formatDoubleToCurrencyEdit( value, splitamount );
                                wxString splitcateg = mmDBWrapper::getCategoryName( db_, q2.GetInt( wxT( "CATEGID" ) ) );
                                wxString splitsubcateg = mmDBWrapper::getSubCategoryName( db_,
                                                         q2.GetInt( wxT( "CATEGID" ) ), q2.GetInt( wxT( "SUBCATEGID" ) ) );
                                text << wxT( 'S' ) << splitcateg << ( splitsubcateg != wxT( "" ) ? wxT( ":" ) : wxT( "" ) ) << splitsubcateg << endl
                                << wxT( '$' ) << ( type == wxT( "Withdrawal" ) ? wxT( "-" ) : wxT( "" ) ) << splitamount << endl
                                // E Split memo — any text to go with this split item. I saggest Category:Subcategory = Amount for earch line
                                << wxT( 'E' ) << splitcateg << ( splitsubcateg != wxT( "" ) ? wxT( ":" ) : wxT( "" ) ) << splitsubcateg << ( type == wxT( "Withdrawal" ) ? wxT( " -" ) : wxT( " " ) ) << splitamount << endl;
                        }

                        q2.Finalize();
                }

                text << wxT( '^' ) << endl;
                numRecords++;
        }

        q1.Finalize();

        wxString msg = wxString::Format( wxT( "%d transactions exported" ), numRecords );
        mmShowErrorMessage( 0, msg, _( "Export to QIF" ) );
}

void mmSetCategory( mmCoreDB* core, wxString categ, wxString subcateg, int& categID, int& subCategID )
{
        bool isUnknown = false;

        if ( categ.Trim().IsEmpty() ) {
                categ = wxT( "Unknown" );
                isUnknown = true;
        }

        // Create category (unknown or otherwise) if it doesn't already exist
        categID = core->categoryList_.getCategoryID( categ );

        if ( categID == -1 ) {
                categID =  core->categoryList_.addCategory( categ );
        }

        // Create sub category, provided it is not for the Unknown category
        if ( !isUnknown && !subcateg.Trim().IsEmpty() ) {
                subCategID = core->categoryList_.getSubCategoryID( categID, subcateg );

                if ( subCategID == -1 ) {
                        subCategID = core->categoryList_.addSubCategory( categID, subcateg );
                }
        }
}

int mmImportCSV( mmCoreDB* core )
{
        wxSQLite3Database* db_ = core->db_.get();

        if ( mmDBWrapper::getNumAccounts( db_ ) == 0 ) {
                mmShowErrorMessage( 0, _( "No Account available! Cannot Import! Create a new account first!" ),
                                    _( "Error" ) );
                return -1;
        }

        wxArrayString as;
        int fromAccountID = -1;

        wxSQLite3ResultSet q1 = db_->ExecuteQuery( g_AccountNameSQL );

        while ( q1.NextRow() ) {
                as.Add( q1.GetString( wxT( "ACCOUNTNAME" ) ) );
        }

        q1.Finalize();

        wxString delimit = mmDBWrapper::getInfoSettingValue( db_, wxT( "DELIMITER" ), mmex::DEFDELIMTER );

        wxSingleChoiceDialog scd( 0, _( "Choose Account to import to:" ), _( "CSV Import" ), as );

        if ( scd.ShowModal() == wxID_OK ) {
                wxString acctName = scd.GetStringSelection();
                fromAccountID = mmDBWrapper::getAccountID( db_, acctName );
                boost::shared_ptr<mmCurrency> pCurrencyPtr = core->accountList_.getCurrencyWeakPtr( fromAccountID ).lock();
                wxASSERT( pCurrencyPtr );

                wxString fileName = wxFileSelector( wxT( "Choose CSV data file to import" ),
                                                    wxT( "" ), wxT( "" ), wxT( "" ), wxT( "*.csv" ), wxFILE_MUST_EXIST );

                if ( !fileName.empty() ) {
                        wxFileInputStream input( fileName );
                        wxTextInputStream text( input );

                        wxFileName logFile = mmex::GetLogDir( true );
                        logFile.SetFullName( fileName );
                        logFile.SetExt( wxT( "txt" ) );

                        wxFileOutputStream outputLog( logFile.GetFullPath() );
                        wxTextOutputStream log( outputLog );

                        // We have a fixed format for now
                        // date, payeename, "withdrawal/deposit", amount, category, subcategory, transactionnumber, notes
                        int countNumTotal = 0;
                        int countImported = 0;

                        while ( !input.Eof() ) {
                                wxString line = text.ReadLine();

                                if ( !line.IsEmpty() )
                                        ++countNumTotal;
                                else
                                        continue;

                                wxString dt = wxDateTime::Now().FormatISODate();
                                wxString payee;
                                wxString type;
                                wxString amount;
                                wxString categ;
                                wxString subcateg;
                                wxString transNum;
                                wxString notes;
                                wxString transfer;

                                wxStringTokenizer tkz( line, delimit, wxTOKEN_RET_EMPTY_ALL );

                                if ( tkz.HasMoreTokens() )
                                        dt = tkz.GetNextToken();
                                else {
                                        log << _( "Line : " ) << countNumTotal
                                        << _( " missing date, skipping." ) << endl;
                                        continue;
                                }

                                if ( tkz.HasMoreTokens() )
                                        payee = mmCleanString( tkz.GetNextToken() );
                                else {
                                        log << _( "Line : " ) << countNumTotal
                                        << _( " missing payee, skipping." ) << endl;
                                        continue;
                                }

                                if ( tkz.HasMoreTokens() )
                                        type = mmCleanString( tkz.GetNextToken() );
                                else {
                                        log << _( "Line : " ) << countNumTotal
                                        << _( " missing transaction type, skipping." ) << endl;
                                        continue;
                                }

                                if ( tkz.HasMoreTokens() )
                                        amount = mmCleanString( tkz.GetNextToken() );
                                else {
                                        log << _( "Line : " ) << countNumTotal
                                        << _( " missing amount, skipping." ) << endl;
                                        continue;
                                }

                                if ( tkz.HasMoreTokens() )
                                        categ = mmCleanString( tkz.GetNextToken() );

                                if ( tkz.HasMoreTokens() )
                                        subcateg = mmCleanString( tkz.GetNextToken() );

                                if ( tkz.HasMoreTokens() )
                                        transNum = mmCleanString( tkz.GetNextToken() );

                                if ( tkz.HasMoreTokens() )
                                        notes = mmCleanString( tkz.GetNextToken() );

                                if ( tkz.HasMoreTokens() )
                                        transfer = mmCleanString( tkz.GetNextToken() );

                                if ( dt.Trim().IsEmpty() || payee.Trim().IsEmpty() ||
                                                type.Trim().IsEmpty() || amount.Trim().IsEmpty() ) {
                                        log << _( "Line : " ) << countNumTotal
                                        << _( "one of the following fields: date, payee, transaction type, amount strings is empty, skipping" ) << endl;
                                        continue;
                                }

                                if ( type.Cmp( wxT( "Withdrawal" ) ) && type.Cmp( wxT( "Deposit" ) ) ) {
                                        log << _( "Line : " ) << countNumTotal
                                        << _( " unrecognized transaction type, skipping." ) << endl;
                                        continue;
                                }

                                double val = 0.0;

                                if ( !amount.ToDouble( &val ) ) {
                                        log << _( "Line : " ) << countNumTotal
                                        << _( " invalid amount, skipping." ) << endl;

                                        continue;
                                }

                                bool isTransfer = false;
                                int toAccountID = -1;

                                if ( transfer == wxT( "T" ) ) {
                                        // Check if defined accounts exist
                                        if ( core->accountList_.accountExists( payee ) ) {
                                                toAccountID = core->accountList_.getAccountID( payee );
                                                isTransfer = true;
                                                type = wxT( "Transfer" );
                                        } else {
                                                log << _( "Line : " ) << countNumTotal
                                                << _( " transfer transaction not imported." ) << endl;
                                        }
                                }

                                wxDateTime dtdt = mmParseDisplayStringToDate( db_, dt );
                                wxString convDate = dtdt.FormatISODate();

                                int payeeID = -1;
                                int categID = -1;
                                int subCategID = -1;

                                if ( !isTransfer ) {
                                        if ( !core->payeeList_.payeeExists( payee ) ) {
                                                //payee does not exist
                                                payeeID = core->payeeList_.addPayee( payee );
                                                mmSetCategory( core, categ, subcateg, categID, subCategID );
                                        } else {
                                                payeeID = core->payeeList_.getPayeeID( payee );

                                                if ( categ.Trim().IsEmpty() ) {
                                                        // CSV category not specified, use last known for payee
                                                        boost::shared_ptr<mmPayee> pPayee = core->payeeList_.getPayeeSharedPtr( payeeID );
                                                        boost::shared_ptr<mmCategory> pCategory = pPayee->category_.lock();

                                                        if ( !pCategory ) {
                                                                // Last known not set - set to unknown
                                                                mmSetCategory( core, categ, subcateg, categID, subCategID );
                                                        } else {
                                                                if ( pCategory->parent_.lock() ) {
                                                                        categID = pCategory->parent_.lock()->categID_;
                                                                        subCategID = pCategory->categID_;
                                                                } else {
                                                                        categID = pCategory->categID_;
                                                                        subCategID = -1;
                                                                }
                                                        }
                                                } else {
                                                        mmSetCategory( core, categ, subcateg, categID, subCategID );
                                                }
                                        }
                                } else { // transfer
                                        mmSetCategory( core, categ, subcateg, categID, subCategID );
                                }

                                wxString status = wxT( "F" );



                                boost::shared_ptr<mmBankTransaction> pTransaction( new mmBankTransaction( core->db_ ) );
                                pTransaction->accountID_ = fromAccountID;
                                pTransaction->toAccountID_ = toAccountID;

                                if ( !isTransfer )
                                        pTransaction->payee_ = core->payeeList_.getPayeeSharedPtr( payeeID );
                                else
                                        pTransaction->payee_ = boost::shared_ptr<mmPayee>();

                                pTransaction->transType_ = type;
                                pTransaction->amt_ = val;
                                pTransaction->status_ = status;
                                pTransaction->transNum_ = transNum;
                                pTransaction->notes_ = notes;
                                pTransaction->category_ = core->categoryList_.getCategorySharedPtr( categID, subCategID );
                                pTransaction->date_ = dtdt;
                                pTransaction->toAmt_ = 0.0;
                                pTransaction->updateAllData( core, fromAccountID, pCurrencyPtr );

                                core->bTransactionList_.addTransaction( core, pTransaction );

                                countImported++;
                                log << _( "Line : " ) << countNumTotal << _( " imported OK." ) << endl;
                        } // while EOF

                        wxString msg = wxString::Format( _( "Total Lines : %d.\nTotal Imported : %d\nLog file written to : %s.\n\nImported transactions have been flagged so you can review them." ),
                                                         countNumTotal, countImported, logFile.GetFullPath().c_str() );
                        mmShowErrorMessage( 0, msg, _( "Import from CSV" ) );
                        outputLog.Close();

                        fileviewer( logFile.GetFullPath(), 0 ).ShowModal();
                }
        }

        return fromAccountID;
}

int mmImportCSVMMNET( mmCoreDB* core )
{
        wxSQLite3Database* db_ = core->db_.get();
        mmShowErrorMessage( 0,
                            _( "Please verify that the CSV file from MMEX.NET contains only one account!" ),
                            _( "Import from CSV" ) );

        if ( mmDBWrapper::getNumAccounts( db_ ) == 0 ) {
                mmShowErrorMessage( 0, _( "No Account available! Cannot Import! Create a new account first!" ),
                                    _( "Error" ) );
                return -1;
        }

        wxArrayString as;
        int fromAccountID = -1;

        wxSQLite3ResultSet q1 = db_->ExecuteQuery( g_AccountNameSQL );

        while ( q1.NextRow() ) {
                as.Add( q1.GetString( wxT( "ACCOUNTNAME" ) ) );
        }

        q1.Finalize();

        wxString delimit = mmDBWrapper::getInfoSettingValue( db_, wxT( "DELIMITER" ), mmex::DEFDELIMTER );

        wxSingleChoiceDialog scd( 0, _( "Choose Account to import to:" ),_( "CSV Import" ), as );

        if ( scd.ShowModal() == wxID_OK ) {
                wxString acctName = scd.GetStringSelection();
                fromAccountID = mmDBWrapper::getAccountID( db_, acctName );

                boost::shared_ptr<mmCurrency> pCurrencyPtr = core->accountList_.getCurrencyWeakPtr( fromAccountID ).lock();
                wxASSERT( pCurrencyPtr );

                wxString fileName = wxFileSelector( _( "Choose MM.NET CSV data file to import" ), wxT( "" ), wxT( "" ), wxT( "" ), wxT( "*.csv" ), wxFILE_MUST_EXIST );

                if ( !fileName.IsEmpty() ) {
                        wxFileInputStream input( fileName );
                        wxTextInputStream text( input );

                        wxFileName logFile = mmex::GetLogDir( true );
                        logFile.SetFullName( fileName );
                        logFile.SetExt( wxT( "txt" ) );

                        wxFileOutputStream outputLog( logFile.GetFullPath() );
                        wxTextOutputStream log( outputLog );

                        /* The following is the MM.NET CSV format */
                        /* date, payeename, amount(+/-), Number, status, category : subcategory, notes */
                        int countNumTotal = 0;
                        int countImported = 0;

                        while ( !input.Eof() ) {
                                wxString line = text.ReadLine();

                                if ( !line.IsEmpty() )
                                        countNumTotal++;
                                else
                                        continue;

                                wxString dt = wxDateTime::Now().FormatISODate();
                                wxString payee;
                                wxString type;
                                wxString amount;
                                wxString categ;
                                wxString subcateg;
                                wxString transNum;
                                wxString notes;

                                wxStringTokenizer tkz( line, delimit, wxTOKEN_RET_EMPTY_ALL );

                                if ( tkz.HasMoreTokens() )
                                        dt = tkz.GetNextToken();
                                else {
                                        log << _( "Line : " ) << countNumTotal
                                        << _( " missing date, skipping." ) << endl;
                                        continue;
                                }

                                if ( tkz.HasMoreTokens() )
                                        payee = tkz.GetNextToken();
                                else {
                                        log << _( "Line : " ) << countNumTotal
                                        << _( " missing payee, skipping." ) << endl;
                                        continue;
                                }

                                if ( tkz.HasMoreTokens() )
                                        amount = tkz.GetNextToken();
                                else {
                                        log << _( "Line : " ) << countNumTotal
                                        << _( " missing amount, skipping." ) << endl;
                                        continue;
                                }

                                if ( tkz.HasMoreTokens() )
                                        transNum = tkz.GetNextToken();

                                if ( tkz.HasMoreTokens() )
                                        wxString status = tkz.GetNextToken();

                                if ( tkz.HasMoreTokens() )
                                        categ = tkz.GetNextToken();
                                else {
                                        log << _( "Line : " ) << countNumTotal
                                        << _( " missing category, skipping." ) << endl;
                                        continue;
                                }


                                if ( tkz.HasMoreTokens() )
                                        notes = tkz.GetNextToken();

                                double val = 0.0;

                                if ( !amount.ToDouble( &val ) ) {
                                        log << _( "Line : " ) << countNumTotal
                                        << _( " invalid amount, skipping." ) << endl;

                                        continue;
                                }

                                if ( val <= 0.0 )
                                        type = wxT( "Withdrawal" );
                                else
                                        type = wxT( "Deposit" );

                                val = fabs( val );

                                if ( dt.Trim().IsEmpty() || payee.Trim().IsEmpty() ||
                                                type.Trim().IsEmpty() || amount.Trim().IsEmpty() ||
                                                categ.Trim().IsEmpty() ) {
                                        log << _( "Line : " ) << countNumTotal
                                        << _( " One of the following fields: date, payee, transaction type, amount, category strings is empty, skipping" ) << endl;
                                        continue;
                                }

                                wxDateTime dtdt;
                                const wxChar* chr = dtdt.ParseDate( dt.GetData() );

                                if ( chr == NULL ) {
                                        log << _( "Line : " ) << countNumTotal
                                        << _( " invalid date, skipping." ) << endl;
                                        continue;
                                }

                                wxString convDate = dtdt.FormatISODate();

                                int payeeID = -1;
                                int categID = -1;
                                int subCategID = -1;

                                if ( !core->payeeList_.payeeExists( payee ) ) {
                                        payeeID = core->payeeList_.addPayee( payee );

                                }

                                // category contains ":" to separate categ:subcateg
                                wxStringTokenizer cattkz( categ, wxT( ":" ) );
                                wxString cat, subcat;
                                subcat = wxT( "" );

                                if ( cattkz.HasMoreTokens() )
                                        cat = cattkz.GetNextToken();

                                if ( cattkz.HasMoreTokens() )
                                        subcat = cattkz.GetNextToken();

                                categID = core->categoryList_.getCategoryID( categ );

                                if ( categID == -1 ) {
                                        categID =  core->categoryList_.addCategory( categ );
                                }

                                if ( !subcat.IsEmpty() ) {
                                        subCategID = core->categoryList_.getSubCategoryID( categID, subcateg );

                                        if ( subCategID == -1 ) {
                                                subCategID = core->categoryList_.addSubCategory( categID, subcateg );
                                        }
                                }

                                wxString status = wxT( "F" );
                                int toAccountID = -1;

                                boost::shared_ptr<mmBankTransaction> pTransaction( new mmBankTransaction( core->db_ ) );
                                pTransaction->accountID_ = fromAccountID;
                                pTransaction->toAccountID_ = toAccountID;
                                pTransaction->payee_ = core->payeeList_.getPayeeSharedPtr( payeeID );
                                pTransaction->transType_ = type;
                                pTransaction->amt_ = val;
                                pTransaction->status_ = status;
                                pTransaction->transNum_ = transNum;
                                pTransaction->notes_ = notes;
                                pTransaction->category_ = core->categoryList_.getCategorySharedPtr( categID, subCategID );
                                pTransaction->date_ = dtdt;
                                pTransaction->toAmt_ = 0.0;
                                pTransaction->updateAllData( core, fromAccountID, pCurrencyPtr );

                                core->bTransactionList_.addTransaction( core, pTransaction );

                                countImported++;
                                log << _( "Line : " ) << countNumTotal << _( " imported OK." ) << endl;
                        } // while EOF

                        wxString msg = wxString::Format( _( "Total Lines : %d \nTotal Imported : %d\n\nLog file written to : %s.\n\nImported transactions have been flagged so you can review them. " ), countNumTotal, countImported, logFile.GetFullPath().c_str() );
                        mmShowErrorMessage( 0, msg, _( "Import from CSV" ) );
                        outputLog.Close();

                        fileviewer( logFile.GetFullPath(), 0 ).ShowModal();
                }
        }

        return fromAccountID;
}


wxString mmGetDateForDisplay( wxSQLite3Database* /*db*/, const wxDateTime &dt )
{
        return dt.Format( mmOptions::dateFormat );
}

wxDateTime mmParseDisplayStringToDate( wxSQLite3Database* db, const wxString& dtstr )
{
        wxString selection = mmDBWrapper::getInfoSettingValue( db, wxT( "DATEFORMAT" ), mmex::DEFDATEFORMAT );
        wxDateTime dt;
        const wxChar* char1 = dt.ParseFormat( dtstr.GetData(), selection.GetData() );

        if ( char1 == NULL )
                return wxDateTime::Now();

        return dt;
}

wxString mmGetDateForStorage( const wxDateTime &dt )
{
        return dt.FormatISODate();
}

wxDateTime mmGetStorageStringAsDate( const wxString& str )
{
        wxDateTime dt;
        dt.ParseDate( str.GetData() );
        return dt;
}

wxColour mmGetColourFromString( const wxString& str )
{
        wxStringTokenizer tkz( str, wxT( "," ), wxTOKEN_RET_EMPTY_ALL );
        unsigned char red = 0xFF;
        unsigned char blue = 0xFF;
        unsigned char green = 0xFF;

        if ( tkz.HasMoreTokens() ) {
                long longVal;
                tkz.GetNextToken().ToLong( &longVal );
                red = longVal;

                if ( tkz.HasMoreTokens() ) {
                        tkz.GetNextToken().ToLong( &longVal );
                        green = longVal;

                        if ( tkz.HasMoreTokens() ) {
                                tkz.GetNextToken().ToLong( &longVal );
                                blue = longVal;
                        }
                }
        }

        return wxColour( red, green, blue );
}

wxString mmGetStringFromColour( wxColour color )
{
        wxString clr = wxString::Format( wxT( "%d,%d,%d" ), color.Red(),
                                         color.Green(),
                                         color.Blue() );
        return clr;
}

void mmLoadColorsFromDatabase( wxSQLite3Database* db_ )
{
        mmColors::listAlternativeColor0 = mmGetColourFromString( mmDBWrapper::getINISettingValue( db_,
                                          wxT( "LISTALT0" ), wxT( "225,237,251" ) ) );
        mmColors::listAlternativeColor1 = mmGetColourFromString( mmDBWrapper::getINISettingValue( db_,
                                          wxT( "LISTALT1" ), wxT( "255,255,255" ) ) );
        mmColors::listBackColor = mmGetColourFromString( mmDBWrapper::getINISettingValue( db_,
                                  wxT( "LISTBACK" ), wxT( "255,255,255" ) ) );
        mmColors::navTreeBkColor = mmGetColourFromString( mmDBWrapper::getINISettingValue( db_,
                                   wxT( "NAVTREE" ), wxT( "255,255,255" ) ) );
        mmColors::listBorderColor = mmGetColourFromString( mmDBWrapper::getINISettingValue( db_,
                                    wxT( "LISTBORDER" ), wxT( "0,0,0" ) ) );
        mmColors::listDetailsPanelColor = mmGetColourFromString( mmDBWrapper::getINISettingValue( db_,
                                          wxT( "LISTDETAILSPANEL" ), wxT( "244,247,251" ) ) );
        mmColors::listFutureDateColor = mmGetColourFromString( mmDBWrapper::getINISettingValue( db_,
                                        wxT( "LISTFUTUREDATES" ), wxT( "116,134,168" ) ) );
}

void mmSaveColorsToDatabase( wxSQLite3Database* db_ )
{
        mmDBWrapper::setINISettingValue( db_, wxT( "LISTALT0" ),
                                         mmGetStringFromColour( mmColors::listAlternativeColor0 ) );
        mmDBWrapper::setINISettingValue( db_, wxT( "LISTALT1" ),
                                         mmGetStringFromColour( mmColors::listAlternativeColor1 ) );
        mmDBWrapper::setINISettingValue( db_, wxT( "LISTBACK" ),
                                         mmGetStringFromColour( mmColors::listBackColor ) );
        mmDBWrapper::setINISettingValue( db_, wxT( "NAVTREE" ),
                                         mmGetStringFromColour( mmColors::navTreeBkColor ) );
        mmDBWrapper::setINISettingValue( db_, wxT( "LISTBORDER" ),
                                         mmGetStringFromColour( mmColors::listBorderColor ) );
        mmDBWrapper::setINISettingValue( db_, wxT( "LISTDETAILSPANEL" ),
                                         mmGetStringFromColour( mmColors::listDetailsPanelColor ) );
        mmDBWrapper::setINISettingValue( db_, wxT( "LISTFUTUREDATES" ),
                                         mmGetStringFromColour( mmColors::listFutureDateColor ) );
}

void mmRestoreDefaultColors()
{
        mmColors::listAlternativeColor0 = wxColour( 225, 237, 251 );
        mmColors::listAlternativeColor1 = wxColour( 255, 255, 255 );
        mmColors::listBackColor = wxColour( 255, 255, 255 );
        mmColors::navTreeBkColor = wxColour( 255, 255, 255 );
        mmColors::listBorderColor = wxColour( 0, 0, 0 );
        mmColors::listDetailsPanelColor = wxColour( 244, 247, 251 );
        mmColors::listFutureDateColor = wxColour( 116, 134, 168 );
}

/* Set the default colors */
wxColour mmColors::listAlternativeColor0 = wxColour( 225, 237, 251 );
wxColour mmColors::listAlternativeColor1 = wxColour( 255, 255, 255 );
wxColour mmColors::listBackColor = wxColour( 255, 255, 255 );
wxColour mmColors::navTreeBkColor = wxColour( 255, 255, 255 );
wxColour mmColors::listBorderColor = wxColour( 0, 0, 0 );
wxColour mmColors::listDetailsPanelColor = wxColour( 244, 247, 251 );
wxColour mmColors::listFutureDateColor = wxColour( 116, 134, 168 );

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

	loadSettings(cur.pfxSymbol_,
	             cur.sfxSymbol_,
	             dec,
	             grp,
	             cur.unit_,
	             cur.cent_,
                     cur.scaleDl_
                    );
}
//----------------------------------------------------------------------------

mmex::CurrencyFormatter& mmex::CurrencyFormatter::instance()
{
	static CurrencyFormatter me;
	return me;
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

bool mmex::formatCurrencyToDouble( const wxString& str, double& val )
{
        val = 0;

	const CurrencyFormatter &fmt = CurrencyFormatter::instance();

        wxString s;
	s.Alloc(str.length());        

        for (size_t i = 0; i < str.length() ; ++i) {

                if (str[i] == fmt.getGroupSeparator()) {
			// skip group separator char
                } else if (str[i] == fmt.getDecimalPoint()) {
                        s += wxT('.');
                } else {
                        s += str[i];
                }
        }

        return !s.empty() && s.ToDouble(&val);
}
//----------------------------------------------------------------------------
