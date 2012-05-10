/*************************************************************************
 Copyright (C) 2006 Madhan Kanagavel
 Modified: Copyright (C) 2010 Stefano Giorgio      

 This program is free software; you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation; either version 2 of the License, or
 (at your option) any later version.

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with this program; if not, write to the Free Software
 Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

 Purpose:
 This class tabulates the income and expense for a 12 month period.
 It is similar to the class: mmReportIncExpensesOverTime except that it
 covers a financial year period defined by the user.
 Typically from 1 July YYYY to 30 June YYYY over 2 years but can be any
 period within year.
*************************************************************************/

#ifndef _MM_EX_REPORTINCEXPENSESFINANCIALPERIOD_H_
#define _MM_EX_REPORTINCEXPENSESFINANCIALPERIOD_H_

#include "reportbase.h"

class mmGUIFrame;
class mmReportIncExpensesOverFinancialPeriod : public mmPrintableBase 
{
public:
	mmReportIncExpensesOverFinancialPeriod(mmGUIFrame* frame, mmCoreDB* core, int year);

    wxString getHTMLText();

private:
    mmGUIFrame* frame_;
    int year_;
    int printYear_;
};

#endif //_MM_EX_REPORTINCEXPENSESFINANCIALPERIOD_H_
