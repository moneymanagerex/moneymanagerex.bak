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
    color0 = ("bgcolor=\"") + mmColors::listAlternativeColor0.GetAsString(wxC2S_HTML_SYNTAX) + wxT ("\"");
    color1 = ("bgcolor=\"") + mmColors::listAlternativeColor1.GetAsString(wxC2S_HTML_SYNTAX) + wxT ("\"");
    // init font size from config
    fontSize = wxT ("\"") + mmIniOptions::instance().fontSize_ + wxT ("\"");
}

void mmHTMLBuilder::init()
{
    html += ("<html>\n<head>\n<meta http-equiv=\"content-type\" content=\"text/html;charset=UTF-8\"><title>");
    html += mmex::getProgramName();
    html += (" - ");
    html += _("Report");
    html += ("</title>\n</head><body bgcolor=\"") + mmColors::listBackColor.GetAsString(wxC2S_HTML_SYNTAX);
    html += ("\" text=\"#000000\" link=\"#0000cc\" vlink=\"#551a8b\" alink=\"#ff0000\">");
    html += wxString::Format(("<font size=%s>\n"), fontSize.c_str());

    //if I need more space on the top of home page and reports I will delete user name from settings
    if (mmIniOptions::instance().userNameString_ != wxT (""))
    {
        int cols = 1;
        startTable(("100%"));
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
    html += ("\n</font></body>\n</html>\n");
}

void mmHTMLBuilder::addHeader(int level, const wxString& header)
{
    long font_size;
    if(!mmIniOptions::instance().fontSize_.ToLong(&font_size)) { font_size = 3; }
    font_size = level + font_size;
    if (font_size>7) font_size=7;
    html += wxString::Format(("<font size=\"%ld\"><b>%s</b></font><br>\n"), font_size, header.c_str());
}

void mmHTMLBuilder::addHeaderItalic(int level, const wxString& header)
{
    long font_size;
    if(!mmIniOptions::instance().fontSize_.ToLong(&font_size)) { font_size = 3; }
    font_size = level + font_size;
    if (font_size>7) font_size=7;
    html += wxString::Format(("<font size=\"%ld\"><i>%s</i></font><br>\n"), font_size, header.c_str());
}

void mmHTMLBuilder::addParaText(const wxString& text)
{
    html += wxString::Format(("<p><font size=%s>%s</font></p>\n"), fontSize.c_str(), text.c_str());
}

void mmHTMLBuilder::addLineBreak()
{
    html += ("<br>\n");
}

void mmHTMLBuilder::addHorizontalLine(int size)
{
    html += ("<hr");
    if(size > 0)
    html += wxString::Format((" size=\"%d\""), size);
    html += (">\n");
}

void mmHTMLBuilder::addImage(const wxString& src)
{
    if(!src.empty())
        html += ("<img src=\"") + src + ("\" border=\"0\">");
}

void mmHTMLBuilder::startCenter()
{
    html += ("<center>");
}

void mmHTMLBuilder::endCenter()
{
    html += ("</center>");
}

void mmHTMLBuilder::startTable(const wxString& width, const wxString& valign)
{
    html += ("<table cellspacing=\"1\"");
    //Comment line above then uncomment line below for debug homepage
    //html += ("<table border=\"2\" cellspacing=\"1\"");
    if(!width.empty())
        html += wxString::Format((" width=\"%s\""),width.c_str());
    if(!valign.empty())
        html += wxString::Format((" valign=\"%s\""), valign.c_str());
    html += (">\n");
    bgswitch = true;
}

void mmHTMLBuilder::startTableRow()
{
    html << ("<tr ") << (bgswitch ? color1 : color0) << (">");
}

void mmHTMLBuilder::startTableCell(const wxString& width)
{
    html += ("<td");
    if(!width.empty())
        html += wxString::Format((" width=\"%s\""),width.c_str());
    html += (">");
}

void mmHTMLBuilder::addRowSeparator(int cols)
{

    html += ("<tr bgcolor=\"") + mmColors::listBorderColor.GetAsString(wxC2S_HTML_SYNTAX) + ("\" height=\"1\"><td height=\"1");
    if(cols > 1)
        html += wxString::Format(("\" colspan=\"%d"), cols);
    html += ("\"></td></tr>\n");
    bgswitch = true;
}

void mmHTMLBuilder::addTotalRow(const wxString& caption, int cols, const wxString& value)
{
    html += ("<tr bgcolor=\"") + mmColors::listBackColor.GetAsString(wxC2S_HTML_SYNTAX) + ("\"><td");
    if(cols - 1 > 1)
        html += wxString::Format((" colspan=\"%d\""), cols - 1);
    html += wxString::Format(("><b><i>&nbsp;&nbsp;%s"), caption.c_str());
    html += wxString::Format(("</i></b></td><td nowrap align=\"right\"><b><i>%s</i></b></td></tr>\n"), value.c_str());
}

void mmHTMLBuilder::addTotalRow(const wxString& caption, int cols, std::vector<wxString>& data)
{
    html += ("<tr bgcolor=\"") + mmColors::listBackColor.GetAsString(wxC2S_HTML_SYNTAX) + ("\"><td");
    if(cols - data.size() > 1)
        html += wxString::Format((" colspan=\"%ld\""), cols - data.size());
    html += ("><b><i>&nbsp;&nbsp;");
    html += caption;
    html += ("</i></b>");

    for (unsigned long idx = 0; idx < data.size(); idx++)
    {
        html += ("</td><td nowrap align=\"right\"><b><i>");
        html += data[idx];
        html += ("</i></b>");
    }
    html += ("</td></tr>\n");
}

void mmHTMLBuilder::addTableHeaderRow(const wxString& value, int cols)
{
    html += ("<tr><th align=\"left\" valign=\"center\" bgcolor=\"#d5d6de\"");
    if(cols > 1)
    {
        html += (" colspan=\"");
        html += wxString::Format(("%d"), cols);
        html += ("\"");
    }
    html += ("><b>&nbsp;");
    html += value;
    html += ("</b></th></tr>\n");
    bgswitch = true;
}

void mmHTMLBuilder::addTableHeaderCell(const wxString& value, bool numeric)
{
    html += (numeric ? ("<th nowrap align=\"right\" ") : ("<th align=\"left\" "));
    html += (" valign=\"center\" bgcolor=\"#d5d6de\">");
    html += wxString::Format(wxT ("<b>&nbsp;%s</b></th>\n"), value.c_str());
    bgswitch = false;
}

void mmHTMLBuilder::addTableHeaderCell(const wxString& value)
{
    html += ("<th nowrap align=\"left\" valign=\"center\" bgcolor=\"#d5d6de\">");
    html += ("<b>&nbsp;") + value + ("</b></th>\n");
    bgswitch = false;
}

void mmHTMLBuilder::addTableCell(const wxString& value, bool numeric, bool italic, bool bold, const wxString& fontColor)
{
    html << (numeric ? ("<td nowrap align=\"right\" >") : ("<td>"));

    if(!fontColor.empty())
        html += ("<font color=") + fontColor + wxT (">");

    if (!bold && !italic)    html += value;
    else if (bold && italic) html += ("<b><i>") + value + ("</i></b>");
    else if (bold)           html += ("<b>") + value + ("</b>");
    else if (italic)         html += ("<i>") + value + ("</i>");

    if(!fontColor.empty())
        html += ("</font>");
    html += ("</td>\n");
}

void mmHTMLBuilder::addTableCellLink(const wxString& href, const wxString& value, bool numeric, bool italic, bool bold, const wxString& fontColor)
{
    addTableCell(("<a href=\"") + href + ("\">") + value + ("</a>\n"), numeric, italic, bold, fontColor);
}

void mmHTMLBuilder::addTableHeaderCellLink(const wxString& href, const wxString& value)
{
    addTableHeaderCell(("<a href=\"") + href + ("\">") + value + ("</a>\n"), false);
}

void mmHTMLBuilder::addTableHeaderRowLink(const wxString& href, const wxString& value, int cols)
{
    addTableHeaderRow(("<a href=\"") + href + ("\">") + value + ("</a>\n"), cols);
}

void mmHTMLBuilder::endTable()
{
    html += ("</table>\n");
}

void mmHTMLBuilder::endTableRow()
{
    html += ("</tr>\n");
    bgswitch = !bgswitch;
}

void mmHTMLBuilder::endTableCell()
{
    html += ("</td>\n");
}
