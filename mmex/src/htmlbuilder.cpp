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
#include "htmlbuilder.h"
#include "util.h"

void mmHTMLBuilder::init()
{
    html_ += wxT("<html><head><meta http-equiv=\"content-type\" content=\"text/html; \
                 charset=UTF-8\"><title>");  
    html_ += mmIniOptions::appName_;
    html_ += wxT(" - Report</title>");
    html_ += wxT("</head>           \
        <body bgcolor=#FFFFFF text=#000000 link=#0000cc \
        vlink=#551a8b alink=#ff0000>");

   html_ += wxT("<table cellspacing=\"0\" cellpadding=\"1\" border=\"0\">");
   html_ += wxT("<tr><td>");
   if (mmIniOptions::enableCustomLogo_)
        html_ += wxT("<img src=\"")+ mmIniOptions::logoName_ + wxT("\"></img>");
   html_ += wxT("</td><td width=\"300\">");
   addHeader(5, mmIniOptions::userNameString_);
   html_ += wxT("</td></tr></table>");

}

void mmHTMLBuilder::end()
{
    html_ += wxT("</body></html>");
}

void mmHTMLBuilder::addLineBreak()
{
    html_ += wxT("<br>");
}

void mmHTMLBuilder::addHorizontalLine()
{
      html_ += wxT("<hr>");
}

void mmHTMLBuilder::beginTable(const wxString& options)
{
      html_ += wxT("<table cellspacing=\"1\" cellpadding=\"3\" border=\"1\">");
}
 
void mmHTMLBuilder::endTable()
{
    html_ += wxT("</table>");
}

void mmHTMLBuilder::addRow(std::vector<wxString>& data, const wxString& options, 
                           const wxString& tdoptions)
{
    if (options == wxT(""))
        html_ += wxT("<tr>");
    else
    {
        html_ += wxT("<tr ");
        html_ += options;
        html_ += wxT(" >");
    }
    if (tdoptions == wxT(""))
    {
        for (unsigned long idx = 0; idx < data.size(); idx++)
        {
            html_ += wxT("<td>");
            html_ += data[idx];
            html_ += wxT("</td>");
        }
    }
    else
    {
        for (unsigned long idx = 0; idx < data.size(); idx++)
        {
            html_ += wxT("<td ");
            html_ += tdoptions;
            html_ += wxT(" >");
            html_ += data[idx];
            html_ += wxT("</td>");
        }
    }

    html_ += wxT("</tr>");
}

void mmHTMLBuilder::addTableHeaderRow(std::vector<wxString>& data,
                                      const wxString& options,
                                      const wxString& tdoptions)
{
     if (options == wxT(""))
        html_ += wxT("<tr>");
    else
    {
        html_ += wxT("<tr ");
        html_ += options;
        html_ += wxT(" >");
    }
    for (unsigned long idx = 0; idx < data.size(); idx++)
    {
        html_ += wxT("<th ");
        html_ += tdoptions;
        html_ += wxT(" ><b>");
        html_ += data[idx];
        html_ += wxT("</b></th>");
    }

    html_ += wxT("</tr>");
}

void mmHTMLBuilder::addHeader(int level, const wxString& header)
{
    html_ += wxT("<H");
    html_ += wxString::Format(wxT("%d"), level);
    html_ += wxT(">");
    html_ += header;
    html_ += wxT("</H");
    html_ += wxString::Format(wxT("%d"), level);
    html_ += wxT(">");
}

void mmHTMLBuilder::addParaText(const wxString& text)
{
    html_ += wxT("<p>");
    html_ += text;
    html_ += wxT("</p>");
}

void mmHTMLBuilder::addHTML(const wxString& text)
{
    html_ += text;
}

