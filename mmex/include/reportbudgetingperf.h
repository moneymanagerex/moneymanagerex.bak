#ifndef _MM_EX_REPORTBUDGETINGPERFORMANCE_H_
#define _MM_EX_REPORTBUDGETINGPERFORMANCE_H_

#include "defs.h"
#include "htmlbuilder.h"
#include "reportbase.h"
#include "util.h"
#include "dbwrapper.h"
#include "budgetingpanel.h"

class mmReportBudgetingPerformance : public mmPrintableBase 
{
public:
    mmReportBudgetingPerformance(wxSQLite3Database* db, int year) 
        : db_(db),
          year_(year)
    {
    }

    virtual wxString getHTMLText()
    {
        wxString yearStr = mmDBWrapper::getBudgetYearForID(db_, year_);

        mmHTMLBuilder hb;
        hb.init();
        hb.addHeader(3, _("Budget Performance for Year : ") + yearStr );

        wxDateTime now = wxDateTime::Now();
        wxString dt = wxT("Today's Date: ") + mmGetNiceDateString(now);
        hb.addHeader(7, dt);
        hb.addLineBreak();
        hb.addLineBreak();

        hb.addHTML(wxT("<font size=\"-2\">"));

        int budgetYearID_ =  year_;
        long year;
        yearStr.ToLong(&year);
        double estIncome = 0.0;
        double estExpenses = 0.0;
        double actIncome = 0.0;
        double actExpenses = 0.0;

        wxDateTime yearBegin(1, wxDateTime::Jan, year);
        wxDateTime yearEnd(31, wxDateTime::Dec, year);

        hb.beginTable();
        std::vector<wxString> headerR;
        headerR.push_back(wxT("Category"));
        headerR.push_back(wxT("Type"));
        headerR.push_back(wxT("Jan"));
        headerR.push_back(wxT("Feb"));
        headerR.push_back(wxT("March"));
        headerR.push_back(wxT("April"));
        headerR.push_back(wxT("May"));
        headerR.push_back(wxT("June"));
        headerR.push_back(wxT("July"));
        headerR.push_back(wxT("Aug"));
        headerR.push_back(wxT("Sep"));
        headerR.push_back(wxT("Oct"));
        headerR.push_back(wxT("Nov"));
        headerR.push_back(wxT("Dec"));
        headerR.push_back(wxT("Full Year"));
        hb.addTableHeaderRow(headerR, wxT(" bgcolor=\"#80B9E8\""));

        std::vector<wxString> data;

        mmBEGINSQL_LITE_EXCEPTION;
        mmDBWrapper::loadBaseCurrencySettings(db_);

        wxSQLite3StatementBuffer bufSQL;
        bufSQL.Format("select * from CATEGORY_V1 order by CATEGNAME;");
        wxSQLite3ResultSet q1 = db_->ExecuteQuery(bufSQL);
        while (q1.NextRow())
        {
            mmBudgetEntryHolder th;
            th.categID_ = q1.GetInt(wxT("CATEGID"));
            th.catStr_ = q1.GetString(wxT("CATEGNAME"));
            th.subcategID_ = -1;
            th.subCatStr_ = wxT("");
            th.amt_ = 0.0;
            th.period_ = wxT("None");
            th.estimatedStr_ = wxT("0.0");
            th.estimated_ = 0.0;
            th.actualStr_ = wxT("0.0");
            th.actual_ = 0.0;
            mmDBWrapper::getBudgetEntry(db_, budgetYearID_, th.categID_, th.subcategID_, th.period_, 
                th.amt_);

            double totalEstimated_ = 0.0;
            if (th.period_ == wxT("Monthly"))
            {
                th.estimated_ = th.amt_;
                totalEstimated_ = th.amt_ * 12.0;
            }
            else if (th.period_ == wxT("Yearly"))
            {
                th.estimated_ = th.amt_ / 12.0;
                totalEstimated_ = th.amt_;
            }
            else if (th.period_ == wxT("Weekly"))
            {
                th.estimated_ = th.amt_ * 4;
                totalEstimated_ = th.amt_ * 52;
            }
            else if (th.period_ == wxT("Bi-Weekly"))
            {
                th.estimated_ = th.amt_ * 2;
                totalEstimated_ = th.amt_ * 26;
            }
            else if (th.period_ == wxT("Bi-Monthly"))
            {
                th.estimated_ = th.amt_ / 2.0;
                totalEstimated_ = th.amt_ * 6;
            }
            else if (th.period_ == wxT("Quarterly"))
            {
                th.estimated_ = th.amt_ / 4.0;
                totalEstimated_ = th.amt_ * 4;
            }
            else if (th.period_ == wxT("Half-Yearly"))
            {
                th.estimated_ = th.amt_ / 6.0;
                totalEstimated_ = th.amt_ * 2;
            }
            else
                wxASSERT(true);

            if (totalEstimated_ < 0)
                estExpenses += totalEstimated_;
            else
                estIncome += totalEstimated_;

            mmCurrencyFormatter::formatDoubleToCurrencyEdit(th.estimated_, th.estimatedStr_);

            wxString totalEstimatedStr_;
            mmCurrencyFormatter::formatDoubleToCurrencyEdit(totalEstimated_, totalEstimatedStr_);

            th.actual_ = mmDBWrapper::getAmountForCategory(db_, th.categID_, th.subcategID_, 
                false,  yearBegin, yearEnd);
            mmCurrencyFormatter::formatDoubleToCurrencyEdit(th.actual_, th.actualStr_);

            if (th.actual_ < 0)
                actExpenses += th.actual_;
            else
                actIncome += th.actual_;

            wxString displayAmtString;
            if (mmCurrencyFormatter::formatDoubleToCurrencyEdit(th.amt_, displayAmtString))
                th.amtString_ = displayAmtString;


            // estimated stuff
            if ((totalEstimated_ != 0.0) || (th.actual_ != 0.0))
            {
                data.clear();
                data.push_back(th.catStr_);
                data.push_back(wxT("Estimated"));
                for (int yidx = 0; yidx < 12; yidx++)
                    data.push_back(th.estimatedStr_);
                data.push_back(totalEstimatedStr_);
                hb.addRow(data);

                // actual stuff
                data.clear();
                data.push_back(th.catStr_);
                data.push_back(wxT("Actual"));

                for (int yidx = 0; yidx < 12; yidx++)
                {
                    wxDateTime dtBegin(1, (wxDateTime::Month)yidx, year);
                    wxDateTime dtEnd = dtBegin.GetLastMonthDay((wxDateTime::Month)yidx, year);

                    double actualMonthVal = mmDBWrapper::getAmountForCategory(db_, th.categID_, th.subcategID_, 
                        false,  dtBegin, dtEnd);
                    wxString actualMonthValStr;
                    mmCurrencyFormatter::formatDoubleToCurrencyEdit(actualMonthVal, actualMonthValStr);
                    data.push_back(actualMonthValStr);
                }
                // year end
                data.push_back(th.actualStr_);
                hb.addRow(data,  wxT(" bgcolor=\"#DCEDD5\" "));
            }

            wxSQLite3StatementBuffer bufSQL1;
            bufSQL1.Format("select * from SUBCATEGORY_V1 where CATEGID=%d;", th.categID_);
            wxSQLite3ResultSet q2 = db_->ExecuteQuery(bufSQL1); 
            bool hasSubCateg = false;
            while(q2.NextRow())
            {
                mmBudgetEntryHolder thsub;
                thsub.categID_ = q1.GetInt(wxT("CATEGID"));
                thsub.catStr_ = q1.GetString(wxT("CATEGNAME"));
                thsub.subcategID_ = q2.GetInt(wxT("SUBCATEGID"));
                thsub.subCatStr_   = q2.GetString(wxT("SUBCATEGNAME"));
                thsub.amt_ = 0.0;
                thsub.period_ = wxT("None");
                thsub.estimatedStr_ = wxT("0.0");
                thsub.estimated_ = 0.0;
                thsub.actualStr_ = wxT("0.0");
                thsub.actual_ = 0.0;
                mmDBWrapper::getBudgetEntry(db_, budgetYearID_, thsub.categID_, thsub.subcategID_, thsub.period_, 
                    thsub.amt_);

                totalEstimated_ = 0.0;
                if (thsub.period_ == wxT("Monthly"))
                {
                    totalEstimated_ = thsub.amt_ * 12;
                    thsub.estimated_ = thsub.amt_;
                }
                else if (thsub.period_ == wxT("Yearly"))
                {
                    thsub.estimated_ = thsub.amt_ / 12.0;
                    totalEstimated_  = thsub.amt_;
                }
                else if (thsub.period_ == wxT("Weekly"))
                {
                    thsub.estimated_ = thsub.amt_ * 4;
                    totalEstimated_ = thsub.amt_ * 52;
                }
                else if (thsub.period_ == wxT("Bi-Weekly"))
                {
                    thsub.estimated_ = thsub.amt_ * 2;
                    totalEstimated_ = thsub.amt_ * 26;
                }
                else if (thsub.period_ == wxT("Bi-Monthly"))
                {
                    thsub.estimated_ = thsub.amt_ / 2.0;
                    totalEstimated_ = thsub.amt_ * 6;
                }
                else if (thsub.period_ == wxT("Quarterly"))
                {
                    thsub.estimated_ = thsub.amt_ / 4.0;
                    totalEstimated_ = thsub.amt_ * 4;
                }
                else if (thsub.period_ == wxT("Half-Yearly"))
                {
                    thsub.estimated_ = thsub.amt_ / 6.0;
                    totalEstimated_ = thsub.amt_ * 2;
                }
                else
                    wxASSERT(true);

                mmCurrencyFormatter::formatDoubleToCurrencyEdit(thsub.estimated_, thsub.estimatedStr_);
                if (thsub.estimated_ < 0)
                    estExpenses += totalEstimated_;
                else
                    estIncome += totalEstimated_;

                thsub.actual_ = mmDBWrapper::getAmountForCategory(db_, thsub.categID_, thsub.subcategID_, 
                    false,  yearBegin, yearEnd);
                mmCurrencyFormatter::formatDoubleToCurrencyEdit(thsub.actual_, thsub.actualStr_);
                if (thsub.actual_ < 0)
                    actExpenses += thsub.actual_;
                else
                    actIncome += thsub.actual_;


                if (mmCurrencyFormatter::formatDoubleToCurrencyEdit(thsub.amt_, displayAmtString))
                    thsub.amtString_ = displayAmtString;

                if ((totalEstimated_ != 0.0) || (th.actual_ != 0.0))
                {
                    data.clear();
                    wxString cn = thsub.catStr_+ wxT(" : ") + thsub.subCatStr_;
                    data.push_back(cn);
                    data.push_back(wxT("Estimated"));
                    for (int yidx = 0; yidx < 12; yidx++)
                        data.push_back(thsub.estimatedStr_);

                    mmCurrencyFormatter::formatDoubleToCurrencyEdit(totalEstimated_, totalEstimatedStr_);
                    data.push_back(totalEstimatedStr_);
                    hb.addRow(data);

                    data.clear();
                    cn = thsub.catStr_+ wxT(" : ") + thsub.subCatStr_;
                    data.push_back(cn);
                    data.push_back(wxT("Actual"));
                    for (int yidx = 0; yidx < 12; yidx++)
                    {
                        wxDateTime dtBegin(1, (wxDateTime::Month)yidx, year);
                        wxDateTime dtEnd = dtBegin.GetLastMonthDay((wxDateTime::Month)yidx, year);

                        double actualMonthVal = mmDBWrapper::getAmountForCategory(db_, thsub.categID_, thsub.subcategID_, 
                            false,  dtBegin, dtEnd);
                        wxString actualMonthValStr;
                        mmCurrencyFormatter::formatDoubleToCurrencyEdit(actualMonthVal, actualMonthValStr);
                        data.push_back(actualMonthValStr);
                    }


                    data.push_back( thsub.actualStr_);
                    hb.addRow(data, wxT(" bgcolor=\"#DCEDD5\" "));
                }

            }
            q2.Finalize();
        }
        q1.Finalize();

        mmENDSQL_LITE_EXCEPTION;

        hb.endTable();

        hb.addHTML(wxT("</font>"));

        hb.end();
        return hb.getHTMLText();
    }

private:
    wxSQLite3Database* db_;
    int year_;
};

#endif
