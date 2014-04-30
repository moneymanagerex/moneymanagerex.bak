/*******************************************************
Copyright (C) 2014 Nikolay

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

#include "qif_import.h"
#include <vector>

bool mmQIFImport::isLineOK(const wxString& line)
{
    return wxString("!DNPAT^MLSE$C/UI").Contains(line.Left(1));
}

wxString mmQIFImport::getLineData(const wxString& line)
{
    return line.substr(1, line.Length() - 1);
}

wxString mmQIFImport::getFileLine(wxTextInputStream& textFile, int& lineNumber)
{
    wxString textLine = textFile.ReadLine();
    lineNumber++;
    textLine.Trim(); // remove any trailing spaces

    return textLine;
}

wxString mmQIFImport::getFinancistoProject(wxString& sSubCateg)
{
    //Additional parsint for Financisto
    wxString sProject = "";
    wxStringTokenizer cattkz(sSubCateg, "/");

    sSubCateg = cattkz.GetNextToken();
    if (cattkz.HasMoreTokens())
        sProject = cattkz.GetNextToken().Trim();
    return sProject;
}

qifAccountInfoType mmQIFImport::accountInfoType(const wxString& line)
{
    if (line.IsEmpty())
        return UnknownInfo;

    wxChar fChar = line.GetChar(0);
    switch (fChar)
    {
    case 'N':
        return Name;
    case 'T':
        return AccountType;
    case 'D':
        return Description;
    case 'L':
        return CreditLimit;
    case '/':
        return BalanceDate;
    case '$':
        return Balance;
    case '^':
        return EOT;
    default:
        return UnknownInfo;
    }
}

qifLineType mmQIFImport::lineType(const wxString& line)
{
    if (line.IsEmpty())
        return UnknownType;

    wxChar fChar = line.GetChar(0);
    switch (fChar)
    {
    case '!':
        return AcctType;
    case 'D':
        return Date;
    case 'N':
        return TransNumber;
    case 'P':
        return Payee;
    case 'A':
        return Address;
    case 'T':
        return Amount;
    case '^':
        return EOTLT;
    case 'M':
        return Memo;
    case 'L':
        return Category;
    case 'S':
        return CategorySplit;
    case 'E':
        return MemoSplit;
    case '$':
        return AmountSplit;
    case 'C':
        return Status;
    default:
        return UnknownType;
    }
}

bool mmQIFImport::handle_file(wxFileInputStream& input)
{
    wxTextInputStream text(input, "\x09", wxConvUTF8);

    bool record_start = false;
    std::vector<Line_Value> record;  // each record may contains mult lines
    while (input.IsOk() && !input.Eof())
    {
        wxString line = text.ReadLine();
        Line_Value lv;
        if (this->handle_line(line, lv))
        {
            if (lv.first == EOTLT) 
            {
                // process record 
                this->handle_record(record);
                // release record
                record.clear();
            }
            else
            {
                record.push_back(lv);
            }
        }
        else
        {
            // TODO
        }
    }

    return true;
}

bool mmQIFImport::handle_file(const wxString& input_file)
{
    wxFileInputStream input(input_file);
    return this->handle_file(input); 
}

bool mmQIFImport::handle_line(const wxString& line, Line_Value& lv)
{
    lv.first = lineType(line);
    lv.second = getLineData(line);

    return lv.first != UnknownType;
}

bool mmQIFImport::handle_record(const Record& record)
{
    for (const auto& line : record)
    {
       // TODO 
    }
    return true;
}

