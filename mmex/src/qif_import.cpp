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

wxString getFinancistoProject(wxString& sSubCateg)
{
    //Additional parsint for Financisto
    wxString sProject = wxT("");
    wxStringTokenizer cattkz(sSubCateg, wxT("/"));

    sSubCateg = cattkz.GetNextToken();
    if (cattkz.HasMoreTokens())
        sProject = cattkz.GetNextToken();
    return sProject;
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

int mmImportQIF(wxWindow *parent_, mmCoreDB* core )
{
    if (!warning_message()) return -1;

    wxString acctName, sMsg;
    wxArrayString accounts_name = core->accountList_.getAccountsName();

    int fromAccountID = core->accountList_.GetAccountId(acctName);

    wxString sDefCurrencyName = core->currencyList_.getCurrencyName(core->currencyList_.GetBaseCurrencySettings());

    wxString chooseExt;
    chooseExt << _("QIF Files ") << wxT("(*.qif)|*.qif;*.QIF|")
              << _("All Files ") << wxT("(*.*)|*.*");
    wxString fileName = wxFileSelector(_("Choose QIF data file to import"),
        wxEmptyString, wxEmptyString, wxEmptyString, chooseExt, wxFD_OPEN|wxFD_CHANGE_DIR|wxFD_FILE_MUST_EXIST);
    wxFileName logFile = mmex::GetLogDir(true);
    logFile.SetFullName(fileName);
    logFile.SetExt(wxT("log"));

    fileviewer file_dlg(wxT(""), parent_);
    file_dlg.Show();
    wxTextCtrl*& logWindow = file_dlg.textCtrl_;
    bool canceledbyuser = false;

    if (fileName.IsEmpty()) return -1;

    wxFileInputStream input(fileName);
    wxTextInputStream text(input);

    wxFileOutputStream outputLog(logFile.GetFullPath());
    wxTextOutputStream log(outputLog);

    wxString readLine;
    int numLines = 0;
    int trxNumLine = 1;
    int numImported = 0;

    wxString dt = wxDateTime::Now().FormatISODate();
    wxString date_format = mmOptions::instance().dateFormat_;
    wxString sPayee, type, sAmount, transNum, notes, convDate, sToAccountName;
    wxString sFullCateg, sCateg, sSubCateg, sSplitCategs, sSplitAmount, sValid;

    wxDateTime dtdt = wxDateTime::Now().GetDateOnly();
    int payeeID = -1, categID = -1, subCategID = -1, to_account_id = -1, from_account_id = -1;
    double val = 0.0, dSplitAmount = 0.0;
    bool bTrxComplited = true;

    std::vector< boost::shared_ptr<mmBankTransaction> > vQIF_trxs;
    boost::shared_ptr<mmSplitTransactionEntries> mmSplit(new mmSplitTransactionEntries());

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
        }
        readLine = getFileLine(text, numLines);

        if (readLine.Length() == 0)
            continue;

        bool isOK = isLineOK(readLine);
        if (!isOK)
        {
            sMsg = wxString()<< _("Line: ") << numLines << wxT("  ") << _(" Unknown QIF line: ") << readLine;
            log  << sMsg << endl;
            logWindow->AppendText(wxString() << sMsg << wxT("\n"));
            continue;
        }

        if (lineType(readLine) == AcctType)
        {
            wxString accountType = getLineData(readLine);
            if ((!accountType.CmpNoCase(wxT("Type:Bank"))) ||
                (!accountType.CmpNoCase(wxT("Type:Cash"))) ||
                (!accountType.CmpNoCase(wxT("Type:CCard"))))
            {
                sMsg = wxString::Format(_("Importing account type: %s"), accountType.c_str());
                log << sMsg << endl;
                logWindow->AppendText(sMsg << wxT("\n"));
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
                // account information
                // Need to read till we get to end of account information
                while( (readLine = getFileLine(text, numLines) ) != wxT("^"))
                {
                    numLines++;
                    bTrxComplited = true;
                    int i = accountInfoType(readLine);
                    if (i == Name)
                    {
                        if (core->accountList_.GetAccountId(getLineData(readLine)) > -1)
                            acctName = getLineData(readLine);
                        else
                        {
                            //TODO: Repeated code
                            mmAccount* ptrBase = new mmAccount();
                            boost::shared_ptr<mmAccount> pAccount(ptrBase);

                            pAccount->favoriteAcct_ = true;
                            pAccount->status_ = mmAccount::MMEX_Open;
                            pAccount->acctType_ = ACCOUNT_TYPE_BANK;
                            pAccount->name_ = getLineData(readLine);
                            pAccount->initialBalance_ = 0;
                            pAccount->currency_ = core->currencyList_.getCurrencySharedPtr(sDefCurrencyName);
                            // prevent same account being added multiple times in case of using 'Back' and 'Next' in wizard.
                            if ( ! core->accountList_.AccountExists(pAccount->name_))
                                from_account_id = core->accountList_.AddAccount(pAccount);
                            accounts_name.Add(pAccount->name_);
                            acctName = pAccount->name_;
                            sMsg = wxString::Format(_("Added account '%s'"), acctName.c_str());
                            log << sMsg << endl;
                            logWindow->AppendText(wxString()<< sMsg << wxT("\n"));

                        }

                        fromAccountID = core->accountList_.GetAccountId(acctName);

                        sMsg = wxString::Format(_("Line: %ld"), numLines) << wxT(" : ")
                            << wxString::Format(_("Account name: %s"), acctName.c_str());
                        log << sMsg << endl;
                        logWindow->AppendText(wxString()<< sMsg << wxT("\n"));
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
            // we do not know how to process this type yet
            wxString errMsgStr = _("Cannot process these QIF Account Types yet.");
            wxString errLineMsgStr = wxString::Format(_("Line: %ld"), numLines)
                << wxT("\n") << readLine;

            log << errLineMsgStr << endl;
            log << errMsgStr << endl;
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

            dtdt = mmParseDisplayStringToDate(dt, date_format).GetDateOnly();
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
                log << sMsg << endl;
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

            /* //Trick  for cut non standart qif category usage in Financisto application
            //Category field may contains additional information like Project
            //Format Category[:Subcategory][/Project] //*/
            if (sFullCateg.Contains(wxT("/")))
                transNum.Prepend(wxString::Format(wxT("[%s] "), getFinancistoProject(sFullCateg).c_str()));
            core->categoryList_.parseCategoryString(sFullCateg, sCateg, categID, sSubCateg, subCategID);

            if (categID == -1 && !sCateg.IsEmpty())
            {
                categID =  core->categoryList_.AddCategory(sCateg);
                sMsg = wxString::Format(_("Added category %s"), sCateg);
                log << sMsg << endl;
                logWindow->AppendText(sMsg << wxT("\n"));
            }
            if (subCategID == -1 && categID != -1 && !sSubCateg.IsEmpty())
            {
                subCategID = core->categoryList_.AddSubCategory(categID, sSubCateg);
                sMsg = wxString::Format(_("Added subcategory %s"), sSubCateg);
                log << sMsg << endl;
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
            pSplitEntry->category_     = core->categoryList_.GetCategorySharedPtr(categID, subCategID);
            wxASSERT(pSplitEntry->category_.lock());
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
                log << sMsg << endl;
                logWindow->AppendText(wxString()<< sMsg << wxT("\n"));
                bValid = false;
            }
            else if (type.Trim().IsEmpty())
            {
                sMsg = _("Transaction Type is missing");
                log << sMsg << endl;
                logWindow->AppendText(sMsg << wxT("\n"));
                bValid = false;
            }
            else if (sAmount.Trim().IsEmpty())
            {
                sMsg = _("Amount is missing");
                log << sMsg << endl;
                logWindow->AppendText(sMsg << wxT("\n"));
                bValid = false;
            }

            to_account_id = -1;

            if (sFullCateg.Left(1).Contains(wxT("[")) && sFullCateg.Right(1).Contains(wxT("]")))
            {
                sToAccountName = sFullCateg.substr(1, sFullCateg.Length()-2);

                sFullCateg = _("Transfer");

                if (accounts_name.Index(sToAccountName) == wxNOT_FOUND)
                {
                    mmAccount* ptrBase = new mmAccount();
                    boost::shared_ptr<mmAccount> pAccount(ptrBase);

                    pAccount->favoriteAcct_ = true;
                    pAccount->status_ = mmAccount::MMEX_Open;
                    pAccount->acctType_ = ACCOUNT_TYPE_BANK;
                    pAccount->name_ = sToAccountName;
                    pAccount->initialBalance_ = 0;
                    pAccount->currency_ = core->currencyList_.getCurrencySharedPtr(sDefCurrencyName);
                    // prevent same account being added multiple times in case of using 'Back' and 'Next' in wizard.
                    if ( ! core->accountList_.AccountExists(pAccount->name_))
                        to_account_id = core->accountList_.AddAccount(pAccount);
                    accounts_name.Add(sToAccountName);

                    sMsg = wxString::Format(_("Added account '%s'"), sToAccountName.c_str());
                    log << sMsg << endl;
                    logWindow->AppendText(wxString()<< sMsg << wxT("\n"));
                }
                to_account_id = core->accountList_.GetAccountId(sToAccountName);
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
                    log << sMsg << endl;
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
                    log << sMsg << endl;
                    logWindow->AppendText(sMsg << wxT("\n"));
                    sPayee = _("Unknown");
                }
                if (!core->payeeList_.PayeeExists(sPayee))
                {
                    payeeID = core->payeeList_.AddPayee(sPayee);
                    sMsg = wxString::Format(_("Payee Added: %s"), sPayee.c_str());
                    log << sMsg << endl;
                    logWindow->AppendText(wxString()<< sMsg << wxT("\n"));
                }
                else
                    payeeID = core->payeeList_.GetPayeeId(sPayee);
            }
            if (mmSplit->entries_.size() > 0)
            {
                categID = -1;
                sFullCateg = _("Split Category");
            }
            else
            {
                sFullCateg = core->categoryList_.GetFullCategoryString(categID, subCategID);
            }

            if (!bValid) sValid = wxT("NO"); else sValid = wxT("OK");
            sMsg = wxString::Format(
                wxT("Line:%ld Trx:%ld %s D:%s Acc:'%s' %s P:'%s%s' Amt:%s C:'%s' \n")
                , trxNumLine
                , numImported + 1
                , sValid.c_str()
                , convDate.c_str()
                , core->accountList_.GetAccountName(from_account_id).c_str()
                , wxString((type == TRANS_TYPE_TRANSFER_STR ? wxT("<->") : wxT(""))).c_str()
                , core->accountList_.GetAccountName(to_account_id).c_str()
                , core->payeeList_.GetPayeeName(payeeID).c_str()
                , (wxString()<<val).c_str()
                , sFullCateg.c_str()
                );
            logWindow->AppendText(sMsg);
            log << sMsg << endl;

            for (size_t i = 0; i < mmSplit->entries_.size(); ++i)
            {
                int c = mmSplit->entries_[i]->categID_;
                int s = mmSplit->entries_[i]->subCategID_;
                wxString cn = core->categoryList_.GetCategoryName(c);
                wxString sn = core->categoryList_.GetSubCategoryName(c, s);
                double v = mmSplit->entries_[i]->splitAmount_;
                sMsg = (cn << wxT(":") << sn << wxT(" ") << v << wxT("\n"));
                logWindow->AppendText(sMsg);
                log << sMsg << endl;
            }
            bTrxComplited = true;
            if (!bValid) continue;

            boost::shared_ptr<mmBankTransaction> pTransaction(new mmBankTransaction(core->db_));
            pTransaction->date_ = dtdt;
            pTransaction->accountID_ = from_account_id;
            pTransaction->toAccountID_ = to_account_id;
            pTransaction->payee_ = core->payeeList_.GetPayeeSharedPtr(payeeID);
            pTransaction->payeeStr_ = sPayee;
            pTransaction->transType_ = type;
            pTransaction->amt_ = val;
            pTransaction->status_ = status;
            pTransaction->transNum_ = transNum;
            pTransaction->notes_ = notes;
            pTransaction->toAmt_ = val;
            if (mmSplit->numEntries()) categID = -1;
            pTransaction->category_ = core->categoryList_.GetCategorySharedPtr(categID, subCategID);
            *pTransaction->splitEntries_ = *mmSplit;

            //For any transfer transaction always mirrored transaction present
            //Just take alternate amount and skip it
            if (type == TRANS_TYPE_TRANSFER_STR)
            {
                std::vector<boost::shared_ptr<mmBankTransaction> >& refTrans = vQIF_trxs;
                for (unsigned int index = 0; index < vQIF_trxs.size(); index++)
                {
                    if (refTrans[index]->transType_ != TRANS_TYPE_TRANSFER_STR) continue;
                    if (refTrans[index]->date_!= dtdt) continue;
                    if (refTrans[index]->amt_ < 0 && val < 0 || refTrans[index]->amt_ > 0 && val >0 ) continue;
                    if (refTrans[index]->accountID_!= from_account_id) continue;
                    if (refTrans[index]->transNum_ != transNum) continue;
                    if (refTrans[index]->notes_ != notes) continue;

                    sMsg = wxString::Format(wxT("%f -> %f \n"),refTrans[index]->toAmt_ ,val);
                    if (val > 0.0)
                        refTrans[index]->toAmt_ = val;
                    else
                        refTrans[index]->amt_ = val;

                    bValid = false;
                    logWindow->AppendText(sMsg);
                    log << sMsg << endl;
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
    log << sMsg << endl;
    logWindow->AppendText(sMsg << wxT("\n"));

    sMsg  = wxString::Format(_("Log file written to : %s \n"), logFile.GetFullPath().c_str())
          << _("Please confirm saving...");
    logWindow->AppendText(sMsg << wxT("\n"));

    canceledbyuser = file_dlg.ShowModal() == wxID_CANCEL;
    // Since all database transactions are only in memory,
    if (!canceledbyuser)
    {
        core->db_.get()->Begin();

        std::vector<boost::shared_ptr<mmBankTransaction> >& refTrans = vQIF_trxs;

        //TODO: Update transfer transactions toAmount

        for (unsigned int index = 0; index < vQIF_trxs.size(); index++)
        {
            //fromAccountID = refTrans[index]->accountID_;
            boost::shared_ptr<mmCurrency> pCurrencyPtr = core->accountList_.getCurrencyWeakPtr(fromAccountID).lock();
            wxASSERT(pCurrencyPtr);
            refTrans[index]->amt_ = fabs(refTrans[index]->amt_);
            refTrans[index]->toAmt_ = fabs(refTrans[index]->toAmt_);
            refTrans[index]->updateAllData(core, fromAccountID, pCurrencyPtr);
            core->bTransactionList_.addTransaction(core, refTrans[index]);
        }

        core->db_.get()->Commit();
        sMsg = _("Import finished successfully");
        log << endl << sMsg << endl;
    }
    else
    {
        sMsg = _("Imported transactions discarded by user!");
        log << endl << sMsg << endl;
    }

    wxMessageDialog(parent_, sMsg, _("QIF Import"), wxOK|wxICON_WARNING).ShowModal();
    outputLog.Close();
    //clear the vector to avoid memory leak - done at same level created.
    vQIF_trxs.clear();

    return fromAccountID;
}

