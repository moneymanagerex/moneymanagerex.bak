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

//----------------------------------------------------------------------------
#ifndef _MM_EX_PANELBASE_H_
#define _MM_EX_PANELBASE_H_
//----------------------------------------------------------------------------
#include <wx/panel.h>
#include <wx/string.h>
//----------------------------------------------------------------------------
class wxSQLite3Database;

struct mmHolderBase
{
    int id_;
    wxString valueStr_;
    double value_;
};

class mmPanelBase : public wxPanel
{
public: 
    virtual wxString getReportText() const { return wxGetEmptyString(); }
    
    mmPanelBase() {}
    mmPanelBase(wxSQLite3Database* db, wxSQLite3Database* inidb): db_(db), inidb_(inidb) {}

    wxSQLite3Database* db_;
    wxSQLite3Database* inidb_;
};
//----------------------------------------------------------------------------
#endif // _MM_EX_PANELBASE_H_
//----------------------------------------------------------------------------
