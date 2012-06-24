#include "dbwrapper.h"
#include "fileviewerdialog.h"
#include "guiid.h"
#include "mmcoredb.h"
#include "mmex.h"
#include "platfdep.h"
#include "util.h"

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
    AcctType      = 1, // !
    Date          = 2, // D
    Amount        = 3, // T
    Address       = 4, // A
    Payee         = 5, // P
    EOTLT         = 6, // ^
    TransNumber   = 7, // N
    Status        = 8, // C
    UnknownType   = 9,
    Memo          = 10, // M
    Category      = 11, // L
    CategorySplit = 12, // S
    MemoSplit     = 13, // E
    AmountSplit   = 14  // '$'
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
    if (line.IsEmpty())
        return false;

    wxChar fChar = line.GetChar(0);
    if ((fChar == '!') ||
        (fChar == 'D') ||
        (fChar == 'N') ||
        (fChar == 'P') ||
        (fChar == 'M') ||
        (fChar == 'C') ||
        (fChar == '^') ||
        (fChar == 'A') ||
        (fChar == 'L') ||
        (fChar == 'T') ||
        (fChar == 'S') ||
        (fChar == 'E') ||
        (fChar == '/') ||
        (fChar == '$') ||
        (fChar == 'U'))
        return true;

    return false;
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
    msgStr << _("To import QIF files correctly, the date format in the QIF file must match the date option set in MMEX.") << ("\n\n")
           << _("Are you are sure you want to proceed with the import?");
    wxMessageDialog msgDlg(NULL, msgStr, _("QIF Import"), wxYES_NO|wxICON_QUESTION);
    if (msgDlg.ShowModal() != wxID_YES)
        return false;
    return true;
}

int mmImportQIF(mmCoreDB* core, wxString destinationAccountName )
{
    wxSQLite3Database* db_ = core->db_.get();

    if ( core->getNumAccounts() == 0 ) {
        wxMessageBox(_( "No account available for import"), _("QIF Import"), wxOK|wxICON_WARNING );
        return -1;
    }

    if (!warning_message()) return -1;

    wxString acctName;
    if (destinationAccountName == wxEmptyString)
    {
        wxArrayString as = core->getAccountsName();

        wxSingleChoiceDialog scd(0, _("Choose Account to import to:"), _("QIF Import"), as);
        if (scd.ShowModal() != wxID_OK)
            return -1;

        acctName = scd.GetStringSelection();
    }
    else
        acctName = destinationAccountName;

    int fromAccountID = core->getAccountID(acctName);

    boost::shared_ptr<mmCurrency> pCurrencyPtr = core->getCurrencyWeakPtr(fromAccountID).lock();
    wxASSERT(pCurrencyPtr);

    wxString chooseExt;
    chooseExt << _("QIF Files ") << ("(*.qif)|*.qif;*.QIF|")
              << _("All Files ") << ("(*.*)|*.*");
    wxString fileName = wxFileSelector(_("Choose QIF data file to import"),
        wxEmptyString, wxEmptyString, wxEmptyString, chooseExt, wxFD_OPEN|wxFD_CHANGE_DIR|wxFD_FILE_MUST_EXIST);
    wxFileName logFile = mmex::GetLogDir(true);
    logFile.SetFullName(fileName);
    logFile.SetExt("log");
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
        //wxString date_format = mmDBWrapper::getInfoSettingValue(db, ("DATEFORMAT"), mmex::DEFDATEFORMAT);
        wxString date_format = mmOptions::instance().dateFormat;
        wxString payee, type, amount, categ, subcateg, transNum, notes, convDate;
        wxDateTime dtdt = wxDateTime::Now();
        int payeeID = -1, categID = -1, subCategID = -1;
        subCategID = -1;
        double val = 0.0;

        std::vector<int> QIF_transID;
        core->db_.get()->Begin();

        wxProgressDialog dlg(_("QIF Import"), _("Transactions imported from QIF: "), 100,
            NULL, wxPD_AUTO_HIDE |  wxPD_CAN_ABORT);
        while(!input.Eof())
        {
            wxString progressMsg;
            progressMsg << _("Transactions imported from QIF") << ("\n")
                        << _("to account ") << acctName << (": ") << numImported;
            dlg.Update(static_cast<int>((static_cast<double>(numImported)/100.0 - numImported/100) *99), progressMsg);

            if (!dlg.Update(-1)) // if cancel clicked
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
                log << _("Line: ") << numLines << ("  ") << _(" Unknown QIF line: ") << readLine << endl;
                continue;
            }

            if (lineType(readLine) == AcctType)
            {
                wxString accountType = getLineData(readLine);
                if ((!accountType.CmpNoCase("Type:Bank")) ||
                    (!accountType.CmpNoCase("Type:Cash")) ||
                    (!accountType.CmpNoCase("Type:CCard")))
                {
                    log << _("Importing account type: ") << accountType << endl;
                    continue;
                }

                if (accountType == "Account")
                {
                    // account information
                    // Need to read till we get to end of account information
                    while( (readLine = getFileLine(text, numLines) ) != ("^"))
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
                if ( accountType == ("Option:AutoSwitch") )
                {
                    while((readLine = getFileLine(text, numLines)) != ("^"))
                    {
                        // ignore all lines
                    }
                    continue;
                }
                else if ( accountType == ("Type:Security") || accountType == ("Clear:AutoSwitch"))
                {
                    continue;
                }
                else if ( accountType == ("Type:Cat") )
                {
                    bool reading = true;
                    while( reading )
                    {
                        readLine = getFileLine(text, numLines);
                        if (readLine == ("!Type:Bank"))
                            reading = false;
                    }
                    continue;
                }

                // we do not know how to process this type yet
                wxString errMsgStr = _("Cannot process these QIF Account Types yet.");
                wxString errLineMsgStr = wxString() << _("Line: ") << numLines << ("  ") << readLine;

                log << errLineMsgStr << endl;
                log << errMsgStr << endl;
                wxMessageBox( errLineMsgStr + ("\n\n") + errMsgStr, _("QIF Import"), wxICON_ERROR);

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
                    payee = ("Unknown");
                }

                if (!core->payeeExists(payee))
                {
                    log << _("Payee Added: ") << payee << endl;
                    payeeID = core->addPayee(payee);
                }
                else
                    payeeID = core->getPayeeID(payee);

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
                notes << getLineData(readLine) << ("\n");
                continue;
            }
            else if (lineType(readLine) == Category)
            {
                categ = getLineData(readLine);

                wxStringTokenizer cattkz(categ, (":"));

                if (cattkz.HasMoreTokens())
                    cat = cattkz.GetNextToken();
                if (cattkz.HasMoreTokens())
                    subcat = cattkz.GetNextToken();

                categID = core->getCategoryID(cat);
                if (categID == -1)
                {
                    categID =  core->addCategory(cat);
                }

                if (!subcat.IsEmpty())
                {
                    subCategID = core->getSubCategoryID(categID, subcat);
                    if (subCategID == -1)
                    {
                        subCategID = core->addSubCategory(categID, subcat);
                    }
                }
                else
                    subCategID = -1;

                continue;
            }
            else if (lineType(readLine) == EOTLT)
            {
                wxString status = ("F");

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

                if (payee.Trim().IsEmpty())
                {
                    if (payee.Trim().IsEmpty())
                    {
                        payee = ("Unknown");
                    }

                    if (!core->payeeExists(payee))
                    {
                        payeeID = core->addPayee(payee);
                        log << _("Payee Added: ") << payee << endl;
                    }
                    else
                    {
                        payeeID = core->getPayeeID(payee);
                    }
                }

                if (categ.Trim().IsEmpty())
                {
                    // check if category exists for this payee.
                    mmDBWrapper::getPayee(db_, payeeID, categID, subCategID);

                    if (categID == -1)
                    {
                        log << _("Category is empty, marking transaction as Unknown category") << endl;

                        categID = core->getCategoryID("Unknown");
                        if (categID == -1)
                        {
                            categID =  core->addCategory("Unknown");
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

                int toAccountID = -1;

                boost::shared_ptr<mmBankTransaction> pTransaction(new mmBankTransaction(core->db_));
                pTransaction->accountID_ = fromAccountID;
                pTransaction->toAccountID_ = toAccountID;
                pTransaction->payee_ = core->getPayeeSharedPtr(payeeID);
                pTransaction->transType_ = type;
                pTransaction->amt_ = val;
                pTransaction->status_ = status;
                pTransaction->transNum_ = transNum;
                pTransaction->notes_ = (wxString(notes.Last()) == ("\n") ? notes.RemoveLast() : notes);
                pTransaction->category_ = core->getCategorySharedPtr(categID, subCategID);
                pTransaction->date_ = dtdt;
                pTransaction->toAmt_ = 0.0;
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

        dlg.Update(100);

        log << _("Transactions imported from QIF: ") << numImported << endl;
        wxString confirmMsg;
        confirmMsg  << _("Total Imported : ") << numImported << "\n\n"
                    << _("Log file written to : ") << logFile.GetFullPath() << "\n\n"
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
    fileviewer(logFile.GetFullPath(), 0).ShowModal();

    return fromAccountID;
}


void mmExportQIF(mmCoreDB* core, wxSQLite3Database* db_)
{
    if (core->getNumAccounts() == 0)
    {
        wxMessageBox(_("No Account available for export"), _("QIF Export"), wxOK|wxICON_WARNING);
        return;
    }


    const wxString delimit = mmDBWrapper::getInfoSettingValue(db_, "DELIMITER", mmex::DEFDELIMTER);

    const wxArrayString as = core->getAccountsName();
    wxSingleChoiceDialog scd(0, _("Choose Account to Export from:"),_("QIF Export"), as);

    wxString acctName;

    if (scd.ShowModal() != wxID_OK) return;
    acctName = scd.GetStringSelection();

    const wxString chooseExt = _("QIF Files") + (" (*.qif)|*.qif;*.QIF");
    wxString fileName = wxFileSelector(_("Choose QIF data file to Export"),
                        wxEmptyString, wxEmptyString, wxEmptyString, chooseExt, wxFD_SAVE | wxFD_OVERWRITE_PROMPT);

    if (fileName.IsEmpty()) return;

    correctEmptyFileExt("qif", fileName);

    wxFileOutputStream output(fileName);
    wxTextOutputStream text(output);
    int fromAccountID = core->getAccountID(acctName);
    wxString amtSeparator =  core->accountList_.getAccountCurrencyDecimalChar(fromAccountID);

    static const char sql[] =
            "SELECT transid, transdate as DATE, "
            "transcode as TRANSACTIONTYPE, transamount as AMOUNT, totransamount as TOAMOUNT, "
            " SUBCATEGID, "
            "CATEGID, PAYEEID, "
            "TRANSACTIONNUMBER, NOTES, TOACCOUNTID, ACCOUNTID "
            "FROM checkingaccount_v1 "
            "WHERE ACCOUNTID = ? OR TOACCOUNTID = ?"
            "ORDER BY transdate";

    wxSQLite3Statement st = db_->PrepareStatement(sql);
    st.Bind(1, fromAccountID);
    st.Bind(2, fromAccountID);

    wxSQLite3ResultSet q1 = st.ExecuteQuery();
    long numRecords = 0;

    text << ("!Account") << endl
         << ("N") << acctName <<  endl
         << ("TChecking") << endl
         << ("^") <<  endl
         << ("!Type:Cash") << endl;

    while (q1.NextRow())
    {
        const wxString transid = q1.GetString("TRANSID");
        const wxString dateDBString = q1.GetString("DATE");
        const wxDateTime dtdt = mmGetStorageStringAsDate(dateDBString);
        const wxString dateString = mmGetDateForDisplay(db_, dtdt);

        int sid, cid;
        wxString payee = mmDBWrapper::getPayee(db_, q1.GetInt("PAYEEID"), sid, cid);
        const wxString type = q1.GetString("TRANSACTIONTYPE");

        wxString amount = adjustedExportAmount(amtSeparator, q1.GetString("AMOUNT"));
        //Amount should be formated
        double value = 0.0;
        mmex::formatCurrencyToDouble(amount, value);
        mmex::formatDoubleToCurrencyEdit(value, amount);

        wxString toamount = q1.GetString("TOAMOUNT");
        //Amount should be formated
        value = 0.0;
        mmex::formatCurrencyToDouble(toamount, value);
        mmex::formatDoubleToCurrencyEdit(value, toamount);

        const wxString transNum = q1.GetString("TRANSACTIONNUMBER");
        const wxString categ = core->getCategoryName(q1.GetInt("CATEGID"));
        const wxString subcateg = mmDBWrapper::getSubCategoryName(db_,
                            q1.GetInt("CATEGID"), q1.GetInt("SUBCATEGID"));
        wxString notes = q1.GetString("NOTES");
        notes.Replace("''", "'");
        notes.Replace("\n", " ");
        wxString subcategStr = "" ;

        if (type == "Transfer")
        {
            subcategStr = type;
            int tAccountID = q1.GetInt("TOACCOUNTID");
            int fAccountID = q1.GetInt("ACCOUNTID");

            const wxString fromAccount = core->getAccountName(fAccountID);
            const wxString toAccount = core->getAccountName(tAccountID);

            if (tAccountID == fromAccountID) {
                payee = wxString() << "[" << fromAccount << "]";
                amount = toamount;
            } else if (fAccountID == fromAccountID) {
                payee = wxString() << "[" << toAccount << "]";
                amount.Prepend("-");
            }
        }
        else
        {
            subcategStr << categ << (subcateg != "" ? ":" : "") << subcateg;
        }

        text << ('D') << dateString << endl
             << ('T') << (type == ("Withdrawal") ? ("-") : ("")) << amount << endl //FIXME: is T needed when Transfer?
             << ('P') << payee << endl
             << ('N') << transNum << endl
             //Category or Transfer
             << ('L') << subcategStr << endl
             << ('M') << notes << endl;
        if (type == ("Transfer"))
        {
            text << ('$') << amount << endl;
        }

        //if categ id is empty the transaction has been splited
        if (categ.IsEmpty() && subcateg.IsEmpty())
        {
            static const char sql4splitedtrx[] =
                    "SELECT SUBCATEGID, CATEGID, SPLITTRANSAMOUNT "
                    "FROM splittransactions_v1 "
                    "WHERE TRANSID = ?";

            wxSQLite3Statement st2 = db_->PrepareStatement(sql4splitedtrx);
            st2.Bind(1, transid);

            wxSQLite3ResultSet q2 = st2.ExecuteQuery();

            while (q2.NextRow())
            {
                wxString splitamount = adjustedExportAmount(amtSeparator,q2.GetString("SPLITTRANSAMOUNT"));
                //Amount should be formated
                value = 0.0;
                mmex::formatCurrencyToDouble(splitamount, value);
                mmex::formatDoubleToCurrencyEdit(value, splitamount);
                wxString splitcateg = core->getCategoryName(q2.GetInt("CATEGID"));
                wxString splitsubcateg = mmDBWrapper::getSubCategoryName(db_,
                                            q2.GetInt("CATEGID"), q2.GetInt("SUBCATEGID"));
                text << ('S') << splitcateg << (splitsubcateg != "" ? ":" : "") << splitsubcateg << endl
                << ('$') << (type == "Withdrawal" ? "-" : "") << splitamount << endl
                // E Split memo — any text to go with this split item. I saggest Category:Subcategory = Amount for earch line
                << ('E') << splitcateg << (splitsubcateg != "" ? ":" : "") << splitsubcateg << (type == "Withdrawal" ? " -" : " ") << splitamount << endl;
            }

            q2.Finalize();
        }

        text << ('^') << endl;
        numRecords++;
    }

    q1.Finalize();

    wxString msg = wxString::Format(_("%ld transactions exported"), numRecords);
    mmShowErrorMessage(0, msg, _("Export to QIF"));
}


/*
Quicken Interchange Format (QIF) files

Ref: http://en.wikipedia.org/wiki/Quicken_Interchange_Format
     http://www.respmech.com/mym2qifw/qif_new.htm

The Quicken interchange format (QIF) is a specially formatted text (ASCII) file that
enables Quicken transactions to be moved from one Quicken account register into
another Quicken account register, or to or from other programs that support the QIF format.

Note: For Quicken to translate data from a text file into the Quicken register as transactions,
the text file must be in the QIF format.

Required File Formatting

Each transaction must end with a symbol, indicating the end of entry. Each item in the
transaction must display on a separate line. When Quicken exports an account register or list,
it adds a line to the top of the file that identifies the type of account or list. Listed below
are the header lines Quicken adds to the exported files:

Header              Type of data
!Type:Bank          Bank account transactions
!Type:Cash          Cash account transactions
!Type:CCard         Credit card account transactions
!Type:Invst         Investment account transactions
!Type:Oth A         Asset account transactions
!Type:Oth L         Liability account transactions

!Account            Account list or which account follows
!Type:Cat           Category list
!Type:Class         Class list
!Type:Memorized     Memorized transaction list

Quicken can be configured to import all transfers, regardless of whether Ignore Transfers
is selected when the file is imported. To do this, add a line to the file being imported
into the Quicken account. Use a text editor or word processor to put the following line
immediately after the header line at the top of the file:

    !Option:AllXfr

Items for Non-Investment Accounts

Each item in a bank, cash, credit card, other liability, or other asset account must
begin with a letter that indicates the field in the Quicken
register. The non-split items can be in any sequence:
Field  Indicator Explanations
D      Date
T      Amount
C      Cleared status
N      Num (check or reference number)
P      Payee
M      Memo
A      Address (up to five lines; the sixth line is an optional message)
L      Category (Category/Subcategory/Transfer/Class)
S      Category in split (Category/Transfer/Class)
E      Memo in split
$      Dollar amount of split
^      End of entry

Note: Repeat the S, E, and $ lines as many times as needed for additional items in a split.
If an item is omitted from the transaction in the QIF file, Quicken treats it as a blank item.

Items for Investment Accounts
Field  Indicator Explanation
D      Date
N      Action
Y      Security name
I      Price
Q      Quantity (number of shares or split ratio)
T      Transaction amount
C      Cleared status
P      Text in the first line for transfers and reminders
M      Memo
O      Commission
L      Account for the transfer
$      Amount transferred
^      End of entry

Items for Account Information

The account header !Account is used in two places, at the start of an account list and the
start of a list of transactions to specify to which account they belong.
Field  Indicator Explanation
N   Name
T   Type of account
D   Description
L   Credit limit (only for credit card account)
/   Statement balance date
$   Statement balance
^   End of entry

Items for a Category List
Field   Indicator Explanation
N   Category name:subcategory name
D   Description
T   Tax related if included, not tax related if omitted
I   Income category
E   Expense category (if category is unspecified, Quicken assumes expense type)
B   Budget amount (only in a Budget Amounts QIF file)
R   Tax schedule information
^   End of entry

Items for a Class List
Field   Indicator Explanation
N   Class name
D   Description
^   End of entry


Items for a Memorized Transaction List

Immediately preceding the ^ character, each entry must end with one of the following file
indicators to specify the transaction type.

    KC

    KD

    KP

    KI

    KE

With that exception, memorized transaction entries have the same format as regular
transaction entries (non-investment accounts). However, the Date or Num field is included.
All items are optional, but if an amortization record is included, all seven amortization
lines must also be included.

Field   Indicator Explanation
KC  Check transaction
KD  Deposit transaction
KP  Payment transaction
KI  Investment transaction
KE  Electronic payee transaction
T   Amount
C   Cleared status
P   Payee
M   Memo
A   Address
L   Category or Transfer/Class
S   Category/class in split
E   Memo in split
$   Dollar amount of split
1   Amortization: First payment date
2   Amortization: Total years for loan
3   Amortization: Number of payments already made
4   Amortization: Number of periods per year
5   Amortization: Interest rate
6   Amortization: Current loan balance
7   Amortization: Original loan amount
^   End of entry

Examples of QIF files

Normal Transactions Example
Transaction Item    Comment (not in file)
!Type:Bank  Header
D6/ 1/94    Date
T-1,000.00  Amount
N1005   Check number
PBank Of Mortgage   Payee
L[linda]    Category
S[linda]    First category in split
$-253.64    First amount in split
SMort Int   Second category in split
$=746.36    Second amount in split
^   End of transaction
D6/ 2/94    Date
T75.00  Amount
PDeposit    Payee
^   End of transaction
D6/ 3/94    Date
T-10.00     Amount
PAnthony Hopkins    Payee
MFilm   Memo
LEntertain  Category
AP.O. Box 27027     Address (line 1)
ATucson, AZ     Address (line 2)
A85726  Address (line 3)
A   Address (line 4)
A   Address (line 5)
A   Address (line 6)
^   End of transaction

Investment Example
Transaction Item    Comment (not in file)
!Type:Invst     Header line
D8/25/93    Date
NShrsIn     Action (optional)
Yibm4   Security
I11.260     Price
Q88.81  Quantity
CX  Cleared status
T1,000.00   Amount
MOpening    Balance Memo
^   End of transaction
D8/25/93    Date
NBuyX   Action
Yibm4   Security
I11.030     Price
Q9.066  Quantity
T100.00     Amount
MEst. price as of 8/25/93   Memo
L[CHECKING]     Account for transfer
$100.00     Amount transferred
^   End of transaction

Memorized List Example
Transaction Item    Comment (not in file)
!Type:Memorized     Header line
T-50.00     Amount
POakwood Gardens    Payee
MRent   Memo
KC  Check transaction
^   End of transaction

*/
