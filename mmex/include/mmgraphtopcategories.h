#include "mmgraphgenerator.h"
#include "mmex.h"

#ifndef _MM_EX_GRAPHTOPCATEGORIES_H_
#define _MM_EX_GRAPHTOPCATEGORIES_H_

class mmGraphTopCategories : public mmGraphGenerator
{
public:
    mmGraphTopCategories(std::vector<mmGUIFrame::CategInfo>& categList)
        : mmGraphGenerator(wxT("top_categories"), 
                           wxT("top_categories.png"))
    {
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
        int maxVal = 0;
        for (int idx = 0; idx < (int)categList.size(); idx++)
        {
            if (idx > 5)
                break;
            dataStr += wxT("\"");     
            dataStr += categList[idx].categ;
            dataStr += wxT("\" "); 
            dataStr += wxString::Format(wxT("%.0f"), fabs(categList[idx].amount));
            if (maxVal < fabs(categList[idx].amount))
                maxVal = fabs(categList[idx].amount);
            dataStr += wxT("\n");
        }
        
        wxString maxStr = wxT("100");
        if (categList.size() > 0)
            maxStr = wxString::Format(wxT("%d"), maxVal);
        
        fileContents.Replace(wxT("$MAX"), maxStr);
        fileContents.Replace(wxT("$DATA"), dataStr);

        {
            wxFileOutputStream output( fullScriptPathRelative_ );
            wxTextOutputStream text( output );
            text << fileContents;
        }
    }
};

#endif