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

#ifndef _MM_EX_HTMLBUILDER_H_
#define _MM_EX_HTMLBUILDER_H_

#include "defs.h"

class mmHTMLBuilder
{
public:
    mmHTMLBuilder() {}
    ~mmHTMLBuilder() {}

public: 
    void init();
    void end();
    void clear() { html_ = wxT(""); }
    void addHeader(int level, const wxString& header);
    void addParaText(const wxString& text);
    void addLineBreak();
    void addHorizontalLine();
    void beginTable(const wxString& options=wxT(""));
    void endTable();
    void addRow(std::vector<wxString>& data, const wxString& rowoptions=wxT(""), 
        const wxString& tdoptions=wxT(""));
    void addTableHeaderRow(std::vector<wxString>& data, 
        const wxString& options=wxT(""),const wxString& tdoptions=wxT(""));
    void addHTML(const wxString& text);
    wxString getHTMLText() { return html_; }

private:
    wxString html_;
};

#endif
