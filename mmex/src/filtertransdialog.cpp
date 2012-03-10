/////////////////////////////////////////////////////////////////////////////
// Name:        filtertransdialog.cpp
// Purpose:     
// Author:      
// Modified by: 
// Created:     08/30/06 07:23:20
// RCS-ID:      
// Copyright:   
// Licence:     
/////////////////////////////////////////////////////////////////////////////

// Generated by DialogBlocks (Personal Edition), 08/30/06 07:23:20

#if defined(__GNUG__) && !defined(__APPLE__)
#pragma implementation "filtertransdialog.h"
#endif

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

#include "filtertransdialog.h"

#include "defs.h"
#include "util.h"
#include "dbwrapper.h"
#include "categdialog.h"
#include "payeedialog.h"
#include "paths.h"

#include <algorithm>
#include <vector>

bool sortTransactionsByDate1( boost::shared_ptr<mmBankTransaction> elem1, 
                             boost::shared_ptr<mmBankTransaction> elem2 )
{
   return elem1->date_ < elem2->date_;
}

IMPLEMENT_DYNAMIC_CLASS( mmFilterTransactionsDialog, wxDialog )

BEGIN_EVENT_TABLE( mmFilterTransactionsDialog, wxDialog )

    EVT_CHECKBOX( ID_CHECKBOXACCOUNT,   mmFilterTransactionsDialog::OnCheckboxaccountClick )
    EVT_CHECKBOX( ID_CHECKBOXDATERANGE, mmFilterTransactionsDialog::OnCheckboxDateRangeClick )
    EVT_CHECKBOX( ID_CHECKBOXPAYEE,     mmFilterTransactionsDialog::OnCheckboxpayeeClick )
    EVT_CHECKBOX( ID_CHECKBOXCATEGORY,  mmFilterTransactionsDialog::OnCheckboxcategoryClick )
    EVT_CHECKBOX( ID_CHECKBOXSTATUS,    mmFilterTransactionsDialog::OnCheckboxstatusClick )
    EVT_CHECKBOX( ID_CHECKBOXTYPE,      mmFilterTransactionsDialog::OnCheckboxtypeClick )
    EVT_CHECKBOX( ID_CHECKBOXAMOUNTRANGE, mmFilterTransactionsDialog::OnCheckboxamountrangeClick )
    EVT_CHECKBOX( ID_CHECKBOXNOTES,     mmFilterTransactionsDialog::OnCheckboxnotesClick )
    EVT_BUTTON( wxID_OK,            mmFilterTransactionsDialog::OnButtonokClick )
    EVT_BUTTON( wxID_CANCEL,        mmFilterTransactionsDialog::OnButtoncancelClick )
    EVT_BUTTON(ID_BUTTONPAYEE,          mmFilterTransactionsDialog::OnPayee)
    EVT_BUTTON(ID_BUTTONCATEGORY,       mmFilterTransactionsDialog::OnCategs)
    EVT_CHECKBOX( ID_CHECKBOXTRANSNUM,  mmFilterTransactionsDialog::OnCheckboxTransNumberClick )

END_EVENT_TABLE()

mmFilterTransactionsDialog::mmFilterTransactionsDialog( )
{
}

mmFilterTransactionsDialog::mmFilterTransactionsDialog(std::vector< boost::shared_ptr<mmBankTransaction> >* trans,
                                                       mmCoreDB* core,
                                                       wxWindow* parent, wxWindowID id, 
                                                       const wxString& caption, 
                                                       const wxPoint& pos, const wxSize& size, 
                                                       long style )
: trans_(trans), core_(core), db_(core->db_.get())
, categID_(-1), subcategID_(-1), payeeID_(-1), refAccountID_(-1)
{
    Create(parent, id, caption, pos, size, style);
}

bool mmFilterTransactionsDialog::Create( wxWindow* parent, wxWindowID id, 
                                        const wxString& caption, const wxPoint& pos, 
                                        const wxSize& size, long style )
{
    accountCheckBox = NULL;
    accountDropDown = NULL;
    dateRangeCheckBox = NULL;
    fromDateCtrl = NULL;
    toDateControl = NULL;
    payeeCheckBox = NULL;
    btnPayee = NULL;
    categoryCheckBox = NULL;
    btnCategory = NULL;
    statusCheckBox = NULL;
    choiceStatus = NULL;
    typeCheckBox = NULL;
    choiceType = NULL;
    amountRangeCheckBox = NULL;
    amountMinEdit = NULL;
    amountMaxEdit = NULL;
    notesCheckBox = NULL;
    notesEdit = NULL;

    SetExtraStyle(GetExtraStyle()|wxWS_EX_BLOCK_EVENTS);
    wxDialog::Create( parent, id, caption, pos, size, style );

    CreateControls();
    GetSizer()->Fit(this);
    GetSizer()->SetSizeHints(this);

    SetIcon(mmex::getProgramIcon());

    Centre();
    return true;
}

void mmFilterTransactionsDialog::CreateControls()
{    
    int fieldWidth = 210;

    wxBoxSizer* itemBoxSizer2 = new wxBoxSizer(wxHORIZONTAL);
    this->SetSizer(itemBoxSizer2);
    
    wxBoxSizer* itemBoxSizer3 = new wxBoxSizer(wxVERTICAL);
    itemBoxSizer2->Add(itemBoxSizer3, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

    wxStaticBox* itemStaticBoxSizer4Static = new wxStaticBox(this, wxID_ANY, _(" Specify "));
    wxStaticBoxSizer* itemStaticBoxSizer4 = new wxStaticBoxSizer(itemStaticBoxSizer4Static, wxVERTICAL);
    itemBoxSizer3->Add(itemStaticBoxSizer4, 0, wxALIGN_CENTER_HORIZONTAL|wxALL, 5);
    
    this->SetSizer(itemBoxSizer2);

    /******************************************************************************
     Items Panel
    *******************************************************************************/
    wxPanel* itemPanel = new wxPanel( this, ID_PANEL11, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
    itemStaticBoxSizer4->Add(itemPanel, 1, wxGROW|wxALL, 5);

    wxFlexGridSizer* itemPanelSizer = new wxFlexGridSizer(2, 2, 0, 0);
    itemPanel->SetSizer(itemPanelSizer);

    //--Start of Row --------------------------------------------------------
    accountCheckBox = new wxCheckBox( itemPanel, ID_CHECKBOXACCOUNT, _("Account: "), 
                                      wxDefaultPosition, wxDefaultSize, wxCHK_2STATE );
    accountCheckBox->SetValue(FALSE);
    itemPanelSizer->Add(accountCheckBox, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    wxArrayString as = core_->getAccountsName();
   
    accountDropDown = new wxChoice( itemPanel, ID_CHOICE4, wxDefaultPosition, wxSize(fieldWidth,-1), as, 0 );
    itemPanelSizer->Add(accountDropDown, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxALL, 5);
    //--End of Row --------------------------------------------------------

    dateRangeCheckBox = new wxCheckBox( itemPanel, ID_CHECKBOXDATERANGE, _("Date Range: "),
                                        wxDefaultPosition, wxDefaultSize, wxCHK_2STATE );
    dateRangeCheckBox->SetValue(FALSE);
    itemPanelSizer->Add(dateRangeCheckBox, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    fromDateCtrl = new wxDatePickerCtrl( itemPanel, ID_CHOICE5, wxDefaultDateTime, 
                                         wxDefaultPosition, wxDefaultSize, wxDP_DROPDOWN);
    toDateControl = new wxDatePickerCtrl( itemPanel, ID_CHOICE6, wxDefaultDateTime, 
                                          wxDefaultPosition, wxDefaultSize, wxDP_DROPDOWN);

    wxBoxSizer* dateSizer = new wxBoxSizer(wxHORIZONTAL);
    dateSizer->Add(fromDateCtrl, 0, wxALIGN_LEFT|wxALL, 5);
    dateSizer->Add(toDateControl, 0, wxALIGN_LEFT|wxALL, 5);
    itemPanelSizer->Add(dateSizer, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxALL, 0);
    //--End of Row --------------------------------------------------------
    
    payeeCheckBox = new wxCheckBox( itemPanel, ID_CHECKBOXPAYEE, _("Payee: "), 
                                    wxDefaultPosition, wxDefaultSize, wxCHK_2STATE );
    payeeCheckBox->SetValue(FALSE);
    itemPanelSizer->Add(payeeCheckBox, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    btnPayee = new wxButton( itemPanel, ID_BUTTONPAYEE, _("Select Payee"), 
                             wxDefaultPosition, wxSize(fieldWidth,-1), 0 );
    itemPanelSizer->Add(btnPayee, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxALL, 5);
    //--End of Row --------------------------------------------------------

    categoryCheckBox = new wxCheckBox( itemPanel, ID_CHECKBOXCATEGORY, _("Category: "), 
                                       wxDefaultPosition, wxDefaultSize, wxCHK_2STATE );
    categoryCheckBox->SetValue(FALSE);
    itemPanelSizer->Add(categoryCheckBox, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    btnCategory = new wxButton( itemPanel, ID_BUTTONCATEGORY, _("Select Category"), 
                                wxDefaultPosition, wxSize(fieldWidth,-1), 0 );
    itemPanelSizer->Add(btnCategory, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxALL, 5);
    //--End of Row --------------------------------------------------------

    statusCheckBox = new wxCheckBox( itemPanel, ID_CHECKBOXSTATUS, _("Status: "), 
                                     wxDefaultPosition, wxDefaultSize, wxCHK_2STATE );
    statusCheckBox->SetValue(FALSE);
    itemPanelSizer->Add(statusCheckBox, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    wxArrayString choiceStatusStrings;
    choiceStatusStrings.Add(_("None"));
    choiceStatusStrings.Add(_("Reconciled"));
    choiceStatusStrings.Add(_("Void"));
    choiceStatusStrings.Add(_("Follow up"));
    choiceStatusStrings.Add(_("Duplicate"));

    choiceStatus = new wxChoice( itemPanel, ID_CHOICE7, wxDefaultPosition, wxDefaultSize, choiceStatusStrings);
    itemPanelSizer->Add(choiceStatus, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxALL, 5);
    choiceStatus->SetSelection(mmIniOptions::instance().transStatusReconciled_);
    choiceStatus->SetToolTip(_("Specify the status for the transaction"));
    //--End of Row --------------------------------------------------------

    typeCheckBox = new wxCheckBox( itemPanel, ID_CHECKBOXTYPE, _("Type: "), 
                                   wxDefaultPosition, wxDefaultSize, wxCHK_2STATE );
    typeCheckBox->SetValue(FALSE);
    
    wxArrayString choiceTypeStrings;
    choiceTypeStrings.Add(_("Withdrawal"));
    choiceTypeStrings.Add(_("Deposit"));
    choiceTypeStrings.Add(_("Transfer"));
    choiceType = new wxChoice( itemPanel, ID_CHOICE8, wxDefaultPosition, wxDefaultSize, choiceTypeStrings);

    choiceType->SetSelection(0);
    choiceType->Show(false);
    choiceType->SetToolTip(_("Specify the type of transaction."));

    wxBoxSizer* typeSizer = new wxBoxSizer(wxVERTICAL);
    cbTypeWithdrawal_ = new wxCheckBox( itemPanel, ID_DIALOG_TRANSFILTER_CB_TYPE_WITHDRAWAL, _("Withdrawal"), wxDefaultPosition, wxDefaultSize, wxCHK_2STATE );
    cbTypeWithdrawal_->Enable(false);
    cbTypeDeposit_ = new wxCheckBox( itemPanel, ID_DIALOG_TRANSFILTER_CB_TYPE_DEPOSIT, _("Deposit"), wxDefaultPosition, wxDefaultSize, wxCHK_2STATE );
    cbTypeDeposit_->Enable(false);
    cbTypeTransfer_ = new wxCheckBox( itemPanel, ID_DIALOG_TRANSFILTER_CB_TYPE_TRANSFER, _("Transfer"), wxDefaultPosition, wxDefaultSize, wxCHK_2STATE );
    cbTypeTransfer_->Enable(false);

    itemPanelSizer->Add(typeCheckBox, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxALL, 5);
    itemPanelSizer->Add(typeSizer, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxALL, 5);
    typeSizer ->Add(choiceType, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxALL, 5);
    typeSizer ->Add(cbTypeWithdrawal_, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxALL, 2);
    typeSizer ->Add(cbTypeDeposit_, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxALL, 2);
    typeSizer ->Add(cbTypeTransfer_, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxALL, 2);
    
    //--End of Row --------------------------------------------------------

    amountRangeCheckBox = new wxCheckBox( itemPanel, ID_CHECKBOXAMOUNTRANGE, _("Amount Range: "), 
                                          wxDefaultPosition, wxDefaultSize, wxCHK_2STATE );
    amountRangeCheckBox->SetValue(FALSE);
    itemPanelSizer->Add(amountRangeCheckBox, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    amountMinEdit = new wxTextCtrl( itemPanel, ID_TEXTCTRL13, wxT(""), wxDefaultPosition, wxSize(fieldWidth/2-5,-1), wxALIGN_RIGHT|wxTE_PROCESS_ENTER , doubleValidator() );
    amountMaxEdit = new wxTextCtrl( itemPanel, ID_TEXTCTRL14, wxT(""), wxDefaultPosition, wxSize(fieldWidth/2-5,-1), wxALIGN_RIGHT|wxTE_PROCESS_ENTER , doubleValidator() );

    wxBoxSizer* amountSizer = new wxBoxSizer(wxHORIZONTAL);
    amountSizer->Add(amountMinEdit, 0, wxALIGN_LEFT|wxALL, 5);
    amountSizer->Add(amountMaxEdit, 0, wxALIGN_LEFT|wxALL, 5);
    itemPanelSizer->Add(amountSizer, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxALL, 0);
    //--End of Row --------------------------------------------------------

    transNumberCheckBox = new wxCheckBox( itemPanel, ID_CHECKBOXTRANSNUM, _("Number: "),
                                          wxDefaultPosition, wxDefaultSize, wxCHK_2STATE );
    transNumberCheckBox->SetValue(FALSE);
    itemPanelSizer->Add(transNumberCheckBox, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    transNumberEdit = new wxTextCtrl( itemPanel, ID_TEXTTRANSNUM, _T(""), wxDefaultPosition, wxSize(fieldWidth,-1), 0 );
    itemPanelSizer->Add(transNumberEdit, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxALL, 5);
    //--End of Row --------------------------------------------------------

    notesCheckBox = new wxCheckBox( itemPanel, ID_CHECKBOXNOTES, _("Notes: "), 
                                    wxDefaultPosition, wxDefaultSize, wxCHK_2STATE );
    notesCheckBox->SetValue(FALSE);
    itemPanelSizer->Add(notesCheckBox, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    notesEdit = new wxTextCtrl( itemPanel, ID_TEXTCTRL15, _T(""), wxDefaultPosition, wxSize(fieldWidth,-1), 0 );
    itemPanelSizer->Add(notesEdit, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxALL, 5);
    //--End of Row --------------------------------------------------------

    /******************************************************************************
     Button Panel with OK/Cancel buttons   
    *******************************************************************************/
    wxPanel* buttonPanel = new wxPanel( this, ID_PANEL12, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
    itemBoxSizer3->Add(buttonPanel, 0, wxALIGN_CENTER_HORIZONTAL|wxALL, 5);

    wxBoxSizer* buttonPanelSizer = new wxBoxSizer(wxHORIZONTAL);
    buttonPanel->SetSizer(buttonPanelSizer);

    wxButton* itemButton30 = new wxButton( buttonPanel, wxID_OK, _("&OK"));
    buttonPanelSizer->Add(itemButton30, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

    wxButton* itemButtonCancel = new wxButton( buttonPanel, wxID_CANCEL, _("&Cancel"));
    buttonPanelSizer->Add(itemButtonCancel, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);
    itemButtonCancel->SetFocus();
    
    // disable all controls at startup
    accountDropDown->Enable(false);
    fromDateCtrl->Enable(false);
    toDateControl->Enable(false);
    btnPayee->Enable(false);
    btnCategory->Enable(false);
    choiceStatus->Enable(false);
    choiceType->Enable(false);
    amountMinEdit->Enable(false);
    amountMaxEdit->Enable(false);
    notesEdit->Enable(false);
    transNumberEdit->Enable(false);
}   

/*!
 * Should we show tooltips?
 */

bool mmFilterTransactionsDialog::ShowToolTips()
{
    return TRUE;
}

void mmFilterTransactionsDialog::OnCheckboxaccountClick( wxCommandEvent& /*event*/ )
{
    accountDropDown->Enable(accountCheckBox->GetValue());
}


void mmFilterTransactionsDialog::OnCheckboxDateRangeClick( wxCommandEvent& /*event*/ )
{
    fromDateCtrl->Enable(this->dateRangeCheckBox->GetValue());
    toDateControl->Enable(this->dateRangeCheckBox->GetValue());
}

void mmFilterTransactionsDialog::OnCheckboxpayeeClick( wxCommandEvent& /*event*/ )
{
    btnPayee->Enable(this->payeeCheckBox->GetValue());
}

void mmFilterTransactionsDialog::OnCheckboxcategoryClick( wxCommandEvent& /*event*/ )
{
    btnCategory->Enable(this->categoryCheckBox->GetValue());
}

void mmFilterTransactionsDialog::OnCheckboxstatusClick( wxCommandEvent& /*event*/ )
{
    choiceStatus->Enable(statusCheckBox->GetValue());
}

void mmFilterTransactionsDialog::OnCheckboxtypeClick( wxCommandEvent& /*event*/ )
{
    choiceType->Enable(typeCheckBox->GetValue());
    cbTypeWithdrawal_->Enable(typeCheckBox->GetValue());
    cbTypeDeposit_->Enable(typeCheckBox->GetValue());
    cbTypeTransfer_->Enable(typeCheckBox->GetValue());

}

void mmFilterTransactionsDialog::OnCheckboxamountrangeClick( wxCommandEvent& /*event*/ )
{
    amountMinEdit->Enable(amountRangeCheckBox->GetValue());
    amountMaxEdit->Enable(amountRangeCheckBox->GetValue());
}

void mmFilterTransactionsDialog::OnCheckboxnotesClick( wxCommandEvent& /*event*/ )
{
    notesEdit->Enable(notesCheckBox->GetValue());
}

void mmFilterTransactionsDialog::OnCheckboxTransNumberClick( wxCommandEvent& /*event*/ )
{
    transNumberEdit->Enable(transNumberCheckBox->GetValue());
}

void mmFilterTransactionsDialog::OnButtonokClick( wxCommandEvent& /*event*/ )
{
    std::vector< boost::shared_ptr<mmBankTransaction> >::const_iterator i;
    for (i = core_->bTransactionList_.transactions_.begin(); i != core_->bTransactionList_.transactions_.end(); i++ )
    {
        boost::shared_ptr<mmBankTransaction> pBankTransaction = *i;
        if (pBankTransaction)
        {
            /* START FILTERING TRANSACTIONS */
            if (accountCheckBox->GetValue())
            {
                wxString acctName = accountDropDown->GetStringSelection();
                int fromAccountID = core_->getAccountID(acctName);
                refAccountID_ = fromAccountID;
                refAccountStr_ = acctName;

                if ((pBankTransaction->accountID_ != fromAccountID) && (pBankTransaction->toAccountID_ != fromAccountID))
                    continue; // skip
            }

            if (dateRangeCheckBox->GetValue())
            {
                wxDateTime dtBegin = fromDateCtrl->GetValue();
                wxDateTime dtEnd = toDateControl->GetValue();

                if ((dtBegin == dtEnd) && (dtBegin.IsSameDate(pBankTransaction->date_)))
                {

                }
                else
                {
                    if (!pBankTransaction->date_.IsBetween(dtBegin, dtEnd))
                        continue; // skip
                }
            }

            if (payeeCheckBox->GetValue() && (payeeID_ != -1))
            {
                if (pBankTransaction->payeeID_ != payeeID_)
                    continue; // skip
            }

            if (categoryCheckBox->GetValue() && (categID_ != -1))
            {
                bool ignoreSubCateg = false;
                if (subcategID_ == -1)
                    ignoreSubCateg = true;
                if (!pBankTransaction->containsCategory(categID_, subcategID_, ignoreSubCateg))
                {
                    pBankTransaction->reportCategAmountStr_ = wxT("");
                    continue;
                }

                if (pBankTransaction->splitEntries_->numEntries() > 0)
                {
                    pBankTransaction->reportCategAmount_ = fabs(pBankTransaction->getAmountForSplit(categID_, subcategID_));

                    boost::shared_ptr<mmCurrency> pCurrencyPtr = core_->getCurrencyWeakPtr(pBankTransaction->accountID_).lock();
                    wxASSERT(pCurrencyPtr);
                    mmex::formatDoubleToCurrencyEdit(pBankTransaction->reportCategAmount_, pBankTransaction->reportCategAmountStr_);
                }
                else
                {
                    pBankTransaction->reportCategAmount_ = -1;
                    pBankTransaction->reportCategAmountStr_.clear();
                }
            }

            if (statusCheckBox->GetValue())
            {
                if (getTransformedTrxStatus(choiceStatus->GetSelection()) != pBankTransaction->status_)
                    continue; //skip
            }

            if (typeCheckBox->GetValue())
            {
                wxString transCode = pBankTransaction->transType_;
                wxString transCodeSelectedWithdraval=wxT("-");
                wxString transCodeSelectedDeposit=wxT("-");
                wxString transCodeSelectedTransfer=wxT("-");
                if (cbTypeWithdrawal_->GetValue())
                    transCodeSelectedWithdraval = TRANS_TYPE_WITHDRAWAL_STR;
                if (cbTypeDeposit_->GetValue())
                    transCodeSelectedDeposit = TRANS_TYPE_DEPOSIT_STR;
                if (cbTypeTransfer_->GetValue())
                    transCodeSelectedTransfer = TRANS_TYPE_TRANSFER_STR;

                if (transCodeSelectedWithdraval != transCode && transCodeSelectedDeposit != transCode && transCodeSelectedTransfer != transCode)
                    continue; // skip
            }

            if (amountRangeCheckBox->GetValue())
            {
                wxString minamt = amountMinEdit->GetValue();
                wxString maxamt = amountMaxEdit->GetValue();
                if (!minamt.IsEmpty())
                {
                    double amount;
                    if (!mmex::formatCurrencyToDouble(minamt, amount) || (amount < 0.0))
                    {
                        mmShowErrorMessage(this, _("Invalid Amount Entered "), _("Error"));
                        return;
                    }

                    if (pBankTransaction->amt_ < amount)
                        continue; // skip
                }

                if (!maxamt.IsEmpty())
                {
                    double amount;
                    if (!mmex::formatCurrencyToDouble(maxamt, amount) || (amount < 0.0))
                    {
                        mmShowErrorMessage(this, _("Invalid Amount Entered "), _("Error"));
                        return;
                    }

                    if (pBankTransaction->amt_ > amount)
                        continue; // skip
                }
            }

            if (notesCheckBox->GetValue())
            {
                wxString notes = notesEdit->GetValue().Trim().Lower();
                wxString orig = pBankTransaction->notes_.Lower();
                if (!orig.Contains(notes))
                    continue;
            }
        
            if (transNumberCheckBox->GetValue())
            {
                wxString transNumber = transNumberEdit->GetValue().Trim().Lower();
                wxString orig = pBankTransaction->transNum_.Lower();
                if (!orig.Contains(transNumber))
                    continue;
            }

            (*trans_).push_back(pBankTransaction);
        }
    }

    std::sort((*trans_).begin(), (*trans_).end(), sortTransactionsByDate1);
    
    EndModal(wxID_OK);
}

void mmFilterTransactionsDialog::OnButtoncancelClick( wxCommandEvent& /*event*/ )
{
    EndModal(wxID_CANCEL);
}

void mmFilterTransactionsDialog::OnCategs(wxCommandEvent& /*event*/)
{
    mmCategDialog dlg(core_, this);
    dlg.ShowModal();

    if (dlg.categID_ == -1)
    {
        // check if categ and subcateg are now invalid
        wxString catName = core_->getCategoryName(categID_);
        if (catName.IsEmpty())
        {
            // cannot find category
            categID_ = -1;
            subcategID_ = -1;
            btnCategory->SetLabel(_("Select Category"));
            return;
        }

        if (dlg.subcategID_ != -1)
        {
            wxString subcatName = mmDBWrapper::getSubCategoryName(db_, categID_, subcategID_);
            if (subcatName.IsEmpty())
            {
                subcategID_ = -1;
                btnCategory->SetLabel(catName);
                return;
            }
        }
        else
        {
            catName.Replace(wxT("&"), wxT("&&"));
            btnCategory->SetLabel(catName);
        }
        
        return;
    }
    
    categID_ = dlg.categID_;
    subcategID_ = dlg.subcategID_;

    wxString catName = core_->getCategoryName(dlg.categID_);
    catName.Replace(wxT("&"), wxT("&&"));
    wxString categString = catName;

    if (dlg.subcategID_ != -1)
    {
        wxString subcatName = mmDBWrapper::getSubCategoryName(db_,
            dlg.categID_, dlg.subcategID_);
        subcatName.Replace(wxT("&"), wxT("&&"));
        categString += wxT(" : ");
        categString += subcatName;
    }
    
     btnCategory->SetLabel(categString);
}

void mmFilterTransactionsDialog::OnPayee(wxCommandEvent& /*event*/)
{
    mmPayeeDialog dlg(this, core_);
    if ( dlg.ShowModal() == wxID_OK )
    {
        payeeID_ = dlg.getPayeeId();
        if (payeeID_ == -1)
        {
            btnPayee->SetLabel(wxT("Select Payee"));
            return;
        }
        wxString payeeName = mmDBWrapper::getPayee(db_, payeeID_, categID_, subcategID_);
        btnPayee->SetLabel(mmReadyDisplayString(payeeName));
    }
}

bool mmFilterTransactionsDialog::getDateRange(wxDateTime& startDate, wxDateTime& endDate)
{
    bool validDates = false;
    if (dateRangeCheckBox->IsChecked())
    {
        startDate = fromDateCtrl->GetValue();
        endDate = toDateControl->GetValue();
        validDates = true;
    }
    return validDates;
}

wxString mmFilterTransactionsDialog::userDateRangeStr()
{
    wxString dateStr;
    if (dateRangeCheckBox->IsChecked())
    {
        wxString dtBegin = mmGetDateForDisplay(db_, fromDateCtrl->GetValue());
        wxString dtEnd = mmGetDateForDisplay(db_, toDateControl->GetValue());
        dateStr << _("From ") << dtBegin << _(" to ") << dtEnd;
    }
    return dateStr;
}

wxString mmFilterTransactionsDialog::userPayeeStr()
{
    wxString payeeStr;
    if (payeeCheckBox->IsChecked())
    {
        payeeStr = btnPayee->GetLabelText();
    }
    return payeeStr;
}

wxString mmFilterTransactionsDialog::userCategoryStr()
{
    wxString catStr;
    if (categoryCheckBox->IsChecked())
    {
        catStr = btnCategory->GetLabelText();
    }
    return catStr;
}

wxString mmFilterTransactionsDialog::userStatusStr()
{
    wxString statusStr;
    if (statusCheckBox->IsChecked())
    {
        statusStr = choiceStatus->GetLabelText();
    }
    return statusStr;
}

wxString mmFilterTransactionsDialog::userTypeStr()
{
    wxString transCode = wxEmptyString;
    if (typeCheckBox->IsChecked())
    {
        if (cbTypeWithdrawal_->GetValue())
            transCode = TRANS_TYPE_WITHDRAWAL_STR;
        if (cbTypeDeposit_->GetValue())
            transCode << (transCode.IsEmpty() ? wxT("") : wxT(", ")) << TRANS_TYPE_DEPOSIT_STR;
        if (cbTypeTransfer_->GetValue())
            transCode << (transCode.IsEmpty() ? wxT("") : wxT(", ")) << TRANS_TYPE_TRANSFER_STR;
    }
    return transCode;
}

wxString mmFilterTransactionsDialog::userAmountRangeStr()
{
    wxString amountRangeStr;
    if (amountRangeCheckBox->IsChecked())
    {
        wxString minamt = amountMinEdit->GetValue();
        wxString maxamt = amountMaxEdit->GetValue();
        amountRangeStr << _("Min: ") << minamt << _(" Max: ") << maxamt;
    }
    return amountRangeStr;
}

wxString mmFilterTransactionsDialog::userTransNumberStr()
{
    wxString numberStr;
    if (transNumberCheckBox->IsChecked())
    {
        numberStr = transNumberEdit->GetValue().Trim().Lower();
    }
    return numberStr;
}

wxString mmFilterTransactionsDialog::userNotesStr()
{
    wxString notesStr;
    if (notesCheckBox->IsChecked())
    {
        notesStr = notesEdit->GetValue().Trim().Lower();
    }
    return notesStr;
}
