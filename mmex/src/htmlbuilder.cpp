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
    color0_ = wxT("bgcolor=\"") + mmColors::listAlternativeColor0.GetAsString(wxC2S_HTML_SYNTAX) + wxT ("\"");
    color1_ = wxT("bgcolor=\"") + mmColors::listAlternativeColor1.GetAsString(wxC2S_HTML_SYNTAX) + wxT ("\"");
    // init font size from config
    font_size_ = mmIniOptions::instance().html_font_size_;
}

void mmHTMLBuilder::init()
{
    bgswitch_ = true;
    html_+= wxT("<html>\n<head>\n<meta http-equiv=\"Content-Type\" content=\"text/html; charset=UTF-8\" /><title>");  
    html_+= mmex::getProgramName();
    html_+= wxT(" - ");
    html_+= _("Report");
    html_+= wxT("</title>\n</head><body bgcolor=\"") + mmColors::listBackColor.GetAsString(wxC2S_HTML_SYNTAX);
    html_+= wxT("\" text=\"#000000\" link=\"#0000cc\" vlink=\"#551a8b\" alink=\"#ff0000\">");
    html_+= wxString::Format(wxT("<font size=\"%d\">\n"), font_size_);

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
    html_+= wxT("\n</font></body>\n</html>\n");
}

void mmHTMLBuilder::addHeader(const int level, const wxString& header)
{
    int header_font_size = level + font_size_;
    if (header_font_size > 7) header_font_size = 7;
    html_+= wxString::Format(wxT("<font size=\"%d\"><b>%s</b></font><br>\n"), header_font_size, header.c_str());
}

void mmHTMLBuilder::addHeaderItalic(const int level, const wxString& header)
{
    int header_font_size = level + font_size_;
    if (header_font_size > 7) header_font_size = 7;
    html_+= wxString::Format(wxT("<font size=\"%d\"><i>%s</i></font><br>\n"), header_font_size, header.c_str());
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
    html_+= wxString::Format(wxT("<p><font size=\"%d\">%s</font></p>\n"), font_size_, text.c_str());
}

void mmHTMLBuilder::addLineBreak()
{
    html_+= wxT("<br>\n");
}

void mmHTMLBuilder::addHorizontalLine(int size)
{
    html_+= wxT("<hr");
    if(size > 0)
    html_+= wxString::Format(wxT(" size=\"%d\""), size);
    html_+= wxT(">\n");
}

void mmHTMLBuilder::addImage(const wxString& src)
{
    if(!src.empty())
        html_+= wxT("<img src=\"") + src + wxT("\" border=\"0\">");
}

void mmHTMLBuilder::startCenter()
{
    html_+= wxT("<center>");
}

void mmHTMLBuilder::endCenter()
{
    html_+= wxT("</center>");
}

void mmHTMLBuilder::startTable(const wxString& width, const wxString& valign)
{
    html_+= wxT("<table cellspacing=\"1\"");
    //Comment line above then uncomment line below for debug homepage 
    //html_+= wxT("<table border=\"2\" cellspacing=\"1\"");
    if(!width.empty())
        html_+= wxString::Format(wxT(" width=\"%s\""),width.c_str());
    if(!valign.empty()) 
        html_+= wxString::Format(wxT(" valign=\"%s\""), valign.c_str());
    html_+= wxT(">\n");
    bgswitch_ = true;
}

void mmHTMLBuilder::startTableRow()
{
    html_<< wxT("<tr ") << (bgswitch_ ? color1_ : color0_) << wxT(">");
}

void mmHTMLBuilder::startTableCell(const wxString& width)
{
    html_+= wxT("<td");
    if(!width.empty())
        html_+= wxString::Format(wxT(" width=\"%s\""),width.c_str());
    html_+= wxT(">");
}

void mmHTMLBuilder::addRowSeparator(int cols) 
{

    html_+= wxT("<tr bgcolor=\"") + mmColors::listBorderColor.GetAsString(wxC2S_HTML_SYNTAX) + wxT("\" height=\"1\"><td height=\"1");
    if(cols > 1)
        html_+= wxString::Format(wxT("\" colspan=\"%d"), cols);
    html_+= wxT("\"></td></tr>\n");
    bgswitch_ = true;
}

void mmHTMLBuilder::addTotalRow(const wxString& caption, int cols, const wxString& value) 
{
    html_+= wxT("<tr bgcolor=\"") + mmColors::listBackColor.GetAsString(wxC2S_HTML_SYNTAX) + wxT("\"><td");
    if(cols - 1 > 1) 
        html_+= wxString::Format(wxT(" colspan=\"%d\""), cols - 1);
    html_+= wxString::Format(wxT("><font size=\"%d\"><b><i>&nbsp;&nbsp;%s"), font_size_, caption.c_str());
    html_+= wxString::Format(wxT("</i></b></font></td><td nowrap align=\"right\"><font size=\"%d\"><b><i>%s</i></b></font></td></tr>\n"), font_size_, value.c_str());
}

void mmHTMLBuilder::addTotalRow(const wxString& caption, int cols, std::vector<wxString>& data) 
{
    html_+= wxT("<tr bgcolor=\"") + mmColors::listBackColor.GetAsString(wxC2S_HTML_SYNTAX) + wxT("\"><td");
    if((long)cols - data.size() > 1) 
        html_+= wxString::Format(wxT(" colspan=\"%ld\""), (long)cols - data.size());
    html_+= wxString::Format(wxT("><font size=\"%d\"><b><i>&nbsp;&nbsp;"), font_size_);
    html_+= caption;
    html_+= wxT("</i></b></font>");

    for (unsigned long idx = 0; idx < data.size(); idx++)
    {
        html_+= wxString::Format(wxT("</td><td nowrap align=\"right\"><font size=\"%d\"><b><i>"), font_size_);
        html_+= data[idx];
        html_+= wxT("</i></b></font>");
    }
    html_+= wxT("</td></tr>\n");
}

void mmHTMLBuilder::addTableHeaderRow(const wxString& value, int cols) 
{
    html_+= wxT("<tr><th align=\"left\" valign=\"center\" bgcolor=\"#d5d6de\"");
    if(cols > 1)
        html_+= wxString::Format(wxT(" colspan=\"%d\" "), cols);
    html_+= wxString::Format(wxT("><font size=\"%d\"><b>&nbsp;"), font_size_);
    html_+= value;
    html_+= wxT("</b></font></th></tr>\n");
    bgswitch_ = true;
}

void mmHTMLBuilder::addTableHeaderCell(const wxString& value, bool numeric) 
{
    html_+= numeric ? wxT("<th nowrap align=\"right\" ") : wxT("<th align=\"left\" ");
    html_+= wxT(" valign=\"center\" bgcolor=\"#d5d6de\">");
    html_+= wxT("<b>&nbsp;") + value + wxT("</b></th>\n");
    bgswitch_ = false;
}

void mmHTMLBuilder::addTableCell(const wxString& value, bool numeric, bool italic, bool bold, const wxString& fontColor) 
{
    html_<< (numeric ? wxT("<td nowrap align=\"right\" >") : wxT("<td>"));

    if(!fontColor.empty())
        html_+= wxT("<font color=") + fontColor + wxT(">");

    if (!bold && !italic)    html_+= value;
    else if (bold && italic) html_+= wxT("<b><i>") + value + wxT("</i></b>");
    else if (bold)           html_+= wxT("<b>") + value + wxT("</b>");
    else if (italic)         html_+= wxT("<i>") + value + wxT("</i>");

    if(!fontColor.empty())
        html_+= wxT("</font>");
    html_+= wxT("</td>\n");
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
    html_+= wxT("</table>\n");
}

void mmHTMLBuilder::endTableRow()
{
    html_+= wxT("</tr>\n");
    bgswitch_ = !bgswitch_;
}

void mmHTMLBuilder::endTableCell()
{
    html_+= wxT("</td>\n");
}

