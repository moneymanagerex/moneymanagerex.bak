#ifndef _MM_EX_REPORTBUDGETING_SETUP_H_
#define _MM_EX_REPORTBUDGETING_SETUP_H_

#include "reportbudget.h"
#include "mmcoredb.h"

class mmReportBudgetingSetup : public mmReportBudget 
{
public:
    mmReportBudgetingSetup(mmCoreDB* core, mmGUIFrame* mainFrame, int budgetYearID);

    wxString getHTMLText();

private:
    mmCoreDB* core_;
    wxSQLite3Database* db_;
    int budgetYearID_;

    wxString ActualAmountColour( mmBudgetEntryHolder& budEntry, bool total = false);

};

#endif // _MM_EX_REPORTBUDGETING_SETUP_H_
