
#include "qif_import.h"
#include "constants.h"
#include "util.h"
#include "mmCurrencyFormatter.h"
#include "fileviewerdialog.h"
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
    mmCoreDB* core
    , wxWindow* parent, wxWindowID id
    , const wxString& caption, const wxPoint& pos
    , const wxSize& size, long style
) :
    core_(core)
    , parent_(parent)
    , last_imported_acc_id_(-1)
{
    Create(parent, id, caption, pos, size, style);
}

bool mmQIFImportDialog::Create( wxWindow* parent, wxWindowID id, const wxString& caption,
                           const wxPoint& pos, const wxSize& size, long style )
{
    SetExtraStyle(GetExtraStyle()|wxWS_EX_BLOCK_EVENTS);
    wxDialog::Create( parent, id, caption, pos, size, style );

    ColName_[COL_DATE]     = _("Date");
    ColName_[COL_NUMBER]   = _("Number");
    ColName_[COL_PAYEE]    = _("Payee");
    ColName_[COL_STATUS]   = _("Status");
    ColName_[COL_CATEGORY] = _("Category");
    ColName_[COL_VALUE]    = _("Value");
    ColName_[COL_NOTES]    = _("Notes");

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
    wxBoxSizer* left_sizer = new wxBoxSizer(wxVERTICAL);

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
    left_sizer->Add(flex_sizer, flagsExpand);

    // Date Format Settings
    dateFormat_ = mmOptions::instance().dateFormat_;

    wxStaticText* dateFormat = new wxStaticText(this, wxID_STATIC, _("Date Format"));
    choiceDateFormat_ = new wxComboBox(this, wxID_ANY);
    for (const auto& i : date_formats_map())
    {
        choiceDateFormat_->Append(i.second, new wxStringClientData(i.first));
        if (dateFormat_ == i.first) choiceDateFormat_->SetStringSelection(i.second);
    }
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

    //Log viewer
    wxBoxSizer* log_sizer = new wxBoxSizer(wxVERTICAL);

    log_field_ = new wxTextCtrl( this, wxID_STATIC, "", wxDefaultPosition, wxSize(300, -1), wxTE_MULTILINE|wxHSCROLL );
    log_sizer->Add(log_field_, 1, wxGROW|wxALL, 5);

    wxButton* itemClearButton = new wxButton(this, wxID_CLEAR, _("Clear"));
    log_sizer->Add(itemClearButton, 0, wxALIGN_CENTER|wxALL, 5);
    itemClearButton->Connect(wxID_CLEAR, wxEVT_COMMAND_BUTTON_CLICKED
        , wxCommandEventHandler(mmQIFImportDialog::OnButtonClear), NULL, this);

    //Data viewer
    wxPanel* data_panel = new wxPanel(this, wxID_ANY);
    wxBoxSizer* data_sizer = new wxBoxSizer(wxHORIZONTAL);
    data_panel->SetSizer(data_sizer);

    dataListBox_ = new wxDataViewListCtrl(data_panel
        , wxID_ANY, wxDefaultPosition, wxSize(100, 200));
    dataListBox_->AppendTextColumn( ColName_[COL_DATE], wxDATAVIEW_CELL_INERT, 140);
    dataListBox_->AppendTextColumn( ColName_[COL_NUMBER], wxDATAVIEW_CELL_INERT, 80);
    dataListBox_->AppendTextColumn( ColName_[COL_PAYEE], wxDATAVIEW_CELL_INERT, 120);
    dataListBox_->AppendTextColumn( ColName_[COL_STATUS], wxDATAVIEW_CELL_INERT, 60);
    dataListBox_->AppendTextColumn( ColName_[COL_CATEGORY], wxDATAVIEW_CELL_INERT, 120);
    dataListBox_->AppendTextColumn( ColName_[COL_VALUE], wxDATAVIEW_CELL_INERT, 100);
    dataListBox_->AppendTextColumn( ColName_[COL_NOTES], wxDATAVIEW_CELL_INERT, 300);
    data_sizer->Add(dataListBox_, flagsExpand);

    //Compose all sizers togethe
    wxBoxSizer* top_sizer = new wxBoxSizer(wxHORIZONTAL);
    main_sizer->Add(top_sizer, flagsExpand);
    top_sizer->Add(left_sizer, flags);
    top_sizer->Add(log_sizer, flagsExpand);
    main_sizer->Add(data_panel, flagsExpand);

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
    wxArrayString accounts_type;
    accounts_type.Add(ACCOUNT_TYPE_BANK);
    accounts_type.Add(ACCOUNT_TYPE_TERM);
    accounts_id_ = core_->accountList_.getAccountsID(accounts_type);

    for (const auto& entry : accounts_id_)
    {
        accounts_name_.Add(core_->accountList_.GetAccountName(entry));
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
    return wxString("!DNPAT^MLSE$C/UI").Contains(line.Left(1));
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
    wxString sProject = "";
    wxStringTokenizer cattkz(sSubCateg, "/");

    sSubCateg = cattkz.GetNextToken();
    if (cattkz.HasMoreTokens())
        sProject = cattkz.GetNextToken();
    return sProject;
}

bool mmQIFImportDialog::warning_message()
{
    wxString msgStr;
    msgStr << _("To import QIF files correctly, the date format in the QIF file must match the date option set in MMEX.") << "\n\n"
           << _("Are you are sure you want to proceed with the import?");
    wxMessageDialog msgDlg(NULL, msgStr, _("QIF Import"), wxYES_NO|wxICON_QUESTION);
    if (msgDlg.ShowModal() != wxID_YES)
        return false;
    return true;
}

int mmQIFImportDialog::mmImportQIF(wxTextFile& tFile)
{
    wxString acctName = core_->accountList_.GetAccountName(last_imported_acc_id_);
    fromAccountID_ = core_->accountList_.GetAccountId(acctName);
    wxString sMsg;

    //Check date restrictions
    wxDateTime fromDate = wxDateTime::Now(), toDate = wxDateTime::Now();
    bool bFromDate = dateFromCheckBox_->IsChecked();
    bool bToDate = dateToCheckBox_->IsChecked();
    if (bFromDate)
        fromDate = fromDateCtrl_->GetValue().GetDateOnly();
    if (bToDate)
        toDate = toDateCtrl_->GetValue().GetDateOnly();

    wxArrayString accounts_name = core_->accountList_.getAccountsName();

    wxString sDefCurrencyName = core_->currencyList_.getCurrencyName(core_->currencyList_.GetBaseCurrencySettings());

    wxTextCtrl*& logWindow = log_field_;

    wxString readLine;
    int numLines = 0;
    int trxNumLine = 1;

    wxString dt = wxDateTime::Now().FormatISODate();
    wxString sPayee, type, sAmount, transNum, notes, convDate, sToAccountName;
    wxString sFullCateg, sCateg, sSubCateg, sSplitCategs, sSplitAmount, sValid;

    wxDateTime dtdt = wxDateTime::Now().GetDateOnly();
    int payeeID = -1, categID = -1, subCategID = -1, to_account_id = -1, from_account_id = -1;
    double val = 0.0, dSplitAmount = 0.0;
    bool bTrxComplited = true;

    mmSplitTransactionEntries* mmSplit(new mmSplitTransactionEntries());

    for (readLine = tFile.GetFirstLine(); !tFile.Eof(); readLine = tFile.GetNextLine())
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
            transNum = "";
            notes = "";
            convDate = wxDateTime::Now().FormatISODate();

            bTrxComplited = false;
            trxNumLine = numLines - 1;
            sMsg = "-------------------------------------------------------------------------------------------------------------------------\n";
            logWindow->AppendText(sMsg);
        }

        if (readLine.Length() == 0)
            continue;

        bool isOK = isLineOK(readLine);
        if (!isOK)
        {
            sMsg = wxString()<< _("Line: ") << numLines << "  " << _(" Unknown QIF line: ") << readLine;
            logWindow->AppendText(wxString() << sMsg << "\n");
            continue;
        }

        if (lineType(readLine) == AcctType)
        {
            wxString accountType = getLineData(readLine);
            if  ((!accountType.CmpNoCase("Type:Bank"))
                    || (!accountType.CmpNoCase("Type:Cash"))
                    || (!accountType.CmpNoCase("Type:CCard"))
                    || (!accountType.CmpNoCase("Type:Oth L"))
                )
            {
                sMsg = wxString::Format(_("Importing account type: %s"), accountType);
                logWindow->AppendText(sMsg << "\n");
                bTrxComplited = true;
                continue;
            }

            if ( accountType == "Type:Cat" )
            {
                bool reading = true;
                while(!tFile.Eof() && reading )
                {
                    readLine = tFile.GetNextLine();
                    if (lineType(readLine) == AcctType  || tFile.Eof())
                    {
                        reading = false;
                        accountType = getLineData(readLine);
                    }
                }
            }

            if ( accountType == "Account")
            {
                wxString sDescription = "";
                wxString sBalance = "";
                // account information
                // Need to read till we get to end of account information
                while( (readLine = tFile.GetNextLine() ) != "^")
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
                        if (!sBalance.ToDouble(&val) && ! CurrencyFormatter::formatCurrencyToDouble(sBalance, val))
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
                    mmAccount* pAccount(ptrBase);

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
                    sMsg = wxString::Format(_("Added account '%s'"), acctName)
                        << "\n" << wxString::Format(_("Initial Balance: %s"), (wxString()<<val));
                    logWindow->AppendText(wxString()<< sMsg << "\n");
                }

                fromAccountID_ = core_->accountList_.GetAccountId(acctName);

                sMsg = wxString::Format(_("Line: %ld"), numLines) << " : "
                    << wxString::Format(_("Account name: %s"), acctName);
                logWindow->AppendText(wxString()<< sMsg << "\n");

                continue;
            }

            // ignore these type of lines
            if ( accountType == "Option:AutoSwitch" )
            {
                readLine = tFile.GetNextLine();
                while(readLine != "^" || tFile.Eof())
                {
                    // ignore all lines
                }
                continue;
            }
            else if ( accountType == "Type:Security" || accountType == "Clear:AutoSwitch")
            {
                continue;
            }
            // we do not know how to process this type yet
            wxString errMsgStr = _("Cannot process these QIF Account Types yet.");
            wxString errLineMsgStr = wxString::Format(_("Line: %ld"), numLines)
                << "\n" << readLine;

            logWindow->AppendText(wxString()<< errLineMsgStr << "\n" << errMsgStr << "\n");
            wxMessageBox( errLineMsgStr + "\n\n" + errMsgStr, _("QIF Import"), wxICON_ERROR);

            // exit: while(!input.Eof()) loop and allow to exit routine and allow user to save or abort
            break;
        }

        to_account_id = -1;
        from_account_id = fromAccountID_;
        bool bValid = true;

        if (lineType(readLine) == Date) // 'D'
        {
            dt = getLineData(readLine);

            mmParseDisplayStringToDate(dtdt, dt, dateFormat_);
            dtdt = dtdt.GetDateOnly();
            convDate = dtdt.FormatISODate();
            continue;
        }
        else if (lineType(readLine) == Amount) // 'T'
        {
            sAmount = getLineData(readLine);

            if (!sAmount.ToDouble(&val) && ! CurrencyFormatter::formatCurrencyToDouble(sAmount, val))
            {
                sMsg = wxString::Format(_("Line: %ld invalid amount, skipping."), numLines);
                logWindow->AppendText(sMsg << "\n");
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
            notes << getLineData(readLine) << "\n";
            continue;
        }
        else if (lineType(readLine) == Category || lineType(readLine) == CategorySplit) // 'S' // 'L'
        {
            sFullCateg = getLineData(readLine);

            if (sFullCateg.Left(1).Contains("[") && sFullCateg.Right(1).Contains("]"))
            {
                sToAccountName = sFullCateg.substr(1, sFullCateg.Length()-2);
                sFullCateg = _("Transfer");
                type = TRANS_TYPE_TRANSFER_STR;
            }

            /* //Trick  for cut non standart qif category usage in Financisto application
            //Category field may contains additional information like Project
            //Format Category[:Subcategory][/Project] //*/
            if (sFullCateg.Contains("/"))
                transNum.Prepend(wxString::Format("[%s] ", getFinancistoProject(sFullCateg)));

            core_->categoryList_.parseCategoryString(sFullCateg, sCateg, categID, sSubCateg, subCategID);

            if (categID == -1 && !sCateg.IsEmpty())
            {
                categID =  core_->categoryList_.AddCategory(sCateg);
                sMsg = wxString::Format(_("Added category: %s"), sCateg);
                logWindow->AppendText(sMsg << "\n");
            }
            if (subCategID == -1 && categID != -1 && !sSubCateg.IsEmpty())
            {
                subCategID = core_->categoryList_.AddSubCategory(categID, sSubCateg);
                sMsg = wxString::Format(_("Added subcategory: %s"), sSubCateg);
                logWindow->AppendText(sMsg << "\n");
            }

            continue;
        }
        else if (lineType(readLine) == AmountSplit) // '$'
        {
            sSplitAmount = getLineData(readLine);

            //get amount
            if (!sSplitAmount.ToDouble(&dSplitAmount) && ! CurrencyFormatter::formatCurrencyToDouble(sSplitAmount, dSplitAmount))
                dSplitAmount = 0; //wrong amount
            //
            if (type == TRANS_TYPE_WITHDRAWAL_STR)
                dSplitAmount = -dSplitAmount;
            //Add split entry
            mmSplitTransactionEntry* pSplitEntry(new mmSplitTransactionEntry);
            pSplitEntry->splitAmount_  = dSplitAmount;
            pSplitEntry->categID_      = categID;
            pSplitEntry->subCategID_   = subCategID;

            mmSplit->addSplit(pSplitEntry);
            continue;
        }
        //MemoSplit
        else if (lineType(readLine) == Address) // 'A'
        {
            notes << getLineData(readLine) << "\n";
            continue;
        }
        else if (lineType(readLine) == EOTLT) // ^
        {
            wxString status = "F";

            if (dt.Trim().IsEmpty())
            {
                sMsg = _("Date is missing");
                logWindow->AppendText(wxString()<< sMsg << "\n");
                bValid = false;
            }
            if (sAmount.Trim().IsEmpty())
            {
                sMsg = _("Amount is missing");
                logWindow->AppendText(sMsg << "\n");
                bValid = false;
            }

            if (sFullCateg.Trim().IsEmpty() && type != TRANS_TYPE_TRANSFER_STR)
            {
                sMsg = _("Category is missing");
                logWindow->AppendText(sMsg << "\n");
                sFullCateg = _("Unknown");

                core_->categoryList_.parseCategoryString(sFullCateg, sCateg, categID, sSubCateg, subCategID);
                if (categID == -1 && !sCateg.IsEmpty())
                {
                    categID =  core_->categoryList_.AddCategory(sCateg);
                    sMsg = wxString::Format(_("Added category: %s"), sCateg);
                    logWindow->AppendText(sMsg << "\n");
                }
            }

            to_account_id = -1;
            if (type == TRANS_TYPE_TRANSFER_STR)
            {
                if (accounts_name.Index(sToAccountName) == wxNOT_FOUND)
                {
                    mmAccount* ptrBase = new mmAccount();
                    mmAccount* pAccount(ptrBase);

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

                    sMsg = wxString::Format(_("Added account '%s'"), sToAccountName);
                    logWindow->AppendText(wxString()<< sMsg << "\n");
                }
                to_account_id = core_->accountList_.GetAccountId(sToAccountName);
                if (val > 0.0)
                {
                    from_account_id = to_account_id;
                    to_account_id = fromAccountID_;
                }
                payeeID = -1;
                if (to_account_id == -1 || from_account_id == -1)
                {
                    sMsg = _("Account missing");
                    logWindow->AppendText(sMsg << "\n");
                    bValid = false;
                }
            }
            else
            {
                if (val > 0.0)
                    type = TRANS_TYPE_DEPOSIT_STR;
                else
                    type = TRANS_TYPE_WITHDRAWAL_STR;

                to_account_id = -1;
                if (sPayee.IsEmpty())
                {
                    sMsg = _("Payee missing");
                    logWindow->AppendText(sMsg << "\n");
                    sPayee = _("Unknown");
                }

                if (!core_->payeeList_.PayeeExists(sPayee))
                {
                    payeeID = core_->payeeList_.AddPayee(sPayee);
                    sMsg = wxString::Format(_("Payee Added: %s"), sPayee);
                    logWindow->AppendText(wxString()<< sMsg << "\n");
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
                sValid = "OK";
            else
                sValid = "NO";

            if ((bValid) && (bFromDate) && ((dtdt < fromDate) || (bToDate && (dtdt > toDate))))
            {
                sValid = "SKIP";
                bValid = false;
            }
            sMsg = wxString::Format(
                "Line:%ld Trx:%ld %s D:%s Acc:'%s' %s P:'%s%s' Amt:%s C:'%s' \n"
                , trxNumLine
                , vQIF_trxs_.size() + 1
                , sValid
                , convDate
                , core_->accountList_.GetAccountName(from_account_id)
                , wxString((type == TRANS_TYPE_TRANSFER_STR ? "<->" : ""))
                , core_->accountList_.GetAccountName(to_account_id)
                , core_->payeeList_.GetPayeeName(payeeID)
                , (wxString()<<val)
                , sFullCateg
                );
            logWindow->AppendText(sMsg);

            for (size_t i = 0; i < mmSplit->entries_.size(); ++i)
            {
                int c = mmSplit->entries_[i]->categID_;
                int s = mmSplit->entries_[i]->subCategID_;

                wxString cn = core_->categoryList_.GetCategoryName(c);
                wxString sn = core_->categoryList_.GetSubCategoryName(c, s);
                double v = mmSplit->entries_[i]->splitAmount_;
                sMsg = (cn << ":" << sn << " " << v << "\n");
                logWindow->AppendText(sMsg);
            }
            bTrxComplited = true;
            if (!bValid) continue;

            std::shared_ptr<mmBankTransaction> pTransaction(new mmBankTransaction(core_));
            pTransaction->date_ = dtdt;
            pTransaction->accountID_ = from_account_id;
            pTransaction->toAccountID_ = to_account_id;
            pTransaction->payeeID_ = payeeID;
            pTransaction->payeeStr_ = core_->payeeList_.GetPayeeName(payeeID);
            pTransaction->transType_ = type;
            pTransaction->amt_ = val;
            pTransaction->status_ = status;
            pTransaction->transNum_ = transNum;
            pTransaction->notes_ = notes;
            pTransaction->toAmt_ = val;
            if (mmSplit->numEntries()) categID = -1;
            pTransaction->categID_ = categID;
            pTransaction->subcategID_ = subCategID;
            pTransaction->fullCatStr_ = core_->categoryList_.GetFullCategoryString(categID, subCategID);
            *pTransaction->splitEntries_ = *mmSplit;

            mmCurrency* pCurrencyPtr = core_->accountList_.getCurrencySharedPtr(from_account_id);
            wxASSERT(pCurrencyPtr);

            //For any transfer transaction always mirrored transaction present
            //Just take alternate amount and skip it
            if (type == TRANS_TYPE_TRANSFER_STR)
            {
                for (const auto& refTrans : vQIF_trxs_)
                {
                    if (refTrans->transType_ != TRANS_TYPE_TRANSFER_STR) continue;
                    if (refTrans->status_ == "D") continue;
                    if (refTrans->date_!= dtdt) continue;
                    if (((refTrans->amt_ < 0) && (val < 0)) || ((refTrans->amt_ > 0) && (val >0))) continue;
                    if (refTrans->accountID_!= from_account_id) continue;
                    if (refTrans->transNum_ != transNum) continue;
                    if (refTrans->notes_ != notes) continue;

                    if (val > 0.0)
                        refTrans->toAmt_ = val;
                    else
                        refTrans->amt_ = val;
                    refTrans->status_ = "D";

                    sMsg = wxString::Format("%f -> %f (%f)\n", refTrans->amt_
                        , refTrans->toAmt_
                        , (fabs(refTrans->amt_)/fabs(refTrans->toAmt_)<1)
                            ? fabs(refTrans->toAmt_)/fabs(refTrans->amt_)
                            : fabs(refTrans->amt_)/fabs(refTrans->toAmt_));
                    logWindow->AppendText(sMsg);

                    bValid = false;
                    break;
                }
            }

            if (bValid)
            {
                vQIF_trxs_.push_back(pTransaction);
            }
        }
    }

    sMsg = wxString::Format(_("Transactions imported from QIF: %ld"), vQIF_trxs_.size());
    logWindow->AppendText(sMsg << "\n");

    int num = 0;
    for (const auto& transaction : vQIF_trxs_)
    {
        wxVector<wxVariant> data;
        data.push_back(wxVariant(transaction.get()->date_.FormatISODate()));
        data.push_back(wxVariant(transaction.get()->transNum_));
        data.push_back(wxVariant(transaction.get()->payeeStr_));
        data.push_back(wxVariant(transaction.get()->status_));
        data.push_back(wxVariant(core_->categoryList_.GetFullCategoryString(transaction.get()->categID_, transaction.get()->subcategID_)));
		data.push_back(wxVariant(wxString::Format("%.2f",transaction.get()->value(-1))));
        data.push_back(wxVariant(transaction.get()->notes_));
        dataListBox_->AppendItem(data, (wxUIntPtr)num++);
    }

    return fromAccountID_;
}

void mmQIFImportDialog::OnFileSearch(wxCommandEvent& /*event*/)
{
    sFileName_ = file_name_ctrl_->GetValue();

    const wxString choose_ext = _("QIF Files");
    sFileName_ = wxFileSelector(_("Choose QIF data file to Import"),
        wxEmptyString, sFileName_, wxEmptyString,
        choose_ext + " (*.qif)|*.qif;*.QIF"
            , wxFD_OPEN|wxFD_CHANGE_DIR|wxFD_FILE_MUST_EXIST);

    if (!sFileName_.IsEmpty())
        correctEmptyFileExt( "qif", sFileName_);
	else
		return; // user pressed cancel
    file_name_ctrl_->SetValue(sFileName_);

    wxTextFile tFile(sFileName_);
    if (!tFile.Open())
    {
        wxMessageBox(_("Unable to open file."), _("QIF Import"), wxOK|wxICON_ERROR);
        return;
    }
    wxString str;
    for (str = tFile.GetFirstLine(); !tFile.Eof(); str = tFile.GetNextLine())
    {
        *log_field_ << wxString::Format(_("Line %i"), tFile.GetCurrentLine()+1)
            << "\t"
            << str << "\n";
    }

    if (checkQIFFile(tFile))
        mmImportQIF(tFile);
}

bool mmQIFImportDialog::checkQIFFile(wxTextFile& tFile)
{
    bbFile_->Enable(false);
    bbFile_->SetBitmapLabel(wxBitmap(empty_xpm));
    bbFormat_->Enable(false);
    bbFormat_->SetBitmapLabel(wxBitmap(empty_xpm));
    newAccounts_->Clear();
    bbAccounts_->SetBitmapLabel(wxBitmap(empty_xpm));
    btnOK_->Enable(false);

    bool dateFormatIsOK = true;
    wxString sAccountName, str;
    for (str = tFile.GetFirstLine(); !tFile.Eof(); str = tFile.GetNextLine())
    {
        if (str.Length() == 0)
            continue;

        if (!isLineOK(str))
        {
            wxString sError = wxString()
                << wxString::Format(_("Line %i"), tFile.GetCurrentLine()+1)
                << "\n"
                << str;
            mmShowErrorMessageInvalid(this, sError);
            return false;
        }

        if ( lineType(str) == AcctType && getLineData(str) == "Account")
        {
            bool reading = true;
            while(!tFile.Eof() && reading )
            {
                str = tFile.GetNextLine();
                if (accountInfoType(str) == Name)
                {
                    sAccountName = getLineData(str);
                    if (accounts_name_.Index(sAccountName) == wxNOT_FOUND)
                    {
                        newAccounts_->Append(sAccountName);
                    }
                }
                reading = (accountInfoType(str) != EOT);
            }
            continue;
        }

        if (lineType(str) == Date)
        {
            wxDateTime dtdt;
            wxString sDate = getLineData(str);

            if (!mmParseDisplayStringToDate(dtdt, sDate, dateFormat_))
                dateFormatIsOK = false;
            continue;
        }
    }

    bbFile_->Enable(true);
    bbFile_->SetBitmapLabel(wxBitmap(flag_xpm));
    bbFormat_->Enable(dateFormatIsOK);
    if (dateFormatIsOK)
        bbFormat_->SetBitmapLabel(wxBitmap(flag_xpm));

    if (sAccountName.IsEmpty() && last_imported_acc_id_<0)
    {
        wxArrayString data = core_->accountList_.getAccountsName();
        sAccountName = wxGetSingleChoice(_("Choose Account to Import to")
            , _("Account"), data);
        last_imported_acc_id_ = core_->accountList_.GetAccountId(sAccountName);
        if (last_imported_acc_id_ < 0) return false;
    }

    if (newAccounts_->GetCount() > 0)
    {
        int iBaseCurrencyID = core_->currencyList_.GetBaseCurrencySettings();
        if (iBaseCurrencyID < 0)
        {
            mmShowErrorMessageInvalid(this, _("Base Currency Not Set"));
            return false;
        }
        newAccounts_->Enable(true);
        newAccounts_->SetSelection(0);
        bbAccounts_->SetBitmapLabel(wxBitmap(flag_xpm));
    }

    if (dateFormatIsOK)
    {
        bbFormat_->SetBitmapLabel(wxBitmap(flag_xpm));
        bbFormat_->Enable(dateFormatIsOK);
        btnOK_->Enable(true);
    }
    return true;
}

void mmQIFImportDialog::OnDateMaskChange(wxCommandEvent& /*event*/)
{
    wxStringClientData* data = (wxStringClientData*)(choiceDateFormat_->GetClientObject(choiceDateFormat_->GetSelection()));
    if (data) dateFormat_ = data->GetData();
    wxTextFile tFile(sFileName_);
    if (!tFile.IsOpened())
        if (tFile.Open())
            checkQIFFile(tFile);
}

void mmQIFImportDialog::OnCheckboxClick( wxCommandEvent& /*event*/ )
{
    fromDateCtrl_->Enable(dateFromCheckBox_->GetValue());
    toDateCtrl_->Enable(dateToCheckBox_->GetValue());
}

void mmQIFImportDialog::OnOk(wxCommandEvent& /*event*/)
{
    wxString sMsg;
    wxMessageDialog msgDlg(this, _("Do you want to import all transaction ?"),
                                        _("Confirm Import"),
                                        wxYES_NO | wxNO_DEFAULT | wxICON_QUESTION);
    if (msgDlg.ShowModal() == wxID_YES)
    {
        core_->db_.get()->Begin();

        //TODO: Update transfer transactions toAmount

        for (const auto& refTrans : vQIF_trxs_)
        {
            //fromAccountID_ = refTrans->accountID_;
            mmCurrency* pCurrencyPtr = core_->accountList_.getCurrencySharedPtr(fromAccountID_);
            wxASSERT(pCurrencyPtr);
            refTrans->amt_ = fabs(refTrans->amt_);
            refTrans->toAmt_ = fabs(refTrans->toAmt_);
            //refTrans->updateAllData(core_, fromAccountID_, pCurrencyPtr);

            core_->bTransactionList_.addTransaction(refTrans.get());
            last_imported_acc_id_ = refTrans->accountID_;
        }

        core_->db_.get()->Commit();
        //FIXME: Some bug here and all transactions after import reloaded
        core_->bTransactionList_.LoadTransactions();
        sMsg = _("Import finished successfully");
        btnOK_->Enable(false);
    }
    else
    {
        sMsg = _("Imported transactions discarded by user!");
    }
    wxMessageDialog(parent_, sMsg, _("QIF Import"), wxOK|wxICON_WARNING).ShowModal();
    //clear the vector to avoid memory leak - done at same level created.

}

void mmQIFImportDialog::OnCancel(wxCommandEvent& /*event*/)
{
    EndModal(wxID_CANCEL);
}

void mmQIFImportDialog::OnQuit(wxCloseEvent& /*event*/)
{
    EndModal(wxID_CANCEL);
}

void mmQIFImportDialog::OnButtonClear(wxCommandEvent& /*event*/)
{
    log_field_->Clear();
}
