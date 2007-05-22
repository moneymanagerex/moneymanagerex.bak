#include "mmgraphgenerator.h"

#ifndef _MM_EX_GRAPHINCEXPENSESMONTH_H_
#define _MM_EX_GRAPHINCEXPENSESMONTH_H_

class mmGraphIncExpensesMonth : public mmGraphGenerator
{
public:
    mmGraphIncExpensesMonth()
        : mmGraphGenerator(wxT("inc_expenses_month"), 
                           wxT("inc_expenses_month.png"))
    {
    }

    virtual void init(double income, double expenses)
    {
       if (!isGraphEnabled())
          return;

        wxString fileContents = wxT("");  
        {
            wxFileInputStream input( fullScriptTemplatePathRelative_ );
            wxTextInputStream text( input );

            while (!input.Eof() )
            {
                wxString line = text.ReadLine();
                if (!line.IsEmpty())
                {
                    fileContents += line;
                    fileContents += wxT("\n");
                }
                else
                {
                    fileContents += wxT("\n");
                }
            }
        }
        wxString displayIncString = wxString::Format(wxT("%.0f"), income);
        wxString displayExpString = wxString::Format(wxT("%.0f"), expenses);

        int maxVal = (income > expenses)? (int)income + 200 : (int)expenses + 200;
        wxString displayMaxValString = wxString::Format(wxT("%d"), maxVal);

        fileContents.Replace(wxT("$INCOME"), displayIncString);
        fileContents.Replace(wxT("$EXPENSES"), displayExpString);
        fileContents.Replace(wxT("$MAX"), displayMaxValString);
        

        {
            wxFileOutputStream output( fullScriptPathRelative_ );
            wxTextOutputStream text( output );
            text << fileContents;
        }
    }
};

#endif