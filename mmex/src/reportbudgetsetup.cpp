#include "reportbudgetsetup.h"

#include "defs.h"
#include "htmlbuilder.h"
#include "reportbase.h"
#include "util.h"
#include "dbwrapper.h"
#include "mmcoredb.h"
#include "budgetingpanel.h"
#include "mmex.h"

mmReportBudgetingSetup::mmReportBudgetingSetup(mmCoreDB* core, mmGUIFrame* mainFrame, int budgetYearID)
    : mmReportBudget(mainFrame), core_(core), db_(core_->db_.get()), budgetYearID_(budgetYearID)
{
}

wxString mmReportBudgetingSetup::getHTMLText()
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

    bool monthlyBudget = (startYearStr.length() > 5);
    if (monthlyBudget) {
        SetBudgetMonth(startYearStr, yearBegin, yearEnd);
    } else {
        AdjustDateForEndFinancialYear(yearEnd);
    }

    mmHTMLBuilder hb;
    hb.init();
    wxString headerStartupMsg = _("Budget Category Summary for ");
    if (mainFrame_->budgetCategoryTotal())
        headerStartupMsg = _("Budget Categories for ");
    hb.addHeader(3, headerStartupMsg + headingStr + _("<br>( Estimated Vs Actual )"));
    DisplayDateHeading(hb, yearBegin, yearEnd);

    double estIncome = 0.0;
    double estExpenses = 0.0;
    double actIncome = 0.0;
    double actExpenses = 0.0;

    hb.startCenter();

    hb.startTable(wxT("65%"));
    hb.startTableRow();
    hb.addTableHeaderCell(_("Category"));
    hb.addTableHeaderCell(_("Sub Category"));
    hb.addTableHeaderCell(_("Frequency"));
    hb.addTableHeaderCell(_("Amount"), true);
    hb.addTableHeaderCell(_("Estimated"), true);
    hb.addTableHeaderCell(_("Actual"), true);
    hb.endTableRow();

    mmDBWrapper::loadBaseCurrencySettings(db_);

    static const char sql[] = 
    "select CATEGID, CATEGNAME "
    "from CATEGORY_V1 "
    "order by CATEGNAME";

    static const char sql_sub[] = 
    "select SUBCATEGID, SUBCATEGNAME "
    "from SUBCATEGORY_V1 "
    "where CATEGID = ? "
    "order by SUBCATEGNAME";

    wxSQLite3Statement st = db_->PrepareStatement(sql_sub);

    wxSQLite3ResultSet q1 = db_->ExecuteQuery(sql);
	while (q1.NextRow())
	{
		mmBudgetEntryHolder th;
        initBudgetEntryFields(th);
		th.categID_ = q1.GetInt(wxT("CATEGID"));
		th.catStr_ = q1.GetString(wxT("CATEGNAME"));
		mmDBWrapper::getBudgetEntry(db_, budgetYearID_, th.categID_, th.subcategID_,
            th.period_, th.amt_);

        setBudgetEstimate(th,monthlyBudget,yearBegin,yearEnd);

		if (th.estimated_ < 0)
			estExpenses += th.estimated_;
		else
			estIncome += th.estimated_;
		mmex::formatDoubleToCurrencyEdit(th.estimated_, th.estimatedStr_);

		th.actual_ = mmDBWrapper::getAmountForCategory(db_, th.categID_, th.subcategID_,
            false,  yearBegin, yearEnd);
		mmex::formatDoubleToCurrencyEdit(th.actual_, th.actualStr_);

		if (th.actual_ < 0)
			actExpenses += th.actual_;
		else
			actIncome += th.actual_;

		wxString displayAmtString;
		mmex::formatDoubleToCurrencyEdit(th.amt_, displayAmtString);
		th.amtString_ = displayAmtString;

        if (mainFrame_->budgetCategoryTotal())
        {
		    //START:CATEGORY ROW
		    hb.startTableRow();
		    hb.addTableCell(th.catStr_, false, true);
		    hb.addTableCell(wxT(""), false, true);
		    hb.addTableCell(th.period_, false, true); 
		    hb.addTableCell(th.amtString_, true);//Amount
		    hb.addTableCell(th.estimatedStr_, true);//estimated for subcategory
		    hb.addTableCell(th.actualStr_, true, false, false, ActualAmountColour(th));//actual Amount for subcategory
		    hb.endTableRow();
		    //END:CATEGORY ROW
        }
        /*************************************************************************** 
         Create a TOTALS entry for the category.
         ***************************************************************************/
        mmBudgetEntryHolder catTotals;
        catTotals.budgetEntryID_ = -1;
        catTotals.categID_ = -1;
        catTotals.catStr_  = th.catStr_;
        catTotals.subcategID_ = -1;
        catTotals.subCatStr_ = wxEmptyString;
        catTotals.period_    = wxEmptyString;
        catTotals.amt_       = th.amt_;
        catTotals.estimated_ = th.estimated_;
        catTotals.actual_    = th.actual_;
        mmex::formatDoubleToCurrencyEdit(catTotals.amt_, catTotals.amtString_);
        mmex::formatDoubleToCurrencyEdit(catTotals.estimated_, catTotals.estimatedStr_);
        mmex::formatDoubleToCurrencyEdit(catTotals.actual_, catTotals.actualStr_);
        /***************************************************************************/

		//START: SUBCATEGORY ROW
        st.Bind(1, th.categID_);
        wxSQLite3ResultSet q2 = st.ExecuteQuery(); 

        while(q2.NextRow())
		{
			mmBudgetEntryHolder thsub;
            initBudgetEntryFields(thsub);
			thsub.categID_ = th.categID_;
			thsub.catStr_ = th.catStr_;
			thsub.subcategID_ = q2.GetInt(wxT("SUBCATEGID"));
			thsub.subCatStr_   = q2.GetString(wxT("SUBCATEGNAME"));
			mmDBWrapper::getBudgetEntry(db_, budgetYearID_, thsub.categID_, thsub.subcategID_,
                thsub.period_, thsub.amt_);

            setBudgetEstimate(thsub,monthlyBudget,yearBegin,yearEnd);

			mmex::formatDoubleToCurrencyEdit(thsub.estimated_, thsub.estimatedStr_);
			if (thsub.estimated_ < 0)
				estExpenses += thsub.estimated_;
			else
				estIncome += thsub.estimated_;

			thsub.actual_ = mmDBWrapper::getAmountForCategory(db_, thsub.categID_, thsub.subcategID_, 
				false,  yearBegin, yearEnd);
			mmex::formatDoubleToCurrencyEdit(thsub.actual_, thsub.actualStr_);
			if (thsub.actual_ < 0)
				actExpenses += thsub.actual_;
			else
				actIncome += thsub.actual_;

			mmex::formatDoubleToCurrencyEdit(thsub.amt_, displayAmtString);
			thsub.amtString_ = displayAmtString;
	        
            if (mainFrame_->budgetCategoryTotal())
            {
                hb.startTableRow();
                hb.addTableCell(th.catStr_, false, true);//CATEGORY			
                hb.addTableCell(thsub.subCatStr_, false, true);//SUBCATEGORY			
                hb.addTableCell(thsub.period_, false, true);//FREQUENCY
                hb.addTableCell(thsub.amtString_, true);//Amount
                hb.addTableCell(thsub.estimatedStr_, true);//estimated amount for subcategory
                hb.addTableCell(thsub.actualStr_, true,false,false,ActualAmountColour(thsub));//actual amount for subcategory
                hb.endTableRow(); 
            }
            //END: SUBCATEGORY ROW

            /*************************************************************************** 
             Update the TOTALS entry for the subcategory.
             ***************************************************************************/
            catTotals.amt_          += thsub.amt_;
            catTotals.estimated_    += thsub.estimated_;
            catTotals.actual_       += thsub.actual_;
            mmex::formatDoubleToCurrencyEdit(catTotals.amt_,       catTotals.amtString_);
            mmex::formatDoubleToCurrencyEdit(catTotals.estimated_, catTotals.estimatedStr_);
            mmex::formatDoubleToCurrencyEdit(catTotals.actual_,    catTotals.actualStr_);
        }
        st.Reset();

        /*************************************************************************** 
            Display a TOTALS entry for the category.
        ****************************************************************************/
        if (mainFrame_->budgetCategoryTotal()) {
            hb.addRowSeparator(6);
        }

        hb.startTableRow();
		hb.addTableCell(catTotals.catStr_, false, true, true, wxT("blue"));
		hb.addTableCell(wxT(""), false, true, true, wxT("blue"));
		hb.addTableCell(catTotals.period_, false, true, true, wxT("blue")); 
		hb.addTableCell(catTotals.amtString_, true, false,true, wxT("blue"));//Amount
		hb.addTableCell(catTotals.estimatedStr_, true, false, true, wxT("blue"));//estimated for subcategory
		hb.addTableCell(catTotals.actualStr_, true, false, true, ActualAmountColour(catTotals,true));//actual Amount for subcategory
		hb.endTableRow();
    	hb.addRowSeparator(6);
        /***************************************************************************/
    }
	q1.Finalize();
    st.Finalize();

    hb.endTable();
	hb.endCenter();

	wxString estIncomeStr, actIncomeStr,  estExpensesStr, actExpensesStr;
	mmex::formatDoubleToCurrency(estIncome, estIncomeStr);
	mmex::formatDoubleToCurrency(actIncome, actIncomeStr);
	if (estExpenses < 0.0)
		estExpenses = -estExpenses;
	if (actExpenses < 0.0)
		actExpenses = -actExpenses;
	mmex::formatDoubleToCurrency(estExpenses, estExpensesStr);
	mmex::formatDoubleToCurrency(actExpenses, actExpensesStr);

	wxString incEstStr = wxString::Format(_("Estimated Income: %s"), estIncomeStr.c_str());
	wxString incActStr = wxString::Format(_("Actual Income:  %s"), actIncomeStr.c_str());
	wxString expEstStr = wxString::Format(_("Estimated Expenses: %s"), estExpensesStr.c_str());
	wxString expActStr = wxString::Format(_("Actual Expenses:  %s"), actExpensesStr.c_str());

    //Summary of Estimated Vs Actual totals
	hb.addLineBreak();
	hb.startCenter();
	hb.startTable(wxT("50%"));		
	hb.addRowSeparator(2);
	hb.startTableRow();
	hb.addTableCell(incEstStr,true,true);
	hb.addTableCell(incActStr,true,true);
	hb.endTableRow();

	hb.startTableRow();
	hb.addTableCell(expEstStr,true,true);
	hb.addTableCell(expActStr,true,true);
	hb.endTableRow();
	hb.addRowSeparator(2);
	hb.endTable();
	hb.endCenter();

    hb.end();
    return hb.getHTMLText();
}

wxString mmReportBudgetingSetup::ActualAmountColour( mmBudgetEntryHolder& budEntry, bool total)
{
    wxString actAmtColStr = wxT("black");
    if (total) {
        actAmtColStr = wxT("blue");
    }
    
    if (budEntry.amt_ == 0) {
        actAmtColStr = wxT("blue");
    } else {
        if (budEntry.actual_ < budEntry.estimated_)
            actAmtColStr = wxT("red");
    }

    return actAmtColStr;
}
