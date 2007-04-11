/*******************************************************
 Copyright (C) 2006 Madhan Kanagavel

 This program is free software; you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation; either version 2 of the License, or
 (at your option) any later version.
 
 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.
 
 You should have received a copy of the GNU General Public License
 along with this program; if not, write to the Free Software
 Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 /*******************************************************/
#ifndef _MM_EX_REPORTSUMMARY_H_
#define _MM_EX_REPORTSUMMARY_H_

#include "defs.h"
#include "htmlbuilder.h"
#include "reportbase.h"
#include "util.h"
#include "dbwrapper.h"

class mmReportSummary : public mmPrintableBase 
{
public:
    mmReportSummary(mmCoreDB* core) : core_(core), db_(core_->db_.get()) {}

    virtual wxString getHTMLText()
    {
        mmHTMLBuilder hb;
        hb.init();
        hb.addHeader(3, _("Summary of Accounts"));

        wxDateTime now = wxDateTime::Now();
        wxString dt(wxDateTime::GetWeekDayName(now.GetWeekDay()) + wxString(wxT(", ")));
        dt += wxDateTime::GetMonthName(now.GetMonth()) + wxString(wxT(" "));
        dt += wxString::Format(wxT("%d"), now.GetDay()) + wxT(", ") 
            + wxString::Format(wxT("%d"), now.GetYear());
        hb.addHeader(7, dt);
        hb.addLineBreak();
        hb.addLineBreak();
        hb.addLineBreak();

        mmDBWrapper::loadBaseCurrencySettings(db_);
        int ct = 0;  
        double tincome = 0.0;
        double texpenses = 0.0;
        double tBalance = 0.0;

        hb.beginTable();

        std::vector<wxString> headerR;
        headerR.push_back(_("Account Name  "));
        headerR.push_back(_("Balance   "));
        hb.addTableHeaderRow(headerR, wxT(" bgcolor=\"#80B9E8\""));

        for (int iAdx = 0; iAdx < (int) core_->accountList_.accounts_.size(); iAdx++)
        {
           mmCheckingAccount* pCA 
              = dynamic_cast<mmCheckingAccount*>(core_->accountList_.accounts_[iAdx].get());
           if (pCA)
           {
              double bal = pCA->initialBalance_ 
                  + core_->bTransactionList_.getBalance(pCA->accountID_);
              
              boost::shared_ptr<mmCurrency> pCurrencyPtr = core_->accountList_.getCurrencyWeakPtr(pCA->accountID_).lock();
              wxASSERT(pCurrencyPtr);
              mmCurrencyFormatter::loadSettings(pCurrencyPtr);
              double rate = pCurrencyPtr->baseConv_;

              tBalance += bal * rate;;

              wxString balance;
              mmCurrencyFormatter::formatDoubleToCurrency(bal, balance);

              hb.addHTML(wxT("<tr><td>")); 
              hb.addHTML(pCA->accountName_);
              hb.addHTML(wxT("</td><td align=\"right\">"));
              hb.addHTML(balance);
              hb.addHTML(wxT("</td></tr>"));
           }
        }
        hb.endTable();
        

        /* Stocks */
        double stockBalance = mmDBWrapper::getStockInvestmentBalance(db_);
        wxString stockBalanceStr;
        mmDBWrapper::loadBaseCurrencySettings(db_);
        mmCurrencyFormatter::formatDoubleToCurrency(stockBalance, stockBalanceStr);
        hb.addLineBreak();
        hb.addLineBreak();
        wxString dispStr = _("Stock Investments :") + stockBalanceStr; 
        hb.addHeader(7, dispStr);
        hb.addLineBreak();

        /* Assets */
        double assetBalance = mmDBWrapper::getAssetBalance(db_);
        wxString assetBalanceStr;
        mmDBWrapper::loadBaseCurrencySettings(db_);
        mmCurrencyFormatter::formatDoubleToCurrency(assetBalance, assetBalanceStr);
        dispStr = _("Assets :") + assetBalanceStr; 
        hb.addHeader(7, dispStr);
        hb.addLineBreak();


        tBalance += stockBalance;
        tBalance += assetBalance;
        wxString tBalanceStr;
        mmDBWrapper::loadBaseCurrencySettings(db_);
        mmCurrencyFormatter::formatDoubleToCurrency(tBalance, tBalanceStr);
        dispStr = _("Total Balance on all Accounts :") + tBalanceStr; 
        hb.addHeader(7, dispStr);

        hb.end();
        return hb.getHTMLText();

    }

private:
    mmCoreDB* core_;
    wxSQLite3Database* db_;
};

#endif
