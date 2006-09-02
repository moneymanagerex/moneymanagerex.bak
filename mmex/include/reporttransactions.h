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

#ifndef _MM_EX_REPORTTRANSACT_H_
#define _MM_EX_REPORTTRANSACT_H_

#include "defs.h"
#include "htmlbuilder.h"
#include "reportbase.h"
#include "util.h"
#include "dbwrapper.h"
#include "mmcheckingpanel.h"
#include <algorithm>
#include <vector>



class mmReportTransactions : public mmPrintableBase 
{
public:
    mmReportTransactions(std::vector<mmTransactionHolder*>* trans, wxSQLite3Database* db) 
        : trans_(trans), db_(db)
    {
    }

    ~mmReportTransactions()
    {
        std::vector<mmTransactionHolder*>& refTrans = *trans_;
        for (unsigned int index = 0; index < refTrans.size(); index++)
        {
            delete refTrans[index];
        }

        delete trans_;
    }

    virtual wxString getHTMLText()
    {
        mmHTMLBuilder hb;
        hb.init();
        hb.addHeader(3, _("Transaction List"));

        wxDateTime now = wxDateTime::Now();
        wxString dt = _("Today's Date: ") + mmGetNiceDateString(now);
        hb.addHeader(7, dt);
        hb.addLineBreak();
        hb.addLineBreak();
     
        hb.beginTable();
        std::vector<wxString> headerR;
        headerR.push_back(_("Date  "));
        headerR.push_back(_("Account"));
        headerR.push_back(_("Payee  "));
        headerR.push_back(_("Type   "));
        headerR.push_back(_("Status"));
        headerR.push_back(_("Amount   "));
        hb.addTableHeaderRow(headerR, wxT(" bgcolor=\"#80B9E8\""));

        double total = 0;
        for (unsigned int index = 0; index < trans_->size(); index++)
        {
            std::vector<wxString> data;
            std::vector<mmTransactionHolder*>& refTrans = *trans_;

            data.push_back(refTrans[index]->dateStr_);
            data.push_back(refTrans[index]->fromAccountStr_);
            data.push_back(refTrans[index]->payeeStr_);
            data.push_back(refTrans[index]->transType_);
            data.push_back(refTrans[index]->status_);
            data.push_back(refTrans[index]->depositStr_);
            double dbRate = mmDBWrapper::getCurrencyBaseConvRate(db_, refTrans[index]->accountID_);
            double transAmount = refTrans[index]->amt_ * dbRate;

            if (refTrans[index]->transType_ == wxT("Deposit"))
            {
                total += transAmount;
            }
            else if (refTrans[index]->transType_ == wxT("Withdrawal"))
            {
                total -= transAmount;
            }
            hb.addRow(data);
        }
        hb.endTable();

        wxString balanceStr;
        mmDBWrapper::loadBaseCurrencySettings(db_);
        mmCurrencyFormatter::formatDoubleToCurrency(total, balanceStr);
        dt = _("Total Amount: ") + balanceStr;

        hb.addLineBreak();
        hb.addLineBreak();

        hb.addHeader(7, dt);

        hb.end();
        return hb.getHTMLText();

    }

private:
    std::vector<mmTransactionHolder*>* trans_;
     wxSQLite3Database* db_;
    wxDateTime dtBegin_;
    wxDateTime dtEnd_;
    bool ignoreDate_;
};

#endif
