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

#include "budgetyearentrydialog.h"
#include "util.h"

IMPLEMENT_DYNAMIC_CLASS( mmBudgetYearEntryDialog, wxDialog )

BEGIN_EVENT_TABLE( mmBudgetYearEntryDialog, wxDialog )
    EVT_BUTTON(ID_DIALOG_BUDGETYEARENTRY_BUTTON_OK, mmBudgetYearEntryDialog::OnOk)
END_EVENT_TABLE()

mmBudgetYearEntryDialog::mmBudgetYearEntryDialog( )
{
    db_ = 0;
    budgetYear_ = wxT("");
    yearToCopy_ = wxT("");
}

mmBudgetYearEntryDialog::mmBudgetYearEntryDialog( wxSQLite3Database* db, 
                                       wxWindow* parent, 
                                       wxWindowID id, const wxString& caption, 
                                       const wxPoint& pos, const wxSize& size, long style )
{
    db_ = db;
    budgetYear_ = wxT("");
    yearToCopy_ = wxT("");
    Create(parent, id, caption, pos, size, style);
}

bool mmBudgetYearEntryDialog::Create( wxWindow* parent, wxWindowID id, 
                           const wxString& caption, const wxPoint& pos, 
                           const wxSize& size, long style )
{
    SetExtraStyle(GetExtraStyle()|wxWS_EX_BLOCK_EVENTS);
    wxDialog::Create( parent, id, caption, pos, size, style );

    CreateControls();
    GetSizer()->Fit(this);
    GetSizer()->SetSizeHints(this);

    wxIcon icon(wxT("mmex.ico"), wxBITMAP_TYPE_ICO, 32, 32);
    SetIcon(icon);
    
    fillControls();

    Centre();
    return TRUE;
}

void mmBudgetYearEntryDialog::fillControls()
{

}

void mmBudgetYearEntryDialog::CreateControls()
{    
   mmBudgetYearEntryDialog* itemDialog1 = this;

    wxBoxSizer* itemBoxSizer2 = new wxBoxSizer(wxVERTICAL);
    itemDialog1->SetSizer(itemBoxSizer2);

    wxFlexGridSizer* itemGridSizer2 = new wxFlexGridSizer(2, 2, 0, 0);
    itemBoxSizer2->Add(itemGridSizer2, 0, wxALL);
    
    wxStaticText* itemStaticText3 = new wxStaticText( itemDialog1, 
        wxID_STATIC, _("Budget Year:"), wxDefaultPosition, wxDefaultSize, 0 );
    itemGridSizer2->Add(itemStaticText3, 0, 
        wxALIGN_LEFT |wxALIGN_CENTER_VERTICAL|wxALL|wxADJUST_MINSIZE, 5);

    textYear_ = new wxTextCtrl( itemDialog1, ID_DIALOG_BUDGETYEARENTRY_TEXTCTRL_YEAR, wxT(""), 
        wxDefaultPosition, wxDefaultSize, 0 );
    itemGridSizer2->Add(textYear_, 0, wxALIGN_LEFT |wxALIGN_CENTER_VERTICAL|wxALL, 5);
    textYear_->SetToolTip(_("Enter the year in YYYY format to specify year"));

    wxStaticText* itemStaticText51 = new wxStaticText( itemDialog1, 
        wxID_STATIC, _("Base Budget On:"), 
        wxDefaultPosition, wxDefaultSize, 0 );
    itemGridSizer2->Add(itemStaticText51, 0, 
        wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxALL|wxADJUST_MINSIZE, 5);

    wxString itemYearStrings[] =  
    {
        wxT("None"),
    };
    itemChoice_ = new wxChoice( itemDialog1, ID_DIALOG_BUDGETYEARENTRY_COMBO_YEARS, 
        wxDefaultPosition, wxDefaultSize, 1, itemYearStrings, 0 );
    itemGridSizer2->Add(itemChoice_, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxALL, 5);
    itemChoice_->SetSelection(0);
    itemChoice_->SetToolTip(_("Specify year to base budget on."));
    
    mmBEGINSQL_LITE_EXCEPTION;
    wxSQLite3StatementBuffer bufSQL;
    bufSQL.Format("select * from BUDGETYEAR_V1 order by BUDGETYEARNAME;");
    wxSQLite3ResultSet q1 = db_->ExecuteQuery(bufSQL);
    int index = 1;
    while (q1.NextRow())
    {
        wxString payeeString = q1.GetString(wxT("BUDGETYEARNAME"));
        int budgetYearID = q1.GetInt(wxT("BUDGETYEARID"));
        itemChoice_->Insert(payeeString, index++);
    }
    q1.Finalize();
    mmENDSQL_LITE_EXCEPTION;
  
    
    wxBoxSizer* itemBoxSizer9 = new wxBoxSizer(wxHORIZONTAL);
    itemBoxSizer2->Add(itemBoxSizer9, 0, wxALIGN_CENTER_HORIZONTAL|wxALL, 5);

   
    wxButton* itemButton7 = new wxButton( itemDialog1, 
        ID_DIALOG_BUDGETYEARENTRY_BUTTON_OK, _("OK"), wxDefaultPosition, wxDefaultSize, 0 );
    itemBoxSizer9->Add(itemButton7, 0, 
        wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    wxButton* itemButton8 = new wxButton( itemDialog1, wxID_CANCEL,
        _("Cancel"), wxDefaultPosition, wxDefaultSize, 0 );
    itemBoxSizer9->Add(itemButton8, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL|wxALL, 5);
}

void mmBudgetYearEntryDialog::OnOk(wxCommandEvent& event)
{
    wxString currText = textYear_->GetValue().Trim();
    wxString baseYear = itemChoice_->GetStringSelection();
    if (!currText.IsEmpty())
    {
        long newYear = -1;
        if (!currText.ToLong(&newYear) || (newYear < 1900) || (newYear > 3000))
        {
            mmShowErrorMessage(this, _("Year is not valid"), _("Error"));
            return;            
        }
        else
        {
            if (mmDBWrapper::getBudgetYearID(db_, currText) != -1)
            {   
                mmShowErrorMessage(this, _("Budget Year already exists"), _("Error"));
                return;
            }
            else
            {
                mmDBWrapper::addBudgetYear(db_, currText);

                if (baseYear != wxT("None"))
                {

                    int baseYearID = mmDBWrapper::getBudgetYearID(db_, baseYear);
                    int newYearID    = mmDBWrapper::getBudgetYearID(db_, currText);

                    mmDBWrapper::copyBudgetYear(db_, newYearID, baseYearID);


                }
                EndModal(wxID_OK);
            }
        }
    }
    else
    {
       mmShowErrorMessage(this, _("Year is not valid"), _("Error"));
       return;            
    }
    EndModal(wxID_OK);
}

