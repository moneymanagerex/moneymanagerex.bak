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
mmYahoo::mmYahoo(wxSQLite3Database* db) : db_(db)
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
    wxConfigBase *config = wxConfigBase::Get();

    UpdatingEnabled_ = config->ReadBool("STOCKS_REFRESH_ENABLED", false);
    
    OpenTimeStr_ = config->Read("STOCKS_MARKET_OPEN_TIME", "10:15:00");
    CloseTimeStr_ = config->Read("STOCKS_MARKET_CLOSE_TIME", "16:40:00");
    
    wxString datetime_str = config->Read("STOCKS_LAST_REFRESH_DATETIME", "");

#if wxCHECK_VERSION(2,9,0)
    if (!LastRefreshDT_.ParseDateTime(datetime_str))
#else 
    if (!LastRefreshDT_.ParseDateTime(datetime_str.GetData()))
#endif

    LastRefreshDT_ = wxInvalidDateTime;

    config->Read("STOCKS_REFRESH_MINUTES", "30").ToLong(&UpdateIntervalMinutes_);

    Server_ = config->Read("HTTP_YAHOO_SERVER","download.finance.yahoo.com");
    Suffix_ = config->Read("HTTP_YAHOO_SUFFIX", "");

    CSVColumns_ = ("sl1n");  /// Extreme basic version - only symbol & current price & name
    CSVTemporaryFile_.Clear();

    // Proxy
    long LongTemp;
    config->Read("HTTP_USE_PROXY", "0").ToLong(&LongTemp);
}

/// ------------------------------------------------------------------
/// Store settings back into database
/// ------------------------------------------------------------------
void mmYahoo::WriteSettings()
{
    wxConfigBase *config = wxConfigBase::Get();
    config->Write("STOCKS_REFRESH_ENABLED", UpdatingEnabled_);
    config->Write("STOCKS_MARKET_OPEN_TIME", OpenTimeStr_);
    config->Write("STOCKS_MARKET_CLOSE_TIME", CloseTimeStr_) ;
    if (LastRefreshDT_.IsValid() )
    {
        config->Write("STOCKS_LAST_REFRESH_DATETIME",
            wxString::Format(("%s %s"),
            LastRefreshDT_.FormatISODate().c_str(),LastRefreshDT_.FormatISOTime().c_str()));
    }
    config->Write("STOCKS_REFRESH_MINUTES", UpdateIntervalMinutes_);
    config->Write("STOCKS_MARKET_CLOSE_TIME", CloseTimeStr_) ;
    config->Write("HTTP_YAHOO_SUFFIX", Suffix_);
}
