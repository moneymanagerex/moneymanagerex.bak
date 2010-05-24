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
#include "util.h"
#include "defs.h"
#include "paths.h"


IMPLEMENT_DYNAMIC_CLASS( mmBudgetYearDialog, wxDialog )

BEGIN_EVENT_TABLE( mmBudgetYearDialog, wxDialog )
    EVT_BUTTON(ID_DIALOG_BUDGETYEAR_BUTTON_OK, mmBudgetYearDialog::OnOk)
    EVT_BUTTON(ID_DIALOG_BUDGETYEAR_BUTTON_ADD, mmBudgetYearDialog::OnAdd)
    EVT_BUTTON(ID_DIALOG_BUDGETYEAR_BUTTON_DELETE, mmBudgetYearDialog::OnDelete)
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
        wxString payeeString = q1.GetString(wxT("BUDGETYEARNAME"));
        int budgetYearID = q1.GetInt(wxT("BUDGETYEARID"));
        listBox_->Insert(payeeString, index++, new mmBudgetYearListBoxItem(budgetYearID));
    }
    q1.Finalize();
}

void mmBudgetYearDialog::CreateControls()
{    
    mmBudgetYearDialog* itemDialog1 = this;

    wxBoxSizer* itemBoxSizer2 = new wxBoxSizer(wxVERTICAL);
    itemDialog1->SetSizer(itemBoxSizer2);

    wxBoxSizer* itemBoxSizer3 = new wxBoxSizer(wxHORIZONTAL);
    itemBoxSizer2->Add(itemBoxSizer3, 0, wxGROW|wxALL, 5);

    listBox_ = new wxListBox( itemDialog1, ID_DIALOG_BUDGETYEAR_LISTBOX_BUDGETYEARS , 
        wxDefaultPosition, wxSize(100, 200));
    itemBoxSizer3->Add(listBox_, 1, wxGROW|wxALL, 1);

    wxBoxSizer* itemBoxSizer5 = new wxBoxSizer(wxHORIZONTAL);
    itemBoxSizer2->Add(itemBoxSizer5, 1, wxGROW|wxALL, 5);

    wxButton* itemButton7 = new wxButton( itemDialog1, ID_DIALOG_BUDGETYEAR_BUTTON_ADD, 
        _("&Add Budget Year"), 
        wxDefaultPosition, wxDefaultSize, 0 );
    itemBoxSizer5->Add(itemButton7, 0, wxALIGN_CENTER_VERTICAL|wxALL, 1);
    itemButton7->SetToolTip(_("Add a new budget year"));

    wxButton* itemButton8 = new wxButton( itemDialog1, ID_DIALOG_BUDGETYEAR_BUTTON_DELETE, 
        _("&Delete Budget Year"), wxDefaultPosition, wxDefaultSize, 0 );
    itemBoxSizer5->Add(itemButton8, 0, wxALIGN_CENTER_VERTICAL|wxALL, 1);
    itemButton8->SetToolTip(_("Delete existing budget year"));
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

