#ifndef _MM_EX_REPORTTRANSSTATS_H_
#define _MM_EX_REPORTTRANSSTATS_H_

#include "reportbase.h"

class mmReportTransactionStats : public mmPrintableBase
{
public:
    mmReportTransactionStats(mmCoreDB* core, int year);
    wxString getHTMLText();

private:
    int year_;
};

#endif // _MM_EX_REPORTTRANSSTATS_H_
