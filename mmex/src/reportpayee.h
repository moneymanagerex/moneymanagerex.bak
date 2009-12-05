#ifndef _MM_EX_REPORTPAYEE_H_
#define _MM_EX_REPORTPAYEE_H_

#include "reportbase.h"
#include <wx/datetime.h>

class mmCoreDB;

class mmReportPayeeExpenses : public mmPrintableBase 
{
public:
    mmReportPayeeExpenses(mmCoreDB* core, bool ignoreDate, wxDateTime dtBegin, wxDateTime dtEnd);

    wxString getHTMLText();

private:
    mmCoreDB* core_;

    wxDateTime dtBegin_;
    wxDateTime dtEnd_;
    
    bool ignoreDate_;
};

#endif //_MM_EX_REPORTPAYEE_H_
