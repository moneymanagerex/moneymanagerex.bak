#ifndef _MM_EX_REPORTCASHFLOW_H_
#define _MM_EX_REPORTCASHFLOW_H_

#include "defs.h"
#include "htmlbuilder.h"
#include "reportbase.h"
#include "util.h"
#include "dbwrapper.h"

class mmReportCashFlow : public mmPrintableBase 
{
public:
    mmReportCashFlow(mmCoreDB* core, const wxArrayString* accountArray = NULL) 
        : core_(core), accountArray_(accountArray)
    {
    }

    virtual wxString getHTMLText()
    {
        core_->currencyList_.loadBaseCurrencySettings();

        mmHTMLBuilder hb;
        hb.init();
        hb.addHeader(3, _("Cash Flow Forecast For All Accounts - A Year Ahead"));

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
           mmCheckingAccount* pCA 
              = dynamic_cast<mmCheckingAccount*>(core_->accountList_.accounts_[iAdx].get());
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

              double bal = pCA->initialBalance_ 
                  + core_->bTransactionList_.getBalance(pCA->accountID_);
              
              boost::shared_ptr<mmCurrency> pCurrencyPtr 
                 = core_->accountList_.getCurrencyWeakPtr(pCA->accountID_).lock();
              wxASSERT(pCurrencyPtr);
              mmCurrencyFormatter::loadSettings(pCurrencyPtr);
              double rate = pCurrencyPtr->baseConv_;

              tBalance += bal * rate;;

           }
        }

        // We now know the total balance on the account
        // Start by walking through the repeating transaction list

        wxSQLite3StatementBuffer bufSQL;
        bufSQL.Format("select * from BILLSDEPOSITS_V1;");
        wxSQLite3ResultSet q1 = core_->db_.get()->ExecuteQuery(bufSQL);

        wxDateTime yearFromNow = wxDateTime::Now().Add(wxDateSpan::Year());

        forecastVec fvec;
        while (q1.NextRow())
        {
           int bdID                = q1.GetInt(wxT("BDID"));
        
           wxString nextOccurrString = q1.GetString(wxT("NEXTOCCURRENCEDATE"));
           wxDateTime nextOccurDate = mmGetStorageStringAsDate(nextOccurrString);
           
           int repeats             = q1.GetInt(wxT("REPEATS"));
           int numRepeats          = q1.GetInt(wxT("NUMOCCURRENCES"));
           wxString transType      = q1.GetString(wxT("TRANSCODE"));
           double amt              = q1.GetDouble(wxT("TRANSAMOUNT"));
           double toAmt            = q1.GetDouble(wxT("TOTRANSAMOUNT"));

           bool processNumRepeats = false;
           if (numRepeats != -1)
              processNumRepeats = true;

           if (repeats == 0)
              continue;

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
           mmCurrencyFormatter::formatDoubleToCurrency(forecastOver12Months[idx] + tBalance, balance);

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

private:
   struct mmRepeatForecast
   {
      wxDateTime date;
      double amount;
   };
   mmCoreDB* core_;
   typedef std::vector<mmRepeatForecast> forecastVec;
   std::vector< forecastVec > bdForecastVec;
   const wxArrayString* accountArray_;
};

#endif
