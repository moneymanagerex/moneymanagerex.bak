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
    bSizer1000 = new wxBoxSizer(wxVERTICAL);

    wxFlexGridSizer* fgSizer_main;
    fgSizer_main = new wxFlexGridSizer(0, 2, 10, 10);

    m_staticText34 = new wxStaticText(this, wxID_ANY,
        _("Yahoo Server"));
    fgSizer_main->Add(m_staticText34, 0, wxALIGN_RIGHT);

    m_YahooServer = new wxTextCtrl(this, idYahooServer,
        _("finance.au.yahoo.com"));
    m_YahooServer->SetToolTip( _("Do not enter anything except the server name (or IP address, if you're feeling lucky)") );
    m_YahooServer->SetValue(m_yahoopointer->Server_);

    fgSizer_main->Add(m_YahooServer, 0, wxEXPAND);

    m_staticText2 = new wxStaticText(this, wxID_ANY, _("Yahoo Suffix"));
    fgSizer_main->Add(m_staticText2, 0, wxALIGN_RIGHT);

    m_YahooSuffix = new wxTextCtrl(this,
        idYahooSuffix, wxEmptyString);
    m_YahooSuffix->SetToolTip( _("Enter the suffix used by Yahoo for the market of your stocks. \nIf your stocks are in multiple markets, leave this blank and add  the suffix to each symbol in your portfolio.") );
    m_YahooSuffix->SetValue(m_yahoopointer->Suffix_);

    fgSizer_main->Add(m_YahooSuffix, 0, wxEXPAND);

    fgSizer_main->AddSpacer(1);
    wxStaticText *m_staticText33 = new wxStaticText(this,
        wxID_ANY, _("Market open"));
    fgSizer_main->Add(m_staticText33);
    m_staticText3 = new wxStaticText(this,
        wxID_ANY, _("from"));
    fgSizer_main->Add(m_staticText3, 0, wxALIGN_RIGHT);

    wxFlexGridSizer* fgSizer5;
    fgSizer5 = new wxFlexGridSizer(1, 3, 0, 0);
    fgSizer5->SetFlexibleDirection(wxBOTH);
    fgSizer5->SetNonFlexibleGrowMode(wxFLEX_GROWMODE_SPECIFIED);

    m_MarketOpenHour = new wxSpinCtrl(this,
        idMarketOpenHour, wxEmptyString, wxDefaultPosition,
        wxDefaultSize, wxSP_ARROW_KEYS, 0, 10, 0);
    m_MarketOpenHour->SetToolTip( _("Allow for any delay in quotes") );
    long LongTemp;
    m_yahoopointer->OpenTimeStr_.Mid(0,2).ToLong(&LongTemp);
    m_MarketOpenHour->SetValue(LongTemp);

    fgSizer5->Add(m_MarketOpenHour);

    m_staticText20 = new wxStaticText(this, wxID_ANY, wxT(" : "));
    fgSizer5->Add(m_staticText20);

    m_MarketOpenMinute = new wxSpinCtrl(this,
        idMarketOpenMinute, wxEmptyString, wxDefaultPosition,
        wxDefaultSize, wxSP_ARROW_KEYS, 0, 59, 0);
    m_MarketOpenMinute->SetToolTip( _("Allow for any delay in quotes") );
    m_yahoopointer->OpenTimeStr_.Mid(3,2).ToLong(&LongTemp);
    m_MarketOpenMinute->SetValue(LongTemp);

    fgSizer5->Add(m_MarketOpenMinute);

    fgSizer_main->Add(fgSizer5);

    m_staticText4 = new wxStaticText(this,
        wxID_ANY, _("to"));
    fgSizer_main->Add(m_staticText4, 0, wxALIGN_RIGHT);

    wxFlexGridSizer* fgSizer6;
    fgSizer6 = new wxFlexGridSizer(1, 3, 0, 0);
    fgSizer6->SetFlexibleDirection(wxBOTH);
    fgSizer6->SetNonFlexibleGrowMode(wxFLEX_GROWMODE_SPECIFIED);

    m_MarketCloseHour = new wxSpinCtrl(this,
        idMarketCloseHour, wxEmptyString,
        wxDefaultPosition, wxDefaultSize, wxSP_ARROW_KEYS, 0, 23, 0);
    m_MarketCloseHour->SetToolTip(_("Allow for any delay in quotes"));
    m_yahoopointer->CloseTimeStr_.Mid(0,2).ToLong(&LongTemp);
    m_MarketCloseHour->SetValue(LongTemp);

    fgSizer6->Add(m_MarketCloseHour);

    m_staticText201 = new wxStaticText(this,
        wxID_ANY, wxT(" : "));
    fgSizer6->Add(m_staticText201);

    m_MarketCloseMinute = new wxSpinCtrl( this,
        idMarketCloseMinute, wxEmptyString, wxDefaultPosition,
        wxDefaultSize, wxSP_ARROW_KEYS, 0, 59, 0);
    m_MarketCloseMinute->SetToolTip( _("Allow for any delay in quotes"));
    m_yahoopointer->CloseTimeStr_.Mid(3,2).ToLong(&LongTemp);
    m_MarketCloseMinute->SetValue(LongTemp);

    fgSizer6->Add( m_MarketCloseMinute);
    fgSizer_main->Add(fgSizer6, 1, wxEXPAND);

    fgSizer_main->AddSpacer(1);
    m_checkBoxRefreshPrices = new wxCheckBox(this,
        idCBDoRefresh, _("&Refresh prices every"));
    m_checkBoxRefreshPrices->SetValue(m_yahoopointer->UpdatingEnabled_ ? true : false);
    fgSizer_main->Add(m_checkBoxRefreshPrices);
    fgSizer_main->AddSpacer(1);

    wxBoxSizer* bSizer29;
    bSizer29 = new wxBoxSizer(wxHORIZONTAL);

    m_RefreshInterval = new wxSpinCtrl(this,
        idRefreshInterval, wxEmptyString, wxDefaultPosition,
        wxDefaultSize, wxSP_ARROW_KEYS, 1, 999, 5);
    m_RefreshInterval->SetValue(m_yahoopointer->UpdateIntervalMinutes_);
    fgSizer_main->Add(m_RefreshInterval);

    m_staticText6 = new wxStaticText(this, wxID_ANY,
        _("minutes while market is open"));
    fgSizer_main->AddSpacer(1);
    fgSizer_main->Add(m_staticText6);

    fgSizer_main->Add(bSizer29);

    bSizer1000->Add(fgSizer_main, 1, wxEXPAND|wxALL, 15);

    m_sdbSettings = new wxStdDialogButtonSizer();

    wxPanel* itemPanel25 = new wxPanel(this,
        wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL);
    bSizer1000->Add(itemPanel25, 0, wxALIGN_RIGHT);
    wxStdDialogButtonSizer*  itemStdDialogButtonSizer1 = new wxStdDialogButtonSizer;
    itemPanel25->SetSizer(itemStdDialogButtonSizer1);
    wxButton* itemButtonOK = new wxButton(itemPanel25, wxID_OK);
    itemStdDialogButtonSizer1->Add(itemButtonOK, 0, wxRIGHT, 10);
    wxButton* itemButtonCancel = new wxButton(itemPanel25, wxID_CANCEL);
    itemStdDialogButtonSizer1->Add(itemButtonCancel, 0, wxRIGHT, 10);

    m_sdbSettings->Realize();
    bSizer1000->Add(m_sdbSettings, 0, wxALL, 10);

    this->SetSizer(bSizer1000);

    GetSizer()->Fit(this);
    GetSizer()->SetSizeHints(this);
    Centre();
}
