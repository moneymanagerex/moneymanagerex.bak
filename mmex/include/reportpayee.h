#ifndef _MM_EX_REPORTPAYEE_H_
#define _MM_EX_REPORTPAYEE_H_

#include "defs.h"
#include "htmlbuilder.h"
#include "reportbase.h"
#include "util.h"
#include "dbwrapper.h"

class mmReportPayeeExpenses : public mmPrintableBase 
{
public:
    mmReportPayeeExpenses(wxSQLite3Database* db, bool ignoreDate, 
        wxDateTime dtBegin, 
        wxDateTime dtEnd) 
        : db_(db),
          ignoreDate_(ignoreDate),
          dtBegin_(dtBegin),
          dtEnd_(dtEnd)
    {
    }

    virtual wxString getHTMLText()
    {
        mmHTMLBuilder hb;
        hb.init();
        hb.addHeader(3, _("To Whom the Money Goes"));

        wxDateTime now = wxDateTime::Now();
        wxString dt = wxT("Today's Date: ") + mmGetNiceDateString(now);
        hb.addHeader(7, dt);
        hb.addLineBreak();
        hb.addLineBreak();

        if (!ignoreDate_)
        {
            wxString dtRange = wxT("From: ") 
                + mmGetNiceDateSimpleString(dtBegin_.Add(wxDateSpan::Day())) + wxT(" To: ") +
                mmGetNiceDateSimpleString(dtEnd_);
            hb.addHeader(7, dtRange);
            hb.addLineBreak();
        }

        hb.addLineBreak();

        hb.beginTable();
        std::vector<wxString> headerR;
        headerR.push_back(wxT("Payee    "));
        headerR.push_back(wxT("Amount   "));
        hb.addTableHeaderRow(headerR, wxT(" bgcolor=\"#80B9E8\""));

        mmDBWrapper::loadBaseCurrencySettings(db_);
        mmBEGINSQL_LITE_EXCEPTION;

        wxSQLite3StatementBuffer bufSQL;
        bufSQL.Format("select * from PAYEE_V1 order by PAYEENAME;");
        wxSQLite3ResultSet q1 = db_->ExecuteQuery(bufSQL);
        while (q1.NextRow())
        {
            int payeeID          = q1.GetInt(wxT("PAYEEID"));
            wxString payeeString = q1.GetString(wxT("PAYEENAME"));
            wxString balance;
            double amt = mmDBWrapper::getAmountForPayee(db_, payeeID, ignoreDate_, 
                dtBegin_, dtEnd_);
            mmCurrencyFormatter::formatDoubleToCurrency(amt, balance);

            if (amt != 0.0)
            {
                std::vector<wxString> data;
                data.push_back(payeeString);
              
                data.push_back(balance);
                hb.addRow(data);
            }
            
        }
        q1.Finalize();
        mmENDSQL_LITE_EXCEPTION;
        hb.endTable();

        hb.end();
        return hb.getHTMLText();

    }

private:
    wxSQLite3Database* db_;
    wxDateTime dtBegin_;
    wxDateTime dtEnd_;
    bool ignoreDate_;
};

#endif
