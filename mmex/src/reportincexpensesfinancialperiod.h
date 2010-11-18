///////////////////////////////////////////////////////////////////////////////
///     This class tabulates the income and expense for a 12 month period. 
///     It is similar to the class: mmReportIncExpensesOverTime except that it
///     covers a financial year from 1 July YYYY to 30 June YYYY over 2 years.
///
///////////////////////////////////////////////////////////////////////////////
#ifndef _MM_EX_REPORTINCEXPENSESFINANCIALPERIOD_H_
#define _MM_EX_REPORTINCEXPENSESFINANCIALPERIOD_H_

#include "reportbase.h"

class mmCoreDB;

class mmReportIncExpensesOverFinancialPeriod : public mmPrintableBase 
{
public:
	mmReportIncExpensesOverFinancialPeriod(mmCoreDB* core, int year);

    wxString getHTMLText();

private:
    mmCoreDB* core_;
    int year_;
};

#endif //_MM_EX_REPORTINCEXPENSESFINANCIALPERIOD_H_
