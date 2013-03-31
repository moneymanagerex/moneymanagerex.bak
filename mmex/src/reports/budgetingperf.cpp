#include "budgetingperf.h"
#include "../budgetingpanel.h"
#include "../htmlbuilder.h"
#include "../mmex.h"

mmReportBudgetingPerformance::mmReportBudgetingPerformance(mmCoreDB* core, mmGUIFrame* mainFrame, int budgetYearID)
: mmReportBudget(mainFrame, core)
, budgetYearID_(budgetYearID)
{}

void mmReportBudgetingPerformance::DisplayEstimateMonths(mmHTMLBuilder& hb, mmBudgetEntryHolder& budgetEntry, int startMonth)
{
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
        mmex::formatDoubleToCurrencyEdit(budgetEntry.estimated_ / 12, monthEstimateStr);
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
        double actualMonthVal = core_->bTransactionList_.getAmountForCategory(budgetEntry.categID_, budgetEntry.subcategID_,
            false, dtBegin, dtEnd, evaluateTransfer, transferAsDeposit, mmIniOptions::instance().ignoreFutureTransactions_
        );
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
    wxString startYearStr = mmDBWrapper::getBudgetYearForID(core_->db_.get(), budgetYearID_);
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
    hb.addHeader(2, _("Budget Performance for ") + headingStr );
    DisplayDateHeading(hb, yearBegin, yearEnd);

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

    core_->currencyList_.LoadBaseCurrencySettings();

    wxSQLite3Statement st = core_->db_.get()->PrepareStatement(SELECT_SUBCATEGS_FROM_SUBCATEGORY_V1);
    wxSQLite3ResultSet q1 = core_->db_.get()->ExecuteQuery(SELECT_ALL_FROM_CATEGORY_V1);
        
    while (q1.NextRow())
    {
        mmBudgetEntryHolder th;
        initBudgetEntryFields(th, budgetYearID_);
        th.categID_ = q1.GetInt(wxT("CATEGID"));
        th.catStr_  = q1.GetString(wxT("CATEGNAME"));
        mmDBWrapper::getBudgetEntry(core_->db_.get(), budgetYearID_, th.categID_, th.subcategID_, th.period_, th.amt_);

        // Set the estimated amount for the year
        setBudgetYearlyEstimate(th);
        double totalEstimated_ = th.estimated_;
        wxString totalEstimatedStr_;
        mmex::formatDoubleToCurrencyEdit(totalEstimated_, totalEstimatedStr_);

        // set the actual amount for the year
        bool transferAsDeposit = true;
        if (th.amt_ < 0)
        {
            transferAsDeposit = false;
        }
        th.actual_ = core_->bTransactionList_.getAmountForCategory(th.categID_, th.subcategID_, false,
            yearBegin, yearEnd, evaluateTransfer, transferAsDeposit, mmIniOptions::instance().ignoreFutureTransactions_
        );
        mmex::formatDoubleToCurrencyEdit(th.actual_, th.actualStr_);

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
            initBudgetEntryFields(thsub, budgetYearID_);
            thsub.categID_ = th.categID_;
            thsub.catStr_  = th.catStr_;
            thsub.subcategID_ = q2.GetInt(wxT("SUBCATEGID"));
            thsub.subCatStr_  = q2.GetString(wxT("SUBCATEGNAME"));

            mmDBWrapper::getBudgetEntry(core_->db_.get(), budgetYearID_, thsub.categID_, thsub.subcategID_, thsub.period_, thsub.amt_);
 
            // Set the estimated amount for the year
            setBudgetYearlyEstimate(thsub);
            totalEstimated_ = thsub.estimated_;
            mmex::formatDoubleToCurrencyEdit(totalEstimated_, totalEstimatedStr_);

            // set the actual abount for the year
            transferAsDeposit = true;
            if (thsub.amt_ < 0)
            {
                transferAsDeposit = false;
            }
            thsub.actual_ = core_->bTransactionList_.getAmountForCategory(thsub.categID_, thsub.subcategID_, false,
                yearBegin, yearEnd, evaluateTransfer, transferAsDeposit, mmIniOptions::instance().ignoreFutureTransactions_
            );
            mmex::formatDoubleToCurrencyEdit(thsub.actual_, thsub.actualStr_);
            
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
