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

#include "stockdialog.h"
#include "wx/datectrl.h"
#include "util.h"
#include "dbwrapper.h"
#include "currencydialog.h"
#include "defs.h"
#include "paths.h"

IMPLEMENT_DYNAMIC_CLASS( mmStockDialog, wxDialog )

BEGIN_EVENT_TABLE( mmStockDialog, wxDialog )
    EVT_BUTTON(ID_BUTTON_STOCK_OK, mmStockDialog::OnOk)
    EVT_BUTTON(ID_BUTTON_STOCK_CANCEL, mmStockDialog::OnCancel)
    EVT_BUTTON(ID_BUTTON_STOCKS_HELDAT, mmStockDialog::OnAccountButton)
	EVT_BUTTON(ID_BUTTON_STOCK_WEBPRICE, mmStockDialog::OnStockPriceButton)
END_EVENT_TABLE()

mmStockDialog::mmStockDialog( )
{
}

mmStockDialog::mmStockDialog(wxSQLite3Database* db, int stockID, bool edit,
                             wxWindow* parent, wxWindowID id, 
                             const wxString& caption, const wxPoint& pos, 
                             const wxSize& size, long style )
{
    db_ = db;
    stockID_ = stockID;
    edit_ = edit;
    accountID_ = -1;
    Create(parent, id, caption, pos, size, style);
    mmDBWrapper::loadBaseCurrencySettings(db_);
}

bool mmStockDialog::Create( wxWindow* parent, wxWindowID id, const wxString& caption, 
                           const wxPoint& pos, const wxSize& size, long style )
{
    SetExtraStyle(GetExtraStyle()|wxWS_EX_BLOCK_EVENTS);
    wxDialog::Create( parent, id, caption, pos, size, style );

    CreateControls();
    GetSizer()->Fit(this);
    GetSizer()->SetSizeHints(this);

    SetIcon(mmex::getProgramIcon());
    
    fillControls();

    if (edit_)
    {
        dataToControls();
    }

    Centre();
    return TRUE;
}

void mmStockDialog::dataToControls()
{
    static const char sql[] = 
    "select HELDAT, STOCKNAME, SYMBOL, NOTES, PURCHASEDATE, NUMSHARES, VALUE, PURCHASEPRICE, CURRENTPRICE, COMMISSION "
    "from STOCK_V1 "
    "where STOCKID = ?";

    wxSQLite3Statement st = db_->PrepareStatement(sql); 
    st.Bind(1, stockID_);

    wxSQLite3ResultSet q1 = st.ExecuteQuery();
    if (q1.NextRow())
    {
        heldAt_->SetLabel(mmDBWrapper::getAccountName(db_, q1.GetInt(wxT("HELDAT"))));
        accountID_ = q1.GetInt(wxT("HELDAT"));
        stockName_->SetValue(q1.GetString(wxT("STOCKNAME")));
        stockSymbol_->SetValue(q1.GetString(wxT("SYMBOL")));
        notes_->SetValue(q1.GetString(wxT("NOTES")));

        wxString dateString = q1.GetString(wxT("PURCHASEDATE"));
        wxDateTime dtdt = mmGetStorageStringAsDate(dateString);
        wxString dt = mmGetDateForDisplay(db_, dtdt);
        dpc_->SetValue(dtdt);

		wxString numShares;
		mmex::formatDoubleToCurrencyEdit(q1.GetDouble(wxT("NUMSHARES")), numShares);
		numShares_->SetValue(numShares);

        wxString dispAmount;
        mmex::formatDoubleToCurrencyEdit(q1.GetDouble(wxT("VALUE")), dispAmount);
        valueInvestment_->SetLabel(dispAmount);
    
        mmex::formatDoubleToCurrencyEdit(q1.GetDouble(wxT("PURCHASEPRICE")), dispAmount);
        purchasePrice_->SetValue(dispAmount);

        mmex::formatDoubleToCurrencyEdit(q1.GetDouble(wxT("CURRENTPRICE")), dispAmount);
        currentPrice_->SetValue(dispAmount);

        mmex::formatDoubleToCurrencyEdit(q1.GetDouble(wxT("COMMISSION")), dispAmount);
        commission_->SetValue(dispAmount);
    }
    st.Finalize();
}

void mmStockDialog::fillControls()
{
    
}

void mmStockDialog::CreateControls()
{    
    mmStockDialog* itemDialog1 = this;

    wxBoxSizer* itemBoxSizer2 = new wxBoxSizer(wxHORIZONTAL);
    itemDialog1->SetSizer(itemBoxSizer2);

    wxBoxSizer* itemBoxSizer3 = new wxBoxSizer(wxVERTICAL);
    itemBoxSizer2->Add(itemBoxSizer3, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

    wxStaticBox* itemStaticBoxSizer4Static = new wxStaticBox(itemDialog1, wxID_ANY, _("Stock Investment Details"));
    wxStaticBoxSizer* itemStaticBoxSizer4 = new wxStaticBoxSizer(itemStaticBoxSizer4Static, wxVERTICAL);
    itemBoxSizer3->Add(itemStaticBoxSizer4, 0, wxALIGN_CENTER_HORIZONTAL|wxALL, 5);

    wxPanel* itemPanel5 = new wxPanel( itemDialog1, ID_PANEL3, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
    itemStaticBoxSizer4->Add(itemPanel5, 1, wxGROW|wxALL, 5);

    wxFlexGridSizer* itemFlexGridSizer6 = new wxFlexGridSizer(4, 4, 0, 0);
    itemPanel5->SetSizer(itemFlexGridSizer6);

    wxStaticText* itemStaticText7 = new wxStaticText( itemPanel5, wxID_STATIC, _("Held At"), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer6->Add(itemStaticText7, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxALL|wxADJUST_MINSIZE, 5);

    heldAt_ = new wxButton( itemPanel5, ID_BUTTON_STOCKS_HELDAT, _("Select Account"), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer6->Add(heldAt_, 0, wxGROW|wxALIGN_CENTER_VERTICAL|wxALL, 5);
    heldAt_->SetToolTip(_("Enter the name of the financial institution where the investment is held"));
    
    wxStaticText* itemStaticText9 = new wxStaticText( itemPanel5, wxID_STATIC, _("Date"), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer6->Add(itemStaticText9, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxALL|wxADJUST_MINSIZE, 5);
    
    dpc_ = new wxDatePickerCtrl( itemPanel5, ID_DPC_STOCK_PDATE, wxDefaultDateTime, wxDefaultPosition, wxDefaultSize, wxDP_DROPDOWN);
    itemFlexGridSizer6->Add(dpc_, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxALL, 5);
    dpc_->SetToolTip(_("Specify the purchase date of the stock investment"));

    wxStaticText* itemStaticText11 = new wxStaticText( itemPanel5, wxID_STATIC, _("Stock Name"), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer6->Add(itemStaticText11, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxALL|wxADJUST_MINSIZE, 5);

    stockName_ = new wxTextCtrl( itemPanel5, ID_TEXTCTRL_STOCKNAME, wxT(""), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer6->Add(stockName_, 0, wxGROW|wxALIGN_CENTER_VERTICAL|wxALL, 5);
    stockName_->SetToolTip(_("Enter the stock company name"));

    wxStaticText* itemStaticText13 = new wxStaticText( itemPanel5, wxID_STATIC, _("Symbol"), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer6->Add(itemStaticText13, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxALL|wxADJUST_MINSIZE, 5);

    stockSymbol_ = new wxTextCtrl( itemPanel5, ID_TEXTCTRL_STOCK_SYMBOL, wxT(""), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer6->Add(stockSymbol_, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL|wxALL, 5);
    stockSymbol_->SetToolTip(_("Enter the stock symbol. (Optional) Include exchange. eg: IBM.BE"));

    wxStaticText* itemStaticText15 = new wxStaticText( itemPanel5, wxID_STATIC, _("Number of Shares"), 
        wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer6->Add(itemStaticText15, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxALL|wxADJUST_MINSIZE, 5);

    numShares_ = new wxTextCtrl( itemPanel5, ID_TEXTCTRL_NUMBER_SHARES, wxT(""), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer6->Add(numShares_, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxALL, 5);
    numShares_->SetToolTip(_("Enter number of shares held"));

    wxStaticText* itemStaticText17 = new wxStaticText( itemPanel5, wxID_STATIC, _("Purchase Price"),
        wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer6->Add(itemStaticText17, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL|wxALL|wxADJUST_MINSIZE, 5);

    purchasePrice_ = new wxTextCtrl( itemPanel5, ID_TEXTCTRL_STOCK_PP, wxT(""), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer6->Add(purchasePrice_, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL|wxALL, 5);
    purchasePrice_->SetToolTip(_("Enter purchase price for each stock"));

    wxStaticText* itemStaticText211 = new wxStaticText( itemPanel5, wxID_STATIC, _("Commission"), 
        wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer6->Add(itemStaticText211, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxALL|wxADJUST_MINSIZE, 5);

    commission_ = new wxTextCtrl( itemPanel5, ID_TEXTCTRL_STOCK_COMMISSION, wxT("0"), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer6->Add(commission_, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxALL, 5);
    commission_->SetToolTip(_("Enter any commission paid"));

    wxStaticText* itemStaticText21 = new wxStaticText( itemPanel5, wxID_STATIC, _("Current Price"), 
        wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer6->Add(itemStaticText21, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxALL|wxADJUST_MINSIZE, 5);

    currentPrice_ = new wxTextCtrl( itemPanel5, ID_TEXTCTRL_STOCK_CP, wxT(""), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer6->Add(currentPrice_, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL|wxALL, 5);
    currentPrice_->SetToolTip(_("Enter current stock price"));

    wxStaticText* itemStaticText19 = new wxStaticText( itemPanel5, wxID_STATIC, _("Notes"), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer6->Add(itemStaticText19, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxALL|wxADJUST_MINSIZE, 5);

    notes_ = new wxTextCtrl( itemPanel5, ID_TEXTCTRL_STOCK_NOTES, wxT(""), wxDefaultPosition, wxSize(200, 75), wxTE_MULTILINE );
    itemFlexGridSizer6->Add(notes_, 0, wxGROW|wxALIGN_CENTER_VERTICAL|wxALL, 5);
    notes_->SetToolTip(_("Enter notes associated with this investment"));

    wxStaticText* itemStaticText25 = new wxStaticText( itemPanel5, wxID_STATIC, _("Value"), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer6->Add(itemStaticText25, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxALL|wxADJUST_MINSIZE, 5);

    valueInvestment_ = new wxStaticText( itemPanel5, ID_STATIC_STOCK_VALUE, wxT("--"), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer6->Add(valueInvestment_, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL|wxALL|wxADJUST_MINSIZE, 5);
    
    wxPanel* itemPanel27 = new wxPanel( itemDialog1, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
    itemBoxSizer3->Add(itemPanel27, 0, wxALIGN_RIGHT|wxALL, 5);

    wxBoxSizer* itemBoxSizer28 = new wxBoxSizer(wxHORIZONTAL);
    itemPanel27->SetSizer(itemBoxSizer28);

    wxButton* itemButton31 = new wxButton( itemPanel27, ID_BUTTON_STOCK_WEBPRICE, _("Display Stock Web Page"));
    itemBoxSizer28->Add(itemButton31, 0, wxALIGN_CENTER_VERTICAL|wxALL, 1);
    itemButton31->SetToolTip(_("Will display the web page for the specified Stock symbol"));

    wxButton* itemButton29 = new wxButton( itemPanel27, ID_BUTTON_STOCK_OK, _("OK"));
    itemBoxSizer28->Add(itemButton29, 0, wxALIGN_CENTER_VERTICAL|wxALL, 1);
    itemButton29->SetForegroundColour(wxColour(wxT("FOREST GREEN")));

    wxButton* itemButton30 = new wxButton( itemPanel27, ID_BUTTON_STOCK_CANCEL, _("Cancel"));
    itemBoxSizer28->Add(itemButton30, 0, wxALIGN_CENTER_VERTICAL|wxALL, 1);
    itemButton30->SetForegroundColour(wxColour(wxT("RED")));
    itemButton30->SetFocus();
}

void mmStockDialog::OnAccountButton(wxCommandEvent& /*event*/)
{
    static const char sql[] = 
    "select ACCOUNTNAME "
    "from ACCOUNTLIST_V1 "
    "where ACCOUNTTYPE = 'Investment' "
    "order by ACCOUNTNAME";

    wxArrayString as;
    
    wxSQLite3ResultSet q1 = db_->ExecuteQuery(sql);
    while (q1.NextRow())
    {
        as.Add(q1.GetString(wxT("ACCOUNTNAME")));
    }
    q1.Finalize();
    
    wxSingleChoiceDialog scd(0, _("Choose Investment Account"), _("Select Account"), as);
    if (scd.ShowModal() == wxID_OK)
    {
        wxString acctName = scd.GetStringSelection();
        accountID_ = mmDBWrapper::getAccountID(db_, acctName);
        heldAt_->SetLabel(acctName);
    }
}

void mmStockDialog::OnCancel(wxCommandEvent& /*event*/)
{
    Close(TRUE);
}

void mmStockDialog::OnStockPriceButton(wxCommandEvent& /*event*/)
{
	wxString stockSymbol = stockSymbol_->GetValue().Trim();

	if (!stockSymbol.IsEmpty())
	{
		// Use Google for stock quotes
        wxString stockURL = mmDBWrapper::getInfoSettingValue(db_, wxT("STOCKURL"), mmex::DEFSTOCKURL);
        //wxString paddedURL = wxT("\"") + stockURL + wxT("\"");
		//wxString httpString = wxString::Format(paddedURL, stockSymbol);
        //wxExecute(_T("explorer ") + httpString, wxEXEC_ASYNC, NULL ); 
    
        int yahooSite = stockURL.Find(wxT("yahoo"));
        if ( yahooSite != wxNOT_FOUND )
        {
            int hasSuffix = stockSymbol.Find(wxT("."));
            if ( hasSuffix == wxNOT_FOUND)
            {
                wxString stockSuffix = mmDBWrapper::getInfoSettingValue(db_,wxT("HTTP_YAHOO_SUFFIX"), wxT(""));
                if (! stockSuffix.IsEmpty() )
                    stockSymbol << stockSuffix;
            }
        }
        wxString httpString = wxString::Format(stockURL.c_str(), stockSymbol.c_str());
		wxLaunchDefaultBrowser(httpString);
	}
}

void mmStockDialog::OnOk(wxCommandEvent& /*event*/)
{
    wxString pdate = dpc_->GetValue().FormatISODate();
    
    if (accountID_ == -1)
    {
        mmShowErrorMessageInvalid(this, _("Held At"));
        return;
    }
    wxString heldAt =  mmDBWrapper::getAccountName(db_,accountID_);
        
    wxString stockName = stockName_->GetValue();
    wxString stockSymbol = stockSymbol_->GetValue();
    wxString notes       = notes_->GetValue();

    wxString numSharesStr = numShares_->GetValue().Trim();
    if (numSharesStr == wxT(""))
    {
        mmShowErrorMessageInvalid(this, _("Num Shares"));
        return;
    }
    
     double numShares = 0;
	 if (!mmex::formatCurrencyToDouble(numSharesStr, numShares) || (numShares < 0.0))
    {
        mmShowErrorMessage(this, _("Invalid number of shares entered "), _("Error"));
        return;
    }

    if (numShares <= 0)
    {
        mmShowErrorMessageInvalid(this, _("Num Shares"));
        return;
    }

    wxString pPriceStr    = purchasePrice_->GetValue().Trim();
    double pPrice;
    if (!mmex::formatCurrencyToDouble(pPriceStr, pPrice) || (pPrice < 0.0))
    {
        mmShowErrorMessage(this, _("Invalid purchase price entered "), _("Error"));
        return;
    }
    
    wxString currentPriceStr = currentPrice_->GetValue().Trim();
    double cPrice;
    if (!mmex::formatCurrencyToDouble(currentPriceStr, cPrice) || (cPrice < 0.0))
    {
        //mmShowErrorMessage(this, _("Invalid current price entered "), _("Error"));        
        //return;
        // we assume current price = purchase price
        cPrice = pPrice;
    }
    
    wxString commissionStr = commission_->GetValue().Trim();
    double commission;
    if (!mmex::formatCurrencyToDouble(commissionStr, commission) || (commission < 0.0))
    {
        mmShowErrorMessage(this, _("Invalid commission entered "), _("Error"));
        return;
    }

    double cValue = cPrice * numShares;

    if (!edit_)
    {
        static const char sql[]  = 
        "insert into STOCK_V1 ( "
          "HELDAT, PURCHASEDATE, STOCKNAME, SYMBOL, "
          "NUMSHARES, PURCHASEPRICE, NOTES, CURRENTPRICE, "
          "VALUE, COMMISSION "
        " ) values (?, ?, ?, ?, ?, ?, ?, ?, ?, ?)";
        
        wxSQLite3Statement st = db_->PrepareStatement(sql);

        int i = 0;
        st.Bind(++i, accountID_);
        st.Bind(++i, pdate);
        st.Bind(++i, stockName);
        st.Bind(++i, stockSymbol);
        st.Bind(++i, numShares);
        st.Bind(++i, pPrice);
        st.Bind(++i, notes);
        st.Bind(++i, cPrice);
        st.Bind(++i, cValue);
        st.Bind(++i, commission);

        wxASSERT(st.GetParamCount() == i);

        st.ExecuteUpdate();
        st.Finalize();
    }
    else 
    {
        static const char sql[]  = 
        "update STOCK_V1 "
        "SET HELDAT=?, PURCHASEDATE=?, STOCKNAME=?, SYMBOL=?, "
            "NUMSHARES=?, PURCHASEPRICE=?, NOTES=?, CURRENTPRICE=?, "
            "VALUE=?, COMMISSION=? "
        "WHERE STOCKID = ?";

        wxSQLite3Statement st = db_->PrepareStatement(sql);

        int i = 0;
        st.Bind(++i, accountID_);
        st.Bind(++i, pdate);
        st.Bind(++i, stockName);
        st.Bind(++i, stockSymbol);
        st.Bind(++i, numShares);
        st.Bind(++i, pPrice);
        st.Bind(++i, notes);
        st.Bind(++i, cPrice);
        st.Bind(++i, cValue);
        st.Bind(++i, commission);
        st.Bind(++i, stockID_);

        wxASSERT(st.GetParamCount() == i);

        st.ExecuteUpdate();
        st.Finalize();
    }

    EndModal(wxID_OK);
}

