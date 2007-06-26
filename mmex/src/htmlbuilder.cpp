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
 /*******************************************************/
#include "htmlbuilder.h"
#include "util.h"

void mmHTMLBuilder::init()
{
    html += wxT("<html><head><meta http-equiv=\"content-type\" content=\"text/html;charset=UTF-8\"><title>");  
    html += mmIniOptions::appName_;
    html += wxT(" - ");
	html += _("Report");
	html += wxT("</title></head><body bgcolor=\"#ffffff\" text=\"#000000\" link=\"#0000cc\" vlink=\"#551a8b\" alink=\"#ff0000\">");

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
			addHeader(5, mmIniOptions::userNameString_);
			endTableCell();
		endTableRow();
		addRowSeparator(cols);
	endTable();
}

void mmHTMLBuilder::end()
{
    html += wxT("</body></html>");
}

void mmHTMLBuilder::addHeader(int level, const wxString& header)
{
    html += wxT("<h");
    html += wxString::Format(wxT("%d"), level);
    html += wxT(">");
    html += header;
    html += wxT("</h");
    html += wxString::Format(wxT("%d"), level);
    html += wxT(">");
}

void mmHTMLBuilder::addParaText(const wxString& text)
{
    html += wxT("<p>");
    html += text;
    html += wxT("</p>");
}

void mmHTMLBuilder::addLineBreak()
{
    html += wxT("<br>");
}

void mmHTMLBuilder::addHorizontalLine()
{
    html += wxT("<hr size=\"2\" width=\"95%\" align=\"left\">");
}

void mmHTMLBuilder::addImage(const wxString& src)
{
	html += wxT("<img src=\"");
	html += src;
	html += wxT("\" border=\"0\">");
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
	{
		html += wxT(" width=\"");
		html += width;
		html += wxT("\"");
	}
	if(!valign.empty())
	{
		html += wxT(" valign=\"");
		html += valign;
		html += wxT("\"");
	}
	html += wxT(">");
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
	{
		html += wxT(" width=\"");
		html += width;
		html += wxT("\"");
	}
	html += wxT(">");
}

void mmHTMLBuilder::addRowSeparator(int cols) {

	html += wxT("<tr bgcolor=\"#000000\" height=\"1\"><td height=\"1");
	if(cols > 1)
	{
		html += wxT("\" colspan=\"");
		html += wxString::Format(wxT("%d"), cols);
	}
    html += wxT("\"></td></tr>");
	bgswitch = true;
}

void mmHTMLBuilder::addTotalRow(const wxString& caption, int cols, const wxString& value) {
	html += wxT("<tr bgcolor=\"#ffffff\"><td");
	if(cols - 1 > 1) {
		html += wxT(" colspan=\"");
		html += wxString::Format(wxT("%d"), cols - 1);
		html += wxT("\"");
	}
	html += wxT("><b><i>&nbsp;&nbsp;");
	html += caption;
	html += wxT("</i></b></td><td align=\"right\"><b><i>");
	html += value;
	html += wxT("</i></b></td></tr>");
}

void mmHTMLBuilder::addTotalRow(const wxString& caption, int cols, std::vector<wxString>& data) {
	html += wxT("<tr bgcolor=\"#ffffff\"><td");
	if(cols - data.size() > 1) {
		html += wxT(" colspan=\"");
		html += wxString::Format(wxT("%d"), cols - data.size());
		html += wxT("\"");
	}
	html += wxT("><b><i>&nbsp;&nbsp;");
	html += caption;
	for (unsigned long idx = 0; idx < data.size(); idx++)
	{
		html += wxT("</i></b></td><td align=\"right\"><b><i>");
		html += data[idx];
	}
	html += wxT("</i></b></td></tr>");
}

void mmHTMLBuilder::addTableHeaderRow(const wxString& value, int cols) {
	html += wxT("<tr><th align=\"left\" valign=\"center\" bgcolor=\"#d5d6de\"");
	if(cols > 1)
	{
		html += wxT(" colspan=\"");
		html += wxString::Format(wxT("%d"), cols);
		html += wxT("\"");
	}
	html += wxT("><b>&nbsp;");
    html += value;
	html += wxT("</b></th></tr>");
	bgswitch = true;
}

void mmHTMLBuilder::addTableHeaderCell(const wxString& value) {
	html += wxT("<th align=\"left\" valign=\"center\" bgcolor=\"#d5d6de\"><b>&nbsp;");
    html += value;
	html += wxT("</b></th>");
	bgswitch = false;
}

void mmHTMLBuilder::addTableCell(const wxString& value, bool numeric, bool italic, bool bold, const wxString& fontColor) {

	html += wxT("<td align=\"");
	if(numeric)
	{
		html += wxT("right");
	}
	else
	{
		html += wxT("left");
	}
	html += wxT("\" bgcolor=\"");
	
	if(bgswitch)
	{
		html += wxT("#ffffff");
	}
	else
	{
		html += wxT("#e1edfb");
	}
    html += wxT("\">");

	if(!fontColor.empty())
	{
		html += wxT("<font color=\"");
		html += fontColor;
		html += wxT("\">");
	}
	
	if(bold)
	{
		html += wxT("<b>");
	}
	if(italic)
	{
		html += wxT("<i>");
	}
	html += value;
	if(italic)
	{
		html += wxT("</i>");
	}
	if(bold)
	{
		html += wxT("</b>");
	}
	if(!fontColor.empty())
	{
		html += wxT("</font>");
	}
	html += wxT("</td>");
}

void mmHTMLBuilder::addTableCellLink(const wxString& href, const wxString& value, bool numeric, bool italic, bool bold, const wxString& fontColor)
{
	addTableCell(wxT("<a href=\"") + href + wxT("\">") + value + wxT("</a>"), numeric, italic, bold, fontColor);
}

void mmHTMLBuilder::addTableHeaderCellLink(const wxString& href, const wxString& value) {
	addTableHeaderCell(wxT("<a href=\"") + href + wxT("\">") + value + wxT("</a>"));
}

void mmHTMLBuilder::addTableHeaderRowLink(const wxString& href, const wxString& value, int cols) {
	addTableHeaderRow(wxT("<a href=\"") + href + wxT("\">") + value + wxT("</a>"), cols);
}

void mmHTMLBuilder::endTable()
{
	html += wxT("</table>");
}

void mmHTMLBuilder::endTableRow()
{
	html += wxT("</tr>");
	bgswitch = !bgswitch;
}

void mmHTMLBuilder::endTableCell()
{
	html += wxT("</td>");
}

