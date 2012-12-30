#include "constants.h"
#include "util.h"
#include "dbwrapper.h"
#include "guiid.h"
#include "fileviewerdialog.h"
#include "mmex.h"
#include "mmcoredb.h"
#include "platfdep.h"
#include "qif_export.h"

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

bool isLineOK(const wxString& line)
{
    return wxString(wxT("!DNPAT^MLSE$C/U")).Contains(line.Left(1));
}

wxString getLineData(const wxString& line)
{
    wxString dataString = line.substr(1, line.Length()-1);
    return dataString;
}

wxString getFileLine(wxTextInputStream& textFile, int& lineNumber)
{
    wxString textLine = textFile.ReadLine();
    lineNumber ++;
    textLine.Trim(); // remove any trailing spaces

    return textLine;
}

bool warning_message()
{
    wxString msgStr;
    msgStr << _("To import QIF files correctly, the date format in the QIF file must match the date option set in MMEX.") << wxT("\n\n")
           << _("Are you are sure you want to proceed with the import?");
    wxMessageDialog msgDlg(NULL, msgStr, _("QIF Import"), wxYES_NO|wxICON_QUESTION);
    if (msgDlg.ShowModal() != wxID_YES)
        return false;
    return true;
}

int mmImportQIF(wxWindow *parent_, mmCoreDB* core, wxString destinationAccountName )
{
    wxSQLite3Database* db_ = core->db_.get();

    if ( core->accountList_.getNumAccounts() == 0 ) {
        wxMessageBox(_( "No account available for import"), _("QIF Import"), wxOK|wxICON_WARNING );
        return -1;
    }

    if (!warning_message()) return -1;

    wxString acctName;
    wxArrayString accounts_name = core->accountList_.getAccountsName();

    if (destinationAccountName == wxEmptyString)
    {
        wxSingleChoiceDialog scd(parent_, _("Choose Account to import to:"), _("QIF Import"), accounts_name);
        if (scd.ShowModal() != wxID_OK)
            return -1;

        acctName = scd.GetStringSelection();
    }
    else
        acctName = destinationAccountName;

    int fromAccountID = core->accountList_.GetAccountId(acctName);

    boost::shared_ptr<mmCurrency> pCurrencyPtr = core->accountList_.getCurrencyWeakPtr(fromAccountID).lock();
    wxASSERT(pCurrencyPtr);

    wxString chooseExt;
    chooseExt << _("QIF Files ") << wxT("(*.qif)|*.qif;*.QIF|")
              << _("All Files ") << wxT("(*.*)|*.*");
    wxString fileName = wxFileSelector(_("Choose QIF data file to import"),
        wxEmptyString, wxEmptyString, wxEmptyString, chooseExt, wxFD_OPEN|wxFD_CHANGE_DIR|wxFD_FILE_MUST_EXIST);
    wxFileName logFile = mmex::GetLogDir(true);
    logFile.SetFullName(fileName);
    logFile.SetExt(wxT("log"));
    bool canceledbyuser = false;

    if ( !fileName.IsEmpty() )
    {
        wxFileInputStream input(fileName);
        wxTextInputStream text(input);

        wxFileOutputStream outputLog(logFile.GetFullPath());
        wxTextOutputStream log(outputLog);

        wxString readLine;
        int numLines = 0;
        int numImported = 0;

        wxString dt = wxDateTime::Now().FormatISODate();
        //wxString date_format = mmDBWrapper::getInfoSettingValue(db, wxT("DATEFORMAT"), mmex::DEFDATEFORMAT);
        wxString date_format = mmOptions::instance().dateFormat_;
        wxString payee, type, amount, categ, subcateg, transNum, notes, convDate, from_account_name;
        wxDateTime dtdt = wxDateTime::Now();
        int payeeID = -1, categID = -1, subCategID = -1, to_account_id = -1;
        double val = 0.0;

        std::vector<int> QIF_transID;
        core->db_.get()->Begin();

        wxProgressDialog* dlg = new wxProgressDialog(_("QIF Import"), _("Transactions imported from QIF: "), 100,
            NULL, wxPD_AUTO_HIDE |  wxPD_CAN_ABORT);
        while(!input.Eof())
        {
            wxString progressMsg;
            progressMsg << _("Transactions imported from QIF") << wxT("\n")
                        << _("to account ") << acctName << wxT(": ") << numImported;
            if (!dlg->Update(static_cast<int>((static_cast<double>(numImported)/100.0 - numImported/100) *99), progressMsg))
            {
                canceledbyuser = true;
                break; // abort processing
            }

            readLine = getFileLine(text, numLines);
            if (readLine.Length() == 0)
                continue;

            bool isOK = isLineOK(readLine);
            if (!isOK)
            {
                log << _("Line: ") << numLines << wxT("  ") << _(" Unknown QIF line: ") << readLine << endl;
                continue;
            }

            if (lineType(readLine) == AcctType)
            {
                wxString accountType = getLineData(readLine);
                if ((!accountType.CmpNoCase(wxT("Type:Bank"))) ||
                    (!accountType.CmpNoCase(wxT("Type:Cash"))) ||
                    (!accountType.CmpNoCase(wxT("Type:CCard"))))
                {
                    log << _("Importing account type: ") << accountType << endl;
                    continue;
                }

                if (accountType == wxT("Account"))
                {
                    // account information
                    // Need to read till we get to end of account information
                    while( (readLine = getFileLine(text, numLines) ) != wxT("^"))
                    {
                        numLines++;
                        int i = accountInfoType(readLine);
                        if (i == Name)
                        {
                            log << _("Line: " ) << numLines << _(" : ")
                                << getLineData(readLine) << _(" account name ") << endl;
                            continue;
                        }
                        else if (i == AccountType || i == Description || i == CreditLimit || i  == BalanceDate || i == Balance)
                        {
                            continue;
                        }
                    }
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
                else if ( accountType == wxT("Type:Cat") )
                {
                    bool reading = true;
                    while( reading )
                    {
                        readLine = getFileLine(text, numLines);
                        if (/*readLine.Contains(wxT("!Account")) ||*/ readLine.Contains(wxT("!Type:")) || input.Eof())
                            reading = false;
                    }
                    continue;
                }

                // we do not know how to process this type yet
                wxString errMsgStr = _("Cannot process these QIF Account Types yet.");
                wxString errLineMsgStr = wxString() << _("Line: ") << numLines << wxT("  ") << readLine;

                log << errLineMsgStr << endl;
                log << errMsgStr << endl;
                wxMessageBox( errLineMsgStr + wxT("\n\n") + errMsgStr, _("QIF Import"), wxICON_ERROR);

                // exit: while(!input.Eof()) loop and allow to exit routine and allow user to save or abort
                break;
                // return -1;
            }

            wxString cat, subcat;

            if (lineType(readLine) == Payee)
            {
                payee = getLineData(readLine);
                if (payee.Trim().IsEmpty())
                {
                    payee = wxT("Unknown");
                }

                if (!core->payeeList_.PayeeExists(payee))
                {
                    log << _("Payee Added: ") << payee << endl;
                    payeeID = core->payeeList_.AddPayee(payee);
                }
                else
                    payeeID = core->payeeList_.GetPayeeId(payee);

                continue;
            }
            else if (lineType(readLine) == Amount)
            {
                amount = getLineData(readLine);

                if (!mmex::formatCurrencyToDouble(amount, val))
                {
                    log << _("Line: " ) << numLines << _(" invalid amount, skipping.") << endl;
                    continue;
                }

                if (val <= 0.0)
                    type = TRANS_TYPE_WITHDRAWAL_STR;
                else
                    type = TRANS_TYPE_DEPOSIT_STR;
                val = fabs(val);
                continue;
            }
            else if (lineType(readLine) == Address)
            {
                continue;
            }
            else if (lineType(readLine) == Date)
            {
                dt = getLineData(readLine);

                dtdt = mmParseDisplayStringToDate(db_, dt, date_format);
                convDate = dtdt.FormatISODate();
                continue;
            }
            else if (lineType(readLine) == TransNumber)
            {
                transNum = getLineData(readLine);
                continue;
            }
            else if (lineType(readLine) == Memo || lineType(readLine) == MemoSplit )
            {
                notes << getLineData(readLine) << wxT("\n");
                continue;
            }
            else if (lineType(readLine) == Category)
            {
                categ = getLineData(readLine);

                if (categ.Left(1).Contains(wxT("[")) && categ.Right(1).Contains(wxT("]")))
                {
                    from_account_name = categ.substr(1, categ.Length()-2);
                    to_account_id = -1;
                    
                    categ = wxT("Transfer");
                    type = wxT("Transfer");
                    if (accounts_name.Index(from_account_name) != wxNOT_FOUND)
                        to_account_id = core->accountList_.GetAccountId(from_account_name);
                }

                {
                    wxStringTokenizer cattkz(categ, wxT(":"));
    
                    if (cattkz.HasMoreTokens())
                        cat = cattkz.GetNextToken();
                    if (cattkz.HasMoreTokens())
                        subcat = cattkz.GetNextToken();
    
                    categID = core->categoryList_.GetCategoryId(cat);
                    if (categID == -1)
                    {
                        categID =  core->categoryList_.AddCategory(cat);
                    }
    
                    if (!subcat.IsEmpty())
                    {
                        subCategID = core->categoryList_.GetSubCategoryID(categID, subcat);
                        if (subCategID == -1)
                        {
                            subCategID = core->categoryList_.AddSubCategory(categID, subcat);
                        }
                    }
                    else
                        subCategID = -1;
    
                    continue;
                }
            }
            else if (lineType(readLine) == EOTLT)
            {
                wxString status = wxT("F");

                if (dt.Trim().IsEmpty())
                {
                    log << _("Date is empty, ") << endl;
                }
                else if (type.Trim().IsEmpty())
                {
                    log << _("Transaction Type is empty") << endl;
                }
                else if (amount.Trim().IsEmpty())
                {
                    log << _("Amount is empty") << endl;
                }

                if (to_account_id == -1)
                {
                    log << _("Account: ") << from_account_name << wxT(" ") << _("Unknown") << endl;
                    //FIXME: Transfer transaction with sane to and from account id will be created 
                    to_account_id = fromAccountID;
                }

                if (payee.Trim().IsEmpty())
                {
                    if (type != wxT("Transfer"))
                    {
                        payee = wxT("Unknown");

                        if (!core->payeeList_.PayeeExists(payee))
                        {
                            payeeID = core->payeeList_.AddPayee(payee);
                            log << _("Payee Added: ") << payee << endl;
                        }
                        else
                        {
                            payeeID = core->payeeList_.GetPayeeId(payee);
                        }
                    }
                    else
                        payeeID = -1;
                }

                if (categ.Trim().IsEmpty())
                {
                    // check if category exists for this payee.
                    boost::shared_ptr<mmPayee> pPayee = core->payeeList_.GetPayeeSharedPtr(payeeID);
                    if (pPayee->categoryId_ == -1)
                    {
                        log << _("Category is empty, marking transaction as Unknown category") << endl;

                        categID = core->categoryList_.GetCategoryId(wxT("Unknown"));
                        if (categID == -1)
                        {
                            categID =  core->categoryList_.AddCategory(wxT("Unknown"));
                        }
                    }
                }

                if(dt.Trim().IsEmpty()  || type.Trim().IsEmpty() || amount.Trim().IsEmpty())
                {
                    log << _("Skipping QIF transaction because date, type, amount is empty/invalid, transaction skipped had ");
                    log << _(" payee ") << payee << _(" type ") << type
                        << _(" amount ") << amount << _(" date ") << convDate
                        << endl;

                    continue;
                }

                boost::shared_ptr<mmBankTransaction> pTransaction(new mmBankTransaction(core->db_));
                pTransaction->accountID_ = fromAccountID;
                pTransaction->toAccountID_ = to_account_id;
                pTransaction->payee_ = core->payeeList_.GetPayeeSharedPtr(payeeID);
                pTransaction->transType_ = type;
                pTransaction->amt_ = val;
                pTransaction->status_ = status;
                pTransaction->transNum_ = transNum;
                pTransaction->notes_ = (wxString(notes.Last()) == wxT("\n") ? notes.RemoveLast() : notes);
                pTransaction->category_ = core->categoryList_.GetCategorySharedPtr(categID, subCategID);
                pTransaction->date_ = dtdt;
                pTransaction->toAmt_ = val;
                pTransaction->updateAllData(core, fromAccountID, pCurrencyPtr);

                int transID = core->bTransactionList_.addTransaction(core, pTransaction);
                QIF_transID.push_back(transID);

                numImported++;
                payee.clear();
                type.clear();
                amount.clear();
                categ.clear();
                notes.clear();
                subCategID = -1;
                transNum.clear();
                categID = -1;
                val = 0.0;
                convDate = wxDateTime::Now().FormatISODate();
                continue;
            }
        }

        dlg->Destroy();

        log << _("Transactions imported from QIF: ") << numImported << endl;
        wxString confirmMsg;
        confirmMsg  << _("Total Imported : ") << numImported << wxT ("\n\n")
                    << _("Log file written to : ") << logFile.GetFullPath() << wxT ("\n\n")
                    << _("Please confirm saving...");
        if (!canceledbyuser && wxMessageBox(confirmMsg, _("QIF Import"),wxOK|wxCANCEL|wxICON_INFORMATION) == wxCANCEL)
            canceledbyuser = true;

        // Since all database transactions are only in memory,
        if (!canceledbyuser)
        {
            // we need to save them to the database.
            core->db_.get()->Commit();
            log << endl << _("Transactions saved to database in account: ") << acctName << endl;
        }
        else
        {
            // we need to remove the transactions from the transaction list
            while (numImported > 0)
            {
                numImported --;
                int transID = QIF_transID[numImported];
                core->bTransactionList_.removeTransaction(fromAccountID,transID);
            }
            // and discard the database changes.
            core->db_.get()->Rollback();
            log  << endl << _("Imported transactions discarded by user!") << endl;
        }

        outputLog.Close();
        //clear the vector to avoid memory leak - done at same level created.
        QIF_transID.clear();
    }

    if ( !fileName.IsEmpty() )
    fileviewer(logFile.GetFullPath(), parent_).ShowModal();

    return fromAccountID;
}
