#include "mmgraphgenerator.h"
#include "mmex.h"

#ifndef _MM_EX_GRAPHPIE_H_
#define _MM_EX_GRAPHPIE_H_

bool sortValueList( ValuePair& elem1, ValuePair& elem2 )
{
    return fabs(elem1.amount) > fabs(elem2.amount);
}

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
            std::sort(valueList.begin(), valueList.end(), sortValueList);
            for (int idx = 0; idx < (int)valueList.size(); idx++)
            {
                if (idx < 10)
                {
                    labelStr += wxT("\"");     
                    labelStr += valueList[idx].label.Left(20);
                    labelStr += wxT("\"(@@PCT%)\n"); 
                }

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