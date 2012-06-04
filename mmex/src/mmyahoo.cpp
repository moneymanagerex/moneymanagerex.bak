/*******************************************************
 Copyright (C) 2007 Greg Newton

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
  *******************************************************/

#include "stockspanel.h"
#include "util.h"
#include "dbwrapper.h"
#include "mmyahoo.h"

/** There is NO default constructor - we need a settings database to talk to **/
mmYahoo::mmYahoo(wxSQLite3Database* inidb, wxSQLite3Database* db) : inidb_(inidb), db_(db)
{
    ReadSettings();
}
mmYahoo::~mmYahoo()
{
    WriteSettings();
}

/// ------------------------------------------------------------------
/// Fetch settings from settings database
/// ------------------------------------------------------------------
void mmYahoo::ReadSettings()
{
    // Get values from mmexini.db3
    mmDBWrapper::getINISettingValue(inidb_, ("STOCKS_REFRESH_ENABLED"), 
                                            ("0")).ToLong(&UpdatingEnabled_);
    // Get values from user database
    OpenTimeStr_  = mmDBWrapper::getInfoSettingValue(db_, ("STOCKS_MARKET_OPEN_TIME"), 
                                                          ("10:15:00")).GetData() ;
    CloseTimeStr_ = mmDBWrapper::getInfoSettingValue(db_, ("STOCKS_MARKET_CLOSE_TIME"), 
                                                          ("16:40:00")).GetData() ;
    // Get time of last update from user database
    wxString datetime_str = mmDBWrapper::getInfoSettingValue(db_, ("STOCKS_LAST_REFRESH_DATETIME"), (""));
#if wxCHECK_VERSION(2,9,0)
    if (!LastRefreshDT_.ParseDateTime(datetime_str))
#else 
    if (!LastRefreshDT_.ParseDateTime(datetime_str.GetData()))
#endif
        LastRefreshDT_ = wxInvalidDateTime;

    mmDBWrapper::getInfoSettingValue(db_, ("STOCKS_REFRESH_MINUTES"), ("30")).ToLong(&UpdateIntervalMinutes_);

    // Server
    Server_ = mmDBWrapper::getInfoSettingValue(db_,("HTTP_YAHOO_SERVER"),("download.finance.yahoo.com"));
    Suffix_ = mmDBWrapper::getInfoSettingValue(db_,("HTTP_YAHOO_SUFFIX"), (""));

    //TODO:Store CSVColumns_ in config / INI item
    // CSVColumns_ = ("snghl1c1vd1t1");
    CSVColumns_ = ("sl1n");  /// Extreme basic version - only symbol & current price & name
    CSVTemporaryFile_.Clear();

    // Proxy
    long LongTemp;
    mmDBWrapper::getINISettingValue(inidb_, ("HTTP_USE_PROXY"), ("0")).ToLong(&LongTemp);
}

/// ------------------------------------------------------------------
/// Store settings back into database
/// ------------------------------------------------------------------
void mmYahoo::WriteSettings()
{
    // Status - Save to mmexini.db3
    mmDBWrapper::setINISettingValue(inidb_, ("STOCKS_REFRESH_ENABLED"), wxString::Format(("%ld"), UpdatingEnabled_));
    
    // Save to user database
    mmDBWrapper::setInfoSettingValue(db_, ("STOCKS_MARKET_OPEN_TIME"), OpenTimeStr_) ;
    mmDBWrapper::setInfoSettingValue(db_, ("STOCKS_MARKET_CLOSE_TIME"), CloseTimeStr_) ;
    if (LastRefreshDT_.IsValid() )
    {
        mmDBWrapper::setInfoSettingValue(db_, ("STOCKS_LAST_REFRESH_DATETIME"), wxString::Format(("%s %s"),
                                              LastRefreshDT_.FormatISODate().c_str(),LastRefreshDT_.FormatISOTime().c_str()));
    }
    mmDBWrapper::setInfoSettingValue(db_, ("STOCKS_REFRESH_MINUTES"),wxString::Format(("%ld"),UpdateIntervalMinutes_));
    // Server
    mmDBWrapper::setInfoSettingValue(db_,("HTTP_YAHOO_SERVER"),Server_);
    mmDBWrapper::setInfoSettingValue(db_,("HTTP_YAHOO_SUFFIX"),Suffix_);
}

