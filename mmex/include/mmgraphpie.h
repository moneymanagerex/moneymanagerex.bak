#include "mmgraphgenerator.h"
#include "mmex.h"

#ifndef _MM_EX_GRAPHPIE_H_
#define _MM_EX_GRAPHPIE_H_

class mmGraphPie : public mmGraphGenerator
{
public:
    mmGraphPie()
        : mmGraphGenerator(wxT("pie_chart"), 
                           wxT("pie_chart.png"))
    {

    }

    virtual void init(std::vector<ValuePair>& valueList)
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
       
        wxString dataStr;
        wxString labelStr;
        if (valueList.size() == 0)
        {
            labelStr += wxT("\"");     
            labelStr += wxT("No Payees");
            labelStr += wxT("\"\\n(@@PCT%)\n"); 

            dataStr += wxString::Format(wxT("%d"), 100);
            dataStr += wxT("\n");
        }
        else
        {
            for (int idx = 0; idx < (int)valueList.size(); idx++)
            {
                labelStr += wxT("\"");     
                labelStr += valueList[idx].label;
                labelStr += wxT("\"\\n(@@PCT%)\n"); 

                dataStr += wxString::Format(wxT("%.0f"), fabs(valueList[idx].amount));
                dataStr += wxT("\n");
            }
        }
       
        fileContents.Replace(wxT("$LABELS"), labelStr);
        fileContents.Replace(wxT("$DATA"), dataStr);

        {
            wxFileOutputStream output( fullScriptPathRelative_ );
            wxTextOutputStream text( output );
            text << fileContents;
        }
    }
};

#endif