
#include "cashflow.h"
#include "../constants.h"
#include "../htmlbuilder.h"

mmReportCashFlow::mmReportCashFlow(mmCoreDB* core, mmGUIFrame* frame, int cashflowreporttype, const wxArrayString* accountArray)
: mmPrintableBase(core)
, frame_(frame)
, accountArray_(accountArray)
, activeTermAccounts_(false)
, activeBankAccounts_(false)
, cashflowreporttype_(cashflowreporttype)
{}

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
    core_->currencyList_.LoadBaseCurrencySettings();

    mmHTMLBuilder hb;
    hb.init();
    int years = -1;
    if (cashflowreporttype_ == 0) // Monthly for 10 years
    {
        years =10; 
    }
    else if (cashflowreporttype_ == 1) // Daily for 1 year
    {
        years = 1;
    }
    
    wxASSERT(years != -1);
    
    wxString headerMsg = wxString::Format (_("Cash Flow Forecast for %d Years Ahead"), years);
        hb.addHeader(2, headerMsg );
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

    hb.addHeader(2, headerMsg);
    hb.addDateNow();
    hb.addLineBreak();

    hb.startCenter();

    hb.startTable(wxT("65%"));
    hb.startTableRow();
    hb.addTableHeaderCell(_("Date"));
    hb.addTableHeaderCell(_("Total"), true);
    hb.addTableHeaderCell(_("Difference"), true);
    hb.endTableRow();

    double tInitialBalance = 0.0;
    std::map<wxDateTime, double> daily_balance;
          
    std::pair<mmAccountList::const_iterator, mmAccountList::const_iterator> range = core_->accountList_.range();
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


        core_->bTransactionList_.getDailyBalance(core_, account->id_, daily_balance);

        wxSharedPtr<mmCurrency> pCurrencyPtr = core_->accountList_.getCurrencySharedPtr(account->id_);
        wxASSERT(pCurrencyPtr);
        mmex::CurrencyFormatter::instance().loadSettings(*pCurrencyPtr);
        double rate = pCurrencyPtr->baseConv_;
        tInitialBalance += account->initialBalance_ * rate;
   }

    // We now know the total balance on the account
    // Start by walking through the repeating transaction list

    wxDateTime yearFromNow = wxDateTime::Now().Add(wxDateSpan::Years(years));
    forecastVec fvec;

    wxSQLite3ResultSet q1 = core_->db_.get()->ExecuteQuery(SELECT_ALL_FROM_BILLSDEPOSITS_V1);

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
            if (wxNOT_FOUND == accountArray_->Index(core_->accountList_.GetAccountName(accountID))) //linear search
                isAccountFound = false;

            if (wxNOT_FOUND == accountArray_->Index(core_->accountList_.GetAccountName(toAccountID))) //linear search
                isToAccountFound = false;
        }

        if (!isAccountFound && !isToAccountFound) continue; // skip account

        double convRate = core_->accountList_.getAccountBaseCurrencyConvRate(accountID);
        double toConvRate = core_->accountList_.getAccountBaseCurrencyConvRate(toAccountID);

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
            else if (repeats == 11) // repeat in numRepeats Days (Once only)
            {
                if (numRepeats > 0)
                {
                    nextOccurDate = nextOccurDate.Add(wxDateSpan::Days(numRepeats));
                    numRepeats = -1;
                }
                else break;
            }
            else if (repeats == 12) // repeat in numRepeats Months (Once only)
            {
                if (numRepeats > 0)
                {
                    nextOccurDate = nextOccurDate.Add(wxDateSpan::Months(numRepeats));
                    numRepeats = -1;
                }
                else break;
            }
            else if (repeats == 13) // repeat every numRepeats Days
            {
                if (numRepeats > 0)
                {
                    nextOccurDate = nextOccurDate.Add(wxDateSpan::Days(numRepeats));
                }
                else break;
            }
            else if (repeats == 14) // repeat every numRepeats Months
            {
                if (numRepeats > 0)
                {
                    nextOccurDate = nextOccurDate.Add(wxDateSpan::Months(numRepeats));
                }
                else break;
            }
            else if ((repeats == 15) || (repeats == 16))
            {
                nextOccurDate = nextOccurDate.Add(wxDateSpan::Month());
                nextOccurDate = nextOccurDate.SetToLastMonthDay(nextOccurDate.GetMonth(),nextOccurDate.GetYear());
                if (repeats == 16) // last weekday of month
                {
                    if (nextOccurDate.GetWeekDay() == wxDateTime::Sun || nextOccurDate.GetWeekDay() == wxDateTime::Sat)
                        nextOccurDate.SetToPrevWeekDay(wxDateTime::Fri);
                }
            }
            else break;
        } // end while
    } //end query
    q1.Finalize();

    // Now we have a vector of dates and amounts over next year
    int fcstsz = -1;
    std::vector<double> forecastOver12Months;
    if (cashflowreporttype_ == 0)
    {
        fcstsz = 12 * years;
    }
    else if (cashflowreporttype_ == 1)
    {
        fcstsz = years * 366;
    }
    wxASSERT(fcstsz != -1);
    forecastOver12Months.resize(fcstsz, 0.0);;

    for (int idx = 0; idx < (int)forecastOver12Months.size(); idx++)
    {
        wxDateTime dtBegin = wxDateTime::Now();
        wxDateTime dtEnd;
        if (cashflowreporttype_ == 0)
        {
            dtEnd   = wxDateTime::Now().Add(wxDateSpan::Months(idx));
        }
        else if (cashflowreporttype_ == 1)
        {
            dtEnd   = wxDateTime::Now().Add(wxDateSpan::Days(idx));
        }

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

    core_->currencyList_.LoadBaseCurrencySettings();

    bool initialMonths = true;
    int displayYear    = wxDateTime::Now().GetYear();

    for (int idx = 0; idx < (int)forecastOver12Months.size(); idx++)
    {
        wxDateTime dtEnd;
        if (cashflowreporttype_ == 0)
        {
            dtEnd   = wxDateTime::Now().Add(wxDateSpan::Months(idx));
        }
        else if (cashflowreporttype_ == 1)
        {
            dtEnd   = wxDateTime::Now().Add(wxDateSpan::Days(idx));
        }
           
        wxString balanceStr;
        double balance = forecastOver12Months[idx] + tInitialBalance;
        mmex::formatDoubleToCurrency(balance, balanceStr);
        wxString diffStr;
        double diff;
        diff = (idx==0 ? 0 : forecastOver12Months[idx] - forecastOver12Months[idx-1]) ;
        mmex::formatDoubleToCurrency(diff, diffStr);

        bool addSeparator = false;
        bool addSeparatorAfter = false;

        if (frame_->budgetFinancialYears())
        {
            if (initialMonths && (dtEnd.GetMonth() == getUserDefinedFinancialYear().GetMonth()))
            {
                addSeparator  = true;
                initialMonths = false;
            }
            else if ((dtEnd.GetMonth() == getUserDefinedFinancialYear().GetMonth()) && (displayYear != dtEnd.GetYear()))
            {
                addSeparator = true;
                displayYear  = dtEnd.GetYear();
            }
        }
        else if (displayYear != dtEnd.GetYear())
        {
            addSeparator = true;
            displayYear  = dtEnd.GetYear();
        }

        if (addSeparator) hb.addRowSeparator(3);

        wxString dtStr ; 
        //dtStr << mmGetNiceShortMonthName(dtEnd.GetMonth()) << wxT(" ") << dtEnd.GetYear();
        dtStr << mmGetDateForDisplay(dtEnd); 

        hb.startTableRow();
        hb.addTableCell(dtStr, false, true);
        hb.addTableCell(balanceStr, true, true, true, ((balance < 0) ? wxT("RED") : wxT("BLACK")));
        hb.addTableCell((idx==0 ? wxT ("") : diffStr), true, true, true, (diff < 0 ? wxT("RED") : wxT("BLACK"))) ;
        hb.endTableRow();
        
        // Add a separator for each month in daily cash flow report
        if (cashflowreporttype_ == 1)
        {
            wxDateTime dtLMD;// Last day of the month
            dtLMD = dtLMD.SetToLastMonthDay(dtEnd.GetMonth(),dtEnd.GetYear());
            
            if (dtEnd.IsSameDate(dtLMD))
                addSeparatorAfter = true;
        }
        if (addSeparatorAfter) hb.addRowSeparator(3);
    }

    hb.addRowSeparator(3);
    hb.endTable();
    hb.endCenter();
    hb.end();

    return hb.getHTMLText();
}
