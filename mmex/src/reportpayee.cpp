#include "reportpayee.h"

#include "defs.h"
#include "htmlbuilder.h"
#include "reportbase.h"
#include "util.h"
#include "dbwrapper.h"
#include "mmgraphpie.h"
#include "mmcoredb.h"
#include "reportbudget.h"

mmReportPayeeExpenses::mmReportPayeeExpenses(
    mmCoreDB* core,
    bool ignoreDate,
    wxDateTime dtBegin,
    wxDateTime dtEnd,
    const wxString& title
) : core_(core), dtBegin_(dtBegin), dtEnd_(dtEnd), ignoreDate_(ignoreDate), title_(title)
{
}

wxString mmReportPayeeExpenses::getHTMLText()
{
    mmHTMLBuilder hb;
    hb.init();
    hb.addHeader(3, title_);

    mmCommonReportDetails dateDisplay(NULL);
    dateDisplay.DisplayDateHeading(hb, dtBegin_, dtEnd_, !ignoreDate_);
    // Start date needs to be set to one day before, to get correct date period.
    dtBegin_.Subtract(wxDateSpan::Day());   

	hb.startCenter();

    int numPayees = (int)core_->payeeList_.payees_.size();
    // Add the graph
    mmGraphPie gg;
    if (numPayees)
    {
        hb.addImage(gg.getOutputFileName());
    }
	hb.startTable(wxT("50%"));
	hb.startTableRow();
	hb.addTableHeaderCell(_("Payee"));
	hb.addTableHeaderCell(_("Amount"));
	hb.endTableRow();

    core_->currencyList_.loadBaseCurrencySettings();
        
    double total = 0.0;
    double positiveTotal = 0.0;
    double negativeTotal = 0.0;
    std::vector<ValuePair> valueList;
    for (int idx = 0; idx < numPayees; idx++)
    {
        wxString balance;
        double amt = core_->bTransactionList_.getAmountForPayee(core_->payeeList_.payees_[idx]->payeeID_,
            ignoreDate_, dtBegin_, dtEnd_, mmIniOptions::ignore_future_transactions_
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
            vp.label = core_->payeeList_.payees_[idx]->payeeName_;
            vp.amount = amt;
            valueList.push_back(vp);

            hb.startTableRow();
            hb.addTableCell(core_->payeeList_.payees_[idx]->payeeName_, false, true);

            hb.addTableCell(balance, true, true, true, (amt<0.0 ? wxT("RED") : wxT("BLACK")));
            hb.endTableRow();
		}
    }

    wxString payeetotalStr;
    wxString positiveTotalStr;
    wxString negativeTotalStr;
    wxString colorStr = (total<0.0 ? wxT("RED") : wxT("BLACK"));
    mmex::formatDoubleToCurrency(total, payeetotalStr);
    mmex::formatDoubleToCurrency(positiveTotal, positiveTotalStr);
    mmex::formatDoubleToCurrency(negativeTotal, negativeTotalStr);

    hb.addRowSeparator(2);
    hb.startTableRow();
    hb.addTableCell(_("Income:"),false, true, true, wxT("BLACK"));
    hb.addTableCell(positiveTotalStr, true, false, true, wxT("BLACK"));
	hb.endTableRow();
    hb.startTableRow();
    hb.addTableCell(_("Expenses:"),false, true, true, wxT("BLACK"));
    hb.addTableCell(negativeTotalStr, true, false, true, wxT("RED"));
	hb.endTableRow();
    hb.addRowSeparator(2);
    hb.startTableRow();
    hb.addTableCell(_("Payees Total: "),false, true, true, wxT("BLACK"));
    hb.addTableCell(payeetotalStr, true, false, true, colorStr);
	hb.endTableRow();

    hb.endTable();
	hb.endCenter();

    hb.end();

    gg.init(valueList);
    gg.Generate(title_);

    return hb.getHTMLText();
}
