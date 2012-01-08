#ifndef _MM_EX_REPORTBUDGETING_PERFORMANCE_H_
#define _MM_EX_REPORTBUDGETING_PERFORMANCE_H_

#include "reportbudget.h"
#include "mmcoredb.h"

class mmReportBudgetingPerformance : public mmReportBudget
{
public:
    mmReportBudgetingPerformance(mmCoreDB* core, mmGUIFrame* mainFrame, int budgetYearID);

    wxString getHTMLText();

private:
    mmCoreDB* core_;
    wxSQLite3Database* db_;
    int budgetYearID_;

    void DisplayEstimateMonths(mmHTMLBuilder& hb, mmBudgetEntryHolder& budgetEntry, int startMonth);
    void DisplayActualMonths(mmHTMLBuilder& hb, mmBudgetEntryHolder& budgetEntry, int startMonth, long startYear);

};

#endif // _MM_EX_REPORTBUDGETING_PERFORMANCE_H_
