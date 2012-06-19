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

#include "budgetyeardialog.h"
#include "budgetyearentrydialog.h"
#include "dbwrapper.h"
#include "util.h"
#include "defs.h"
#include "paths.h"
#include <wx/statline.h>

IMPLEMENT_DYNAMIC_CLASS( mmBudgetYearDialog, wxDialog )

BEGIN_EVENT_TABLE( mmBudgetYearDialog, wxDialog )
    EVT_BUTTON(wxID_OK, mmBudgetYearDialog::OnOk)
    EVT_BUTTON(wxID_ADD, mmBudgetYearDialog::OnAdd)
    EVT_BUTTON(ID_DIALOG_BUDGETYEAR_BUTTON_ADD_MONTH, mmBudgetYearDialog::OnAddMonth)
    EVT_BUTTON(wxID_DELETE, mmBudgetYearDialog::OnDelete)
END_EVENT_TABLE()

mmBudgetYearDialog::mmBudgetYearDialog( )
{
    db_ = 0;
    budgetYearID_ = -1;
}

mmBudgetYearDialog::mmBudgetYearDialog( wxSQLite3Database* db, 
                                       wxWindow* parent, 
                                       wxWindowID id, const wxString& caption, 
                                       const wxPoint& pos, const wxSize& size, long style )
{
    db_ = db;
    budgetYearID_ = -1;
    Create(parent, id, caption, pos, size, style);
}

bool mmBudgetYearDialog::Create( wxWindow* parent, wxWindowID id, 
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

void mmBudgetYearDialog::fillControls()
{
    if (!db_)
       return;

    static const char sql[] = 
        "select BUDGETYEARNAME, BUDGETYEARID "
        "from BUDGETYEAR_V1 "
        "order by BUDGETYEARNAME";

    wxSQLite3ResultSet q1 = db_->ExecuteQuery(sql);
    int index = 0;
    while (q1.NextRow())
    {
        wxString payeeString = q1.GetString(("BUDGETYEARNAME"));
        int budgetYearID = q1.GetInt(("BUDGETYEARID"));
        listBox_->Insert(payeeString, index++, new mmListBoxItem(budgetYearID, payeeString));
    }
    q1.Finalize();
}

void mmBudgetYearDialog::CreateControls()
{    
    wxBoxSizer* itemBoxSizer2 = new wxBoxSizer(wxVERTICAL);
    this->SetSizer(itemBoxSizer2);

    wxBoxSizer* itemBoxSizer3 = new wxBoxSizer(wxHORIZONTAL);
    itemBoxSizer2->Add(itemBoxSizer3, 0, wxGROW|wxALL, 5);

    listBox_ = new wxListBox( this, ID_DIALOG_BUDGETYEAR_LISTBOX_BUDGETYEARS , 
        wxDefaultPosition, wxSize(100, 200));
    itemBoxSizer3->Add(listBox_, 1, wxGROW|wxALL, 1);

    wxBoxSizer* itemBoxSizer5 = new wxBoxSizer(wxHORIZONTAL);
    itemBoxSizer2->Add(itemBoxSizer5, 1, wxGROW|wxALL, 5);

    wxButton* itemButton7 = new wxButton( this, wxID_ADD, 
        _("&Add Year"));
    itemBoxSizer5->Add(itemButton7, 0, wxALIGN_CENTER_VERTICAL);
    itemButton7->SetToolTip(_("Add a new budget year"));

    wxButton* itemBudgetMonth = new wxButton( this, ID_DIALOG_BUDGETYEAR_BUTTON_ADD_MONTH, 
        _("&Add Month"));
    itemBoxSizer5->Add(itemBudgetMonth, 0, wxALIGN_CENTER_VERTICAL|wxLEFT, 5);
    itemBudgetMonth->SetToolTip(_("Add a new budget month"));

    wxButton* itemButtonDelete = new wxButton( this, wxID_DELETE);
    itemBoxSizer5->Add(itemButtonDelete, 0, wxALIGN_CENTER_VERTICAL|wxLEFT, 5);
    itemButtonDelete->SetToolTip(_("Delete existing budget"));

    wxStaticLine* line = new wxStaticLine ( this, wxID_STATIC, wxDefaultPosition, wxDefaultSize, wxLI_HORIZONTAL );
    itemBoxSizer2->Add(line, 0, wxGROW|wxALL, 5);
    
    wxPanel* itemPanel25 = new wxPanel( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
    itemBoxSizer2->Add(itemPanel25, 0, wxALIGN_RIGHT, 5);

    wxBoxSizer* itemButtonSizer = new wxBoxSizer(wxHORIZONTAL);
    itemPanel25->SetSizer(itemButtonSizer);

    wxButton* itemButtonOK = new wxButton( itemPanel25, wxID_OK);
    itemButtonSizer->Add(itemButtonOK, 0, wxALIGN_CENTER_VERTICAL|wxRIGHT|wxBOTTOM, 5);

    wxButton* itemButtonCancel = new wxButton( itemPanel25, wxID_CANCEL);
    itemButtonSizer->Add(itemButtonCancel, 0, wxALIGN_CENTER_VERTICAL|wxRIGHT|wxBOTTOM, 5);
	itemButtonCancel->SetFocus();
}

void mmBudgetYearDialog::OnAdd(wxCommandEvent& /*event*/)
{
    mmBudgetYearEntryDialog dlg(db_, this); 
    if ( dlg.ShowModal() == wxID_OK )
    {
        listBox_->Clear();
        fillControls();
    }
}
 
void mmBudgetYearDialog::OnAddMonth(wxCommandEvent& /*event*/)
{
    mmBudgetYearEntryDialog dlg(db_, this, true); 
    if ( dlg.ShowModal() == wxID_OK )
    {
        listBox_->Clear();
        fillControls();
    }
}

void mmBudgetYearDialog::OnDelete(wxCommandEvent& /*event*/)
{
    wxString budgetYearString = listBox_->GetStringSelection();
    mmDBWrapper::deleteBudgetYear(db_, budgetYearString);
    listBox_->Clear();
    fillControls();
}
 
void mmBudgetYearDialog::OnOk(wxCommandEvent& /*event*/)
{
    EndModal(wxID_OK);
}

