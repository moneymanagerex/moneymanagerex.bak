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

#include <wx/datectrl.h>

namespace
{

enum { DEF_CHANGE_NONE, DEF_CHANGE_APPRECIATE, DEF_CHANGE_DEPRECIATE };
enum { DEF_ASSET_PROPERTY, DEF_ASSET_AUTO, DEF_ASSET_HOUSE, DEF_ASSET_ART, DEF_ASSET_JEWELLERY, DEF_ASSET_CASH, DEF_ASSET_OTHER };

enum { 
  IDC_COMBO_TYPE = wxID_HIGHEST + 1,
  IDC_BUTTON_OK,
  IDC_BUTTON_CANCEL
};

} // namespace


IMPLEMENT_DYNAMIC_CLASS( mmAssetDialog, wxDialog )

BEGIN_EVENT_TABLE( mmAssetDialog, wxDialog )
    EVT_BUTTON(IDC_BUTTON_OK, mmAssetDialog::OnOk)
    EVT_BUTTON(IDC_BUTTON_CANCEL, mmAssetDialog::OnCancel)
    EVT_CHOICE(IDC_COMBO_TYPE, mmAssetDialog::OnChangeAppreciationType)
END_EVENT_TABLE()


mmAssetDialog::mmAssetDialog(wxWindow* parent, wxSQLite3Database* db, int assetID, bool edit) :
	m_db(db),
	m_assetID(assetID),
	m_edit(edit)
{
	long style = wxCAPTION | wxSYSTEM_MENU | wxCLOSE_BOX;

	if (Create(parent, wxID_ANY, _("New/Edit Asset"), wxDefaultPosition, wxSize(400, 300), style)) {
		mmDBWrapper::loadBaseCurrencySettings(m_db);
	}
}

bool mmAssetDialog::Create(wxWindow* parent, wxWindowID id, const wxString& caption, const wxPoint& pos, const wxSize& size, long style)
{
    SetExtraStyle(GetExtraStyle()|wxWS_EX_BLOCK_EVENTS);

    if (!wxDialog::Create(parent, id, caption, pos, size, style)) {
    	return false;
    }

    CreateControls();
    GetSizer()->Fit(this);
    GetSizer()->SetSizeHints(this);

    SetIcon(mmex::getProgramIcon());
    
    fillControls();

    if (m_edit) {
        dataToControls();
    } else {   
	enableDisableRate(false);
    }


    Centre();
    return true;
}

void mmAssetDialog::dataToControls()
{
    static const char sql[] = 
    "select ASSETNAME, "
           "NOTES, "
           "STARTDATE, "
           "VALUE, "
           "VALUECHANGERATE, "
           "VALUECHANGE, "
           "ASSETTYPE "
    "from ASSETS_V1 "
    "where ASSETID = ?";

    wxSQLite3Statement st = m_db->PrepareStatement(sql);
    st.Bind(1, m_assetID);

    wxSQLite3ResultSet q1 = st.ExecuteQuery();
    if (q1.NextRow())
    {
        m_assetName->SetValue(q1.GetString(wxT("ASSETNAME")));
        m_notes->SetValue(q1.GetString(wxT("NOTES")));

        wxString dateString = q1.GetString(wxT("STARTDATE"));
        wxDateTime dtdt = mmGetStorageStringAsDate(dateString);
        wxString dt = mmGetDateForDisplay(m_db, dtdt);
        m_dpc->SetValue(dtdt);

		wxString value;
		mmex::formatDoubleToCurrencyEdit(q1.GetDouble(wxT("VALUE")), value);
		m_value->SetValue(value);

        wxString valueChangeRate;
        valueChangeRate.Printf(wxT("%.3f"), q1.GetDouble(wxT("VALUECHANGERATE")));
        m_valueChangeRate->SetValue(valueChangeRate);
    
        wxString valueChangeTypeStr = q1.GetString(wxT("VALUECHANGE"));
        if (valueChangeTypeStr == wxT("None"))
		{
            m_valueChange->SetSelection(DEF_CHANGE_NONE);
			enableDisableRate(false);
		}
        else if (valueChangeTypeStr == wxT("Appreciates"))
		{
			m_valueChange->SetSelection(DEF_CHANGE_APPRECIATE);
			enableDisableRate(true);
		}
        else if (valueChangeTypeStr == wxT("Depreciates"))

		{
            m_valueChange->SetSelection(DEF_CHANGE_DEPRECIATE);
			enableDisableRate(true);
		}
        else
		{
            wxASSERT(false);
		}
		
         wxString assetTypeStr = q1.GetString(wxT("ASSETTYPE"));
         if (assetTypeStr == wxT("Property"))
            m_assetType->SetSelection(DEF_ASSET_PROPERTY);
         else if (assetTypeStr == wxT("Automobile"))
            m_assetType->SetSelection(DEF_ASSET_AUTO);
         else if (assetTypeStr == wxT("Household Object"))
            m_assetType->SetSelection(DEF_ASSET_HOUSE);
         else if (assetTypeStr == wxT("Art"))
            m_assetType->SetSelection(DEF_ASSET_ART);
		 else if (assetTypeStr == wxT("Jewellery"))
			 m_assetType->SetSelection(DEF_ASSET_JEWELLERY);
		 else if (assetTypeStr == wxT("Cash"))
			 m_assetType->SetSelection(DEF_ASSET_CASH);
         else if (assetTypeStr == wxT("Other"))
            m_assetType->SetSelection(DEF_ASSET_OTHER);

    }
    st.Finalize();
}

void mmAssetDialog::fillControls()
{
    
}

void mmAssetDialog::CreateControls()
{    
    mmAssetDialog* itemDialog1 = this;

    wxBoxSizer* itemBoxSizer2 = new wxBoxSizer(wxHORIZONTAL);
    itemDialog1->SetSizer(itemBoxSizer2);

    wxBoxSizer* itemBoxSizer3 = new wxBoxSizer(wxVERTICAL);
    itemBoxSizer2->Add(itemBoxSizer3, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

    wxStaticBox* itemStaticBoxSizer4Static = new wxStaticBox(itemDialog1, wxID_ANY, _("Asset Details"));
    wxStaticBoxSizer* itemStaticBoxSizer4 = new wxStaticBoxSizer(itemStaticBoxSizer4Static, 
        wxVERTICAL);
    itemBoxSizer3->Add(itemStaticBoxSizer4, 0, wxALIGN_CENTER_HORIZONTAL|wxALL, 5);

    wxPanel* itemPanel5 = new wxPanel( itemDialog1, ID_PANEL3, wxDefaultPosition, 
        wxDefaultSize, wxTAB_TRAVERSAL );
    itemStaticBoxSizer4->Add(itemPanel5, 1, wxGROW|wxALL, 5);

    wxFlexGridSizer* itemFlexGridSizer6 = new wxFlexGridSizer(8, 2, 0, 0);
    itemPanel5->SetSizer(itemFlexGridSizer6);

    wxStaticText* itemStaticText7 = new wxStaticText( itemPanel5, wxID_STATIC, 
        _("Name"), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer6->Add(itemStaticText7, 0, 
        wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxALL|wxADJUST_MINSIZE, 5);

    m_assetName = new wxTextCtrl( itemPanel5, wxID_ANY, wxGetEmptyString(), wxDefaultPosition, wxDefaultSize, 0 );
    m_assetName->SetToolTip(_("Enter the name of the asset"));
    itemFlexGridSizer6->Add(m_assetName, 0, wxGROW|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    wxStaticText* itemStaticText9 = new wxStaticText( itemPanel5, wxID_STATIC, 
        _("Date"), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer6->Add(itemStaticText9, 0, 
        wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxALL|wxADJUST_MINSIZE, 5);
    
    m_dpc = new wxDatePickerCtrl( itemPanel5, wxID_ANY, wxDefaultDateTime, 
              wxDefaultPosition, wxSize(120, -1), wxDP_DROPDOWN|wxDP_SHOWCENTURY);    
    itemFlexGridSizer6->Add(m_dpc, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxALL, 5);
    m_dpc->SetToolTip(_("Specify the date of purchase of asset"));

    wxStaticText* itemStaticText15 = new wxStaticText( itemPanel5, wxID_STATIC,
        _("Asset Type"), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer6->Add(itemStaticText15, 0, 
        wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxALL|wxADJUST_MINSIZE, 5);

    const wxString itemAssetTypeStrings[] =  
    {
        _("Property"),
        _("Automobile"),
        _("Household Object"),
        _("Art"),
        _("Jewellery"),
		_("Cash"),
        _("Other"),
    };
    m_assetType = new wxChoice( itemPanel5, wxID_ANY, wxDefaultPosition, wxSize(150,-1), 7, itemAssetTypeStrings, 0 );
    m_assetType->SetToolTip(_("Select type of asset"));
    m_assetType->SetSelection(DEF_ASSET_PROPERTY);
    itemFlexGridSizer6->Add(m_assetType, 0, 
        wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxALL, 5);

     wxStaticText* itemStaticText31 = new wxStaticText( itemPanel5, 
        wxID_STATIC, _("Value"), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer6->Add(itemStaticText31, 0, 
        wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxALL|wxADJUST_MINSIZE, 5);

    m_value = new wxTextCtrl( itemPanel5, wxID_ANY, wxGetEmptyString(), wxDefaultPosition, wxSize(150,-1), wxALIGN_RIGHT );
    m_value->SetToolTip(_("Enter the current value of the asset"));
    itemFlexGridSizer6->Add(m_value, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxALL, 5);


    wxStaticText* itemStaticText11 = new wxStaticText( itemPanel5, wxID_STATIC, _("Change in Value"), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer6->Add(itemStaticText11, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxALL|wxADJUST_MINSIZE, 5);

    const wxString itemTypeStrings[] =  
    {
        _("None"),
        _("Appreciates"),
        _("Depreciates"),
    };

    m_valueChange = new wxChoice( itemPanel5, IDC_COMBO_TYPE, wxDefaultPosition, wxSize(150,-1), 3, itemTypeStrings, 0 );
    m_valueChange->SetToolTip(_("Specify if the value of the asset changes over time"));
    m_valueChange->SetSelection(DEF_CHANGE_NONE);
    itemFlexGridSizer6->Add(m_valueChange, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

    m_valueChangeRateLabel = new wxStaticText( itemPanel5, wxID_STATIC, _("% Rate"), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer6->Add(m_valueChangeRateLabel, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxALL|wxADJUST_MINSIZE, 5);

    m_valueChangeRate = new wxTextCtrl( itemPanel5, wxID_ANY, wxGetEmptyString(), wxDefaultPosition, wxSize(150,-1), wxALIGN_RIGHT );
    m_valueChangeRate->SetToolTip(_("Enter the rate at which the asset changes its value in % per year"));
    itemFlexGridSizer6->Add(m_valueChangeRate, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxALL, 5);
    enableDisableRate(false);

    //wxStaticText* itemStaticText25 = new wxStaticText( itemPanel5, wxID_STATIC, _("Value"), wxDefaultPosition, wxDefaultSize, 0 );
    //itemFlexGridSizer6->Add(itemStaticText25, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxALL|wxADJUST_MINSIZE, 5);

    //m_valueValue = new wxStaticText( itemPanel5, ID_STATIC_STOCK_VALUE, wxT("--"), wxDefaultPosition, wxDefaultSize, 0 );
    //itemFlexGridSizer6->Add(m_valueValue, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxALL|wxADJUST_MINSIZE, 5);

    wxStaticText* itemStaticText19 = new wxStaticText( itemPanel5, wxID_STATIC, _("Notes"));
    itemFlexGridSizer6->Add(itemStaticText19, 0,wxALIGN_LEFT|wxALIGN_LEFT|wxALL|wxADJUST_MINSIZE, 5);

    m_notes = new wxTextCtrl( itemPanel5, wxID_ANY, wxGetEmptyString(), wxDefaultPosition, wxSize(220, 170), wxTE_MULTILINE );
    m_notes->SetToolTip(_("Enter notes associated with this asset"));
    itemFlexGridSizer6->Add(m_notes, 0, wxGROW|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    wxPanel* itemPanel27 = new wxPanel( itemDialog1, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
    itemBoxSizer3->Add(itemPanel27, 0, wxALIGN_RIGHT|wxALL, 5);

    wxBoxSizer* itemBoxSizer28 = new wxBoxSizer(wxHORIZONTAL);
    itemPanel27->SetSizer(itemBoxSizer28);

    wxButton* itemButton29 = new wxButton( itemPanel27, IDC_BUTTON_OK, _("OK"), wxDefaultPosition, wxDefaultSize, 0 );
    itemButton29->SetForegroundColour(wxColour(wxT("FOREST GREEN")));
    itemBoxSizer28->Add(itemButton29, 0, wxALIGN_CENTER_VERTICAL|wxALL, 1);

    wxButton* itemButton30 = new wxButton( itemPanel27, IDC_BUTTON_CANCEL, _("Cancel"), wxDefaultPosition, wxDefaultSize, 0 );
    itemButton30->SetForegroundColour(wxColour(wxT("RED")));
    itemBoxSizer28->Add(itemButton30, 0, wxALIGN_CENTER_VERTICAL|wxALL, 1);
    itemButton30->SetFocus();

}

void mmAssetDialog::OnCancel(wxCommandEvent& /*event*/)
{
    Close(true);
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
	if (!mmex::formatCurrencyToDouble(valueStr, value) || (value < 0.0))
    {
        mmShowErrorMessage(this, _("Invalid Value "), _("Error"));
        return;
    }

    int valueChangeType = m_valueChange->GetSelection();
    wxString valueChangeTypeStr;
    if (valueChangeType == DEF_CHANGE_NONE)
        valueChangeTypeStr = wxT("None");
    else if (valueChangeType == DEF_CHANGE_APPRECIATE)
        valueChangeTypeStr = wxT("Appreciates");
    else if (valueChangeType == DEF_CHANGE_DEPRECIATE)
        valueChangeTypeStr = wxT("Depreciates");
    else
        wxASSERT(false);


    wxString valueChangeRateStr = m_valueChangeRate->GetValue().Trim();
    if (valueChangeRateStr.IsEmpty() && valueChangeType != DEF_CHANGE_NONE)
    {
        mmShowErrorMessageInvalid(this, _("Rate of Change in Value"));
        return;
    }
    double valueChangeRate = 0;
    if(valueChangeRateStr.ToDouble(&valueChangeRate) == false) {
        valueChangeRate = -1.0;
    }
	//This should be unnecessary with hidden controls
	if ((valueChangeType != DEF_CHANGE_NONE) && (valueChangeRate < 0.0))
    {
        mmShowErrorMessage(this, _("Invalid Value "), _("Error"));
        return;
    }

    int assetType = m_assetType->GetSelection();
    wxString assetTypeStr;
    if (assetType == DEF_ASSET_PROPERTY)
        assetTypeStr = wxT("Property");
    else if (assetType == DEF_ASSET_AUTO)
        assetTypeStr = wxT("Automobile");
    else if (assetType == DEF_ASSET_HOUSE)
        assetTypeStr = wxT("Household Object");
    else if (assetType == DEF_ASSET_ART)
        assetTypeStr = wxT("Art");
	else if (assetType == DEF_ASSET_JEWELLERY)
		assetTypeStr = wxT("Jewellery");
	else if (assetType == DEF_ASSET_CASH)
		assetTypeStr = wxT("Cash");
    else if (assetType == DEF_ASSET_OTHER)
        assetTypeStr = wxT("Other");
    else
        wxASSERT(false);

    if (!m_edit)
    {
        static const char sql[] = 
        "insert into ASSETS_V1 ("
          "STARTDATE, ASSETNAME, VALUE, VALUECHANGE, NOTES, VALUECHANGERATE, ASSETTYPE "
        ") values (?, ?, ?, ?, ?, ?, ?)";

        wxSQLite3Statement st = m_db->PrepareStatement(sql);
        
        int i = 0;
        st.Bind(++i, pdate);
        st.Bind(++i, name);
        st.Bind(++i, value);
        st.Bind(++i, valueChangeTypeStr);
        st.Bind(++i, notes);
        st.Bind(++i, valueChangeRate);
        st.Bind(++i, assetTypeStr);

        wxASSERT(st.GetParamCount() == i);

        st.ExecuteUpdate();
        st.Finalize();
    }
    else 
    {
        static const char sql[] = 
        "update ASSETS_V1 "
        "SET STARTDATE = ?, ASSETNAME = ?, "
            "VALUE = ?, VALUECHANGE = ?,"
            "NOTES = ?, VALUECHANGERATE = ?, "
            "ASSETTYPE = ? "
        "where ASSETID = ?";

        wxSQLite3Statement st = m_db->PrepareStatement(sql);

        int i = 0;
        st.Bind(++i, pdate);
        st.Bind(++i, name);
        st.Bind(++i, value);
        st.Bind(++i, valueChangeTypeStr);
        st.Bind(++i, notes);
        st.Bind(++i, valueChangeRate);
        st.Bind(++i, assetTypeStr);
        st.Bind(++i, m_assetID);

        wxASSERT(st.GetParamCount() == i);

        st.ExecuteUpdate();
        st.Finalize();
    }

    EndModal(wxID_OK);
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
