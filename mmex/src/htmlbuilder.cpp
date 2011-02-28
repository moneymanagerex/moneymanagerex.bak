/*******************************************************
 Copyright (C) 2006 Madhan Kanagavel, Paulo Lopes

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

#include "htmlbuilder.h"
#include "util.h"
#include "constants.h"

mmHTMLBuilder::mmHTMLBuilder() {
	// init colors from config
	color0 = wxT("bgcolor=\"#");
	color0 += wxString::Format(wxT("%x"), (int) mmColors::listAlternativeColor0.Red());
	color0 += wxString::Format(wxT("%x"), (int) mmColors::listAlternativeColor0.Green());
	color0 += wxString::Format(wxT("%x"), (int) mmColors::listAlternativeColor0.Blue());
	color0 += wxT ("\"");

	color1 = wxT("bgcolor=\"#");
	color1 += wxString::Format(wxT("%x"), (int) mmColors::listAlternativeColor1.Red());
	color1 += wxString::Format(wxT("%x"), (int) mmColors::listAlternativeColor1.Green());
	color1 += wxString::Format(wxT("%x"), (int) mmColors::listAlternativeColor1.Blue());
	color1 += wxT ("\"");
	// init font size from config
	fontSize = wxT ("\"") + mmIniOptions::fontSize_ + wxT ("\"");
}

void mmHTMLBuilder::init()
{
    html += wxT("<html>\n<head>\n<meta http-equiv=\"content-type\" content=\"text/html;charset=UTF-8\"><title>");  
    html += mmex::getProgramName();
    html += wxT(" - ");
	html += _("Report");
	html += wxT("</title>\n</head><body bgcolor=\"#ffffff\" text=\"#000000\" link=\"#0000cc\" vlink=\"#551a8b\" alink=\"#ff0000\">");

    //if I need more space on the top of home page and reports I will delete user name from settings
    if (mmIniOptions::userNameString_ != wxT (""))
    {
    int cols = 1;
    startTable(wxT("100%"));
    startTableRow();
    if (mmIniOptions::enableCustomLogo_)
    {
        startTableCell();
        addImage(mmIniOptions::logoName_);
        endTableCell();
        cols++;
    }
    startTableCell();
    addHeader(2, mmIniOptions::userNameString_);
    endTableCell();
    endTableRow();
    endTable();
    addHorizontalLine(2);
    }
}

void mmHTMLBuilder::end()
{
    html += wxT("\n</body>\n</html>\n");
}

void mmHTMLBuilder::addHeader(int level, const wxString& header)
{
    html += wxT("<h") + wxString::Format(wxT("%d"), level) + wxT(">");
    html += header;
    html += wxT("</h") + wxString::Format(wxT("%d"), level) + wxT(">\n");
}

void mmHTMLBuilder::addParaText(const wxString& text)
{
    html += wxT("<p><font size=") + fontSize + wxT(">");
    html += text;
    html += wxT("</font></p>\n");
}

void mmHTMLBuilder::addLineBreak()
{
    html += wxT("<br>\n");
}

void mmHTMLBuilder::addHorizontalLine(int size)
{
	html += wxT("<hr");
	if(size > 0)
	html += wxT(" size=\"") + wxString::Format(wxT("%d"), size) + wxT("\"");
    html += wxT(">\n");
}

void mmHTMLBuilder::addImage(const wxString& src)
{
	if(!src.empty())
		html += wxT("<img src=\"") + src + wxT("\" border=\"0\">");
}

void mmHTMLBuilder::startCenter()
{
	html += wxT("<center>");
}

void mmHTMLBuilder::endCenter()
{
	html += wxT("</center>");
}

void mmHTMLBuilder::startTable(const wxString& width, const wxString& valign)
{
	html += wxT("<table cellspacing=\"1\"");
	if(!width.empty())
	html += wxT(" width=\"") + width + wxT("\"");
	if(!valign.empty()) 
	html += wxT(" valign=\"") + valign + wxT("\"");
	html += wxT(">\n");
	bgswitch = true;
}

void mmHTMLBuilder::startTableRow()
{
	html += wxT("<tr>");
}

void mmHTMLBuilder::startTableCell(const wxString& width)
{
	html += wxT("<td");
	if(!width.empty())
	html += wxT(" width=\"") + width + wxT("\"");
	html += wxT(">");
}

void mmHTMLBuilder::addRowSeparator(int cols) {

	html += wxT("<tr bgcolor=\"#000000\" height=\"1\"><td height=\"1");
	if(cols > 1)
	{
		html += wxT("\" colspan=\"");
		html += wxString::Format(wxT("%d"), cols);
	}
    html += wxT("\"></td></tr>\n");
	bgswitch = true;
}

void mmHTMLBuilder::addTotalRow(const wxString& caption, int cols, const wxString& value) {
	html += wxT("<tr bgcolor=\"#ffffff\"><td");
	if(cols - 1 > 1) 
	{
		html += wxT(" colspan=\"");
		html += wxString::Format(wxT("%d"), cols - 1);
		html += wxT("\"");
	}
	html += wxT("><font size=") + fontSize + wxT("><b><i>&nbsp;&nbsp;");
	html += caption;
	html += wxT("</i></b></font></td><td nowrap align=\"right\"><font size=") + fontSize + wxT("><b><i>");
	html += value;
	html += wxT("</i></b></font></td></tr>\n");
}

void mmHTMLBuilder::addTotalRow(const wxString& caption, int cols, std::vector<wxString>& data) {
	html += wxT("<tr bgcolor=\"#ffffff\"><td");
	if(cols - data.size() > 1) 
	{
		html += wxT(" colspan=\"");
		html += wxString::Format(wxT("%d"), cols - data.size());
		html += wxT("\"");
	}
	html += wxT("><font size=") + fontSize + wxT("><b><i>&nbsp;&nbsp;");
	html += caption;
	html += wxT("</i></b></font>");

	for (unsigned long idx = 0; idx < data.size(); idx++)
	{
		html += wxT("</td><td nowrap align=\"right\"><font size=") + fontSize + wxT("><b><i>");
		html += data[idx];
		html += wxT("</i></b></font>");
	}
	html += wxT("</td></tr>\n");
}

void mmHTMLBuilder::addTableHeaderRow(const wxString& value, int cols) {
	html += wxT("<tr><th align=\"left\" valign=\"center\" bgcolor=\"#d5d6de\"");
	if(cols > 1)
	{
		html += wxT(" colspan=\"");
		html += wxString::Format(wxT("%d"), cols);
		html += wxT("\"");
	}
	html += wxT("><font size=") + fontSize + wxT("><b>&nbsp;");
    html += value;
	html += wxT("</b></font></th></tr>\n");
	bgswitch = true;
}

void mmHTMLBuilder::addTableHeaderCell(const wxString& value) {
	html += wxT("<th nowrap align=\"left\" valign=\"center\" bgcolor=\"#d5d6de\">");
	html += wxT ("<font size=") + fontSize + wxT(">") + wxT("<b>&nbsp;") + value + wxT("</b></font></th>\n");
	bgswitch = false;
}

void mmHTMLBuilder::addTableCell(const wxString& value, bool numeric, bool italic, bool bold, const wxString& fontColor) {

    html += (numeric ? wxT("<td nowrap align=\"right\" ") : wxT("<td align=\"left\" ")) + (bgswitch ? color1 : color0) + wxT("><font size=") + fontSize;

    if(!fontColor.empty())
    html += wxT(" color=\"") + fontColor + wxT ("\"");

    html += wxT(">");

    if (!bold && !italic)    html += value + wxT("</font></td>\n");
    else if (bold && italic) html += wxT("<b><i>") + value + wxT("</i></b></font></td>\n");
    else if (bold)           html += wxT("<b>") + value + wxT("</b></font></td>\n");
    else if (italic)         html += wxT("<i>") + value + wxT("</i></font></td>\n");

}

void mmHTMLBuilder::addTableCellLink(const wxString& href, const wxString& value, bool numeric, bool italic, bool bold, const wxString& fontColor)
{
	addTableCell(wxT("<a href=\"") + href + wxT("\">") + value + wxT("</a>\n"), numeric, italic, bold, fontColor);
}

void mmHTMLBuilder::addTableHeaderCellLink(const wxString& href, const wxString& value) {
	addTableHeaderCell(wxT("<a href=\"") + href + wxT("\">") + value + wxT("</a>\n"));
}

void mmHTMLBuilder::addTableHeaderRowLink(const wxString& href, const wxString& value, int cols) {
	addTableHeaderRow(wxT("<a href=\"") + href + wxT("\">") + value + wxT("</a>\n"), cols);
}

void mmHTMLBuilder::endTable()
{
	html += wxT("</table>\n");
}

void mmHTMLBuilder::endTableRow()
{
	html += wxT("</tr>\n");
	bgswitch = !bgswitch;
}

void mmHTMLBuilder::endTableCell()
{
	html += wxT("</td>\n");
}

