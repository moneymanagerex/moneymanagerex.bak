#ifndef _MM_EX_REPORTCASHFLOW_H_
#define _MM_EX_REPORTCASHFLOW_H_

#include "reportbase.h"

#include <wx/datetime.h>
#include <vector>

class wxArrayString;

class mmReportCashFlow : public mmPrintableBase 
{
public:
    mmReportCashFlow(mmCoreDB* core, const wxArrayString* accountArray = 0);

    wxString getHTMLText();
	
    void activateTermAccounts(); 
	void activateBankAccounts();

private:
   
   struct mmRepeatForecast 
   {
      wxDateTime date;
      double amount;
   };

   typedef std::vector<mmRepeatForecast> forecastVec;
   std::vector<forecastVec> bdForecastVec;

   const wxArrayString* accountArray_;
   bool activeTermAccounts_;
   bool activeBankAccounts_;
};

#endif // _MM_EX_REPORTCASHFLOW_H_
