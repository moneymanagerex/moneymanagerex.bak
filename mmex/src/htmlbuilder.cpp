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
#include "mmCurrencyFormatter.h"
#include "mmOption.h"
#include "constants.h"

mmHTMLBuilder::mmHTMLBuilder()
{
    // init colors from config
    color_.color1 = mmColors::listAlternativeColor0.GetAsString(wxC2S_HTML_SYNTAX);
    color_.color0 = mmColors::listBackColor.GetAsString(wxC2S_HTML_SYNTAX);
    color_.link = "#0000cc";
    color_.vlink = "#551a8b";
    color_.alink = "#ff0000";
    color_.table_header = "#d5d6de";
    // init font size from config
    font_size_ = mmIniOptions::instance().html_font_size_;

    today_.date = wxDateTime::Now().GetDateOnly();
    today_.date_str = today_.date.FormatDate();
    today_.todays_date = wxString::Format(_("Today's Date: %s"), today_.date_str);
}

void mmHTMLBuilder::init()
{
    color_.bgswitch = true;
    html_ = wxString::Format(wxString::FromUTF8(tags::HTML), mmex::getProgramName());
    html_+= wxString::Format("<body bgcolor=\"%s\" "
        , mmColors::listBackColor.GetAsString(wxC2S_HTML_SYNTAX));
    html_+= wxString::Format("\" text=\"%s\" "
        , mmColors::listBorderColor.GetAsString(wxC2S_HTML_SYNTAX));
    html_+= wxString::Format("link=\"%s\" ", color_.link);
    html_+= wxString::Format("vlink=\"%s\" ", color_.vlink);
    html_+= wxString::Format("alink=\"%s\">", color_.alink);
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
    html_+= tags::END;
}

void mmHTMLBuilder::addHeader(const int level, const wxString& header)
{
    int header_font_size = level + font_size_;
    if (header_font_size > 7) header_font_size = 7;
    html_+= wxString::Format(tags::HEADER, header_font_size, header);
}

void mmHTMLBuilder::addHeaderItalic(const int level, const wxString& header)
{
    int header_font_size = level + font_size_;
    if (header_font_size > 7) header_font_size = 7;
    html_+= wxString::Format(tags::HEADER_ITALIC, header_font_size, header);
}

void mmHTMLBuilder::addDateNow()
{
    addHeaderItalic(1, today_.todays_date);
    addLineBreak();
}

void mmHTMLBuilder::addParaText(const wxString& text)
{
    html_+= wxString::Format(tags::PARAGRAPH, font_size_, text);
}

void mmHTMLBuilder::addText(const wxString& text)
{
    html_+= text;
}

void mmHTMLBuilder::addLineBreak()
{
    html_+= tags::BR;
}

void mmHTMLBuilder::addHorizontalLine(const int size)
{
    html_+= wxString::Format(tags::HOR_LINE, size);
}

void mmHTMLBuilder::addImage(const wxString& src)
{
    if(!src.empty())
        html_+= wxString::Format(tags::IMAGE, src);
}

void mmHTMLBuilder::startCenter()
{
    html_+= tags::CENTER;
}

void mmHTMLBuilder::endCenter()
{
    html_+= tags::CENTER_END;
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
    color_.bgswitch = true;
}

void mmHTMLBuilder::startTableRow(const wxString& custom_color)
{
    wxString s = tags::TABLE_ROW;
    if (custom_color.IsEmpty())
        html_ += wxString::Format(s, (color_.bgswitch ? color_.color0 : color_.color1));
    else
        html_ += wxString::Format(s, custom_color);
}

void mmHTMLBuilder::startTableCell(const wxString& width)
{
    html_+= wxString::Format(tags::TABLE_CELL, width.IsEmpty()? "0" : width);
}

void mmHTMLBuilder::addRowSeparator(const int cols)
{
    color_.bgswitch = true;

    if (cols > 0)
    {
        startTableRow();
        html_+= wxString::Format(tags::TABLE_CELL_SPAN, cols);
        startTable("100%", "top", "1");
        endTable();
        endTableCell();
        endTableRow();
        color_.bgswitch = true;
    }
}

void mmHTMLBuilder::addTotalRow(const wxString& caption
    , const int cols, const wxString& value)
{
    html_+= wxString::Format(tags::TABLE_ROW,  mmColors::listBackColor.GetAsString(wxC2S_HTML_SYNTAX));
    html_+= wxString::Format(tags::TABLE_CELL_SPAN, cols - 1);
    html_+= wxString::Format(tags::FONT_SIZE, font_size_);
    html_+= wxString::Format("<b><i>&nbsp;&nbsp;%s", caption);
    html_+= wxString::Format("</i></b></font></td>");
    html_+= "<td nowrap align=\"right\">";
    html_+= wxString::Format(tags::FONT_SIZE, font_size_);
    html_+= wxString::Format("<b><i>%s</i></b></font></td></tr>\n", value);
}

void mmHTMLBuilder::addTotalRow(const wxString& caption, int cols, double value)
{
    wxString value_str;
    CurrencyFormatter::formatDoubleToCurrency(value, value_str);
    this->addTotalRow(caption, cols, value_str);
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
        html_+= "</td><td nowrap align=\"right\">";
        html_+= wxString::Format("<font size=\"%d\">", font_size_);
        html_+= "<b><i>";
        html_+= data[idx];
        html_+= "</i></b></font>";
    }
    html_+= "</td></tr>\n";
}

void mmHTMLBuilder::addTotalRow(const wxString& caption, int cols, const std::vector<double>& data)
{
    std::vector<wxString> data_str;
    for (const auto& value: data)
    {
        wxString value_str;
        CurrencyFormatter::formatDoubleToCurrency(value, value_str);
        data_str.push_back(value_str);
    }
    this->addTotalRow(caption, cols, data_str);
}

void mmHTMLBuilder::addTableHeaderRow(const wxString& value, const int cols)
{
    html_+= wxString::Format("<tr><th align=\"left\" valign=\"center\" bgcolor=\"%s\"", color_.table_header);
    if(cols > 1)
        html_+= wxString::Format(" colspan=\"%d\" ", cols);
    html_+= wxString::Format("><font size=\"%d\"><b>&nbsp;", font_size_);
    html_+= value;
    html_+= "</b></font></th></tr>\n";
    color_.bgswitch = true;
}

void mmHTMLBuilder::addTableHeaderCell(const wxString& value, const bool& numeric)
{
    html_+= numeric ? "<th nowrap align=\"right\" " : "<th align=\"left\" ";
    html_+= wxString::Format(" valign=\"center\" bgcolor=\"%s\">", color_.table_header);
    html_+= "<b>&nbsp;" + value + "</b></th>\n";
    color_.bgswitch = false;
}

void mmHTMLBuilder::addMoneyCell(double amount, bool color)
{
    wxString balance;
     CurrencyFormatter::formatDoubleToCurrency(amount, balance);
    this->addTableCell(balance, true, true, true, (amount < 0 && color) ? "RED": "");
}

void mmHTMLBuilder::addMoneyCell(double amount, const wxString& color)
{
    wxString balance;
     CurrencyFormatter::formatDoubleToCurrency(amount, balance);
    this->addTableCell(balance, true, true, true, color);
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
    addTableCell(wxString::Format(tags::TABLE_CELL_LINK, href, value )
        , numeric, italic, bold, fontColor);
}

void mmHTMLBuilder::addTableHeaderCellLink(const wxString& href, const wxString& value)
{
    addTableHeaderCell(wxString::Format(tags::TABLE_HEADER_CELL_LINK, href, value ), false);
}

void mmHTMLBuilder::addTableHeaderRowLink(const wxString& href
    , const wxString& value, const int cols)
{
    addTableHeaderRow(wxString::Format(tags::TABLE_HEADER_ROW_LINK, href, value ), cols);
}

void mmHTMLBuilder::endTable()
{
    html_+= "</table>\n";
}

void mmHTMLBuilder::endTableRow()
{
    html_+= "</tr>\n";
    color_.bgswitch = !color_.bgswitch;
}

void mmHTMLBuilder::endTableCell()
{
    html_+= "</td>\n";
}

void mmHTMLBuilder::DisplayDateHeading(const wxDateTime& startYear, const wxDateTime& endYear, bool withDateRange)
{

    wxString todaysDate = "";
    if (withDateRange)
    {
        todaysDate << today_.todays_date << "<br><br>"
        << wxString::Format(_("From %s till %s")
            , mmGetNiceDateSimpleString(startYear).Prepend("<b>").Append("</b> ")
            , mmGetNiceDateSimpleString(endYear).Prepend("<b>").Append("</b> "));
    }
    else
    {
        todaysDate << _("Over Time");
    }
    this->addHeaderItalic(1, todaysDate);
    this->addLineBreak();
    this->addLineBreak();
}

