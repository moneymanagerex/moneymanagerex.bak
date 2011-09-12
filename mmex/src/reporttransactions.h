/*******************************************************
 Copyright (C) 2006 Madhan Kanagavel

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
 ********************************************************/

#ifndef _MM_EX_REPORTTRANSACT_H_
#define _MM_EX_REPORTTRANSACT_H_

#include "reportbase.h"
#include <wx/datetime.h>

#include <vector>
#include <boost/shared_ptr.hpp>
#include "filtertransdialog.h"

class mmBankTransaction;
class mmCoreDB;

class mmReportTransactions : public mmPrintableBase 
{
public:
    mmReportTransactions(std::vector< boost::shared_ptr<mmBankTransaction> >* trans, 
        mmCoreDB* core, int refAccountID, wxString refAccountStr, mmFilterTransactionsDialog* transDialog);
   ~mmReportTransactions();

    wxString getHTMLText();

private:
    std::vector<boost::shared_ptr<mmBankTransaction> >* trans_;
    mmCoreDB* core_;
    wxDateTime dtBegin_;
    wxDateTime dtEnd_;
    bool ignoreDate_;
    int refAccountID_;
    wxString refAccountStr_;
    mmFilterTransactionsDialog* transDialog_;
};

#endif // _MM_EX_REPORTTRANSACT_H_
