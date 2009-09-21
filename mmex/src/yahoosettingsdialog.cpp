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
#endif //WX_PRECOMP

#include "yahoosettingsdialog.h"

///////////////////////////////////////////////////////////////////////////
BEGIN_EVENT_TABLE( YahooSettingsDialog, wxDialog )
END_EVENT_TABLE()

YahooSettingsDialog::YahooSettingsDialog( mmYahoo* myp, 
                                         wxWindow* parent, 
                                         int id, 
                                         wxString title, 
                                         wxPoint pos, 
                                         wxSize size, 
                                         int style ) 
: wxDialog( parent, id, title, pos, size, style ),
m_yahoopointer(myp)
{
	wxBoxSizer* bSizer1000;
	bSizer1000 = new wxBoxSizer( wxVERTICAL );

    wxBoxSizer* bSizer1100;
	bSizer1100 = new wxBoxSizer( wxVERTICAL );

	wxFlexGridSizer* fgSizer_main;
	fgSizer_main = new wxFlexGridSizer( 4, 2, 0, 0 );
	fgSizer_main->AddGrowableCol( 2 );
	fgSizer_main->SetFlexibleDirection( wxHORIZONTAL );
	fgSizer_main->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );

	m_staticText34 = new wxStaticText( this, wxID_ANY, 
        _("Yahoo Server"), wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer_main->Add( m_staticText34, 0, wxALIGN_RIGHT|wxALL, 5 );

	m_YahooServer = new wxTextCtrl( this, idYahooServer, 
        _("finance.au.yahoo.com"), wxDefaultPosition, wxDefaultSize, 0 );
	m_YahooServer->SetToolTip( _("Do not enter anything except the server name (or IP address, if you're feeling lucky)") );
    m_YahooServer->SetValue(m_yahoopointer->Server_);

	fgSizer_main->Add( m_YahooServer, 0, wxALL|wxEXPAND, 5 );

	m_staticText2 = new wxStaticText( this, wxID_ANY, 
        _("Yahoo Suffix"), wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer_main->Add( m_staticText2, 0, wxALIGN_RIGHT|wxALL, 5 );

	m_YahooSuffix = new wxTextCtrl( this, 
        idYahooSuffix, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	m_YahooSuffix->SetToolTip( _("Enter the suffix used by Yahoo for the market of your stocks. \nIf your stocks are in multiple markets, leave this blank and add  the suffix to each symbol in your portfolio.") );
	m_YahooSuffix->SetValue(m_yahoopointer->Suffix_);

	fgSizer_main->Add( m_YahooSuffix, 0, wxALL|wxEXPAND, 5 );

	m_staticText3 = new wxStaticText( this, wxID_ANY, _("Market open from"), wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer_main->Add( m_staticText3, 0, wxALIGN_RIGHT|wxALL, 5 );

	wxFlexGridSizer* fgSizer5;
	fgSizer5 = new wxFlexGridSizer( 1, 3, 0, 0 );
	fgSizer5->SetFlexibleDirection( wxBOTH );
	fgSizer5->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );

	m_MarketOpenHour = new wxSpinCtrl( this, idMarketOpenHour, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxSP_ARROW_KEYS, 0, 10, 0);
	m_MarketOpenHour->SetToolTip( _("Allow for any delay in quotes") );
	long LongTemp;
	m_yahoopointer->OpenTimeStr_.Mid(0,2).ToLong(&LongTemp);
	m_MarketOpenHour->SetValue( LongTemp );

	fgSizer5->Add( m_MarketOpenHour, 0, wxALL, 5 );

	m_staticText20 = new wxStaticText( this, wxID_ANY, _(":"), wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer5->Add( m_staticText20, 0, wxBOTTOM|wxTOP, 5 );

	m_MarketOpenMinute = new wxSpinCtrl( this, idMarketOpenMinute, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxSP_ARROW_KEYS, 0, 59, 0);
	m_MarketOpenMinute->SetToolTip( _("Allow for any delay in quotes") );
	m_yahoopointer->OpenTimeStr_.Mid(3,2).ToLong(&LongTemp);
	m_MarketOpenMinute->SetValue( LongTemp );

	fgSizer5->Add( m_MarketOpenMinute, 0, wxALL, 5 );

	fgSizer_main->Add( fgSizer5, 1, wxEXPAND, 5 );

	m_staticText4 = new wxStaticText( this, wxID_ANY, _("to"), wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer_main->Add( m_staticText4, 0, wxALIGN_RIGHT|wxALL, 5 );

	wxFlexGridSizer* fgSizer6;
	fgSizer6 = new wxFlexGridSizer( 1, 3, 0, 0 );
	fgSizer6->SetFlexibleDirection( wxBOTH );
	fgSizer6->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );

	m_MarketCloseHour = new wxSpinCtrl( this, idMarketCloseHour, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxSP_ARROW_KEYS, 0, 23, 0);
	m_MarketCloseHour->SetToolTip( _("Allow for any delay in quotes") );
	m_yahoopointer->CloseTimeStr_.Mid(0,2).ToLong(&LongTemp);
	m_MarketCloseHour->SetValue( LongTemp );

	fgSizer6->Add( m_MarketCloseHour, 0, wxALL, 5 );

	m_staticText201 = new wxStaticText( this, 
        wxID_ANY, _(":"), wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer6->Add( m_staticText201, 0, wxBOTTOM|wxTOP, 5 );

	m_MarketCloseMinute = new wxSpinCtrl( this, idMarketCloseMinute, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxSP_ARROW_KEYS, 0, 59, 0);
	m_MarketCloseMinute->SetToolTip( _("Allow for any delay in quotes") );
	m_yahoopointer->CloseTimeStr_.Mid(3,2).ToLong(&LongTemp);
    m_MarketCloseMinute->SetValue( LongTemp );

	fgSizer6->Add( m_MarketCloseMinute, 0, wxALL, 5 );

	fgSizer_main->Add( fgSizer6, 1, wxEXPAND, 5 );

	m_checkBoxRefreshPrices = new wxCheckBox( this, 
        idCBDoRefresh, _("&Refresh prices every"), wxDefaultPosition, wxDefaultSize, 0 );
    m_checkBoxRefreshPrices->SetValue(m_yahoopointer->UpdatingEnabled_ ? true : false);

	fgSizer_main->Add( m_checkBoxRefreshPrices, 0, wxALIGN_RIGHT|wxALL, 5 );

	wxBoxSizer* bSizer29;
	bSizer29 = new wxBoxSizer( wxHORIZONTAL );

	wxBoxSizer* bSizer30;
	bSizer30 = new wxBoxSizer( wxVERTICAL );

	m_RefreshInterval = new wxSpinCtrl( this, 
        idRefreshInterval, wxEmptyString, wxDefaultPosition, 
        wxDefaultSize, wxSP_ARROW_KEYS, 1, 999, 5);
	m_RefreshInterval->SetValue(m_yahoopointer->UpdateIntervalMinutes_);

	bSizer30->Add( m_RefreshInterval, 0, wxALL, 5 );

	bSizer29->Add( bSizer30, 1, wxEXPAND, 5 );

	wxBoxSizer* bSizer31;
	bSizer31 = new wxBoxSizer( wxVERTICAL );

	m_staticText6 = new wxStaticText( this, wxID_ANY, 
        _("minutes while market is open"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer31->Add( m_staticText6, 0, wxALL, 5 );

	bSizer29->Add( bSizer31, 1, wxEXPAND, 5 );

	fgSizer_main->Add( bSizer29, 2, wxEXPAND, 5 );

	bSizer1100->Add(fgSizer_main, 1, wxEXPAND, 5);
    bSizer1000->Add(bSizer1100, 1, wxEXPAND | wxALL, 5);

	m_sdbSettings = new wxStdDialogButtonSizer();
	m_sdbSettings->AddButton( new wxButton( this, wxID_OK ) );
	m_sdbSettings->AddButton( new wxButton( this, wxID_CANCEL ) );
	m_sdbSettings->Realize();
	bSizer1000->Add( m_sdbSettings, 0, wxEXPAND, 10 );

	this->SetSizer( bSizer1000 );

    GetSizer()->Fit(this);
    GetSizer()->SetSizeHints(this);
    Centre();
}
