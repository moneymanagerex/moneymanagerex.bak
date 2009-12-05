#ifndef _MM_EX_REPORTINCEXP_H_
#define _MM_EX_REPORTINCEXP_H_

#include "reportbase.h"
#include <wx/datetime.h>

class mmCoreDB;

class mmReportIncomeExpenses : public mmPrintableBase 
{
public:
    mmReportIncomeExpenses(mmCoreDB* core, bool ignoreDate, wxDateTime dtBegin, wxDateTime dtEnd);

    wxString getHTMLText();

private:
    mmCoreDB* core_;
    wxDateTime dtBegin_;
    wxDateTime dtEnd_;
    bool ignoreDate_;


};

#endif // _MM_EX_REPORTINCEXP_H_
