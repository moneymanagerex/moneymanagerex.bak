/*******************************************************
 ********************************************************/
#include "constants.h"
#include "qif_export.h"
#include "wx/datectrl.h"
#include "wx/dateevt.h"
#include "util.h"
#include "dbwrapper.h"
#include "defs.h"
#include "paths.h"
#include <wx/event.h>
#include <wx/choice.h>

#include <sstream>

// Defines for Transaction: (Status and Type) now located in dbWrapper.h


IMPLEMENT_DYNAMIC_CLASS( mmQIFExportDialog, wxDialog )

BEGIN_EVENT_TABLE( mmQIFExportDialog, wxDialog )
    EVT_CHECKBOX(wxID_ANY, mmQIFExportDialog::OnCheckboxClick )
    EVT_BUTTON(wxID_OK, mmQIFExportDialog::OnOk)
    EVT_BUTTON(wxID_CANCEL, mmQIFExportDialog::OnCancel)
    EVT_CLOSE(mmQIFExportDialog::OnQuit)
END_EVENT_TABLE()

mmQIFExportDialog::mmQIFExportDialog(
    mmCoreDB* core,
    wxWindow* parent, wxWindowID id,
    const wxString& caption, const wxPoint& pos,
    const wxSize& size, long style
) :
    core_(core),
    parent_(parent)
{
    Create(parent, id, caption, pos, size, style);
}

bool mmQIFExportDialog::Create( wxWindow* parent, wxWindowID id, const wxString& caption,
                           const wxPoint& pos, const wxSize& size, long style )
{
    SetExtraStyle(GetExtraStyle()|wxWS_EX_BLOCK_EVENTS);
    wxDialog::Create( parent, id, caption, pos, size, style );

    CreateControls();
    GetSizer()->Fit(this);
    GetSizer()->SetSizeHints(this);
    SetIcon(mmex::getProgramIcon());
    Centre();
    Fit();

    fillControls();

    return TRUE;
}

void mmQIFExportDialog::fillControls()
{
    wxArrayString accounts_type;
    accounts_type.Add(ACCOUNT_TYPE_BANK);
    accounts_type.Add(ACCOUNT_TYPE_TERM);
    accounts_id_ = core_->accountList_.getAccountsID(accounts_type);

    for (size_t i = 0; i < accounts_id_.Count(); ++i)
    {
        accounts_name_.Add(core_->accountList_.GetAccountName(accounts_id_[i]));
        items_index_.Add(i);
    }
}

void mmQIFExportDialog::CreateControls()
{
    const int border = 5;
    const int fieldWidth = 180;
    wxSizerFlags flags, flagsExpand;
    flags.Align(wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL).Border(wxALL, border);
    flagsExpand.Align(wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL).Border(wxALL|wxGROW, border).Expand().Proportion(1);

    wxBoxSizer* main_sizer = new wxBoxSizer(wxVERTICAL);
    this->SetSizer(main_sizer);
    wxBoxSizer* box_sizer1 = new wxBoxSizer(wxVERTICAL);
    main_sizer->Add(box_sizer1, flagsExpand);

    wxNotebook* export_notebook = new wxNotebook(this,
        wxID_ANY, wxDefaultPosition, wxDefaultSize, wxNB_MULTILINE );
    wxPanel* main_tab = new wxPanel(export_notebook, wxID_ANY);
    export_notebook->AddPage(main_tab, _("Parameters"));
    wxBoxSizer *tab1_sizer = new wxBoxSizer(wxVERTICAL);
    main_tab->SetSizer(tab1_sizer);

    wxPanel* log_tab = new wxPanel(export_notebook, wxID_ANY);
    export_notebook->AddPage(log_tab, _("Log"));
    wxBoxSizer *tab2_sizer = new wxBoxSizer(wxVERTICAL);
    log_tab->SetSizer(tab2_sizer);

    box_sizer1->Add(export_notebook, flagsExpand);

    //
    wxString choices[] = { _("QIF"), _("CSV")};
    int num = sizeof(choices) / sizeof(wxString);
    m_radio_box_ = new wxRadioBox(main_tab, wxID_ANY, wxT("")
        , wxDefaultPosition, wxDefaultSize, num, choices, 2, wxRA_SPECIFY_COLS);
    tab1_sizer->Add(m_radio_box_, flags.Center());

    wxFlexGridSizer* flex_sizer = new wxFlexGridSizer(0, 2, 0, 0);
    tab1_sizer->Add(flex_sizer, flags.Left());

    // Categories -------------------------------------------------
    cCategs_ = new wxCheckBox(main_tab, wxID_ANY,
        _("Categories"), wxDefaultPosition, wxDefaultSize, wxCHK_2STATE );
    cCategs_->SetValue(FALSE);
    flex_sizer->Add(cCategs_, flags);
    flex_sizer->AddSpacer(1);

    // Accounts --------------------------------------------
    accountsCheckBox_ = new wxCheckBox( main_tab, wxID_ANY, _("Accounts")
        , wxDefaultPosition, wxDefaultSize, wxCHK_2STATE );
    bSelectedAccounts_ = new wxButton(main_tab, wxID_STATIC, _("All")
        , wxDefaultPosition, wxSize(fieldWidth,-1));
    bSelectedAccounts_ -> Connect(wxID_ANY,
        wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(mmQIFExportDialog::OnAccountsButton), NULL, this);
    accountsCheckBox_->SetValue(true);
    flex_sizer->Add(accountsCheckBox_, flags);
    flex_sizer->Add(bSelectedAccounts_, flags);

    // From Date --------------------------------------------
    dateFromCheckBox_ = new wxCheckBox( main_tab, wxID_ANY, _("From Date")
        , wxDefaultPosition, wxDefaultSize, wxCHK_2STATE );
    fromDateCtrl_ = new wxDatePickerCtrl( main_tab, wxID_STATIC, wxDefaultDateTime
        , wxDefaultPosition, wxSize(fieldWidth,-1), wxDP_DROPDOWN);
    fromDateCtrl_->Enable(false);
    flex_sizer->Add(dateFromCheckBox_, flags);
    flex_sizer->Add(fromDateCtrl_, flags);

    // To Date --------------------------------------------
    dateToCheckBox_ = new wxCheckBox( main_tab, wxID_ANY, _("To Date")
        , wxDefaultPosition, wxDefaultSize, wxCHK_2STATE );
    toDateCtrl_ = new wxDatePickerCtrl( main_tab, wxID_STATIC, wxDefaultDateTime
        , wxDefaultPosition, wxSize(fieldWidth,-1), wxDP_DROPDOWN);
    toDateCtrl_->Enable(false);
    flex_sizer->Add(dateToCheckBox_, flags);
    flex_sizer->Add(toDateCtrl_, flags);

    // Encoding --------------------------------------------

    // File Name --------------------------------------------
    toFileCheckBox_ = new wxCheckBox( main_tab, wxID_ANY, _("Write to File")
        , wxDefaultPosition, wxDefaultSize, wxCHK_2STATE );
    toFileCheckBox_->SetValue(true);
    file_name_label_ = new wxStaticText(main_tab, wxID_ANY, _("File Name:"));
    button_search_ = new wxButton(main_tab, wxID_SAVE, _("Choose &file"));
    button_search_->Connect(wxID_SAVE, wxEVT_COMMAND_BUTTON_CLICKED
        , wxCommandEventHandler(mmQIFExportDialog::OnFileSearch), NULL, this);

    m_text_ctrl_ = new wxTextCtrl(main_tab, wxID_FILE, wxEmptyString,
        wxDefaultPosition, wxDefaultSize, wxTE_PROCESS_ENTER );
    m_text_ctrl_->Connect(wxID_FILE, wxEVT_COMMAND_TEXT_UPDATED
        , wxCommandEventHandler(mmQIFExportDialog::OnFileNameChanged), NULL, this);
    m_text_ctrl_->Connect(wxID_FILE, wxEVT_COMMAND_TEXT_ENTER
        , wxCommandEventHandler(mmQIFExportDialog::OnFileNameEntered), NULL, this);

    flex_sizer->Add(toFileCheckBox_, flags);
    flex_sizer->AddSpacer(1);
    flex_sizer->Add(file_name_label_, flags);
    flex_sizer->Add(button_search_, flags);
    tab1_sizer->Add(m_text_ctrl_, 0, wxALL|wxGROW, border);

    //Log viewer
    log_field_ = new wxTextCtrl( log_tab, wxID_ANY, wxT("")
        , wxDefaultPosition, wxDefaultSize, wxTE_MULTILINE|wxHSCROLL );
    tab2_sizer->Add(log_field_, 1, wxGROW|wxALL, border);

    wxButton* itemClearButton = new wxButton(log_tab, wxID_CLEAR);
    tab2_sizer->Add(itemClearButton, flags);
    itemClearButton->Connect(wxID_CLEAR, wxEVT_COMMAND_BUTTON_CLICKED
        , wxCommandEventHandler(mmQIFExportDialog::OnButtonClear), NULL, this);

    /**********************************************************************************************
     Button Panel with OK and Cancel Buttons
    ***********************************************************************************************/
    wxPanel* buttons_panel = new wxPanel(this, wxID_ANY);
    main_sizer->Add(buttons_panel, flags.Center().Border(wxALL, 0));

    wxStdDialogButtonSizer*  buttons_sizer = new wxStdDialogButtonSizer;
    buttons_panel->SetSizer(buttons_sizer);

    wxButton* itemButtonOK = new wxButton( buttons_panel, wxID_OK, _("&OK"));
    wxButton* itemButtonCancel_ = new wxButton( buttons_panel, wxID_CANCEL, _("&Cancel"));
    itemButtonOK->Connect(wxID_OK, wxEVT_COMMAND_BUTTON_CLICKED
        , wxCommandEventHandler(mmQIFExportDialog::OnOk), NULL, this);

    buttons_sizer->Add(itemButtonOK, flags.Border(wxBOTTOM|wxRIGHT, 10));
    buttons_sizer->Add(itemButtonCancel_, flags);

    buttons_sizer->Realize();

}

void mmQIFExportDialog::OnButtonClear(wxCommandEvent& /*event*/)
{
    log_field_->Clear();
}

void mmQIFExportDialog::OnAccountsButton(wxCommandEvent& /*event*/)
{
    items_index_.clear();
    wxMultiChoiceDialog s_acc(this, _("Choose Account to Export from:")
        , _("QIF Export"), accounts_name_);
    if (s_acc.ShowModal() == wxID_OK)
        items_index_ = s_acc.GetSelections();
    if (items_index_.GetCount() == 1)
    {
        bSelectedAccounts_->SetLabel(core_->accountList_.GetAccountName(accounts_id_[items_index_[0]]));
    }
    else if (items_index_.GetCount() > 1)
    {
        bSelectedAccounts_->SetLabel(wxT("..."));
    }
    for (size_t i = 0; i < items_index_.GetCount(); ++i)
    {
        *log_field_ << (core_->accountList_.GetAccountName(accounts_id_[items_index_[i]]));
        *log_field_ << wxT("\n");
    }
}

void mmQIFExportDialog::OnFileSearch(wxCommandEvent& /*event*/)
{
    wxString fileName = m_text_ctrl_->GetValue();
    const bool qif_csv = m_radio_box_->GetSelection() == 0;

    const wxString choose_ext = qif_csv ? _("QIF Files") : _("CSV Files");
    fileName = wxFileSelector(qif_csv
        ? _("Choose QIF data file to Export")
        : _("Choose CSV data file to Export"),
        wxEmptyString, fileName, wxEmptyString,
        choose_ext + (qif_csv ? wxT(" (*.qif)|*.qif;*.QIF") : wxT(" (*.csv)|*.csv;*.CSV"))
            , wxFD_SAVE | wxFD_OVERWRITE_PROMPT);

    if (!fileName.IsEmpty())
        correctEmptyFileExt(qif_csv ? wxT("qif"):wxT("csv") , fileName);
    m_text_ctrl_->SetValue(fileName);
}

void mmQIFExportDialog::OnOk(wxCommandEvent& /*event*/)
{
    bool bCorrect = false;
    wxString sErrorMsg = wxT("");
    if (core_->accountList_.getNumAccounts() == 0 && accountsCheckBox_->GetValue())
    {
        sErrorMsg =_("No Account available for export");
    }
    else if (items_index_.Count() < 1 && accountsCheckBox_->GetValue())
    {
        sErrorMsg =_("No Accounts selected for export");
    }
    else if (toFileCheckBox_->GetValue() && m_text_ctrl_->GetValue().IsEmpty())
    {
        sErrorMsg =_("File name is empty");
    }
    else if (dateToCheckBox_->GetValue() && dateFromCheckBox_->GetValue() && fromDateCtrl_->GetValue() > toDateCtrl_->GetValue())
    {
        sErrorMsg =_("To Date less than From Date");
    }
    else
        bCorrect = true;

    if (bCorrect)
        mmExportQIF();
    else
        wxMessageBox(sErrorMsg, _("QIF Export"), wxOK|wxICON_WARNING);
}

void mmQIFExportDialog::OnCancel(wxCommandEvent& /*event*/)
{

    EndModal(wxID_CANCEL);
}

void mmQIFExportDialog::OnQuit(wxCloseEvent& /*event*/)
{
        EndModal(wxID_CANCEL);
}
void mmQIFExportDialog::OnCheckboxClick( wxCommandEvent& /*event*/ )
{
    bSelectedAccounts_->Enable(accountsCheckBox_->GetValue());
    if (dateFromCheckBox_->GetValue()) dateFromCheckBox_->SetValue(accountsCheckBox_->GetValue());
    if (dateToCheckBox_->GetValue()) dateToCheckBox_->SetValue(accountsCheckBox_->GetValue());
    fromDateCtrl_->Enable(dateFromCheckBox_->GetValue());
    toDateCtrl_->Enable(dateToCheckBox_->GetValue());
    button_search_->Enable(toFileCheckBox_->GetValue());
    file_name_label_->Enable(toFileCheckBox_->GetValue());
    m_text_ctrl_->Enable(toFileCheckBox_->GetValue());

}

void mmQIFExportDialog::OnFileNameChanged(wxCommandEvent& event)
{
    wxString file_name = m_text_ctrl_->GetValue();
    if (file_name.Contains(wxT("\n")) || file_name.Contains(wxT("file://")))
    {

        file_name.Replace(wxT("\n"), wxT(""));
#ifdef __WXGTK__
        file_name.Replace(wxT("file://"), wxT(""));
        file_name.Trim();
#endif
        m_text_ctrl_->SetEvtHandlerEnabled(false);
        m_text_ctrl_->SetValue(file_name);
        m_text_ctrl_->SetEvtHandlerEnabled(true);
    }
    event.Skip();

    wxFileName csv_file(file_name);

}
void mmQIFExportDialog::OnFileNameEntered(wxCommandEvent& event)
{
    wxString file_name = m_text_ctrl_->GetValue();
    file_name.Trim();

    event.Skip();
    wxCommandEvent evt(wxEVT_COMMAND_BUTTON_CLICKED, wxID_SAVE);
    this->GetEventHandler()->AddPendingEvent(evt);
}

wxString mmQIFExportDialog::writeAccHeader(int accountID, bool qif)
{
    wxString buffer = wxT("");
    if (qif)
    {
        boost::shared_ptr<mmAccount> pAccount = core_->accountList_.GetAccountSharedPtr(accountID);
        wxASSERT(pAccount);
        boost::shared_ptr<mmCurrency> pCurrency = pAccount->currency_.lock();
        wxASSERT(pCurrency);

        const wxString sAccName = core_->accountList_.GetAccountName(accountID);
        double dInitBalance = pAccount->initialBalance_;
        const wxString sInitBalance = wxString::Format(wxT("%f"), dInitBalance);
        const wxString sCurrencyCode = wxT("[") + pCurrency->currencySymbol_ + wxT("]");

        buffer = wxString(wxT("!Account")) << wxT("\n")
            << wxT("N") << sAccName <<  wxT("\n")
            << wxT("TBank") << wxT("\n")
            << wxT("D") << sCurrencyCode << wxT("\n")
            << (dInitBalance != 0 ? wxString(wxT("$")) << sInitBalance << wxT("\n") : wxT(""))
            << wxT("^") <<  wxT("\n")
            << wxT("!Type:Cash") << wxT("\n");
    }
    return buffer;
}

wxString mmQIFExportDialog::exportCategories(bool qif)
{
    wxString buffer_qif, buffer_csv;

    buffer_qif << wxT("!Type:Cat") << wxT("\n");
    std::pair<mmCategoryList::const_iterator, mmCategoryList::const_iterator> range = core_->categoryList_.Range();
    for (mmCategoryList::const_iterator it = range.first; it != range.second; ++ it)
    {
        const boost::shared_ptr<mmCategory> category = *it;
        const wxString categ_name = category->categName_;
        bool bIncome = false;
        core_->bTransactionList_.IsCategoryUsed(category->categID_
                , -1, bIncome, true);
        buffer_qif << wxT("N") << categ_name <<  wxT("\n")
            << (bIncome ? wxT("I") : wxT("E")) << wxT("\n")
            << wxT("^") << wxT("\n");
        buffer_csv << categ_name <<  delimit_ << wxT("\n");

        for (std::vector<boost::shared_ptr<mmCategory> >::const_iterator cit =  category->children_.begin();
                cit != category->children_.end();
                ++ cit)
        {
            const boost::shared_ptr<mmCategory> sub_category = *cit;
            bIncome = false;
            bool bSubcateg = sub_category->categID_ != -1;
            core_->bTransactionList_.IsCategoryUsed(category->categID_
                , sub_category->categID_, bIncome, false);
            wxString full_categ_name = wxString()
                << categ_name << (qif ? (bSubcateg ? wxString()<<wxT(":") : wxString()<<wxT("")) : delimit_)
                << sub_category->categName_;
            buffer_qif << wxT("N") << full_categ_name << wxT("\n")
                << (bIncome ? wxT("I") : wxT("E")) << wxT("\n")
                << wxT("^") << wxT("\n");
            buffer_csv << full_categ_name << wxT("\n");
        }
    }
    return qif ? buffer_qif : buffer_csv;
}

void mmQIFExportDialog::mmExportQIF()
{
    const bool qif_csv = m_radio_box_->GetSelection() == 0;
    const bool exp_categ = cCategs_->GetValue();
    const bool exp_transactions = (accountsCheckBox_->GetValue() && items_index_.GetCount()>0);
    const bool write_to_file = toFileCheckBox_->GetValue();
    wxString sErrorMsg;
    wxString buffer;
    delimit_ = core_->dbInfoSettings_->GetStringSetting(wxT("DELIMITER"), mmex::DEFDELIMTER);
    long numRecords = 0;

    //Export categories
    if (exp_categ)
    {
        buffer << exportCategories(qif_csv);
        sErrorMsg << _("Categories exported") << wxT("\n");
    }

    //Export transactions
    numRecords = 0;
    wxArrayInt transferTrxId;

    if (exp_transactions)
    {
        wxArrayInt selected_accounts_id;
        for (size_t a = 0; a < items_index_.Count() ; ++a)
        {
            selected_accounts_id.Add(accounts_id_[items_index_[a]]);
        }

        for (size_t a = 0; a < selected_accounts_id.Count(); ++a)
        {
            const int fromAccountID = selected_accounts_id[a];
            wxString acctName = core_->accountList_.GetAccountName(fromAccountID);

            buffer << writeAccHeader(fromAccountID, qif_csv);

            for (size_t i = 0; i < core_->bTransactionList_.transactions_.size(); ++i)
            {
                boost::shared_ptr<mmBankTransaction> pBankTransaction = core_->bTransactionList_.transactions_[i];
                if ((pBankTransaction->accountID_ != fromAccountID) && (pBankTransaction->toAccountID_ != fromAccountID))
                   continue;

                if (dateFromCheckBox_->GetValue() && pBankTransaction->date_ < fromDateCtrl_->GetValue() )
                    continue;
                if (dateToCheckBox_->GetValue() && pBankTransaction->date_ > toDateCtrl_->GetValue() )
                    continue;

                wxDateTime dtdt = pBankTransaction->date_;
                wxString dateString = mmGetDateForDisplay(dtdt);

                wxString payee = core_->payeeList_.GetPayeeName(pBankTransaction->payeeID_);
                wxString type = pBankTransaction->transType_;

                const int trans_id = pBankTransaction->transactionID();
                const int fAccountID = pBankTransaction->accountID_;
                const wxString fromAccount = core_->accountList_.GetAccountName(fAccountID);
                const int tAccountID = pBankTransaction->toAccountID_;

                wxString transNum = pBankTransaction->transNum_;
                wxString categ = wxT("");
                if (pBankTransaction->categID_ != -1)
                    categ = pBankTransaction->fullCatStr_;
                wxString notes = (pBankTransaction->notes_);
                notes.Replace(wxT("''"), wxT("'"));
                notes.Replace(wxT("\n"), wxT(" "));

                double value = pBankTransaction->amt_;
                wxString amount = /*adjustedExportAmount(amtSeparator,*/ wxString()<<value/*)*/;
                wxString toamount;

                if (transferTrxId.Index(trans_id) == wxNOT_FOUND)
                    numRecords++;
                if (type == wxT("Transfer"))
                {
                    const wxString toAccount = core_->accountList_.GetAccountName(tAccountID);
                    double tovalue = pBankTransaction->toAmt_;
                    toamount = /*adjustedExportAmount(amtSeparator,*/ wxString()<<tovalue/*)*/;
                    wxString amount_temp = amount;

                    if (tAccountID == fromAccountID) {
                        categ = wxString::Format(wxT("[%s]"), fromAccount.c_str());
                        amount = toamount;
                        toamount = amount_temp;
                        toamount.Prepend(wxT('-'));
                    } else if (fAccountID == fromAccountID) {
                        categ = wxString::Format(wxT("[%s]"), toAccount.c_str());
                        amount.Prepend(wxT('-'));
                    }
                    if (selected_accounts_id.Index(tAccountID) == wxNOT_FOUND)
                        transferTrxId.Add(trans_id);
                    //Transaction number used to make transaction unique
                    // to proper merge transfer records
                    if (transNum.IsEmpty() && notes.IsEmpty())
                        transNum = wxString::Format(wxT("#%ld"), trans_id);
                }
                else if (type == wxT("Withdrawal"))
                    amount.Prepend(wxT('-'));

                if (qif_csv)
                {
                    buffer << wxT('D') << dateString << wxT("\n");
                    buffer << wxT('T') << amount << wxT("\n");
                    if (!payee.IsEmpty())
                        buffer << wxT('P') << payee << wxT("\n");
                    if (!transNum.IsEmpty())
                        buffer << wxT('N') << transNum << wxT("\n");
                    if (!categ.IsEmpty())
                        buffer << wxT('L') << categ << wxT("\n");
                    if (!notes.IsEmpty())
                        buffer << wxT('M') << notes << wxT("\n");
                }

                //if categ id is empty that mean this is split transaction
                if (pBankTransaction->categID_ == -1)
                {
                    mmSplitTransactionEntries* splits = pBankTransaction->splitEntries_.get();
                    pBankTransaction->getSplitTransactions(splits);

                    for (int i = 0; i < (int)splits->entries_.size(); ++i)
                    {
                        value = splits->entries_[i]->splitAmount_;
                        if (type == wxT("Withdrawal"))
                            value = -value;
                        wxString split_amount = wxString()<<value;

                        wxString split_categ = core_->categoryList_.GetFullCategoryString(
                            splits->entries_[i]->categID_, splits->entries_[i]->subCategID_);
                        if (qif_csv)
                        {
                            buffer << wxT('S') << split_categ << wxT("\n")
                                << wxT('$') << split_amount << wxT("\n");
                        }
                        else
                        {
                            buffer << trans_id << delimit_
                                << inQuotes(acctName, delimit_) << delimit_
                                << inQuotes(dateString, delimit_) << delimit_
                                << inQuotes(payee, delimit_) << delimit_
                                << pBankTransaction->status_ << delimit_
                                << type << delimit_
                                << inQuotes(split_categ, delimit_) << delimit_
                                << inQuotes(split_amount, delimit_) << delimit_
                                << wxT("") << delimit_
                                << inQuotes(notes, delimit_)
                                << wxT("\n");
                        }
                    }
                }
                else
                {
                    if (!qif_csv)
                        buffer << trans_id << delimit_
                        << inQuotes(acctName, delimit_) << delimit_
                        << inQuotes(dateString, delimit_) << delimit_
                        << inQuotes(payee, delimit_) << delimit_
                        << pBankTransaction->status_ << delimit_
                        << type << delimit_
                        << inQuotes(categ, delimit_) << delimit_
                        << inQuotes(amount, delimit_) << delimit_
                        << inQuotes(toamount, delimit_) << delimit_
                        << inQuotes(notes, delimit_)
                        << wxT("\n");
                }
                buffer << wxT('^') << wxT("\n");
            }
        }
    }

    if (write_to_file)
    {
        wxString fileName = m_text_ctrl_->GetValue();
        wxFileOutputStream output(fileName);
        wxTextOutputStream text(output);
        text << buffer;
    }

    *log_field_ << buffer;
    sErrorMsg << wxTRANSLATE("Number of transactions exported: %ld");
    const wxString msg = wxString::Format(sErrorMsg, numRecords);
    wxMessageDialog msgDlg(parent_, wxGetTranslation(msg)
        , _("Export to QIF"), wxOK|wxICON_INFORMATION);
    msgDlg.ShowModal();
}
