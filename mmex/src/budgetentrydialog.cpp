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

    if (budget->PERIOD == ("None"))
        itemChoice_->SetSelection(DEF_FREQ_NONE);
    else if (budget->PERIOD == ("Monthly"))
        itemChoice_->SetSelection(DEF_FREQ_MONTHLY);
    else if (budget->PERIOD == ("Yearly"))
        itemChoice_->SetSelection(DEF_FREQ_YEARLY);
    else if (budget->PERIOD == ("Weekly"))
        itemChoice_->SetSelection(DEF_FREQ_WEEKLY);
    else if (budget->PERIOD == ("Bi-Weekly"))
        itemChoice_->SetSelection(DEF_FREQ_BIWEEKLY);
    else if (budget->PERIOD == ("Bi-Monthly"))
        itemChoice_->SetSelection(DEF_FREQ_BIMONTHLY);
    else if (budget->PERIOD == ("Quarterly"))
        itemChoice_->SetSelection(DEF_FREQ_QUARTERLY);
    else if (budget->PERIOD == ("Half-Yearly"))
        itemChoice_->SetSelection(DEF_FREQ_HALFYEARLY);
    else if (budget->PERIOD == ("Daily"))
        itemChoice_->SetSelection(DEF_FREQ_DAILY);
    else
        wxASSERT(false);

    mmDBWrapper::loadBaseCurrencySettings(db_);
    wxString displayAmtString = ("0");

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
    wxSizerFlags flags, flagsExpand;
    flags.Align(wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL).Border(wxALL, 5);
    flagsExpand.Align(wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL).Border(wxALL, 5).Expand(); 

    wxBoxSizer* itemBoxSizer2 = new wxBoxSizer(wxVERTICAL);
    this->SetSizer(itemBoxSizer2);

    wxStaticBox* itemStaticBoxSizer4Static = new wxStaticBox(this, wxID_ANY, (""));
    wxStaticBoxSizer* itemStaticBoxSizer4 = new wxStaticBoxSizer(itemStaticBoxSizer4Static, wxVERTICAL);
    itemBoxSizer2->Add(itemStaticBoxSizer4, flags);

    wxPanel* itemPanel7 = new wxPanel(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
    itemStaticBoxSizer4->Add(itemPanel7, flagsExpand);

    wxFlexGridSizer* itemGridSizer2 = new wxFlexGridSizer(0, 2, 5, 5);
    itemPanel7->SetSizer(itemGridSizer2);
    
    wxStaticText* itemTextCatTag = new wxStaticText(itemPanel7, wxID_STATIC, _("Category: "));
    wxStaticText* itemTextCatName = new wxStaticText(itemPanel7,
        wxID_STATIC, core_->getCategoryName(categID_));

    wxStaticText* itemTextEstCatTag = new wxStaticText(itemPanel7, wxID_STATIC, _("Estimated:"));
    wxStaticText* itemTextEstCatAmt = new wxStaticText(itemPanel7, wxID_STATIC, catEstimateAmountStr_);
    wxStaticText* itemTextActCatTag = new wxStaticText(itemPanel7, wxID_STATIC, _("Actual:"));
    wxStaticText* itemTextActCatAmt = new wxStaticText(itemPanel7, wxID_STATIC, catActualAmountStr_);
    
    itemGridSizer2->Add(itemTextCatTag, flags);
    itemGridSizer2->Add(itemTextCatName, flags);

    if (subcategID_ >= 0) {
    wxStaticText* itemTextSubCatTag = new wxStaticText(itemPanel7,
        wxID_STATIC, _("Sub Category: "));
    wxStaticText* itemTextSubCatName = new wxStaticText( itemPanel7, wxID_STATIC, 
        mmDBWrapper::getSubCategoryName(db_,categID_, subcategID_));
    itemGridSizer2->Add(itemTextSubCatTag, flags);
    itemGridSizer2->Add(itemTextSubCatName, flags);
    }

    itemGridSizer2->Add(itemTextEstCatTag, flags);
    itemGridSizer2->Add(itemTextEstCatAmt, flags);
    itemGridSizer2->Add(itemTextActCatTag, flags);
    itemGridSizer2->Add(itemTextActCatAmt, flags);

    wxStaticText* itemStaticText101 = new wxStaticText( itemPanel7, wxID_STATIC, 
        _("Type:"));
    itemGridSizer2->Add(itemStaticText101, flags);

    wxArrayString itemTypeStrings;  
    itemTypeStrings.Add(_("Expense"));
    itemTypeStrings.Add(_("Income"));

    type_ = new wxChoice(itemPanel7, ID_DIALOG_BUDGETENTRY_COMBO_TYPE, 
        wxDefaultPosition, wxDefaultSize, itemTypeStrings);
    itemGridSizer2->Add(type_, flags);
    type_->SetSelection(DEF_TYPE_EXPENSE);
    type_->SetToolTip(_("Specify whether this category is an income or an expense category"));

    wxStaticText* itemStaticText51 = new wxStaticText(itemPanel7,
        wxID_STATIC, _("Frequency:"));
    itemGridSizer2->Add(itemStaticText51, flags);

    itemChoice_ = new wxChoice(itemPanel7, ID_DIALOG_BUDGETENTRY_COMBO_FREQTYPE);
    wxString frequency[] = {  
        wxTRANSLATE("None"),
        wxTRANSLATE("Weekly"),
        wxTRANSLATE("Bi-Weekly"),
        wxTRANSLATE("Monthly"),
        wxTRANSLATE("Bi-Monthly"),
        wxTRANSLATE("Quarterly"),
        wxTRANSLATE("Half-Yearly"),
        wxTRANSLATE("Yearly"),
        wxTRANSLATE("Daily")};
    for(size_t i = 0; i < sizeof(frequency)/sizeof(wxString); ++i)
        itemChoice_->Append(wxGetTranslation(frequency[i]), new wxStringClientData(frequency[i]));
    itemChoice_->SetSelection(DEF_FREQ_MONTHLY);
    itemChoice_->SetToolTip(_("Specify the frequency of the expense or deposit"));
    itemChoice_->Connect(ID_DIALOG_BUDGETENTRY_COMBO_FREQTYPE, wxEVT_CHAR,
        wxKeyEventHandler(mmBudgetEntryDialog::onChoiceChar), NULL, this);
    itemGridSizer2->Add(itemChoice_, flags);

    wxStaticText* itemStaticText3 = new wxStaticText(itemPanel7,
        wxID_STATIC, _("Amount:"));
    itemGridSizer2->Add(itemStaticText3, flags);

    textAmount_ = new wxTextCtrl( itemPanel7, 
        ID_DIALOG_BUDGETENTRY_TEXTCTRL_AMOUNT, "", wxDefaultPosition,
        wxDefaultSize, wxALIGN_RIGHT|wxTE_PROCESS_ENTER, wxFloatingPointValidator<float>(2));
    itemGridSizer2->Add(textAmount_, flagsExpand);
    textAmount_->SetToolTip(_("Enter the amount budgeted for this category."));
    textAmount_->SetFocus();
    
    wxBoxSizer* itemBoxSizer9 = new wxBoxSizer(wxHORIZONTAL);
    itemBoxSizer2->Add(itemBoxSizer9, flags.Right());
   
    wxButton* itemButton7 = new wxButton(this, wxID_OK);
    itemBoxSizer9->Add(itemButton7, flags);

    wxButton* itemButton8 = new wxButton(this, wxID_CANCEL);
    itemBoxSizer9->Add(itemButton8, flags);
}


void mmBudgetEntryDialog::OnOk(wxCommandEvent& event)
{
    wxString period;
    wxStringClientData* frequency_obj = (wxStringClientData *)itemChoice_->GetClientObject(itemChoice_->GetSelection());
    if (frequency_obj) period = frequency_obj->GetData();

    int typeSelection = type_->GetSelection();
    wxString displayAmtString = textAmount_->GetValue().Trim();
    double amt = 0.0;
    if (!mmex::formatCurrencyToDouble(displayAmtString, amt) || (amt < 0.0))
    {
        mmShowErrorMessage(this, _("Invalid Amount Entered "), _("Error"));
        return;
    }

    if (period == "None" && amt > 0.0) {
        itemChoice_->SetFocus();
        itemChoice_->SetSelection(DEF_FREQ_MONTHLY);
        event.Skip();
        return;
    }
    
    if (amt == 0.0)
        period = "None";

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
