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
#include "mmcoredb.h"
//----------------------------------------------------------------------------
class wxSQLite3Database;
class wxListCtrl;

struct mmHolderBase
{
    int id_;
    wxString valueStr_;
    double value_;
};

class mmListCtrl : public wxListCtrl
{
public:
    mmListCtrl(wxWindow* parent, wxWindowID id
        , const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize
        , long style = wxLC_ICON
        , const wxValidator& validator = wxDefaultValidator
        , const wxString& name = wxListCtrlNameStr)
        : wxListCtrl(parent, id, pos, size, style, validator, name)
    {}

    virtual ~mmListCtrl() 
    {}
};

class mmPanelBase : public wxPanel
{
public: 
    virtual wxString getReportText() const { return wxGetEmptyString(); }
    
    mmPanelBase() {}
    mmPanelBase(wxSQLite3Database* db, wxSQLite3Database* inidb, mmCoreDB* core = 0): db_(db), inidb_(inidb), core_(core) {}

    wxSQLite3Database* db_; //TODO remove
    wxSQLite3Database* inidb_;
    mmCoreDB* core_;
};
//----------------------------------------------------------------------------
#endif // _MM_EX_PANELBASE_H_
//----------------------------------------------------------------------------
