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
 /*******************************************************/

#include "assetdialog.h"
#include "wx/datectrl.h"
#include "util.h"
#include "dbwrapper.h"
#include "currencydialog.h"

#define DEF_CHANGE_NONE 0
#define DEF_CHANGE_APPRECIATE 1
#define DEF_CHANGE_DEPRECIATE 2

#define DEF_ASSET_PROPERTY 0
#define DEF_ASSET_AUTO 1
#define DEF_ASSET_HOUSE 2
#define DEF_ASSET_ART 3
#define DEF_ASSET_JEWELLERY 4
#define DEF_ASSET_CASH 5
#define DEF_ASSET_OTHER 6
 
IMPLEMENT_DYNAMIC_CLASS( mmAssetDialog, wxDialog )

BEGIN_EVENT_TABLE( mmAssetDialog, wxDialog )
    EVT_BUTTON(ID_BUTTON_ASSET_OK, mmAssetDialog::OnOk)
    EVT_BUTTON(ID_BUTTON_ASSET_CANCEL, mmAssetDialog::OnCancel)
END_EVENT_TABLE()

mmAssetDialog::mmAssetDialog( )
{
}

mmAssetDialog::mmAssetDialog(wxSQLite3Database* db, int assetID, bool edit,
                             wxWindow* parent, wxWindowID id, 
                             const wxString& caption, const wxPoint& pos, 
                             const wxSize& size, long style )
{
    db_ = db;
    assetID_ = assetID;
    edit_ = edit;
    Create(parent, id, caption, pos, size, style);
    mmDBWrapper::loadBaseCurrencySettings(db_);
}

bool mmAssetDialog::Create( wxWindow* parent, wxWindowID id, const wxString& caption, 
                           const wxPoint& pos, const wxSize& size, long style )
{
    SetExtraStyle(GetExtraStyle()|wxWS_EX_BLOCK_EVENTS);
    wxDialog::Create( parent, id, caption, pos, size, style );

    CreateControls();
    GetSizer()->Fit(this);
    GetSizer()->SetSizeHints(this);

    wxIcon icon(wxT("mmex.ico"), wxBITMAP_TYPE_ICO, 32, 32);
    SetIcon(icon);
    
    fillControls();

    if (edit_)
    {
        dataToControls();
    }

    Centre();
    return TRUE;
}

void mmAssetDialog::dataToControls()
{
    mmBEGINSQL_LITE_EXCEPTION;

    wxSQLite3StatementBuffer bufSQL;
    bufSQL.Format("select * from ASSETS_V1 where ASSETID=%d;", assetID_);
    wxSQLite3ResultSet q1 = db_->ExecuteQuery(bufSQL);
    if (q1.NextRow())
    {
        assetName_->SetValue(q1.GetString(wxT("ASSETNAME")));
        notes_->SetValue(q1.GetString(wxT("NOTES")));

        wxString dateString = q1.GetString(wxT("STARTDATE"));
        wxDateTime dtdt = mmGetStorageStringAsDate(dateString);
        wxString dt = mmGetDateForDisplay(db_, dtdt);
        dpc_->SetValue(dtdt);

		wxString value;
		mmCurrencyFormatter::formatDoubleToCurrencyEdit(q1.GetDouble(wxT("VALUE")), 
            value);
		value_->SetValue(value);

        wxString valueChangeRate;
        valueChangeRate.Printf(wxT("%.3f"), q1.GetDouble(wxT("VALUECHANGERATE")));
        valueChangeRate_->SetValue(valueChangeRate);
    
        wxString valueChangeTypeStr = q1.GetString(wxT("VALUECHANGE"));
        if (valueChangeTypeStr == wxT("None"))
            valueChange_->SetSelection(DEF_CHANGE_NONE);
        else if (valueChangeTypeStr == wxT("Appreciates"))
            valueChange_->SetSelection(DEF_CHANGE_APPRECIATE);
        else if (valueChangeTypeStr == wxT("Depreciates"))
            valueChange_->SetSelection(DEF_CHANGE_DEPRECIATE);
        else
            wxASSERT(false);

         wxString assetTypeStr = q1.GetString(wxT("ASSETTYPE"));
         if (assetTypeStr == wxT("Property"))
            assetType_->SetSelection(DEF_ASSET_PROPERTY);
         else if (assetTypeStr == wxT("Automobile"))
            assetType_->SetSelection(DEF_ASSET_AUTO);
         else if (assetTypeStr == wxT("Household Object"))
            assetType_->SetSelection(DEF_ASSET_HOUSE);
         else if (assetTypeStr == wxT("Art"))
            assetType_->SetSelection(DEF_ASSET_ART);
		 else if (assetTypeStr == wxT("Jewellery"))
			 assetType_->SetSelection(DEF_ASSET_JEWELLERY);
		 else if (assetTypeStr == wxT("Cash"))
			 assetType_->SetSelection(DEF_ASSET_CASH);
         else if (assetTypeStr == wxT("Other"))
            assetType_->SetSelection(DEF_ASSET_OTHER);

    }
    q1.Finalize();
    mmENDSQL_LITE_EXCEPTION;
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

    wxStaticBox* itemStaticBoxSizer4Static = new wxStaticBox(itemDialog1, wxID_ANY, 
        _("Asset Details"));
    wxStaticBoxSizer* itemStaticBoxSizer4 = new wxStaticBoxSizer(itemStaticBoxSizer4Static, 
        wxVERTICAL);
    itemBoxSizer3->Add(itemStaticBoxSizer4, 0, wxALIGN_CENTER_HORIZONTAL|wxALL, 5);

    wxPanel* itemPanel5 = new wxPanel( itemDialog1, ID_PANEL3, wxDefaultPosition, 
        wxDefaultSize, wxTAB_TRAVERSAL );
    itemStaticBoxSizer4->Add(itemPanel5, 1, wxGROW|wxALL, 5);

    wxFlexGridSizer* itemFlexGridSizer6 = new wxFlexGridSizer(4, 4, 0, 0);
    itemPanel5->SetSizer(itemFlexGridSizer6);

    wxStaticText* itemStaticText7 = new wxStaticText( itemPanel5, wxID_STATIC, 
        _("Name"), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer6->Add(itemStaticText7, 0, 
        wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxALL|wxADJUST_MINSIZE, 5);

    assetName_ = new wxTextCtrl( itemPanel5, ID_TEXTCTRL_ASSETNAME, wxT(""), 
        wxDefaultPosition, wxDefaultSize, 0 );
    assetName_->SetToolTip(_("Enter the name of the asset"));
    itemFlexGridSizer6->Add(assetName_, 0, wxGROW|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    wxStaticText* itemStaticText9 = new wxStaticText( itemPanel5, wxID_STATIC, 
        _("Date"), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer6->Add(itemStaticText9, 0, 
        wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxALL|wxADJUST_MINSIZE, 5);
    
    dpc_ = new wxDatePickerCtrl( itemPanel5, ID_DPC_ASSET_PDATE, wxDefaultDateTime, 
              wxDefaultPosition, wxDefaultSize, wxDP_DROPDOWN);    
    itemFlexGridSizer6->Add(dpc_, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxALL, 5);
    dpc_->SetToolTip(_("Specify the date of purchase of asset"));

    wxStaticText* itemStaticText15 = new wxStaticText( itemPanel5, wxID_STATIC,
        _("Asset Type"), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer6->Add(itemStaticText15, 0, 
        wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxALL|wxADJUST_MINSIZE, 5);

     wxString itemAssetTypeStrings[] =  
    {
        _("Property"),
        _("Automobile"),
        _("Household Object"),
        _("Art"),
		_("Jewellery"),
		_("Cash"),
        _("Other"),
    };
    assetType_ = new wxChoice( itemPanel5, ID_DIALOG_ASSETDIALOG_COMBO_ASSETTYPE, 
        wxDefaultPosition, wxDefaultSize, 7, itemAssetTypeStrings, 0 );
    assetType_->SetToolTip(_("Select type of asset"));
    assetType_->SetSelection(DEF_ASSET_PROPERTY);
    itemFlexGridSizer6->Add(assetType_, 0, 
        wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL|wxALL, 5);

     wxStaticText* itemStaticText31 = new wxStaticText( itemPanel5, 
        wxID_STATIC, _("Value"), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer6->Add(itemStaticText31, 0, 
        wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxALL|wxADJUST_MINSIZE, 5);

    value_ = new wxTextCtrl( itemPanel5, ID_TEXTCTRL_ASSETDIALOG_VALUE, wxT(""), 
        wxDefaultPosition, wxDefaultSize, 0 );
    value_->SetToolTip(_("Enter the current value of the asset"));
    itemFlexGridSizer6->Add(value_, 0, 
        wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL|wxALL, 5);


    wxStaticText* itemStaticText11 = new wxStaticText( itemPanel5, wxID_STATIC, 
        _("Change in Value"), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer6->Add(itemStaticText11, 0, 
        wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxALL|wxADJUST_MINSIZE, 5);

    wxString itemTypeStrings[] =  
    {
        _("None"),
        _("Appreciates"),
        _("Depreciates"),
    };
    valueChange_ = new wxChoice( itemPanel5, ID_DIALOG_ASSETDIALOG_COMBO_TYPE, 
        wxDefaultPosition, wxDefaultSize, 3, itemTypeStrings, 0 );
    valueChange_->SetToolTip(_("Specify if the value of the asset changes over time"));
    valueChange_->SetSelection(DEF_CHANGE_NONE);
    itemFlexGridSizer6->Add(valueChange_, 0, wxGROW|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    wxStaticText* itemStaticText13 = new wxStaticText( itemPanel5, 
        wxID_STATIC, _("Rate of Change"), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer6->Add(itemStaticText13, 0, 
        wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxALL|wxADJUST_MINSIZE, 5);

    valueChangeRate_ = new wxTextCtrl( itemPanel5, ID_TEXTCTRL_ASSETDIALOG_CHANGERATE, wxT(""), 
        wxDefaultPosition, wxDefaultSize, 0 );
    valueChangeRate_->SetToolTip(_("Enter the rate at which the asset changes its value in % per year"));
    itemFlexGridSizer6->Add(valueChangeRate_, 0, 
        wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL|wxALL, 5);
    

    wxStaticText* itemStaticText19 = new wxStaticText( itemPanel5, wxID_STATIC, 
        _("Notes"), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer6->Add(itemStaticText19, 0, 
        wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxALL|wxADJUST_MINSIZE, 5);

    notes_ = new wxTextCtrl( itemPanel5, ID_TEXTCTRL_ASSET_NOTES, wxT(""), 
        wxDefaultPosition, wxSize(-1, 75), wxTE_MULTILINE );
    notes_->SetToolTip(_("Enter notes associated with this asset"));
    itemFlexGridSizer6->Add(notes_, 0, wxGROW|wxALIGN_CENTER_VERTICAL|wxALL, 5);

  
    wxPanel* itemPanel27 = new wxPanel( itemDialog1, wxID_ANY, 
        wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
    itemBoxSizer3->Add(itemPanel27, 0, wxALIGN_RIGHT|wxALL, 5);

    wxBoxSizer* itemBoxSizer28 = new wxBoxSizer(wxHORIZONTAL);
    itemPanel27->SetSizer(itemBoxSizer28);

    wxButton* itemButton29 = new wxButton( itemPanel27, ID_BUTTON_ASSET_OK, 
        _("OK"), wxDefaultPosition, wxDefaultSize, 0 );
    itemBoxSizer28->Add(itemButton29, 0, wxALIGN_CENTER_VERTICAL|wxALL, 1);

    wxButton* itemButton30 = new wxButton( itemPanel27, ID_BUTTON_ASSET_CANCEL, 
        _("Cancel"), wxDefaultPosition, wxDefaultSize, 0 );
    itemBoxSizer28->Add(itemButton30, 0, wxALIGN_CENTER_VERTICAL|wxALL, 1);

}

void mmAssetDialog::OnCancel(wxCommandEvent& event)
{
    Close(TRUE);
}

void mmAssetDialog::OnOk(wxCommandEvent& event)
{
    mmBEGINSQL_LITE_EXCEPTION;
    
    wxString pdate = dpc_->GetValue().FormatISODate();
    wxString notes       = notes_->GetValue().Trim();
    wxString name       = assetName_->GetValue().Trim();

    wxString valueStr = value_->GetValue().Trim();
    if (valueStr == wxT(""))
    {
        mmShowErrorMessageInvalid(this, _("Value"));
        return;
    }
    double value = 0;
	if (!mmCurrencyFormatter::formatCurrencyToDouble(valueStr, value) 
        || (value < 0.0))
    {
        mmShowErrorMessage(this, _("Invalid Value "), _("Error"));
        return;
    }

    int valueChangeType = valueChange_->GetSelection();
    wxString valueChangeTypeStr;
    if (valueChangeType == DEF_CHANGE_NONE)
        valueChangeTypeStr = wxT("None");
    else if (valueChangeType == DEF_CHANGE_APPRECIATE)
        valueChangeTypeStr = wxT("Appreciates");
    else if (valueChangeType == DEF_CHANGE_DEPRECIATE)
        valueChangeTypeStr = wxT("Depreciates");
    else
        wxASSERT(false);


    wxString valueChangeRateStr = valueChangeRate_->GetValue().Trim();
    if ((valueChangeRateStr == wxT("")) && (valueChangeType != DEF_CHANGE_NONE))
    {
        mmShowErrorMessageInvalid(this, _("Rate of Change in Value"));
        return;
    }
    double valueChangeRate = 0;
    if(valueChangeRateStr.ToDouble(&valueChangeRate) == false) {
        valueChangeRate = -1.0;
    }
	if ((valueChangeType != DEF_CHANGE_NONE) && (valueChangeRate < 0.0))
    {
        mmShowErrorMessage(this, _("Invalid Value "), _("Error"));
        return;
    }

    int assetType = assetType_->GetSelection();
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

    if (!edit_)
    {
        wxString bufSQL = wxString::Format(wxT("insert into ASSETS_V1 (STARTDATE, ASSETNAME, VALUE, \
                      VALUECHANGE, NOTES, VALUECHANGERATE, ASSETTYPE)\
                      values ('%s', '%s', %f, '%s', '%s', %f, '%s');"),
                      pdate.c_str(), mmCleanString(name).c_str(), 
                      value, valueChangeTypeStr.c_str(), mmCleanString(notes.c_str()).c_str(), 
                      valueChangeRate, assetTypeStr.c_str());  

        int retVal = db_->ExecuteUpdate(bufSQL);
        
    }
    else 
    {
        wxString bufSQL = wxString::Format(wxT("update ASSETS_V1 SET STARTDATE='%s', ASSETNAME='%s', VALUE=%f, VALUECHANGE='%s', \
                      NOTES='%s', VALUECHANGERATE=%f, ASSETTYPE='%s' where ASSETID=%d;"),
                      pdate.c_str(), mmCleanString(name).c_str(), 
                      value, valueChangeTypeStr.c_str(), mmCleanString(notes.c_str()).c_str(), 
                      valueChangeRate, assetTypeStr.c_str(), assetID_);  

       int retVal = db_->ExecuteUpdate(bufSQL);
    }
    mmENDSQL_LITE_EXCEPTION;

    EndModal(wxID_OK);
}

