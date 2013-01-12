
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
#include "constants.h"
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

    EVT_CHECKBOX(wxID_ANY,    mmFilterTransactionsDialog::OnCheckboxClick )
    EVT_BUTTON  (wxID_OK,     mmFilterTransactionsDialog::OnButtonokClick )
    EVT_BUTTON  (wxID_CANCEL, mmFilterTransactionsDialog::OnButtoncancelClick )
    EVT_BUTTON  (wxID_SAVE,   mmFilterTransactionsDialog::OnButtonSaveClick )

END_EVENT_TABLE()

mmFilterTransactionsDialog::mmFilterTransactionsDialog( )
{
}

mmFilterTransactionsDialog::mmFilterTransactionsDialog(
    mmCoreDB* core,
    wxWindow* parent, wxWindowID id, const wxString& caption,
    const wxPoint& pos, const wxSize& size,long style)
: core_(core)
, categID_(-1)
, subcategID_(-1)
, payeeID_(-1)
, refAccountID_(-1)
{
    Create(parent, id, caption, pos, size, style);
}

bool mmFilterTransactionsDialog::Create( wxWindow* parent, wxWindowID id,
                                        const wxString& caption, const wxPoint& pos,
                                        const wxSize& size, long style )
{
    SetExtraStyle(GetExtraStyle()|wxWS_EX_BLOCK_EVENTS);
    wxDialog::Create( parent, id, caption, pos, size, style );

    CreateControls();
    GetStoredSettings(-1);
    dataToControls();
    GetSizer()->Fit(this);
    GetSizer()->SetSizeHints(this);

    SetIcon(mmex::getProgramIcon());

    Centre();
    return true;
}

void mmFilterTransactionsDialog::dataToControls()
{
    wxStringTokenizer tkz(settings_string_, wxT(";"), wxTOKEN_RET_EMPTY_ALL);
    wxString value = wxT("");
    bool status;

    status = get_next_value(tkz, value);
    accountCheckBox_ ->SetValue(status);
    accountDropDown_ ->Enable(status);
    accountDropDown_ ->SetStringSelection(value);

    status = get_next_value(tkz, value);
    dateRangeCheckBox_ ->SetValue(status);
    fromDateCtrl_ ->Enable(status);
    fromDateCtrl_ ->SetValue(mmGetStorageStringAsDate(value));
    get_next_value(tkz, value);
    toDateControl_ ->Enable(status);
    toDateControl_ ->SetValue(mmGetStorageStringAsDate(value));

    status = get_next_value(tkz, value);
    payeeCheckBox_ ->SetValue(status);
    cbPayee_ ->Enable(status);
    cbPayee_ ->SetValue(value);

    status = get_next_value(tkz, value);
    categoryCheckBox_ ->SetValue(status);
    btnCategory_ ->Enable(status);

    wxStringTokenizer categ_token(value, wxT(":"), wxTOKEN_RET_EMPTY_ALL);
    categID_ = core_->categoryList_.GetCategoryId(categ_token.GetNextToken().Trim());
    wxString subcateg_name = categ_token.GetNextToken().Trim(false);
    if (!subcateg_name.IsEmpty())
        subcategID_ = core_->categoryList_.GetSubCategoryID(categID_, subcateg_name);
    btnCategory_ ->SetLabel(core_->categoryList_.GetFullCategoryString(categID_, subcategID_));

    status = get_next_value(tkz, value);
    statusCheckBox_ ->SetValue(status);
    choiceStatus_ ->Enable(status);
    choiceStatus_ ->SetStringSelection(value);

    status = get_next_value(tkz, value);
    typeCheckBox_ ->SetValue(status);
    cbTypeWithdrawal_ ->SetValue(value.Contains(wxT("W")));
    cbTypeWithdrawal_ ->Enable(status);
    cbTypeDeposit_ ->SetValue(value.Contains(wxT("D")));
    cbTypeDeposit_ ->Enable(status);
    cbTypeTransfer_ ->SetValue(value.Contains(wxT("T")));
    cbTypeTransfer_ ->Enable(status);

    status = get_next_value(tkz, value);
    amountRangeCheckBox_ ->SetValue(status);
    amountMinEdit_ ->Enable(status);
    amountMinEdit_ ->SetValue(value);
    get_next_value(tkz, value);
    amountMaxEdit_ ->Enable(status);
    amountMaxEdit_ ->SetValue(value);

    status = get_next_value(tkz, value);
    transNumberCheckBox_ ->SetValue(status);
    transNumberEdit_ ->Enable(status);
    transNumberEdit_ ->SetValue(value);

    status = get_next_value(tkz, value);
    notesCheckBox_ ->SetValue(status);
    notesEdit_ ->Enable(status);
    notesEdit_ ->SetValue(value);
}
void mmFilterTransactionsDialog::CreateControls()
{
    wxSizerFlags flags, flagsExpand;
    flags.Align(wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL).Border(wxALL, 5);
    flagsExpand.Align(wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxEXPAND).Border(wxALL, 5).Proportion(1);

    wxBoxSizer* itemBoxSizer2 = new wxBoxSizer(wxHORIZONTAL);

    wxBoxSizer* itemBoxSizer3 = new wxBoxSizer(wxVERTICAL);
    itemBoxSizer2->Add(itemBoxSizer3, flagsExpand);

    wxStaticBox* static_box_sizer = new wxStaticBox(this, wxID_ANY, _(" Specify "));
    wxStaticBoxSizer* itemStaticBoxSizer4 = new wxStaticBoxSizer(static_box_sizer, wxVERTICAL);
    itemBoxSizer3->Add(itemStaticBoxSizer4, flagsExpand);

    this->SetSizer(itemBoxSizer2);

    /******************************************************************************
     Items Panel
    *******************************************************************************/
    wxPanel* itemPanel = new wxPanel( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
    itemStaticBoxSizer4->Add(itemPanel, 1, wxGROW|wxALL, 5);

    wxBoxSizer* itemBoxSizer4 = new wxBoxSizer(wxVERTICAL);
    wxFlexGridSizer* itemPanelSizer = new wxFlexGridSizer(0, 2, 0, 0);
    itemPanelSizer->AddGrowableCol(1, 1);

    itemPanel->SetSizer(itemBoxSizer4);
    itemBoxSizer4->Add(itemPanelSizer, flagsExpand);

    //--Start of Row --------------------------------------------------------
    accountCheckBox_ = new wxCheckBox( itemPanel, wxID_ANY, _("Account"),
                                      wxDefaultPosition, wxDefaultSize, wxCHK_2STATE );
    itemPanelSizer->Add(accountCheckBox_, flags);

    wxArrayString as = core_->accountList_.getAccountsName();

    accountDropDown_ = new wxChoice( itemPanel, wxID_ANY, wxDefaultPosition, wxDefaultSize, as, 0 );
    itemPanelSizer->Add(accountDropDown_, flagsExpand);

    //--End of Row --------------------------------------------------------

    dateRangeCheckBox_ = new wxCheckBox( itemPanel, wxID_ANY, _("Date Range"),
                                        wxDefaultPosition, wxDefaultSize, wxCHK_2STATE );
    itemPanelSizer->Add(dateRangeCheckBox_, flags);

    fromDateCtrl_ = new wxDatePickerCtrl( itemPanel, wxID_ANY, wxDefaultDateTime,
                                         wxDefaultPosition, wxDefaultSize, wxDP_DROPDOWN);
    toDateControl_ = new wxDatePickerCtrl( itemPanel, wxID_ANY, wxDefaultDateTime,
                                          wxDefaultPosition, wxDefaultSize, wxDP_DROPDOWN);

    wxBoxSizer* dateSizer = new wxBoxSizer(wxHORIZONTAL);
    dateSizer->Add(fromDateCtrl_, flagsExpand);
    dateSizer->Add(toDateControl_, flagsExpand);
    itemPanelSizer->Add(dateSizer, flagsExpand);
    //--End of Row --------------------------------------------------------

    payeeCheckBox_ = new wxCheckBox( itemPanel, wxID_ANY, _("Payee"),
                                    wxDefaultPosition, wxDefaultSize, wxCHK_2STATE );
    itemPanelSizer->Add(payeeCheckBox_, flags);

    cbPayee_ = new wxComboBox(itemPanel, ID_DIALOG_TRANS_PAYEECOMBO, wxT(""),
        wxDefaultPosition, wxDefaultSize,
        core_->payeeList_.FilterPayees(wxT("")), wxTE_PROCESS_ENTER);
    cbPayee_->Connect(ID_DIALOG_TRANS_PAYEECOMBO, wxEVT_COMMAND_TEXT_UPDATED,
        wxCommandEventHandler(mmFilterTransactionsDialog::OnPayeeUpdated), NULL, this);

#if wxCHECK_VERSION(2,9,0)
        cbPayee_->AutoComplete(core_->payeeList_.FilterPayees(wxT("")));
#endif

    itemPanelSizer->Add(cbPayee_, flagsExpand);
    //--End of Row --------------------------------------------------------

    categoryCheckBox_ = new wxCheckBox( itemPanel, wxID_ANY, _("Category"),
                                       wxDefaultPosition, wxDefaultSize, wxCHK_2STATE );
    //categoryCheckBox_->SetValue(false);
    itemPanelSizer->Add(categoryCheckBox_, flags);

    btnCategory_ = new wxButton( itemPanel, wxID_ANY, wxT(""),
                                wxDefaultPosition, wxDefaultSize);
    btnCategory_->Connect(wxID_ANY, wxEVT_COMMAND_BUTTON_CLICKED,
        wxCommandEventHandler(mmFilterTransactionsDialog::OnCategs), NULL, this);

    itemPanelSizer->Add(btnCategory_, flagsExpand);
    //--End of Row --------------------------------------------------------

    statusCheckBox_ = new wxCheckBox( itemPanel, wxID_ANY, _("Status"),
                                     wxDefaultPosition, wxDefaultSize, wxCHK_2STATE );
    statusCheckBox_->SetValue(false);
    itemPanelSizer->Add(statusCheckBox_, flags);

    choiceStatus_ = new wxChoice(itemPanel, wxID_ANY);

    for(size_t i = 0; i < sizeof(TRANSACTION_STATUS)/sizeof(wxString); ++i)
        choiceStatus_->Append(wxGetTranslation(TRANSACTION_STATUS[i]),
        new wxStringClientData(TRANSACTION_STATUS[i]));

    itemPanelSizer->Add(choiceStatus_, flagsExpand);
    choiceStatus_->SetToolTip(_("Specify the status for the transaction"));
    //--End of Row --------------------------------------------------------

    typeCheckBox_ = new wxCheckBox( itemPanel, wxID_ANY, _("Type"),
                                   wxDefaultPosition, wxDefaultSize, wxCHK_2STATE );

    wxFlexGridSizer* typeSizer = new wxFlexGridSizer(0, 2, 0, 0);
    typeSizer->AddGrowableCol(0, 1);
    typeSizer->AddGrowableCol(1, 1);
    cbTypeWithdrawal_ = new wxCheckBox( itemPanel, wxID_ANY, _("Withdrawal"),
        wxDefaultPosition, wxDefaultSize, wxCHK_2STATE );
    cbTypeDeposit_ = new wxCheckBox( itemPanel, wxID_ANY, _("Deposit"),
        wxDefaultPosition, wxDefaultSize, wxCHK_2STATE );
    cbTypeTransfer_ = new wxCheckBox( itemPanel, wxID_ANY, _("Transfer"),
        wxDefaultPosition, wxDefaultSize, wxCHK_2STATE );

    itemPanelSizer->Add(typeCheckBox_, flags);
    itemPanelSizer->Add(typeSizer, flagsExpand);
    typeSizer ->Add(cbTypeWithdrawal_, 1, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxALL, 2);
    typeSizer ->Add(cbTypeDeposit_, 1, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxALL, 2);
    typeSizer ->Add(cbTypeTransfer_, 1, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxALL, 2);
    typeSizer ->AddSpacer(2);

    //--End of Row --------------------------------------------------------

    amountRangeCheckBox_ = new wxCheckBox( itemPanel, wxID_ANY, _("Amount Range"),
                                          wxDefaultPosition, wxDefaultSize, wxCHK_2STATE );
    itemPanelSizer->Add(amountRangeCheckBox_, flags);

    amountMinEdit_ = new wxTextCtrl( itemPanel, wxID_ANY, wxT(""),
        wxDefaultPosition, wxDefaultSize, wxALIGN_RIGHT|wxTE_PROCESS_ENTER , doubleValidator() );
    amountMaxEdit_ = new wxTextCtrl( itemPanel, wxID_ANY, wxT(""),
        wxDefaultPosition, wxDefaultSize, wxALIGN_RIGHT|wxTE_PROCESS_ENTER , doubleValidator() );

    wxBoxSizer* amountSizer = new wxBoxSizer(wxHORIZONTAL);
    amountSizer->Add(amountMinEdit_, flagsExpand);
    amountSizer->Add(amountMaxEdit_, flagsExpand);
    itemPanelSizer->Add(amountSizer, flagsExpand);
    //--End of Row --------------------------------------------------------

    transNumberCheckBox_ = new wxCheckBox( itemPanel, wxID_ANY, _("Number"),
		wxDefaultPosition, wxDefaultSize, wxCHK_2STATE );
    itemPanelSizer->Add(transNumberCheckBox_, flags);

    transNumberEdit_ = new wxTextCtrl( itemPanel, wxID_STATIC, wxT(""));
    itemPanelSizer->Add(transNumberEdit_, flagsExpand);
    //--End of Row --------------------------------------------------------

    notesCheckBox_ = new wxCheckBox( itemPanel, wxID_ANY, _("Notes"),
                                    wxDefaultPosition, wxDefaultSize, wxCHK_2STATE );
    itemPanelSizer->Add(notesCheckBox_, flags);

    notesEdit_ = new wxTextCtrl( itemPanel, wxID_ANY, _T(""));
    itemPanelSizer->Add(notesEdit_, flagsExpand);
    //--End of Row --------------------------------------------------------

    wxBoxSizer* settings_box_sizer = new wxBoxSizer(wxHORIZONTAL);

    wxString choices[] = { wxT("0"), wxT("1"), wxT("2"), wxT("3"), wxT("4"), wxT("5"), wxT("6"), wxT("7"), wxT("8"), wxT("9")};
    int num = sizeof(choices) / sizeof(wxString);
    m_radio_box_ = new wxRadioBox(this, wxID_APPLY, wxT(""),
        wxDefaultPosition, wxDefaultSize, num, choices, num, wxRA_SPECIFY_COLS);
    m_radio_box_->Connect(wxID_APPLY, wxEVT_COMMAND_RADIOBOX_SELECTED,
        wxCommandEventHandler(mmFilterTransactionsDialog::OnSettingsSelected), NULL, this);

    int view_no = core_->iniSettings_->GetIntSetting(wxT("TRANSACTIONS_FILTER_VIEW_NO"), 0);
    m_radio_box_->SetSelection(view_no);
    m_radio_box_->Show(true);

    itemBoxSizer3->Add(settings_box_sizer, flags.Center());
    settings_box_sizer->Add( m_radio_box_, flags);

    /******************************************************************************
     Button Panel with OK/Cancel buttons
    *******************************************************************************/
    wxPanel* buttonPanel = new wxPanel( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
    itemBoxSizer3->Add(buttonPanel, flags);

    wxBoxSizer* buttonPanelSizer = new wxBoxSizer(wxHORIZONTAL);
    buttonPanel->SetSizer(buttonPanelSizer);

    wxButton* itemButtonOK = new wxButton( buttonPanel, wxID_OK);

    wxButton* itemButtonCancel = new wxButton( buttonPanel, wxID_CANCEL);
    itemButtonCancel->SetFocus();

    wxBitmapButton* save_button = new wxBitmapButton( buttonPanel,
        wxID_SAVE, wxBitmap(save_xpm), wxDefaultPosition,
        wxSize(itemButtonOK->GetSize().GetHeight(), itemButtonOK->GetSize().GetHeight()));

    save_button->Show(true);

    buttonPanelSizer->Add(save_button, flags);
    buttonPanelSizer->Add(itemButtonOK, flags);
    buttonPanelSizer->Add(itemButtonCancel, flags);

}

/*!
 * Should we show tooltips?
 */

bool mmFilterTransactionsDialog::ShowToolTips()
{
    return TRUE;
}

void mmFilterTransactionsDialog::OnCheckboxClick( wxCommandEvent& event )
{
    btnCategory_->Enable(categoryCheckBox_->IsChecked());
    accountDropDown_->Enable(accountCheckBox_->IsChecked());
    fromDateCtrl_->Enable(dateRangeCheckBox_->IsChecked());
    toDateControl_->Enable(dateRangeCheckBox_->IsChecked());
    cbPayee_->Enable(payeeCheckBox_->IsChecked());
    btnCategory_->Enable(categoryCheckBox_->IsChecked());
    choiceStatus_->Enable(statusCheckBox_->IsChecked());
    cbTypeWithdrawal_->Enable(typeCheckBox_->IsChecked());
    cbTypeDeposit_->Enable(typeCheckBox_->IsChecked());
    cbTypeTransfer_->Enable(typeCheckBox_->IsChecked());
    amountMinEdit_->Enable(amountRangeCheckBox_->IsChecked());
    amountMaxEdit_->Enable(amountRangeCheckBox_->IsChecked());
    notesEdit_->Enable(notesCheckBox_->IsChecked());
    transNumberEdit_->Enable(transNumberCheckBox_->IsChecked());
    accountDropDown_->Enable(accountCheckBox_->IsChecked());

    event.Skip();
}

void mmFilterTransactionsDialog::OnButtonokClick( wxCommandEvent& /*event*/ )
{
    if (accountCheckBox_->IsChecked())
    {
        refAccountStr_ = accountDropDown_->GetStringSelection();
        refAccountID_ = core_->accountList_.GetAccountId(refAccountStr_);
    }

    if (payeeCheckBox_->IsChecked())
    {
        payeeID_ = core_->payeeList_.GetPayeeId(cbPayee_->GetValue());
    }

    if (amountRangeCheckBox_->IsChecked())
    {
        wxString minamt = amountMinEdit_->GetValue();
        wxString maxamt = amountMaxEdit_->GetValue();
        if (!minamt.IsEmpty())
        {
            double amount;
            if (!mmex::formatCurrencyToDouble(minamt, amount) || (amount < 0.0))
            {
                mmShowErrorMessage(this, _("Invalid Amount Entered "), _("Error"));
                return;
            }
        }

        if (!maxamt.IsEmpty())
        {
            double amount;
            if (!mmex::formatCurrencyToDouble(maxamt, amount) || (amount < 0.0))
            {
                mmShowErrorMessage(this, _("Invalid Amount Entered "), _("Error"));
                return;
            }
        }
    }

    EndModal(wxID_OK);
}

void mmFilterTransactionsDialog::OnButtoncancelClick( wxCommandEvent& /*event*/ )
{
    EndModal(wxID_CANCEL);
}

void mmFilterTransactionsDialog::OnCategs(wxCommandEvent& /*event*/)
{
    mmCategDialog dlg(core_, this);

    if (dlg.ShowModal() == wxID_OK)
    {
        categID_ = dlg.getCategId();
        subcategID_ = dlg.getSubCategId();
        btnCategory_->SetLabel(core_->categoryList_.GetFullCategoryString(categID_, subcategID_));
    }

}

bool mmFilterTransactionsDialog::somethingSelected()
{
    return
    getAccountCheckBox()
    || getDateRangeCheckBox()
    || getPayeeCheckBox()
    || getCategoryCheckBox()
    || getStatusCheckBox()
    || getTypeCheckBox()
    || getAmountRangeCheckBox()
    || getNumberCheckBox()
    || getNotesCheckBox();
}

wxString mmFilterTransactionsDialog::getAccountName()
{
    wxString accountName = core_->accountList_.GetAccountName(refAccountID_);
    return accountName;
}

bool mmFilterTransactionsDialog::getDateRange(wxDateTime& startDate, wxDateTime& endDate) const
{
    if (dateRangeCheckBox_->IsChecked())
    {
        startDate = fromDateCtrl_->GetValue();
        endDate = toDateControl_->GetValue();
        return true;
    }
    return false;
}

wxString mmFilterTransactionsDialog::userDateRangeStr() const
{
    wxString dateStr;
    if (dateRangeCheckBox_->IsChecked())
    {
        wxString dtBegin = mmGetDateForDisplay(db_, fromDateCtrl_->GetValue());
        wxString dtEnd = mmGetDateForDisplay(db_, toDateControl_->GetValue());
        dateStr << wxString::Format(_("From %s to %s"), dtBegin.c_str(), dtEnd.c_str());
    }
    return dateStr;
}

int mmFilterTransactionsDialog::getPayeeID() const
{
    wxString payeeStr = cbPayee_->GetValue();
    int payeeID = core_->payeeList_.GetPayeeId(payeeStr);
    return payeeID;

}
wxString mmFilterTransactionsDialog::userPayeeStr() const
{
    if (payeeCheckBox_->IsChecked())
        return cbPayee_->GetValue();
    return wxT("");
}

wxString mmFilterTransactionsDialog::userCategoryStr() const
{
    if (categoryCheckBox_->IsChecked())
        return btnCategory_->GetLabelText();
    return wxT("");
}

wxString mmFilterTransactionsDialog::getStatus() const
{
    wxString status;
    wxStringClientData* status_obj =
        (wxStringClientData *)choiceStatus_->GetClientObject(choiceStatus_->GetSelection());
    if (status_obj) status = status_obj->GetData().Left(1);
    status.Replace(wxT("N"), wxT(""));
    return status;
}

wxString mmFilterTransactionsDialog::getType() const
{
    wxString withdraval = wxT("");
    wxString deposit = wxT("");
    wxString transfer = wxT("");
    if (cbTypeWithdrawal_->GetValue())
        withdraval = TRANS_TYPE_WITHDRAWAL_STR;
    if (cbTypeDeposit_->GetValue())
        deposit = TRANS_TYPE_DEPOSIT_STR;
    if (cbTypeTransfer_->GetValue())
        transfer = TRANS_TYPE_TRANSFER_STR;

    return withdraval+wxT(";")+deposit+wxT(";")+transfer;
}

wxString mmFilterTransactionsDialog::userTypeStr() const
{
    wxString transCode = wxEmptyString;
    if (typeCheckBox_->IsChecked())
    {
        if (cbTypeWithdrawal_->GetValue())
            transCode = wxGetTranslation(TRANS_TYPE_WITHDRAWAL_STR);
        if (cbTypeDeposit_->GetValue())
            transCode << (transCode.IsEmpty() ? wxT("") : wxT(", ")) << wxGetTranslation(TRANS_TYPE_DEPOSIT_STR);
        if (cbTypeTransfer_->GetValue())
            transCode << (transCode.IsEmpty() ? wxT("") : wxT(", ")) << wxGetTranslation(TRANS_TYPE_TRANSFER_STR);
    }
    return transCode;
}

wxString mmFilterTransactionsDialog::userStatusStr() const
{
    if (statusCheckBox_->IsChecked())
        return choiceStatus_->GetStringSelection();
    return wxT("");
}

double mmFilterTransactionsDialog::getAmountMin()
{
    double amount = 0;
    if (!mmex::formatCurrencyToDouble(amountMinEdit_->GetValue(), amount) || (amount < 0.0))
        amount = 0;

    return amount;
}

double mmFilterTransactionsDialog::getAmountMax()
{
    double amount = 0;
    if (!mmex::formatCurrencyToDouble(amountMaxEdit_->GetValue(), amount) || (amount < 0.0))
        amount = 0;

    return amount;
}

wxString mmFilterTransactionsDialog::userAmountRangeStr() const
{
    wxString amountRangeStr;
    if (amountRangeCheckBox_->IsChecked())
    {
        wxString minamt = amountMinEdit_->GetValue();
        wxString maxamt = amountMaxEdit_->GetValue();
        amountRangeStr << _("Min: ") << minamt << wxT(" ") << _("Max: ") << maxamt;
    }
    return amountRangeStr;
}

void mmFilterTransactionsDialog::OnButtonSaveClick( wxCommandEvent& /*event*/ )
{
    int i = m_radio_box_->GetSelection();
    settings_string_ = GetCurrentSettings();
    core_->iniSettings_->SetStringSetting(wxString::Format(wxT("TRANSACTIONS_FILTER_%d"), i), settings_string_);
}

void mmFilterTransactionsDialog::OnSettingsSelected( wxCommandEvent& event )
{
    GetStoredSettings(event.GetSelection());
    dataToControls();
}

wxString mmFilterTransactionsDialog::GetStoredSettings(int id)
{
    if (id < 0) {
        id = core_->iniSettings_->GetIntSetting(wxT("TRANSACTIONS_FILTER_VIEW_NO"), 0);
    } else {
        core_->iniSettings_->SetIntSetting(wxT("TRANSACTIONS_FILTER_VIEW_NO"), id);
    }
    settings_string_ = core_->iniSettings_->GetStringSetting(
                              wxString::Format(wxT("TRANSACTIONS_FILTER_%d"), id),
                              wxT("0;;0;;0;;0;;0;;0;;0;;0;;0;;0;;0;;"));
    return settings_string_;
}

bool mmFilterTransactionsDialog::get_next_value( wxStringTokenizer& tkz, wxString& value)
{
    value = wxT("");
    bool on = wxT("1") == tkz.GetNextToken();
    value = tkz.GetNextToken();
    return on;
}

wxString mmFilterTransactionsDialog::GetCurrentSettings()
{
    settings_string_.clear();

    settings_string_ << accountCheckBox_->GetValue() << wxT(";");
    settings_string_ << accountDropDown_->GetStringSelection() << wxT(";");

    settings_string_ << dateRangeCheckBox_->GetValue() << wxT(";");
    settings_string_ << fromDateCtrl_->GetValue().FormatISODate() << wxT(";");
    settings_string_ << dateRangeCheckBox_->GetValue() << wxT(";");
    settings_string_ << toDateControl_->GetValue().FormatISODate() << wxT(";");

    settings_string_ << payeeCheckBox_->GetValue() << wxT(";");
    settings_string_ << cbPayee_->GetValue() << wxT(";");

    settings_string_ << categoryCheckBox_->GetValue() << wxT(";");
    settings_string_ << btnCategory_ ->GetLabel() << wxT(";");

    settings_string_ << statusCheckBox_->GetValue() << wxT(";");
    settings_string_ << choiceStatus_ ->GetStringSelection() << wxT(";");

    settings_string_ << typeCheckBox_->GetValue() << wxT(";")
    << (cbTypeWithdrawal_->GetValue() && typeCheckBox_->GetValue() ? wxT("W") : wxT(""))
    << (cbTypeDeposit_->GetValue() && typeCheckBox_->GetValue() ? wxT("D") : wxT(""))
    << (cbTypeTransfer_->GetValue() && typeCheckBox_->GetValue() ? wxT("T") : wxT(""))
    << wxT(";");

    settings_string_ << amountRangeCheckBox_->GetValue() << wxT(";");
    settings_string_ << amountMinEdit_->GetValue() << wxT(";");
    settings_string_ << amountRangeCheckBox_->GetValue() << wxT(";");
    settings_string_ << amountMaxEdit_->GetValue() << wxT(";");

    settings_string_ << transNumberCheckBox_->GetValue() << wxT(";");
    settings_string_ << transNumberEdit_->GetValue() << wxT(";");

    settings_string_ << notesCheckBox_->GetValue() << wxT(";");
    settings_string_ << notesEdit_->GetValue() << wxT(";");

    return settings_string_;
}

void mmFilterTransactionsDialog::setAccountToolTip(wxString tip) const
{
    accountDropDown_->SetToolTip(tip);
}

void mmFilterTransactionsDialog::OnPayeeUpdated(wxCommandEvent& event)
{
    wxString value = cbPayee_->GetValue();

    if (value == prev_value_) return;
    //Line above to fix infinite loop for wx-2.9 GTK
    //Line below seems does not working in wx-2.9 GTK
    cbPayee_ -> SetEvtHandlerEnabled(false);
    prev_value_ = value;
    cbPayee_ -> Clear();
    wxArrayString data;

    data = core_->payeeList_.FilterPayees(wxT(""));
    for (size_t i = 0; i < data.Count(); ++i)
    {
        if (data[i].Lower().Matches(value.Lower().Append(wxT("*"))))
            cbPayee_ ->Append(data[i]);
    }

#if wxCHECK_VERSION(2,9,0)
        cbPayee_->AutoComplete(data);
#endif

    if (cbPayee_->GetCount() == 1)
        cbPayee_->SetSelection(0);
    else
        cbPayee_->SetValue(value);
    cbPayee_->SetInsertionPoint(value.Length()+1);
    cbPayee_ -> SetEvtHandlerEnabled(true);
    event.Skip();
}
