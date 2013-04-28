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
#include "constants.h"
#include "util.h"
#include "defs.h"
#include "paths.h"

#include <wx/combobox.h>
#include <wx/valnum.h>

#ifndef __VISUALC__
#define INT_PTR intptr_t
#endif

IMPLEMENT_DYNAMIC_CLASS( mmCurrencyDialog, wxDialog )

BEGIN_EVENT_TABLE( mmCurrencyDialog, wxDialog )
    EVT_BUTTON(ID_DIALOG_CURRENCY_BUTTON_UPDATE, mmCurrencyDialog::OnUpdate)
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
        currencyNameCombo_->Append(name, (void*)(INT_PTR)currencyID_);
        currencyNameCombo_->SetStringSelection(name);
    }
    else
    {
       wxASSERT(false);
       currencyNameCombo_->SetSelection(0);
    }

    updateControls();
}

void mmCurrencyDialog::updateControls()
{
    wxString currencyName = currencyNameCombo_->GetStringSelection();
    wxSharedPtr<mmCurrency > pCurrency = core_->currencyList_.getCurrencySharedPtr(currencyName);

    pfxTx_->SetValue(pCurrency->pfxSymbol_);
    sfxTx_->SetValue(pCurrency->sfxSymbol_);
    decTx_->SetValue(pCurrency->dec_);
    grpTx_->SetValue(pCurrency->grp_);
    unitTx_->SetValue(pCurrency->unit_);
    centTx_->SetValue(pCurrency->cent_);
    scaleTx_->SetValue(wxString() << pCurrency->scaleDl_);
    baseConvRate_->SetValue(wxString() << pCurrency->baseConv_);
    currencySymbolCombo_->SetValue(pCurrency->currencySymbol_);

    wxString dispAmount;
    wxString dispAmount2;
    double amount = 1000;

    core_->currencyList_.LoadBaseCurrencySettings();
    mmex::formatDoubleToCurrency(amount, dispAmount);
    dispAmount2 = wxString() << dispAmount << " " << _("Converted to:") << " ";

    mmDBWrapper::loadCurrencySettings(core_->db_.get(), pCurrency->currencyID_);
    if (pCurrency->baseConv_ != 0.0 )
        amount = amount / pCurrency->baseConv_;
    else
        amount = 0.0;
    mmex::formatDoubleToCurrency(amount, dispAmount);
    baseRateSample_->SetLabel(dispAmount2 + dispAmount);

    amount = 123456.78;
    mmex::formatDoubleToCurrency(amount, dispAmount);
    dispAmount = wxString() << "123456.78 " << _("Shown As: ") << dispAmount;
    sampleText_->SetLabel(dispAmount);

    // resize the dialog window
    Fit();
}

void mmCurrencyDialog::CreateControls()
{
    wxSizerFlags flags, flagsExpand;
    flags.Align(wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL).Border(wxALL, 5);
    flagsExpand.Align(wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL).Border(wxALL, 5).Expand();
    const wxSize size = wxSize(220, -1);

    wxBoxSizer* itemBoxSizer2 = new wxBoxSizer(wxVERTICAL);
    this->SetSizer(itemBoxSizer2);

    wxFlexGridSizer* itemFlexGridSizer3 = new wxFlexGridSizer(0, 2, 0, 0);
    itemFlexGridSizer3->AddGrowableCol(1);
    itemBoxSizer2->Add(itemFlexGridSizer3, flags);

    //--------------------------

    for(size_t i = 0; i < sizeof(CURRENCIES)/sizeof(wxString); ++i)
    {
       currency_symbols_.Add(CURRENCIES[i]);
       currency_names_.Add(CURRENCIES[++i]);
    }

    itemFlexGridSizer3->Add(new wxStaticText( this, wxID_STATIC, _("Currency Name")), flags);

    currencyNameCombo_ = new wxComboBox( this, ID_DIALOG_CURRENCY_CHOICE, "",
        wxDefaultPosition, size, currency_names_);
    itemFlexGridSizer3->Add(currencyNameCombo_, flags);

    currencyNameCombo_->Connect(ID_DIALOG_CURRENCY_CHOICE, wxEVT_COMMAND_COMBOBOX_SELECTED,
        wxCommandEventHandler(mmCurrencyDialog::OnCurrencyNameSelected), NULL, this);

    itemFlexGridSizer3->Add(new wxStaticText( this, wxID_STATIC, _("Currency Symbol")), flags);

    currencySymbolCombo_ = new wxComboBox( this, wxID_ANY, "",
        wxDefaultPosition, wxDefaultSize, currency_symbols_);
    itemFlexGridSizer3->Add(currencySymbolCombo_, flagsExpand);

    currencyNameCombo_->AutoComplete(currency_symbols_);

    itemFlexGridSizer3->Add(new wxStaticText( this, wxID_STATIC, _("Unit Name")), flags);
    unitTx_ = new wxTextCtrl( this, ID_DIALOG_CURRENCY_TEXT_UNIT, "");
    itemFlexGridSizer3->Add(unitTx_, flagsExpand);

    itemFlexGridSizer3->Add(new wxStaticText( this, wxID_STATIC, _("Cents Name")), flags);
    centTx_ = new wxTextCtrl( this, ID_DIALOG_CURRENCY_TEXT_CENTS, "");
    itemFlexGridSizer3->Add(centTx_, flagsExpand);

    itemFlexGridSizer3->Add(new wxStaticText( this, wxID_STATIC, _("Prefix Symbol")), flags);
    pfxTx_ = new wxTextCtrl( this, ID_DIALOG_CURRENCY_TEXT_PFX, "");
    itemFlexGridSizer3->Add(pfxTx_, flagsExpand);

    itemFlexGridSizer3->Add(new wxStaticText( this, wxID_STATIC, _("Suffix Symbol")), flags);
    sfxTx_ = new wxTextCtrl( this, ID_DIALOG_CURRENCY_TEXT_SFX, "");
    itemFlexGridSizer3->Add(sfxTx_, flagsExpand);

    itemFlexGridSizer3->Add(new wxStaticText( this, wxID_STATIC, _("Decimal Char")), flags);
    decTx_ = new wxTextCtrl( this, ID_DIALOG_CURRENCY_TEXT_DECIMAL, "");
    itemFlexGridSizer3->Add(decTx_, flagsExpand);

    itemFlexGridSizer3->Add(new wxStaticText( this, wxID_STATIC, _("Grouping Char")), flags);
    grpTx_ = new wxTextCtrl( this, ID_DIALOG_CURRENCY_TEXT_GROUP, "");
    itemFlexGridSizer3->Add(grpTx_, flagsExpand);

    itemFlexGridSizer3->Add(new wxStaticText( this, wxID_STATIC, _("Scale")), flags);
    scaleTx_ = new wxTextCtrl( this, ID_DIALOG_CURRENCY_TEXT_SCALE, "",
        wxDefaultPosition, wxDefaultSize, wxALIGN_RIGHT|wxTE_PROCESS_ENTER , wxFloatingPointValidator<double>() );
    itemFlexGridSizer3->Add(scaleTx_, flagsExpand);

    itemFlexGridSizer3->Add(new wxStaticText( this, wxID_STATIC, _("Conversion to Base Rate")), flags);
    baseConvRate_ = new wxTextCtrl( this, ID_DIALOG_CURRENCY_TEXT_BASECONVRATE, "",
        wxDefaultPosition, wxDefaultSize, wxALIGN_RIGHT|wxTE_PROCESS_ENTER , wxFloatingPointValidator<double>() );
    itemFlexGridSizer3->Add(baseConvRate_, flagsExpand);
    baseConvRate_ ->SetToolTip(_("Other currency conversion rate. Set Base Currency to 1."));

    //--------------------------
    wxStaticBox* itemStaticBox_02 = new wxStaticBox(this, wxID_ANY, _("Base Rate Conversion Sample:"));
    wxStaticBoxSizer* itemStaticBoxSizer_02 = new wxStaticBoxSizer(itemStaticBox_02, wxHORIZONTAL);
    itemBoxSizer2->Add(itemStaticBoxSizer_02, flagsExpand);

    baseRateSample_ = new wxStaticText( this, wxID_STATIC, "");
    itemStaticBoxSizer_02->Add(baseRateSample_, flags);

    //--------------------------
    wxStaticBox* itemStaticBox_01 = new wxStaticBox(this, wxID_STATIC, _("Value Display Sample:"));
    wxStaticBoxSizer* itemStaticBoxSizer_01 = new wxStaticBoxSizer(itemStaticBox_01, wxHORIZONTAL);
    itemBoxSizer2->Add(itemStaticBoxSizer_01, flagsExpand);

    sampleText_ = new wxStaticText( this, wxID_STATIC, "");
    itemStaticBoxSizer_01->Add(sampleText_, flags);

    //--------------------------
    wxBoxSizer* itemBoxSizer22 = new wxBoxSizer(wxHORIZONTAL);
    itemBoxSizer2->Add(itemBoxSizer22, flags.Centre());

    wxButton* itemButton24 = new wxButton( this, ID_DIALOG_CURRENCY_BUTTON_UPDATE, _("&Update"));
    itemBoxSizer22->Add(itemButton24, flags);
    itemButton24->SetToolTip(_("Save any changes made"));

    wxButton* itemButton25 = new wxButton( this, wxID_CANCEL, _("&Close"));
    itemBoxSizer22->Add(itemButton25, flags);
    itemButton25->SetToolTip(_("Any changes will be lost without update"));
}

void mmCurrencyDialog::OnUpdate(wxCommandEvent& /*event*/)
{
    wxString currencyName = currencyNameCombo_->GetValue();

    long scal = 0;
    scaleTx_->GetValue().ToLong(&scal);
    if (scal <= 0)
    {
        wxMessageDialog dlg(this, _("Scale should be greater than zero"), _("Error"), wxOK|wxICON_ERROR);
        dlg.ShowModal();
        return;
    }

    double convRate = 1.0;
    baseConvRate_->GetValue().ToDouble(&convRate);
    if (convRate < 0.0)
    {
        wxMessageDialog dlg(this, _("Base Conversion Rate should be positive"), _("Error"), wxOK|wxICON_ERROR);
        dlg.ShowModal();
        return;
    }

    wxSharedPtr<mmCurrency> pCurrency = core_->currencyList_.getCurrencySharedPtr(currencyID_);
    //wxASSERT(pCurrency->currencyID_ == currencyID_);

    pCurrency->pfxSymbol_ = pfxTx_->GetValue();
    pCurrency->sfxSymbol_ = sfxTx_->GetValue();
    pCurrency->dec_ = decTx_->GetValue();
    pCurrency->grp_ =  grpTx_->GetValue();
    pCurrency->unit_ = unitTx_->GetValue();
    pCurrency->cent_ = centTx_->GetValue();
    pCurrency->scaleDl_ = static_cast<int>(scal);
    pCurrency->baseConv_ = convRate;
    pCurrency->currencySymbol_ = currencySymbolCombo_->GetValue();
    pCurrency->currencyName_ = currencyName;

    core_->currencyList_.UpdateCurrency(pCurrency);

    fillControls();
}

void mmCurrencyDialog::OnCurrencyNameSelected(wxCommandEvent& /*event*/)
{
    wxString currency_name = currencyNameCombo_->GetValue();
    int index = currency_names_.Index(currency_name);
    wxString currency_symbol;
    if (index != wxNOT_FOUND)
    {
        currency_symbol = currency_symbols_[index];
        currencySymbolCombo_->SetValue(currency_symbol);
    }
}
