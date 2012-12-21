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
    font_size_ = mmIniOptions::instance().html_font_size_;
}

void mmHTMLBuilder::init()
{
    html += wxT("<html>\n<head>\n<meta http-equiv=\"Content-Type\" content=\"text/html; charset=UTF-8\" /><title>");  
    html += mmex::getProgramName();
    html += wxT(" - ");
    html += _("Report");
    html += wxT("</title>\n</head><body bgcolor=\"") + mmColors::listBackColor.GetAsString(wxC2S_HTML_SYNTAX);
    html += wxT("\" text=\"#000000\" link=\"#0000cc\" vlink=\"#551a8b\" alink=\"#ff0000\">");
    html += wxString::Format(wxT("<font size=\"%d\">\n"), font_size_);

    //if I need more space on the top of home page and reports I will delete user name from settings
    if (mmOptions::instance().userNameString_ != wxT (""))
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
        addHeader(2, mmOptions::instance().userNameString_);
        endTableCell();
        endTableRow();
        endTable();
        addHorizontalLine(2);
    }
}

void mmHTMLBuilder::end()
{
    html += wxT("\n</font></body>\n</html>\n");
}

void mmHTMLBuilder::addHeader(const int level, const wxString& header)
{
    int header_font_size = level + font_size_;
    if (header_font_size > 7) header_font_size = 7;
    html += wxString::Format(wxT("<font size=\"%d\"><b>%s</b></font><br>\n"), header_font_size, header.c_str());
}

void mmHTMLBuilder::addHeaderItalic(const int level, const wxString& header)
{
    int header_font_size = level + font_size_;
    if (header_font_size > 7) header_font_size = 7;
    html += wxString::Format(wxT("<font size=\"%d\"><i>%s</i></font><br>\n"), header_font_size, header.c_str());
}

void mmHTMLBuilder::addDateNow()
{
    wxDateTime now = wxDateTime::Now();
    wxString dt = _("Today's Date: ") + mmGetNiceDateString(now);
    addHeaderItalic(1, dt);
    addLineBreak();
}

void mmHTMLBuilder::addParaText(const wxString& text)
{
    html += wxString::Format(wxT("<p><font size=\"%d\">%s</font></p>\n"), font_size_, text.c_str());
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

void mmHTMLBuilder::addRowSeparator(int cols) 
{

    html += wxT("<tr bgcolor=\"") + mmColors::listBorderColor.GetAsString(wxC2S_HTML_SYNTAX) + wxT("\" height=\"1\"><td height=\"1");
    if(cols > 1)
        html += wxString::Format(wxT("\" colspan=\"%d"), cols);
    html += wxT("\"></td></tr>\n");
    bgswitch = true;
}

void mmHTMLBuilder::addTotalRow(const wxString& caption, int cols, const wxString& value) 
{
    html += wxT("<tr bgcolor=\"") + mmColors::listBackColor.GetAsString(wxC2S_HTML_SYNTAX) + wxT("\"><td");
    if(cols - 1 > 1) 
        html += wxString::Format(wxT(" colspan=\"%d\""), cols - 1);
    html += wxString::Format(wxT("><font size=\"%d\"><b><i>&nbsp;&nbsp;%s"), font_size_, caption.c_str());
    html += wxString::Format(wxT("</i></b></font></td><td nowrap align=\"right\"><font size=\"%d\"><b><i>%s</i></b></font></td></tr>\n"), font_size_, value.c_str());
}

void mmHTMLBuilder::addTotalRow(const wxString& caption, int cols, std::vector<wxString>& data) 
{
    html += wxT("<tr bgcolor=\"") + mmColors::listBackColor.GetAsString(wxC2S_HTML_SYNTAX) + wxT("\"><td");
    if((long)cols - data.size() > 1) 
        html += wxString::Format(wxT(" colspan=\"%ld\""), (long)cols - data.size());
    html += wxString::Format(wxT("><font size=\"%d\"><b><i>&nbsp;&nbsp;"), font_size_);
    html += caption;
    html += wxT("</i></b></font>");

    for (unsigned long idx = 0; idx < data.size(); idx++)
    {
        html += wxString::Format(wxT("</td><td nowrap align=\"right\"><font size=\"%d\"><b><i>"), font_size_);
        html += data[idx];
        html += wxT("</i></b></font>");
    }
    html += wxT("</td></tr>\n");
}

void mmHTMLBuilder::addTableHeaderRow(const wxString& value, int cols) 
{
    html += wxT("<tr><th align=\"left\" valign=\"center\" bgcolor=\"#d5d6de\"");
    if(cols > 1)
        html += wxString::Format(wxT(" colspan=\"%d\" "), cols);
    html += wxString::Format(wxT("><font size=\"%d\"><b>&nbsp;"), font_size_);
    html += value;
    html += wxT("</b></font></th></tr>\n");
    bgswitch = true;
}

void mmHTMLBuilder::addTableHeaderCell(const wxString& value, bool numeric) 
{
    html += numeric ? wxT("<th nowrap align=\"right\" ") : wxT("<th align=\"left\" ");
    html += wxT(" valign=\"center\" bgcolor=\"#d5d6de\">");
    html += wxT("<b>&nbsp;") + value + wxT("</b></th>\n");
    bgswitch = false;
}

void mmHTMLBuilder::addTableCell(const wxString& value, bool numeric, bool italic, bool bold, const wxString& fontColor) 
{
    html << (numeric ? wxT("<td nowrap align=\"right\" >") : wxT("<td>"));

    if(!fontColor.empty())
        html += wxT("<font color=") + fontColor + wxT(">");

    if (!bold && !italic)    html += value;
    else if (bold && italic) html += wxT("<b><i>") + value + wxT("</i></b>");
    else if (bold)           html += wxT("<b>") + value + wxT("</b>");
    else if (italic)         html += wxT("<i>") + value + wxT("</i>");

    if(!fontColor.empty())
        html += wxT("</font>");
    html += wxT("</td>\n");
}

void mmHTMLBuilder::addTableCellLink(const wxString& href, const wxString& value, bool numeric, bool italic, bool bold, const wxString& fontColor)
{
    addTableCell(wxT("<a href=\"") + href + wxT("\">") + value + wxT("</a>\n"), numeric, italic, bold, fontColor);
}

void mmHTMLBuilder::addTableHeaderCellLink(const wxString& href, const wxString& value) 
{
    addTableHeaderCell(wxT("<a href=\"") + href + wxT("\">") + value + wxT("</a>\n"), false);
}

void mmHTMLBuilder::addTableHeaderRowLink(const wxString& href, const wxString& value, int cols) 
{
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

