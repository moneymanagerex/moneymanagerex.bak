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

namespace tags
{
static const char HTML[] = 
    "<html>\n<head>\n<meta http-equiv=\"Content-Type\" content=\"text/html; charset=UTF-8\" />"
    "<title>%s - Report</title>\n""</head>";
static const wxString TABLE_ROW = "<tr bgcolor=\"%s\" >";
static const wxString TABLE_CELL = "<td width=\"%s\" >";
static const wxString TABLE_CELL_SPAN = "<td colspan=\"%d\" >";
static const wxString TABLE_CELL_LINK = "<a href=\"%s\">%s</a>\n";
static const wxString TABLE_HEADER_CELL_LINK = "<a href=\"%s\">%s</a>\n";
static const wxString TABLE_HEADER_ROW_LINK = "<a href=\"%s\">%s</a>\n";
static const wxString TABLE_CELL_RIGHT_BI = "<td nowrap align=\"right\"><font size=\"%d\"><b><i>%s</i></b></font></td>\n";
static const wxString HEADER = "<font size=\"%i\"><b>%s</b></font><br>\n";
static const wxString HEADER_ITALIC = "<font size=\"%i\"><i>%s</i></font>\n";
static const wxString PARAGRAPH = "<p><font size=\"%d\">%s</font></p>\n";
static const wxString BI = "<b><i>%s</i></b>";
static const wxString BOLD = "<b>%s</b>";
static const wxString ITALIC = "<i>%s</i>";
static const wxString FONT_COLOR = "<font color=\"%s\">";
static const wxString FONT_SIZE = "<font size=\"%d\">";
static const wxString HOR_LINE = "<hr size=\"%d\">\n";
static const wxString IMAGE = "<img src=\"%s\" border=\"0\">";
static const wxString END = "\n</font></body>\n</html>\n";
static const wxString BR = "<br>\n";
static const wxString CENTER = "<center>";
static const wxString CENTER_END = "</center>";
}

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
    void addHeader(const int level, const wxString& header);
    void addHeaderItalic(const int level, const wxString& header);
    void addDateNow();

    /** Create an HTML paragrapth */
    void addParaText(const wxString& text);
    void addText(const wxString& text);

    /** Create an HTML line break */
    void addLineBreak();

    /** Create an HTML HorizontalLine */
    void addHorizontalLine(const int size = 0);

    /** Create an HTML Image tag */
    void addImage(const wxString& src);

    /** Centers the content from this point on */
    void startCenter();

    /** Stops the centering of content */
    void endCenter();

    /** Start a table element */
    void startTable(const wxString& width = wxGetEmptyString()
        , const wxString& valign = wxGetEmptyString(), const wxString& border = wxGetEmptyString());

    /** Starts a table row */
    void startTableRow(const wxString& custom_color = wxGetEmptyString());

    /** Starts a table cell (use only if want to nest other elements inside */
    void startTableCell(const wxString& width = "");

    /** Add a special row that is a separator, cols is the number of columns the row has to spread along */
    void addRowSeparator(const int cols);

    /** Add a special row that will format total values */
    void addTotalRow(const wxString& caption, const int cols, const wxString& value);
    void addTotalRow(const wxString& caption, int cols, double value);

    /** Add a special row that will format total values */
    void addTotalRow(const wxString& caption, const int cols, const std::vector<wxString>& data);
    void addTotalRow(const wxString& caption, int cols, const std::vector<double>& data);

    /** Add a Table header cell */
    void addTableHeaderCell(const wxString& value, const bool& numeric = false);

    /** Add a Table header cell link */
    void addTableHeaderCellLink(const wxString& href, const wxString& value);

    /** Add a Table header row */
    void addTableHeaderRow(const wxString& value, const int cols);

    /** Add a Table header row with link */
    void addTableHeaderRowLink(const wxString& href, const wxString& value, const int cols);

    void addMoneyCell(double amount, bool color = true);
    void addMoneyCell(double amount, const wxString& color);
    void addTableCell(const wxDateTime& date);

    /** Add a Cell value */
    void addTableCell(const wxString& value
        , const bool& numeric = false
        , const bool& italic = false
        , const bool& bold = false
        , const wxString& fontColor = wxGetEmptyString());

    /** Add a Cell value */
    void addTableCellLink(const wxString& href, const wxString& value
        , const bool& numeric = false
        , const bool& italic = false
        , const bool& bold = false
        , const wxString& fontColor = wxGetEmptyString());

    void endTable();
    void endTableRow();
    void endTableCell();

    wxString getHTMLText() const { return html_; }

    void addHTML(const wxString& raw) { html_ += raw; }
    void DisplayDateHeading(const wxDateTime& startYear, const wxDateTime& endYear, bool withDateRange = true);

private:
    wxString html_;

    int font_size_;
    struct color_ {
        wxString color0;
        wxString color1;
        wxString table_header;
        wxString link;
        wxString vlink;
        wxString alink;
        bool bgswitch;
    } color_;
    struct today_ {
        wxDateTime date;
        wxString date_str;
        wxString todays_date;
    } today_;
};

#endif
