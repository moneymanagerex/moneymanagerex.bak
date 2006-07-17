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
 /*******************************************************/
#include "currencydialog.h"
#include "util.h"

IMPLEMENT_DYNAMIC_CLASS( mmCurrencyDialog, wxDialog )

BEGIN_EVENT_TABLE( mmCurrencyDialog, wxDialog )
    EVT_BUTTON(ID_DIALOG_CURRENCY_BUTTON_ADD, mmCurrencyDialog::OnAdd)
    EVT_BUTTON(ID_DIALOG_CURRENCY_BUTTON_SELECT, mmCurrencyDialog::OnBSelect)
    EVT_BUTTON(ID_DIALOG_CURRENCY_BUTTON_UPDATE, mmCurrencyDialog::OnEdit)
    EVT_CHOICE(ID_DIALOG_CURRENCY_CHOICE, mmCurrencyDialog::OnCurrencyTypeChanged)  
END_EVENT_TABLE()

mmCurrencyDialog::mmCurrencyDialog( )
{
    db_ = 0;
    currencyID_ = -1;
}

mmCurrencyDialog::~mmCurrencyDialog()
{
    currencyID_ = -1;     
}

mmCurrencyDialog::mmCurrencyDialog( wxSQLite3Database* db, wxWindow* parent, 
                                   wxWindowID id, const wxString& caption, 
                                   const wxPoint& pos, const wxSize& size, long style )
{
    db_ = db;
    currencyID_ = -1;
    Create(parent, id, caption, pos, size, style);
}

mmCurrencyDialog::mmCurrencyDialog(wxSQLite3Database* db,  int currencyID, wxWindow* parent, 
                                   wxWindowID id, const wxString& caption, 
                                   const wxPoint& pos, const wxSize& size, long style )
{
    db_ = db;
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

    wxIcon icon(mainicon_xpm);
    SetIcon(icon);
    
    fillControls();

    Centre();
    return TRUE;
}

void mmCurrencyDialog::fillControls()
{
    if (!db_)
       return;
    currencyChoice_->Clear();

    mmBEGINSQL_LITE_EXCEPTION;
    wxString bufSQL = wxString::Format(wxT("select * from CURRENCYFORMATS_V1"));
    wxSQLite3ResultSet q1 = db_->ExecuteQuery(bufSQL);
    while (q1.NextRow())
    {
        wxString currencyString = q1.GetString(wxT("CURRENCYNAME"));
        int currencyID         = q1.GetInt(wxT("CURRENCYID"));
        currencyChoice_->Append(currencyString, (void*) currencyID);
    }
    q1.Finalize();
    
    if (currencyID_ != -1)
    {
        wxString name = mmDBWrapper::getCurrencyName(db_, currencyID_);
        currencyChoice_->SetStringSelection(name);
    }
    else
        currencyChoice_->SetSelection(0);
    updateControls();

    mmENDSQL_LITE_EXCEPTION;
}

void mmCurrencyDialog::updateControls()
{
    wxString currencyName = currencyChoice_->GetStringSelection();
    mmBEGINSQL_LITE_EXCEPTION;
    wxString bufSQL = wxString::Format(wxT("select * from CURRENCYFORMATS_V1 where CURRENCYNAME='%s';"), mmCleanString(currencyName).c_str());
    wxSQLite3ResultSet q1 = db_->ExecuteQuery(bufSQL);
    if (q1.NextRow())
    {
        wxString pfxSymbol = q1.GetString(wxT("PFX_SYMBOL"));
        wxString sfxSymbol = q1.GetString(wxT("SFX_SYMBOL"));
        wxString dec = q1.GetString(wxT("DECIMAL_POINT"));
        wxString grp = q1.GetString(wxT("GROUP_SEPARATOR"));
        wxString unit = q1.GetString(wxT("UNIT_NAME"));
        wxString cent = q1.GetString(wxT("CENT_NAME"));
        wxString scale = q1.GetString(wxT("SCALE"));
        int currencyID = q1.GetInt(wxT("CURRENCYID"));
        wxString baseRate = q1.GetString(wxT("BASECONVRATE"));

        wxTextCtrl* pfxTx = (wxTextCtrl*)FindWindow(ID_DIALOG_CURRENCY_TEXT_PFX);
        wxTextCtrl* sfxTx = (wxTextCtrl*)FindWindow(ID_DIALOG_CURRENCY_TEXT_SFX);
        wxTextCtrl* decTx = (wxTextCtrl*)FindWindow(ID_DIALOG_CURRENCY_TEXT_DECIMAL);
        wxTextCtrl* grpTx = (wxTextCtrl*)FindWindow(ID_DIALOG_CURRENCY_TEXT_GROUP);
        wxTextCtrl* unitTx = (wxTextCtrl*)FindWindow(ID_DIALOG_CURRENCY_TEXT_UNIT);
        wxTextCtrl* centTx = (wxTextCtrl*)FindWindow(ID_DIALOG_CURRENCY_TEXT_CENTS);
        wxTextCtrl* scaleTx = (wxTextCtrl*)FindWindow(ID_DIALOG_CURRENCY_TEXT_SCALE);
        wxStaticText* sample = (wxStaticText*)FindWindow(ID_DIALOG_CURRENCY_STATIC_SAMPLE);
        wxTextCtrl* baseConvRate = (wxTextCtrl*)FindWindow(ID_DIALOG_CURRENCY_TEXT_BASECONVRATE);

        pfxTx->SetValue(pfxSymbol);
        sfxTx->SetValue(sfxSymbol);
        decTx->SetValue(dec);
        grpTx->SetValue(grp);
        unitTx->SetValue(unit);
        centTx->SetValue(cent);
        scaleTx->SetValue(scale);
        baseConvRate->SetValue(baseRate);
        
        wxString dispAmount;
        double amount = 123456.78;
        mmDBWrapper::loadSettings(db_, currencyID);
        mmCurrencyFormatter::formatDoubleToCurrency(amount, dispAmount);
        sample->SetLabel(dispAmount);

    }
    q1.Finalize();
    mmENDSQL_LITE_EXCEPTION;

}

void mmCurrencyDialog::OnCurrencyTypeChanged(wxCommandEvent& event)
{
    int currencyID = (int)currencyChoice_->GetClientData();
    updateControls();
}

void mmCurrencyDialog::CreateControls()
{    
    mmCurrencyDialog* itemDialog1 = this;

    wxBoxSizer* itemBoxSizer2 = new wxBoxSizer(wxVERTICAL);
    itemDialog1->SetSizer(itemBoxSizer2);

    wxFlexGridSizer* itemFlexGridSizer3 = new wxFlexGridSizer(2, 2, 0, 0);
    itemBoxSizer2->Add(itemFlexGridSizer3, 0, wxALL, 5);

    wxStaticText* itemStaticText4 = new wxStaticText( itemDialog1, wxID_STATIC, 
        _("Currency Name"), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer3->Add(itemStaticText4, 0, 
        wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxALL|wxADJUST_MINSIZE, 5);

    wxString* itemChoice5Strings = NULL;
    currencyChoice_ = new wxChoice( itemDialog1, ID_DIALOG_CURRENCY_CHOICE, 
        wxDefaultPosition, wxDefaultSize, 0, itemChoice5Strings, 0 );
    itemFlexGridSizer3->Add(currencyChoice_, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    wxStaticText* itemStaticText6 = new wxStaticText( itemDialog1, 
        wxID_STATIC, _("Prefix Symbol"), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer3->Add(itemStaticText6, 0, 
        wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxALL|wxADJUST_MINSIZE, 5);

    wxTextCtrl* itemTextCtrl7 = new wxTextCtrl( itemDialog1, 
        ID_DIALOG_CURRENCY_TEXT_PFX, wxT(""), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer3->Add(itemTextCtrl7, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    wxStaticText* itemStaticText8 = new wxStaticText( itemDialog1, 
        wxID_STATIC, _("Suffix Symbol"), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer3->Add(itemStaticText8, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxALL|wxADJUST_MINSIZE, 5);

    wxTextCtrl* itemTextCtrl9 = new wxTextCtrl( itemDialog1, ID_DIALOG_CURRENCY_TEXT_SFX, wxT(""), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer3->Add(itemTextCtrl9, 0, 
        wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    wxStaticText* itemStaticText10 = new wxStaticText( itemDialog1, wxID_STATIC, _("Decimal Char"), 
        wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer3->Add(itemStaticText10, 0, 
        wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxALL|wxADJUST_MINSIZE, 5);

    wxTextCtrl* itemTextCtrl11 = new wxTextCtrl( itemDialog1, 
        ID_DIALOG_CURRENCY_TEXT_DECIMAL, wxT(""), 
        wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer3->Add(itemTextCtrl11, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    wxStaticText* itemStaticText12 = new wxStaticText( itemDialog1, 
        wxID_STATIC, _("Grouping Char"), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer3->Add(itemStaticText12, 0, 
        wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxALL|wxADJUST_MINSIZE, 5);

    wxTextCtrl* itemTextCtrl13 = new wxTextCtrl( itemDialog1, 
        ID_DIALOG_CURRENCY_TEXT_GROUP, wxT(""), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer3->Add(itemTextCtrl13, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    wxStaticText* itemStaticText14 = new wxStaticText( itemDialog1, 
        wxID_STATIC, _("Unit Name"), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer3->Add(itemStaticText14, 0, 
        wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxALL|wxADJUST_MINSIZE, 5);

    wxTextCtrl* itemTextCtrl15 = new wxTextCtrl( itemDialog1, 
        ID_DIALOG_CURRENCY_TEXT_UNIT, 
        wxT(""), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer3->Add(itemTextCtrl15, 0, 
        wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    wxStaticText* itemStaticText16 = new wxStaticText( itemDialog1, wxID_STATIC, 
        _("Cents Name"), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer3->Add(itemStaticText16, 0, 
        wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxALL|wxADJUST_MINSIZE, 5);

    wxTextCtrl* itemTextCtrl17 = new wxTextCtrl( itemDialog1, 
        ID_DIALOG_CURRENCY_TEXT_CENTS, wxT(""), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer3->Add(itemTextCtrl17, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    wxStaticText* itemStaticText81 = new wxStaticText( itemDialog1, wxID_STATIC, 
        _("Conversion to Base Rate"), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer3->Add(itemStaticText81, 0, 
        wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxALL|wxADJUST_MINSIZE, 5);

    wxTextCtrl* itemTextCtrl82 = new wxTextCtrl( itemDialog1, 
        ID_DIALOG_CURRENCY_TEXT_BASECONVRATE, wxT(""), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer3->Add(itemTextCtrl82, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    wxStaticText* itemStaticText18 = new wxStaticText( itemDialog1, 
        wxID_STATIC, _("Scale"), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer3->Add(itemStaticText18, 0, 
        wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxALL|wxADJUST_MINSIZE, 5);

    wxTextCtrl* itemTextCtrl19 = new wxTextCtrl( itemDialog1, 
        ID_DIALOG_CURRENCY_TEXT_SCALE, wxT(""), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer3->Add(itemTextCtrl19, 0, 
        wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxALL, 5);
    
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

    wxButton* itemButton23 = new wxButton( itemDialog1, 
        ID_DIALOG_CURRENCY_BUTTON_ADD, _("Add New"), wxDefaultPosition, wxDefaultSize, 0 );
    itemBoxSizer22->Add(itemButton23, 0, wxALIGN_CENTER_HORIZONTAL|wxALL, 5);

    wxButton* itemButton24 = new wxButton( itemDialog1, 
        ID_DIALOG_CURRENCY_BUTTON_UPDATE, _("Update"), wxDefaultPosition, wxDefaultSize, 0 );
    itemBoxSizer22->Add(itemButton24, 0, wxALIGN_CENTER_HORIZONTAL|wxALL, 5);

    wxButton* itemButton25 = new wxButton( itemDialog1, 
        ID_DIALOG_CURRENCY_BUTTON_SELECT, _("Select"), wxDefaultPosition, wxDefaultSize, 0 );
    itemBoxSizer22->Add(itemButton25, 0, wxALIGN_CENTER_HORIZONTAL|wxALL, 5);
}

void mmCurrencyDialog::OnAdd(wxCommandEvent& event)
{
    wxTextEntryDialog* dlg = new wxTextEntryDialog(this, _("Name of Currency to Add"), 
        _("Add Currency"));
    if ( dlg->ShowModal() == wxID_OK )
    {
        wxString currText = dlg->GetValue().Trim();
        if (!currText.IsEmpty())
        {
            int currID = mmDBWrapper::getCurrencyID(db_, currText);
            if (currID == -1)
            {
                mmBEGINSQL_LITE_EXCEPTION;
                wxString bufSQLStr = wxString::Format(wxT("insert into CURRENCYFORMATS_V1 (CURRENCYNAME, PFX_SYMBOL, SFX_SYMBOL, DECIMAL_POINT,   \
                                                          GROUP_SEPARATOR, UNIT_NAME, CENT_NAME, SCALE, BASECONVRATE) values ('%s', '$', '', '.', ',', 'dollar', 'cents', 100, '1.0');"), mmCleanString(currText).c_str());
                int retVal = db_->ExecuteUpdate(bufSQLStr);
                mmENDSQL_LITE_EXCEPTION;
                
                currencyID_  = mmDBWrapper::getCurrencyID(db_, currText);
                fillControls();
                
            }
            else
            {
                mmShowErrorMessage(this, _("Currency name already exists!"), _("Error"));
                
            }
        }
    }
    dlg->Destroy();
}

void mmCurrencyDialog::OnBSelect(wxCommandEvent& event)
{
    wxString currencyName = currencyChoice_->GetStringSelection();
    currencyID_ = mmDBWrapper::getCurrencyID(db_, currencyName);
    EndModal(wxID_OK);
}

void mmCurrencyDialog::OnEdit(wxCommandEvent& event)
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

    double scal = 0.0;
    scaleTx->GetValue().ToDouble(&scal);

    double convRate = 1.0;
    baseConvRate->GetValue().ToDouble(&convRate);
    if (convRate <= 0.0)
    {
        mmShowErrorMessageInvalid(this, _("Base Conversion Rate should be greater than 0.0"));
        return;
    }

    mmBEGINSQL_LITE_EXCEPTION;
    wxString sqlStmt = wxString::Format(wxT("update CURRENCYFORMATS_V1 set PFX_SYMBOL='%s', SFX_SYMBOL='%s', DECIMAL_POINT='%s', \
                GROUP_SEPARATOR='%s', UNIT_NAME='%s', CENT_NAME='%s', SCALE='%f', BASECONVRATE='%f' where CURRENCYNAME='%s';"),
                mmCleanString(pfxTx->GetValue()).c_str(), mmCleanString(sfxTx->GetValue()).c_str(), mmCleanString(decTx->GetValue()).c_str(),mmCleanString(grpTx->GetValue()).c_str(), mmCleanString(unitTx->GetValue()).c_str(),
                mmCleanString(centTx->GetValue()).c_str(), scal, convRate, mmCleanString(currencyName).c_str());

    int retVal = db_->ExecuteUpdate(sqlStmt);
    mmENDSQL_LITE_EXCEPTION;

    fillControls();
}
