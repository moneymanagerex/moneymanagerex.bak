/*******************************************************
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

#include "assetdialog.h"
#include "util.h"
#include "dbwrapper.h"
#include "currencydialog.h"
#include "defs.h"
#include "paths.h"
#include "constants.h"

#include <wx/datectrl.h>

namespace
{

enum { DEF_CHANGE_NONE, DEF_CHANGE_APPRECIATE, DEF_CHANGE_DEPRECIATE };
enum { DEF_ASSET_PROPERTY, DEF_ASSET_AUTO, DEF_ASSET_HOUSE, DEF_ASSET_ART, DEF_ASSET_JEWELLERY, DEF_ASSET_CASH, DEF_ASSET_OTHER };

enum 
{ 
  IDC_COMBO_TYPE = wxID_HIGHEST + 1,
  IDC_NOTES,
};

} // namespace


IMPLEMENT_DYNAMIC_CLASS( mmAssetDialog, wxDialog )

BEGIN_EVENT_TABLE( mmAssetDialog, wxDialog )
    EVT_BUTTON(wxID_OK, mmAssetDialog::OnOk)
    EVT_BUTTON(wxID_CANCEL, mmAssetDialog::OnCancel)
    EVT_CHOICE(IDC_COMBO_TYPE, mmAssetDialog::OnChangeAppreciationType)
    EVT_CHILD_FOCUS(mmAssetDialog::changeFocus)
END_EVENT_TABLE()


mmAssetDialog::mmAssetDialog(wxWindow* parent, mmCoreDB* core, mmAssetHolder* asset_holder, bool edit) :
    core_(core),
    asset_holder_(asset_holder),
    m_edit(edit)
{
    long style = wxCAPTION | wxSYSTEM_MENU | wxCLOSE_BOX;

    if (Create(parent, wxID_ANY, _("New/Edit Asset"), wxDefaultPosition, wxSize(400, 300), style))
        core_->currencyList_.LoadBaseCurrencySettings();
}

bool mmAssetDialog::Create(wxWindow* parent, wxWindowID id, const wxString& caption, const wxPoint& pos, const wxSize& size, long style)
{
    SetExtraStyle(GetExtraStyle()|wxWS_EX_BLOCK_EVENTS);

    if (!wxDialog::Create(parent, id, caption, pos, size, style)) 
        return false;

    CreateControls();
    GetSizer()->Fit(this);
    GetSizer()->SetSizeHints(this);

    SetIcon(mmex::getProgramIcon());
    
    fillControls();

    if (m_edit)
        dataToControls();
    else   
        enableDisableRate(false);

    Centre();
    return true;
}

void mmAssetDialog::dataToControls()
{
    assetID_ = asset_holder_->id_;

    m_assetName->SetValue(asset_holder_->assetName_);
    m_notes->SetValue(asset_holder_->assetNotes_);

    wxDateTime dtdt = asset_holder_->assetDate_;
    wxString dt = mmGetDateForDisplay(core_->db_.get(), dtdt);
    m_dpc->SetValue(dtdt);

    wxString value;
    mmex::formatDoubleToCurrencyEdit(asset_holder_->value_, value);
    m_value->SetValue(value);

    wxString valueChangeRate;
    valueChangeRate.Printf(wxT("%.3f"), asset_holder_->valueChange_);
    m_valueChangeRate->SetValue(valueChangeRate);

    wxString valueChangeTypeStr = asset_holder_->sAssetValueChange_;
	m_valueChange->SetStringSelection(wxGetTranslation(valueChangeTypeStr));
	enableDisableRate(valueChangeTypeStr != wxT("None"));
    m_assetType->SetStringSelection(wxGetTranslation(asset_holder_->assetType_));
}

void mmAssetDialog::fillControls()
{
}

void mmAssetDialog::CreateControls()
{    
    wxSizerFlags flags, flagsExpand;
    flags.Align(wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL).Border(wxALL, 5);
    flagsExpand.Align(wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL).Border(wxALL, 5).Expand();

    wxBoxSizer* itemBoxSizer2 = new wxBoxSizer(wxHORIZONTAL);
    this->SetSizer(itemBoxSizer2);

    wxBoxSizer* itemBoxSizer3 = new wxBoxSizer(wxVERTICAL);
    itemBoxSizer2->Add(itemBoxSizer3, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

    wxStaticBox* itemStaticBoxSizer4Static = new wxStaticBox(this, wxID_ANY, _("Asset Details"));
    wxStaticBoxSizer* itemStaticBoxSizer4 = new wxStaticBoxSizer(itemStaticBoxSizer4Static, 
        wxVERTICAL);
    itemBoxSizer3->Add(itemStaticBoxSizer4, flags);

    wxPanel* itemPanel5 = new wxPanel( this, wxID_STATIC, wxDefaultPosition, 
        wxDefaultSize, wxTAB_TRAVERSAL );
    itemStaticBoxSizer4->Add(itemPanel5, flags);

    wxFlexGridSizer* itemFlexGridSizer6 = new wxFlexGridSizer(0, 2, 0, 0);
    itemPanel5->SetSizer(itemFlexGridSizer6);

    itemFlexGridSizer6->Add(new wxStaticText( itemPanel5, wxID_STATIC, _("Name")), flags);

    m_assetName = new wxTextCtrl( itemPanel5, wxID_ANY, wxGetEmptyString(), wxDefaultPosition, wxDefaultSize, 0 );
    m_assetName->SetToolTip(_("Enter the name of the asset"));
    itemFlexGridSizer6->Add(m_assetName, flagsExpand);

    itemFlexGridSizer6->Add(new wxStaticText( itemPanel5, wxID_STATIC, _("Date")), flags);
    
    m_dpc = new wxDatePickerCtrl( itemPanel5, wxID_ANY, wxDefaultDateTime, 
              wxDefaultPosition, wxSize(120, -1), wxDP_DROPDOWN|wxDP_SHOWCENTURY);    
    itemFlexGridSizer6->Add(m_dpc, flags);
    m_dpc->SetToolTip(_("Specify the date of purchase of asset"));

    itemFlexGridSizer6->Add(new wxStaticText( itemPanel5, wxID_STATIC, _("Asset Type")), flags);

    m_assetType = new wxChoice( itemPanel5, wxID_STATIC, wxDefaultPosition, wxSize(150,-1));
    size_t size = sizeof(ASSET_TYPE)/sizeof(wxString);
    for(size_t i = 0; i < size; ++i)
        m_assetType->Append(wxGetTranslation(ASSET_TYPE[i]),
            new wxStringClientData(ASSET_TYPE[i]));

    m_assetType->SetToolTip(_("Select type of asset"));
    m_assetType->SetSelection(DEF_ASSET_PROPERTY);
    itemFlexGridSizer6->Add(m_assetType, 0, 
        wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    itemFlexGridSizer6->Add(new wxStaticText( itemPanel5, wxID_STATIC, _("Value")), flags);

    m_value = new wxTextCtrl( itemPanel5, wxID_STATIC, wxGetEmptyString(), wxDefaultPosition, wxSize(150,-1), wxALIGN_RIGHT|wxTE_PROCESS_ENTER , doubleValidator() );
    m_value->SetToolTip(_("Enter the current value of the asset"));
    itemFlexGridSizer6->Add(m_value, flags);

    itemFlexGridSizer6->Add(new wxStaticText( itemPanel5, wxID_STATIC, _("Change in Value")), flags);

    m_valueChange = new wxChoice( itemPanel5, IDC_COMBO_TYPE, wxDefaultPosition, wxSize(150,-1));
    wxString value_change[] = {
        wxTRANSLATE("None"),
        wxTRANSLATE("Appreciates"),
        wxTRANSLATE("Depreciates")};
    size = sizeof(value_change)/sizeof(wxString);
    for(size_t i = 0; i < size; ++i)
    m_valueChange->Append(wxGetTranslation(value_change[i]),
        new wxStringClientData(value_change[i]));

    m_valueChange->SetToolTip(_("Specify if the value of the asset changes over time"));
    m_valueChange->SetSelection(DEF_CHANGE_NONE);
    itemFlexGridSizer6->Add(m_valueChange, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

    m_valueChangeRateLabel = new wxStaticText( itemPanel5, wxID_STATIC, _("% Rate"));
    itemFlexGridSizer6->Add(m_valueChangeRateLabel, flags);

    m_valueChangeRate = new wxTextCtrl( itemPanel5, wxID_STATIC, wxGetEmptyString(), wxDefaultPosition, wxSize(150,-1), wxALIGN_RIGHT|wxTE_PROCESS_ENTER , doubleValidator() );
    m_valueChangeRate->SetToolTip(_("Enter the rate at which the asset changes its value in % per year"));
    itemFlexGridSizer6->Add(m_valueChangeRate, flags);
    enableDisableRate(false);

    itemFlexGridSizer6->Add(new wxStaticText( itemPanel5, wxID_STATIC, _("Notes")), flags);

    itemFlexGridSizer6->AddSpacer(1);

    m_notes = new wxTextCtrl( this, IDC_NOTES, wxGetEmptyString(), wxDefaultPosition, wxSize(220, 170), wxTE_MULTILINE );
    m_notes->SetToolTip(_("Enter notes associated with this asset"));
    itemStaticBoxSizer4->Add(m_notes, 0, wxGROW|wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT|wxBOTTOM, 10);

    wxPanel* itemPanel27 = new wxPanel( this, wxID_STATIC, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
    itemBoxSizer3->Add(itemPanel27, flags.Right());

    wxBoxSizer* itemBoxSizer28 = new wxBoxSizer(wxHORIZONTAL);
    itemPanel27->SetSizer(itemBoxSizer28);

    wxButton* itemButton29 = new wxButton( itemPanel27, wxID_OK);
    itemBoxSizer28->Add(itemButton29, flags);

    wxButton* itemButton30 = new wxButton( itemPanel27, wxID_CANCEL);
    itemBoxSizer28->Add(itemButton30, flags);
    itemButton30->SetFocus();
}

void mmAssetDialog::OnChangeAppreciationType(wxCommandEvent& /*event*/)
{
    int selection = m_valueChange->GetSelection();
    // Disable for "None", Enable for "Appreciates" or "Depreciates"
    enableDisableRate(selection != DEF_CHANGE_NONE);
}

void mmAssetDialog::enableDisableRate(bool en)
{
    if (en)
    {
        m_valueChangeRate->SetEditable(true);
        m_valueChangeRate->Enable(true);
        m_valueChangeRateLabel->Enable(true);
    }
    else 
    {
        //m_valueChangeRate->SetValue(wxT("0"));
        m_valueChangeRate->SetEditable(false);
        m_valueChangeRate->Enable(false);
        m_valueChangeRateLabel->Enable(false);
    }
}

void mmAssetDialog::OnOk(wxCommandEvent& /*event*/)
{
    wxString pdate = m_dpc->GetValue().FormatISODate();
    wxString notes = m_notes->GetValue().Trim();
    wxString name = m_assetName->GetValue().Trim();

    wxString valueStr = m_value->GetValue().Trim();
    if (valueStr.IsEmpty())
    {
        mmShowErrorMessageInvalid(this, _("Value"));
        return;
    }
    double value = 0;
    if (!mmex::formatCurrencyToDouble(valueStr, value))
    {
        mmShowErrorMessage(this, _("Invalid Value "), _("Error"));
        return;
    }

    int valueChangeType = m_valueChange->GetSelection();
    wxString valueChangeTypeStr= wxT("");
    wxStringClientData* value_change_obj = (wxStringClientData *)m_valueChange->GetClientObject(m_valueChange->GetSelection());
    if (value_change_obj) valueChangeTypeStr = value_change_obj->GetData();

    wxString valueChangeRateStr = m_valueChangeRate->GetValue().Trim();
    if (valueChangeRateStr.IsEmpty() && valueChangeType != DEF_CHANGE_NONE)
    {
        mmShowErrorMessageInvalid(this, _("Rate of Change in Value"));
        return;
    }
    double valueChangeRate = 0;
    if(!valueChangeRateStr.ToDouble(&valueChangeRate)) {
        valueChangeRate = -1.0;
    }
    //This should be unnecessary with hidden controls
    if ((valueChangeType != DEF_CHANGE_NONE) && (valueChangeRate < 0.0))
    {
        mmShowErrorMessage(this, _("Invalid Value "), _("Error"));
        return;
    }

    wxString asset_type = wxT("");
    wxStringClientData* type_obj = (wxStringClientData *)m_assetType->GetClientObject(m_assetType->GetSelection());
    if (type_obj) asset_type = type_obj->GetData();

    wxSQLite3Statement st;
    if (m_edit)
        st = core_->db_.get()->PrepareStatement(UPDATE_ASSETS_V1);
    else
        st = core_->db_.get()->PrepareStatement(INSERT_INTO_ASSETS_V1);
        
    int i = 0;
    st.Bind(++i, pdate);
    st.Bind(++i, name);
    st.Bind(++i, value);
    st.Bind(++i, valueChangeTypeStr);
    st.Bind(++i, notes);
    st.Bind(++i, valueChangeRate);
    st.Bind(++i, asset_type);
    if (m_edit) st.Bind(++i, assetID_);

    wxASSERT(st.GetParamCount() == i);

    st.ExecuteUpdate();
    st.Finalize();

    mmOptions::instance().databaseUpdated_ = true;
    if (!m_edit)
        assetID_ = core_->db_.get()->GetLastRowId().ToLong();

    EndModal(wxID_OK);
}

void mmAssetDialog::OnCancel(wxCommandEvent& /*event*/)
{
    if (assetRichText)
        return;
    else 
        EndModal(wxID_CANCEL);
}

void mmAssetDialog::changeFocus(wxChildFocusEvent& event)
{
    wxWindow *w = event.GetWindow();
    if ( w ) 
        assetRichText = (w->GetId() == IDC_NOTES ? true : false);    
}
