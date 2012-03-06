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
	color0 = wxT("bgcolor=\"") + mmColors::listAlternativeColor0.GetAsString(wxC2S_HTML_SYNTAX) + wxT ("\"");
	color1 = wxT("bgcolor=\"") + mmColors::listAlternativeColor1.GetAsString(wxC2S_HTML_SYNTAX) + wxT ("\"");
	// init font size from config
	fontSize = wxT ("\"") + mmIniOptions::instance().fontSize_ + wxT ("\"");
}

void mmHTMLBuilder::init()
{
    html += wxT("<html>\n<head>\n<meta http-equiv=\"content-type\" content=\"text/html;charset=UTF-8\"><title>");  
    html += mmex::getProgramName();
    html += wxT(" - ");
	html += _("Report");
	html += wxT("</title>\n</head><body bgcolor=\"") + mmColors::listBackColor.GetAsString(wxC2S_HTML_SYNTAX) + wxT("\" text=\"#000000\" link=\"#0000cc\" vlink=\"#551a8b\" alink=\"#ff0000\">");
//	html += wxT("<tr bgcolor=\"") + mmColors::listBorderColor.GetAsString(wxC2S_HTML_SYNTAX) + wxT("\" height=\"1\"><td height=\"1");

    //if I need more space on the top of home page and reports I will delete user name from settings
    if (mmIniOptions::instance().userNameString_ != wxT (""))
    {
    int cols = 1;
    startTable(wxT("100%"));
    startTableRow();
    if (mmIniOptions::instance().enableCustomLogo_)
    {
        startTableCell();
        addImage(mmIniOptions::instance().logoName_);
        endTableCell();
        cols++;
    }
    startTableCell();
    addHeader(2, mmIniOptions::instance().userNameString_);
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
    html += wxString::Format(wxT("<h%d>%s</h%d>\n"), level, header.c_str(), level);
}

void mmHTMLBuilder::addParaText(const wxString& text)
{
    html += wxString::Format(wxT("<p><font size=%s>%s</font></p>\n"), fontSize.c_str(), text.c_str());
}

void mmHTMLBuilder::addLineBreak()
{
    html += wxT("<br>\n");
}

void mmHTMLBuilder::addHorizontalLine(int size)
{
	html += wxT("<hr");
	if(size > 0)
	html += wxString::Format(wxT(" size=\"%d\""), size);
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
	//Comment line above then uncomment line below for debug homepage 
	//html += wxT("<table border=\"2\" cellspacing=\"1\"");
	if(!width.empty())
	    html += wxString::Format(wxT(" width=\"%s\""),width.c_str());
	if(!valign.empty()) 
	    html += wxString::Format(wxT(" valign=\"%s\""), valign.c_str());
	html += wxT(">\n");
	bgswitch = true;
}

void mmHTMLBuilder::startTableRow()
{
	html << wxT("<tr ") << (bgswitch ? color1 : color0) << wxT(">");
}

void mmHTMLBuilder::startTableCell(const wxString& width)
{
	html += wxT("<td");
	if(!width.empty())
	    html += wxString::Format(wxT(" width=\"%s\""),width.c_str());
	html += wxT(">");
}

void mmHTMLBuilder::addRowSeparator(int cols) {

	html += wxT("<tr bgcolor=\"") + mmColors::listBorderColor.GetAsString(wxC2S_HTML_SYNTAX) + wxT("\" height=\"1\"><td height=\"1");
	if(cols > 1)
		html += wxString::Format(wxT("\" colspan=\"%d"), cols);
    html += wxT("\"></td></tr>\n");
	bgswitch = true;
}

void mmHTMLBuilder::addTotalRow(const wxString& caption, int cols, const wxString& value) {
	html += wxT("<tr bgcolor=\"") + mmColors::listBackColor.GetAsString(wxC2S_HTML_SYNTAX) + wxT("\"><td");
	if(cols - 1 > 1) 
		html += wxString::Format(wxT(" colspan=\"%d\""), cols - 1);
	html += wxString::Format(wxT("><font size=%s><b><i>&nbsp;&nbsp;%s"), fontSize.c_str(), caption.c_str());
	html += wxString::Format(wxT("</i></b></font></td><td nowrap align=\"right\"><font size=%s><b><i>%s</i></b></font></td></tr>\n"), fontSize.c_str(), value.c_str());
}

void mmHTMLBuilder::addTotalRow(const wxString& caption, int cols, std::vector<wxString>& data) {
	html += wxT("<tr bgcolor=\"") + mmColors::listBackColor.GetAsString(wxC2S_HTML_SYNTAX) + wxT("\"><td");
	if(cols - data.size() > 1) 
		html += wxString::Format(wxT(" colspan=\"%d\""), cols - data.size());
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

void mmHTMLBuilder::addTableHeaderCell(const wxString& value, bool numeric) {
	html += (numeric ? wxT("<th nowrap align=\"right\" ") : wxT("<th align=\"left\" "));
	html += wxT(" valign=\"center\" bgcolor=\"#d5d6de\">");
	html += wxString::Format(wxT ("<font size=%s><b>&nbsp;%s</b></font></th>\n"), fontSize.c_str(), value.c_str());
	bgswitch = false;
}
void mmHTMLBuilder::addTableHeaderCell(const wxString& value) {
	html += wxT("<th nowrap align=\"left\" valign=\"center\" bgcolor=\"#d5d6de\">");
	html += wxT ("<font size=") + fontSize + wxT(">") + wxT("<b>&nbsp;") + value + wxT("</b></font></th>\n");
	bgswitch = false;
}

void mmHTMLBuilder::addTableCell(const wxString& value, bool numeric, bool italic, bool bold, const wxString& fontColor) {

    html << (numeric ? wxT("<td nowrap align=\"right\" ") : wxT("<td")) << wxT("><font size=") << fontSize;

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
	addTableHeaderCell(wxT("<a href=\"") + href + wxT("\">") + value + wxT("</a>\n"), false);
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

