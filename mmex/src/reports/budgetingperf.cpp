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
		hb.addMoneyCell(budgetEntry.estimated_ / 12);
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

        if(actualMonthVal < budgetEntry.estimated_)
		{
			hb.addMoneyCell(actualMonthVal, "RED");
        }
        else
        {
			hb.addMoneyCell(actualMonthVal);
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
    hb.DisplayDateHeading(yearBegin, yearEnd);

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
        th.categID_ = q1.GetInt("CATEGID");
        th.catStr_  = q1.GetString("CATEGNAME");
        mmDBWrapper::getBudgetEntry(core_->db_.get(), budgetYearID_, th.categID_, th.subcategID_, th.period_, th.amt_);

        // Set the estimated amount for the year
        setBudgetYearlyEstimate(th);
        double totalEstimated_ = th.estimated_;

        // set the actual amount for the year
        bool transferAsDeposit = true;
        if (th.amt_ < 0)
        {
            transferAsDeposit = false;
        }
        th.actual_ = core_->bTransactionList_.getAmountForCategory(th.categID_, th.subcategID_, false,
            yearBegin, yearEnd, evaluateTransfer, transferAsDeposit, mmIniOptions::instance().ignoreFutureTransactions_
        );

        // estimated stuff
        if ((totalEstimated_ != 0.0) || (th.actual_ != 0.0))
        {
            hb.startTableRow();
			hb.addTableCell(th.catStr_, false, true);
			hb.addTableCell(_("Estimated"));
            
            DisplayEstimateMonths(hb, th, startMonth);
           
			hb.addMoneyCell(totalEstimated_);
            hb.addTableCell("-");
            hb.endTableRow();

            // actual stuff
            hb.startTableRow();
			hb.addTableCell(th.catStr_, false, true);
			hb.addTableCell(_("Actual"));

            DisplayActualMonths(hb, th, startMonth, startYear);

            // year end
			if(th.actual_ < totalEstimated_)
			{
				hb.addMoneyCell(th.actual_, "RED");
			}
			else
			{
				hb.addMoneyCell(th.actual_);
			}

            if (((totalEstimated_ < 0) && (th.actual_ < 0)) ||
                ((totalEstimated_ > 0) && (th.actual_ > 0)))
            {
                double percent = (fabs(th.actual_) / fabs(totalEstimated_)) * 100.0;
                hb.addTableCell(wxString::Format("%.0f", percent));
            }
            else
            {
                hb.addTableCell("-");
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
            thsub.subcategID_ = q2.GetInt("SUBCATEGID");
            thsub.subCatStr_  = q2.GetString("SUBCATEGNAME");

            mmDBWrapper::getBudgetEntry(core_->db_.get(), budgetYearID_, thsub.categID_, thsub.subcategID_, thsub.period_, thsub.amt_);
 
            // Set the estimated amount for the year
            setBudgetYearlyEstimate(thsub);
            totalEstimated_ = thsub.estimated_;

            // set the actual abount for the year
            transferAsDeposit = true;
            if (thsub.amt_ < 0)
            {
                transferAsDeposit = false;
            }
            thsub.actual_ = core_->bTransactionList_.getAmountForCategory(thsub.categID_, thsub.subcategID_, false,
                yearBegin, yearEnd, evaluateTransfer, transferAsDeposit, mmIniOptions::instance().ignoreFutureTransactions_
            );
            
            if ((totalEstimated_ != 0.0) || (thsub.actual_ != 0.0))
            {
                hb.startTableRow();
                hb.addTableCell(thsub.catStr_+ ": " + thsub.subCatStr_, false, true);
                hb.addTableCell(_("Estimated"));
			
                DisplayEstimateMonths(hb, thsub, startMonth);

				hb.addMoneyCell(totalEstimated_);
                hb.addTableCell("-");
                hb.endTableRow();

                hb.startTableRow();
                hb.addTableCell(thsub.catStr_+ ": " + thsub.subCatStr_, false, true);
                hb.addTableCell(_("Actual"));

                DisplayActualMonths(hb, thsub, startMonth, startYear);

                // year end
                if(thsub.actual_ < totalEstimated_)
                {
					hb.addMoneyCell(thsub.actual_, "RED");
                }
                else
                {
					hb.addMoneyCell(thsub.actual_);
                }

                if (((totalEstimated_ < 0) && (thsub.actual_ < 0)) ||
                    ((totalEstimated_ > 0) && (thsub.actual_ > 0)))
                {
                    double percent = (fabs(thsub.actual_) / fabs(totalEstimated_)) * 100.0;
                    hb.addTableCell(wxString::Format("%.0f", percent));
                }
                else
                {
                    hb.addTableCell("-");
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
