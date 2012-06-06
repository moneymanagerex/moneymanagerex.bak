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
 ********************************************************/

#ifdef WX_PRECOMP

#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif //__BORLANDC__

#else
#include <wx/wx.h>
#include <wx/config.h>
#include <wx/confbase.h>
#include <wx/fileconf.h>
#endif //WX_PRECOMP

#include "yahoosettingsdialog.h"

///////////////////////////////////////////////////////////////////////////
BEGIN_EVENT_TABLE( YahooSettingsDialog, wxDialog )
END_EVENT_TABLE()

YahooSettingsDialog::YahooSettingsDialog( wxWindow* parent,
                                         int id,
                                         wxString title,
                                         wxPoint pos,
                                         wxSize size,
                                         int style )
: wxDialog( parent, id, title, pos, size, style )
{
    wxConfigBase *config = wxConfigBase::Get();
    wxSizerFlags flags, flags_expand;
    flags.Align(wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL).Border(wxLEFT|wxTOP, 4);
    flags_expand.Align(wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL).Border(wxLEFT|wxTOP, 4).Expand();

    wxBoxSizer* bSizer1000;
    bSizer1000 = new wxBoxSizer(wxVERTICAL);

    wxFlexGridSizer* fgSizer_main;
    fgSizer_main = new wxFlexGridSizer(0, 2, 10, 10);

    wxStaticText* m_staticText34 = new wxStaticText(this, wxID_STATIC,
        _("Yahoo Server"));
    fgSizer_main->Add(m_staticText34, flags);

    m_YahooServer = new wxTextCtrl(this, idYahooServer,
        _("finance.au.yahoo.com"));
    m_YahooServer->SetToolTip( _("Do not enter anything except the server name (or IP address, if you're feeling lucky)") );
    m_YahooServer->SetValue(config->Read("HTTP_YAHOO_SERVER","download.finance.yahoo.com"));

    fgSizer_main->Add(m_YahooServer, flags_expand);

    wxStaticText* m_staticText2 = new wxStaticText(this, wxID_STATIC, _("Yahoo Suffix"));
    fgSizer_main->Add(m_staticText2, flags);

    m_YahooSuffix = new wxTextCtrl(this,
        idYahooSuffix, wxEmptyString);
    m_YahooSuffix->SetToolTip( _("Enter the suffix used by Yahoo for the market of your stocks. \nIf your stocks are in multiple markets, leave this blank and add  the suffix to each symbol in your portfolio.") );
    m_YahooSuffix->SetValue(config->Read("HTTP_YAHOO_SUFFIX", ""));

    fgSizer_main->Add(m_YahooSuffix, flags_expand);

    fgSizer_main->AddSpacer(1);
    wxStaticText* m_staticText33 = new wxStaticText(this,
        wxID_STATIC, _("Market open"));
    fgSizer_main->Add(m_staticText33, flags);
    wxStaticText* m_staticText3 = new wxStaticText(this,
        wxID_STATIC, _("from"));
    fgSizer_main->Add(m_staticText3, flags);

    wxFlexGridSizer* fgSizer5 = new wxFlexGridSizer(1, 3, 0, 0);

    m_MarketOpenHour = new wxSpinCtrl(this,
        idMarketOpenHour, wxEmptyString, wxDefaultPosition,
        wxDefaultSize, wxSP_ARROW_KEYS, 0, 10, 0);
    m_MarketOpenHour->SetToolTip( _("Allow for any delay in quotes") );
    long LongTemp;
    //m_yahoopointer->OpenTimeStr_.Mid(0,2).ToLong(&LongTemp);
    (config->Read("STOCKS_MARKET_OPEN_TIME", "10:15:00")).Mid(0,2).ToLong(&LongTemp);
    m_MarketOpenHour->SetValue(LongTemp);

    wxStaticText* m_staticText20 = new wxStaticText(this, wxID_STATIC, (" : "));

    m_MarketOpenMinute = new wxSpinCtrl(this,
        idMarketOpenMinute, wxEmptyString, wxDefaultPosition,
        wxDefaultSize, wxSP_ARROW_KEYS, 0, 59, 0);
    m_MarketOpenMinute->SetToolTip( _("Allow for any delay in quotes") );
    //m_yahoopointer->OpenTimeStr_.Mid(3,2).ToLong(&LongTemp);
    (config->Read("STOCKS_MARKET_OPEN_TIME", "10:15:00")).Mid(3,2).ToLong(&LongTemp);
    m_MarketOpenMinute->SetValue(LongTemp);

    fgSizer5->Add(m_MarketOpenHour, flags);
    fgSizer5->Add(m_staticText20, flags);
    fgSizer5->Add(m_MarketOpenMinute, flags);
    fgSizer_main->Add(fgSizer5);

    wxStaticText* m_staticText4 = new wxStaticText(this, wxID_STATIC, _("to"));
    fgSizer_main->Add(m_staticText4, flags);

    wxFlexGridSizer* fgSizer6 = new wxFlexGridSizer(1, 3, 0, 0);

    m_MarketCloseHour = new wxSpinCtrl(this,
        idMarketCloseHour, wxEmptyString,
        wxDefaultPosition, wxDefaultSize, wxSP_ARROW_KEYS, 0, 23, 0);
    m_MarketCloseHour->SetToolTip(_("Allow for any delay in quotes"));
    //m_yahoopointer->CloseTimeStr_.Mid(0,2).ToLong(&LongTemp);
    (config->Read("STOCKS_MARKET_CLOSE_TIME", "16:40:00")).Mid(0,2).ToLong(&LongTemp);
    m_MarketCloseHour->SetValue(LongTemp);

    wxStaticText* m_staticText201 = new wxStaticText(this, wxID_STATIC, (" : "));

    m_MarketCloseMinute = new wxSpinCtrl( this,
        idMarketCloseMinute, wxEmptyString, wxDefaultPosition,
        wxDefaultSize, wxSP_ARROW_KEYS, 0, 59, 0);
    m_MarketCloseMinute->SetToolTip( _("Allow for any delay in quotes"));
    //m_yahoopointer->CloseTimeStr_.Mid(3,2).ToLong(&LongTemp);
    (config->Read("STOCKS_MARKET_CLOSE_TIME", "16:40:00")).Mid(3,2).ToLong(&LongTemp);
    m_MarketCloseMinute->SetValue(LongTemp);

    fgSizer6->Add(m_MarketCloseHour, flags);
    fgSizer6->Add(m_staticText201, flags);
    fgSizer6->Add( m_MarketCloseMinute, flags);
    fgSizer_main->Add(fgSizer6);

    m_checkBoxRefreshPrices = new wxCheckBox(this,
        idCBDoRefresh, _("&Refresh interval"));
    m_checkBoxRefreshPrices->SetValue(config->ReadBool("STOCKS_REFRESH_ENABLED", false));

    wxFlexGridSizer* fgSizer9 = new wxFlexGridSizer(1, 2, 0, 0);


    m_RefreshInterval = new wxSpinCtrl(this,
        idRefreshInterval, wxEmptyString, wxDefaultPosition,
        wxDefaultSize, wxSP_ARROW_KEYS, 1, 999, 5);
    m_RefreshInterval->SetValue(config->ReadLong("STOCKS_REFRESH_MINUTES", 30));
    m_RefreshInterval->SetToolTip(_("Refresh interval in minutes while market is open"));

    wxStaticText* label_minutes = new wxStaticText(this, wxID_STATIC, _("minutes"));

    fgSizer_main->AddSpacer(1);    
    fgSizer_main->Add(m_checkBoxRefreshPrices, flags);
    fgSizer_main->AddSpacer(1);    
    fgSizer_main->Add(fgSizer9);
    fgSizer9->Add(m_RefreshInterval, flags);
    fgSizer9->Add(label_minutes, flags);

    bSizer1000->Add(fgSizer_main, 1, wxEXPAND|wxALL, 15);

    m_sdbSettings = new wxStdDialogButtonSizer();

    wxStdDialogButtonSizer*  itemStdDialogButtonSizer1 = new wxStdDialogButtonSizer;
    bSizer1000->Add(itemStdDialogButtonSizer1, 0, wxALIGN_RIGHT|wxALL, 10);

    wxButton* itemButtonOK = new wxButton(this, wxID_OK);
    itemStdDialogButtonSizer1->Add(itemButtonOK, flags);
    wxButton* itemButtonCancel = new wxButton(this, wxID_CANCEL);
    itemStdDialogButtonSizer1->Add(itemButtonCancel, flags);

    m_sdbSettings->Realize();
    bSizer1000->Add(m_sdbSettings);

    this->SetSizer(bSizer1000);

    GetSizer()->Fit(this);
    GetSizer()->SetSizeHints(this);
    Centre();
}
