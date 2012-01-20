#ifndef _MM_EX_REPORTINCEXP_H_
#define _MM_EX_REPORTINCEXP_H_

#include "reportbase.h"
#include <wx/datetime.h>

class mmReportIncomeExpenses : public mmPrintableBase 
{
public:
    mmReportIncomeExpenses(mmCoreDB* core, bool ignoreDate, wxDateTime dtBegin, wxDateTime dtEnd, wxString title);

    wxString getHTMLText();

private:
    wxDateTime dtBegin_;
    wxDateTime dtEnd_;
    bool ignoreDate_;
    wxString title_;

};

#endif // _MM_EX_REPORTINCEXP_H_
