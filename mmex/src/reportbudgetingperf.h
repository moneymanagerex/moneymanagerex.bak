#ifndef _MM_EX_REPORTBUDGETINGPERFORMANCE_H_
#define _MM_EX_REPORTBUDGETINGPERFORMANCE_H_

#include "reportbase.h"

class mmCoreDB;
class wxSQLite3Database;


class mmReportBudgetingPerformance : public mmPrintableBase 
{
public:
    mmReportBudgetingPerformance(mmCoreDB* core, int year);

    wxString getHTMLText();

private:
    mmCoreDB* core_;
    wxSQLite3Database* db_;
    int year_;
};

#endif // _MM_EX_REPORTBUDGETINGPERFORMANCE_H_
