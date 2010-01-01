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

////@begin includes
////@end includes

#include "filtertransdialog.h"

#include "defs.h"
#include "util.h"
#include "dbwrapper.h"
#include "categdialog.h"
#include "payeedialog.h"
#include "paths.h"
#include <algorithm>
#include <vector>

////@begin XPM images
////@end XPM images

// Defines for Transaction Type
#define DEF_WITHDRAWAL 0
#define DEF_DEPOSIT    1
#define DEF_TRANSFER   2

// Defines for Transaction Status
#define DEF_STATUS_NONE       0
#define DEF_STATUS_RECONCILED 1
#define DEF_STATUS_VOID       2
#define DEF_STATUS_FOLLOWUP   3
#define DEF_STATUS_DUPLICATE  4


bool sortTransactionsByDate1( boost::shared_ptr<mmBankTransaction> elem1, 
                             boost::shared_ptr<mmBankTransaction> elem2 )
{
   return elem1->date_ < elem2->date_;
}

/*!
 * mmFilterTransactionsDialog type definition
 */

IMPLEMENT_DYNAMIC_CLASS( mmFilterTransactionsDialog, wxDialog )

/*!
 * mmFilterTransactionsDialog event table definition
 */

BEGIN_EVENT_TABLE( mmFilterTransactionsDialog, wxDialog )

////@begin mmFilterTransactionsDialog event table entries
    EVT_CHECKBOX( ID_CHECKBOXACCOUNT, mmFilterTransactionsDialog::OnCheckboxaccountClick )

    EVT_CHECKBOX( ID_CHECKBOXDATERANGE, mmFilterTransactionsDialog::OnCheckboxDateRangeClick )

    EVT_CHECKBOX( ID_CHECKBOXPAYEE, mmFilterTransactionsDialog::OnCheckboxpayeeClick )

    EVT_CHECKBOX( ID_CHECKBOXCATEGORY, mmFilterTransactionsDialog::OnCheckboxcategoryClick )

    EVT_CHECKBOX( ID_CHECKBOXSTATUS, mmFilterTransactionsDialog::OnCheckboxstatusClick )

    EVT_CHECKBOX( ID_CHECKBOXTYPE, mmFilterTransactionsDialog::OnCheckboxtypeClick )

    EVT_CHECKBOX( ID_CHECKBOXAMOUNTRANGE, mmFilterTransactionsDialog::OnCheckboxamountrangeClick )

    EVT_CHECKBOX( ID_CHECKBOXNOTES, mmFilterTransactionsDialog::OnCheckboxnotesClick )

    EVT_BUTTON( ID_BUTTONOK, mmFilterTransactionsDialog::OnButtonokClick )

    EVT_BUTTON( ID_BUTTONCANCEL, mmFilterTransactionsDialog::OnButtoncancelClick )

    EVT_BUTTON(ID_BUTTONPAYEE, mmFilterTransactionsDialog::OnPayee)

    EVT_BUTTON(ID_BUTTONCATEGORY, mmFilterTransactionsDialog::OnCategs)

    EVT_CHECKBOX( ID_CHECKBOXTRANSNUM, mmFilterTransactionsDialog::OnCheckboxTransNumberClick )
    

////@end mmFilterTransactionsDialog event table entries

END_EVENT_TABLE()

/*!
 * mmFilterTransactionsDialog constructors
 */

mmFilterTransactionsDialog::mmFilterTransactionsDialog( )
{
}

mmFilterTransactionsDialog::mmFilterTransactionsDialog(std::vector< boost::shared_ptr<mmBankTransaction> >* trans,
                                                       mmCoreDB* core,
                                                       wxWindow* parent, wxWindowID id, 
                                                       const wxString& caption, 
                                                       const wxPoint& pos, const wxSize& size, 
                                                       long style )
{
    trans_ = trans;
    core_ = core;
    db_ = core_->db_.get();
    categID_ = -1;
    subcategID_ = -1;
    payeeID_ = -1;
    Create(parent, id, caption, pos, size, style);
}

/*!
 * mmFilterTransactionsDialog creator
 */

bool mmFilterTransactionsDialog::Create( wxWindow* parent, wxWindowID id, 
                                        const wxString& caption, const wxPoint& pos, 
                                        const wxSize& size, long style )
{
////@begin mmFilterTransactionsDialog member initialisation
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
////@end mmFilterTransactionsDialog member initialisation

////@begin mmFilterTransactionsDialog creation
    SetExtraStyle(GetExtraStyle()|wxWS_EX_BLOCK_EVENTS);
    wxDialog::Create( parent, id, caption, pos, size, style );

    CreateControls();
    GetSizer()->Fit(this);
    GetSizer()->SetSizeHints(this);

    SetIcon(mmex::getProgramIcon());

    Centre();
    return true;
}

/*!
 * Control creation for mmFilterTransactionsDialog
 */

void mmFilterTransactionsDialog::CreateControls()
{    
    mmFilterTransactionsDialog* itemDialog1 = this;

    wxBoxSizer* itemBoxSizer2 = new wxBoxSizer(wxVERTICAL);
    itemDialog1->SetSizer(itemBoxSizer2);

    wxPanel* itemPanel3 = new wxPanel( itemDialog1, ID_PANEL11, 
       wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
    itemBoxSizer2->Add(itemPanel3, 1, wxGROW|wxALL, 5);

    wxFlexGridSizer* itemFlexGridSizer4 = new wxFlexGridSizer(2, 3, 0, 0);
    itemPanel3->SetSizer(itemFlexGridSizer4);

    accountCheckBox = new wxCheckBox( itemPanel3, ID_CHECKBOXACCOUNT,
       _("Specify Account"), wxDefaultPosition, wxDefaultSize, wxCHK_2STATE );
    accountCheckBox->SetValue(FALSE);
    itemFlexGridSizer4->Add(accountCheckBox, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    wxArrayString as;

    static const char sql[] = 
    "select ACCOUNTNAME "
    "from ACCOUNTLIST_V1 "
    "where ACCOUNTTYPE = 'Checking' "
    "order by ACCOUNTNAME";

    wxSQLite3ResultSet q1 = db_->ExecuteQuery(sql);
    while (q1.NextRow())
    {
        as.Add(q1.GetString(wxT("ACCOUNTNAME")));
    }
    q1.Finalize();
   
    accountDropDown = new wxChoice( itemPanel3, ID_CHOICE4, wxDefaultPosition, wxDefaultSize, as, 0 );
    itemFlexGridSizer4->Add(accountDropDown, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    itemFlexGridSizer4->Add(5, 5, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    dateRangeCheckBox = new wxCheckBox( itemPanel3, ID_CHECKBOXDATERANGE, 
       _("Specify Date Range"), wxDefaultPosition, wxDefaultSize, wxCHK_2STATE );
    dateRangeCheckBox->SetValue(FALSE);
    itemFlexGridSizer4->Add(dateRangeCheckBox, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    fromDateCtrl = new wxDatePickerCtrl( itemPanel3, ID_CHOICE5, 
       wxDefaultDateTime, wxDefaultPosition, wxDefaultSize, wxDP_DROPDOWN);
    itemFlexGridSizer4->Add(fromDateCtrl, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    toDateControl = new wxDatePickerCtrl( itemPanel3, ID_CHOICE6,  
       wxDefaultDateTime, wxDefaultPosition, wxDefaultSize, wxDP_DROPDOWN);
    itemFlexGridSizer4->Add(toDateControl, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    payeeCheckBox = new wxCheckBox( itemPanel3, ID_CHECKBOXPAYEE, 
       _("Specify Payee"), wxDefaultPosition, wxDefaultSize, wxCHK_2STATE );
    payeeCheckBox->SetValue(FALSE);
    itemFlexGridSizer4->Add(payeeCheckBox, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    btnPayee = new wxButton( itemPanel3, ID_BUTTONPAYEE, 
       _("Select Payee"), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer4->Add(btnPayee, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    itemFlexGridSizer4->Add(5, 5, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    categoryCheckBox = new wxCheckBox( itemPanel3, ID_CHECKBOXCATEGORY, 
       _("Specify Category"), wxDefaultPosition, wxDefaultSize, wxCHK_2STATE );
    categoryCheckBox->SetValue(FALSE);
    itemFlexGridSizer4->Add(categoryCheckBox, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    btnCategory = new wxButton( itemPanel3, ID_BUTTONCATEGORY, 
       _("Select Category"), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer4->Add(btnCategory, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    itemFlexGridSizer4->Add(5, 5, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    statusCheckBox = new wxCheckBox( itemPanel3, ID_CHECKBOXSTATUS, 
       _("Specify Status"), wxDefaultPosition, wxDefaultSize, wxCHK_2STATE );
    statusCheckBox->SetValue(FALSE);
    itemFlexGridSizer4->Add(statusCheckBox, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    wxString choiceStatusStrings[] = 
    {
        _("None"),
        _("Reconciled"),
        _("Void"),
        _("Follow up"),
        _("Duplicate")
    };  
    choiceStatus = new wxChoice( itemPanel3, ID_CHOICE7, wxDefaultPosition, 
       wxDefaultSize, 5, choiceStatusStrings, 0 );
    itemFlexGridSizer4->Add(choiceStatus, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxALL, 5);
    choiceStatus->SetSelection(0);
    choiceStatus->SetToolTip(_("Specify the status for the transaction"));

    itemFlexGridSizer4->Add(5, 5, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    typeCheckBox = new wxCheckBox( itemPanel3, ID_CHECKBOXTYPE, 
       _("Specify Type"), wxDefaultPosition, wxDefaultSize, wxCHK_2STATE );
    typeCheckBox->SetValue(FALSE);
    itemFlexGridSizer4->Add(typeCheckBox, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    
    wxString choiceTypeStrings[] = 
    {
        _("Withdrawal"),
        _("Deposit"),
        _("Transfer")
    };  
    choiceType = new wxChoice( itemPanel3, ID_CHOICE8, wxDefaultPosition, 
       wxDefaultSize, 3, choiceTypeStrings, 0 );
    itemFlexGridSizer4->Add(choiceType, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxALL, 5);
    choiceType->SetSelection(0);
    choiceType->SetToolTip(_("Specify the type of transaction."));

    itemFlexGridSizer4->Add(5, 5, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    amountRangeCheckBox = new wxCheckBox( itemPanel3, ID_CHECKBOXAMOUNTRANGE, 
       _("Specify Amount Range"), wxDefaultPosition, wxDefaultSize, wxCHK_2STATE );
    amountRangeCheckBox->SetValue(FALSE);
    itemFlexGridSizer4->Add(amountRangeCheckBox, 0, 
       wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    amountMinEdit = new wxTextCtrl( itemPanel3, ID_TEXTCTRL13, _T(""), 
       wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer4->Add(amountMinEdit, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    amountMaxEdit = new wxTextCtrl( itemPanel3, ID_TEXTCTRL14, _T(""), 
       wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer4->Add(amountMaxEdit, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    transNumberCheckBox = new wxCheckBox( itemPanel3, ID_CHECKBOXTRANSNUM, 
       _("Specify Number"), wxDefaultPosition, wxDefaultSize, wxCHK_2STATE );
    transNumberCheckBox->SetValue(FALSE);
    itemFlexGridSizer4->Add(transNumberCheckBox, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    transNumberEdit = new wxTextCtrl( itemPanel3, ID_TEXTTRANSNUM, _T(""), 
       wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer4->Add(transNumberEdit, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    itemFlexGridSizer4->Add(5, 5, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    notesCheckBox = new wxCheckBox( itemPanel3, ID_CHECKBOXNOTES, 
       _("Specify Notes"), wxDefaultPosition, wxDefaultSize, wxCHK_2STATE );
    notesCheckBox->SetValue(FALSE);
    itemFlexGridSizer4->Add(notesCheckBox, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    notesEdit = new wxTextCtrl( itemPanel3, ID_TEXTCTRL15, _T(""), 
       wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer4->Add(notesEdit, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    wxPanel* itemPanel28 = new wxPanel( itemDialog1, ID_PANEL12, 
       wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
    itemBoxSizer2->Add(itemPanel28, 0, wxALIGN_CENTER_HORIZONTAL|wxALL, 5);

    wxBoxSizer* itemBoxSizer29 = new wxBoxSizer(wxHORIZONTAL);
    itemPanel28->SetSizer(itemBoxSizer29);

    wxButton* itemButton30 = new wxButton( itemPanel28, ID_BUTTONOK, _("OK"), 
       wxDefaultPosition, wxDefaultSize, 0 );
    itemBoxSizer29->Add(itemButton30, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

    wxButton* itemButton31 = new wxButton( itemPanel28, ID_BUTTONCANCEL, _("Cancel"), 
       wxDefaultPosition, wxDefaultSize, 0 );
    itemBoxSizer29->Add(itemButton31, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

////@end mmFilterTransactionsDialog content construction

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
   std::vector< boost::shared_ptr<mmBankTransaction> >::iterator i;
   for (i = core_->bTransactionList_.transactions_.begin(); 
        i != core_->bTransactionList_.transactions_.end(); i++ )
   {
      boost::shared_ptr<mmBankTransaction> pBankTransaction = *i;
      if (pBankTransaction)
      {
         /* START FILTERING TRANSACTIONS */
         if (accountCheckBox->GetValue())
         {
            wxString acctName = accountDropDown->GetStringSelection();
            int fromAccountID = mmDBWrapper::getAccountID(db_, acctName);

            if ((pBankTransaction->accountID_ != fromAccountID) 
               && (pBankTransaction->toAccountID_ != fromAccountID))
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
                pBankTransaction->reportCategAmount_ 
                    =  fabs(pBankTransaction->getAmountForSplit(categID_, subcategID_));

                boost::shared_ptr<mmCurrency> pCurrencyPtr = core_->accountList_.getCurrencyWeakPtr(pBankTransaction->accountID_).lock();
                wxASSERT(pCurrencyPtr);
                mmCurrencyFormatter::formatDoubleToCurrencyEdit(pBankTransaction->reportCategAmount_, pBankTransaction->reportCategAmountStr_);
            }
            else
            {
                pBankTransaction->reportCategAmount_ = -1;
                pBankTransaction->reportCategAmountStr_.clear();
            }
        }

        if (statusCheckBox->GetValue())
        {
            wxString status;
            if (choiceStatus->GetSelection()      == DEF_STATUS_NONE)
            {
                status = wxT(""); // nothing yet
            }
            else if (choiceStatus->GetSelection() == DEF_STATUS_RECONCILED)
            {
                status = wxT("R"); 
            }
            else if (choiceStatus->GetSelection() == DEF_STATUS_VOID)
            {
                status = wxT("V"); 
            }
            else if (choiceStatus->GetSelection() == DEF_STATUS_FOLLOWUP)
            {
                status = wxT("F"); 
            }
            else if (choiceStatus->GetSelection() == DEF_STATUS_DUPLICATE)
            {
                status = wxT("D"); 
            }
            if (status != pBankTransaction->status_)
                continue; //skip
        }

        if (typeCheckBox->GetValue())
        {
            wxString transCode;
            int tCode = choiceType->GetSelection();
            if (tCode == DEF_WITHDRAWAL)
                transCode = wxT("Withdrawal");
            else if (tCode == DEF_DEPOSIT)
                transCode = wxT("Deposit");
            else if (tCode == DEF_TRANSFER)
                transCode = wxT("Transfer");

            if (transCode != pBankTransaction->transType_)
                continue; // skip
        }

        if (amountRangeCheckBox->GetValue())
        {
            wxString minamt = amountMinEdit->GetValue();
            wxString maxamt = amountMaxEdit->GetValue();
            if (!minamt.IsEmpty())
            {
                double amount;
                if (!mmCurrencyFormatter::formatCurrencyToDouble(minamt, amount) 
                    || (amount < 0.0))
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
                if (!mmCurrencyFormatter::formatCurrencyToDouble(maxamt, amount) 
                    || (amount < 0.0))
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
    Close(TRUE);
}

void mmFilterTransactionsDialog::OnCategs(wxCommandEvent& /*event*/)
{
    mmCategDialog *dlg = new mmCategDialog(core_, this);
    if ( dlg->ShowModal() == wxID_OK )
    {
       
    }
    dlg->Destroy();
    
    if (dlg->categID_ == -1)
    {
        // check if categ and subcateg are now invalid
        wxString catName = mmDBWrapper::getCategoryName(db_, categID_);
        if (catName.IsEmpty())
        {
            // cannot find category
            categID_ = -1;
            subcategID_ = -1;
            btnCategory->SetLabel(_("Select Category"));
            return;
        }

        if (dlg->subcategID_ != -1)
        {
            wxString subcatName = mmDBWrapper::getSubCategoryName(db_, 
                categID_, subcategID_);
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
    
    categID_ = dlg->categID_;
    subcategID_ = dlg->subcategID_;

    wxString catName = mmDBWrapper::getCategoryName(db_, dlg->categID_);
    catName.Replace(wxT("&"), wxT("&&"));
    wxString categString = catName;

    if (dlg->subcategID_ != -1)
    {
        wxString subcatName = mmDBWrapper::getSubCategoryName(db_,
            dlg->categID_, dlg->subcategID_);
        subcatName.Replace(wxT("&"), wxT("&&"));
        categString += wxT(" : ");
        categString += subcatName;
    }
    
     btnCategory->SetLabel(categString);
}

void mmFilterTransactionsDialog::OnPayee(wxCommandEvent& /*event*/)
{
    mmPayeeDialog* dlg = new mmPayeeDialog(core_, this);    
    if ( dlg->ShowModal() == wxID_OK )
    {
        payeeID_ = dlg->payeeID_;
        if (payeeID_ == -1)
        {
            btnPayee->SetLabel(wxT("Select Payee"));
            return;
        }
        wxString payeeName = mmDBWrapper::getPayee(db_, 
            payeeID_, categID_, subcategID_);
        btnPayee->SetLabel(mmReadyDisplayString(payeeName));
    }
    dlg->Destroy();
}
