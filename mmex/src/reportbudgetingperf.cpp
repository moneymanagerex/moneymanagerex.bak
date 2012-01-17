#include "reportbudgetingperf.h"

#include "defs.h"
#include "htmlbuilder.h"
#include "reportbase.h"
#include "util.h"
#include "dbwrapper.h"
#include "mmcoredb.h"
#include "budgetingpanel.h"
#include "mmex.h"

mmReportBudgetingPerformance::mmReportBudgetingPerformance(mmCoreDB* core, mmGUIFrame* mainFrame, int budgetYearID)
    : mmReportBudget(mainFrame),
      core_(core), db_(core_->db_.get()),
      budgetYearID_(budgetYearID)
{
}

void mmReportBudgetingPerformance::DisplayEstimateMonths(mmHTMLBuilder& hb, mmBudgetEntryHolder& budgetEntry, int startMonth)
{
    int daysInMonth[] = {31,28,31,30,31,30,31,31,30,31,30,31};

    int month;
    for (int yidx = 0; yidx < 12; yidx++)
    {
        month = yidx + startMonth;
        if (month > 11)
        {
            month = month - 12;
        }
        // Set the estimate for each month
        wxString monthEstimateStr;
        long monthEstimate = budgetEntry.estimated_ * daysInMonth[month];
        mmex::formatDoubleToCurrencyEdit(monthEstimate, monthEstimateStr);
		hb.addTableCell(monthEstimateStr, true, true);
    }
}

void mmReportBudgetingPerformance::DisplayActualMonths(mmHTMLBuilder& hb, mmBudgetEntryHolder& budgetEntry, int startMonth, long startYear)
{
    bool evaluateTransfer = false;
    if (mainFrame_->budgetTransferTotal())
    {
        evaluateTransfer = true;
    }
    int month;
    for (int yidx = 0; yidx < 12; yidx++)
    {
        double currentStartYear = startYear;
        double currentEndYear = startYear;
        month = yidx + startMonth;
        if (month > 11)
        {
            month = month - 12;
            currentStartYear ++;
            currentEndYear++;
        }
        wxDateTime dtBegin(1, (wxDateTime::Month)month, currentStartYear);
        wxDateTime dtEnd = dtBegin.GetLastMonthDay((wxDateTime::Month)month, currentEndYear);
        bool transferAsDeposit = true;
        if (budgetEntry.amt_ < 0)
        {
            transferAsDeposit = false;
        }
        double actualMonthVal = core_->bTransactionList_.getAmountForCategory(budgetEntry.categID_, budgetEntry.subcategID_, false, dtBegin, dtEnd, evaluateTransfer, transferAsDeposit, mmIniOptions::ignoreFutureTransactions_);

        wxString actualMonthValStr;
        mmex::formatDoubleToCurrencyEdit(actualMonthVal, actualMonthValStr);

        if(actualMonthVal < budgetEntry.estimated_)
		{
            hb.addTableCell(actualMonthValStr, true, true, true, wxT("#ff0000"));
        }
        else
        {
            hb.addTableCell(actualMonthValStr, true);
        }
    }
}

wxString mmReportBudgetingPerformance::getHTMLText()
{
    int startDay = 1;
    int startMonth = wxDateTime::Jan;
    int endDay   = 31;
    int endMonth = wxDateTime::Dec;

    long startYear;
    wxString startYearStr = mmDBWrapper::getBudgetYearForID(db_, budgetYearID_);
    startYearStr.ToLong(&startYear);

    wxString headingStr = AdjustYearValues(startDay, startMonth, startYear, startYearStr);
    wxDateTime yearBegin(startDay, (wxDateTime::Month)startMonth, startYear);
    wxDateTime yearEnd(endDay, (wxDateTime::Month)endMonth, startYear);

    AdjustDateForEndFinancialYear(yearEnd);

    bool evaluateTransfer = false;
    if (mainFrame_->budgetTransferTotal())
    {
        evaluateTransfer = true;
    }
    mmHTMLBuilder hb;
    hb.init();
    hb.addHeader(3, _("Budget Performance for ") + headingStr );
    DisplayDateHeading(hb, yearBegin, yearEnd);

    double estIncome = 0.0;
    double estExpenses = 0.0;
    double actIncome = 0.0;
    double actExpenses = 0.0;

    hb.startCenter();

    hb.startTable();
	hb.startTableRow();
	hb.addTableHeaderCell(_("Category"));
	hb.addTableHeaderCell(_("Type"));

    int month;
    for (int i = 0; i < 12; i++)
    {
        month = i + startMonth;
        if (month > 11)
        {
            month = month - 12;
        }
        hb.addTableHeaderCell(mmGetNiceShortMonthName(month));
    }
	hb.addTableHeaderCell(_("Overall"));
    hb.addTableHeaderCell(_("%"));
	hb.endTableRow();

    static const char sql[] = 
    "select CATEGID, CATEGNAME "
    "from CATEGORY_V1 "
    "order by CATEGNAME";

    static const char sql_sub[] = 
    "select SUBCATEGID, SUBCATEGNAME "
    "from SUBCATEGORY_V1 "
    "where CATEGID = ? "
    "order by SUBCATEGNAME";

    mmDBWrapper::loadBaseCurrencySettings(db_);

    wxSQLite3Statement st = db_->PrepareStatement(sql_sub);
    wxSQLite3ResultSet q1 = db_->ExecuteQuery(sql);
        
    while (q1.NextRow())
    {
        mmBudgetEntryHolder th;
        initBudgetEntryFields(th);
        th.categID_ = q1.GetInt(wxT("CATEGID"));
        th.catStr_  = q1.GetString(wxT("CATEGNAME"));
        mmDBWrapper::getBudgetEntry(db_, budgetYearID_, th.categID_, th.subcategID_, th.period_, th.amt_);

        // Set the estimated amount for the year
        setBudgetDailyEstimateAmount(th, startMonth);
        double totalEstimated_ = th.estimated_ * 365;
        wxString totalEstimatedStr_;
        mmex::formatDoubleToCurrencyEdit(totalEstimated_, totalEstimatedStr_);

        // Set the overall estimated values
        if (totalEstimated_ < 0)
            estExpenses += totalEstimated_;
        else
            estIncome += totalEstimated_;

        // set the actual amount for the year
        bool transferAsDeposit = true;
        if (th.amt_ < 0)
        {
            transferAsDeposit = false;
        }
        th.actual_ = core_->bTransactionList_.getAmountForCategory(th.categID_, th.subcategID_, false,  yearBegin, yearEnd, evaluateTransfer, transferAsDeposit, mmIniOptions::ignoreFutureTransactions_);
        mmex::formatDoubleToCurrencyEdit(th.actual_, th.actualStr_);

        if (th.actual_ < 0)
            actExpenses += th.actual_;
        else
            actIncome += th.actual_;

        wxString displayAmtString;
        mmex::formatDoubleToCurrencyEdit(th.amt_, displayAmtString);
        th.amtString_ = displayAmtString;

        // estimated stuff
        if ((totalEstimated_ != 0.0) || (th.actual_ != 0.0))
        {
            hb.startTableRow();
			hb.addTableCell(th.catStr_, false, true);
			hb.addTableCell(_("Estimated"));
            
            DisplayEstimateMonths(hb, th, startMonth);
           
            hb.addTableCell(totalEstimatedStr_, true, true, true);
            hb.addTableCell(wxT("-"));
            hb.endTableRow();

            // actual stuff
            hb.startTableRow();
			hb.addTableCell(th.catStr_, false, true);
			hb.addTableCell(_("Actual"));

            DisplayActualMonths(hb, th, startMonth, startYear);

            // year end
			if(th.actual_ < totalEstimated_)
			{
				hb.addTableCell(th.actualStr_, true, true, true, wxT("#ff0000"));
			}
			else
			{
				hb.addTableCell(th.actualStr_, true, false, true);
			}

            if (((totalEstimated_ < 0) && (th.actual_ < 0)) ||
                ((totalEstimated_ > 0) && (th.actual_ > 0)))
            {
                double percent = (fabs(th.actual_) / fabs(totalEstimated_)) * 100.0;
                hb.addTableCell(wxString::Format(wxT("%.0f"), percent));
            }
            else
            {
                hb.addTableCell(wxT("-"));
            }
				
            hb.endTableRow();

			hb.addRowSeparator(16);
        }

        st.Bind(1, th.categID_);
        wxSQLite3ResultSet q2 = st.ExecuteQuery(); 

        while(q2.NextRow())
        {
            mmBudgetEntryHolder thsub;
            initBudgetEntryFields(thsub);
            thsub.categID_ = th.categID_;
            thsub.catStr_  = th.catStr_;
            thsub.subcategID_ = q2.GetInt(wxT("SUBCATEGID"));
            thsub.subCatStr_  = q2.GetString(wxT("SUBCATEGNAME"));

            mmDBWrapper::getBudgetEntry(db_, budgetYearID_, thsub.categID_, thsub.subcategID_, thsub.period_, thsub.amt_);
 
            // Set the estimated amount for the year
            setBudgetDailyEstimateAmount(thsub, startMonth);
            totalEstimated_ = thsub.estimated_ * 365;
            mmex::formatDoubleToCurrencyEdit(totalEstimated_, totalEstimatedStr_);

            // Set the overall estimated amount for the year
            if (totalEstimated_ < 0)
                estExpenses += totalEstimated_;
            else
                estIncome += totalEstimated_;

            // set the actual abount for the year
            transferAsDeposit = true;
            if (thsub.amt_ < 0)
            {
                transferAsDeposit = false;
            }
            thsub.actual_ = core_->bTransactionList_.getAmountForCategory(thsub.categID_, thsub.subcategID_, false,  yearBegin, yearEnd, evaluateTransfer, transferAsDeposit, mmIniOptions::ignoreFutureTransactions_);
            mmex::formatDoubleToCurrencyEdit(thsub.actual_, thsub.actualStr_);
            
            // set the overall actual abount for the year
            if (thsub.actual_ < 0)
                actExpenses += thsub.actual_;
            else
                actIncome += thsub.actual_;

            mmex::formatDoubleToCurrencyEdit(thsub.amt_, displayAmtString);
            thsub.amtString_ = displayAmtString;

            if ((totalEstimated_ != 0.0) || (thsub.actual_ != 0.0))
            {
                hb.startTableRow();
                hb.addTableCell(thsub.catStr_+ wxT(": ") + thsub.subCatStr_, false, true);
                hb.addTableCell(_("Estimated"));
			
                DisplayEstimateMonths(hb, thsub, startMonth);

                hb.addTableCell(totalEstimatedStr_, true, true, true);
                hb.addTableCell(wxT("-"));
                hb.endTableRow();

                hb.startTableRow();
                hb.addTableCell(thsub.catStr_+ wxT(": ") + thsub.subCatStr_, false, true);
                hb.addTableCell(_("Actual"));

                DisplayActualMonths(hb, thsub, startMonth, startYear);

                // year end
                if(thsub.actual_ < totalEstimated_)
                {
                    hb.addTableCell(thsub.actualStr_, true, true, true, wxT("#ff0000"));
                }
                else
                {
                    hb.addTableCell(thsub.actualStr_, true, false, true);
                }

                if (((totalEstimated_ < 0) && (thsub.actual_ < 0)) ||
                    ((totalEstimated_ > 0) && (thsub.actual_ > 0)))
                {
                    double percent = (fabs(thsub.actual_) / fabs(totalEstimated_)) * 100.0;
                    hb.addTableCell(wxString::Format(wxT("%.0f"), percent));
                }
                else
                {
                    hb.addTableCell(wxT("-"));
                }

                hb.endTableRow();
                hb.addRowSeparator(16);
            } 
        }
        st.Reset();
    }

    st.Finalize();
    q1.Finalize();

    hb.endTable();
	hb.endCenter();

    hb.end();
    return hb.getHTMLText();
}
