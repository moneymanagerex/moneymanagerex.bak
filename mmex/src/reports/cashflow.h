#ifndef _MM_EX_REPORTCASHFLOW_H_
#define _MM_EX_REPORTCASHFLOW_H_

#include "../reportbase.h"
#include "../mmex.h"

class wxArrayString;

class mmReportCashFlow : public mmPrintableBase 
{
public:
    mmReportCashFlow(mmCoreDB* core, mmGUIFrame* frame, int cashflowreporttype, const wxArrayString* accountArray = 0);

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

   mmGUIFrame* frame_;
   const wxArrayString* accountArray_;
   bool activeTermAccounts_;
   bool activeBankAccounts_;
   int cashflowreporttype_;
};

class mmReportCashFlowAllAccounts : public mmReportCashFlow
{
public:
    mmReportCashFlowAllAccounts(mmCoreDB* core, mmGUIFrame* frame): mmReportCashFlow(core, frame, 0, 0)
    {
        this->activateBankAccounts();
        this->activateTermAccounts();
    }
};

class mmReportCashFlowBankAccounts : public mmReportCashFlow
{
public:
    mmReportCashFlowBankAccounts(mmCoreDB* core, mmGUIFrame* frame): mmReportCashFlow(core, frame, 0, 0)
    {
        this->activateBankAccounts();
    }
};

class mmReportCashFlowTermAccounts: public mmReportCashFlow
{
public:
    mmReportCashFlowTermAccounts(mmCoreDB* core, mmGUIFrame* frame): mmReportCashFlow(core, frame, 0, 0)
    {
        this->activateTermAccounts();
    }
};

#endif // _MM_EX_REPORTCASHFLOW_H_
