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

#include "transdialog.h"
#include "wx/datectrl.h"
#include "categdialog.h"
#include "payeedialog.h"
#include "util.h"
#include "dbwrapper.h"
#include "splittransactionsdialog.h"

// Defines for Transaction Type
#define DEF_WITHDRAWAL 0
#define DEF_DEPOSIT    1
#define DEF_TRANSFER   2

// Defines for Transaction Status
#define DEF_STATUS_NONE       0
#define DEF_STATUS_RECONCILED 1
#define DEF_STATUS_VOID       2
#define DEF_STATUS_FOLLOWUP   3
#define DEF_STATUS_DUPLICATE   4

IMPLEMENT_DYNAMIC_CLASS( mmTransDialog, wxDialog )

BEGIN_EVENT_TABLE( mmTransDialog, wxDialog )
    EVT_BUTTON(ID_DIALOG_TRANS_BUTTON_OK, mmTransDialog::OnOk)
    EVT_BUTTON(ID_DIALOG_TRANS_BUTTONCATEGS, mmTransDialog::OnCategs)
    EVT_BUTTON(ID_DIALOG_TRANS_BUTTONPAYEE, mmTransDialog::OnPayee)
    EVT_BUTTON(ID_DIALOG_TRANS_BUTTONTO, mmTransDialog::OnTo)
    EVT_CHOICE(ID_DIALOG_TRANS_TYPE, mmTransDialog::OnTransTypeChanged)  
    EVT_DATE_CHANGED(ID_DIALOG_TRANS_BUTTONDATE, mmTransDialog::OnDateChanged) 
    EVT_BUTTON(ID_DIALOG_TRANS_BUTTONADVANCED, mmTransDialog::OnAdvanced)
    EVT_CHECKBOX(ID_DIALOG_TRANS_SPLITCHECKBOX, mmTransDialog::OnSplitChecked)
END_EVENT_TABLE()

mmTransDialog::mmTransDialog( )
{
}

mmTransDialog::mmTransDialog(wxSQLite3Database* db, 
                             mmCoreDB* core,
                             int accountID, int transID, bool edit, 
                             wxSQLite3Database* inidb,
                             wxWindow* parent, wxWindowID id, 
                             const wxString& caption, const wxPoint& pos, 
                             const wxSize& size, long style )
{
    db_ = db;
    core_ = core;
    transID_ = transID;
    accountID_ = accountID;
    edit_ = edit;
    categID_ = -1;
    subcategID_ = -1;
    payeeID_ = -1;
    toID_ = -1;
    toTransAmount_ = -1;
    advancedToTransAmountSet_ = false;
    Create(parent, id, caption, pos, size, style);
    inidb_ = inidb;
}

bool mmTransDialog::Create( wxWindow* parent, wxWindowID id, const wxString& caption, 
                           const wxPoint& pos, const wxSize& size, long style )
{
    SetExtraStyle(GetExtraStyle()|wxWS_EX_BLOCK_EVENTS);
    wxDialog::Create( parent, id, caption, pos, size, style );

    CreateControls();
    GetSizer()->Fit(this);
    GetSizer()->SetSizeHints(this);

    wxIcon icon(mainicon_xpm);
    SetIcon(icon);
    
    fillControls();

    boost::shared_ptr<mmSplitTransactionEntries> split(new mmSplitTransactionEntries());
    split_ = split;

    if (edit_)
    {
        dataToControls();
    }
    
   
    Centre();
    return TRUE;
}

void mmTransDialog::dataToControls()
{
    //choiceTrans_->Disable();
    mmBEGINSQL_LITE_EXCEPTION;

    wxSQLite3StatementBuffer bufSQL;
    bufSQL.Format("select * from CHECKINGACCOUNT_V1 where TRANSID=%d;", transID_);
    wxSQLite3ResultSet q1 = db_->ExecuteQuery(bufSQL);
    if (q1.NextRow())
    {
        categID_ = q1.GetInt(wxT("CATEGID"));
        subcategID_ = q1.GetInt(wxT("SUBCATEGID"));
        accountID_ = q1.GetInt(wxT("ACCOUNTID"));
       
        wxString dateString = q1.GetString(wxT("TRANSDATE"));
        wxDateTime dtdt = mmGetStorageStringAsDate(dateString);
        wxString dt = mmGetDateForDisplay(db_, dtdt);
        dpc_->SetValue(dtdt);

        wxString transNumString = q1.GetString(wxT("TRANSACTIONNUMBER"));
        wxString statusString  = q1.GetString(wxT("STATUS"));
        wxString notesString  = mmUnCleanString(q1.GetString(wxT("NOTES")));
        wxString transTypeString = q1.GetString(wxT("TRANSCODE"));
        double transAmount = q1.GetDouble(wxT("TRANSAMOUNT"));
        toTransAmount_ = q1.GetDouble(wxT("TOTRANSAMOUNT"));
      
        if (statusString == wxT(""))
        {
            choiceStatus_->SetSelection(DEF_STATUS_NONE);
        }
        else if (statusString == wxT("R"))
        {
             choiceStatus_->SetSelection(DEF_STATUS_RECONCILED);
        }
        else if (statusString == wxT("V"))
        {
             choiceStatus_->SetSelection(DEF_STATUS_VOID);
        }
        else if (statusString == wxT("F"))
        {
             choiceStatus_->SetSelection(DEF_STATUS_FOLLOWUP);
        }
		else if (statusString == wxT("D"))
        {
             choiceStatus_->SetSelection(DEF_STATUS_DUPLICATE);
        }

         if (transTypeString == wxT("Withdrawal"))
            choiceTrans_->SetSelection(DEF_WITHDRAWAL);
        else if (transTypeString == wxT("Deposit"))
            choiceTrans_->SetSelection(DEF_DEPOSIT);
        else if (transTypeString == wxT("Transfer"))
            choiceTrans_->SetSelection(DEF_TRANSFER);
        updateControlsForTransType();

        payeeID_ = q1.GetInt(wxT("PAYEEID"));
        toID_ = q1.GetInt(wxT("TOACCOUNTID"));
        

        q1.Finalize();

        wxString payeeString;
        bufSQL.Format("select * from PAYEE_V1 where PAYEEID=%d;", payeeID_);
        q1 = db_->ExecuteQuery(bufSQL);
        if (q1.NextRow())
        {
            payeeString = q1.GetString(wxT("PAYEENAME"));
            q1.Finalize();
        }

        *split_.get() = *core_->bTransactionList_.getBankTransactionPtr(transID_)->splitEntries_.get();

        if (split_->numEntries() > 0)
        {
            bCategory_->SetLabel(_("Split Category"));
            cSplit_->SetValue(true);
        }
        else
        {
            wxString catName = mmDBWrapper::getCategoryName(db_, categID_);
            wxString categString = catName;

            if (subcategID_ != -1)
            {
                wxString subcatName = mmDBWrapper::getSubCategoryName(db_, categID_, subcategID_);
                categString += wxT(" : ");
                categString += subcatName;
            }
            bCategory_->SetLabel(categString);
        }

        textNotes_->SetValue(notesString);
        textNumber_->SetValue(transNumString);

       
        if (split_->numEntries() > 0)
        {
            transAmount = split_->getTotalSplits();
            textAmount_->Enable(false);
        }
        wxString dispAmount;
        mmCurrencyFormatter::formatDoubleToCurrencyEdit(transAmount, dispAmount);
        textAmount_->SetValue(dispAmount);

        bPayee_->SetLabel(payeeString);

        if (transTypeString == wxT("Transfer"))
        {
            wxString fromAccount = mmDBWrapper::getAccountName(db_, accountID_);
            wxString toAccount = mmDBWrapper::getAccountName(db_, toID_);

            bPayee_->SetLabel(fromAccount);
            bTo_->SetLabel(toAccount);
            payeeID_ = accountID_;
        }
    }
    mmENDSQL_LITE_EXCEPTION;

}

void mmTransDialog::fillControls()
{
 }

void mmTransDialog::CreateControls()
{    
    mmTransDialog* itemDialog1 = this;

    wxBoxSizer* itemBoxSizer2 = new wxBoxSizer(wxHORIZONTAL);
    itemDialog1->SetSizer(itemBoxSizer2);

    wxBoxSizer* itemBoxSizer3 = new wxBoxSizer(wxVERTICAL);
    itemBoxSizer2->Add(itemBoxSizer3, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

    wxBoxSizer* itemBoxSizer4 = new wxBoxSizer(wxHORIZONTAL);
    itemBoxSizer3->Add(itemBoxSizer4, 0, wxALIGN_LEFT|wxALL, 5);

    wxStaticText* itemStaticText5 = new wxStaticText( itemDialog1, wxID_STATIC,
        _("Transaction Type"), wxDefaultPosition, wxDefaultSize, 0 );
    itemBoxSizer4->Add(itemStaticText5, 0, wxALIGN_CENTER_VERTICAL|wxALL|wxADJUST_MINSIZE, 5);

    wxString itemChoice6Strings[] = 
    {
        _("Withdrawal"),
        _("Deposit"),
        _("Transfer")
    };  
    
    choiceTrans_ = new wxChoice( itemDialog1, ID_DIALOG_TRANS_TYPE, wxDefaultPosition, 
        wxDefaultSize, 3, itemChoice6Strings, 0 );
    itemBoxSizer4->Add(choiceTrans_, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);
    choiceTrans_->SetSelection(0);
    choiceTrans_->SetToolTip(_("Specify the type of transactions to be created."));

    bAdvanced_ = new wxButton( itemDialog1, ID_DIALOG_TRANS_BUTTONADVANCED, _("Advanced"), 
        wxDefaultPosition, wxSize(100, -1), 0 );
    bAdvanced_->Enable(false);
    itemBoxSizer4->Add(bAdvanced_, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxALL, 5);
    bAdvanced_->SetToolTip(_("Specify advanced settings for transaction"));
    

    wxPanel* itemPanel7 = new wxPanel( itemDialog1, wxID_ANY, wxDefaultPosition, 
        wxDefaultSize, wxTAB_TRAVERSAL );
    itemBoxSizer3->Add(itemPanel7, 1, wxGROW|wxALL, 5);

    wxFlexGridSizer* itemFlexGridSizer8 = new wxFlexGridSizer(4, 4, 0, 0);
    itemPanel7->SetSizer(itemFlexGridSizer8);

    wxStaticText* itemStaticText9 = new wxStaticText( itemPanel7, ID_DIALOG_TRANS_STATIC_PAYEE, 
        _("Payee"), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer8->Add(itemStaticText9, 0, 
        wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxALL|wxADJUST_MINSIZE, 5);

    bPayee_ = new wxButton( itemPanel7, ID_DIALOG_TRANS_BUTTONPAYEE, 
        _("Select Payee"), wxDefaultPosition, wxSize(200, -1), 0 );
    itemFlexGridSizer8->Add(bPayee_, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxALL, 5);
    bPayee_->SetToolTip(_("Specify to whom the transaction is going to or coming from "));
    
    wxStaticText* itemStaticText11 = new wxStaticText( itemPanel7, wxID_STATIC, 
        _("Number"), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer8->Add(itemStaticText11, 0,
        wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxALL|wxADJUST_MINSIZE, 5);

    textNumber_ = new wxTextCtrl( itemPanel7, ID_DIALOG_TRANS_TEXTNUMBER, wxT(""), 
        wxDefaultPosition, wxSize(100, -1), 0 );
    itemFlexGridSizer8->Add(textNumber_, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxALL, 5);
    textNumber_->SetToolTip(_("Specify any associated check number or transaction number"));

    wxStaticText* itemStaticText13 = new wxStaticText( itemPanel7,
        ID_DIALOG_TRANS_STATIC_FROM, _("To"), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer8->Add(itemStaticText13, 0, 
        wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxALL|wxADJUST_MINSIZE, 5);
    itemStaticText13->Show(false);

    bTo_ = new wxButton( itemPanel7, ID_DIALOG_TRANS_BUTTONTO, _("Select To Acct"), 
        wxDefaultPosition, wxSize(200, -1), 0 );
    itemFlexGridSizer8->Add(bTo_, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxALL, 5);
    bTo_->Show(false);
    bTo_->SetToolTip(_("Specify the transfer account"));

    wxStaticText* itemStaticText15 = new wxStaticText( itemPanel7, wxID_STATIC, _("Date"), 
        wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer8->Add(itemStaticText15, 0, 
        wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxALL|wxADJUST_MINSIZE, 5);

    dpc_ = new wxDatePickerCtrl( itemPanel7, ID_DIALOG_TRANS_BUTTONDATE, wxDefaultDateTime, 
        wxDefaultPosition, wxSize(100, -1), wxDP_DROPDOWN);
    itemFlexGridSizer8->Add(dpc_, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL|wxALL, 5);
    dpc_->SetToolTip(_("Specify the date of the transaction"));

    wxStaticText* itemStaticText17 = new wxStaticText( itemPanel7, 
        wxID_STATIC, _("Category"), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer8->Add(itemStaticText17, 0, 
        wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxALL|wxADJUST_MINSIZE, 5);

    // ******************************** //
    wxBoxSizer* itemBoxSizer18 = new wxBoxSizer(wxHORIZONTAL);
    itemFlexGridSizer8->Add(itemBoxSizer18, 0, wxGROW|wxALIGN_CENTER_VERTICAL|wxALL, 5);
    
    bCategory_ = new wxButton( itemPanel7, 
        ID_DIALOG_TRANS_BUTTONCATEGS, _("Select Category"), 
        wxDefaultPosition, wxSize(200, -1), 0 );
    itemBoxSizer18->Add(bCategory_, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxALL, 5);
    bCategory_->SetToolTip(_("Specify the category for this transaction"));

    cSplit_ = new wxCheckBox( itemPanel7, 
        ID_DIALOG_TRANS_SPLITCHECKBOX, _("Split"), wxDefaultPosition, wxDefaultSize, wxCHK_2STATE );
    cSplit_->SetValue(FALSE);
    itemBoxSizer18->Add(cSplit_, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

    ////////////////////////////////////////////

    wxStaticText* itemStaticText51 = new wxStaticText( itemPanel7, wxID_STATIC, 
        _("Status"), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer8->Add(itemStaticText51, 0, 
        wxALIGN_CENTER_VERTICAL|wxALL|wxADJUST_MINSIZE, 5);

    wxString itemChoice7Strings[] = 
    {
        _("None"),
        _("Reconciled"),
        _("Void"),
        _("Follow up"),
		_("Duplicate")
    };  
    
    choiceStatus_ = new wxChoice( itemPanel7, ID_DIALOG_TRANS_STATUS, wxDefaultPosition, 
        wxSize(100, -1), 5, itemChoice7Strings, 0 );
    itemFlexGridSizer8->Add(choiceStatus_, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);
    choiceStatus_->SetSelection(0);
    choiceStatus_->SetToolTip(_("Specify the status for the transaction"));

    wxStaticText* itemStaticText21 = new wxStaticText( itemPanel7,
        wxID_STATIC, _("Notes"), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer8->Add(itemStaticText21, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxALL|wxADJUST_MINSIZE, 5);

    textNotes_ = new wxTextCtrl( itemPanel7, 
        ID_DIALOG_TRANS_TEXTNOTES, wxT(""), wxDefaultPosition, wxSize(200, -1), 0 );
    itemFlexGridSizer8->Add(textNotes_, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxALL, 5);
    textNotes_->SetToolTip(_("Specify any text notes you want to add to this transaction."));

    wxStaticText* itemStaticText23 = new wxStaticText( itemPanel7, wxID_STATIC, 
        _("Amount"), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer8->Add(itemStaticText23, 0, 
        wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxALL|wxADJUST_MINSIZE, 5);

    textAmount_ = new wxTextCtrl( itemPanel7, ID_DIALOG_TRANS_TEXTAMOUNT, 
        wxT(""), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer8->Add(textAmount_, 0, 
        wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxALL, 5);
    textAmount_->SetToolTip(_("Specify the amount for this transaction"));

    wxPanel* itemPanel25 = new wxPanel( itemDialog1, wxID_ANY, 
        wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
    itemBoxSizer3->Add(itemPanel25, 0, wxALIGN_RIGHT|wxALL, 5);

    wxBoxSizer* itemBoxSizer26 = new wxBoxSizer(wxHORIZONTAL);
    itemPanel25->SetSizer(itemBoxSizer26);

    wxButton* itemButton27 = new wxButton( itemPanel25, 
        ID_DIALOG_TRANS_BUTTON_OK, _("OK"), wxDefaultPosition, wxDefaultSize, 0 );
    itemBoxSizer26->Add(itemButton27, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

    wxButton* itemButton28 = new wxButton( itemPanel25, 
        wxID_CANCEL, _("Cancel"), wxDefaultPosition, wxDefaultSize, 0 );
    itemBoxSizer26->Add(itemButton28, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);
}

void mmTransDialog::OnCancel(wxCommandEvent& event)
{
    Close(TRUE);
}

void mmTransDialog::OnPayee(wxCommandEvent& event)
{
    bool selectPayees = true;
    if (choiceTrans_->GetSelection() == DEF_TRANSFER)
        selectPayees = false;

    mmPayeeDialog *dlg = new mmPayeeDialog(core_, selectPayees, this);    
    if ( dlg->ShowModal() == wxID_OK )
    {
        if (selectPayees)
        {
            payeeID_ = dlg->payeeID_;
            if (payeeID_ == -1)
            {
                bPayee_->SetLabel(wxT("Select Payee"));
                return;
            }
            int tempCategID = -1;
            int tempSubCategID = -1;
            wxString payeeName = mmDBWrapper::getPayee(db_, 
                payeeID_, tempCategID, tempSubCategID);
            bPayee_->SetLabel(mmReadyDisplayString(payeeName));

            if (tempCategID == -1)
            {
                return;
            }

            wxString catName = mmDBWrapper::getCategoryName(db_, tempCategID);
            wxString categString = catName;

            if (tempSubCategID != -1)
            {
                wxString subcatName = mmDBWrapper::getSubCategoryName(db_,
                    tempCategID, tempSubCategID);
                categString += wxT(" : ");
                categString += subcatName;
            }

            categID_ = tempCategID;
            subcategID_ = tempSubCategID;
            bCategory_->SetLabel(categString);
        }
        else
        {

            if (dlg->payeeID_ == -1)
                return;
            payeeID_ = dlg->payeeID_;
            wxString acctName = mmDBWrapper::getAccountName(db_, payeeID_);
            bPayee_->SetLabel(acctName);

        }
    }
    else
    {
        wxString payeeName = mmDBWrapper::getPayee(db_, 
            payeeID_, categID_, subcategID_);
        if (payeeName.IsEmpty())
        {
            payeeID_ = -1;
            categID_ = -1;
            subcategID_ = -1;
            bCategory_->SetLabel(_("Select Category"));
            bPayee_->SetLabel(_("Select Payee"));
        }
        else
        {
            bPayee_->SetLabel(payeeName);
        }
        
    }

    dlg->Destroy();
}


void mmTransDialog::OnTo(wxCommandEvent& event)
{
    bool selectPayees = true;
     if (choiceTrans_->GetSelection() == DEF_TRANSFER)
        selectPayees = false;
    
    mmPayeeDialog *dlg = new mmPayeeDialog(core_, selectPayees, this);    
    if ( dlg->ShowModal() == wxID_OK )
    {
        if (dlg->payeeID_ == -1)
            return;
        toID_ = dlg->payeeID_;
        wxString acctName = mmDBWrapper::getAccountName(db_, toID_);
        bTo_->SetLabel(acctName);
    }
    else
    {
        int categID, subcategID;
        wxString toName = mmDBWrapper::getPayee(db_, toID_, categID, subcategID);
        if (toName.IsEmpty())
        {
            toID_ = -1;
            bCategory_->SetLabel(_("Select Category"));
            bTo_->SetLabel(_("Select To"));
        }
        else
        {
            bTo_->SetLabel(toName);
        }
    }
   
    dlg->Destroy();
}

void mmTransDialog::OnDateChanged(wxDateEvent& event)
{
    
}

void mmTransDialog::OnAdvanced(wxCommandEvent& event)
{
    wxString dispString = textAmount_->GetValue();
    if (toTransAmount_ > 0.0)
    {
        mmCurrencyFormatter::formatDoubleToCurrencyEdit(toTransAmount_, dispString);
    }
    wxTextEntryDialog* dlg = new wxTextEntryDialog(this, _("To Account Amount Entry"), 
        _("Amount to be recorded in To Account"),  dispString);
    if ( dlg->ShowModal() == wxID_OK )
    {
        wxString currText = dlg->GetValue().Trim();
        if (!currText.IsEmpty())
        {
            double amount;
            if (!mmCurrencyFormatter::formatCurrencyToDouble(currText, amount) 
                 || (amount < 0.0))
            {
                mmShowErrorMessage(this, _("Invalid To Amount Entered "), _("Error"));
            }
            else
            {
                toTransAmount_ = amount;
                advancedToTransAmountSet_ = true;
            }
        }
    }
    dlg->Destroy();
}

void mmTransDialog::OnCategs(wxCommandEvent& event)
{
    if (cSplit_->GetValue())
    {
        SplitTransactionDialog* dlg = new SplitTransactionDialog(core_, split_.get(), this);
        if (dlg->ShowModal() == wxID_OK)
        {
            wxString dispAmount;
            mmCurrencyFormatter::formatDoubleToCurrencyEdit(split_->getTotalSplits(), dispAmount);
            textAmount_->SetValue(dispAmount);
        }
        dlg->Destroy();
    }
    else
    {
        mmCategDialog *dlg = new mmCategDialog(core_, this);
        if ( dlg->ShowModal() == wxID_OK )
        {
            if (dlg->categID_ == -1)
            {
                // check if categ and subcateg are now invalid
                wxString catName = mmDBWrapper::getCategoryName(db_, categID_);
                if ( catName.IsEmpty())
                {
                    // cannot find category
                    categID_ = -1;
                    subcategID_ = -1;
                    bCategory_->SetLabel(_("Select Category"));
                    return;
                }

                if (dlg->subcategID_ != -1)
                {
                    wxString subcatName = mmDBWrapper::getSubCategoryName(db_,
                        categID_, subcategID_);
                    if (subcatName.IsEmpty())
                    {
                        subcategID_ = -1;
                        bCategory_->SetLabel(catName);
                        return;
                    }
                }
                else
                {
                    catName.Replace(wxT("&"), wxT("&&"));
                    bCategory_->SetLabel(catName);
                }

                return;
            }

            categID_ = dlg->categID_;
            subcategID_ = dlg->subcategID_;

            wxString catName = mmDBWrapper::getCategoryName(db_, dlg->categID_);
            catName.Replace (wxT("&"), wxT("&&"));
            wxString categString = catName;

            if (dlg->subcategID_ != -1)
            {
                wxString subcatName = mmDBWrapper::getSubCategoryName(db_,
                    dlg->categID_, dlg->subcategID_);
                subcatName.Replace (wxT("&"), wxT("&&"));
                categString += wxT(" : ");
                categString += subcatName;
            }

            bCategory_->SetLabel(categString);
        }
        dlg->Destroy();
    }
}

void mmTransDialog::OnTransTypeChanged(wxCommandEvent& event)
{
    updateControlsForTransType();
}

void mmTransDialog::updateControlsForTransType()
{
    wxStaticText* st = (wxStaticText*)FindWindow(ID_DIALOG_TRANS_STATIC_FROM);
    wxStaticText* stp = (wxStaticText*)FindWindow(ID_DIALOG_TRANS_STATIC_PAYEE);
    
     if (choiceTrans_->GetSelection() == DEF_WITHDRAWAL)
     {
        
        fillControls();
        st->Show(false);
        bTo_->Show(false);
        stp->SetLabel(_("Payee"));

        bPayee_->SetLabel(_("Select Payee"));
        payeeID_ = -1;
        toID_    = -1;
       
        bAdvanced_->Enable(false);
        bPayee_->SetToolTip(_("Specify to whom the transaction is going to or coming from "));
     }
     else if (choiceTrans_->GetSelection() == DEF_DEPOSIT)
    {
       
        fillControls();
        bTo_->Show(false);
        st->Show(false);    

        stp->SetLabel(_("From"));

        bPayee_->SetLabel(_("Select Payee"));
        payeeID_ = -1;
        toID_    = -1;
        bAdvanced_->Enable(false);
        bPayee_->SetToolTip(_("Specify to whom the transaction is going to or coming from "));
    }
    else if (choiceTrans_->GetSelection() == DEF_TRANSFER)
    {
        bPayee_->SetLabel(_("Select From Account"));
        bTo_->SetLabel(_("Select To Account"));
        payeeID_ = -1;
        toID_    = -1;
        
        bTo_->Show(true);
        st->Show(true);
        stp->SetLabel(_("From"));   
        bAdvanced_->Enable(true);

        bPayee_->SetToolTip(_("Specify the account from which the transfer is occurring"));
    }
}

void mmTransDialog::OnOk(wxCommandEvent& event)
{
    wxString transCode = wxT("");
    int tCode = choiceTrans_->GetSelection();
    if (tCode == DEF_WITHDRAWAL)
       transCode = wxT("Withdrawal");
    else if (tCode == DEF_DEPOSIT)
        transCode = wxT("Deposit");
    else if (tCode == DEF_TRANSFER)
        transCode = wxT("Transfer");

    if (payeeID_ == -1)
    {
        if (transCode != wxT("Transfer"))
            mmShowErrorMessageInvalid(this, _("Payee "));
        else
            mmShowErrorMessageInvalid(this, _("From Account "));
        return;
    }

    if (cSplit_->GetValue())
    {
        if (split_->numEntries() == 0)
        {
            mmShowErrorMessageInvalid(this, _("Category "));
            return;
        }
    }
    else
    {
        if (categID_ == -1)
        {
            mmShowErrorMessageInvalid(this, _("Category "));
            return;
        }
    }

    double amount;
    if (cSplit_->GetValue())
    {
        amount = split_->getTotalSplits();
        if (amount < 0.0)
        {
            mmShowErrorMessage(this, _("Invalid Amount Entered "), _("Error"));
            return;
        }
    }
    else
    {
        wxString amountStr = textAmount_->GetValue().Trim();
        if (!mmCurrencyFormatter::formatCurrencyToDouble(amountStr, amount) 
            || (amount < 0.0))
        {
            mmShowErrorMessage(this, _("Invalid Amount Entered "), _("Error"));
            return;
        }
    }

    int toAccountID = -1;
    int fromAccountID = accountID_;
    if (transCode == wxT("Transfer"))
    {
        if (toID_ == -1)
        {
            mmShowErrorMessageInvalid(this, _("To Account "));
            return;
        }
        
        if (payeeID_ == toID_)
        {
            mmShowErrorMessage(this, _("From and To Account cannot be the same."), _("Error"));
            return;
        }

       fromAccountID = payeeID_;
       toAccountID = toID_;
       payeeID_ = -1;
    }
    else
    {
        int catID, subcatID;
        wxString payeeName = mmDBWrapper::getPayee(db_, payeeID_, catID, subcatID );
        mmDBWrapper::updatePayee(db_, 
            mmCleanString(payeeName), payeeID_, categID_, subcategID_);
    }


    if (!advancedToTransAmountSet_ || toTransAmount_ < 0)
    {
        // if we are adding a new record and the user did not touch advanced dialog
        // we are going to use the transfer amount by calculating conversion rate.
        // subsequent edits will not allow automatic update of the amount
        if (!edit_)
        {
            double rateFrom = mmDBWrapper::getCurrencyBaseConvRate(db_, fromAccountID);
            double rateTo = mmDBWrapper::getCurrencyBaseConvRate(db_, toAccountID);

            double convToBaseFrom = rateFrom * amount;
            toTransAmount_ = convToBaseFrom / rateTo;
        }
        else
        {
            // to trans amount not set
            toTransAmount_ = amount;
        }
    }
    
    wxString transNum = mmCleanString(textNumber_->GetValue());
    wxString notes = mmCleanString(textNotes_->GetValue());
    wxString status = wxT(""); // nothing yet

    if (choiceStatus_->GetSelection() == DEF_STATUS_NONE)
    {
        status = wxT(""); // nothing yet
    }
    else if (choiceStatus_->GetSelection() == DEF_STATUS_RECONCILED)
    {
        status = wxT("R"); 
    }
    else if (choiceStatus_->GetSelection() == DEF_STATUS_VOID)
    {
        status = wxT("V"); 
    }
    else if (choiceStatus_->GetSelection() == DEF_STATUS_FOLLOWUP)
    {
        status = wxT("F"); 
    }
	else if (choiceStatus_->GetSelection() == DEF_STATUS_DUPLICATE)
    {
        status = wxT("D"); 
    }
        
    
    wxString date1 = dpc_->GetValue().FormatISODate();

    boost::shared_ptr<mmBankTransaction> pTransaction;
    if (!edit_)
    {
       boost::shared_ptr<mmBankTransaction> pTemp(new mmBankTransaction(core_->db_));
       pTransaction = pTemp;
    }
    else
    {
       pTransaction = core_->bTransactionList_.getBankTransactionPtr(accountID_, transID_);
    }

    boost::shared_ptr<mmCurrency> pCurrencyPtr = core_->accountList_.getCurrencyWeakPtr(fromAccountID).lock();
    wxASSERT(pCurrencyPtr);
     
    pTransaction->accountID_ = fromAccountID;
    pTransaction->toAccountID_ = toAccountID;
    pTransaction->payee_ = core_->payeeList_.getPayeeSharedPtr(payeeID_);
    pTransaction->transType_ = transCode;
    pTransaction->amt_ = amount;
    pTransaction->status_ = status;
    pTransaction->transNum_ = transNum;
    pTransaction->notes_ = mmCleanString(notes.c_str());
    pTransaction->category_ = core_->categoryList_.getCategorySharedPtr(categID_, subcategID_);
    pTransaction->date_ = dpc_->GetValue();
    pTransaction->toAmt_ = toTransAmount_;
 
    *pTransaction->splitEntries_.get() = *split_.get();
    pTransaction->updateAllData(core_, fromAccountID, pCurrencyPtr, true);
 
    if (!edit_)
    {
       core_->bTransactionList_.addTransaction(pTransaction);
       mmPlayTransactionSound(inidb_);
    }
    else
    {
       core_->bTransactionList_.updateTransaction(pTransaction);
    }
    

    EndModal(wxID_OK);
}

void mmTransDialog::OnSplitChecked(wxCommandEvent& event)
{
  bool state = cSplit_->GetValue();
  if (state)
  {
    bCategory_->SetLabel(_("Split Category"));
    textAmount_->Enable(false);
    wxString dispAmount;
    mmCurrencyFormatter::formatDoubleToCurrencyEdit(split_->getTotalSplits(), dispAmount);
    textAmount_->SetValue(dispAmount);
  }
  else
  {
    bCategory_->SetLabel(_("Select Category"));
    textAmount_->Enable(true);
    wxString dispAmount;
    mmCurrencyFormatter::formatDoubleToCurrencyEdit(0.0, dispAmount);
    textAmount_->SetValue(dispAmount);
  }
}