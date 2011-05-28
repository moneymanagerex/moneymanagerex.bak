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
#include "mmcoredb.h"
#include "platfdep.h"

namespace
{
const char g_AccountNameSQL[] = 
    "select ACCOUNTNAME "
    "from ACCOUNTLIST_V1 "
    "where (ACCOUNTTYPE = 'Checking' or ACCOUNTTYPE = 'Term') and STATUS != 'Closed' "
    "order by ACCOUNTNAME";
};
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

int mmImportQIF(mmCoreDB* core)
{
    wxSQLite3Database* db_ = core->db_.get();
    if (mmDBWrapper::getNumAccounts(db_) == 0)
    {
        mmShowErrorMessage(0, _("No Account available! Cannot Import! Create a new account first!"), 
            _("Error"));
        return -1;
    }

    wxMessageDialog msgDlg(NULL, 
_("To import QIF files correctly, the date option set in MMEX \n must match the format of the date in the QIF file.\n\
Are you are sure you want to proceed with the import?"),
                                 _("Confirm QIF import"),
                                        wxYES_NO);
    if (msgDlg.ShowModal() != wxID_YES)
    {
        return -1;
    }
    
    wxArrayString as;
    int fromAccountID = -1;

    wxSQLite3ResultSet q1 = db_->ExecuteQuery(g_AccountNameSQL);
    while (q1.NextRow())
    {
        as.Add(q1.GetString(wxT("ACCOUNTNAME")));
    }
    q1.Finalize();
    
    wxSingleChoiceDialog scd(0, _("Choose Account to import to:"), _("QIF Import"), as);
    if (scd.ShowModal() != wxID_OK)
        return -1;

    wxString acctName = scd.GetStringSelection();
    fromAccountID = mmDBWrapper::getAccountID(db_, acctName);

    boost::shared_ptr<mmCurrency> pCurrencyPtr = core->accountList_.getCurrencyWeakPtr(fromAccountID).lock();
    wxASSERT(pCurrencyPtr);


    wxString fileName = wxFileSelector(_("Choose QIF data file to import"), 
        wxT(""), wxT(""), wxT(""), wxT("*.qif"), wxFILE_MUST_EXIST);
    wxFileName logFile = mmex::GetLogDir(true);
    logFile.SetFullName(fileName);
    logFile.SetExt(wxT("txt"));

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
        wxString payee;
        wxString type;
        wxString amount;
        wxString categ;
        wxString subcateg;
        wxString transNum;
        wxString notes;
        wxString convDate;
        wxDateTime dtdt = wxDateTime::Now();
        int payeeID = -1, categID = -1, subCategID = -1;
        subCategID = -1;
        double val = 0.0;

		wxProgressDialog dlg(_("Please Wait"), _(" transactions imported from QIF"), 101, false, wxPD_AUTO_HIDE | wxPD_APP_MODAL | wxPD_SMOOTH | wxPD_CAN_ABORT);
        while(!input.Eof())
        {   
			notes = wxT("");
			notes << numImported << _(" transactions imported from QIF");
            dlg.Update(static_cast<int>((static_cast<double>(numImported)/100.0 - numImported/100) *100), notes);
            notes = wxT("");

			if (!dlg.Update(-1)) // if cancel clicked
			return -1; // abort processing

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
                if ((!accountType.CmpNoCase(wxT("Type:Bank"))) ||
                    (!accountType.CmpNoCase(wxT("Type:Cash"))) ||
                    (!accountType.CmpNoCase(wxT("Type:CCard"))))
                {
                    log << _("Importing account type : ") << accountType << endl;      
                    continue;
                }

                if (accountType == wxT("Account"))
                {
                    // account information
                    // Need to read till we get to end of account information
                    while((readLine = text.ReadLine()) != wxT("^"))
                    {
                        payee = wxT("");
                        type = wxT("");
                        amount = wxT("");
                        categ = wxT("");
                        notes = wxT("");
                        subCategID = -1;
                        transNum = wxT("");
                        categID = -1;
                        val = 0.0;
                        convDate = wxDateTime::Now().FormatISODate();                        
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
                mmShowErrorMessage(0, _("Cannot process these QIF Account Types yet"), 
                    _("Error"));
                return -1;
            }

            if (lineType(readLine) == Payee)
            {
                payee = getLineData(readLine);
                if (payee.Trim().IsEmpty())
                {
                    payee = wxT("Unknown");
                }

                if (!core->payeeList_.payeeExists(payee))
                {
                    log << _("Adding payee ") << payee << endl;   
                    payeeID = core->payeeList_.addPayee(payee);
                }
                else
                    payeeID = core->payeeList_.getPayeeID(payee);
                
                continue;
            }

            if (lineType(readLine) == Amount)
            {
                amount = getLineData(readLine);
              
                if (!mmex::formatCurrencyToDouble(amount, val))
                {
                    log << _("Line : " ) << numLines 
                        << _(" invalid amount, skipping.") << endl;

                    continue;
                }

                if (val <= 0.0)
                    type = wxT("Withdrawal");
                else
                    type = wxT("Deposit");
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

			    dtdt = mmParseDisplayStringToDate(db_, dt);
                convDate = dtdt.FormatISODate();
                continue;
            }

            if (lineType(readLine) == TransNumber)
            {
                transNum = getLineData(readLine);
                continue;
            }

            if (lineType(readLine) == Memo || lineType(readLine) == MemoSplit )
            {
                notes << getLineData(readLine) << wxT("\n"); //FIXME: \n not needed for every line 
                continue;
            }

            wxString cat, subcat;
            if (lineType(readLine) == Category)
            {
                categ = getLineData(readLine);

                wxStringTokenizer cattkz(categ, wxT(":"));
                
                subcat = wxT("");
                if (cattkz.HasMoreTokens())
                    cat = cattkz.GetNextToken();
                if (cattkz.HasMoreTokens())
                    subcat = cattkz.GetNextToken();

                categID = core->categoryList_.getCategoryID(cat);
                if (categID == -1)
                {
                    categID =  core->categoryList_.addCategory(cat);
                }


                if (!subcat.IsEmpty())
                {
                    subCategID = core->categoryList_.getSubCategoryID(categID, subcat);
                    if (subCategID == -1)
                    {
                        subCategID = core->categoryList_.addSubCategory(categID, subcat);
                    }
                }
                else
                    subCategID = -1;

                continue;
            }

            if (lineType(readLine) == EOTLT)
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

                if (payee.Trim().IsEmpty())
                {
                    if (payee.Trim().IsEmpty())
                    {
                        payee = wxT("Unknown");
                    }

                    if (!core->payeeList_.payeeExists(payee))
                    {
                        payeeID = core->payeeList_.addPayee(payee);
                        log << _("Adding payee ") << payee << endl;    
                    }
                    else
                    {
                        payeeID = core->payeeList_.getPayeeID(payee);
                    }
                }
                
                
                if (categ.Trim().IsEmpty())
                {
                    // check if category exists for this payee.
                    mmDBWrapper::getPayee(db_, payeeID, categID, subCategID);

                    if (categID == -1)
                    {
                        log << _("Category is empty, marking transaction as Unknown category") << endl;

                        categID = core->categoryList_.getCategoryID(wxT("Unknown"));
                        if (categID == -1)
                        {
                            categID =  core->categoryList_.addCategory(wxT("Unknown"));
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
               pTransaction->payee_ = core->payeeList_.getPayeeSharedPtr(payeeID);
               pTransaction->transType_ = type;
               pTransaction->amt_ = val;
               pTransaction->status_ = status;
               pTransaction->transNum_ = transNum;
               pTransaction->notes_ = notes;
               pTransaction->category_ = core->categoryList_.getCategorySharedPtr(categID, subCategID);
               pTransaction->date_ = dtdt;
               pTransaction->toAmt_ = 0.0;
               pTransaction->updateAllData(core, fromAccountID, pCurrencyPtr);

               core->bTransactionList_.addTransaction(core, pTransaction);

                numImported++;
                notes = wxT("");
				continue;
			}
        }
        log << numImported << _(" transactions imported from QIF") << endl;

        outputLog.Close();

        dlg.Update(101);
		dlg.Destroy();
    }
    if ( !fileName.IsEmpty() )
    fileviewer(logFile.GetFullPath(), 0).ShowModal();
   
    return fromAccountID;
}

