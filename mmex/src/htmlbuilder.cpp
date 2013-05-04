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
#include "mmOption.h"
#include "constants.h"

mmHTMLBuilder::mmHTMLBuilder() {
    // init colors from config
    color1_ = mmColors::listAlternativeColor0.GetAsString(wxC2S_HTML_SYNTAX);
    color0_ = mmColors::listBackColor.GetAsString(wxC2S_HTML_SYNTAX);
    color_table_header_ = "#d5d6de";
    // init font size from config
    font_size_ = mmIniOptions::instance().html_font_size_;
}

void mmHTMLBuilder::init()
{
    bgswitch_ = true;
    html_ = "<html>\n<head>\n<meta http-equiv=\"Content-Type\" content=\"text/html; charset=UTF-8\" /><title>";
    html_+= mmex::getProgramName();
    html_+= " - ";
    html_+= _("Report");
    html_+= "</title>\n</head>";
	html_+= wxString::Format("<body bgcolor=\"%s\" "
		, mmColors::listBackColor.GetAsString(wxC2S_HTML_SYNTAX));
    html_+= wxString::Format("\" text=\"%s\" "
		, mmColors::listBorderColor.GetAsString(wxC2S_HTML_SYNTAX));
    html_+= wxString::Format("link=\"%s\" ", "#0000cc");
    html_+= wxString::Format("vlink=\"%s\" ", "#551a8b");
    html_+= wxString::Format("alink=\"%s\">", "#ff0000");
    html_+= wxString::Format("<font size=\"%i\">\n", font_size_);

    //Show user name if provided
    if (mmOptions::instance().userNameString_ != "")
    {
        int cols = 1;
        startTable("100%");
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
    html_+= "\n</font></body>\n</html>\n";
}

void mmHTMLBuilder::addHeader(const int level, const wxString& header)
{
    int header_font_size = level + font_size_;
    if (header_font_size > 7) header_font_size = 7;
    html_+= wxString::Format("<font size=\"%i\"><b>%s</b></font><br>\n"
		, header_font_size, header);
}

void mmHTMLBuilder::addHeaderItalic(const int level, const wxString& header)
{
    int header_font_size = level + font_size_;
    if (header_font_size > 7) header_font_size = 7;
    html_+= wxString::Format("<font size=\"%i\"><i>%s</i></font>\n"
		, header_font_size, header);
}

void mmHTMLBuilder::addDateNow()
{
    wxString dt = wxString::Format(_("Today's Date: %s")
		, mmGetNiceDateString(wxDateTime::Now()));
    addHeaderItalic(1, dt);
    addLineBreak();
}

void mmHTMLBuilder::addParaText(const wxString& text)
{
    html_+= wxString::Format("<p><font size=\"%d\">%s</font></p>\n"
		, font_size_, text);
}

void mmHTMLBuilder::addText(const wxString& text)
{
    html_+= text;
}

void mmHTMLBuilder::addLineBreak()
{
    html_+= "<br>\n";
}

void mmHTMLBuilder::addHorizontalLine(const int size)
{
    html_+= "<hr";
    if(size > 0)
    html_+= wxString::Format(" size=\"%d\"", size);
    html_+= ">\n";
}

void mmHTMLBuilder::addImage(const wxString& src)
{
    if(!src.empty())
        html_+= "<img src=\"" + src + "\" border=\"0\">";
}

void mmHTMLBuilder::startCenter()
{
    html_+= "<center>";
}

void mmHTMLBuilder::endCenter()
{
    html_+= "</center>";
}

void mmHTMLBuilder::startTable(const wxString& width
	, const wxString& valign, const wxString& border)
{
    html_+= "<table cellspacing=\"1\" ";
    html_+= wxString::Format("bgcolor=\"%s\" "
		, mmColors::listBackColor.GetAsString(wxC2S_HTML_SYNTAX));
    //Comment line above then uncomment line below for debug homepage
    //html_+= "<table border=\"2\" cellspacing=\"1\"";
    if(!width.empty())
        html_+= wxString::Format(" width=\"%s\"", width);
    if(!valign.empty())
        html_+= wxString::Format(" valign=\"%s\"", valign);
    if(!border.empty())
        html_+= wxString::Format(" border=\"%s\"", border);
    html_+= ">\n";
    bgswitch_ = true;
}

void mmHTMLBuilder::startTableRow(const wxString& custom_color)
{
    wxString s = "<tr bgcolor=\"%s\" >";
    if (custom_color.IsEmpty())
        html_ += wxString::Format(s, (bgswitch_ ? color0_ : color1_));
    else
        html_ += wxString::Format(s, custom_color);
}

void mmHTMLBuilder::startTableCell(const wxString& width)
{
    html_+= "<td";
    if(!width.empty())
        html_+= wxString::Format(" width=\"%s\"",width);
    html_+= ">";
}

void mmHTMLBuilder::addRowSeparator(const int cols)
{
    bgswitch_ = true;

    if (cols > 0)
    {
        startTableRow();
        html_+= wxString::Format("<td colspan=\"%d\" >", cols);
        startTable("100%", "top", "1");
        endTable();
        endTableCell();
        endTableRow();
        bgswitch_ = true;
    }
}

void mmHTMLBuilder::addTotalRow(const wxString& caption
	, const int cols, const wxString& value)
{
    html_+= "<tr bgcolor=\"" + mmColors::listBackColor.GetAsString(wxC2S_HTML_SYNTAX) + "\"><td";
    if(cols - 1 > 1)
        html_+= wxString::Format(" colspan=\"%d\"", cols - 1);
    html_+= wxString::Format("><font size=\"%d\"><b><i>&nbsp;&nbsp;%s"
		, font_size_, caption);
    html_+= wxString::Format("</i></b></font></td><td nowrap align=\"right\"><font size=\"%d\"><b><i>%s</i></b></font></td></tr>\n"
		, font_size_, value);
}

void mmHTMLBuilder::addTotalRow(const wxString& caption, const int cols
	, const std::vector<wxString>& data)
{
    html_+= "<tr bgcolor=\"";
	html_+= mmColors::listBackColor.GetAsString(wxC2S_HTML_SYNTAX);
	html_+= "\"><td";
    if((long)cols - data.size() > 1)
        html_+= wxString::Format(" colspan=\"%ld\"", (long)cols - data.size());
    html_+= wxString::Format("><font size=\"%d\"><b><i>&nbsp;&nbsp;", font_size_);
    html_+= caption;
    html_+= "</i></b></font>";

    for (unsigned long idx = 0; idx < data.size(); idx++)
    {
        html_+= wxString::Format("</td><td nowrap align=\"right\"><font size=\"%d\"><b><i>"
			, font_size_);
        html_+= data[idx];
        html_+= "</i></b></font>";
    }
    html_+= "</td></tr>\n";
}

void mmHTMLBuilder::addTableHeaderRow(const wxString& value, const int cols)
{
    html_+= wxString::Format("<tr><th align=\"left\" valign=\"center\" bgcolor=\"%s\"", color_table_header_);
    if(cols > 1)
        html_+= wxString::Format(" colspan=\"%d\" ", cols);
    html_+= wxString::Format("><font size=\"%d\"><b>&nbsp;", font_size_);
    html_+= value;
    html_+= "</b></font></th></tr>\n";
    bgswitch_ = true;
}

void mmHTMLBuilder::addTableHeaderCell(const wxString& value, const bool& numeric)
{
    html_+= numeric ? "<th nowrap align=\"right\" " : "<th align=\"left\" ";
    html_+= wxString::Format(" valign=\"center\" bgcolor=\"%s\">", color_table_header_);
    html_+= "<b>&nbsp;" + value + "</b></th>\n";
    bgswitch_ = false;
}

void mmHTMLBuilder::addMoneyCell(double amount, bool color)
{
    wxString balance;
    mmex::formatDoubleToCurrency(amount, balance);
    this->addTableCell(balance, true, true, true, (amount < 0 && color) ? "RED": "");
}

void mmHTMLBuilder::addTableCell(const wxDateTime& date)
{
    wxString date_str = mmGetDateForDisplay(date);
    this->addTableCell(date_str, false);
}

void mmHTMLBuilder::addTableCell(const wxString& value
    , const bool& numeric, const bool& italic, const bool& bold, const wxString& fontColor)
{
    html_<< (numeric ? "<td nowrap align=\"right\" >" : "<td>");

    if(!fontColor.empty())
        html_+= "<font color=\"" + fontColor + "\">";

    if (!bold && !italic)    html_+= value;
    else if (bold && italic) html_+= "<b><i>" + value + "</i></b>";
    else if (bold)           html_+= "<b>" + value + "</b>";
    else if (italic)         html_+= "<i>" + value + "</i>";

    if(!fontColor.empty())
        html_+= "</font>";
    html_+= "</td>\n";
}

void mmHTMLBuilder::addTableCellLink(const wxString& href
    , const wxString& value, const bool& numeric
    , const bool& italic, const bool& bold, const wxString& fontColor)
{
    addTableCell("<a href=\"" + href + "\">" + value + "</a>\n"
		, numeric, italic, bold, fontColor);
}

void mmHTMLBuilder::addTableHeaderCellLink(const wxString& href, const wxString& value)
{
    addTableHeaderCell("<a href=\"" + href + "\">" + value + "</a>\n", false);
}

void mmHTMLBuilder::addTableHeaderRowLink(const wxString& href
	, const wxString& value, const int cols)
{
    addTableHeaderRow("<a href=\"" + href + "\">" + value + "</a>\n", cols);
}

void mmHTMLBuilder::endTable()
{
    html_+= "</table>\n";
}

void mmHTMLBuilder::endTableRow()
{
    html_+= "</tr>\n";
    bgswitch_ = !bgswitch_;
}

void mmHTMLBuilder::endTableCell()
{
    html_+= "</td>\n";
}

void mmHTMLBuilder::DisplayDateHeading(const wxDateTime& startYear, const wxDateTime& endYear, bool withDateRange)
{
    wxString todaysDate = wxString::Format(_("Today's Date: %s"), mmGetNiceDateString(wxDateTime::Now()));
    todaysDate << "<br><br>";
    if (withDateRange)
    {
        todaysDate
        << wxString::Format(_("From %s till %s")
            , mmGetNiceDateSimpleString(startYear).Prepend("<b>").Append("</b> ")
            , mmGetNiceDateSimpleString(endYear).Prepend("<b>").Append("</b> "));
    }
    this->addHeaderItalic(1, todaysDate);
    this->addLineBreak();
    this->addLineBreak();
}

