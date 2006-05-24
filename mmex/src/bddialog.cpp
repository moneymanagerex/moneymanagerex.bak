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

#include "bddialog.h"
#include "wx/datectrl.h"
#include "categdialog.h"
#include "payeedialog.h"
#include "util.h"
#include "dbwrapper.h"

IMPLEMENT_DYNAMIC_CLASS( mmBDDialog, wxDialog )

BEGIN_EVENT_TABLE( mmBDDialog, wxDialog )
    EVT_BUTTON(ID_DIALOG_TRANS_BUTTON_OK, mmBDDialog::OnOk)
    EVT_BUTTON(ID_DIALOG_TRANS_BUTTON_CANCEL, mmBDDialog::OnCancel)
    EVT_BUTTON(ID_DIALOG_TRANS_BUTTONCATEGS, mmBDDialog::OnCategs)
    EVT_BUTTON(ID_DIALOG_TRANS_BUTTONPAYEE, mmBDDialog::OnPayee)
    EVT_BUTTON(ID_DIALOG_TRANS_BUTTONTO, mmBDDialog::OnTo)
    EVT_CHOICE(ID_DIALOG_TRANS_TYPE, mmBDDialog::OnTransTypeChanged)  
    EVT_DATE_CHANGED(ID_DIALOG_TRANS_BUTTONDATE, mmBDDialog::OnDateChanged) 
    EVT_BUTTON(ID_DIALOG_TRANS_BUTTONADVANCED, mmBDDialog::OnAdvanced)
    EVT_BUTTON(ID_DIALOG_BD_COMBOBOX_ACCOUNTNAME, mmBDDialog::OnAccountName)
END_EVENT_TABLE()

// Defines for Transaction Type
#define DEF_WITHDRAWAL 0
#define DEF_DEPOSIT    1
#define DEF_TRANSFER   2

// Defines for Transaction Status
#define DEF_STATUS_NONE       0
#define DEF_STATUS_RECONCILED 1
#define DEF_STATUS_VOID       2
#define DEF_STATUS_FOLLOWUP   3


mmBDDialog::mmBDDialog( )
{
}

mmBDDialog::mmBDDialog(wxSQLite3Database* db, int bdID, bool edit, bool enterOccur,
                       wxWindow* parent, wxWindowID id, const wxString& caption, 
                       const wxPoint& pos, const wxSize& size, long style )
{
    db_ = db;
    bdID_ = bdID;
    edit_ = edit;
    categID_ = -1;
    subcategID_ = -1;
    payeeID_ = -1;
    accountID_ = -1;
    toID_ = -1;
    toTransAmount_ = -1;
    enterOccur_ = enterOccur;

    Create(parent, id, caption, pos, size, style);
}

bool mmBDDialog::Create( wxWindow* parent, wxWindowID id, const wxString& caption, 
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

    if (edit_ || enterOccur_)
    {
        dataToControls();
    }

    Centre();
    return TRUE;
}

void mmBDDialog::dataToControls()
{
    choiceTrans_->Disable();
    if (enterOccur_)
    {
        dpcbd_->Disable();
        itemRepeats_->Disable();
        itemAccountName_->Disable();
    }

    mmBEGINSQL_LITE_EXCEPTION;

    wxSQLite3StatementBuffer bufSQL;
    bufSQL.Format("select * from BILLSDEPOSITS_V1 where BDID=%d;", bdID_);
    wxSQLite3ResultSet q1 = db_->ExecuteQuery(bufSQL);
    if (q1.NextRow())
    {
        categID_ = q1.GetInt(wxT("CATEGID"));
        subcategID_ = q1.GetInt(wxT("SUBCATEGID"));

        wxString transNumString = q1.GetString(wxT("TRANSACTIONNUMBER"));
        wxString statusString  = q1.GetString(wxT("STATUS"));
        wxString notesString  = q1.GetString(wxT("NOTES"));
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

        wxString nextOccurrString = q1.GetString(wxT("NEXTOCCURRENCEDATE"));
        wxString numRepeatStr  = q1.GetString(wxT("NUMOCCURRENCES"));
        if (numRepeatStr != wxT("-1"))
            textNumRepeats_->SetValue(numRepeatStr);

        wxDateTime dtno = mmGetStorageStringAsDate(nextOccurrString);
        wxString dtnostr = mmGetDateForDisplay(db_, dtno);
        dpcbd_->SetValue(dtno);
        dpc_->SetValue(dtno);

        int repeatSel = q1.GetInt(wxT("REPEATS"));
        itemRepeats_->SetSelection(repeatSel);
        if (repeatSel == 0) // if none
            textNumRepeats_->SetValue(wxT(""));
        
        if (transTypeString == wxT("Withdrawal"))
            choiceTrans_->SetSelection(DEF_WITHDRAWAL);
        else if (transTypeString == wxT("Deposit"))
            choiceTrans_->SetSelection(DEF_DEPOSIT);
        else if (transTypeString == wxT("Transfer"))
            choiceTrans_->SetSelection(DEF_TRANSFER);
        updateControlsForTransType();

        payeeID_ = q1.GetInt(wxT("PAYEEID"));
        toID_ = q1.GetInt(wxT("TOACCOUNTID"));
        accountID_ = q1.GetInt(wxT("ACCOUNTID"));
        wxString accountName = mmDBWrapper::getAccountName(db_, accountID_);
        itemAccountName_->SetLabel(accountName);

        q1.Finalize();

        wxString payeeString;
        bufSQL.Format("select * from PAYEE_V1 where PAYEEID=%d;", payeeID_);
        q1 = db_->ExecuteQuery(bufSQL);
        if (q1.NextRow())
        {
            payeeString = q1.GetString(wxT("PAYEENAME"));
            q1.Finalize();
        }

        wxString catName = mmDBWrapper::getCategoryName(db_, categID_);
        wxString categString = catName;

        if (subcategID_ != -1)
        {
            wxString subcatName = mmDBWrapper::getSubCategoryName(db_, categID_, subcategID_);
            categString += wxT(" : ");
            categString += subcatName;
        }

        bCategory_->SetLabel(categString);

        textNotes_->SetValue(notesString);
        textNumber_->SetValue(transNumString);

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

void mmBDDialog::fillControls()
{
    
}

void mmBDDialog::CreateControls()
{    
    mmBDDialog* itemDialog1 = this;

    wxBoxSizer* itemBoxSizer2 = new wxBoxSizer(wxHORIZONTAL);
    itemDialog1->SetSizer(itemBoxSizer2);

    wxBoxSizer* itemBoxSizer3 = new wxBoxSizer(wxVERTICAL);
    itemBoxSizer2->Add(itemBoxSizer3, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

    /* Bills & Deposits Details */
    wxStaticBox* itemStaticBoxSizer4Static = new wxStaticBox(itemDialog1, wxID_ANY, 
         _("Bills && Deposit Details"));
    wxStaticBoxSizer* itemStaticBoxSizer4 = new wxStaticBoxSizer(itemStaticBoxSizer4Static, 
        wxHORIZONTAL);
    itemBoxSizer3->Add(itemStaticBoxSizer4, 0, wxALIGN_LEFT|wxALL, 5);

    wxFlexGridSizer* itemFlexGridSizer5 = new wxFlexGridSizer(2, 4, 0, 0);
    itemStaticBoxSizer4->Add(itemFlexGridSizer5, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

    wxStaticText* itemStaticText6 = new wxStaticText( itemDialog1, wxID_STATIC, 
        _("Account Name"), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer5->Add(itemStaticText6, 0, 
        wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL|wxALL|wxADJUST_MINSIZE, 5);

    itemAccountName_ = new wxButton( itemDialog1, ID_DIALOG_BD_COMBOBOX_ACCOUNTNAME, 
        _("Select Account"), wxDefaultPosition, wxSize(100, -1), 0 );
    itemFlexGridSizer5->Add(itemAccountName_, 0, 
        wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    wxStaticText* itemStaticText8 = new wxStaticText( itemDialog1, 
        wxID_STATIC, _("Next Occurence"), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer5->Add(itemStaticText8, 0, 
        wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL|wxALL|wxADJUST_MINSIZE, 5);

    dpcbd_ = new wxDatePickerCtrl( itemDialog1, ID_DIALOG_BD_BUTTON_NEXTOCCUR, wxDefaultDateTime, 
              wxDefaultPosition, wxDefaultSize, wxDP_DROPDOWN);
    itemFlexGridSizer5->Add(dpcbd_, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxALL, 5);
    dpcbd_->SetToolTip(_("Specify the date of the next bill or deposit"));

    wxStaticText* itemStaticText10 = new wxStaticText( itemDialog1, wxID_STATIC, 
        _("Repeats"), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer5->Add(itemStaticText10, 0, 
        wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxALL|wxADJUST_MINSIZE, 5);
    
    wxString itemComboBox11Strings[] = 
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
    itemRepeats_ = new wxChoice( itemDialog1, ID_DIALOG_BD_COMBOBOX_REPEATS, wxDefaultPosition, 
        wxSize(100, -1), 8, itemComboBox11Strings, 0);
    itemFlexGridSizer5->Add(itemRepeats_, 0, 
        wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL|wxALL, 5);
    itemRepeats_->SetSelection(0);

    wxStaticText* itemStaticText231 = new wxStaticText( itemDialog1, wxID_STATIC, 
        _("Times Repeated"), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer5->Add(itemStaticText231, 0, 
        wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxALL|wxADJUST_MINSIZE, 5);

    textNumRepeats_ = new wxTextCtrl( itemDialog1, ID_DIALOG_BD_TEXTCTRL_NUM_TIMES, wxT(""), 
        wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer5->Add(textNumRepeats_, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxALL, 5);
    textNumRepeats_->SetToolTip(_("Specify the number of times this series repeats. Leave blank if this series continues forever."));

    /* Transaction Details */
    wxStaticBox* itemStaticBoxSizer14Static = new wxStaticBox(itemDialog1, 
        wxID_ANY, _("Transaction Details"));
    wxStaticBoxSizer* itemStaticBoxSizer14 = new wxStaticBoxSizer(itemStaticBoxSizer14Static, 
        wxVERTICAL);
    itemBoxSizer3->Add(itemStaticBoxSizer14, 0, wxALIGN_CENTER_HORIZONTAL|wxALL, 5);

    wxBoxSizer* itemBoxSizer4 = new wxBoxSizer(wxHORIZONTAL);
    itemStaticBoxSizer14->Add(itemBoxSizer4, 0, wxALIGN_LEFT|wxALL, 5);

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
    choiceTrans_->SetSelection(DEF_WITHDRAWAL);
    choiceTrans_->SetToolTip(_("Specify the type of transactions to be created."));

    bAdvanced_ = new wxButton( itemDialog1, ID_DIALOG_TRANS_BUTTONADVANCED, _("Advanced"), 
        wxDefaultPosition, wxSize(100, -1), 0 );
    bAdvanced_->Enable(false);
    itemBoxSizer4->Add(bAdvanced_, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxALL, 5);
    bAdvanced_->SetToolTip(_("Specify advanced settings for transaction"));

    wxPanel* itemPanel7 = new wxPanel( itemDialog1, wxID_ANY, wxDefaultPosition, 
        wxDefaultSize, wxTAB_TRAVERSAL );
    itemStaticBoxSizer14->Add(itemPanel7, 1, wxGROW|wxALL, 5);

    wxFlexGridSizer* itemFlexGridSizer8 = new wxFlexGridSizer(4, 4, 0, 0);
    itemPanel7->SetSizer(itemFlexGridSizer8);

    wxStaticText* itemStaticText9 = new wxStaticText( itemPanel7, ID_DIALOG_TRANS_STATIC_PAYEE, 
        _("Payee"), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer8->Add(itemStaticText9, 0, 
        wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxALL|wxADJUST_MINSIZE, 5);

    bPayee_ = new wxButton( itemPanel7, ID_DIALOG_TRANS_BUTTONPAYEE, _("Select Payee"), 
        wxDefaultPosition, wxSize(200, -1), 0 );
    itemFlexGridSizer8->Add(bPayee_, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxALL, 5);
    bPayee_->SetToolTip(_("Specify where the transaction is going to or coming from "));
    
    wxStaticText* itemStaticText11 = new wxStaticText( itemPanel7, wxID_STATIC, _("Number"), 
        wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer8->Add(itemStaticText11, 0, 
        wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxALL|wxADJUST_MINSIZE, 5);

    textNumber_ = new wxTextCtrl( itemPanel7, ID_DIALOG_TRANS_TEXTNUMBER, wxT(""), 
        wxDefaultPosition, wxSize(100, -1), 0 );
    itemFlexGridSizer8->Add(textNumber_, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxALL, 5);
    textNumber_->SetToolTip(_("Specify any associated check number or transaction number"));

    wxStaticText* itemStaticText13 = new wxStaticText( itemPanel7, ID_DIALOG_TRANS_STATIC_FROM, 
        _("To"), wxDefaultPosition, wxDefaultSize, 0 );
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
        wxDefaultPosition, wxDefaultSize, wxDP_DROPDOWN);
    itemFlexGridSizer8->Add(dpc_, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL|wxALL, 5);
    dpc_->SetToolTip(_("Specify the date of the transaction"));
    if (!edit_)
        dpc_->Enable(false);

    wxStaticText* itemStaticText17 = new wxStaticText( itemPanel7, wxID_STATIC, 
        _("Category"), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer8->Add(itemStaticText17, 0, 
        wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxALL|wxADJUST_MINSIZE, 5);

    bCategory_ = new wxButton( itemPanel7, ID_DIALOG_TRANS_BUTTONCATEGS, _("Select Category"), 
        wxDefaultPosition, wxSize(200, -1), 0 );
    itemFlexGridSizer8->Add(bCategory_, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxALL, 5);
    bCategory_->SetToolTip(_("Specify the category for this transaction"));

    wxStaticText* itemStaticText51 = new wxStaticText( itemPanel7, wxID_STATIC, _("Status"), 
        wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer8->Add(itemStaticText51, 0, wxALIGN_CENTER_VERTICAL|wxALL|wxADJUST_MINSIZE, 5);

    wxString itemChoice7Strings[] = 
    {
        _("None"),
        _("Reconciled"),
        _("Void"),
        _("Follow up")
    };  
    
    choiceStatus_ = new wxChoice( itemPanel7, ID_DIALOG_TRANS_STATUS, wxDefaultPosition, 
        wxSize(100, -1), 4, itemChoice7Strings, 0 );
    itemFlexGridSizer8->Add(choiceStatus_, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);
    choiceStatus_->SetSelection(DEF_STATUS_NONE);
    choiceStatus_->SetToolTip(_("Specify the status for the transaction"));

    wxStaticText* itemStaticText21 = new wxStaticText( itemPanel7, wxID_STATIC, 
        _("Notes"), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer8->Add(itemStaticText21, 0, 
        wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxALL|wxADJUST_MINSIZE, 5);

    textNotes_ = new wxTextCtrl( itemPanel7, ID_DIALOG_TRANS_TEXTNOTES, wxT(""), 
        wxDefaultPosition, wxSize(200, -1), 0 );
    itemFlexGridSizer8->Add(textNotes_, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxALL, 5);
    textNotes_->SetToolTip(_("Specify any text notes you want to add to this transaction."));

    wxStaticText* itemStaticText23 = new wxStaticText( itemPanel7, wxID_STATIC, 
        _("Amount"), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer8->Add(itemStaticText23, 0, 
        wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxALL|wxADJUST_MINSIZE, 5);

    textAmount_ = new wxTextCtrl( itemPanel7, ID_DIALOG_TRANS_TEXTAMOUNT, wxT(""), 
        wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer8->Add(textAmount_, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxALL, 5);
    textAmount_->SetToolTip(_("Specify the amount for this transaction"));

    wxPanel* itemPanel25 = new wxPanel( itemDialog1, wxID_ANY, wxDefaultPosition, 
        wxDefaultSize, wxTAB_TRAVERSAL );
    itemBoxSizer3->Add(itemPanel25, 0, wxALIGN_RIGHT|wxALL, 5);

    wxBoxSizer* itemBoxSizer26 = new wxBoxSizer(wxHORIZONTAL);
    itemPanel25->SetSizer(itemBoxSizer26);

    wxButton* itemButton27 = new wxButton( itemPanel25, ID_DIALOG_TRANS_BUTTON_OK, 
        _("OK"), wxDefaultPosition, wxDefaultSize, 0 );
    itemBoxSizer26->Add(itemButton27, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

    wxButton* itemButton28 = new wxButton( itemPanel25, 
        ID_DIALOG_TRANS_BUTTON_CANCEL, _("Cancel"), wxDefaultPosition, wxDefaultSize, 0 );
    itemBoxSizer26->Add(itemButton28, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);
}

void mmBDDialog::OnCancel(wxCommandEvent& event)
{
    Close(TRUE);
}

void mmBDDialog::OnAccountName(wxCommandEvent& event)
{
    bool selectPayees = false;
    mmPayeeDialog *dlg = new mmPayeeDialog(db_, selectPayees, this);    
    if ( dlg->ShowModal() == wxID_OK )
    {
        if (dlg->payeeID_ == -1)
            return;
        accountID_ = dlg->payeeID_;
        wxString acctName = mmDBWrapper::getAccountName(db_, accountID_);
        itemAccountName_->SetLabel(acctName);
    }
}

void mmBDDialog::OnPayee(wxCommandEvent& event)
{
    bool selectPayees = true;
    if (choiceTrans_->GetSelection() == DEF_TRANSFER)
        selectPayees = false;

    mmPayeeDialog *dlg = new mmPayeeDialog(db_, selectPayees, this);    
    if ( dlg->ShowModal() == wxID_OK )
    {
        if (selectPayees)
        {
            payeeID_ = dlg->payeeID_;
            if (payeeID_ == -1)
            {
                bPayee_->SetLabel(_("Select Payee"));
                return;
            }
            wxString payeeName = mmDBWrapper::getPayee(db_, payeeID_, categID_, subcategID_);
            bPayee_->SetLabel(mmReadyDisplayString(payeeName));

            if (categID_ == -1)
            {
                subcategID_ = -1;
                bCategory_->SetLabel(_("Select Category"));
                return;
            }

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
        wxString payeeName = mmDBWrapper::getPayee(db_, payeeID_, categID_, subcategID_);
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


void mmBDDialog::OnTo(wxCommandEvent& event)
{
    bool selectPayees = true;
    if (choiceTrans_->GetSelection() == DEF_TRANSFER)
        selectPayees = false;

    mmPayeeDialog *dlg = new mmPayeeDialog(db_, selectPayees, this);    
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

void mmBDDialog::OnDateChanged(wxDateEvent& event)
{
    
}

void mmBDDialog::OnAdvanced(wxCommandEvent& event)
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
            }
        }
    }
    dlg->Destroy();
}

void mmBDDialog::OnCategs(wxCommandEvent& event)
{
    mmCategDialog *dlg = new mmCategDialog(db_, this);
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
            bCategory_->SetLabel(_("Select Category"));
            return;
        }

        if (dlg->subcategID_ != -1)
        {
            wxString subcatName = mmDBWrapper::getSubCategoryName(db_, categID_, subcategID_);
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
    catName.Replace(wxT("&"), wxT("&&"));
    wxString categString = catName;

    if (dlg->subcategID_ != -1)
    {
        wxString subcatName = mmDBWrapper::getSubCategoryName(db_, dlg->categID_, dlg->subcategID_);
        subcatName.Replace(wxT("&"), wxT("&&"));
        categString += wxT(" : ");
        categString += subcatName;
    }

    bCategory_->SetLabel(categString);
}

void mmBDDialog::OnTransTypeChanged(wxCommandEvent& event)
{
    updateControlsForTransType();
}

void mmBDDialog::updateControlsForTransType()
{
    wxStaticText* st = (wxStaticText*)FindWindow(ID_DIALOG_TRANS_STATIC_FROM);
    wxStaticText* stp = (wxStaticText*)FindWindow(ID_DIALOG_TRANS_STATIC_PAYEE);
    
     if (choiceTrans_->GetSelection() == DEF_WITHDRAWAL)
     {
        bPayee_->SetLabel(_("Select Payee"));
        fillControls();
        st->Show(false);
        bTo_->Show(false);
        stp->SetLabel(_("Payee"));
        payeeID_ = -1;
        toID_    = -1;
        bAdvanced_->Enable(false);
     }
     else if (choiceTrans_->GetSelection() == DEF_DEPOSIT)
    {
        bPayee_->SetLabel(_("Select Payee"));
        fillControls();
        bTo_->Show(false);
        st->Show(false);    
        stp->SetLabel(_("From"));
        payeeID_ = -1;
        toID_    = -1;
        bAdvanced_->Enable(false);
    }
    else if (choiceTrans_->GetSelection() == DEF_TRANSFER)
    {
        bPayee_->SetLabel(_("Select From"));
        bTo_->SetLabel(_("Select To"));
        payeeID_ = -1;
        toID_    = -1;
        
        bTo_->Show(true);
        st->Show(true);
        stp->SetLabel(_("From"));   
        bAdvanced_->Enable(true);
    }
}

void mmBDDialog::OnOk(wxCommandEvent& event)
{
    mmBEGINSQL_LITE_EXCEPTION;
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

    if ((transCode != wxT("Transfer")) && (accountID_ == -1))
    {
        mmShowErrorMessageInvalid(this, _("Account"));
        return;
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
            payeeName, payeeID_, categID_, subcategID_);
    }

    wxString amountStr = textAmount_->GetValue().Trim();
    double amount;
    if (!mmCurrencyFormatter::formatCurrencyToDouble(amountStr, amount) 
        || (amount < 0.0))
    {
        mmShowErrorMessage(this, _("Invalid Amount Entered "), _("Error"));
        return;
    }

    if (toTransAmount_ < 0)
    {
        // to trans amount not set
        toTransAmount_ = amount;
    }

    
    if (categID_ == -1)
    {
        mmShowErrorMessageInvalid(this, _("Category "));
        return;
    }

    wxString transNum = mmCleanString(textNumber_->GetValue());
    wxString notes = mmCleanString(textNotes_->GetValue());
    wxString status = wxT(""); // nothing yet
    int repeats = itemRepeats_->GetSelection();
    wxString numRepeatStr = textNumRepeats_->GetValue();
    long numRepeats = -1;
    if (numRepeatStr != wxT(""))
        numRepeatStr.ToLong(&numRepeats);
    wxString nextOccurDate = dpcbd_->GetValue().FormatISODate();

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
    
    wxString date1 = dpc_->GetValue().FormatISODate();

    if (!edit_ && !enterOccur_)
    {
        wxString bufSQL = wxString::Format(wxT("insert into BILLSDEPOSITS_V1 (ACCOUNTID, TOACCOUNTID, PAYEEID, TRANSCODE, \
                      TRANSAMOUNT, STATUS, TRANSACTIONNUMBER, NOTES,                               \
                      CATEGID, SUBCATEGID, TRANSDATE, FOLLOWUPID, TOTRANSAMOUNT, REPEATS, NEXTOCCURRENCEDATE, NUMOCCURRENCES)                                              \
                      values (%d, %d, %d, '%s', %f, '%s', '%s', '%s', %d, %d, '%s', -1, %f, %d, '%s', %d);"),
                      fromAccountID, toAccountID, payeeID_, transCode.c_str(), amount,
                      status.c_str(), transNum.c_str(), notes.c_str(), categID_, subcategID_, 
                      date1.c_str(), toTransAmount_, repeats, nextOccurDate.c_str(), numRepeats);  

        int retVal = db_->ExecuteUpdate(bufSQL);
        
    }
    else if (edit_)
    {
        wxString bufSQL = wxString::Format(wxT("update BILLSDEPOSITS_V1 SET ACCOUNTID=%d, TOACCOUNTID=%d, PAYEEID=%d, TRANSCODE='%s', \
                      TRANSAMOUNT=%f, STATUS='%s', TRANSACTIONNUMBER='%s', NOTES='%s',                               \
                      CATEGID=%d, SUBCATEGID=%d, TRANSDATE='%s', TOTRANSAMOUNT=%f, REPEATS=%d, NEXTOCCURRENCEDATE='%s', NUMOCCURRENCES=%d WHERE BDID=%d;"),
                      accountID_, toAccountID, payeeID_, transCode.c_str(), amount,
                      status.c_str(), transNum.c_str(), notes.c_str(),categID_, subcategID_, 
                      date1.c_str(), toTransAmount_, repeats, nextOccurDate.c_str(), numRepeats, bdID_);  

        int retVal = db_->ExecuteUpdate(bufSQL);
    }
    else if (enterOccur_)
    {
        wxString bufSQL = wxString::Format(wxT("insert into CHECKINGACCOUNT_V1 (ACCOUNTID, TOACCOUNTID, PAYEEID, TRANSCODE, \
                      TRANSAMOUNT, STATUS, TRANSACTIONNUMBER, NOTES,                               \
                      CATEGID, SUBCATEGID, TRANSDATE, FOLLOWUPID, TOTRANSAMOUNT)                                              \
                      values (%d, %d, %d, '%s', %f, '%s', '%s', '%s', %d, %d, '%s', -1, %f);"),
                      fromAccountID, toAccountID, payeeID_, transCode.c_str(), amount,
                      status.c_str(), transNum.c_str(), notes.c_str(), categID_, subcategID_, date1.c_str(), toTransAmount_ );  

        int retVal = db_->ExecuteUpdate(bufSQL);
        mmDBWrapper::completeBDInSeries(db_, bdID_);

    }
    mmENDSQL_LITE_EXCEPTION;
    EndModal(wxID_OK);
}

