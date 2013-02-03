/*******************************************************
 Copyright (C) 2006 Madhan Kanagavel
 Copyright (C) 2011,2012 Nikolay & Stefano Giorgio

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

#include "budgetyearentrydialog.h"
#include "util.h"
#include "defs.h"
#include "paths.h"
#include <wx/spinctrl.h>
#include <wx/statline.h>

IMPLEMENT_DYNAMIC_CLASS( mmBudgetYearEntryDialog, wxDialog )

BEGIN_EVENT_TABLE( mmBudgetYearEntryDialog, wxDialog )
    EVT_BUTTON(wxID_OK, mmBudgetYearEntryDialog::OnOk)
END_EVENT_TABLE()

mmBudgetYearEntryDialog::mmBudgetYearEntryDialog() 
: core_()
{
}

mmBudgetYearEntryDialog::mmBudgetYearEntryDialog(
    mmCoreDB* core, wxWindow* parent,
	bool withMonth, wxWindowID id, const wxString& caption, 
	const wxPoint& pos, const wxSize& size, long style ) 
: core_(core)
{
    withMonth_ = withMonth;
    Create(parent, id, caption, pos, size, style);
    if (withMonth_)
        this->SetTitle(_("Budget Month Entry"));
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

    SetIcon(mmex::getProgramIcon());
    
    Centre();
    return TRUE;
}

void mmBudgetYearEntryDialog::CreateControls()
{
    wxBoxSizer* itemBoxSizer2 = new wxBoxSizer(wxVERTICAL);
    this->SetSizer(itemBoxSizer2);

    wxFlexGridSizer* itemGridSizer2 = new wxFlexGridSizer(0, 2, 0, 0);
    itemBoxSizer2->Add(itemGridSizer2, 0, wxALL, 5);
    
    wxStaticText* itemStaticText3 = new wxStaticText( this, wxID_STATIC, _("Budget Year:"));
    itemGridSizer2->Add(itemStaticText3, 0, wxALIGN_LEFT |wxALIGN_CENTER_VERTICAL|wxALL|wxADJUST_MINSIZE, 5);

    wxDateTime today(wxDateTime::Now()); 
    int year = today.GetYear();
    textYear_ = new wxSpinCtrl( this, ID_DIALOG_BUDGETYEARENTRY_TEXTCTRL_YEAR,
        wxEmptyString, wxDefaultPosition, wxSize(100,-1), wxSP_ARROW_KEYS, 1900, 3000, year);
    textYear_->SetToolTip(_("Specify the required year.\nUse Spin buttons to increase or decrease the year."));
    itemGridSizer2->Add(textYear_, 0, wxALIGN_LEFT |wxALIGN_CENTER_VERTICAL|wxALL, 5);

    if (withMonth_)
    {
        wxStaticText* itemStaticTextMonth = new wxStaticText( this, 
            wxID_STATIC, _("Budget Month:"), wxDefaultPosition, wxDefaultSize, 0 );
        itemGridSizer2->Add(itemStaticTextMonth, 0, 
            wxALIGN_LEFT |wxALIGN_CENTER_VERTICAL|wxALL|wxADJUST_MINSIZE, 5);

        int month = today.GetMonth() + 1; // we require months(1..12)
        textMonth_ = new wxSpinCtrl( this, ID_DIALOG_BUDGETYEARENTRY_TEXTCTRL_MONTH,
            wxEmptyString, wxDefaultPosition, wxSize(textYear_->GetSize()), wxSP_ARROW_KEYS, 1, 12, month);
        textMonth_->SetToolTip(_("Specify the required month.\nUse Spin buttons to increase or decrease the month."));
 
        itemGridSizer2->Add(textMonth_, 0, wxALIGN_LEFT |wxALIGN_CENTER_VERTICAL|wxALL, 5);
    }

    wxStaticText* itemStaticText51 = new wxStaticText( this, wxID_STATIC, _("Base Budget On:"),
        wxDefaultPosition, wxDefaultSize, 0 );
    itemGridSizer2->Add(itemStaticText51, 0, 
        wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxALL|wxADJUST_MINSIZE, 5);

    wxArrayString itemYearStrings;
    itemYearStrings.Add(wxT("None"));
    
    itemChoice_ = new wxChoice( this, ID_DIALOG_BUDGETYEARENTRY_COMBO_YEARS, 
        wxDefaultPosition, wxSize(textYear_->GetSize()), itemYearStrings );
    itemGridSizer2->Add(itemChoice_, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxALL, 5);
    itemChoice_->SetToolTip(_("Specify year to base budget on."));

    wxSQLite3ResultSet q1 = core_->db_.get()->ExecuteQuery(SELECT_ALL_FROM_BUDGETYEAR_V1);
    int index = 1;
    while (q1.NextRow())
    {
        wxString budgetYearString = q1.GetString(wxT("BUDGETYEARNAME"));
        itemChoice_->Insert(budgetYearString, index++);
    }
    q1.Finalize();
    
    wxStaticLine* line = new wxStaticLine ( this, wxID_STATIC, wxDefaultPosition, wxDefaultSize, wxLI_HORIZONTAL );
    itemBoxSizer2->Add(line, 0, wxGROW|wxALL, 5);

    wxBoxSizer* itemBoxSizer9 = new wxBoxSizer(wxHORIZONTAL);
    itemBoxSizer2->Add(itemBoxSizer9, 0, wxALIGN_CENTER_HORIZONTAL|wxALL, 5);
   
    wxButton* itemButtonOK = new wxButton( this, wxID_OK, _("OK") );
    itemBoxSizer9->Add(itemButtonOK, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    wxButton* itemButtonCancel = new wxButton( this, wxID_CANCEL, _("Cancel") );
    itemBoxSizer9->Add(itemButtonCancel, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL|wxALL, 5);
}

void mmBudgetYearEntryDialog::OnOk(wxCommandEvent& /*event*/)
{
    wxString currYearText = wxString() << textYear_->GetValue();
    wxString baseYear = itemChoice_->GetStringSelection();

    if (withMonth_)
    {
        wxString currMonthText = wxEmptyString;
        currMonthText << textMonth_->GetValue();
        if (currMonthText.length() != 2 )
            currMonthText = wxString() << wxT("0") << currMonthText;

        currYearText << wxT("-") << currMonthText;
    }

    if (mmDBWrapper::getBudgetYearID(core_->db_.get(), currYearText) != -1)
    {   
        wxMessageBox(_("Budget Year already exists"), SYMBOL_BUDGETYEARENTRYDIALOG_TITLE, wxICON_WARNING);
        return;
    }
    else
    {
        mmDBWrapper::addBudgetYear(core_->db_.get(), currYearText);
        if (baseYear != wxT("None"))
        {
            int baseYearID = mmDBWrapper::getBudgetYearID(core_->db_.get(), baseYear);
            int newYearID  = mmDBWrapper::getBudgetYearID(core_->db_.get(), currYearText);

            mmDBWrapper::copyBudgetYear(core_->db_.get(), newYearID, baseYearID);
        }
    }

    EndModal(wxID_OK);
}
