#include "reportcashflow.h"

#include "defs.h"
#include "htmlbuilder.h"
#include "reportbase.h"
#include "util.h"
#include "dbwrapper.h"

mmReportCashFlow::mmReportCashFlow(mmCoreDB* core, const wxArrayString* accountArray) : 
    mmPrintableBase(core), 
    activeTermAccounts_(false),
    activeBankAccounts_(false),
    accountArray_(accountArray)
{
}

void mmReportCashFlow::activateTermAccounts() 
{
    activeTermAccounts_ = true;
}

void mmReportCashFlow::activateBankAccounts() 
{
    activeBankAccounts_ = true;
}

wxString mmReportCashFlow::getHTMLText()
{
    core_->loadBaseCurrencySettings();

    mmHTMLBuilder hb;
    hb.init();

    int years = 10;
    wxString headerMsg = wxString::Format (_("Cash Flow Forecast for %d Years Ahead"), years);
        hb.addHeader(3, headerMsg );
    headerMsg = _("Accounts: ");
    if (accountArray_ == NULL) 
    {
        if (activeBankAccounts_ && activeTermAccounts_)
            headerMsg << _("All Accounts");
        else if (activeBankAccounts_)
            headerMsg << _("All Bank Accounts");
        else if (activeTermAccounts_)
            headerMsg << _("All Term Accounts");

    } 
    else 
    {
        int arrIdx = 0;
 
        if ( (int)accountArray_->size() == 0 )
            headerMsg << wxT("?");
        //else if ( (int)accountArray_->size() > 1 )
        //    msgString = wxT("s") + msgString; //<-- we can't translate it to other languages

        if ( (int)accountArray_->size() > 0 )
        {
            headerMsg << accountArray_->Item(arrIdx);
            arrIdx ++;
        }
        while ( arrIdx < (int)accountArray_->size() )
        {
            headerMsg << wxT(", ") << accountArray_->Item(arrIdx);
            arrIdx ++;
        }

    }

    hb.addHeader(6, headerMsg);

    wxDateTime now = wxDateTime::Now();
    wxString dt = _("Today's Date: ") + mmGetNiceDateString(now);
    hb.addHeader(7, dt);
    hb.addLineBreak();
    hb.addLineBreak();

    hb.startCenter();

    hb.startTable(wxT("65%"));
    hb.startTableRow();
    hb.addTableHeaderCell(_("Date"));
    hb.addTableHeaderCell(_("Total"));
    hb.addTableHeaderCell(_("Difference"));
    hb.endTableRow();

    double tInitialBalance = 0.0;
    std::map<wxDateTime, double> daily_balance;
          
    std::pair<mmAccountList::const_iterator, mmAccountList::const_iterator> range = core_->rangeAccount(); 
    for (mmAccountList::const_iterator it = range.first; it != range.second; ++ it)
    {
        const mmAccount* account = it->get();
        if (account->status_ == mmAccount::MMEX_Closed || account->acctType_ == ACCOUNT_TYPE_STOCK) continue;

        if (accountArray_) 
        {
            if (wxNOT_FOUND == accountArray_->Index(account->name_)) continue;
        }
        else
        {
            if (! activeTermAccounts_ && account->acctType_ == ACCOUNT_TYPE_TERM) continue;
            if (! activeBankAccounts_ && account->acctType_ == ACCOUNT_TYPE_BANK) continue;
        }


        core_->bTransactionList_.getDailyBalance(account->id_, daily_balance);

        boost::shared_ptr<mmCurrency> pCurrencyPtr = core_->accountList_.getCurrencyWeakPtr(account->id_).lock();
        wxASSERT(pCurrencyPtr);
        mmex::CurrencyFormatter::instance().loadSettings(*pCurrencyPtr);
        double rate = pCurrencyPtr->baseConv_;
        tInitialBalance += account->initialBalance_ * rate;
   }

    // We now know the total balance on the account
    // Start by walking through the repeating transaction list

    static const char sql[] = 
    "select NEXTOCCURRENCEDATE, "
            "REPEATS, "
            "NUMOCCURRENCES, "
            "TRANSCODE, "
            "TRANSAMOUNT, "
            "TOTRANSAMOUNT, "
            "TOACCOUNTID, "
            "ACCOUNTID "
    "from BILLSDEPOSITS_V1";

    wxDateTime yearFromNow = wxDateTime::Now().Add(wxDateSpan::Years(years));
    forecastVec fvec;

    wxSQLite3ResultSet q1 = core_->db_.get()->ExecuteQuery(sql);

    while (q1.NextRow())
    {
        wxString nextOccurrString = q1.GetString(wxT("NEXTOCCURRENCEDATE"));
        wxDateTime nextOccurDate = mmGetStorageStringAsDate(nextOccurrString);
           
        int repeats             = q1.GetInt(wxT("REPEATS"));
        int numRepeats          = q1.GetInt(wxT("NUMOCCURRENCES"));
        wxString transType      = q1.GetString(wxT("TRANSCODE"));
        double amt              = q1.GetDouble(wxT("TRANSAMOUNT"));
        double toAmt            = q1.GetDouble(wxT("TOTRANSAMOUNT"));

        // DeMultiplex the Auto Executable fields from the db entry: REPEATS
        if (repeats >= BD_REPEATS_MULTIPLEX_BASE)    // Auto Execute User Acknowlegement required
            repeats -= BD_REPEATS_MULTIPLEX_BASE;
        if (repeats >= BD_REPEATS_MULTIPLEX_BASE)    // Auto Execute Silent mode
            repeats -= BD_REPEATS_MULTIPLEX_BASE;

        bool processNumRepeats = false;
        if (numRepeats != -1)
            processNumRepeats = true;

        if (repeats == 0)
        {
            numRepeats = 1;
            processNumRepeats = true;
        }

        if (nextOccurDate > yearFromNow)
            continue;

        int accountID = q1.GetInt(wxT("ACCOUNTID"));
        int toAccountID = q1.GetInt(wxT("TOACCOUNTID"));

        bool isAccountFound = true, isToAccountFound = true;
        if (accountArray_ != NULL)
        {
            if (wxNOT_FOUND == accountArray_->Index(core_->getAccountName(accountID))) //linear search
                isAccountFound = false;

            if (wxNOT_FOUND == accountArray_->Index(core_->getAccountName(toAccountID))) //linear search
                isToAccountFound = false;
        }

        if (!isAccountFound && !isToAccountFound) continue; // skip account

        double convRate = mmDBWrapper::getCurrencyBaseConvRate(core_->db_.get(), accountID);
        double toConvRate = mmDBWrapper::getCurrencyBaseConvRate(core_->db_.get(), toAccountID);

        // Process all possible repeating transactions for this BD
        while(1)
        {
            if (nextOccurDate > yearFromNow)
                break;

            if (processNumRepeats)
                numRepeats--;

            mmRepeatForecast rf;
            rf.date = nextOccurDate;
            rf.amount = 0.0;

            if (transType == TRANS_TYPE_WITHDRAWAL_STR)
            {
                rf.amount = -amt * convRate;
            }
            else if (transType == TRANS_TYPE_DEPOSIT_STR)
            {
                rf.amount = +amt * convRate;
            }
            else //if (transType == TRANS_TYPE_TRANSFER_STR)
            {
                if (isAccountFound)
                    rf.amount -= amt * convRate;
                if (isToAccountFound)
                    rf.amount += toAmt * toConvRate;
            }

            fvec.push_back(rf);   

            if (processNumRepeats && (numRepeats <=0))
                break;

            if (repeats == 1)
            {
                nextOccurDate = nextOccurDate.Add(wxTimeSpan::Week());
            }
            else if (repeats == 2)
            {
                nextOccurDate = nextOccurDate.Add(wxTimeSpan::Weeks(2));
            }
            else if (repeats == 3)
            {
                nextOccurDate = nextOccurDate.Add(wxDateSpan::Month());
            }
            else if (repeats == 4)
            {
                nextOccurDate = nextOccurDate.Add(wxDateSpan::Months(2));
            }
            else if (repeats == 5)
            {
                nextOccurDate = nextOccurDate.Add(wxDateSpan::Months(3));
            }
            else if (repeats == 6)
            {
                nextOccurDate = nextOccurDate.Add(wxDateSpan::Months(6));
            }
            else if (repeats == 7)
            {
                nextOccurDate = nextOccurDate.Add(wxDateSpan::Year());
            }
            else if (repeats == 8)
            {
                nextOccurDate = nextOccurDate.Add(wxDateSpan::Months(4));
            }
            else if (repeats == 9)
            {
                nextOccurDate = nextOccurDate.Add(wxDateSpan::Weeks(4));
            }
            else if (repeats == 10)
            {
                nextOccurDate = nextOccurDate.Add(wxDateSpan::Days(1));
            }
        } // end while
    } //end query
    q1.Finalize();

    // Now we have a vector of dates and amounts over next year

    std::vector<double> forecastOver12Months(12 * years, 0.0);
    for (int idx = 0; idx < (int)forecastOver12Months.size(); idx++)
    {
        wxDateTime dtBegin = wxDateTime::Now();
        wxDateTime dtEnd   = wxDateTime::Now().Add(wxDateSpan::Months(idx));

        for (int fcIdx = 0; fcIdx < (int)fvec.size(); fcIdx++)
        {
            if (fvec[fcIdx].date.IsBetween(dtBegin, dtEnd))
                forecastOver12Months[idx] += fvec[fcIdx].amount;
        }

        for (std::map<wxDateTime, double>::const_iterator it = daily_balance.begin(); 
                it != daily_balance.end(); 
                ++ it)
        {
            if (! it->first.IsLaterThan(dtEnd))
                forecastOver12Months[idx] += it->second;
            else
                break;
        }
    }

    core_->loadBaseCurrencySettings();

    for (int idx = 0; idx < (int)forecastOver12Months.size(); idx++)
    {
        wxDateTime dtEnd   = wxDateTime::Now().Add(wxDateSpan::Months(idx));
           
        wxString balanceStr;
        double balance = forecastOver12Months[idx] + tInitialBalance;
        mmex::formatDoubleToCurrency(balance, balanceStr);
        wxString diffStr;
        double diff;
        diff = (idx==0 ? 0 : forecastOver12Months[idx] - forecastOver12Months[idx-1]) ;
        mmex::formatDoubleToCurrency(diff, diffStr);

        wxString dtStr ; 
        //dtStr << mmGetNiceShortMonthName(dtEnd.GetMonth()) << wxT(" ") << dtEnd.GetYear();
        dtStr << mmGetDateForDisplay(core_->db_.get(), dtEnd); 

        hb.startTableRow();
        hb.addTableCell(dtStr, false, true);
        hb.addTableCell(balanceStr, true, true, true, ((balance < 0) ? wxT("RED") : wxT("BLACK")));
        hb.addTableCell((idx==0 ? wxT ("") : diffStr), true, true, true, (diff < 0 ? wxT("RED") : wxT("BLACK"))) ;
        hb.endTableRow();
    }

    hb.endTable();
    hb.endCenter();
    hb.end();

    return hb.getHTMLText();
}
