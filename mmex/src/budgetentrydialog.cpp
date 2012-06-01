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
#include "dbwrapper.h"
#include "mmcoredb.h"
#include "util.h"
#include "defs.h"
#include "paths.h"

enum { DEF_TYPE_EXPENSE, DEF_TYPE_INCOME };
enum { DEF_FREQ_NONE, DEF_FREQ_WEEKLY, DEF_FREQ_BIWEEKLY,
       DEF_FREQ_MONTHLY, DEF_FREQ_BIMONTHLY, DEF_FREQ_QUARTERLY,
       DEF_FREQ_HALFYEARLY, DEF_FREQ_YEARLY, DEF_FREQ_DAILY};


IMPLEMENT_DYNAMIC_CLASS( mmBudgetEntryDialog, wxDialog )

BEGIN_EVENT_TABLE( mmBudgetEntryDialog, wxDialog )
    EVT_BUTTON(wxID_OK, mmBudgetEntryDialog::OnOk)
    EVT_TEXT_ENTER(ID_DIALOG_BUDGETENTRY_TEXTCTRL_AMOUNT, mmBudgetEntryDialog::OnOk)
END_EVENT_TABLE()

mmBudgetEntryDialog::mmBudgetEntryDialog( )
    : db_(0), core_(0), budget_entry_id_(-1), budgetYearID_(-1), categID_(-1), subcategID_(-1)
{ }

mmBudgetEntryDialog::mmBudgetEntryDialog( wxSQLite3Database* db, mmCoreDB* core,
                                            int budget_entry_id,
                                         int budgetYearID, int categID, int subcategID,
                                         const wxString& categoryEstimate, const wxString& CategoryActual,
                                         wxWindow* parent, 
                                         wxWindowID id, const wxString& caption, 
                                         const wxPoint& pos, const wxSize& size, long style )
: db_(db), core_(core), budget_entry_id_(budget_entry_id)
    , budgetYearID_(budgetYearID), categID_(categID), subcategID_(subcategID)
    , catEstimateAmountStr_(categoryEstimate), catActualAmountStr_(CategoryActual)
{
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

    SetIcon(mmex::getProgramIcon());
    
    fillControls();

    Centre();
    return TRUE;
}

void mmBudgetEntryDialog::fillControls()
{
    if (!db_) return;

    DB_View_BUDGETTABLE_V1::Data* budget = BUDGETTABLE_V1.get(budget_entry_id_, db_);
    if (!budget) return;

    double amt = budget->AMOUNT;

    if (budget->PERIOD == wxT("None"))
        itemChoice_->SetSelection(DEF_FREQ_NONE);
    else if (budget->PERIOD == wxT("Monthly"))
        itemChoice_->SetSelection(DEF_FREQ_MONTHLY);
    else if (budget->PERIOD == wxT("Yearly"))
        itemChoice_->SetSelection(DEF_FREQ_YEARLY);
    else if (budget->PERIOD == wxT("Weekly"))
        itemChoice_->SetSelection(DEF_FREQ_WEEKLY);
    else if (budget->PERIOD == wxT("Bi-Weekly"))
        itemChoice_->SetSelection(DEF_FREQ_BIWEEKLY);
    else if (budget->PERIOD == wxT("Bi-Monthly"))
        itemChoice_->SetSelection(DEF_FREQ_BIMONTHLY);
    else if (budget->PERIOD == wxT("Quarterly"))
        itemChoice_->SetSelection(DEF_FREQ_QUARTERLY);
    else if (budget->PERIOD == wxT("Half-Yearly"))
        itemChoice_->SetSelection(DEF_FREQ_HALFYEARLY);
    else if (budget->PERIOD == wxT("Daily"))
        itemChoice_->SetSelection(DEF_FREQ_DAILY);
    else
        wxASSERT(false);

    mmDBWrapper::loadBaseCurrencySettings(db_);
    wxString displayAmtString = wxT("0");

    if (amt <= 0.0)
    {
        type_->SetSelection(DEF_TYPE_EXPENSE);
        amt = -amt;
    }
    else
        type_->SetSelection(DEF_TYPE_INCOME);
    
    mmex::formatDoubleToCurrencyEdit(amt, displayAmtString);
    textAmount_->SetValue(displayAmtString);
}

void mmBudgetEntryDialog::CreateControls()
{    
    wxBoxSizer* itemBoxSizer2 = new wxBoxSizer(wxVERTICAL);
    this->SetSizer(itemBoxSizer2);

    wxStaticBox* itemStaticBoxSizer4Static = new wxStaticBox(this, wxID_ANY, wxT("")); //  ,_("Budget Entry Details"));
    wxStaticBoxSizer* itemStaticBoxSizer4 = new wxStaticBoxSizer(itemStaticBoxSizer4Static, wxVERTICAL);
    itemBoxSizer2->Add(itemStaticBoxSizer4, 0, wxALIGN_CENTER_HORIZONTAL|wxLEFT|wxTOP|wxRIGHT, 10);

    wxPanel* itemPanel7 = new wxPanel(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
    itemStaticBoxSizer4->Add(itemPanel7, 0, wxGROW|wxALL, 10);

    wxFlexGridSizer* itemGridSizer2 = new wxFlexGridSizer(7, 2, 10, 10);
    itemPanel7->SetSizer(itemGridSizer2);
    
    wxStaticText* itemTextCatTag = new wxStaticText( itemPanel7, wxID_STATIC, _("Category: "));
    wxStaticText* itemTextCatName = new wxStaticText( itemPanel7, wxID_STATIC, 
        core_->getCategoryName(categID_), wxDefaultPosition, wxDefaultSize, 0 );

    wxStaticText* itemTextEstCatTag = new wxStaticText(itemPanel7, wxID_STATIC, _("Estimated:"));
    wxStaticText* itemTextEstCatAmt = new wxStaticText(itemPanel7, wxID_STATIC, catEstimateAmountStr_);
    wxStaticText* itemTextActCatTag = new wxStaticText(itemPanel7, wxID_STATIC, _("Actual:"));
    wxStaticText* itemTextActCatAmt = new wxStaticText(itemPanel7, wxID_STATIC, catActualAmountStr_);
    
    itemGridSizer2->Add(itemTextCatTag);
    itemGridSizer2->Add(itemTextCatName);

    if (subcategID_ >= 0) {
    wxStaticText* itemTextSubCatTag = new wxStaticText(itemPanel7, wxID_STATIC, 
        _("Sub Category: "), wxDefaultPosition, wxDefaultSize, 0);
    wxStaticText* itemTextSubCatName = new wxStaticText( itemPanel7, wxID_STATIC, 
        mmDBWrapper::getSubCategoryName(db_,categID_, subcategID_), wxDefaultPosition, wxDefaultSize, 0);
    itemGridSizer2->Add(itemTextSubCatTag);
    itemGridSizer2->Add(itemTextSubCatName);
    }

    itemGridSizer2->Add(itemTextEstCatTag);
    itemGridSizer2->Add(itemTextEstCatAmt);
    itemGridSizer2->Add(itemTextActCatTag);
    itemGridSizer2->Add(itemTextActCatAmt);

    wxStaticText* itemStaticText101 = new wxStaticText( itemPanel7, wxID_STATIC, 
        _("Type:"), wxDefaultPosition, wxDefaultSize, 0 );
    itemGridSizer2->Add(itemStaticText101);

    wxArrayString itemTypeStrings;  
    itemTypeStrings.Add(_("Expense"));
    itemTypeStrings.Add(_("Income"));

    type_ = new wxChoice( itemPanel7, ID_DIALOG_BUDGETENTRY_COMBO_TYPE, 
        wxDefaultPosition, wxDefaultSize, itemTypeStrings);
    itemGridSizer2->Add(type_);
    type_->SetSelection(DEF_TYPE_EXPENSE);
    type_->SetToolTip(_("Specify whether this category is an income or an expense category"));

    wxStaticText* itemStaticText51 = new wxStaticText( itemPanel7, wxID_STATIC, 
        _("Frequency:"), wxDefaultPosition, wxDefaultSize, 0 );
    itemGridSizer2->Add(itemStaticText51);

    wxArrayString itemFrequencyTypeStrings;  
    itemFrequencyTypeStrings.Add(_("None"));
    itemFrequencyTypeStrings.Add(_("Weekly"));
    itemFrequencyTypeStrings.Add(_("Bi-Weekly"));
    itemFrequencyTypeStrings.Add(_("Monthly"));
    itemFrequencyTypeStrings.Add(_("Bi-Monthly"));
    itemFrequencyTypeStrings.Add(_("Quarterly"));
    itemFrequencyTypeStrings.Add(_("Half-Yearly"));
    itemFrequencyTypeStrings.Add(_("Yearly"));
    itemFrequencyTypeStrings.Add(_("Daily"));

    itemChoice_ = new wxChoice( itemPanel7, ID_DIALOG_BUDGETENTRY_COMBO_FREQTYPE, 
        wxDefaultPosition, wxDefaultSize, itemFrequencyTypeStrings);
    itemGridSizer2->Add(itemChoice_);
    itemChoice_->SetSelection(DEF_FREQ_MONTHLY);
    itemChoice_->SetToolTip(_("Specify the frequency of the expense or deposit"));
    itemChoice_->Connect(ID_DIALOG_BUDGETENTRY_COMBO_FREQTYPE, wxEVT_CHAR,
        wxKeyEventHandler(mmBudgetEntryDialog::onChoiceChar), NULL, this);

    wxStaticText* itemStaticText3 = new wxStaticText( itemPanel7, wxID_STATIC, 
        _("Amount:"), wxDefaultPosition, wxDefaultSize, 0 );
    itemGridSizer2->Add(itemStaticText3, 0, 
        wxALIGN_LEFT |wxALIGN_CENTER_VERTICAL|wxALL|wxADJUST_MINSIZE, 0);

    textAmount_ = new wxTextCtrl( itemPanel7, 
        ID_DIALOG_BUDGETENTRY_TEXTCTRL_AMOUNT, wxT(""), wxDefaultPosition,
        wxDefaultSize, wxALIGN_RIGHT|wxTE_PROCESS_ENTER, wxFloatingPointValidator<float>(2));
    itemGridSizer2->Add(textAmount_);
    textAmount_->SetToolTip(_("Enter the amount budgeted for this category."));
    textAmount_->SetFocus();

    //for wx2.9 
    if (subcategID_ < 0) {
    itemGridSizer2->AddSpacer(10);
    itemGridSizer2->AddSpacer(10);
    }
    
    wxBoxSizer* itemBoxSizer9 = new wxBoxSizer(wxHORIZONTAL);
    itemBoxSizer2->Add(itemBoxSizer9, 0, wxALIGN_CENTER|wxALL, 5);
   
    wxButton* itemButton7 = new wxButton( this, wxID_OK,
        _("&OK"), wxDefaultPosition, wxDefaultSize, 0 );
    itemBoxSizer9->Add(itemButton7, 0, wxALIGN_CENTER|wxALL, 5);

    wxButton* itemButton8 = new wxButton( this, wxID_CANCEL, 
        _("&Cancel"), wxDefaultPosition, wxDefaultSize, 0 );
    itemBoxSizer9->Add(itemButton8, 0, wxALIGN_CENTER|wxALL, 5);
}


void mmBudgetEntryDialog::OnOk(wxCommandEvent& event)
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
    else if (periodSel == DEF_FREQ_DAILY)
        period = wxT("Daily");
    else
        wxASSERT(false);

    wxString displayAmtString = textAmount_->GetValue().Trim();
    double amt = 0.0;
    if (!mmex::formatCurrencyToDouble(displayAmtString, amt)
        || (amt < 0.0))
    {
        mmShowErrorMessage(this, _("Invalid Amount Entered "), _("Error"));
        return;
    }

    if (period == wxT("None") && amt > 0) {
        itemChoice_->SetFocus();
        itemChoice_->SetSelection(DEF_FREQ_MONTHLY);
        event.Skip();
        return;
    }
    
    if (amt == 0.0)
        period = wxT("None");

    if (typeSelection == DEF_TYPE_EXPENSE)
        amt = -amt;

    DB_View_BUDGETTABLE_V1::Data* budget = BUDGETTABLE_V1.get(budget_entry_id_, db_);
    if (!budget) budget = BUDGETTABLE_V1.create();
    budget->BUDGETENTRYID = budgetYearID_;
    budget->CATEGID = categID_;
    budget->SUBCATEGID = subcategID_;
    budget->PERIOD = period;
    budget->AMOUNT = amt;
    budget->save(db_);

    EndModal(wxID_OK);
}

void mmBudgetEntryDialog::onChoiceChar(wxKeyEvent& event) {

    int i = itemChoice_->GetSelection();
    if (event.GetKeyCode()==WXK_DOWN) 
    {
        if (i < DEF_FREQ_DAILY ) 
            itemChoice_->SetSelection(++i);
    } 
    else if (event.GetKeyCode()==WXK_UP)
    {
        if (i > DEF_FREQ_NONE)
            itemChoice_->SetSelection(--i);
    } 
    else 
        event.Skip();

}
