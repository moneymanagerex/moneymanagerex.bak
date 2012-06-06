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

#ifndef _YahooSettingsDialog_H_
#define _YahooSettingsDialog_H_

#include "guiid.h"

#include <wx/wx.h>

#include <wx/spinctrl.h>
#include <wx/panel.h>

#include "mmyahoo.h"

///////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
/// Class YahooSettingsDialog
///////////////////////////////////////////////////////////////////////////////
class YahooSettingsDialog : public wxDialog
{
	DECLARE_EVENT_TABLE()
	private:

	protected:
	public:
		enum
		{
			idYahooSettingsDialog = wxID_HIGHEST + 1000,
			idSettingsNotebook,
			idHTTPSettingsPanel,
			idYahooSuffix,
			idYahooServer,
			idMarketOpenHour,
			idMarketOpenMinute,
			idMarketCloseHour,
			idMarketCloseMinute,
			idCBDoRefresh,
			idRefreshInterval,
			idWebProxyBox,
			idProxySettingsPanel,
			idProxyHost,
			idProxyPort,
			idProxyAuthentication,
			idProxyUser,
			idProxyPassword,
			idSaveProxyPassword,
			idProxyExceptions,
		};

		wxPanel* m_HTTPSettingsPanel;
		wxTextCtrl* m_YahooSuffix;
		wxSpinCtrl* m_MarketOpenHour;
		wxSpinCtrl* m_MarketOpenMinute;
		wxSpinCtrl* m_MarketCloseHour;
		wxSpinCtrl* m_MarketCloseMinute;
		wxCheckBox* m_checkBoxRefreshPrices;
		wxSpinCtrl* m_RefreshInterval;
		wxRadioBox* m_WebProxy;
		wxPanel* m_ProxySettingsPanel;
		wxTextCtrl* m_ProxyHost;
		wxTextCtrl* m_ProxyPort;

		wxCheckBox* m_ProxyAuthentication;
		wxTextCtrl* m_ProxyUser;
		wxStdDialogButtonSizer* m_sdbSettings;
		wxTextCtrl* m_YahooServer;

		wxButton* m_proxySettingsButton;

	public:
		YahooSettingsDialog(wxWindow* parent, 
            int id = idYahooSettingsDialog, 
            wxString title = _("Options"), 
            wxPoint pos = wxDefaultPosition, 
            wxSize size = wxDefaultSize, 
            int style = wxDEFAULT_DIALOG_STYLE );
};
#endif //_YahooSettingsDialog_H_
