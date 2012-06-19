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
#include "dbwrapper.h"
#include "mmcoredb.h"
#include "util.h"
#include "defs.h"
#include "paths.h"

#include <wx/combobox.h>

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
        wxString name = core_->getCurrencyName(currencyID_);
        currencyChoice_->Append(name);
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
    boost::shared_ptr<mmCurrency > pCurrency = core_->getCurrencySharedPtr(currencyName);

    wxTextCtrl* pfxTx = (wxTextCtrl*)FindWindow(ID_DIALOG_CURRENCY_TEXT_PFX);
    wxTextCtrl* sfxTx = (wxTextCtrl*)FindWindow(ID_DIALOG_CURRENCY_TEXT_SFX);
    wxTextCtrl* decTx = (wxTextCtrl*)FindWindow(ID_DIALOG_CURRENCY_TEXT_DECIMAL);
    wxTextCtrl* grpTx = (wxTextCtrl*)FindWindow(ID_DIALOG_CURRENCY_TEXT_GROUP);
    wxTextCtrl* unitTx = (wxTextCtrl*)FindWindow(ID_DIALOG_CURRENCY_TEXT_UNIT);
    wxTextCtrl* centTx = (wxTextCtrl*)FindWindow(ID_DIALOG_CURRENCY_TEXT_CENTS);
    wxTextCtrl* scaleTx = (wxTextCtrl*)FindWindow(ID_DIALOG_CURRENCY_TEXT_SCALE);
    wxStaticText* sample = (wxStaticText*)FindWindow(ID_DIALOG_CURRENCY_STATIC_SAMPLE);
    wxStaticText* baseRateSample = (wxStaticText*)FindWindow(ID_DIALOG_CURRENCY_STATIC_CONVERSION);
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
    
    mmDBWrapper::loadBaseCurrencySettings(core_->db_.get());
    mmex::formatDoubleToCurrency(amount, dispAmount);
    dispAmount2 = wxString() << dispAmount << (" ") << _("Converted to:") << (" ");
        
    mmDBWrapper::loadSettings(core_->db_.get(), pCurrency->currencyID_);
    if (pCurrency->baseConv_ != 0.0 )
        amount = amount / pCurrency->baseConv_;
    else
        amount = 0.0;
    mmex::formatDoubleToCurrency(amount, dispAmount);
    baseRateSample->SetLabel(dispAmount2 + dispAmount);

    amount = 123456.78;
    mmex::formatDoubleToCurrency(amount, dispAmount);
    dispAmount = wxString() << ("123456.78 ") << _("Shown As: ") << dispAmount;
    sample->SetLabel(dispAmount);

    // resize the dialog window
    Fit();    
}

void mmCurrencyDialog::OnCurrencyTypeChanged(wxCommandEvent& /*event*/)
{
    updateControls();
}

void mmCurrencyDialog::CreateControls()
{    
    wxSizerFlags flags, flagsExpand;
    flags.Align(wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL).Border(wxTOP|wxLEFT, 0);
    flagsExpand.Align(wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL).Border(wxALL, 0).Expand();

    wxBoxSizer* itemBoxSizer2 = new wxBoxSizer(wxVERTICAL);
    this->SetSizer(itemBoxSizer2);

    wxFlexGridSizer* itemFlexGridSizer3 = new wxFlexGridSizer(0, 2, 10, 10);
    itemBoxSizer2->Add(itemFlexGridSizer3, 0, wxALL, 10);

    //--------------------------
    wxStaticText* itemStaticText4 = new wxStaticText( this, wxID_STATIC, _("Currency Name"));
    itemFlexGridSizer3->Add(itemStaticText4, flags);

    wxString* itemChoice5Strings = NULL;
    currencyChoice_ = new wxChoice( this, ID_DIALOG_CURRENCY_CHOICE, 
        wxDefaultPosition, wxDefaultSize, 0, itemChoice5Strings, 0 );
    itemFlexGridSizer3->Add(currencyChoice_, flags);
    currencyChoice_->Enable(false);
    
     wxStaticText* itemStaticText26 = new wxStaticText( this, wxID_STATIC, _("Currency Symbol"));
    itemFlexGridSizer3->Add(itemStaticText26, flags);

    wxComboBox* symbols_box = new wxComboBox( this, ID_DIALOG_CURRENCY_TEXT_SYMBOL);
    symbols_box->Append("USD");
    symbols_box->Append("EUR");
    symbols_box->Append("AUD");
    symbols_box->Append("BGN");
    symbols_box->Append("BRL");
    symbols_box->Append("CAD");
    symbols_box->Append("CHF");
    symbols_box->Append("CNY");
    symbols_box->Append("CZK");
    symbols_box->Append("DKK");
    symbols_box->Append("EEK");
    symbols_box->Append("GBP");
    symbols_box->Append("HKD");
    symbols_box->Append("HRK");
    symbols_box->Append("HUF");
    symbols_box->Append("IDR");
    symbols_box->Append("ISK");
    symbols_box->Append("JPY");
    symbols_box->Append("KRW");
    symbols_box->Append("LTL");
    symbols_box->Append("LVL");
    symbols_box->Append("MXN");
    symbols_box->Append("MYR");
    symbols_box->Append("NOK");
    symbols_box->Append("NZD");
    symbols_box->Append("PHP");
    symbols_box->Append("PLN");
    symbols_box->Append("RON");
    symbols_box->Append("RUB");
    symbols_box->Append("SEK");
    symbols_box->Append("SGD");
    symbols_box->Append("SKK");
    symbols_box->Append("THB");
    symbols_box->Append("TRY");
    symbols_box->Append("UAH");
    symbols_box->Append("ZAR");

    itemFlexGridSizer3->Add(symbols_box, flagsExpand);

    wxStaticText* unit_name = new wxStaticText( this, wxID_STATIC, _("Unit Name"));
    itemFlexGridSizer3->Add(unit_name, flags);

    wxTextCtrl* unit_name_ctrl = new wxTextCtrl( this, ID_DIALOG_CURRENCY_TEXT_UNIT, "");
    itemFlexGridSizer3->Add(unit_name_ctrl, flagsExpand);

    wxStaticText* cent_name = new wxStaticText( this, wxID_STATIC, _("Cents Name"));
    itemFlexGridSizer3->Add(cent_name, flags);

    wxTextCtrl* cent_name_ctrl = new wxTextCtrl( this, ID_DIALOG_CURRENCY_TEXT_CENTS, (""));
    itemFlexGridSizer3->Add(cent_name_ctrl, flagsExpand);

    wxStaticText* prefix_symbol = new wxStaticText( this, wxID_STATIC, _("Prefix Symbol"));
    itemFlexGridSizer3->Add(prefix_symbol, flags);

    wxTextCtrl* prefix_symbol_ctrl = new wxTextCtrl( this, ID_DIALOG_CURRENCY_TEXT_PFX, (""));
    itemFlexGridSizer3->Add(prefix_symbol_ctrl, flagsExpand);

    wxStaticText* itemStaticText8 = new wxStaticText( this, wxID_STATIC, _("Suffix Symbol"));
    itemFlexGridSizer3->Add(itemStaticText8, flags);

    wxTextCtrl* itemTextCtrl9 = new wxTextCtrl( this, ID_DIALOG_CURRENCY_TEXT_SFX, (""));
    itemFlexGridSizer3->Add(itemTextCtrl9, flagsExpand);

    wxStaticText* itemStaticText10 = new wxStaticText( this, wxID_STATIC, _("Decimal Char"));
    itemFlexGridSizer3->Add(itemStaticText10, flags);

    wxTextCtrl* itemTextCtrl11 = new wxTextCtrl( this, ID_DIALOG_CURRENCY_TEXT_DECIMAL, (""));
    itemFlexGridSizer3->Add(itemTextCtrl11, flagsExpand);

    wxStaticText* itemStaticText12 = new wxStaticText( this, wxID_STATIC, _("Grouping Char"));
    itemFlexGridSizer3->Add(itemStaticText12, flags);

    wxTextCtrl* itemTextCtrl13 = new wxTextCtrl( this, ID_DIALOG_CURRENCY_TEXT_GROUP, (""));
    itemFlexGridSizer3->Add(itemTextCtrl13, flagsExpand);

    wxStaticText* itemStaticText18 = new wxStaticText( this, wxID_STATIC, _("Scale"));
    itemFlexGridSizer3->Add(itemStaticText18, flags);

    wxTextCtrl* itemTextCtrl19 = new wxTextCtrl( this, ID_DIALOG_CURRENCY_TEXT_SCALE, (""), 
        wxDefaultPosition, wxDefaultSize, wxALIGN_RIGHT|wxTE_PROCESS_ENTER , wxFloatingPointValidator<float>(2) );
    itemFlexGridSizer3->Add(itemTextCtrl19, flagsExpand);
    
    wxStaticText* itemStaticText81 = new wxStaticText( this, wxID_STATIC, _("Conversion to Base Rate"));
    itemFlexGridSizer3->Add(itemStaticText81, flags);

    wxTextCtrl* itemTextCtrl82 = new wxTextCtrl( this, ID_DIALOG_CURRENCY_TEXT_BASECONVRATE, (""),
        wxDefaultPosition, wxDefaultSize, wxALIGN_RIGHT|wxTE_PROCESS_ENTER , wxFloatingPointValidator<float>(2) );
    itemFlexGridSizer3->Add(itemTextCtrl82, flagsExpand);
    itemTextCtrl82->SetToolTip(_("Other currency conversion rate. Set Base Currency to 1."));
    //--------------------------
    wxStaticBox* itemStaticBox_02 = new wxStaticBox(this, wxID_ANY, _("Base Rate Conversion Sample:"));
    wxStaticBoxSizer* itemStaticBoxSizer_02 = new wxStaticBoxSizer(itemStaticBox_02, wxHORIZONTAL);
    itemBoxSizer2->Add(itemStaticBoxSizer_02, flagsExpand.Border(wxALL, 5));

    wxStaticText*  sampleText2_ = new wxStaticText( this, ID_DIALOG_CURRENCY_STATIC_CONVERSION, (""));
    itemStaticBoxSizer_02->Add(sampleText2_, flags.Border(wxALL, 5));

    //--------------------------
    wxStaticBox* itemStaticBox_01 = new wxStaticBox(this, wxID_ANY, _("Value Display Sample:"));
    wxStaticBoxSizer* itemStaticBoxSizer_01 = new wxStaticBoxSizer(itemStaticBox_01, wxHORIZONTAL);
    itemBoxSizer2->Add(itemStaticBoxSizer_01, flagsExpand);

    wxStaticText*  sampleText_ = new wxStaticText( this, ID_DIALOG_CURRENCY_STATIC_SAMPLE, "");
    itemStaticBoxSizer_01->Add(sampleText_, flags);
 
    //--------------------------
    wxBoxSizer* itemBoxSizer22 = new wxBoxSizer(wxHORIZONTAL);
    itemBoxSizer2->Add(itemBoxSizer22, flags.Center().Border(0));

    wxButton* itemButton24 = new wxButton( this, ID_DIALOG_CURRENCY_BUTTON_UPDATE, _("&Update"));
    itemBoxSizer22->Add(itemButton24, flags.Border(wxALL, 5));
    itemButton24->SetToolTip(_("Save any changes made"));

    wxButton* itemButton25 = new wxButton( this, wxID_CANCEL, _("&Close"));
    itemBoxSizer22->Add(itemButton25, flags);
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

    boost::shared_ptr<mmCurrency> pCurrency = core_->getCurrencySharedPtr(currencyName);
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
   
    core_->updateCurrency(pCurrency);

    fillControls();
}
