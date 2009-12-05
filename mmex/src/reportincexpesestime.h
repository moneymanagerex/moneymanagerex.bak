#ifndef _MM_EX_REPORTINCEXPENSESTIME_H_
#define _MM_EX_REPORTINCEXPENSESTIME_H_

#include "reportbase.h"

class mmCoreDB;

class mmReportIncExpensesOverTime : public mmPrintableBase 
{
public:
    mmReportIncExpensesOverTime(mmCoreDB* core, int year);

    wxString getHTMLText();

private:
    mmCoreDB* core_;
    int year_;
};

#endif //_MM_EX_REPORTINCEXPENSESTIME_H_
