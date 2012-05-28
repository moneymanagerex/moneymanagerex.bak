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

#include <wx/datectrl.h>
#define _MM_EX_ASSETDIALOG_CPP_REVISION_ID    "$Revision$"
enum { DEF_CHANGE_NONE = 0, DEF_CHANGE_APPRECIATE, DEF_CHANGE_DEPRECIATE };
enum { DEF_ASSET_PROPERTY = 0, DEF_ASSET_AUTO, DEF_ASSET_HOUSE, DEF_ASSET_ART, DEF_ASSET_JEWELLERY, DEF_ASSET_CASH, DEF_ASSET_OTHER };

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

bool mmAssetDialog::Create(wxWindow* parent, wxWindowID id, const wxString& caption, const wxPoint& pos, const wxSize& size, long style)
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
        m_dpc->SetValue(dtdt);

        wxString value;
        mmex::formatDoubleToCurrencyEdit(asset->VALUE, value);
        m_value->SetValue(value);

        wxString valueChangeRate;
        valueChangeRate.Printf(wxT("%.3f"), asset->VALUECHANGERATE);
        m_valueChangeRate->SetValue(valueChangeRate);

        m_valueChange->SetStringSelection(asset->VALUECHANGE);
        if (asset->VALUECHANGE == wxT("None")) enableDisableRate(false);

        m_assetType->SetStringSelection(asset->ASSETTYPE);
    }
}

void mmAssetDialog::CreateControls()
{    
    wxBoxSizer* itemBoxSizer2 = new wxBoxSizer(wxHORIZONTAL);
    this->SetSizer(itemBoxSizer2);

    wxBoxSizer* itemBoxSizer3 = new wxBoxSizer(wxVERTICAL);
    itemBoxSizer2->Add(itemBoxSizer3, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

    wxStaticBox* itemStaticBoxSizer4Static = new wxStaticBox(this, wxID_ANY, _("Asset Details"));
    wxStaticBoxSizer* itemStaticBoxSizer4 = new wxStaticBoxSizer(itemStaticBoxSizer4Static, wxVERTICAL);
    itemBoxSizer3->Add(itemStaticBoxSizer4, 0, wxALIGN_CENTER_HORIZONTAL|wxALL, 5);

    wxPanel* itemPanel5 = new wxPanel(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL);
    itemStaticBoxSizer4->Add(itemPanel5, 1, wxGROW|wxALL, 5);

    wxFlexGridSizer* itemFlexGridSizer6 = new wxFlexGridSizer(8, 2, 0, 0);
    itemPanel5->SetSizer(itemFlexGridSizer6);

    wxStaticText* itemStaticText7 = new wxStaticText(itemPanel5, wxID_STATIC, _("Name"), wxDefaultPosition, wxDefaultSize, 0);
    itemFlexGridSizer6->Add(itemStaticText7, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxALL|wxADJUST_MINSIZE, 5);

    m_assetName = new wxTextCtrl(itemPanel5, wxID_ANY, wxGetEmptyString(), wxDefaultPosition, wxDefaultSize, 0);
    m_assetName->SetToolTip(_("Enter the name of the asset"));
    itemFlexGridSizer6->Add(m_assetName, 0, wxGROW|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    wxStaticText* itemStaticText9 = new wxStaticText(itemPanel5, wxID_STATIC, _("Date"), wxDefaultPosition, wxDefaultSize, 0);
    itemFlexGridSizer6->Add(itemStaticText9, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxALL|wxADJUST_MINSIZE, 5);
    
    m_dpc = new wxDatePickerCtrl(itemPanel5, wxID_ANY, wxDefaultDateTime, wxDefaultPosition, wxSize(120, -1), wxDP_DROPDOWN|wxDP_SHOWCENTURY);    
    itemFlexGridSizer6->Add(m_dpc, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxALL, 5);
    m_dpc->SetToolTip(_("Specify the date of purchase of asset"));

    wxStaticText* itemStaticText15 = new wxStaticText(itemPanel5, wxID_STATIC, _("Asset Type"), wxDefaultPosition, wxDefaultSize, 0);
    itemFlexGridSizer6->Add(itemStaticText15, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxALL|wxADJUST_MINSIZE, 5);

    wxArrayString itemAssetTypeStrings;  
    itemAssetTypeStrings.Add(_("Property"));
    itemAssetTypeStrings.Add(_("Automobile"));
    itemAssetTypeStrings.Add(_("Household Object"));
    itemAssetTypeStrings.Add(_("Art"));
    itemAssetTypeStrings.Add(_("Jewellery"));
    itemAssetTypeStrings.Add(_("Cash"));
    itemAssetTypeStrings.Add(_("Other"));

    m_assetType = new wxChoice(itemPanel5, wxID_ANY, wxDefaultPosition, wxSize(150, -1), itemAssetTypeStrings);
    m_assetType->SetToolTip(_("Select type of asset"));
    m_assetType->SetSelection(DEF_ASSET_PROPERTY);
    itemFlexGridSizer6->Add(m_assetType, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    wxStaticText* itemStaticText31 = new wxStaticText(itemPanel5, wxID_STATIC, _("Value"), wxDefaultPosition, wxDefaultSize, 0);
    itemFlexGridSizer6->Add(itemStaticText31, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxALL|wxADJUST_MINSIZE, 5);

    m_value = new wxTextCtrl(itemPanel5, wxID_ANY, wxGetEmptyString(), wxDefaultPosition, wxSize(150,-1), wxALIGN_RIGHT|wxTE_PROCESS_ENTER , doubleValidator());
    m_value->SetToolTip(_("Enter the current value of the asset"));
    itemFlexGridSizer6->Add(m_value, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    wxStaticText* itemStaticText11 = new wxStaticText(itemPanel5, wxID_STATIC, _("Change in Value"), wxDefaultPosition, wxDefaultSize, 0);
    itemFlexGridSizer6->Add(itemStaticText11, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxALL|wxADJUST_MINSIZE, 5);

    wxArrayString itemTypeStrings; 
    itemTypeStrings.Add(_("None"));
    itemTypeStrings.Add(_("Appreciates"));
    itemTypeStrings.Add(_("Depreciates"));

    m_valueChange = new wxChoice(itemPanel5, IDC_COMBO_TYPE, wxDefaultPosition, wxSize(150,-1), itemTypeStrings);
    m_valueChange->SetToolTip(_("Specify if the value of the asset changes over time"));
    m_valueChange->SetSelection(DEF_CHANGE_NONE);
    itemFlexGridSizer6->Add(m_valueChange, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

    m_valueChangeRateLabel = new wxStaticText(itemPanel5, wxID_STATIC, _("% Rate"), wxDefaultPosition, wxDefaultSize, 0);
    itemFlexGridSizer6->Add(m_valueChangeRateLabel, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxALL|wxADJUST_MINSIZE, 5);

    m_valueChangeRate = new wxTextCtrl(itemPanel5, wxID_ANY, wxGetEmptyString(), wxDefaultPosition, wxSize(150,-1), wxALIGN_RIGHT|wxTE_PROCESS_ENTER , doubleValidator());
    m_valueChangeRate->SetToolTip(_("Enter the rate at which the asset changes its value in % per year"));
    itemFlexGridSizer6->Add(m_valueChangeRate, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxALL, 5);
    enableDisableRate(false);

    wxStaticText* itemStaticText19 = new wxStaticText(itemPanel5, wxID_STATIC, _("Notes"));
    itemFlexGridSizer6->Add(itemStaticText19, 0,wxALIGN_LEFT|wxALIGN_LEFT|wxALL|wxADJUST_MINSIZE, 5);

    m_notes = new wxTextCtrl(itemPanel5, IDC_NOTES, wxGetEmptyString(), wxDefaultPosition, wxSize(220, 170), wxTE_MULTILINE);
    m_notes->SetToolTip(_("Enter notes associated with this asset"));
    itemFlexGridSizer6->Add(m_notes, 0, wxGROW|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    wxPanel* itemPanel27 = new wxPanel(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL);
    itemBoxSizer3->Add(itemPanel27, 0, wxALIGN_RIGHT|wxALL, 5);

    wxBoxSizer* itemBoxSizer28 = new wxBoxSizer(wxHORIZONTAL);
    itemPanel27->SetSizer(itemBoxSizer28);

    wxButton* itemButton29 = new wxButton(itemPanel27, wxID_OK);
    itemBoxSizer28->Add(itemButton29, 0, wxALIGN_CENTER_VERTICAL|wxALL, 1);

    wxButton* itemButton30 = new wxButton(itemPanel27, wxID_CANCEL);
    itemBoxSizer28->Add(itemButton30, 0, wxALIGN_CENTER_VERTICAL|wxALL, 1);
    itemButton30->SetFocus();
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

    DB_View_ASSETS_V1::Data* asset = ASSETS_V1.get(m_assetID, m_db);
    if (! asset)
        asset = ASSETS_V1.create();

    asset->STARTDATE = pdate;
    asset->ASSETNAME = name;
    asset->VALUE = value;
    asset->VALUECHANGE = m_valueChange->GetStringSelection();
    asset->NOTES = notes;
    asset->VALUECHANGERATE = valueChangeRate;
    asset->ASSETTYPE = m_assetType->GetStringSelection();

    asset->save(m_db);

    mmOptions::instance().databaseUpdated_ = true;
    EndModal(wxID_OK);
}

void mmAssetDialog::OnCancel(wxCommandEvent& /*event*/)
{
    EndModal(wxID_CANCEL);
}
