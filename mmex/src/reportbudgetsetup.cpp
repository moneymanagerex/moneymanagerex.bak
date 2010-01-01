#include "reportbudgetsetup.h"

#include "defs.h"
#include "htmlbuilder.h"
#include "reportbase.h"
#include "util.h"
#include "dbwrapper.h"
#include "mmcoredb.h"
#include "budgetingpanel.h"

mmReportBudgetingSetup::mmReportBudgetingSetup(mmCoreDB* core, int year) : 
        core_(core),
        db_(core_->db_.get()),
        year_(year)
{
}

wxString mmReportBudgetingSetup::getHTMLText()
{
       wxString yearStr = mmDBWrapper::getBudgetYearForID(db_, year_);

        mmHTMLBuilder hb;
        hb.init();
        hb.addHeader(3, _("Budget setup for Year : ") + yearStr + _(" (Estimated Vs Actual)"));

        wxDateTime now = wxDateTime::Now();
        wxString dt = _("Today's Date: ") + mmGetNiceDateString(now);
        hb.addHeader(7, dt);
        hb.addLineBreak();
        hb.addLineBreak();

		hb.startCenter();

        int budgetYearID_ =  year_;


		double estIncome = 0.0;
		double estExpenses = 0.0;
		double actIncome = 0.0;
		double actExpenses = 0.0;

		long year = 0;
		mmDBWrapper::getBudgetYearForID(db_, budgetYearID_).ToLong(&year);;
		wxDateTime dtBegin(1, wxDateTime::Jan, year);
		wxDateTime dtEnd(31, wxDateTime::Dec, year);

			hb.startTable();
			hb.startTableRow();
			hb.addTableHeaderCell(_("Category"));
			hb.addTableHeaderCell(_("Sub Category"));
			hb.addTableHeaderCell(_("Frequency"));
			hb.addTableHeaderCell(_("Amount"));
			hb.addTableHeaderCell(_("Estimated"));
			hb.addTableHeaderCell(_("Actual"));
			hb.endTableRow();

		mmDBWrapper::loadBaseCurrencySettings(db_);

		static const char sql[] = 
		"select CATEGID, CATEGNAME "
        "from CATEGORY_V1 "
        "order by CATEGNAME";

        static const char sql_sub[] = 
        "select SUBCATEGID, SUBCATEGNAME "
        "from SUBCATEGORY_V1 "
        "where CATEGID = ?";

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
			th.estimatedStr_ = wxT("0.0");
			th.estimated_ = 0.0;
			th.actualStr_ = wxT("0.0");
			th.actual_ = 0.0;
			mmDBWrapper::getBudgetEntry(db_, budgetYearID_, th.categID_, th.subcategID_, th.period_, 
				th.amt_);

			if (th.period_ == wxT("Monthly"))
			{
				th.estimated_ = th.amt_ * 12;
			}
			else if (th.period_ == wxT("Yearly"))
			{
				th.estimated_ = th.amt_;
			}
			else if (th.period_ == wxT("Weekly"))
			{
				th.estimated_ = th.amt_ * 52;
			}
			else if (th.period_ == wxT("Bi-Weekly"))
			{
				th.estimated_ = th.amt_ * 26;
			}
			else if (th.period_ == wxT("Bi-Monthly"))
			{
				th.estimated_ = th.amt_ * 6;
			}
			 else if (th.period_ == wxT("Quarterly"))
			{
				th.estimated_ = th.amt_ * 4;
			}
			   else if (th.period_ == wxT("Half-Yearly"))
			{
				th.estimated_ = th.amt_ * 2;
			}
			else
				wxASSERT(true);

			if (th.estimated_ < 0)
				estExpenses += th.estimated_;
			else
				estIncome += th.estimated_;

			mmCurrencyFormatter::formatDoubleToCurrencyEdit(th.estimated_, th.estimatedStr_);

			th.actual_ = mmDBWrapper::getAmountForCategory(db_, th.categID_, th.subcategID_, 
						false,  dtBegin, dtEnd);
			mmCurrencyFormatter::formatDoubleToCurrencyEdit(th.actual_, th.actualStr_);

			if (th.actual_ < 0)
				actExpenses += th.actual_;
			else
				actIncome += th.actual_;

			wxString displayAmtString;
			mmCurrencyFormatter::formatDoubleToCurrencyEdit(th.amt_, displayAmtString);
			th.amtString_ = displayAmtString;

			//START:CATEGORY ROW
			hb.startTableRow();
			hb.addTableCell(th.catStr_, false, true);
			hb.addTableCell(wxT(""), false, true);
			hb.addTableCell(th.period_, false, true); 
			hb.addTableCell(th.amtString_, true);//Amount
			hb.addTableCell(th.estimatedStr_, true);//estimated for subcategory
			hb.addTableCell(th.actualStr_, true);//actual Amount for subcategory
			hb.endTableRow();
			//END:CATEGORY ROW

			//START: SUBCATEGORY ROW
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
				thsub.period_ = wxT("None");
				thsub.estimatedStr_ = wxT("0.0");
				thsub.estimated_ = 0.0;
				thsub.actualStr_ = wxT("0.0");
				thsub.actual_ = 0.0;
				mmDBWrapper::getBudgetEntry(db_, budgetYearID_, thsub.categID_, thsub.subcategID_, thsub.period_, 
					thsub.amt_);

				if (thsub.period_ == wxT("Monthly"))
				{
					thsub.estimated_ = thsub.amt_ * 12;
				}
				else if (thsub.period_ == wxT("Yearly"))
				{
					thsub.estimated_ = thsub.amt_;
				}
				else if (thsub.period_ == wxT("Weekly"))
				{
					thsub.estimated_ = thsub.amt_ * 52;
				}
				else if (thsub.period_ == wxT("Bi-Weekly"))
				{
					thsub.estimated_ = thsub.amt_ * 26;
				}
				else if (thsub.period_ == wxT("Bi-Monthly"))
				{
					thsub.estimated_ = thsub.amt_ * 6;
				}
				else if (thsub.period_ == wxT("Quarterly"))
				{
					thsub.estimated_ = thsub.amt_ * 4;
				}
				else if (thsub.period_ == wxT("Half-Yearly"))
				{
					thsub.estimated_ = thsub.amt_ * 2;
				}
				else
					wxASSERT(true);

				mmCurrencyFormatter::formatDoubleToCurrencyEdit(thsub.estimated_, thsub.estimatedStr_);
				if (thsub.estimated_ < 0)
					estExpenses += thsub.estimated_;
				else
					estIncome += thsub.estimated_;

				thsub.actual_ = mmDBWrapper::getAmountForCategory(db_, thsub.categID_, thsub.subcategID_, 
					false,  dtBegin, dtEnd);
				mmCurrencyFormatter::formatDoubleToCurrencyEdit(thsub.actual_, thsub.actualStr_);
				if (thsub.actual_ < 0)
					actExpenses += thsub.actual_;
				else
					actIncome += thsub.actual_;


				mmCurrencyFormatter::formatDoubleToCurrencyEdit(thsub.amt_, displayAmtString);
				thsub.amtString_ = displayAmtString;
				
				hb.startTableRow();
				hb.addTableCell(th.catStr_, false, true);//CATEGORY			
				hb.addTableCell(thsub.subCatStr_, false, true);//SUBCATEGORY			
				hb.addTableCell(thsub.period_, false, true);//FREQUENCY
				hb.addTableCell(thsub.amtString_, true);//Amount
				hb.addTableCell(thsub.estimatedStr_, true);//estimated amount for subcategory
				hb.addTableCell(thsub.actualStr_, true);//actual amount for subcategory

				hb.endTableRow(); 
				//END: SUBCATEGORY ROW
			}
			st.Reset();
		}
		q1.Finalize();
        st.Finalize();

        hb.endTable();
		hb.endCenter();

		wxString estIncomeStr, actIncomeStr,  estExpensesStr, actExpensesStr;
		mmCurrencyFormatter::formatDoubleToCurrency(estIncome, estIncomeStr);
		mmCurrencyFormatter::formatDoubleToCurrency(actIncome, actIncomeStr);
		if (estExpenses < 0.0)
			estExpenses = -estExpenses;
		if (actExpenses < 0.0)
			actExpenses = -actExpenses;
		mmCurrencyFormatter::formatDoubleToCurrency(estExpenses, estExpensesStr);
		mmCurrencyFormatter::formatDoubleToCurrency(actExpenses, actExpensesStr);

		wxString incStr = wxString::Format(_("Estimated Income: %s Actual Income:  %s"), 
			estIncomeStr.c_str(), actIncomeStr.c_str());

		wxString expStr = wxString::Format(_("Estimated Expenses: %s Actual Expenses:  %s"), 
			estExpensesStr.c_str(), actExpensesStr.c_str());
		
		
		//Summary of Estimated Vs Actual totals
		hb.addLineBreak();
		hb.startCenter();
		hb.startTable();		
		hb.addRowSeparator(1);
		hb.startTableRow();
		hb.addTableCell(incStr,false,true);
		hb.endTableRow();

		hb.startTableRow();
		hb.addTableCell(expStr,false,true);
		hb.endTableRow();
		hb.endTable();
		hb.endCenter();

        hb.end();
        return hb.getHTMLText();
}
