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
#include "dbwrapper.h"
#include "mmcoredb.h"

IMPLEMENT_DYNAMIC_CLASS( TransFilterDialog, wxDialog )

BEGIN_EVENT_TABLE( TransFilterDialog, wxDialog )
    EVT_CHECKBOX( ID_DIALOG_TRANSFILTER_CB_DATE_RANGE, TransFilterDialog::OnCBDateRange)
    EVT_CHECKBOX( ID_DIALOG_TRANSFILTER_CB_PAYEE,      TransFilterDialog::OnCBPayee)
    EVT_CHECKBOX( ID_DIALOG_TRANSFILTER_CB_CATEGORY,   TransFilterDialog::OnCBCategory)
    EVT_CHECKBOX( ID_DIALOG_TRANSFILTER_CB_STATUS,     TransFilterDialog::OnCBStatus)
    EVT_CHECKBOX( ID_DIALOG_TRANSFILTER_CB_TYPE,       TransFilterDialog::OnCBType)
    EVT_CHECKBOX( ID_DIALOG_TRANSFILTER_CB_TRANS_NUM,  TransFilterDialog::OnCBTransNum)
    EVT_CHECKBOX( ID_DIALOG_TRANSFILTER_CB_TRANS_NOTES,TransFilterDialog::OnCBNotes)

    EVT_BUTTON( ID_DIALOG_TRANSFILTER_BTN_PAYEE,       TransFilterDialog::OnPayeeSelect)
    EVT_BUTTON( ID_DIALOG_TRANSFILTER_BTN_CATEGORY,    TransFilterDialog::OnCategorySelect)
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
//    // wxMessageBox(("Testing that dialog being destroyed"), ("Transaction Filter Destructor"));
//}

// Initialize our variables
void TransFilterDialog::Init()
{
    dpDateStart_->Enable(false);
    dpDateEnd_->Enable(false);
    btnPayee_->Enable(false);
    btnCategory_->Enable(false);
    choiceStatus_->Enable(false);
    choiceType_->Enable(false);
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
    int fieldWidth = 212;
    wxSizerFlags flags;
    flags.Align(wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL).Border(wxALL, 0);

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

    wxFlexGridSizer* itemPanelSizer = new wxFlexGridSizer(0, 2, 10, 10);
    itemPanel->SetSizer(itemPanelSizer);

    //--Start of Row --------------------------------------------------------
    cbDateRange_ = new wxCheckBox( itemPanel, ID_DIALOG_TRANSFILTER_CB_DATE_RANGE, _("Date Range"),
                                   wxDefaultPosition, wxDefaultSize, wxCHK_2STATE );
    cbDateRange_->SetValue(FALSE);

    wxBoxSizer* dateSizer = new wxBoxSizer(wxHORIZONTAL);
    dpDateStart_ = new wxDatePickerCtrl( itemPanel, wxID_ANY, wxDefaultDateTime,
                                         wxDefaultPosition, wxDefaultSize, wxDP_DROPDOWN);
    dpDateEnd_   = new wxDatePickerCtrl( itemPanel, wxID_ANY, wxDefaultDateTime,
                                         wxDefaultPosition, wxDefaultSize, wxDP_DROPDOWN);
    dateSizer->Add(dpDateStart_, flags);
    dateSizer->AddSpacer(10);
    dateSizer->Add(dpDateEnd_, flags);

    itemPanelSizer->Add(cbDateRange_, flags);
    itemPanelSizer->Add(dateSizer, flags);
    //--End of Row --------------------------------------------------------

    cbPayee_ = new wxCheckBox( itemPanel, ID_DIALOG_TRANSFILTER_CB_PAYEE, _("Payee"),
                               wxDefaultPosition, wxDefaultSize, wxCHK_2STATE );
    cbPayee_->SetValue(FALSE);

    btnPayee_ = new wxButton( itemPanel, ID_DIALOG_TRANSFILTER_BTN_PAYEE, _("Select Payee"),
                              wxDefaultPosition, wxSize(fieldWidth,-1), 0 );

    itemPanelSizer->Add(cbPayee_, flags);
    itemPanelSizer->Add(btnPayee_, flags);
    //--End of Row --------------------------------------------------------

    cbSplitCategory_ = new wxCheckBox( itemPanel, ID_DIALOG_TRANSFILTER_CB_CATEGORY, _("Split Category"),
                                  wxDefaultPosition, wxDefaultSize, wxCHK_2STATE );
    cbSplitCategory_->SetValue(FALSE);
    cbSplitCategory_->SetToolTip(_("Filter Split categories. Include the category for further filtering."));
    itemPanelSizer->AddSpacer(20);
    itemPanelSizer->Add(cbSplitCategory_, flags);
    //--End of Row --------------------------------------------------------

    cbCategory_ = new wxCheckBox( itemPanel, ID_DIALOG_TRANSFILTER_CB_CATEGORY, _("Category"),
                                  wxDefaultPosition, wxDefaultSize, wxCHK_2STATE );
    cbCategory_->SetValue(FALSE);

    btnCategory_ = new wxButton( itemPanel, ID_DIALOG_TRANSFILTER_BTN_CATEGORY, _("Select Category"),
                                 wxDefaultPosition, wxSize(fieldWidth,-1), 0 );

    itemPanelSizer->Add(cbCategory_, flags);
    itemPanelSizer->Add(btnCategory_, flags);
    //--End of Row --------------------------------------------------------

    cbStatus_ = new wxCheckBox( itemPanel, ID_DIALOG_TRANSFILTER_CB_STATUS, _("Status"),
                                wxDefaultPosition, wxDefaultSize, wxCHK_2STATE );
    cbStatus_->SetValue(FALSE);

    choiceStatus_ = new wxChoice( itemPanel, wxID_ANY, wxDefaultPosition, wxSize(fieldWidth,-1));
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
        new wxStringClientData(transaction_status[i].Left(1)));

    choiceStatus_->SetStringSelection(wxGetTranslation(mmIniOptions::instance().transStatusReconciled_));
    choiceStatus_->SetToolTip(_("Specify the status for the transaction"));

    itemPanelSizer->Add(cbStatus_, flags);
    itemPanelSizer->Add(choiceStatus_, flags);
    //--End of Row --------------------------------------------------------

    cbType_ = new wxCheckBox(itemPanel, ID_DIALOG_TRANSFILTER_CB_TYPE, _("Type"),
                              wxDefaultPosition, wxDefaultSize, wxCHK_2STATE );
    cbType_->SetValue(FALSE);

    choiceType_ = new wxChoice(itemPanel, wxID_ANY,wxDefaultPosition, wxSize(fieldWidth,-1));
    wxString transaction_type[] =
    {
        wxTRANSLATE("Withdrawal"),
        wxTRANSLATE("Deposit"),
        wxTRANSLATE("Transfer")
    };
    size_t size = sizeof(transaction_type)/sizeof(wxString);
    if (core_->getNumBankAccounts() < 2) size--;
    for(size_t i = 0; i < size; ++i)
    choiceType_->Append(wxGetTranslation(transaction_type[i]),
        new wxStringClientData(transaction_type[i]));

    choiceType_->SetSelection(0);
    choiceType_->SetToolTip(_("Specify the type of transaction."));

    itemPanelSizer->Add(cbType_, flags);
    itemPanelSizer->Add(choiceType_, flags);
    //--End of Row --------------------------------------------------------

    cbTransNumber_ = new wxCheckBox(itemPanel, ID_DIALOG_TRANSFILTER_CB_TRANS_NUM, _("Number"),
                                     wxDefaultPosition, wxDefaultSize, wxCHK_2STATE);
    cbTransNumber_->SetValue(FALSE);
    txtTransNumber_ = new wxTextCtrl( itemPanel, wxID_ANY, "", wxDefaultPosition, wxSize(fieldWidth,-1), 0);

    itemPanelSizer->Add(cbTransNumber_, flags);
    itemPanelSizer->Add(txtTransNumber_, flags);
    //--End of Row --------------------------------------------------------

    cbNotes_ = new wxCheckBox(itemPanel, ID_DIALOG_TRANSFILTER_CB_TRANS_NOTES, _("Notes"),
                                     wxDefaultPosition, wxDefaultSize, wxCHK_2STATE);
    cbNotes_->SetValue(FALSE);
    txtNotes_ = new wxTextCtrl( itemPanel, wxID_ANY, "",wxDefaultPosition, wxSize(fieldWidth,-1), 0);
    txtNotes_->SetToolTip(_("Will search for the given text in the Notes field"));

    itemPanelSizer->Add(cbNotes_, flags);
    itemPanelSizer->Add(txtNotes_, flags);
    //--End of Row --------------------------------------------------------
    /******************************************************************************
     Button Panel with OK/Cancel buttons
    *******************************************************************************/
    wxPanel* buttonPanel = new wxPanel( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
    thisSizer->Add(buttonPanel, flags.Border(wxALL, 5).Right());

    wxBoxSizer* buttonPanelSizer = new wxBoxSizer(wxHORIZONTAL);
    buttonPanel->SetSizer(buttonPanelSizer);

    wxButton* btnOK = new wxButton( buttonPanel, wxID_OK);
    btnOK->SetToolTip(_("Activates the filter: Will add selected items to display"));
    buttonPanelSizer->Add(btnOK, flags);

    wxButton* btnCancel = new wxButton( buttonPanel, wxID_CANCEL);
    btnCancel->SetToolTip(_("Deactivates the filter"));
    buttonPanelSizer->Add(btnCancel, flags);
    btnCancel->SetFocus();
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

void TransFilterDialog::OnCBDateRange( wxCommandEvent& /*event*/ )
{
    dpDateStart_->Enable(cbDateRange_->GetValue());
    dpDateEnd_->Enable(cbDateRange_->GetValue());
}
void TransFilterDialog::OnCBPayee( wxCommandEvent& /*event*/ )
{
    btnPayee_->Enable(cbPayee_->GetValue());
}

void TransFilterDialog::OnCBCategory( wxCommandEvent& /*event*/ )
{
    btnCategory_->Enable(cbCategory_->GetValue());
}

void TransFilterDialog::OnCBStatus( wxCommandEvent& /*event*/ )
{
    choiceStatus_->Enable(cbStatus_->GetValue());
}

void TransFilterDialog::OnCBType( wxCommandEvent& /*event*/ )
{
    choiceType_->Enable(cbType_->GetValue());
}

void TransFilterDialog::OnCBTransNum( wxCommandEvent& /*event*/ )
{
    txtTransNumber_->Enable(cbTransNumber_->GetValue());
}

void TransFilterDialog::OnCBNotes( wxCommandEvent& /*event*/ )
{
    txtNotes_->Enable(cbNotes_->GetValue());
}

void TransFilterDialog::OnPayeeSelect(wxCommandEvent& /*event*/)
{
    mmPayeeDialog dlg(this, core_);
    if ( dlg.ShowModal() == wxID_OK )
    {
        if (dlg.getPayeeId() != -1)
        {
            int categID, subcategID;
            wxString payeeName = mmDBWrapper::getPayee(core_->db_.get(), dlg.getPayeeId(), categID, subcategID);
            btnPayee_->SetLabel(mmReadyDisplayString(payeeName));
        }
    }
}

void TransFilterDialog::OnCategorySelect(wxCommandEvent& /*event*/)
{
    mmCategDialog dlg(core_, this);
    dlg.setTreeSelection( catName_, subCatName_ );
    dlg.ShowModal();
    if (dlg.categID_ != -1)
    {
        catName_ = mmReadyDisplayString(core_->getCategoryName(dlg.categID_));
        wxString categoryLabelName = catName_;
        if (dlg.subcategID_ != -1)
        {
            subCatName_ = mmReadyDisplayString(mmDBWrapper::getSubCategoryName(core_->db_.get(), dlg.categID_, dlg.subcategID_));
            if (!subCatName_.IsEmpty())
            {
                categoryLabelName << (":") << subCatName_;
            }
        }
        btnCategory_->SetLabel(categoryLabelName);
    }
}

/****************************************************************************************
Interface Methods
*****************************************************************************************/
bool TransFilterDialog::byDateRange(const wxDateTime transDate)
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

bool TransFilterDialog::byPayee(const wxString payee)
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
                    if (core_->GetFullCategoryString(splits->entries_[i]->categID_,
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

bool TransFilterDialog::byCategory(const wxString category, const wxString subCategory)
{
    // ignore the category test when split category checkbox is active
    if (cbSplitCategory_->GetValue()) return true;

    bool result = false;
    if ( cbCategory_->GetValue() )
    {
        wxStringTokenizer tz(btnCategory_->GetLabelText(), (":"));
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

bool TransFilterDialog::byStatus(const wxString status )
{
    if (!cbStatus_->GetValue()) return true;

    wxStringClientData* status_obj = (wxStringClientData *)choiceStatus_->GetClientObject(choiceStatus_->GetSelection());
    wxString statusStr ="?";
    if (status_obj) statusStr = status_obj->GetData();
    statusStr.Replace("N","");

    return (status == statusStr);
}

bool TransFilterDialog::byType(const wxString type)
{
    if (!cbType_->GetValue()) return true;
    wxString transCodeStr = "?";
    wxStringClientData* type_obj = (wxStringClientData *)choiceType_->GetClientObject(choiceType_->GetSelection());
    if (type_obj) transCodeStr = type_obj->GetData();

    return (type == transCodeStr);
}

bool TransFilterDialog::searchResult( wxCheckBox* chkBox, wxTextCtrl* txtCtrl, const wxString sourceStr)
{
    if (!chkBox->GetValue()) return true;
    else if (sourceStr.IsEmpty() && txtCtrl->GetValue().IsEmpty()) return true;
    else if (txtCtrl->GetValue().IsEmpty()) return false;
    else if (sourceStr.Lower().Matches(txtCtrl->GetValue().Trim().Lower()))
        return true;
    return false;
}

bool TransFilterDialog::byTransNumber(const wxString trNum)
{
    return searchResult(cbTransNumber_, txtTransNumber_, trNum);
}

bool TransFilterDialog::byNotes(const wxString notes)
{
    return searchResult(cbNotes_, txtNotes_, notes);
}
