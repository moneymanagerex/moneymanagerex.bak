#ifndef _MM_EX_REPORTBUDGETINGSETUP_H_
#define _MM_EX_REPORTBUDGETINGSETUP_H_

#include "reportbase.h"

class mmCoreDB;
class wxSQLite3Database;


class mmReportBudgetingSetup : public mmPrintableBase 
{
public:
    mmReportBudgetingSetup(mmCoreDB* core, int year);

    wxString getHTMLText();

private:
    mmCoreDB* core_;
    wxSQLite3Database* db_;
    int year_;
};

#endif // _MM_EX_REPORTBUDGETINGSETUP_H_
