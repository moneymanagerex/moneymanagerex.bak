#ifndef _MM_EX_CUSTOMSQLSTATS_H_
#define _MM_EX_CUSTOMSQLSTATS_H_

#include "defs.h"
#include "htmlbuilder.h"
#include "reportbase.h"
#include "util.h"
#include "dbwrapper.h"
#include "budgetingpanel.h"

class mmCustomSQLStats : public mmPrintableBase 
{
public:
    mmCustomSQLStats(mmCoreDB* core, const wxString& a_sqlQuery) 
        : core_(core),
          sqlQuery(a_sqlQuery)
    {
    }

    virtual wxString getHTMLText()
    {
        mmHTMLBuilder hb;
        hb.init();
        hb.addHeader(3, _("Custom SQL Report "));

        wxDateTime now = wxDateTime::Now();
        wxString dt = wxT("Today's Date: ") + mmGetNiceDateString(now);
        hb.addHeader(7, dt);
        hb.addLineBreak();
        hb.addLineBreak();

		hb.startCenter();
        hb.startTable(wxT("50%"));

        mmBEGINSQL_LITE_EXCEPTION;
        wxString bufSQL = wxString::Format(sqlQuery);
        wxSQLite3ResultSet q1 = core_->db_->ExecuteQuery(bufSQL);

        int numCols = q1.GetColumnCount();
        hb.startTableRow();
        for (int idx = 0; idx < numCols; idx++)
        {
    	    hb.addTableHeaderCell(q1.GetColumnName(idx));
        }
        hb.endTableRow();

       
        while (q1.NextRow())
        {
            hb.startTableRow();
            for (int idx = 0; idx < numCols; idx++)
            {
                hb.addTableCell(q1.GetAsString(idx));
            }
            hb.endTableRow();
        }
        
        q1.Finalize();
        }                                     
        catch (wxSQLite3Exception& e)        
        {    
            mmShowErrorMessage(NULL, e.ErrorCodeAsString(e.GetErrorCode()), 
                wxT("Error Executing SQL"));
        }      
        

        hb.endTable();
		hb.endCenter();

        hb.end();
        return hb.getHTMLText();
    }

private:
    mmCoreDB* core_;
    wxString sqlQuery;
};

#endif
