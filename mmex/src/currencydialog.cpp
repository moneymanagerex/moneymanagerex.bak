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
    EVT_BUTTON(ID_DIALOG_CURRENCY_BUTTON_UPDATE, mmCurrencyDialog::OnUpdate)
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
        wxString name = core_->currencyList_.getCurrencyName(currencyID_);
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
    boost::shared_ptr<mmCurrency > pCurrency = core_->currencyList_.getCurrencySharedPtr(currencyName);

    wxTextCtrl* pfxTx = (wxTextCtrl*)FindWindow(ID_DIALOG_CURRENCY_TEXT_PFX);
    wxTextCtrl* sfxTx = (wxTextCtrl*)FindWindow(ID_DIALOG_CURRENCY_TEXT_SFX);
    wxTextCtrl* decTx = (wxTextCtrl*)FindWindow(ID_DIALOG_CURRENCY_TEXT_DECIMAL);
    wxTextCtrl* grpTx = (wxTextCtrl*)FindWindow(ID_DIALOG_CURRENCY_TEXT_GROUP);
    wxTextCtrl* unitTx = (wxTextCtrl*)FindWindow(ID_DIALOG_CURRENCY_TEXT_UNIT);
    wxTextCtrl* centTx = (wxTextCtrl*)FindWindow(ID_DIALOG_CURRENCY_TEXT_CENTS);
    wxTextCtrl* scaleTx = (wxTextCtrl*)FindWindow(ID_DIALOG_CURRENCY_TEXT_SCALE);
    wxTextCtrl* baseConvRate = (wxTextCtrl*)FindWindow(ID_DIALOG_CURRENCY_TEXT_BASECONVRATE);
    wxComboBox* currencySymbol = (wxComboBox*)FindWindow(ID_DIALOG_CURRENCY_TEXT_SYMBOL);

    pfxTx->SetValue(pCurrency->pfxSymbol_);
    sfxTx->SetValue(pCurrency->sfxSymbol_);
    decTx->SetValue(pCurrency->dec_);
    grpTx->SetValue(pCurrency->grp_);
    unitTx->SetValue(pCurrency->unit_);
    centTx->SetValue(pCurrency->cent_);
    scaleTx->SetValue(wxString() << pCurrency->scaleDl_);
    baseConvRate->SetValue(wxString() << pCurrency->baseConv_);
    currencySymbol->SetValue(pCurrency->currencySymbol_);

    wxString dispAmount;
    wxString dispAmount2;
    double amount = 1000;

    core_->currencyList_.LoadBaseCurrencySettings();
    mmex::formatDoubleToCurrency(amount, dispAmount);
    dispAmount2 = wxString() << dispAmount << wxT(" ") << _("Converted to:") << wxT(" ");
        
    mmDBWrapper::loadCurrencySettings(core_->db_.get(), pCurrency->currencyID_);
    if (pCurrency->baseConv_ != 0.0 )
        amount = amount / pCurrency->baseConv_;
    else
        amount = 0.0;
    mmex::formatDoubleToCurrency(amount, dispAmount);
    baseRateSample_->SetLabel(dispAmount2 + dispAmount);

    amount = 123456.78;
    mmex::formatDoubleToCurrency(amount, dispAmount);
    dispAmount = wxString() << wxT("123456.78 ") << _("Shown As: ") << dispAmount;
    sampleText_->SetLabel(dispAmount);

    // resize the dialog window
    Fit();    
}

void mmCurrencyDialog::OnCurrencyTypeChanged(wxCommandEvent& /*event*/)
{
    /*INT_PTR currencyID = (INT_PTR)*/
    currencyChoice_->GetClientData();
    updateControls();
}

void mmCurrencyDialog::CreateControls()
{    
    wxSizerFlags flags, flagsExpand;
    flags.Align(wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL).Border(wxALL, 5);
    flagsExpand.Align(wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL).Border(wxALL, 5).Expand();

    wxBoxSizer* itemBoxSizer2 = new wxBoxSizer(wxVERTICAL);
    this->SetSizer(itemBoxSizer2);

    wxFlexGridSizer* itemFlexGridSizer3 = new wxFlexGridSizer(0, 2, 0, 0);
    itemFlexGridSizer3->AddGrowableCol(1);
    itemBoxSizer2->Add(itemFlexGridSizer3, flags);

    //--------------------------
    wxStaticText* itemStaticText4 = new wxStaticText( this, wxID_STATIC, _("Currency Name"));
    itemFlexGridSizer3->Add(itemStaticText4, flags);

    currencyChoice_ = new wxChoice( this, ID_DIALOG_CURRENCY_CHOICE);
    itemFlexGridSizer3->Add(currencyChoice_, flags);
    currencyChoice_->Enable(false);
    
    wxStaticText* itemStaticText26 = new wxStaticText( this, wxID_STATIC, _("Currency Symbol"));
    itemFlexGridSizer3->Add(itemStaticText26, flags);

    wxArrayString currency_symbols;
    currency_symbols.Add(wxT("USD"));
    currency_symbols.Add(wxT("EUR"));
    currency_symbols.Add(wxT("AUD"));
    currency_symbols.Add(wxT("BGN"));
    currency_symbols.Add(wxT("BRL"));
    currency_symbols.Add(wxT("CAD"));
    currency_symbols.Add(wxT("CHF"));
    currency_symbols.Add(wxT("CNY"));
    currency_symbols.Add(wxT("CZK"));
    currency_symbols.Add(wxT("DKK"));
    currency_symbols.Add(wxT("EEK"));
    currency_symbols.Add(wxT("GBP"));
    currency_symbols.Add(wxT("HKD"));
    currency_symbols.Add(wxT("HRK"));
    currency_symbols.Add(wxT("HUF"));
    currency_symbols.Add(wxT("IDR"));
    currency_symbols.Add(wxT("ISK"));
    currency_symbols.Add(wxT("JPY"));
    currency_symbols.Add(wxT("KRW"));
    currency_symbols.Add(wxT("LTL"));
    currency_symbols.Add(wxT("LVL"));
    currency_symbols.Add(wxT("MXN"));
    currency_symbols.Add(wxT("MYR"));
    currency_symbols.Add(wxT("NOK"));
    currency_symbols.Add(wxT("NZD"));
    currency_symbols.Add(wxT("PHP"));
    currency_symbols.Add(wxT("PLN"));
    currency_symbols.Add(wxT("RON"));
    currency_symbols.Add(wxT("RUB"));
    currency_symbols.Add(wxT("SEK"));
    currency_symbols.Add(wxT("SGD"));
    currency_symbols.Add(wxT("SKK"));
    currency_symbols.Add(wxT("THB"));
    currency_symbols.Add(wxT("TRY"));
    currency_symbols.Add(wxT("UAH"));
    currency_symbols.Add(wxT("ZAR"));

    wxComboBox* itemComboBox27 = new wxComboBox( this, ID_DIALOG_CURRENCY_TEXT_SYMBOL, wxT(""),
        wxDefaultPosition, wxDefaultSize, currency_symbols);
    itemFlexGridSizer3->Add(itemComboBox27, flagsExpand);

    wxStaticText* itemStaticText14 = new wxStaticText( this, wxID_STATIC, _("Unit Name"));
    itemFlexGridSizer3->Add(itemStaticText14, flags);

    wxTextCtrl* itemTextCtrl15 = new wxTextCtrl( this, ID_DIALOG_CURRENCY_TEXT_UNIT, wxT(""));
    itemFlexGridSizer3->Add(itemTextCtrl15, flagsExpand);

    itemFlexGridSizer3->Add(new wxStaticText( this, wxID_STATIC, _("Cents Name")), flags);

    wxTextCtrl* itemTextCtrl17 = new wxTextCtrl( this, ID_DIALOG_CURRENCY_TEXT_CENTS, wxT(""));
    itemFlexGridSizer3->Add(itemTextCtrl17, flagsExpand);

    itemFlexGridSizer3->Add(new wxStaticText( this, wxID_STATIC, _("Prefix Symbol")), flags);

    wxTextCtrl* itemTextCtrl7 = new wxTextCtrl( this, ID_DIALOG_CURRENCY_TEXT_PFX, wxT(""));
    itemFlexGridSizer3->Add(itemTextCtrl7, flagsExpand);

    itemFlexGridSizer3->Add(new wxStaticText( this, wxID_STATIC, _("Suffix Symbol")), flags);

    wxTextCtrl* itemTextCtrl9 = new wxTextCtrl( this, ID_DIALOG_CURRENCY_TEXT_SFX, wxT(""));
    itemFlexGridSizer3->Add(itemTextCtrl9, flagsExpand);

    itemFlexGridSizer3->Add(new wxStaticText( this, wxID_STATIC, _("Decimal Char")), flags);

    wxTextCtrl* itemTextCtrl11 = new wxTextCtrl( this, ID_DIALOG_CURRENCY_TEXT_DECIMAL, wxT(""));
    itemFlexGridSizer3->Add(itemTextCtrl11, flagsExpand);

    itemFlexGridSizer3->Add(new wxStaticText( this, wxID_STATIC, _("Grouping Char")), flags);

    wxTextCtrl* itemTextCtrl13 = new wxTextCtrl( this, ID_DIALOG_CURRENCY_TEXT_GROUP, wxT(""));
    itemFlexGridSizer3->Add(itemTextCtrl13, flagsExpand);

    itemFlexGridSizer3->Add(new wxStaticText( this, wxID_STATIC, _("Scale")), flags);

    wxTextCtrl* itemTextCtrl19 = new wxTextCtrl( this, ID_DIALOG_CURRENCY_TEXT_SCALE, wxT(""), 
        wxDefaultPosition, wxDefaultSize, wxALIGN_RIGHT|wxTE_PROCESS_ENTER , doubleValidator() );
    itemFlexGridSizer3->Add(itemTextCtrl19, flagsExpand);
    
    itemFlexGridSizer3->Add(new wxStaticText( this, wxID_STATIC, _("Conversion to Base Rate")), flags);

    wxTextCtrl* itemTextCtrl82 = new wxTextCtrl( this, ID_DIALOG_CURRENCY_TEXT_BASECONVRATE, wxT(""),
        wxDefaultPosition, wxDefaultSize, wxALIGN_RIGHT|wxTE_PROCESS_ENTER , doubleValidator() );
    itemFlexGridSizer3->Add(itemTextCtrl82, flagsExpand);
    itemTextCtrl82->SetToolTip(_("Other currency conversion rate. Set Base Currency to 1."));
    //--------------------------
    wxStaticBox* itemStaticBox_02 = new wxStaticBox(this, wxID_ANY, _("Base Rate Conversion Sample:"));
    wxStaticBoxSizer* itemStaticBoxSizer_02 = new wxStaticBoxSizer(itemStaticBox_02, wxHORIZONTAL);
    itemBoxSizer2->Add(itemStaticBoxSizer_02, flagsExpand);

    baseRateSample_ = new wxStaticText( this, wxID_STATIC, wxT(""));
    itemStaticBoxSizer_02->Add(baseRateSample_, flags);

    //--------------------------
    wxStaticBox* itemStaticBox_01 = new wxStaticBox(this, wxID_STATIC, _("Value Display Sample:"));
    wxStaticBoxSizer* itemStaticBoxSizer_01 = new wxStaticBoxSizer(itemStaticBox_01, wxHORIZONTAL);
    itemBoxSizer2->Add(itemStaticBoxSizer_01, 0, wxGROW|wxALL, 5);

    sampleText_ = new wxStaticText( this, wxID_STATIC, wxT(""));
    itemStaticBoxSizer_01->Add(sampleText_, flags);
 
    //--------------------------
    wxBoxSizer* itemBoxSizer22 = new wxBoxSizer(wxHORIZONTAL);
    itemBoxSizer2->Add(itemBoxSizer22, 0, wxALIGN_CENTER_HORIZONTAL|wxALL, 5);

    wxButton* itemButton24 = new wxButton( this, ID_DIALOG_CURRENCY_BUTTON_UPDATE, _("&Update"));
    itemBoxSizer22->Add(itemButton24, 0, wxALIGN_CENTER_HORIZONTAL|wxALL, 5);
    itemButton24->SetToolTip(_("Save any changes made"));

    wxButton* itemButton25 = new wxButton( this, wxID_CANCEL, _("&Close"));
    itemBoxSizer22->Add(itemButton25, 0, wxALIGN_CENTER_HORIZONTAL|wxALL, 5);
    itemButton25->SetToolTip(_("Any changes will be lost without update"));
}

void mmCurrencyDialog::OnUpdate(wxCommandEvent& /*event*/)
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

    long scal = 0;
    scaleTx->GetValue().ToLong(&scal);
    if (scal <= 0) 
    {
        wxMessageDialog dlg(this, _("Scale should be greater than zero"), _("Error"), wxICON_ERROR);
        dlg.ShowModal();
        return;
    }

    double convRate = 1.0;
    baseConvRate->GetValue().ToDouble(&convRate);
    if (convRate < 0.0) 
    {
        wxMessageDialog dlg(this, _("Base Conversion Rate should be positive"), _("Error"), wxICON_ERROR);
        dlg.ShowModal();
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
    pCurrency->scaleDl_ = static_cast<int>(scal);
    pCurrency->baseConv_ = convRate;
    pCurrency->currencySymbol_ = currencySymbol->GetValue();
   
    core_->currencyList_.updateCurrency(pCurrency);

    fillControls();
}
