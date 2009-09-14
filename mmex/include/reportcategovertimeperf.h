#ifndef _MM_EX_REPORTCATEGOVERTIME_H_
#define _MM_EX_REPORTCATEGOVERTIME_H_

#include "defs.h"
#include "htmlbuilder.h"
#include "reportbase.h"
#include "util.h"
#include "dbwrapper.h"

class mmReportCategoryOverTimePerformance : public mmPrintableBase 
{
public:
    mmReportCategoryOverTimePerformance(mmCoreDB* core) 
        : core_(core),
          db_(core_->db_.get())
    {
    }

    virtual wxString getHTMLText()
    {
        mmHTMLBuilder hb;
        hb.init();
        hb.addHeader(3, _("Category Income/Expenses Over Last 12 Months "));

        wxDateTime now = wxDateTime::Now();
        wxString dt = _("Today's Date: ") + mmGetNiceDateString(now);
        hb.addHeader(7, dt);
        hb.addLineBreak();
        hb.addLineBreak();

        double actIncome   = 0.0;
        double actExpenses = 0.0;

        wxDateTime tempNow = now;
        wxDateTime prevYearBegin = tempNow.Subtract(wxDateSpan::Year());

        wxDateTime yearBegin(1, prevYearBegin.GetMonth(), prevYearBegin.GetYear());
        wxDateTime yearEnd = now;

		hb.startCenter();
        hb.startTable();

		hb.startTableRow();
		hb.addTableHeaderCell(_("Category"));

        for (int yidx = 1; yidx <= 12; yidx++)
        {
           wxDateTime tempYearBegin = yearBegin;
           wxDateTime dtBegin = tempYearBegin.Add(wxDateSpan::Months(yidx));
           wxDateTime dtEnd = dtBegin.GetLastMonthDay((wxDateTime::Month) dtBegin.GetMonth(), dtBegin.GetYear());
           wxString yearStr = wxString::Format(wxT("%d"), dtBegin.GetYear());
		   hb.addTableHeaderCell(mmGetNiceShortMonthName(dtBegin.GetMonth()) + wxT(" ") + yearStr);
        }
		
        hb.addTableHeaderCell(_("Overall"));
		hb.endTableRow();

        static const char sql[] = 
        "select CATEGID, CATEGNAME "
        "from CATEGORY_V1 "
        "order by CATEGNAME";

        static const char sql_sub[] = 
        "select SUBCATEGID, SUBCATEGNAME "
        "from SUBCATEGORY_V1 "
        "where CATEGID = ?";

        mmBEGINSQL_LITE_EXCEPTION;
        mmDBWrapper::loadBaseCurrencySettings(db_);

        wxSQLite3Statement st = db_->PrepareStatement(sql_sub);
        wxSQLite3ResultSet q1 = db_->ExecuteQuery(sql);

        while (q1.NextRow())
        {
            mmBudgetEntryHolder th;
            th.categID_ = q1.GetInt(wxT("CATEGID"));
            th.catStr_ = q1.GetString(wxT("CATEGNAME"));
            th.subcategID_ = -1;
            th.subCatStr_ = wxT("");
            th.amt_ = 0.0;
            th.period_ = wxT("None");
            th.actualStr_ = wxT("0.0");
            th.actual_ = 0.0;

            th.actual_ = core_->bTransactionList_.getAmountForCategory(th.categID_, th.subcategID_, 
                false,  yearBegin, yearEnd);
            mmCurrencyFormatter::formatDoubleToCurrencyEdit(th.actual_, th.actualStr_);

            if (th.actual_ < 0)
                actExpenses += th.actual_;
            else
                actIncome += th.actual_;

            wxString displayAmtString;
            if (mmCurrencyFormatter::formatDoubleToCurrencyEdit(th.amt_, displayAmtString))
                th.amtString_ = displayAmtString;

            if (th.actual_ != 0.0)
            {
				hb.startTableRow();
				hb.addTableCell(th.catStr_, false, true);

                for (int yidx = 1; yidx <= 12; yidx++)
                {
                   wxDateTime tempYearBegin = yearBegin;
                   wxDateTime dtBegin = tempYearBegin.Add(wxDateSpan::Months(yidx));
                   wxDateTime dtEnd = dtBegin.GetLastMonthDay((wxDateTime::Month) dtBegin.GetMonth(), dtBegin.GetYear());
                   
                    double actualMonthVal = core_->bTransactionList_.getAmountForCategory(
                                 th.categID_, th.subcategID_, false,  dtBegin, dtEnd);
                    wxString actualMonthValStr;
                    mmCurrencyFormatter::formatDoubleToCurrencyEdit(actualMonthVal, actualMonthValStr);
					hb.addTableCell(actualMonthValStr, true);
                }
             
                // year end
				hb.addTableCell(th.actualStr_, true);
				hb.endTableRow();
            }

            st.Bind(1, th.categID_);
            wxSQLite3ResultSet q2 = st.ExecuteQuery(); 

            while(q2.NextRow())
            {
                mmBudgetEntryHolder thsub;
                thsub.categID_ = th.categID_;
                thsub.catStr_ = th.catStr_;
                thsub.subcategID_ = q2.GetInt(wxT("SUBCATEGID"));
                thsub.subCatStr_   = q2.GetString(wxT("SUBCATEGNAME"));
                thsub.amt_ = 0.0;
                thsub.actualStr_ = wxT("0.0");
                thsub.actual_ = 0.0;
                thsub.actual_ = core_->bTransactionList_.getAmountForCategory(thsub.categID_, thsub.subcategID_, 
                    false,  yearBegin, yearEnd);
                mmCurrencyFormatter::formatDoubleToCurrencyEdit(thsub.actual_, thsub.actualStr_);
                if (thsub.actual_ < 0)
                    actExpenses += thsub.actual_;
                else
                    actIncome += thsub.actual_;


                if (mmCurrencyFormatter::formatDoubleToCurrencyEdit(thsub.amt_, displayAmtString))
                    thsub.amtString_ = displayAmtString;

                if (thsub.actual_ != 0.0)
                {
					hb.startTableRow();
					hb.addTableCell(thsub.catStr_+ wxT(": ") + thsub.subCatStr_, false, true);

                    for (int yidx = 1; yidx <= 12; yidx++)
                    {
                       wxDateTime tempYearBegin = yearBegin;
                       wxDateTime dtBegin = tempYearBegin.Add(wxDateSpan::Months(yidx));
                       wxDateTime dtEnd = dtBegin.GetLastMonthDay((wxDateTime::Month) dtBegin.GetMonth(), dtBegin.GetYear());

                        double actualMonthVal 
                           = core_->bTransactionList_.getAmountForCategory(thsub.categID_, thsub.subcategID_, 
                            false,  dtBegin, dtEnd);
                        wxString actualMonthValStr;
                        mmCurrencyFormatter::formatDoubleToCurrencyEdit(actualMonthVal, actualMonthValStr);
						hb.addTableCell(actualMonthValStr, true);
                    }

					hb.addTableCell(thsub.actualStr_, true);
                    hb.endTableRow();
                } 

            }

            st.Reset();
        }

        q1.Finalize();
        st.Finalize();

        mmENDSQL_LITE_EXCEPTION;

        hb.endTable();
		hb.endCenter();

        hb.end();
        return hb.getHTMLText();
    }

private:
    mmCoreDB* core_;
    wxSQLite3Database* db_;
    int year_;
};

#endif
