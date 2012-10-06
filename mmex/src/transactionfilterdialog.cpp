/*************************************************************************
 Copyright (C) 2011, 2012 Stefano Giorgio

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
 *************************************************************************/

#include "transactionfilterdialog.h"
#include "paths.h"
#include "util.h"
#include "payeedialog.h"
#include "categdialog.h"

IMPLEMENT_DYNAMIC_CLASS( TransFilterDialog, wxDialog )

BEGIN_EVENT_TABLE( TransFilterDialog, wxDialog )
    EVT_CHECKBOX( wxID_ANY, TransFilterDialog::OnCBChanged)
    EVT_BUTTON( wxID_OK,       TransFilterDialog::OnButtonOK )
END_EVENT_TABLE()

// default constructor
TransFilterDialog::TransFilterDialog()
{
    Init();
}

TransFilterDialog::TransFilterDialog( mmCoreDB* core, wxWindow* parent, wxWindowID id,
    const wxString& caption, const wxPoint& pos, const wxSize& size, long style )
: core_(core)
{
    Create(parent, id, caption, pos, size, style);
    Init();
}

//TransFilterDialog::~TransFilterDialog( )
//{
//    // wxMessageBox(wxT("Testing that dialog being destroyed"), wxT("Transaction Filter Destructor"));
//}

// Initialize our variables
void TransFilterDialog::Init()
{
    dpDateStart_->Enable(false);
    dpDateEnd_->Enable(false);
    btnPayee_->Enable(false);
    btnCategory_->Enable(false);
    choiceStatus_->Enable(false);
    transaction_type_->Enable(false);
    txtTransNumber_->Enable(false);
    txtNotes_->Enable(false);
}
    
// Creation
bool TransFilterDialog::Create( wxWindow* parent, wxWindowID id,
    const wxString& caption, const wxPoint& pos, const wxSize& size, long style)
{
    wxDialog::Create( parent, id, caption, pos, size, style );
    CreateControls();
    Fit();
    SetIcon(mmex::getProgramIcon());
    Centre();

    return TRUE;
}

// Creates the controls and sizers
void TransFilterDialog::CreateControls()
{
    int fieldWidth = 225;
    wxSizerFlags flags;
    flags.Align(wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL).Border(wxALL, 5);

    wxBoxSizer* thisSizer = new wxBoxSizer(wxVERTICAL);
    this->SetSizer(thisSizer);

    wxStaticBox* itemStaticBox = new wxStaticBox(this, wxID_ANY, _(" Filter Settings "));
    wxStaticBoxSizer* itemStaticBoxSizer = new wxStaticBoxSizer(itemStaticBox, wxVERTICAL);
    thisSizer->Add(itemStaticBoxSizer, 0, wxGROW|wxLEFT|wxTOP|wxRIGHT, 10);

    /******************************************************************************
     Items Panel
    *******************************************************************************/
    wxPanel* itemPanel = new wxPanel( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
    itemStaticBoxSizer->Add(itemPanel, 1, wxGROW|wxALL, 5);

    wxFlexGridSizer* itemPanelSizer = new wxFlexGridSizer(0, 2, 0, 0);
    itemPanel->SetSizer(itemPanelSizer);

    //--Start of Row --------------------------------------------------------
    cbDateRange_ = new wxCheckBox( itemPanel, wxID_STATIC, _("Date Range"));
    cbDateRange_->SetValue(FALSE);

    wxBoxSizer* dateSizer = new wxBoxSizer(wxHORIZONTAL);
    dpDateStart_ = new wxDatePickerCtrl( itemPanel, wxID_STATIC, wxDefaultDateTime, 
                                         wxDefaultPosition, wxSize(fieldWidth/2-5,-1), wxDP_DROPDOWN);
    dpDateEnd_   = new wxDatePickerCtrl( itemPanel, wxID_STATIC, wxDefaultDateTime, 
                                         wxDefaultPosition, wxSize(fieldWidth/2-5,-1), wxDP_DROPDOWN);
    dateSizer->Add(dpDateStart_, flags);
    dateSizer->Add(dpDateEnd_,   flags);

    itemPanelSizer->Add(cbDateRange_, flags);
    itemPanelSizer->Add(dateSizer);
    //--End of Row --------------------------------------------------------
    
    cbPayee_ = new wxCheckBox( itemPanel, wxID_STATIC, _("Payee"));
    cbPayee_->SetValue(FALSE);

    btnPayee_ = new wxButton( itemPanel, wxID_STATIC, _("Select Payee"), 
                              wxDefaultPosition, wxSize(fieldWidth,-1));

    itemPanelSizer->Add(cbPayee_, flags);
    itemPanelSizer->Add(btnPayee_, flags);
    //--End of Row --------------------------------------------------------

    cbSplitCategory_ = new wxCheckBox( itemPanel, wxID_STATIC, _("Split Category"));
    cbSplitCategory_->SetValue(FALSE);
    itemPanelSizer->AddSpacer(20);
    itemPanelSizer->Add(cbSplitCategory_, flags);
    //--End of Row --------------------------------------------------------

    cbCategory_ = new wxCheckBox( itemPanel, wxID_STATIC, _("Category"), 
                                  wxDefaultPosition, wxDefaultSize, wxCHK_2STATE );
    cbCategory_->SetValue(FALSE);

    btnCategory_ = new wxButton( itemPanel, wxID_STATIC, _("Select Category"), 
                                 wxDefaultPosition, wxSize(fieldWidth,-1), 0 );

    itemPanelSizer->Add(cbCategory_, flags);
    itemPanelSizer->Add(btnCategory_, flags);
    //--End of Row --------------------------------------------------------

    cbStatus_ = new wxCheckBox( itemPanel, wxID_STATIC, _("Status"), 
                                wxDefaultPosition, wxDefaultSize, wxCHK_2STATE );
    cbStatus_->SetValue(FALSE);

    choiceStatus_ = new wxChoice( itemPanel, wxID_STATIC, wxDefaultPosition, wxSize(fieldWidth,-1));
    wxString transaction_status[] =
    {
        wxTRANSLATE("None"),
        wxTRANSLATE("Reconciled"),
        wxTRANSLATE("Void"),
        wxTRANSLATE("Follow up"),
        wxTRANSLATE("Duplicate")
    };
    for(size_t i = 0; i < sizeof(transaction_status)/sizeof(wxString); ++i)
        choiceStatus_->Append(wxGetTranslation(transaction_status[i]),
        new wxStringClientData(transaction_status[i]));

    choiceStatus_->SetSelection(mmIniOptions::instance().transStatusReconciled_);

    itemPanelSizer->Add(cbStatus_, flags);
    itemPanelSizer->Add(choiceStatus_, flags);
    //--End of Row --------------------------------------------------------

    cbType_ = new wxCheckBox( itemPanel, wxID_STATIC, _("Type"), 
                              wxDefaultPosition, wxDefaultSize, wxCHK_2STATE );
    cbType_->SetValue(FALSE);
    
    transaction_type_ = new wxChoice( itemPanel, wxID_STATIC, wxDefaultPosition, wxSize(fieldWidth,-1));
    wxString transaction_type[] =
    {
        wxTRANSLATE("Withdrawal"),
        wxTRANSLATE("Deposit"),
        wxTRANSLATE("Transfer")
    };
    // Restrict choise if accounts number less than 2
    size_t size = sizeof(transaction_type)/sizeof(wxString);
    if (core_->accountList_.getNumBankAccounts() < 2) size--;
    for(size_t i = 0; i < size; ++i)
    transaction_type_->Append(wxGetTranslation(transaction_type[i]),
        new wxStringClientData(transaction_type[i]));

    transaction_type_->SetSelection(0);

    itemPanelSizer->Add(cbType_, flags);
    itemPanelSizer->Add(transaction_type_, flags);
    //--End of Row --------------------------------------------------------

    cbTransNumber_ = new wxCheckBox( itemPanel, wxID_STATIC, _("Number"),
                                     wxDefaultPosition, wxDefaultSize, wxCHK_2STATE );
    cbTransNumber_->SetValue(FALSE);
    txtTransNumber_ = new wxTextCtrl( itemPanel, wxID_STATIC,  wxT(""), wxDefaultPosition, wxSize(fieldWidth,-1), 0 );

    itemPanelSizer->Add(cbTransNumber_, flags);
    itemPanelSizer->Add(txtTransNumber_, flags);
    //--End of Row --------------------------------------------------------

    cbNotes_ = new wxCheckBox( itemPanel, wxID_STATIC, _("Notes"),
                                     wxDefaultPosition, wxDefaultSize, wxCHK_2STATE );
    cbNotes_->SetValue(FALSE);
    txtNotes_ = new wxTextCtrl( itemPanel, wxID_STATIC, wxT(""),
        wxDefaultPosition, wxSize(fieldWidth,-1));

    itemPanelSizer->Add(cbNotes_, flags);
    itemPanelSizer->Add(txtNotes_, flags);

    /******************************************************************************
     Button Panel with OK/Cancel buttons   
    *******************************************************************************/
    wxPanel* buttonPanel = new wxPanel( this, wxID_STATIC,
        wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
    thisSizer->Add(buttonPanel, flags.Center());

    wxBoxSizer* buttonPanelSizer = new wxBoxSizer(wxHORIZONTAL);
    buttonPanel->SetSizer(buttonPanelSizer);

    wxButton* btnOK = new wxButton( buttonPanel, wxID_OK);
    buttonPanelSizer->Add(btnOK, flags);

    wxButton* btnCancel = new wxButton( buttonPanel, wxID_CANCEL);
    buttonPanelSizer->Add(btnCancel, flags);
    btnCancel->SetFocus();

    btnPayee_->Connect(wxID_ANY, wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(TransFilterDialog::OnPayeeSelect), NULL, this);
    btnCategory_->Connect(wxID_ANY, wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(TransFilterDialog::OnCategorySelect), NULL, this);

    cbSplitCategory_->SetToolTip(_("Filter Split categories. Include the category for further filtering."));
    choiceStatus_->SetToolTip(_("Specify the status for the transaction"));
    transaction_type_->SetToolTip(_("Specify the type of transaction."));
    txtNotes_->SetToolTip(_("Will search for the given text in the Notes field"));
    btnOK->SetToolTip(_("Activates the filter: Will add selected items to display"));
    btnCancel->SetToolTip(_("Deactivates the filter"));

}

void TransFilterDialog::OnButtonOK( wxCommandEvent& /*event*/ )
{
    if (cbDateRange_->GetValue() || cbPayee_->GetValue()  ||
        cbCategory_->GetValue()  || cbStatus_->GetValue() ||
        cbType_->GetValue()      || cbTransNumber_->GetValue() ||
        cbNotes_->GetValue()     || cbSplitCategory_->GetValue() 
       )
    {
        EndModal(wxID_OK);
    } 
    else
    {
        EndModal(wxID_CANCEL);
    }
}

void TransFilterDialog::OnCBChanged( wxCommandEvent& /*event*/ )
{
    dpDateStart_->Enable(cbDateRange_->GetValue());
    dpDateEnd_->Enable(cbDateRange_->GetValue());
    btnPayee_->Enable(cbPayee_->GetValue());
    btnCategory_->Enable(cbCategory_->GetValue());
    choiceStatus_->Enable(cbStatus_->GetValue());
    transaction_type_->Enable(cbType_->GetValue());
    txtTransNumber_->Enable(cbTransNumber_->GetValue());
    txtNotes_->Enable(cbNotes_->GetValue());
}

void TransFilterDialog::OnPayeeSelect(wxCommandEvent& /*event*/)
{
    mmPayeeDialog dlg(this, core_);
    if ( dlg.ShowModal() == wxID_OK )
    {
        if (dlg.getPayeeId() != -1)
        {
            wxString payeeName = core_->payeeList_.GetPayeeName(dlg.getPayeeId());
            btnPayee_->SetLabel(mmReadyDisplayString(payeeName));
        }
    }
}

void TransFilterDialog::OnCategorySelect(wxCommandEvent& /*event*/)
{
    mmCategDialog dlg(core_, this);
    dlg.setTreeSelection( catName_, subCatName_ );
    if (dlg.ShowModal() == wxID_OK)
    {
        //categID_ = dlg.categID_;
        //subcategID_ = dlg.subcategID_;
        catName_ = mmReadyDisplayString(core_->categoryList_.GetCategoryName(dlg.categID_));
        subCatName_ = core_->categoryList_.GetSubCategoryName(dlg.categID_, dlg.subcategID_);    

        btnCategory_->SetLabel(core_->categoryList_.GetFullCategoryString(dlg.categID_, dlg.subcategID_));
    }
}

/****************************************************************************************
Interface Methods
*****************************************************************************************/
bool TransFilterDialog::byDateRange(wxDateTime transDate)
{
    bool result = false;
    if (cbDateRange_->GetValue())
    {
        wxDateTime dtBegin = dpDateStart_->GetValue();
        wxDateTime dtEnd   = dpDateEnd_->GetValue();
        if ((dtBegin == dtEnd) && (dtBegin.IsSameDate(transDate)))
            result = true;
        else if (transDate.IsBetween(dtBegin, dtEnd))
            result = true;
    } 
    else
        result = true;

    return result;
}

bool TransFilterDialog::byPayee(wxString payee)
{
    bool result = false;
    if (cbPayee_->GetValue())
    {
        if (payee == btnPayee_->GetLabelText() )
        {
            result = true;
        }
    } 
    else
        result = true;

    return result;
}

bool TransFilterDialog::bySplitCategory(mmBankTransaction* trans)
{
    bool result = false;
    if (cbSplitCategory_->GetValue())
    {
        if (trans->categID_ < 0) // the transaction has a split category
        {
            result = true;
            if (cbCategory_->GetValue())  // we are also searching Category/Subcategory
            {
                result = false;
                mmSplitTransactionEntries* splits = trans->splitEntries_.get();
                trans->getSplitTransactions(core_, splits);

                for (int i = 0; i < (int)splits->entries_.size(); ++i)
                {
                    if (core_->categoryList_.GetFullCategoryString(splits->entries_[i]->categID_,
                        splits->entries_[i]->subCategID_) == btnCategory_->GetLabelText()
                       )
                    {
                        result = true;
                        break;
                    }
                }
            }
        }
    }
    else
    {
        result = true;
    }
    return result;
}

bool TransFilterDialog::byCategory(wxString category, wxString subCategory)
{
    // ignore the category test when split category checkbox is active
    if (cbSplitCategory_->GetValue()) return true;
    //TODO: Should we provide the line if sptit category present?
    bool result = false;
    if ( cbCategory_->GetValue() )
    {
        wxStringTokenizer tz(btnCategory_->GetLabelText(), wxT(":"));
        wxString token = tz.GetNextToken();
        if (category == token)
        {
            if (tz.HasMoreTokens())
            {
                token = tz.GetNextToken();
                if (subCategory == token) result = true;
            }
            else
            {
                result = true;
            }
        }
    } 
    else
    {
        result = true;
    }
    return result; 
}

bool TransFilterDialog::byStatus( wxString status )
{

    bool result = false;
    if ( cbStatus_->GetValue() )
    {
        wxString statusStr;
        wxStringClientData* status_obj = (wxStringClientData *)choiceStatus_->GetClientObject(choiceStatus_->GetSelection());
        if (status_obj) statusStr = status_obj->GetData().Left(1);
        statusStr.Replace(wxT("N"), wxT(""));
 
        if (status == statusStr )
        {
            result = true;
        }
    } 
    else
        result = true;

    return result; 
}

bool TransFilterDialog::byType(wxString type)
{
    bool result = false;
    if ( cbType_->GetValue() )
    {

        wxString transaction_type = wxT("");
        wxStringClientData* type_obj = (wxStringClientData *)transaction_type_->GetClientObject(transaction_type_->GetSelection());
        if (type_obj) transaction_type = type_obj->GetData();

        if (type == transaction_type )
            result = true;
    }
    else
        result = true;

    return result; 
}

bool TransFilterDialog::searchResult( wxCheckBox* chkBox, wxTextCtrl* txtCtrl, wxString sourceStr)
{
    bool result = false;
    if ( chkBox->GetValue() ) {
        if (txtCtrl->GetValue().IsEmpty()) {
            if (sourceStr.IsEmpty()) {
                result = true;
            }
        } else if (sourceStr.Trim().Lower().Find(txtCtrl->GetValue().Trim().Lower()) != wxNOT_FOUND ) {
            result = true;
        }
    } else {
        result = true;
    }

    return result; 
}

bool TransFilterDialog::byTransNumber(wxString trNum)
{
    return searchResult(cbTransNumber_,txtTransNumber_,trNum);
}

bool TransFilterDialog::byNotes(wxString notes)
{
    return searchResult(cbNotes_,txtNotes_,notes);
}
