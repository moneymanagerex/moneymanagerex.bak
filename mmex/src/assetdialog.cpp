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
#include "defs.h"
#include "paths.h"
#include "mmex_db_view.h"
#include <boost/foreach.hpp>
#include <wx/datectrl.h>
#define _MM_EX_ASSETDIALOG_CPP_REVISION_ID    "$Revision$"
enum { DEF_CHANGE_NONE = 0, DEF_CHANGE_APPRECIATE, DEF_CHANGE_DEPRECIATE };

enum
{
  IDC_COMBO_TYPE = wxID_HIGHEST + 1,
  IDC_NOTES,
};

IMPLEMENT_DYNAMIC_CLASS(mmAssetDialog, wxDialog)

BEGIN_EVENT_TABLE(mmAssetDialog, wxDialog)
    EVT_BUTTON(wxID_OK, mmAssetDialog::OnOk)
    EVT_BUTTON(wxID_CANCEL, mmAssetDialog::OnCancel)
    EVT_CHOICE(IDC_COMBO_TYPE, mmAssetDialog::OnChangeAppreciationType)
    EVT_DATE_CHANGED(wxID_ANY, mmAssetDialog::OnDateChanged)
END_EVENT_TABLE()


mmAssetDialog::mmAssetDialog(wxWindow* parent, wxSQLite3Database* db, int assetID, bool edit) :
    m_db(db),
    m_assetID(assetID),
    m_edit(edit)
{
    long style = wxCAPTION | wxSYSTEM_MENU | wxCLOSE_BOX;

    if (Create(parent, wxID_ANY, _("New/Edit Asset"), wxDefaultPosition, wxSize(400, 300), style))
        mmDBWrapper::loadBaseCurrencySettings(m_db);
}

bool mmAssetDialog::Create(wxWindow* parent,
    wxWindowID id, const wxString& caption, const wxPoint& pos, const wxSize& size, long style)
{
    SetExtraStyle(GetExtraStyle()|wxWS_EX_BLOCK_EVENTS);

    if (!wxDialog::Create(parent, id, caption, pos, size, style))
        return false;

    CreateControls();
    GetSizer()->Fit(this);
    GetSizer()->SetSizeHints(this);

    SetIcon(mmex::getProgramIcon());

    if (m_edit)
        dataToControls();
    else
        enableDisableRate(false);

    Centre();
    return true;
}

void mmAssetDialog::dataToControls()
{
    const DB_View_ASSETS_V1::Data *asset = ASSETS_V1.get(m_assetID, m_db);
    if (asset)
    {
        m_assetName->SetValue(asset->ASSETNAME);
        m_notes->SetValue(asset->NOTES);

        wxDateTime dtdt = mmGetStorageStringAsDate(asset->STARTDATE);
        wxString dt = mmGetDateForDisplay(m_db, dtdt);
        m_dpc_->SetValue(dtdt);

        wxString value;
        mmex::formatDoubleToCurrencyEdit(asset->VALUE, value);
        m_value->SetValue(value);

        wxString valueChangeRate;
        valueChangeRate.Printf("%.3f", asset->VALUECHANGERATE);
        m_valueChangeRate->SetValue(valueChangeRate);

        m_valueChange->SetStringSelection(wxGetTranslation(asset->VALUECHANGE));
        if (asset->VALUECHANGE == "None") enableDisableRate(false);

        m_assetType->SetStringSelection(wxGetTranslation(asset->ASSETTYPE));
    }
}

void mmAssetDialog::CreateControls()
{
    wxSizerFlags flags;
    flags.Align(wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL).Border(wxALL, 5);
    //flagsExpand.Align(wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL).Border(wxALL, 5).Expand();

    wxBoxSizer* itemBoxSizer2 = new wxBoxSizer(wxVERTICAL);
    this->SetSizer(itemBoxSizer2);

    wxBoxSizer* itemBoxSizer3 = new wxBoxSizer(wxVERTICAL);
    itemBoxSizer2->Add(itemBoxSizer3, flags);

    wxStaticBox* itemStaticBoxSizer4Static = new wxStaticBox(this, wxID_ANY, _("Asset Details"));
    wxStaticBoxSizer* itemStaticBoxSizer4 = new wxStaticBoxSizer(itemStaticBoxSizer4Static, wxVERTICAL);
    itemBoxSizer3->Add(itemStaticBoxSizer4, flags);

    wxFlexGridSizer* itemFlexGridSizer6 = new wxFlexGridSizer(0, 2, 0, 0);
    itemStaticBoxSizer4->Add(itemFlexGridSizer6, flags);

    wxStaticText* itemStaticText7 = new wxStaticText(this,
        wxID_STATIC, _("Name"));
    itemFlexGridSizer6->Add(itemStaticText7, flags);

    m_assetName = new wxTextCtrl(this, wxID_ANY, "", wxDefaultPosition, wxSize(220, -1));
    m_assetName->SetToolTip(_("Enter the name of the asset"));
    itemFlexGridSizer6->Add(m_assetName, flags);

    wxStaticText* itemStaticText9 = new wxStaticText(this,
        wxID_STATIC, _("Date"));
    itemFlexGridSizer6->Add(itemStaticText9, flags);

    m_dpc_ = new wxDatePickerCtrl(this,
        wxID_ANY, wxDefaultDateTime, wxDefaultPosition, wxSize(130, -1),
        wxDP_DROPDOWN|wxDP_SHOWCENTURY|wxDP_ALLOWNONE);
    itemFlexGridSizer6->Add(m_dpc_, flags);
    m_dpc_->SetToolTip(_("Specify the date of purchase of asset"));

    wxStaticText* itemStaticText15 = new wxStaticText(this,
        wxID_STATIC, _("Asset Type"));
    itemFlexGridSizer6->Add(itemStaticText15, flags);

    m_assetType = new wxChoice(this, wxID_ANY, wxDefaultPosition, wxSize(150, -1));
    wxString asset_types[] =
    {
        wxTRANSLATE("Property"),
        wxTRANSLATE("Automobile"),
        wxTRANSLATE("Household Object"),
        wxTRANSLATE("Art"),
        wxTRANSLATE("Jewellery"),
        wxTRANSLATE("Cash"),
        wxTRANSLATE("Other")
    };
    for(size_t i = 0; i < sizeof(asset_types)/sizeof(wxString); ++i)
        m_assetType->Append(wxGetTranslation(asset_types[i]), new wxStringClientData(asset_types[i]));

    m_assetType->SetToolTip(_("Select type of asset"));
    m_assetType->SetSelection(0);
    itemFlexGridSizer6->Add(m_assetType, flags);

    wxStaticText* itemStaticText31 = new wxStaticText(this,
        wxID_STATIC, _("Value"));
    itemFlexGridSizer6->Add(itemStaticText31,
        flags);

    m_value = new wxTextCtrl(this,
        wxID_ANY, wxGetEmptyString(), wxDefaultPosition, wxSize(150,-1),
        wxALIGN_RIGHT|wxTE_PROCESS_ENTER , wxFloatingPointValidator<float>(2));
    m_value->SetToolTip(_("Enter the current value of the asset"));
    itemFlexGridSizer6->Add(m_value, flags);

    wxStaticText* itemStaticText11 = new wxStaticText(this,
        wxID_STATIC, _("Change in Value"));
    itemFlexGridSizer6->Add(itemStaticText11,
        flags);

    m_valueChange = new wxChoice(this, IDC_COMBO_TYPE, wxDefaultPosition, wxSize(150,-1));
    wxString change_types[] =
    {
        wxTRANSLATE("None"),
        wxTRANSLATE("Appreciates"),
        wxTRANSLATE("Depreciates")
    };
    for(size_t i = 0; i < sizeof(change_types)/sizeof(wxString); ++i)
        m_valueChange->Append(wxGetTranslation(change_types[i]), new wxStringClientData(change_types[i]));

    m_valueChange->SetToolTip(_("Specify if the value of the asset changes over time"));
    m_valueChange->SetSelection(0);
    itemFlexGridSizer6->Add(m_valueChange, flags);

    m_valueChangeRateLabel = new wxStaticText(this,
        wxID_STATIC, _("% Rate"));
    itemFlexGridSizer6->Add(m_valueChangeRateLabel,
        flags);

    m_valueChangeRate = new wxTextCtrl(this,
        wxID_ANY, wxGetEmptyString(), wxDefaultPosition,
        wxSize(150,-1), wxALIGN_RIGHT|wxTE_PROCESS_ENTER , wxFloatingPointValidator<float>(2));
    m_valueChangeRate->SetToolTip(_("Enter the rate at which the asset changes its value in % per year"));
    itemFlexGridSizer6->Add(m_valueChangeRate, flags);
    enableDisableRate(false);

    wxStaticText* itemStaticText19 = new wxStaticText(this, wxID_STATIC, _("Notes"));
    itemFlexGridSizer6->Add(itemStaticText19, flags);

    m_notes = new wxTextCtrl(this, IDC_NOTES, wxGetEmptyString(), wxDefaultPosition, wxSize(220, 170), wxTE_MULTILINE);
    m_notes->SetToolTip(_("Enter notes associated with this asset"));
    itemFlexGridSizer6->Add(m_notes, flags);

    wxBoxSizer* itemBoxSizer28 = new wxBoxSizer(wxHORIZONTAL);
    itemBoxSizer2->Add(itemBoxSizer28, flags.Right());

    wxButton* button_OK = new wxButton(this, wxID_OK);
    itemBoxSizer28->Add(button_OK, flags);

    wxButton* button_CA = new wxButton(this, wxID_CANCEL);
    itemBoxSizer28->Add(button_CA, flags);
    button_CA->SetFocus();
}

void mmAssetDialog::OnChangeAppreciationType(wxCommandEvent& /*event*/)
{
    int selection = m_valueChange->GetSelection();
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
        m_valueChangeRate->SetEditable(false);
        m_valueChangeRate->Enable(false);
        m_valueChangeRateLabel->Enable(false);
    }
}

void mmAssetDialog::OnOk(wxCommandEvent& /*event*/)
{
    wxString pdate = wxDateTime::Now().FormatISODate();
    if (m_dpc_->GetValue().IsValid())
        wxString pdate = m_dpc_->GetValue().FormatISODate();
    else
    {
        mmShowErrorMessageInvalid(this, _("Date"));
        return;
    }
    wxString notes = m_notes->GetValue().Trim();
    wxString name = m_assetName->GetValue().Trim();

    wxString valueStr = m_value->GetValue().Trim();

    double value = 0;
    if (!mmex::formatCurrencyToDouble(valueStr, value))
    {
        mmShowErrorMessage(this, _("Invalid Value "), _("Error"));
        return;
    }

    int valueChangeType = m_valueChange->GetSelection();

    wxString valueChangeRateStr = m_valueChangeRate->GetValue().Trim();
    if (valueChangeRateStr.IsEmpty() && valueChangeType != DEF_CHANGE_NONE)
    {
        mmShowErrorMessageInvalid(this, _("Rate of Change in Value"));
        return;
    }
    double valueChangeRate = 0;
    if(valueChangeRateStr.ToDouble(&valueChangeRate) == false) {
        valueChangeRate = 0.0;
    }
    //This should be unnecessary with hidden controls
    if (valueChangeType != DEF_CHANGE_NONE && valueChangeRate < 0.0)
    {
        mmShowErrorMessage(this, _("Invalid Value "), _("Error"));
        return;
    }

    DB_View_ASSETS_V1::Data_Set all_assets = ASSETS_V1.all(m_db);
    BOOST_FOREACH(const DB_View_ASSETS_V1::Data &assets, all_assets)
    {
        wxString asset_name = assets.ASSETNAME.Upper();
        if (assets.ASSETNAME == m_assetName->GetValue()) {
            mmShowErrorMessage(this, _("Name already used"),  _("Error"));
            return;
        }
    }

    DB_View_ASSETS_V1::Data* asset = ASSETS_V1.get(m_assetID, m_db);
    if (! asset)
        asset = ASSETS_V1.create();

    asset->STARTDATE = pdate;
    asset->ASSETNAME = name;
    asset->VALUE = value;
    wxStringClientData* value_change_obj = (wxStringClientData *)m_valueChange->GetClientObject(m_valueChange->GetSelection());
    if (value_change_obj) asset->VALUECHANGE = value_change_obj->GetData();
    asset->NOTES = notes;
    asset->VALUECHANGERATE = valueChangeRate;
    wxStringClientData* asset_type_obj = (wxStringClientData *)m_assetType->GetClientObject(m_assetType->GetSelection());
    if (asset_type_obj) asset->ASSETTYPE = asset_type_obj->GetData();

    asset->save(m_db);

    mmOptions::instance().databaseUpdated_ = true;
    EndModal(wxID_OK);
}

void mmAssetDialog::OnDateChanged(wxDateEvent& event)
{
    if (!event.GetDate().IsValid())
        m_dpc_->SetValue(wxDateTime::Now());
    event.Skip();
}


void mmAssetDialog::OnCancel(wxCommandEvent& /*event*/)
{
    EndModal(wxID_CANCEL);
}
