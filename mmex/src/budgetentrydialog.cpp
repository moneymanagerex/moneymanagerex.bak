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

#include "budgetentrydialog.h"
#include "util.h"
#include "defs.h"

#define DEF_TYPE_EXPENSE 0
#define DEF_TYPE_INCOME 1

#define DEF_FREQ_NONE 0
#define DEF_FREQ_WEEKLY 1
#define DEF_FREQ_BIWEEKLY 2
#define DEF_FREQ_MONTHLY 3
#define DEF_FREQ_BIMONTHLY 4
#define DEF_FREQ_QUARTERLY 5
#define DEF_FREQ_HALFYEARLY 6
#define DEF_FREQ_YEARLY 7


#define MMEX_ICON_FNAME mmGetBaseWorkingPath() + wxT("/mmex.ico")


IMPLEMENT_DYNAMIC_CLASS( mmBudgetEntryDialog, wxDialog )

BEGIN_EVENT_TABLE( mmBudgetEntryDialog, wxDialog )
    EVT_BUTTON(ID_DIALOG_BUDGETENTRY_BUTTON_OK, mmBudgetEntryDialog::OnOk)
END_EVENT_TABLE()

mmBudgetEntryDialog::mmBudgetEntryDialog( )
{
    db_ = 0;
    budgetYearID_ = -1;
    categID_ = -1;
    subcategID_ = -1;
}

mmBudgetEntryDialog::mmBudgetEntryDialog( wxSQLite3Database* db,
                                         int budgetYearID, int categID, int subcategID,
                                         wxWindow* parent, 
                                         wxWindowID id, const wxString& caption, 
                                         const wxPoint& pos, const wxSize& size, long style )
{
    db_ = db;
    budgetYearID_ = budgetYearID;
    categID_ = categID;
    subcategID_ = subcategID;
    Create(parent, id, caption, pos, size, style);
}

bool mmBudgetEntryDialog::Create( wxWindow* parent, wxWindowID id, 
                           const wxString& caption, const wxPoint& pos, 
                           const wxSize& size, long style )
{
    SetExtraStyle(GetExtraStyle()|wxWS_EX_BLOCK_EVENTS);
    wxDialog::Create( parent, id, caption, pos, size, style );

    CreateControls();
    GetSizer()->Fit(this);
    GetSizer()->SetSizeHints(this);

    wxIcon icon(MMEX_ICON_FNAME, wxBITMAP_TYPE_ICO, 32, 32);
    SetIcon(icon);
    
    fillControls();

    Centre();
    return TRUE;
}

void mmBudgetEntryDialog::fillControls()
{
    if (!db_)
       return;

    wxString period = wxT("None");
    double amt = 0.0;
    mmDBWrapper::getBudgetEntry(db_, budgetYearID_, categID_, subcategID_, period, amt);

    if (period == wxT("None"))
        itemChoice_->SetSelection(DEF_FREQ_NONE);
    else if (period == wxT("Monthly"))
        itemChoice_->SetSelection(DEF_FREQ_MONTHLY);
    else if (period == wxT("Yearly"))
        itemChoice_->SetSelection(DEF_FREQ_YEARLY);
    else if (period == wxT("Weekly"))
        itemChoice_->SetSelection(DEF_FREQ_WEEKLY);
    else if (period == wxT("Bi-Weekly"))
        itemChoice_->SetSelection(DEF_FREQ_BIWEEKLY);
    else if (period == wxT("Bi-Monthly"))
        itemChoice_->SetSelection(DEF_FREQ_BIMONTHLY);
    else if (period == wxT("Quarterly"))
        itemChoice_->SetSelection(DEF_FREQ_QUARTERLY);
    else if (period == wxT("Half-Yearly"))
        itemChoice_->SetSelection(DEF_FREQ_HALFYEARLY);
    else
        wxASSERT(false);

    mmDBWrapper::loadBaseCurrencySettings(db_);
    wxString displayAmtString = wxT("0");

    if (amt < 0.0)
    {
        type_->SetSelection(DEF_TYPE_EXPENSE);
        amt = -amt;
    }
    else
        type_->SetSelection(DEF_TYPE_INCOME);
    
    mmCurrencyFormatter::formatDoubleToCurrencyEdit(amt, displayAmtString);
    textAmount_->SetValue(displayAmtString);
}

void mmBudgetEntryDialog::CreateControls()
{    
    mmBudgetEntryDialog* itemDialog1 = this;

    wxBoxSizer* itemBoxSizer2 = new wxBoxSizer(wxVERTICAL);
    itemDialog1->SetSizer(itemBoxSizer2);

    wxFlexGridSizer* itemGridSizer2 = new wxFlexGridSizer(2, 2, 0, 0);
    itemBoxSizer2->Add(itemGridSizer2, 0, wxALL);
    
    wxStaticText* itemStaticText101 = new wxStaticText( itemDialog1, wxID_STATIC, 
        _("Type:"), wxDefaultPosition, wxDefaultSize, 0 );
    itemGridSizer2->Add(itemStaticText101, 0, 
        wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxALL|wxADJUST_MINSIZE, 5);

    wxString itemTypeStrings[] =  
    {
        _("Expense"),
        _("Income"),
    };
    type_ = new wxChoice( itemDialog1, ID_DIALOG_BUDGETENTRY_COMBO_TYPE, 
        wxDefaultPosition, wxDefaultSize, 2, itemTypeStrings, 0 );
    itemGridSizer2->Add(type_, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxALL, 5);
    type_->SetSelection(DEF_TYPE_EXPENSE);
    type_->SetToolTip(_("Specify whether this category is an income or an expense category"));

    wxStaticText* itemStaticText51 = new wxStaticText( itemDialog1, wxID_STATIC, 
        _("Frequency:"), wxDefaultPosition, wxDefaultSize, 0 );
    itemGridSizer2->Add(itemStaticText51, 0, 
        wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxALL|wxADJUST_MINSIZE, 5);

    wxString itemFrequencyTypeStrings[] =  
    {
        _("None"),
        _("Weekly"),
        _("Bi-Weekly"),
        _("Monthly"),
        _("Bi-Monthly"),
        _("Quarterly"),
        _("Half-Yearly"),
        _("Yearly"),
    };
    itemChoice_ = new wxChoice( itemDialog1, ID_DIALOG_BUDGETENTRY_COMBO_FREQTYPE, 
        wxDefaultPosition, wxDefaultSize, 8, itemFrequencyTypeStrings, 0 );
    itemGridSizer2->Add(itemChoice_, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxALL, 5);
    itemChoice_->SetSelection(DEF_FREQ_MONTHLY);
    itemChoice_->SetToolTip(_("Specify the frequency of the expense or deposit"));

    wxStaticText* itemStaticText3 = new wxStaticText( itemDialog1, wxID_STATIC, 
        _("Amount:"), wxDefaultPosition, wxDefaultSize, 0 );
    itemGridSizer2->Add(itemStaticText3, 0, 
        wxALIGN_LEFT |wxALIGN_CENTER_VERTICAL|wxALL|wxADJUST_MINSIZE, 5);

    textAmount_ = new wxTextCtrl( itemDialog1, 
        ID_DIALOG_BUDGETENTRY_TEXTCTRL_AMOUNT, wxT(""), wxDefaultPosition, wxDefaultSize, 0 );
    itemGridSizer2->Add(textAmount_, 0, 
        wxALIGN_LEFT |wxALIGN_CENTER_VERTICAL|wxALL, 5);
    textAmount_->SetToolTip(_("Enter the amount budgeted for this category."));

    wxBoxSizer* itemBoxSizer9 = new wxBoxSizer(wxHORIZONTAL);
    itemBoxSizer2->Add(itemBoxSizer9, 0, wxALIGN_CENTER_HORIZONTAL|wxALL, 5);
   
    wxButton* itemButton7 = new wxButton( itemDialog1, 
        ID_DIALOG_BUDGETENTRY_BUTTON_OK, _("OK"), wxDefaultPosition, wxDefaultSize, 0 );
    itemBoxSizer9->Add(itemButton7, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    wxButton* itemButton8 = new wxButton( itemDialog1, wxID_CANCEL, 
        _("Cancel"), wxDefaultPosition, wxDefaultSize, 0 );
    itemBoxSizer9->Add(itemButton8, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL|wxALL, 5);
}


void mmBudgetEntryDialog::OnOk(wxCommandEvent& /*event*/)
{
    int typeSelection = type_->GetSelection();

    int periodSel = itemChoice_->GetSelection();
    wxString period;
    if (periodSel == DEF_FREQ_NONE)
        period = wxT("None");
    else if (periodSel == DEF_FREQ_MONTHLY)
        period = wxT("Monthly");
    else if (periodSel == DEF_FREQ_YEARLY)
        period = wxT("Yearly");
    else if (periodSel == DEF_FREQ_WEEKLY)
        period = wxT("Weekly");
    else if (periodSel == DEF_FREQ_BIWEEKLY)
        period = wxT("Bi-Weekly");
    else if (periodSel == DEF_FREQ_BIMONTHLY)
        period = wxT("Bi-Monthly");
    else if (periodSel == DEF_FREQ_QUARTERLY)
        period = wxT("Quarterly");
    else if (periodSel == DEF_FREQ_HALFYEARLY)
        period = wxT("Half-Yearly");
    else
        wxASSERT(false);

    wxString displayAmtString = textAmount_->GetValue().Trim();
    double amt = 0.0;
    if (!mmCurrencyFormatter::formatCurrencyToDouble(displayAmtString, amt)
        || (amt < 0.0))
    {
        mmShowErrorMessage(this, _("Invalid Amount Entered "), _("Error"));
        return;
    }

    if (period == wxT("None"))
        amt = 0.0;

    if (typeSelection == DEF_TYPE_EXPENSE)
        amt = -amt;

    mmDBWrapper::updateBudgetEntry(db_, budgetYearID_, categID_, subcategID_, period, amt);

    EndModal(wxID_OK);
}

