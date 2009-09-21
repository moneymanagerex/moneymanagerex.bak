#ifndef _MM_EX_REPORTPAYEE_H_
#define _MM_EX_REPORTPAYEE_H_

#include "defs.h"
#include "htmlbuilder.h"
#include "reportbase.h"
#include "util.h"
#include "dbwrapper.h"
#include "mmgraphpie.h"

class mmReportPayeeExpenses : public mmPrintableBase 
{
public:
    mmReportPayeeExpenses(mmCoreDB* core, bool ignoreDate, 
        wxDateTime dtBegin, 
        wxDateTime dtEnd) 
        : core_(core),
          ignoreDate_(ignoreDate),
          dtBegin_(dtBegin),
          dtEnd_(dtEnd)
    {
    }

    virtual wxString getHTMLText()
    {
        mmHTMLBuilder hb;
        hb.init();
        hb.addHeader(3, _("To Whom the Money Goes"));

        wxDateTime now = wxDateTime::Now();
        wxString dt = _("Today's Date: ") + mmGetNiceDateString(now);
        hb.addHeader(7, dt);
        hb.addLineBreak();
        hb.addLineBreak();

        wxDateTime tBegin = dtBegin_;
        if (!ignoreDate_)
        {
            wxString dtRange = _("From: ") 
                + mmGetNiceDateSimpleString(tBegin.Add(wxDateSpan::Day())) 
                + _(" To: ") 
                + mmGetNiceDateSimpleString(dtEnd_);
            hb.addHeader(7, dtRange);
            hb.addLineBreak();
        }

        hb.addLineBreak();

		hb.startCenter();

        int numPayees = (int)core_->payeeList_.payees_.size();
        // Add the graph
        mmGraphPie gg;
        if (numPayees)
           hb.addImage(gg.GetOutputFileName());

		hb.startTable(wxT("50%"));
		hb.startTableRow();
		hb.addTableHeaderCell(_("Payee"));
		hb.addTableHeaderCell(_("Amount"));
		hb.endTableRow();

        core_->currencyList_.loadBaseCurrencySettings();
        
        
        std::vector<ValuePair> valueList;
        for (int idx = 0; idx < numPayees; idx++)
        {
            wxString balance;
            double amt = core_->bTransactionList_.getAmountForPayee(
               core_->payeeList_.payees_[idx]->payeeID_, ignoreDate_, 
                dtBegin_, dtEnd_);

            mmCurrencyFormatter::formatDoubleToCurrency(amt, balance);

            if (amt != 0.0)
            {
                ValuePair vp;
                vp.label = core_->payeeList_.payees_[idx]->payeeName_;
                vp.amount = amt;
                valueList.push_back(vp);

				hb.startTableRow();
				hb.addTableCell(core_->payeeList_.payees_[idx]->payeeName_, false, true);

				if (amt < 0.0)
					hb.addTableCell(balance, true, true, true, wxT("#ff0000"));
				else
					hb.addTableCell(balance, true, false, true);

				hb.endTableRow();
			}
        }
        hb.endTable();
		hb.endCenter();

        hb.end();

        gg.init(valueList);
        gg.Generate(_("To Whom the Money Goes"));

        return hb.getHTMLText();
    }

private:
    mmCoreDB* core_;
    wxDateTime dtBegin_;
    wxDateTime dtEnd_;
    bool ignoreDate_;
};

#endif
