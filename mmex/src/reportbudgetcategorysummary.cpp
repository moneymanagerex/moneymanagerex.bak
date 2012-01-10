/*************************************************************************
 Copyright (C) 2012 Stefano Giorgio

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
 *************************************************************************/

/************************************************************************* 
 Renamed after extensive modifications to original file reportbudgetsetup.cpp
**************************************************************************/
#include "reportbudgetcategorysummary.h"

#include "defs.h"
#include "htmlbuilder.h"
#include "reportbase.h"
#include "util.h"
#include "dbwrapper.h"
#include "mmcoredb.h"
#include "budgetingpanel.h"
#include "mmex.h"

mmReportBudgetCategorySummary::mmReportBudgetCategorySummary(mmCoreDB* core, mmGUIFrame* mainFrame, int budgetYearID)
    : mmReportBudget(mainFrame), core_(core), db_(core_->db_.get()), budgetYearID_(budgetYearID)
{
}

wxString mmReportBudgetCategorySummary::actualAmountColour( mmBudgetEntryHolder& budEntry, bool total)
{
    wxString actAmtColStr = wxT("black");
    if (total) {
        actAmtColStr = wxT("blue");
    }
    
    if (budEntry.amt_ == 0) {
        actAmtColStr = wxT("blue");
    } else {
        if (budEntry.actual_ < budEntry.estimated_) {
            actAmtColStr = wxT("red");
        }
    }

    return actAmtColStr;
}

// Displays Row: Category, Sub Category, Period, Amount, Estimated, Actual
void mmReportBudgetCategorySummary::displayReportLine(mmHTMLBuilder& hb, mmBudgetEntryHolder budEntry)
{
	hb.startTableRow();
	hb.addTableCell(budEntry.catStr_, false, true);
    hb.addTableCell(budEntry.subCatStr_, false, true);
	hb.addTableCell(budEntry.period_, false, true); 
	hb.addTableCell(budEntry.amtString_, true);
	hb.addTableCell(budEntry.estimatedStr_, true);
	hb.addTableCell(budEntry.actualStr_, true, false, false, actualAmountColour(budEntry));
	hb.endTableRow();
}

wxString mmReportBudgetCategorySummary::getHTMLText()
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
		mmDBWrapper::getBudgetEntry(db_, budgetYearID_, th.categID_, th.subcategID_, th.period_, th.amt_);

        setBudgetEstimate(th,monthlyBudget,yearBegin,yearEnd);
		if (th.estimated_ < 0) {
			estExpenses += th.estimated_;
        } else {
			estIncome += th.estimated_;
        }
		mmex::formatDoubleToCurrencyEdit(th.estimated_, th.estimatedStr_);

        bool transferAsDeposit = true;
        if (th.amt_ < 0)
        {
            transferAsDeposit = false;
        }
		th.actual_ = mmDBWrapper::getAmountForCategory(db_, th.categID_, th.subcategID_, false, yearBegin, yearEnd, true, transferAsDeposit);
		mmex::formatDoubleToCurrencyEdit(th.actual_, th.actualStr_);

		if (th.actual_ < 0) {
			actExpenses += th.actual_;
        } else {
			actIncome += th.actual_;
        }
		wxString displayAmtString;
		mmex::formatDoubleToCurrencyEdit(th.amt_, displayAmtString);
		th.amtString_ = displayAmtString;

        if (mainFrame_->budgetCategoryTotal())
        {
            th.subCatStr_ = wxEmptyString;
            displayReportLine(hb, th);
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
			mmDBWrapper::getBudgetEntry(db_, budgetYearID_, thsub.categID_, thsub.subcategID_, thsub.period_, thsub.amt_);

            setBudgetEstimate(thsub,monthlyBudget,yearBegin,yearEnd);
			mmex::formatDoubleToCurrencyEdit(thsub.estimated_, thsub.estimatedStr_);
			if (thsub.estimated_ < 0) {
				estExpenses += thsub.estimated_;
            } else {
                estIncome += thsub.estimated_;
            }
            
            transferAsDeposit = true;
            if (thsub.amt_ < 0)
            {
                transferAsDeposit = false;
            }
            thsub.actual_ = mmDBWrapper::getAmountForCategory(db_, thsub.categID_, thsub.subcategID_, false, yearBegin, yearEnd, true, transferAsDeposit);
			mmex::formatDoubleToCurrencyEdit(thsub.actual_, thsub.actualStr_);
			if (thsub.actual_ < 0) {
				actExpenses += thsub.actual_;
            } else {
				actIncome += thsub.actual_;
            }
			mmex::formatDoubleToCurrencyEdit(thsub.amt_, displayAmtString);
			thsub.amtString_ = displayAmtString;
	        
            if (mainFrame_->budgetCategoryTotal())
            {
                displayReportLine(hb, thsub);
            }

            /*************************************************************************** 
             Update the TOTALS entry for the subcategory.
             ***************************************************************************/
            catTotals.amt_       += thsub.amt_;
            catTotals.estimated_ += thsub.estimated_;
            catTotals.actual_    += thsub.actual_;
            mmex::formatDoubleToCurrencyEdit(catTotals.amt_,       catTotals.amtString_);
            mmex::formatDoubleToCurrencyEdit(catTotals.estimated_, catTotals.estimatedStr_);
            mmex::formatDoubleToCurrencyEdit(catTotals.actual_,    catTotals.actualStr_);
        }
        //END: SUBCATEGORY ROW
        st.Reset();

        /*************************************************************************** 
            Display a TOTALS entry for the category.
        ****************************************************************************/
        if (mainFrame_->budgetCategoryTotal()) {
            hb.addRowSeparator(6);
        }
        // Category, Sub Category, Period, Amount, Estimated, Actual
        hb.startTableRow();
		hb.addTableCell(catTotals.catStr_, false, true, true, wxT("blue"));
		hb.addTableCell(wxT(""), false, true, true, wxT("blue"));
		hb.addTableCell(catTotals.period_, false, true, true, wxT("blue")); 
		hb.addTableCell(catTotals.amtString_, true, false,true, wxT("blue"));
		hb.addTableCell(catTotals.estimatedStr_, true, false, true, wxT("blue"));
		hb.addTableCell(catTotals.actualStr_, true, false, true, actualAmountColour(catTotals,true));
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
	if (estExpenses < 0.0) {
		estExpenses = -estExpenses;
    }
	if (actExpenses < 0.0) {
		actExpenses = -actExpenses;
    }
	mmex::formatDoubleToCurrency(estExpenses, estExpensesStr);
	mmex::formatDoubleToCurrency(actExpenses, actExpensesStr);

    double difIncome = estIncome - actIncome;
    wxString difIncomeStr;
    wxString incomeDiferenceColour = wxT("red");
    if (difIncome < 0) {
        difIncome = -difIncome;
        incomeDiferenceColour = wxT("black");
    }
	mmex::formatDoubleToCurrency(difIncome, difIncomeStr);

    double difExpense = estExpenses - actExpenses;
    wxString difExpenseStr;
    wxString expenseDiferenceColour = wxT("black");
    if (difExpense < 0) {
        difExpense = -difExpense;
        expenseDiferenceColour = wxT("red");
    }
	mmex::formatDoubleToCurrency(difExpense, difExpenseStr);

    wxString incEstStr = wxString() << _("Estimated Income: ") << estIncomeStr;
	wxString incActStr = wxString() << _("Actual Income: ")    << actIncomeStr;
    wxString incDifStr = wxString() << _("Diference Income: ") << difIncomeStr;
    
    wxString expEstStr = wxString() << _("Estimated Expenses: ") << estExpensesStr;
	wxString expActStr = wxString() << _("Actual Expenses: ")    << actExpensesStr;
    wxString expDifStr = wxString() << _("Diference Expenses: ") << difExpenseStr;

    //Summary of Estimated Vs Actual totals
	hb.addLineBreak();
	hb.startCenter();
	hb.startTable(wxT("50%"));		
//	hb.addRowSeparator(3);
	hb.startTableRow();
	hb.addTableCell(incEstStr, true, true);
	hb.addTableCell(incActStr, true, true);
	hb.addTableCell(incDifStr, true, true, false, incomeDiferenceColour);
	hb.endTableRow();

	hb.startTableRow();
	hb.addTableCell(expEstStr, true, true);
	hb.addTableCell(expActStr, true, true);
	hb.addTableCell(expDifStr, true, true, false, expenseDiferenceColour);
	hb.endTableRow();
	hb.addRowSeparator(3);
	hb.endTable();
	hb.endCenter();

    hb.end();
    return hb.getHTMLText();
}
