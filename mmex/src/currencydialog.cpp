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

#include "currencydialog.h"
#include "util.h"
#include "defs.h"
#include "paths.h"

#include <wx/combobox.h>

#ifndef __VISUALC__
#define INT_PTR intptr_t
#endif


IMPLEMENT_DYNAMIC_CLASS( mmCurrencyDialog, wxDialog )

BEGIN_EVENT_TABLE( mmCurrencyDialog, wxDialog )
    EVT_BUTTON(ID_DIALOG_CURRENCY_BUTTON_CANCEL, mmCurrencyDialog::OnCancel)
    EVT_BUTTON(ID_DIALOG_CURRENCY_BUTTON_UPDATE, mmCurrencyDialog::OnEdit)
    EVT_CHOICE(ID_DIALOG_CURRENCY_CHOICE, mmCurrencyDialog::OnCurrencyTypeChanged)  
END_EVENT_TABLE()

mmCurrencyDialog::mmCurrencyDialog( )
{
    core_ = 0;    
    currencyID_ = -1;
}

mmCurrencyDialog::~mmCurrencyDialog()
{
    currencyID_ = -1;     
}

mmCurrencyDialog::mmCurrencyDialog( mmCoreDB* core, wxWindow* parent,
                                   wxWindowID id, const wxString& caption, 
                                   const wxPoint& pos, const wxSize& size, long style )
{
    core_ = core;
    currencyID_ = -1;
    Create(parent, id, caption, pos, size, style);
}

mmCurrencyDialog::mmCurrencyDialog(mmCoreDB* core,  int currencyID, wxWindow* parent, 
                                   wxWindowID id, const wxString& caption, 
                                   const wxPoint& pos, const wxSize& size, long style )
{
    core_ = core;
    currencyID_ = currencyID;
    Create(parent, id, caption, pos, size, style);
}

bool mmCurrencyDialog::Create( wxWindow* parent, wxWindowID id, 
                              const wxString& caption, const wxPoint& pos, 
                              const wxSize& size, long style )
{
    SetExtraStyle(GetExtraStyle()|wxWS_EX_BLOCK_EVENTS);
    wxDialog::Create( parent, id, caption, pos, size, style );

    CreateControls();
    GetSizer()->Fit(this);
    GetSizer()->SetSizeHints(this);

    SetIcon(mmex::getProgramIcon());
    
    fillControls();

    Centre();
    return TRUE;
}

void mmCurrencyDialog::fillControls()
{
    if (!core_)
       return;
    
    if (currencyID_ != -1)
    {
        wxString name = mmDBWrapper::getCurrencyName(core_->db_.get(), currencyID_);
        currencyChoice_->Append(name, (void*)(INT_PTR)currencyID_);
        currencyChoice_->SetStringSelection(name);
    }
    else
    {
       wxASSERT(false);
       currencyChoice_->SetSelection(0);
    }

    updateControls();
}

void mmCurrencyDialog::updateControls()
{
    wxString currencyName = currencyChoice_->GetStringSelection();
    boost::shared_ptr<mmCurrency > pCurrency 
       = core_->currencyList_.getCurrencySharedPtr(currencyName);

    wxTextCtrl* pfxTx = (wxTextCtrl*)FindWindow(ID_DIALOG_CURRENCY_TEXT_PFX);
    wxTextCtrl* sfxTx = (wxTextCtrl*)FindWindow(ID_DIALOG_CURRENCY_TEXT_SFX);
    wxTextCtrl* decTx = (wxTextCtrl*)FindWindow(ID_DIALOG_CURRENCY_TEXT_DECIMAL);
    wxTextCtrl* grpTx = (wxTextCtrl*)FindWindow(ID_DIALOG_CURRENCY_TEXT_GROUP);
    wxTextCtrl* unitTx = (wxTextCtrl*)FindWindow(ID_DIALOG_CURRENCY_TEXT_UNIT);
    wxTextCtrl* centTx = (wxTextCtrl*)FindWindow(ID_DIALOG_CURRENCY_TEXT_CENTS);
    wxTextCtrl* scaleTx = (wxTextCtrl*)FindWindow(ID_DIALOG_CURRENCY_TEXT_SCALE);
    wxStaticText* sample = (wxStaticText*)FindWindow(ID_DIALOG_CURRENCY_STATIC_SAMPLE);
    wxTextCtrl* baseConvRate = (wxTextCtrl*)FindWindow(ID_DIALOG_CURRENCY_TEXT_BASECONVRATE);
    wxComboBox* currencySymbol = (wxComboBox*)FindWindow(ID_DIALOG_CURRENCY_TEXT_SYMBOL);

    pfxTx->SetValue(pCurrency->pfxSymbol_);
    sfxTx->SetValue(pCurrency->sfxSymbol_);
    decTx->SetValue(pCurrency->dec_);
    grpTx->SetValue(pCurrency->grp_);
    unitTx->SetValue(pCurrency->unit_);
    centTx->SetValue(pCurrency->cent_);
    scaleTx->SetValue(wxString::Format(wxT("%f"), pCurrency->scaleDl_));
    baseConvRate->SetValue(wxString::Format(wxT("%f"), pCurrency->baseConv_));
	currencySymbol->SetValue(pCurrency->currencySymbol_);

    wxString dispAmount;
    double amount = 123456.78;
    mmDBWrapper::loadSettings(core_->db_.get(), pCurrency->currencyID_);
    mmCurrencyFormatter::formatDoubleToCurrency(amount, dispAmount);
    sample->SetLabel(dispAmount);

    // resize the dialog window
    Fit();    
}

void mmCurrencyDialog::OnCurrencyTypeChanged(wxCommandEvent& /*event*/)
{
    /*INT_PTR currencyID = (INT_PTR)*/currencyChoice_->GetClientData();
    updateControls();
}

void mmCurrencyDialog::CreateControls()
{    
    mmCurrencyDialog* itemDialog1 = this;

    wxBoxSizer* itemBoxSizer2 = new wxBoxSizer(wxVERTICAL);
    itemDialog1->SetSizer(itemBoxSizer2);

    wxFlexGridSizer* itemFlexGridSizer3 = new wxFlexGridSizer(2, 2, 0, 0);
    itemFlexGridSizer3->AddGrowableCol(1);
    itemBoxSizer2->Add(itemFlexGridSizer3, 0, wxALL, 5);

    wxStaticText* itemStaticText4 = new wxStaticText( itemDialog1, wxID_STATIC, 
        _("Currency Name"), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer3->Add(itemStaticText4, 0, 
        wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxALL|wxADJUST_MINSIZE, 5);

    wxString* itemChoice5Strings = NULL;
    currencyChoice_ = new wxChoice( itemDialog1, ID_DIALOG_CURRENCY_CHOICE, 
        wxDefaultPosition, wxDefaultSize, 0, itemChoice5Strings, 0 );
    itemFlexGridSizer3->Add(currencyChoice_, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxALL, 5);
    currencyChoice_->Enable(false);
	
	wxStaticText* itemStaticText26 = new wxStaticText( itemDialog1, 
        wxID_STATIC, _("Currency Symbol"), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer3->Add(itemStaticText26, 0, 
        wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxALL|wxADJUST_MINSIZE, 5);

    wxComboBox* itemComboBox27 = new wxComboBox( itemDialog1,
          ID_DIALOG_CURRENCY_TEXT_SYMBOL, wxT(""), wxDefaultPosition, wxDefaultSize, 0, NULL, 0);
    itemFlexGridSizer3->Add(itemComboBox27, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxALL|wxEXPAND, 5);
    // add existing currency symbols
    itemComboBox27->Append(wxT("EUR"));
    itemComboBox27->Append(wxT("USD"));
    itemComboBox27->Append(wxT("JPY"));
    itemComboBox27->Append(wxT("BGN"));
    itemComboBox27->Append(wxT("CZK"));
    itemComboBox27->Append(wxT("DKK"));
    itemComboBox27->Append(wxT("EEK"));
    itemComboBox27->Append(wxT("GBP"));
    itemComboBox27->Append(wxT("HUF"));
    itemComboBox27->Append(wxT("LTL"));
    itemComboBox27->Append(wxT("LVL"));
    itemComboBox27->Append(wxT("PLN"));
    itemComboBox27->Append(wxT("RON"));
    itemComboBox27->Append(wxT("SEK"));
    itemComboBox27->Append(wxT("SKK"));
    itemComboBox27->Append(wxT("CHF"));
    itemComboBox27->Append(wxT("ISK"));
    itemComboBox27->Append(wxT("NOK"));
    itemComboBox27->Append(wxT("HRK"));
    itemComboBox27->Append(wxT("RUB"));
    itemComboBox27->Append(wxT("UAH"));
    itemComboBox27->Append(wxT("TRY"));
    itemComboBox27->Append(wxT("AUD"));
    itemComboBox27->Append(wxT("BRL"));
    itemComboBox27->Append(wxT("CAD"));
    itemComboBox27->Append(wxT("CNY"));
    itemComboBox27->Append(wxT("HKD"));
    itemComboBox27->Append(wxT("IDR"));
    itemComboBox27->Append(wxT("KRW"));
    itemComboBox27->Append(wxT("MXN"));
    itemComboBox27->Append(wxT("MYR"));
    itemComboBox27->Append(wxT("NZD"));
    itemComboBox27->Append(wxT("PHP"));
    itemComboBox27->Append(wxT("SGD"));
    itemComboBox27->Append(wxT("THB"));
    itemComboBox27->Append(wxT("ZAR"));

    wxStaticText* itemStaticText6 = new wxStaticText( itemDialog1, 
        wxID_STATIC, _("Prefix Symbol"), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer3->Add(itemStaticText6, 0, 
        wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxALL|wxADJUST_MINSIZE, 5);

    wxTextCtrl* itemTextCtrl7 = new wxTextCtrl( itemDialog1, 
        ID_DIALOG_CURRENCY_TEXT_PFX, wxT(""), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer3->Add(itemTextCtrl7, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxALL|wxEXPAND, 5);

    wxStaticText* itemStaticText8 = new wxStaticText( itemDialog1, 
        wxID_STATIC, _("Suffix Symbol"), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer3->Add(itemStaticText8, 0, 
       wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxALL|wxADJUST_MINSIZE, 5);

    wxTextCtrl* itemTextCtrl9 = new wxTextCtrl( itemDialog1, 
       ID_DIALOG_CURRENCY_TEXT_SFX, wxT(""), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer3->Add(itemTextCtrl9, 0, 
        wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxALL|wxEXPAND, 5);

    wxStaticText* itemStaticText10 = new wxStaticText( itemDialog1, wxID_STATIC, _("Decimal Char"), 
        wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer3->Add(itemStaticText10, 0, 
        wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxALL|wxADJUST_MINSIZE, 5);

    wxTextCtrl* itemTextCtrl11 = new wxTextCtrl( itemDialog1, 
        ID_DIALOG_CURRENCY_TEXT_DECIMAL, wxT(""), 
        wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer3->Add(itemTextCtrl11, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxALL|wxEXPAND, 5);

    wxStaticText* itemStaticText12 = new wxStaticText( itemDialog1, 
        wxID_STATIC, _("Grouping Char"), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer3->Add(itemStaticText12, 0, 
        wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxALL|wxADJUST_MINSIZE, 5);

    wxTextCtrl* itemTextCtrl13 = new wxTextCtrl( itemDialog1, 
        ID_DIALOG_CURRENCY_TEXT_GROUP, wxT(""), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer3->Add(itemTextCtrl13, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxALL|wxEXPAND, 5);

    wxStaticText* itemStaticText14 = new wxStaticText( itemDialog1, 
        wxID_STATIC, _("Unit Name"), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer3->Add(itemStaticText14, 0, 
        wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxALL|wxADJUST_MINSIZE, 5);

    wxTextCtrl* itemTextCtrl15 = new wxTextCtrl( itemDialog1, 
        ID_DIALOG_CURRENCY_TEXT_UNIT, 
        wxT(""), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer3->Add(itemTextCtrl15, 0, 
        wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxALL|wxEXPAND, 5);

    wxStaticText* itemStaticText16 = new wxStaticText( itemDialog1, wxID_STATIC, 
        _("Cents Name"), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer3->Add(itemStaticText16, 0, 
        wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxALL|wxADJUST_MINSIZE, 5);

    wxTextCtrl* itemTextCtrl17 = new wxTextCtrl( itemDialog1, 
        ID_DIALOG_CURRENCY_TEXT_CENTS, wxT(""), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer3->Add(itemTextCtrl17, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxALL|wxEXPAND, 5);

    wxStaticText* itemStaticText81 = new wxStaticText( itemDialog1, wxID_STATIC, 
        _("Conversion to Base Rate"), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer3->Add(itemStaticText81, 0, 
        wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxALL|wxADJUST_MINSIZE, 5);

    wxTextCtrl* itemTextCtrl82 = new wxTextCtrl( itemDialog1, 
        ID_DIALOG_CURRENCY_TEXT_BASECONVRATE, wxT(""), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer3->Add(itemTextCtrl82, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxALL|wxEXPAND, 5);

    wxStaticText* itemStaticText18 = new wxStaticText( itemDialog1, 
        wxID_STATIC, _("Scale"), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer3->Add(itemStaticText18, 0, 
        wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxALL|wxADJUST_MINSIZE, 5);

    wxTextCtrl* itemTextCtrl19 = new wxTextCtrl( itemDialog1, 
        ID_DIALOG_CURRENCY_TEXT_SCALE, wxT(""), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer3->Add(itemTextCtrl19, 0, 
        wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxALL|wxEXPAND, 5);
    
    wxStaticText* itemStaticText20 = new wxStaticText( itemDialog1, 
        wxID_STATIC, _("Value of 123456.78 will be"), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer3->Add(itemStaticText20, 0, 
        wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxALL|wxADJUST_MINSIZE, 5);

    wxStaticText*  sampleText_ = new wxStaticText( itemDialog1, 
        ID_DIALOG_CURRENCY_STATIC_SAMPLE, _("1234567.89"), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer3->Add(sampleText_, 0, 
        wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxALL|wxADJUST_MINSIZE, 5);

    wxBoxSizer* itemBoxSizer22 = new wxBoxSizer(wxHORIZONTAL);
    itemBoxSizer2->Add(itemBoxSizer22, 0, wxALIGN_CENTER_HORIZONTAL|wxALL, 5);

    wxButton* itemButton24 = new wxButton( itemDialog1, 
        ID_DIALOG_CURRENCY_BUTTON_UPDATE, _("&Update"), wxDefaultPosition, wxDefaultSize, 0 );
    itemBoxSizer22->Add(itemButton24, 0, wxALIGN_CENTER_HORIZONTAL|wxALL, 5);

    wxButton* itemButton25 = new wxButton( itemDialog1, 
        ID_DIALOG_CURRENCY_BUTTON_CANCEL, _("&Close"), wxDefaultPosition, wxDefaultSize, 0 );
    itemBoxSizer22->Add(itemButton25, 0, wxALIGN_CENTER_HORIZONTAL|wxALL, 5);
}

void mmCurrencyDialog::OnCancel(wxCommandEvent& /*event*/)
{
   EndModal(wxID_OK);   
}

void mmCurrencyDialog::OnBSelect(wxCommandEvent& /*event*/)
{
    wxString currencyName = currencyChoice_->GetStringSelection();
    currencyID_ = mmDBWrapper::getCurrencyID(core_->db_.get(), currencyName);
    EndModal(wxID_OK);
}

void mmCurrencyDialog::OnEdit(wxCommandEvent& /*event*/)
{
    wxString currencyName = currencyChoice_->GetStringSelection();

    wxTextCtrl* pfxTx = (wxTextCtrl*)FindWindow(ID_DIALOG_CURRENCY_TEXT_PFX);
    wxTextCtrl* sfxTx = (wxTextCtrl*)FindWindow(ID_DIALOG_CURRENCY_TEXT_SFX);
    wxTextCtrl* decTx = (wxTextCtrl*)FindWindow(ID_DIALOG_CURRENCY_TEXT_DECIMAL);
    wxTextCtrl* grpTx = (wxTextCtrl*)FindWindow(ID_DIALOG_CURRENCY_TEXT_GROUP);
    wxTextCtrl* unitTx = (wxTextCtrl*)FindWindow(ID_DIALOG_CURRENCY_TEXT_UNIT);
    wxTextCtrl* centTx = (wxTextCtrl*)FindWindow(ID_DIALOG_CURRENCY_TEXT_CENTS);
    wxTextCtrl* scaleTx = (wxTextCtrl*)FindWindow(ID_DIALOG_CURRENCY_TEXT_SCALE);
    wxTextCtrl* baseConvRate = (wxTextCtrl*)FindWindow(ID_DIALOG_CURRENCY_TEXT_BASECONVRATE);
	wxComboBox* currencySymbol = (wxComboBox*)FindWindow(ID_DIALOG_CURRENCY_TEXT_SYMBOL);

    double scal = 0.0;
    scaleTx->GetValue().ToDouble(&scal);

    double convRate = 1.0;
    baseConvRate->GetValue().ToDouble(&convRate);
    if (convRate <= 0.0)
    {
        mmShowErrorMessageInvalid(this, _("Base Conversion Rate should be greater than 0.0"));
        return;
    }

    boost::shared_ptr<mmCurrency> pCurrency = core_->currencyList_.getCurrencySharedPtr(currencyName);
    wxASSERT(pCurrency->currencyID_ == currencyID_);
    
    pCurrency->pfxSymbol_ = pfxTx->GetValue();
    pCurrency->sfxSymbol_ = sfxTx->GetValue();
    pCurrency->dec_ = decTx->GetValue();
    pCurrency->grp_ =  grpTx->GetValue();
    pCurrency->unit_ = unitTx->GetValue();
    pCurrency->cent_ = centTx->GetValue();
    pCurrency->scaleDl_ = scal;
    pCurrency->baseConv_ = convRate;
	pCurrency->currencySymbol_ = currencySymbol->GetValue();
   
    core_->currencyList_.updateCurrency(pCurrency);

    fillControls();
}
