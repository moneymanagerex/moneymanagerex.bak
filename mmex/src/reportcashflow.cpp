#include "reportcashflow.h"

#include "defs.h"
#include "htmlbuilder.h"
#include "reportbase.h"
#include "util.h"
#include "dbwrapper.h"
#include "mmcoredb.h"

mmReportCashFlow::mmReportCashFlow(mmCoreDB* core, const wxArrayString* accountArray) : 
    core_(core), 
    accountArray_(accountArray)
{
    activeTermAccounts_  = false;
    termAccountsHeading_ = false;
}

void mmReportCashFlow::activateTermAccounts() 
{
    activeTermAccounts_ = true;
}

void mmReportCashFlow::showTermAccountsHeading()
{
    termAccountsHeading_ = true;
}

wxString mmReportCashFlow::getHTMLText()
{
    core_->currencyList_.loadBaseCurrencySettings();

    mmHTMLBuilder hb;
    hb.init();

    wxString headerMsg = _("Cash Flow Forecast A Year Ahead - For ");
    if (accountArray_ == NULL) 
    {
        if ( !(termAccountsHeading_ == activeTermAccounts_ ) &&
              (termAccountsHeading_ || activeTermAccounts_) )
            headerMsg = headerMsg + _("All Bank Accounts");
        else 
            headerMsg = headerMsg + _("All Accounts");

        hb.addHeader(3, headerMsg );
    } else {
        int arrIdx = 0;
        wxString msgString = wxT(": ");
 
        if ( (int)accountArray_->size() == 0 )
            msgString = msgString + wxT("?");
        else if ( (int)accountArray_->size() > 1 )
            msgString = wxT("s") + msgString;

        if ( (int)accountArray_->size() > 0 )
        {
            msgString = msgString + accountArray_->Item(arrIdx);
            arrIdx ++;
        }
        while ( arrIdx < (int)accountArray_->size() )
        {
            msgString = msgString + wxT(", ") + accountArray_->Item(arrIdx);
            arrIdx ++;
        }

        hb.addHeader(3, headerMsg + _("Account" + msgString));
    }

    wxDateTime now = wxDateTime::Now();
    wxString dt = _("Today's Date: ") + mmGetNiceDateString(now);
    hb.addHeader(7, dt);
    hb.addLineBreak();
    hb.addLineBreak();

	hb.startCenter();

    hb.startTable(wxT("50%"));
	hb.startTableRow();
	hb.addTableHeaderCell(_("Date"));
	hb.addTableHeaderCell(_("Total"));
	hb.endTableRow();

    double tBalance = 0.0;
          
    for (int iAdx = 0; iAdx < (int) core_->accountList_.accounts_.size(); iAdx++)
    {
        mmCheckingAccount* pCA = dynamic_cast<mmCheckingAccount*>(core_->accountList_.accounts_[iAdx].get());
        if (pCA)
        {
            // Check if this account belongs in our list
            if (accountArray_ != NULL)
            {
                bool isFound = false;
                for (int arrIdx = 0; arrIdx < (int)accountArray_->size(); arrIdx++)
                {
                    if (pCA->accountName_ == accountArray_->Item(arrIdx))
                    {
                        isFound = true;
                        break;
                    }
                }
                if (!isFound)
                    continue; // skip account
            }

            double bal = pCA->initialBalance_ + core_->bTransactionList_.getBalance(pCA->accountID_);
            boost::shared_ptr<mmCurrency> pCurrencyPtr = core_->accountList_.getCurrencyWeakPtr(pCA->accountID_).lock();
            wxASSERT(pCurrencyPtr);
            mmex::CurrencyFormatter::instance().loadSettings(*pCurrencyPtr);
            double rate = pCurrencyPtr->baseConv_;

            tBalance += bal * rate;
        }

        if (activeTermAccounts_) // Add Term accounts to cashflows as well
        {
            mmTermAccount* pTA = dynamic_cast<mmTermAccount*>(core_->accountList_.accounts_[iAdx].get());
            if (pTA)
            {
                // Check if this account belongs in our list
                if (accountArray_ != NULL)
                {
                    bool isFound = false;
                    for (int arrIdx = 0; arrIdx < (int)accountArray_->size(); arrIdx++)
                    {
                        if (pTA->accountName_ == accountArray_->Item(arrIdx))
                        {
                            isFound = true;
                            break;
                        }
                    }
                    if (!isFound)
                        continue; // skip account
                }

                double bal = pTA->initialBalance_ + core_->bTransactionList_.getBalance(pTA->accountID_);
                boost::shared_ptr<mmCurrency> pCurrencyPtr = core_->accountList_.getCurrencyWeakPtr(pTA->accountID_).lock();
                wxASSERT(pCurrencyPtr);
                mmex::CurrencyFormatter::instance().loadSettings(*pCurrencyPtr);
                double rate = pCurrencyPtr->baseConv_;

                tBalance += bal * rate;
            }
        }
    }

    // We now know the total balance on the account
    // Start by walking through the repeating transaction list

    static const char sql[] = 
    "select NEXTOCCURRENCEDATE, "
            "REPEATS, "
            "NUMOCCURRENCES, "
            "TRANSCODE, "
            "TRANSAMOUNT, "
            "ACCOUNTID "
    "from BILLSDEPOSITS_V1";

    wxDateTime yearFromNow = wxDateTime::Now().Add(wxDateSpan::Year());
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

        if (transType == wxT("Transfer"))
            continue;

        int accountID     = q1.GetInt(wxT("ACCOUNTID"));
        if (accountArray_ != NULL)
        {
            bool isFound = false;
            for (int arrIdx = 0; arrIdx < (int)accountArray_->size(); arrIdx++)
            {
                if (accountID == core_->accountList_.getAccountID(accountArray_->Item(arrIdx)))
                {
                    isFound = true;
                    break;
                }
            }
            if (!isFound)
                continue; // skip account
        }


        double convRate = mmDBWrapper::getCurrencyBaseConvRate(core_->db_.get(), accountID);

        // Process all possible repeating transactions for this BD
        while(1)
        {
            if (nextOccurDate > yearFromNow)
                break;

            if (processNumRepeats)
                numRepeats--;

            mmRepeatForecast rf;
            rf.date = nextOccurDate;
            wxString dtStr = mmGetDateForDisplay(core_->db_.get(), nextOccurDate);

            if (transType == wxT("Withdrawal"))
                rf.amount = -amt * convRate;
            else
                rf.amount = +amt * convRate;

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
    }
    q1.Finalize();

    // Now we have a vector of dates and amounts over next year
    // Need to use different month ranges to figure out cash flow

    std::vector<double> forecastOver12Months;
    forecastOver12Months.resize(12, 0.0);
    for (int idx = 0; idx < 12; idx++)
    {
        wxDateTime dtBegin = wxDateTime::Now();
        wxDateTime dtEnd   = wxDateTime::Now().Add(wxDateSpan::Months(idx+1));

        for (int fcIdx = 0; fcIdx < (int)fvec.size(); fcIdx++)
        {
            if (!fvec[fcIdx].date.IsBetween(dtBegin, dtEnd))
                continue;
            forecastOver12Months[idx] += fvec[fcIdx].amount;
        }
    }

    core_->currencyList_.loadBaseCurrencySettings();

    for (int idx = 0; idx < (int)forecastOver12Months.size(); idx++)
    {
        wxDateTime dtEnd   = wxDateTime::Now().Add(wxDateSpan::Months(idx+1));
           
        wxString balance;
        mmex::formatDoubleToCurrency(forecastOver12Months[idx] + tBalance, balance);

        wxString dtStr = mmGetDateForDisplay(core_->db_.get(), dtEnd);

		hb.startTableRow();
		hb.addTableCell(dtStr, false, true);
		if(forecastOver12Months[idx] + tBalance < 0)
		{
			hb.addTableCell(balance, true, true, true, wxT("#ff0000"));
		}
		else
		{
			hb.addTableCell(balance, true, false, true);
		}
		hb.endTableRow();
    }

    hb.endTable();
    hb.endCenter();
    hb.end();

    return hb.getHTMLText();
}
