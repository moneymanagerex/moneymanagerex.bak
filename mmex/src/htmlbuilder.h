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

#ifndef _MM_EX_HTMLBUILDER_H_
#define _MM_EX_HTMLBUILDER_H_

#include "defs.h"
#include <vector>

class mmHTMLBuilder
{
public:
    mmHTMLBuilder();
    ~mmHTMLBuilder() {}

public:
	/** Starts a new HMTL report */
	void init();
	
	/** Closes the HMTL report */
	void end();
    
	/** Clears the current HTML document */
	void clear() { html_ = wxGetEmptyString(); }
	
	/** Create an HTML header and returns as a wxString */
    void addHeader(int level, const wxString& header);
    void addHeaderItalic(int level, const wxString& header);
    void addDateNow();
	
	/** Create an HTML paragrapth */
    void addParaText(const wxString& text);
	
	/** Create an HTML line break */
    void addLineBreak();
	
	/** Create an HTML HorizontalLine */
    void addHorizontalLine(int size = 0);
	
	/** Create an HTML Image tag */
	void addImage(const wxString& src);
	
	/** Centers the content from this point on */
	void startCenter();
	
	/** Stops the centering of content */
	void endCenter();

	/** Start a table element */
    void startTable(const wxString& width = wxGetEmptyString(), const wxString& valign = wxGetEmptyString());
	
	/** Starts a table row */
    void startTableRow(const wxString& custom_color = wxGetEmptyString());
	
	/** Starts a table cell (use only if want to nest other elements inside */
	void startTableCell(const wxString& width = wxGetEmptyString());
	
	/** Add a special row that is a separator, cols is the number of columns the row has to spread along */
    void addRowSeparator(int cols);
	
	/** Add a special row that will format total values */
    void addTotalRow(const wxString& caption, int cols, const wxString& value);
	
	/** Add a special row that will format total values */
    void addTotalRow(const wxString& caption, int cols, std::vector<wxString>& data);

	/** Add a Table header cell */
    void addTableHeaderCell(const wxString& value, const bool numeric = false);
	
	/** Add a Table header cell link */
    void addTableHeaderCellLink(const wxString& href, const wxString& value);

	/** Add a Table header row */
    void addTableHeaderRow(const wxString& value, int cols);

	/** Add a Table header row with link */
    void addTableHeaderRowLink(const wxString& href, const wxString& value, int cols);

	/** Add a Cell value */
    void addTableCell(const wxString& value, bool numeric = false, bool italic = false, bool bold = false, const wxString& fontColor = wxGetEmptyString());
	
	/** Add a Cell value */
    void addTableCellLink(const wxString& href, const wxString& value, bool numeric = false, bool italic = false, bool bold = false, const wxString& fontColor = wxGetEmptyString());

    void endTable();
    void endTableRow();
    void endTableCell();
    
    wxString getHTMLText() const { return html_; }

    void addHTML(const wxString& raw) { html_ += raw; }
    
private:
    wxString html_;
	bool bgswitch_;
	wxString color0_;
	wxString color1_;
	int font_size_;
};

#endif
