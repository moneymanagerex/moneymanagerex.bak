#include "payee.h"
#include "budget.h"

#include "../htmlbuilder.h"
#include "../util.h"
#include "../mmgraphpie.h"

mmReportPayeeExpenses::mmReportPayeeExpenses(mmCoreDB* core, bool ignoreDate, const wxString& title, mmDateRange* date_range)
: mmPrintableBase(core)
, ignoreDate_(ignoreDate)
, title_(title)
, date_range_(date_range)
{}

mmReportPayeeExpenses::~mmReportPayeeExpenses()
{
    if (date_range_) delete date_range_;
}

wxString mmReportPayeeExpenses::getHTMLText()
{
    mmHTMLBuilder hb;
    hb.init();
    hb.addHeader(2, title_);

    mmCommonReportDetails dateDisplay(NULL);
    dateDisplay.DisplayDateHeading(hb, date_range_->start_date_, date_range_->end_date_, !ignoreDate_);   

	hb.startCenter();

    // Add the graph
    mmGraphPie gg;
    if (core_->payeeList_.Num())
    {
        hb.addImage(gg.getOutputFileName());
    }
	hb.startTable("50%");
	hb.startTableRow();
	hb.addTableHeaderCell(_("Payee"));
	hb.addTableHeaderCell(_("Amount"), true);
	hb.endTableRow();

    core_->currencyList_.LoadBaseCurrencySettings();
        
    double total = 0.0;
    double positiveTotal = 0.0;
    double negativeTotal = 0.0;
    std::vector<ValuePair> valueList;
    std::pair<mmPayeeList::const_iterator, mmPayeeList::const_iterator> payee_range = core_->payeeList_.Range();
    for (mmPayeeList::const_iterator it = payee_range.first; it != payee_range.second; ++ it)
    {
        wxString balance;
        double amt = core_->bTransactionList_.getAmountForPayee((*it)->id_,
            ignoreDate_, date_range_->start_date_, date_range_->end_date_, mmIniOptions::instance().ignoreFutureTransactions_
        );
        mmex::formatDoubleToCurrency(amt, balance);

        if (amt != 0.0)
        {
            total += amt;
            if (amt>0.0)
            {
                positiveTotal += amt;
            }
            if (amt<0.0)
            {
                negativeTotal += amt;
            }
            ValuePair vp;
            vp.label = (*it)->name_;
            vp.amount = amt;
            valueList.push_back(vp);

            hb.startTableRow();
            hb.addTableCell((*it)->name_, false, true);

            hb.addTableCell(balance, true, true, true, (amt<0.0 ? "RED" : "BLACK"));
            hb.endTableRow();
		}
    }

    wxString payeetotalStr;
    wxString positiveTotalStr;
    wxString negativeTotalStr;
    wxString colorStr = (total<0.0 ? "RED" : "BLACK");
    mmex::formatDoubleToCurrency(total, payeetotalStr);
    mmex::formatDoubleToCurrency(positiveTotal, positiveTotalStr);
    mmex::formatDoubleToCurrency(negativeTotal, negativeTotalStr);

    hb.addRowSeparator(2);
    hb.startTableRow();
    hb.addTableCell(_("Income:"),false, true, true, "BLACK");
    hb.addTableCell(positiveTotalStr, true, false, true, "BLACK");
	hb.endTableRow();
    hb.startTableRow();
    hb.addTableCell(_("Expenses:"),false, true, true, "BLACK");
    hb.addTableCell(negativeTotalStr, true, false, true, "RED");
	hb.endTableRow();
    hb.addRowSeparator(2);
    hb.startTableRow();
    hb.addTableCell(_("Payees Total: "),false, true, true, "BLACK");
    hb.addTableCell(payeetotalStr, true, false, true, colorStr);
	hb.endTableRow();

    hb.endTable();
	hb.endCenter();

    hb.end();

    gg.init(valueList);
    gg.Generate(title_);

    return hb.getHTMLText();
}
