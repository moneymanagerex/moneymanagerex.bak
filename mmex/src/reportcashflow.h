#ifndef _MM_EX_REPORTCASHFLOW_H_
#define _MM_EX_REPORTCASHFLOW_H_

#include "reportbase.h"

#include <wx/datetime.h>
#include <vector>

class mmCoreDB;
class wxArrayString;

class mmReportCashFlow : public mmPrintableBase 
{
public:
    mmReportCashFlow(mmCoreDB* core, const wxArrayString* accountArray = 0);

    wxString getHTMLText();
    void activateTermAccounts(); 
    void showTermAccountsHeading(); 

private:
   
   struct mmRepeatForecast {
      wxDateTime date;
      double amount;
   };

   typedef std::vector<mmRepeatForecast> forecastVec;
   std::vector<forecastVec> bdForecastVec;

   const wxArrayString* accountArray_;
   bool activeTermAccounts_;
   bool termAccountsHeading_;
};

#endif // _MM_EX_REPORTCASHFLOW_H_
