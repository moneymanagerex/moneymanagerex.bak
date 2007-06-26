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
 /*******************************************************/

#include "mmgraphgenerator.h"
#include "util.h"

wxString mmGraphGenerator::envString_ = wxT("sdfhsjdhfjhfdsjhfsd");

mmGraphGenerator::mmGraphGenerator(const wxString& scriptName, 
                                   const wxString& outFileName)
     : scriptName_(scriptName),
       outFileName_(outFileName)
{
   wxString basePath = mmGetBaseWorkingPath() + wxT("\\graphs");
   basePath += wxT("\\");

   fullScriptTemplatePathRelative_  = wxT("graphs\\") + scriptName_ + wxT(".tmpl");
   fullScriptPathRelative_  = wxT("graphs\\") + scriptName_ + wxT(".txt");

   fullscriptPath_ = wxT("\"") + basePath + scriptName_ + wxT(".txt") + wxT("\"");
   
   fulloutfileName_ = wxT("\"") + basePath + outFileName_ + wxT("\"");
   ploticusName_  = wxT("\"") + basePath + wxT("pl.exe") + wxT("\"");
  

   htmlString_ = wxT("<img src=\"graphs\\") + outFileName + wxT("\"></img>");
}

mmGraphGenerator::~mmGraphGenerator()
{}

const wxString& mmGraphGenerator::outputFile() 
{ 
   return outFileName_; 
}

bool mmGraphGenerator::isGraphEnabled()
{
   return mmIniOptions::enableGraphs_;
}

wxString mmGraphGenerator::getHTML()
{
   if (mmIniOptions::enableGraphs_)
      return htmlString_;
   else
      return wxT("");
}

void mmGraphGenerator::setEnv()
{
   mmGraphGenerator::envString_ = mmGetBaseWorkingPath() + wxT("\\graphs");
   bool returnVal = wxSetEnv(wxT("GDFONTPATH"), mmGraphGenerator::envString_);
}

bool mmGraphGenerator::checkGraphFiles()
{
    if (mmOptions::language.size() > 0 && mmOptions::language != wxT("english"))
    {
       wxString fontPathName  = wxT("graphs\\Cyberbit.ttf"); 
       if (!wxFileName::FileExists(fontPathName))
            return false;
    }

    return true;
}

void mmGraphGenerator::generate()
{
   if (!mmIniOptions::enableGraphs_)
      return;

   wxString fullExecPath = ploticusName_ 
                            + wxT(" -png -o ") 
                            + fulloutfileName_ + wxT(" ")
                            + fullscriptPath_;
   if (mmOptions::language.size() > 0 && mmOptions::language != wxT("english"))
   {
        fullExecPath += wxT(" -font Cyberbit.ttf");
   }

   wxArrayString output, errors;
   setEnv(); 

   int code = wxExecute(fullExecPath, output, errors);
   if (code != 0)
   {
      mmShowErrorMessage(0, _("Failed to launch graphing system! Disabling graphs."), _("Graph Error.."));
      mmIniOptions::enableGraphs_ = false;
   }
}