/*
Quicken Interchange Format (QIF) files

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

Header 	Type of data
!Type:Bank 	Bank account transactions
!Type:Cash 	Cash account transactions
!Type:CCard 	Credit card account transactions
!Type:Invst 	Investment account transactions
!Type:Oth A 	Asset account transactions
!Type:Oth L 	Liability account transactions
!Account 	Account list or which account follows
!Type:Cat 	Category list
!Type:Class 	Class list
!Type:Memorized 	Memorized transaction list

Quicken can be configured to import all transfers, regardless of whether Ignore Transfers 
is selected when the file is imported. To do this, add a line to the file being imported 
into the Quicken account. Use a text editor or word processor to put the following line 
immediately after the header line at the top of the file:

    !Option:AllXfr

Items for Non-Investment Accounts

Each item in a bank, cash, credit card, other liability, or other asset account must 
begin with a letter that indicates the field in the Quicken
register. The non-split items can be in any sequence:
Field 	Indicator Explanations
D 	Date
T 	Amount
C 	Cleared status
N 	Num (check or reference number)
P 	Payee
M 	Memo
A 	Address (up to five lines; the sixth line is an optional message)
L 	Category (Category/Subcategory/Transfer/Class)
S 	Category in split (Category/Transfer/Class)
E 	Memo in split
$ 	Dollar amount of split
^ 	End of entry

Note: Repeat the S, E, and $ lines as many times as needed for additional items in a split. 
If an item is omitted from the transaction in the QIF file, Quicken treats it as a blank item.

Items for Investment Accounts
Field 	Indicator Explanation
D 	Date
N 	Action
Y 	Security
I 	Price
Q 	Quantity (number of shares or split ratio)
T 	Transaction amount
C 	Cleared status
P 	Text in the first line for transfers and reminders
M 	Memo
O 	Commission
L 	Account for the transfer
$ 	Amount transferred
^ 	End of entry

Items for Account Information

The account header !Account is used in two places, at the start of an account list and the 
start of a list of transactions to specify to which account they belong.
Field 	Indicator Explanation
N 	Name
T 	Type of account
D 	Description
L 	Credit limit (only for credit card account)
/ 	Statement balance date
$ 	Statement balance
^ 	End of entry

Items for a Category List
Field 	Indicator Explanation
N 	Category name:subcategory name
D 	Description
T 	Tax related if included, not tax related if omitted
I 	Income category
E 	Expense category (if category is unspecified, Quicken assumes expense type)
B 	Budget amount (only in a Budget Amounts QIF file)
R 	Tax schedule information
^ 	End of entry

Items for a Class List
Field 	Indicator Explanation
N 	Class name
D 	Description
^ 	End of entry
  	 

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

Field 	Indicator Explanation
KC 	Check transaction
KD 	Deposit transaction
KP 	Payment transaction
KI 	Investment transaction
KE 	Electronic payee transaction
T 	Amount
C 	Cleared status
P 	Payee
M 	Memo
A 	Address
L 	Category or Transfer/Class
S 	Category/class in split
E 	Memo in split
$ 	Dollar amount of split
1 	Amortization: First payment date
2 	Amortization: Total years for loan
3 	Amortization: Number of payments already made
4 	Amortization: Number of periods per year
5 	Amortization: Interest rate
6 	Amortization: Current loan balance
7 	Amortization: Original loan amount
^ 	End of entry

Examples of QIF files

Normal Transactions Example
Transaction Item 	Comment (not in file)
!Type:Bank 	Header
D6/ 1/94 	Date
T-1,000.00 	Amount
N1005 	Check number
PBank Of Mortgage 	Payee
L[linda] 	Category
S[linda] 	First category in split
$-253.64 	First amount in split
SMort Int 	Second category in split
$=746.36 	Second amount in split
^ 	End of transaction
D6/ 2/94 	Date
T75.00 	Amount
PDeposit 	Payee
^ 	End of transaction
D6/ 3/94 	Date
T-10.00 	Amount
PAnthony Hopkins 	Payee
MFilm 	Memo
LEntertain 	Category
AP.O. Box 27027 	Address (line 1)
ATucson, AZ 	Address (line 2)
A85726 	Address (line 3)
A 	Address (line 4)
A 	Address (line 5)
A 	Address (line 6)
^ 	End of transaction

Investment Example
Transaction Item 	Comment (not in file)
!Type:Invst 	Header line
D8/25/93 	Date
NShrsIn 	Action (optional)
Yibm4 	Security
I11.260 	Price
Q88.81 	Quantity
CX 	Cleared status
T1,000.00 	Amount
MOpening 	Balance Memo
^ 	End of transaction
D8/25/93 	Date
NBuyX 	Action
Yibm4 	Security
I11.030 	Price
Q9.066 	Quantity
T100.00 	Amount
MEst. price as of 8/25/93 	Memo
L[CHECKING] 	Account for transfer
$100.00 	Amount transferred
^ 	End of transaction

Memorized List Example
Transaction Item 	Comment (not in file)
!Type:Memorized 	Header line
T-50.00 	Amount
POakwood Gardens 	Payee
MRent 	Memo
KC 	Check transaction
^ 	End of transaction

*/	 

#include "util.h"
#include "dbwrapper.h"
#include "guiid.h"
#include "fileviewerdialog.h"
#include "mmex.h"

typedef enum qifAccountInfoType 
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

typedef enum qifLineType 
 {
     AcctType = 1, // !
     Date        = 2, // D
     Amount      = 3, // T
     Address     = 4, // A
     Payee       = 5, // P
     EOTLT         = 6, // ^
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
        (fChar == '$'))
        return true;

    return false;
}

wxString getLineData(const wxString& line)
{
    wxString dataString = line.substr(1, line.Length()-1);
    return dataString;
}

int mmImportQIF(wxSQLite3Database* db_)
{
    if (mmDBWrapper::getNumAccounts(db_) == 0)
    {
        mmShowErrorMessage(0, _("No Account available! Cannot Import! Create a new account first!"), 
            _("Error"));
        return -1;
    }

    wxArrayString as;
    int fromAccountID = -1;

    mmBEGINSQL_LITE_EXCEPTION;
    wxSQLite3ResultSet q1 = db_->ExecuteQuery("select * from ACCOUNTLIST_V1 where ACCOUNTTYPE='Checking' order by ACCOUNTNAME;");
    while (q1.NextRow())
    {
        as.Add(q1.GetString(wxT("ACCOUNTNAME")));
    }
    mmENDSQL_LITE_EXCEPTION
    
    wxSingleChoiceDialog* scd = new wxSingleChoiceDialog(0, _("Choose Account to import to:"), 
        _("QIF Import"), as);
    if (scd->ShowModal() != wxID_OK)
        return -1;

    wxString acctName = scd->GetStringSelection();
    fromAccountID = mmDBWrapper::getAccountID(db_, acctName);

    wxString fileName = wxFileSelector(_("Choose QIF data file to import"), 
        wxT(""), wxT(""), wxT(""), wxT("*.qif"), wxFILE_MUST_EXIST);
    if ( !fileName.IsEmpty() )
    {
        wxFileInputStream input( fileName );
        wxTextInputStream text( input );

        /* Create Log File */
        wxFileName fname(wxTheApp->argv[0]);
        wxFileName csvName(fileName);
        wxString logFile = fname.GetPath(wxPATH_GET_VOLUME) + wxT("\\") 
            + csvName.GetName() + wxT(".txt");
        wxFileOutputStream outputLog( logFile );
        wxTextOutputStream log( outputLog );

        wxString readLine;
        int numLines = 0;
        int numImported = 0;

        wxString dt = wxDateTime::Now().FormatISODate();
        wxString payee = wxT("");
        wxString type = wxT("");
        wxString amount = wxT("");
        wxString categ = wxT("");
        wxString subcateg = wxT("");
        wxString transNum = wxT("");
        wxString notes = wxT("");
        wxString convDate = wxDateTime::Now().FormatISODate();
        int payeeID, categID, subCategID;
        subCategID = -1;
        double val = 0.0;
        while(!input.Eof())
        {              
            readLine = text.ReadLine();
            numLines++;
            if (readLine.Length() == 0)
                continue;

            bool isOK = isLineOK(readLine);  
            if (!isOK)
            {
                log << _("Line : " ) << numLines 
                    << _(" incorrect QIF line type") << endl;                    
                continue;
            }

            if (lineType(readLine) == AcctType)
            {
                wxString accountType = getLineData(readLine);
                if ((accountType == wxT("Type:Bank")) ||
                    (accountType == wxT("Type:Cash")) ||
                    (accountType == wxT("Type:CCard")))
                {
                    // cannot handle these yet
                    log << _("Importing account type : ") << accountType << endl;      
                    continue;
                }

                if (accountType == wxT("Account"))
                {
                    // account information
                    // Need to read till we get to end of account information
                    while((readLine = text.ReadLine()) != wxT("^"))
                    {
                        numLines++;
                        if (accountInfoType(readLine) == Name)
                        {
                            log << _("Line : " ) << numLines << _(" : ")
                                << getLineData(readLine) << _(" account name ") << endl;    
                            continue;
                        }

                        if (accountInfoType(readLine) == AccountType)
                        {
                            //accountInfo.type = getLineData(readLine);
                            continue;
                        }

                        if (accountInfoType(readLine) == Description)
                        {
                            //accountInfo.description = getLineData(readLine);
                            continue;
                        }

                        if (accountInfoType(readLine) == CreditLimit)
                        {
                            //accountInfo.creditLimit = getLineData(readLine);
                            continue;
                        }

                        if (accountInfoType(readLine) == BalanceDate)
                        {
                            //accountInfo.balanceDate = Convert.ToDateTime(getLineData(readLine));
                            continue;
                        }

                        if (accountInfoType(readLine) == Balance)
                        {
                            //accountInfo.balance = Convert.ToSingle(getLineData(readLine));
                            continue;
                        }
                    }
                    continue;
                }

                // we do not know how to process this type yet
                log << _(" cannot process Account Types yet ") << endl;    
                return -1;
            }

            if (lineType(readLine) == Payee)
            {
                payee = getLineData(readLine);
                
                if (!mmDBWrapper::getPayeeID(db_, payee, payeeID, categID, subCategID))
                {
                    mmDBWrapper::addPayee(db_, payee, -1, -1);
                    log << _("Adding payee ") << payee << endl;    
                    mmDBWrapper::getPayeeID(db_, payee, payeeID, categID, subCategID);
                }
                continue;
            }

            if (lineType(readLine) == Amount)
            {
                amount = getLineData(readLine);
              
                if (!amount.ToDouble(&val))
                {
                    log << _("Line : " ) << numLines 
                        << _(" invalid amount, skipping.") << endl;

                    continue;
                }

                if (val <= 0.0)
                    type = _("Withdrawal");
                else
                    type = _("Deposit");
                val = fabs(val);
                continue;
            }

            if (lineType(readLine) == Address)
            {
                continue;
            }

            if (lineType(readLine) == Date)
            {
                dt = getLineData(readLine);
                wxDateTime dtdt;
                const wxChar* ch = dtdt.ParseDate(dt.c_str());
                if (ch != NULL)
                    convDate = dtdt.FormatISODate();
                else
                {
                    log << _("unable to parse date, using today's date..please verify and change ")
                        << endl;    
                }
                continue;
            }

            if (lineType(readLine) == TransNumber)
            {
                transNum = getLineData(readLine);
                continue;
            }

            if (lineType(readLine) == Memo)
            {
                notes = getLineData(readLine);
                continue;
            }

            if (lineType(readLine) == Category)
            {
                categ = getLineData(readLine);

                wxStringTokenizer cattkz(categ, wxT(":"));
                wxString cat, subcat;
                subcat = wxT("");
                if (cattkz.HasMoreTokens())
                    cat = cattkz.GetNextToken();
                if (cattkz.HasMoreTokens())
                    subcat = cattkz.GetNextToken();

                categID = mmDBWrapper::getCategoryID(db_, cat);
                if (categID == -1)
                {
                    mmDBWrapper::addCategory(db_, cat);
                    categID = mmDBWrapper::getCategoryID(db_, cat);
                }

                if (!subcat.IsEmpty())
                {
                    subCategID = mmDBWrapper::getSubCategoryID(db_, categID, subcat);
                    if (subCategID == -1)
                    {
                        mmDBWrapper::addSubCategory(db_, categID, subcat);
                        subCategID = mmDBWrapper::getSubCategoryID(db_, categID, subcat);
                    }
                }


                continue;
            }

            if (lineType(readLine) == EOTLT)
            {
                wxString status = wxT("F");

                if (dt.Trim().IsEmpty() || payee.Trim().IsEmpty() ||
                    type.Trim().IsEmpty() || amount.Trim().IsEmpty() ||
                     categ.Trim().IsEmpty())
                {
                    log << _(" One of the following fields: date, payee, transaction type, amount, category strings is empty, skipping") << endl;
                    continue;
                }

                int toAccountID = -1;
                mmBEGINSQL_LITE_EXCEPTION;
                wxString bufSQL = wxString::Format(wxT("insert into CHECKINGACCOUNT_V1 (ACCOUNTID, TOACCOUNTID, PAYEEID, TRANSCODE, \
                                                       TRANSAMOUNT, STATUS, TRANSACTIONNUMBER, NOTES,                               \
                                                       CATEGID, SUBCATEGID, TRANSDATE)                                              \
                                                       values (%d, %d, %d, '%s', %f, '%s', '%s', '%s', %d, %d, '%s');"),
                                                       fromAccountID, toAccountID, payeeID, type.c_str(), val,
                                                       status.c_str(), transNum.c_str(), mmCleanString(notes).c_str(), categID, subCategID, convDate.c_str() );  

               
                int retVal = db_->ExecuteUpdate(bufSQL);
                mmENDSQL_LITE_EXCEPTION;

                payee = wxT("");
                type = wxT("");
                amount = wxT("");
                categ = wxT("");
                notes = wxT("");
                subCategID = -1;
                categID = -1;
                val = 0.0;
                convDate = wxDateTime::Now().FormatISODate();

                numImported++;
                continue;
            }
        }

        log << numImported << _(" transactions imported from QIF") << endl;

        outputLog.Close();

        fileviewer* dlg = new fileviewer(logFile, 0);
        dlg->ShowModal();
        dlg->Destroy();

    }

   
    scd->Destroy();
    return fromAccountID;
}