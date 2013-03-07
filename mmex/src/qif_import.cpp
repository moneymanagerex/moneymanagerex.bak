#include "constants.h"
#include "util.h"
#include "dbwrapper.h"
#include "guiid.h"
#include "fileviewerdialog.h"
#include "mmex.h"
#include "mmcoredb.h"
#include "platfdep.h"
#include "qif_import.h"
#include "paths.h"

enum qifAccountInfoType
{
    Name        = 1, // N
    AccountType = 2, // T
    Description = 3, // D
    CreditLimit = 4, // L
    BalanceDate = 5, // /
    Balance     = 6, // $
    EOT         = 7, // ^
    UnknownInfo = 8
};

enum qifLineType
{
    AcctType    = 1, // !
    Date        = 2, // D
    Amount      = 3, // T
    Address     = 4, // A
    Payee       = 5, // P
    EOTLT       = 6, // ^
    TransNumber = 7, // N
    Status      = 8, // C
    UnknownType = 9,
    Memo        = 10, // M
    Category    = 11,  // L
    CategorySplit  = 12,  // S
    MemoSplit      = 13,  // E
    AmountSplit    = 14   // '$'
};

qifAccountInfoType accountInfoType(const wxString& line)
{
    if (line.IsEmpty())
        return UnknownInfo;

    wxChar fChar = line.GetChar(0);
    switch(fChar)
    {
    case 'N':
        return Name;
    case 'T':
        return AccountType;
    case 'D':
        return Description;
    case 'L':
        return CreditLimit;
    case '/':
        return BalanceDate;
    case '$':
        return Balance;
    case '^':
        return EOT;
    default:
        return UnknownInfo;
    }
}

qifLineType lineType(const wxString& line)
{
    if (line.IsEmpty())
        return UnknownType;

    wxChar fChar = line.GetChar(0);
    switch(fChar)
    {
    case '!':
        return AcctType;
    case 'D':
        return Date;
    case 'N':
        return TransNumber;
    case 'P':
        return Payee;
    case 'A':
        return Address;
    case 'T':
        return Amount;
    case '^':
        return EOTLT;
    case 'M':
        return Memo;
    case 'L':
        return Category;
    case 'S':
        return CategorySplit;
    case 'E':
        return MemoSplit;
    case '$':
        return AmountSplit;
    case 'C':
        return Status;
    default:
        return UnknownType;
    }
}

IMPLEMENT_DYNAMIC_CLASS( mmQIFImportDialog, wxDialog )

BEGIN_EVENT_TABLE( mmQIFImportDialog, wxDialog )
    EVT_CHECKBOX(wxID_ANY, mmQIFImportDialog::OnCheckboxClick )
    EVT_BUTTON(wxID_OK, mmQIFImportDialog::OnOk)
    EVT_BUTTON(wxID_CANCEL, mmQIFImportDialog::OnCancel)
    EVT_CLOSE(mmQIFImportDialog::OnQuit)
END_EVENT_TABLE()

mmQIFImportDialog::mmQIFImportDialog(
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

bool mmQIFImportDialog::Create( wxWindow* parent, wxWindowID id, const wxString& caption,
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

void mmQIFImportDialog::CreateControls()
{
    wxSizerFlags flags, flagsExpand;
    flags.Align(wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL).Border(wxALL, 5);
    flagsExpand.Align(wxALIGN_CENTER).Border(wxALL, 5).Expand().Proportion(1);

    wxBoxSizer* main_sizer = new wxBoxSizer(wxVERTICAL);
    this->SetSizer(main_sizer);
    wxBoxSizer* box_sizer1 = new wxBoxSizer(wxVERTICAL);

    wxFlexGridSizer* flex_sizer = new wxFlexGridSizer(0, 3, 0, 0);
    //flex_sizer->AddGrowableCol(1);

    // File Name --------------------------------------------
    wxStaticText* file_name_label = new wxStaticText(this, wxID_STATIC, _("File Name:"));
    button_search_ = new wxButton(this, wxID_OPEN, _("Choose &file"));
    button_search_->Connect(wxID_OPEN, wxEVT_COMMAND_BUTTON_CLICKED
        , wxCommandEventHandler(mmQIFImportDialog::OnFileSearch), NULL, this);

    file_name_ctrl_ = new wxTextCtrl(this, wxID_FILE, wxEmptyString,
        wxDefaultPosition, wxDefaultSize, wxTE_READONLY);

    bbFile_ = new wxBitmapButton(this, wxID_ANY, wxBitmap(flag_xpm));
    flex_sizer->Add(file_name_label, flags);
    flex_sizer->Add(button_search_, flags);
    flex_sizer->Add(bbFile_, flags);
    main_sizer->Add(file_name_ctrl_, 0, wxALL|wxGROW, 5);
    box_sizer1->Add(flex_sizer, flagsExpand);

    // Date Format Settings
    wxStaticText* dateFormat = new wxStaticText(this, wxID_STATIC, _("Date Format"));
    choiceDateFormat_ = new wxComboBox(this, wxID_ANY, wxT(""),
        wxDefaultPosition, wxDefaultSize, date_format());
    choiceDateFormat_->Connect(wxID_ANY, wxEVT_COMMAND_COMBOBOX_SELECTED,
        wxCommandEventHandler(mmQIFImportDialog::OnDateMaskChange), NULL, this);

    bbFormat_ = new wxBitmapButton(this, wxID_ANY, wxBitmap(flag_xpm));
    flex_sizer->Add(dateFormat, flags);
    flex_sizer->Add(choiceDateFormat_, flags);
    flex_sizer->Add(bbFormat_, flags);

    // Accounts
    wxStaticText* newAccountsText = new wxStaticText(this, wxID_STATIC, _("Missing Accounts"));
    newAccounts_ = new wxChoice(this, wxID_ANY, wxDefaultPosition, wxSize(150,-1));
    bbAccounts_ = new wxBitmapButton(this, wxID_ANY, wxBitmap(flag_xpm));
    flex_sizer->Add(newAccountsText, flags);
    flex_sizer->Add(newAccounts_, flags);
    flex_sizer->Add(bbAccounts_, flags);

    // From Date --------------------------------------------
    dateFromCheckBox_ = new wxCheckBox( this, wxID_ANY, _("From Date")
        , wxDefaultPosition, wxDefaultSize, wxCHK_2STATE );
    fromDateCtrl_ = new wxDatePickerCtrl( this, wxID_STATIC, wxDefaultDateTime
        , wxDefaultPosition, wxSize(150, -1), wxDP_DROPDOWN);
    fromDateCtrl_->Enable(false);
    flex_sizer->Add(dateFromCheckBox_, flags);
    flex_sizer->Add(fromDateCtrl_, flags);
    flex_sizer->AddSpacer(1);

    // To Date --------------------------------------------
    dateToCheckBox_ = new wxCheckBox( this, wxID_ANY, _("To Date")
        , wxDefaultPosition, wxDefaultSize, wxCHK_2STATE );
    toDateCtrl_ = new wxDatePickerCtrl( this, wxID_STATIC, wxDefaultDateTime
        , wxDefaultPosition, wxSize(150, -1), wxDP_DROPDOWN);
    toDateCtrl_->Enable(false);
    flex_sizer->Add(dateToCheckBox_, flags);
    flex_sizer->Add(toDateCtrl_, flags);
    flex_sizer->AddSpacer(1);

    main_sizer->Add(box_sizer1, flagsExpand);

    /**********************************************************************************************
     Button Panel with OK and Cancel Buttons
    ***********************************************************************************************/
    wxPanel* buttons_panel = new wxPanel(this, wxID_ANY);
    main_sizer->Add(buttons_panel, flags.Center());

    wxStdDialogButtonSizer*  buttons_sizer = new wxStdDialogButtonSizer;
    buttons_panel->SetSizer(buttons_sizer);

    btnOK_ = new wxButton( buttons_panel, wxID_OK, _("&OK"));
    wxButton* itemButtonCancel_ = new wxButton( buttons_panel, wxID_CANCEL, _("&Cancel"));
    btnOK_->Connect(wxID_OK, wxEVT_COMMAND_BUTTON_CLICKED
        , wxCommandEventHandler(mmQIFImportDialog::OnOk), NULL, this);

    buttons_sizer->Add(btnOK_, flags);
    buttons_sizer->Add(itemButtonCancel_, flags);

    buttons_sizer->Realize();
    Fit();
}

void mmQIFImportDialog::fillControls()
{
    dateFormat_ = core_->dbInfoSettings_->GetStringSetting(wxT("DATEFORMAT"), mmex::DEFDATEFORMAT);
    choiceDateFormat_->SetStringSelection(FormatDate2DisplayDate(dateFormat_));

    wxArrayString accounts_type;
    accounts_type.Add(ACCOUNT_TYPE_BANK);
    accounts_type.Add(ACCOUNT_TYPE_TERM);
    accounts_id_ = core_->accountList_.getAccountsID(accounts_type);

    for (size_t i = 0; i < accounts_id_.Count(); ++i)
    {
        accounts_name_.Add(core_->accountList_.GetAccountName(accounts_id_[i]));
    }
    bbFile_ ->SetBitmapLabel(wxBitmap(empty_xpm));
    bbFile_ ->Enable(false);
    bbFormat_->SetBitmapLabel(wxBitmap(empty_xpm));
    bbFormat_->Enable(false);
    bbAccounts_->SetBitmapLabel(wxBitmap(empty_xpm));
    bbAccounts_->Enable(false);
    newAccounts_->Enable(false);
    btnOK_->Enable(false);
}

bool mmQIFImportDialog::isLineOK(const wxString& line)
{
    return wxString(wxT("!DNPAT^MLSE$C/UI")).Contains(line.Left(1));
}

wxString mmQIFImportDialog::getLineData(const wxString& line)
{
    wxString dataString = line.substr(1, line.Length()-1);
    return dataString;
}

wxString mmQIFImportDialog::getFileLine(wxTextInputStream& textFile, int& lineNumber)
{
    wxString textLine = textFile.ReadLine();
    lineNumber ++;
    textLine.Trim(); // remove any trailing spaces

    return textLine;
}

wxString mmQIFImportDialog::getFinancistoProject(wxString& sSubCateg)
{
    //Additional parsint for Financisto
    wxString sProject = wxT("");
    wxStringTokenizer cattkz(sSubCateg, wxT("/"));

    sSubCateg = cattkz.GetNextToken();
    if (cattkz.HasMoreTokens())
        sProject = cattkz.GetNextToken();
    return sProject;
}

bool mmQIFImportDialog::warning_message()
{
    wxString msgStr;
    msgStr << _("To import QIF files correctly, the date format in the QIF file must match the date option set in MMEX.") << wxT("\n\n")
           << _("Are you are sure you want to proceed with the import?");
    wxMessageDialog msgDlg(NULL, msgStr, _("QIF Import"), wxYES_NO|wxICON_QUESTION);
    if (msgDlg.ShowModal() != wxID_YES)
        return false;
    return true;
}

int mmQIFImportDialog::mmImportQIF()
{
    wxString acctName, sMsg;

    //Check date restrictions
    wxDateTime fromDate = wxDateTime::Now(), toDate = wxDateTime::Now();
    bool bFromDate = dateFromCheckBox_->IsChecked();
    bool bToDate = dateToCheckBox_->IsChecked();
    if (bFromDate)
        fromDate = fromDateCtrl_->GetValue().GetDateOnly();
    if (bToDate)
        toDate = toDateCtrl_->GetValue().GetDateOnly();

    wxArrayString accounts_name = core_->accountList_.getAccountsName();
    int fromAccountID = -1;

    wxString sDefCurrencyName = core_->currencyList_.getCurrencyName(core_->currencyList_.GetBaseCurrencySettings());

    fileviewer file_dlg(wxT(""), parent_);
    file_dlg.Show();
    wxTextCtrl*& logWindow = file_dlg.textCtrl_;
    bool canceledbyuser = false;

    wxFileInputStream input(sFileName_);
    wxTextInputStream text(input);

    wxString readLine;
    int numLines = 0;
    int trxNumLine = 1;
    int numImported = 0;

    wxString dt = wxDateTime::Now().FormatISODate();
    wxString sPayee, type, sAmount, transNum, notes, convDate, sToAccountName;
    wxString sFullCateg, sCateg, sSubCateg, sSplitCategs, sSplitAmount, sValid;

    wxDateTime dtdt = wxDateTime::Now().GetDateOnly();
    int payeeID = -1, categID = -1, subCategID = -1, to_account_id = -1, from_account_id = -1;
    double val = 0.0, dSplitAmount = 0.0;
    bool bTrxComplited = true;

    std::vector< boost::shared_ptr<mmBankTransaction> > vQIF_trxs;
    boost::shared_ptr< mmSplitTransactionEntries > mmSplit(new mmSplitTransactionEntries());

    while(!input.Eof() && !canceledbyuser)
    {
        //Init variables for each transaction
        if (bTrxComplited)
        {
            sSplitAmount.clear();
            sSplitCategs.clear();
            mmSplit->entries_.clear();

            sPayee.clear();
            type.clear();
            sFullCateg.clear();
            sCateg.clear();
            sSubCateg.clear();
            subCategID = -1;
            categID = -1;
            sAmount.clear();
            val = 0.0;
            dSplitAmount = 0.0;
            transNum = wxT("");
            notes = wxT("");
            convDate = wxDateTime::Now().FormatISODate();

            bTrxComplited = false;
            trxNumLine = numLines - 1;
            sMsg = wxT("-------------------------------------------------------------------------------------------------------------------------\n");
            logWindow->AppendText(sMsg);
        }
        readLine = getFileLine(text, numLines);

        if (readLine.Length() == 0)
            continue;

        bool isOK = isLineOK(readLine);
        if (!isOK)
        {
            sMsg = wxString()<< _("Line: ") << numLines << wxT("  ") << _(" Unknown QIF line: ") << readLine;
            logWindow->AppendText(wxString() << sMsg << wxT("\n"));
            continue;
        }

        if (lineType(readLine) == AcctType)
        {
            wxString accountType = getLineData(readLine);
            if  ((!accountType.CmpNoCase(wxT("Type:Bank")))
                    || (!accountType.CmpNoCase(wxT("Type:Cash")))
                    || (!accountType.CmpNoCase(wxT("Type:CCard")))
                    || (!accountType.CmpNoCase(wxT("Type:Oth L")))
                )
            {
                sMsg = wxString::Format(_("Importing account type: %s"), accountType.c_str());
                logWindow->AppendText(sMsg << wxT("\n"));
                bTrxComplited = true;
                continue;
            }

            if ( accountType == wxT("Type:Cat") )
            {
                bool reading = true;
                while(!input.Eof() && reading )
                {
                    readLine = getFileLine(text, numLines);
                    if (lineType(readLine) == AcctType  || input.Eof())
                    {
                        reading = false;
                        accountType = getLineData(readLine);
                    }
                }
            }

            if ( accountType == wxT("Account"))
            {
                wxString sDescription = wxT("");
                wxString sBalance = wxT("");
                // account information
                // Need to read till we get to end of account information
                while( (readLine = getFileLine(text, numLines) ) != wxT("^"))
                {
                    numLines++;

                    int i = accountInfoType(readLine);
                    if (i == Name)
                    {
                        acctName = getLineData(readLine);
                        continue;
                    }
                    else if (i == Description)
                    {
                        //TODO: Get currency symbol if provided (huck)
                        sDescription = getLineData(readLine);
                        continue;
                    }
                    else if (i == Balance)
                    {
                        sBalance = getLineData(readLine);
                        if (!sBalance.ToDouble(&val) && !mmex::formatCurrencyToDouble(sBalance, val))
                            val = 0;
                        continue;
                    }
                    else if (i == AccountType || i == CreditLimit || i  == BalanceDate)
                    {
                        continue;
                    }
                }

                if (core_->accountList_.GetAccountId(acctName) < 0)
                {
                    //TODO: Repeated code
                    mmAccount* ptrBase = new mmAccount();
                    boost::shared_ptr<mmAccount> pAccount(ptrBase);

                    pAccount->favoriteAcct_ = true;
                    pAccount->status_ = mmAccount::MMEX_Open;
                    pAccount->acctType_ = ACCOUNT_TYPE_BANK;
                    pAccount->name_ = acctName;
                    pAccount->initialBalance_ = val;
                    pAccount->currency_ = core_->currencyList_.getCurrencySharedPtr(sDefCurrencyName);
                    // prevent same account being added multiple times in case of using 'Back' and 'Next' in wizard.
                    if ( ! core_->accountList_.AccountExists(pAccount->name_))
                        from_account_id = core_->accountList_.AddAccount(pAccount);
                    accounts_name.Add(pAccount->name_);
                    acctName = pAccount->name_;
                    sMsg = wxString::Format(_("Added account '%s'"), acctName.c_str())
                        << wxT("\n") << wxString::Format(_("Initial Balance: %s"), (wxString()<<val).c_str());
                    logWindow->AppendText(wxString()<< sMsg << wxT("\n"));
                }

                fromAccountID = core_->accountList_.GetAccountId(acctName);

                sMsg = wxString::Format(_("Line: %ld"), numLines) << wxT(" : ")
                    << wxString::Format(_("Account name: %s"), acctName.c_str());
                logWindow->AppendText(wxString()<< sMsg << wxT("\n"));

                continue;
            }

            // ignore these type of lines
            if ( accountType == wxT("Option:AutoSwitch") )
            {
                while((readLine = getFileLine(text, numLines)) != wxT("^") || input.Eof())
                {
                    // ignore all lines
                }
                continue;
            }
            else if ( accountType == wxT("Type:Security") || accountType == wxT("Clear:AutoSwitch"))
            {
                continue;
            }
            // we do not know how to process this type yet
            wxString errMsgStr = _("Cannot process these QIF Account Types yet.");
            wxString errLineMsgStr = wxString::Format(_("Line: %ld"), numLines)
                << wxT("\n") << readLine;

            logWindow->AppendText(wxString()<< errLineMsgStr << wxT("\n") << errMsgStr << wxT("\n"));
            wxMessageBox( errLineMsgStr + wxT("\n\n") + errMsgStr, _("QIF Import"), wxICON_ERROR);

            // exit: while(!input.Eof()) loop and allow to exit routine and allow user to save or abort
            break;
        }

        to_account_id = -1;
        from_account_id = fromAccountID;
        bool bValid = true;

        if (lineType(readLine) == Date) // 'D'
        {
            dt = getLineData(readLine);

            dtdt = mmParseDisplayStringToDate(dt, dateFormat_).GetDateOnly();
            convDate = dtdt.FormatISODate();
            continue;
        }
        else if (lineType(readLine) == Amount) // 'T'
        {
            sAmount = getLineData(readLine);
            // Until the value has been received, we don't know transaction type
            // At the same time we don't know it transfer or no
            // Therefore the type of transaction defined as withdrawal
            type = TRANS_TYPE_WITHDRAWAL_STR;

            if (!sAmount.ToDouble(&val) && !mmex::formatCurrencyToDouble(sAmount, val))
            {
                sMsg = wxString::Format(_("Line: %ld invalid amount, skipping."), numLines);
                logWindow->AppendText(sMsg << wxT("\n"));
                continue;
            }
            continue;
        }
        else if (lineType(readLine) == Payee) // 'P'
        {
            sPayee = getLineData(readLine);
            continue;
        }
        else if (lineType(readLine) == TransNumber) // 'N'
        {
            transNum = getLineData(readLine);
            continue;
        }
        else if (lineType(readLine) == Memo || lineType(readLine) == MemoSplit ) // 'M' // 'E'
        {
            notes << getLineData(readLine) << wxT("\n");
            continue;
        }
        else if (lineType(readLine) == Category || lineType(readLine) == CategorySplit) // 'S' // 'L'
        {
            sFullCateg = getLineData(readLine);

            if (sFullCateg.Left(1).Contains(wxT("[")) && sFullCateg.Right(1).Contains(wxT("]")))
            {
                sToAccountName = sFullCateg.substr(1, sFullCateg.Length()-2);
                sFullCateg = _("Transfer");
            }

            /* //Trick  for cut non standart qif category usage in Financisto application
            //Category field may contains additional information like Project
            //Format Category[:Subcategory][/Project] //*/
            if (sFullCateg.Contains(wxT("/")))
                transNum.Prepend(wxString::Format(wxT("[%s] "), getFinancistoProject(sFullCateg).c_str()));

            core_->categoryList_.parseCategoryString(sFullCateg, sCateg, categID, sSubCateg, subCategID);

            if (categID == -1 && !sCateg.IsEmpty())
            {
                categID =  core_->categoryList_.AddCategory(sCateg);
                sMsg = wxString::Format(_("Added category: %s"), sCateg.c_str());
                logWindow->AppendText(sMsg << wxT("\n"));
            }
            if (subCategID == -1 && categID != -1 && !sSubCateg.IsEmpty())
            {
                subCategID = core_->categoryList_.AddSubCategory(categID, sSubCateg);
                sMsg = wxString::Format(_("Added subcategory: %s"), sSubCateg.c_str());
                logWindow->AppendText(sMsg << wxT("\n"));
            }

            continue;
        }
        else if (lineType(readLine) == AmountSplit) // '$'
        {
            sSplitAmount = getLineData(readLine);

            //get amount
            if (!sSplitAmount.ToDouble(&dSplitAmount) && !mmex::formatCurrencyToDouble(sSplitAmount, dSplitAmount))
                dSplitAmount = 0; //wrong amount
            //
            if (type == TRANS_TYPE_WITHDRAWAL_STR)
                dSplitAmount = -dSplitAmount;
            //Add split entry
            boost::shared_ptr<mmSplitTransactionEntry> pSplitEntry(new mmSplitTransactionEntry);
            pSplitEntry->splitAmount_  = dSplitAmount;
            pSplitEntry->categID_      = categID;
            pSplitEntry->subCategID_   = subCategID;

            mmSplit->addSplit(pSplitEntry);
            continue;
        }
        //MemoSplit
        else if (lineType(readLine) == Address) // 'A'
        {
            notes << getLineData(readLine) << wxT("\n");
            continue;
        }
        else if (lineType(readLine) == EOTLT) // ^
        {
            wxString status = wxT("F");

            if (dt.Trim().IsEmpty())
            {
                sMsg = _("Date is missing");
                logWindow->AppendText(wxString()<< sMsg << wxT("\n"));
                bValid = false;
            }
            else if (type.Trim().IsEmpty())
            {
                sMsg = _("Transaction Type is missing");
                logWindow->AppendText(sMsg << wxT("\n"));
                bValid = false;
            }
            else if (sAmount.Trim().IsEmpty())
            {
                sMsg = _("Amount is missing");
                logWindow->AppendText(sMsg << wxT("\n"));
                bValid = false;
            }

            if (sFullCateg.Trim().IsEmpty() && type != TRANS_TYPE_TRANSFER_STR)
            {
                sMsg = _("Category is missing");
                logWindow->AppendText(sMsg << wxT("\n"));
                sFullCateg = _("Unknown");

                core_->categoryList_.parseCategoryString(sFullCateg, sCateg, categID, sSubCateg, subCategID);
                if (categID == -1 && !sCateg.IsEmpty())
                {
                    categID =  core_->categoryList_.AddCategory(sCateg);
                    sMsg = wxString::Format(_("Added category: %s"), sCateg.c_str());
                    logWindow->AppendText(sMsg << wxT("\n"));
                }
            }

            to_account_id = -1;
            if (sFullCateg == _("Transfer"))
            {
                if (accounts_name.Index(sToAccountName) == wxNOT_FOUND)
                {
                    mmAccount* ptrBase = new mmAccount();
                    boost::shared_ptr<mmAccount> pAccount(ptrBase);

                    pAccount->favoriteAcct_ = true;
                    pAccount->status_ = mmAccount::MMEX_Open;
                    pAccount->acctType_ = ACCOUNT_TYPE_BANK;
                    pAccount->name_ = sToAccountName;
                    pAccount->initialBalance_ = 0;
                    pAccount->currency_ = core_->currencyList_.getCurrencySharedPtr(sDefCurrencyName);
                    // prevent same account being added multiple times in case of using 'Back' and 'Next' in wizard.
                    if ( ! core_->accountList_.AccountExists(pAccount->name_))
                        to_account_id = core_->accountList_.AddAccount(pAccount);
                    accounts_name.Add(sToAccountName);

                    sMsg = wxString::Format(_("Added account '%s'"), sToAccountName.c_str());
                    logWindow->AppendText(wxString()<< sMsg << wxT("\n"));
                }
                to_account_id = core_->accountList_.GetAccountId(sToAccountName);
                if (val > 0.0)
                {
                    from_account_id = to_account_id;
                    to_account_id = fromAccountID;
                }
                type = TRANS_TYPE_TRANSFER_STR;
            }

            if (val > 0.0 && type != TRANS_TYPE_TRANSFER_STR)
                type = TRANS_TYPE_DEPOSIT_STR;

            if (type == TRANS_TYPE_TRANSFER_STR)
            {
                payeeID = -1;
                if (to_account_id == -1 || from_account_id == -1)
                {
                    sMsg = _("Account missing");
                    logWindow->AppendText(sMsg << wxT("\n"));
                    bValid = false;
                }
            }
            else
            {
                to_account_id = -1;
                if (sPayee.IsEmpty())
                {
                    sMsg = _("Payee missing");
                    logWindow->AppendText(sMsg << wxT("\n"));
                    sPayee = _("Unknown");
                }
                if (!core_->payeeList_.PayeeExists(sPayee))
                {
                    payeeID = core_->payeeList_.AddPayee(sPayee);
                    sMsg = wxString::Format(_("Payee Added: %s"), sPayee.c_str());
                    logWindow->AppendText(wxString()<< sMsg << wxT("\n"));
                }
                else
                    payeeID = core_->payeeList_.GetPayeeId(sPayee);
            }
            if (mmSplit->entries_.size() > 0)
            {
                categID = -1;
                sFullCateg = _("Split Category");
            }
            else
            {
                sFullCateg = core_->categoryList_.GetFullCategoryString(categID, subCategID);
            }

            if (bValid)
                sValid = wxT("OK");
            else
                sValid = wxT("NO");

            if (bFromDate && dtdt < fromDate || bToDate && dtdt > toDate)
            {
                sValid = wxT("SKIP");
                bValid = false;
            }
            sMsg = wxString::Format(
                wxT("Line:%ld Trx:%ld %s D:%s Acc:'%s' %s P:'%s%s' Amt:%s C:'%s' \n")
                , trxNumLine
                , numImported + 1
                , sValid.c_str()
                , convDate.c_str()
                , core_->accountList_.GetAccountName(from_account_id).c_str()
                , wxString((type == TRANS_TYPE_TRANSFER_STR ? wxT("<->") : wxT(""))).c_str()
                , core_->accountList_.GetAccountName(to_account_id).c_str()
                , core_->payeeList_.GetPayeeName(payeeID).c_str()
                , (wxString()<<val).c_str()
                , sFullCateg.c_str()
                );
            logWindow->AppendText(sMsg);

            for (size_t i = 0; i < mmSplit->entries_.size(); ++i)
            {
                int c = mmSplit->entries_[i]->categID_;
                int s = mmSplit->entries_[i]->subCategID_;

                wxString cn = core_->categoryList_.GetCategoryName(c);
                wxString sn = core_->categoryList_.GetSubCategoryName(c, s);
                double v = mmSplit->entries_[i]->splitAmount_;
                sMsg = (cn << wxT(":") << sn << wxT(" ") << v << wxT("\n"));
                logWindow->AppendText(sMsg);
            }
            bTrxComplited = true;
            if (!bValid) continue;

            boost::shared_ptr<mmBankTransaction> pTransaction(new mmBankTransaction(core_->db_));
            pTransaction->date_ = dtdt;
            pTransaction->accountID_ = from_account_id;
            pTransaction->toAccountID_ = to_account_id;
            pTransaction->payee_ = core_->payeeList_.GetPayeeSharedPtr(payeeID);
            pTransaction->payeeStr_ = sPayee;
            pTransaction->transType_ = type;
            pTransaction->amt_ = val;
            pTransaction->status_ = status;
            pTransaction->transNum_ = transNum;
            pTransaction->notes_ = notes;
            pTransaction->toAmt_ = val;
            if (mmSplit->numEntries()) categID = -1;
            pTransaction->category_ = core_->categoryList_.GetCategorySharedPtr(categID, subCategID);
            *pTransaction->splitEntries_ = *mmSplit;

            //For any transfer transaction always mirrored transaction present
            //Just take alternate amount and skip it
            if (type == TRANS_TYPE_TRANSFER_STR)
            {
                std::vector<boost::shared_ptr<mmBankTransaction> >& refTrans = vQIF_trxs;
                for (unsigned int index = 0; index < vQIF_trxs.size(); index++)
                {
                    if (refTrans[index]->transType_ != TRANS_TYPE_TRANSFER_STR) continue;
                    if (refTrans[index]->status_ == wxT("D")) continue;
                    if (refTrans[index]->date_!= dtdt) continue;
                    if (refTrans[index]->amt_ < 0 && val < 0 || refTrans[index]->amt_ > 0 && val >0 ) continue;
                    if (refTrans[index]->accountID_!= from_account_id) continue;
                    if (refTrans[index]->transNum_ != transNum) continue;
                    if (refTrans[index]->notes_ != notes) continue;

                    if (val > 0.0)
                        refTrans[index]->toAmt_ = val;
                    else
                        refTrans[index]->amt_ = val;
                    refTrans[index]->status_ = wxT("D");

                    sMsg = wxString::Format(wxT("%f -> %f (%f)\n"), refTrans[index]->amt_
                        , refTrans[index]->toAmt_
                        , (fabs(refTrans[index]->amt_)/fabs(refTrans[index]->toAmt_)<1)
                            ? fabs(refTrans[index]->toAmt_)/fabs(refTrans[index]->amt_)
                            : fabs(refTrans[index]->amt_)/fabs(refTrans[index]->toAmt_));
                    logWindow->AppendText(sMsg);

                    bValid = false;
                    break;
                }
            }

            if (bValid)
            {
                vQIF_trxs.push_back(pTransaction);
                numImported++;
            }
        }
    }

    sMsg = wxString::Format(_("Transactions imported from QIF: %ld"), numImported);
    logWindow->AppendText(sMsg << wxT("\n"));

    canceledbyuser = file_dlg.ShowModal() == wxID_CANCEL;
    // Since all database transactions are only in memory,
    if (!canceledbyuser)
    {
        core_->db_.get()->Begin();

        std::vector<boost::shared_ptr<mmBankTransaction> >& refTrans = vQIF_trxs;

        //TODO: Update transfer transactions toAmount

        for (unsigned int index = 0; index < vQIF_trxs.size(); index++)
        {
            //fromAccountID = refTrans[index]->accountID_;
            boost::shared_ptr<mmCurrency> pCurrencyPtr = core_->accountList_.getCurrencyWeakPtr(fromAccountID).lock();
            wxASSERT(pCurrencyPtr);
            refTrans[index]->amt_ = fabs(refTrans[index]->amt_);
            refTrans[index]->toAmt_ = fabs(refTrans[index]->toAmt_);
            refTrans[index]->status_ = wxT("F");
            refTrans[index]->updateAllData(core_, fromAccountID, pCurrencyPtr);
            core_->bTransactionList_.addTransaction(core_, refTrans[index]);
        }

        core_->db_.get()->Commit();
        sMsg = _("Import finished successfully");
    }
    else
    {
        sMsg = _("Imported transactions discarded by user!");
    }

    wxMessageDialog(parent_, sMsg, _("QIF Import"), wxOK|wxICON_WARNING).ShowModal();
    //clear the vector to avoid memory leak - done at same level created.
    vQIF_trxs.clear();

    return fromAccountID;
}

void mmQIFImportDialog::OnFileSearch(wxCommandEvent& /*event*/)
{
    sFileName_ = file_name_ctrl_->GetValue();

    const wxString choose_ext = _("QIF Files");
    sFileName_ = wxFileSelector(_("Choose QIF data file to Import"),
        wxEmptyString, sFileName_, wxEmptyString,
        choose_ext + wxT(" (*.qif)|*.qif;*.QIF")
            , wxFD_OPEN|wxFD_CHANGE_DIR|wxFD_FILE_MUST_EXIST);

    if (!sFileName_.IsEmpty())
        correctEmptyFileExt( wxT("qif"), sFileName_);
    file_name_ctrl_->SetValue(sFileName_);
    checkQIFFile(sFileName_);
}

bool mmQIFImportDialog::checkQIFFile(wxString fileName)
{
    if (fileName.IsEmpty()) return false;

    bbFile_->Enable(false);
    bbFile_->SetBitmapLabel(wxBitmap(empty_xpm));
    bbFormat_->Enable(false);
    bbFormat_->SetBitmapLabel(wxBitmap(empty_xpm));
    newAccounts_->Clear();
    bbAccounts_->SetBitmapLabel(wxBitmap(empty_xpm));
    btnOK_->Enable(false);

    wxFileInputStream input(sFileName_);
    wxTextInputStream text(input);

    wxString readLine;

    wxString sAccountName;
    bool dateFormatIsOK = true;
    int numLines = 0;

    while(!input.Eof())
    {
        readLine = getFileLine(text, numLines);

        if (readLine.Length() == 0)
            continue;

        if (!isLineOK(readLine))
        {
            return false;
        }

        if ( lineType(readLine) == AcctType && getLineData(readLine) == wxT("Account"))
        {
            bool reading = true;
            while(!input.Eof() && reading )
            {
                readLine = getFileLine(text, numLines);
                if (accountInfoType(readLine) == Name)
                {
                    sAccountName = getLineData(readLine);
                    if (accounts_name_.Index(sAccountName) == wxNOT_FOUND)
                    {
                        newAccounts_->Append(sAccountName);
                    }
                }
                reading = (accountInfoType(readLine) != EOT);
            }
            continue;
        }

        if (lineType(readLine) == Date)
        {
            wxDateTime dtdt;
            wxString sDate = getLineData(readLine);
            if (!dtdt.ParseFormat(sDate, dateFormat_, wxDateTime::Now()))
                dateFormatIsOK = false;
            continue;
        }
    }

    bbFile_->Enable(true);
    bbFile_->SetBitmapLabel(wxBitmap(flag_xpm));
    bbFormat_->Enable(dateFormatIsOK);
    if (dateFormatIsOK)
        bbFormat_->SetBitmapLabel(wxBitmap(flag_xpm));
    if (newAccounts_->GetCount() != 0)
    {
        newAccounts_->Enable(true);
        newAccounts_->SetSelection(0);
    }
    else
    {
        bbAccounts_->SetBitmapLabel(wxBitmap(flag_xpm));
    }
    if (dateFormatIsOK)
    {
        btnOK_->Enable(true);
    }
    return true;
}

void mmQIFImportDialog::OnDateMaskChange(wxCommandEvent& /*event*/)
{
    dateFormat_ = DisplayDate2FormatDate(choiceDateFormat_->GetValue());
    checkQIFFile(sFileName_);
}

void mmQIFImportDialog::OnCheckboxClick( wxCommandEvent& /*event*/ )
{
    fromDateCtrl_->Enable(dateFromCheckBox_->GetValue());
    toDateCtrl_->Enable(dateToCheckBox_->GetValue());

}

void mmQIFImportDialog::OnOk(wxCommandEvent& /*event*/)
{
    mmImportQIF();
}

void mmQIFImportDialog::OnCancel(wxCommandEvent& /*event*/)
{
    EndModal(wxID_CANCEL);
}

void mmQIFImportDialog::OnQuit(wxCloseEvent& /*event*/)
{
    EndModal(wxID_CANCEL);
}

