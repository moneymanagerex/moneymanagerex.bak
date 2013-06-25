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

#ifndef _MM_EX_PANELBASE_H_
#define _MM_EX_PANELBASE_H_

#include "mmcoredb.h"
#include "util.h"
#include <wx/listctrl.h>
//----------------------------------------------------------------------------
class wxSQLite3Database;
class wxListItemAttr;

struct mmHolderBase
{
    int id_;
    wxString valueStr_;
    double value_;
};

class mmListCtrl: public wxListCtrl
{
public:
    mmListCtrl(wxWindow *parent,
                wxWindowID winid = wxID_ANY,
                const wxPoint& pos = wxDefaultPosition,
                const wxSize& size = wxDefaultSize,
                long style = wxLC_REPORT): wxListCtrl(parent, winid, pos, size, style)
                , attr1_(new wxListItemAttr(mmColors::listBorderColor, mmColors::listAlternativeColor0, wxNullFont))
                , attr2_(new wxListItemAttr(mmColors::listBorderColor, mmColors::listAlternativeColor1, wxNullFont))
                {}
    virtual ~mmListCtrl()
    {
        if (attr1_) delete attr1_;
        if (attr2_) delete attr2_;
    }
public:
    wxListItemAttr *attr1_, *attr2_; // style1
public:
    virtual wxListItemAttr* OnGetItemAttr(long row) const
    {
        return row % 2 ? attr2_ : attr1_;
    }
};

class mmPanelBase : public wxPanel
{
public: 
    mmPanelBase() {}
    mmPanelBase(mmCoreDB* core)
    : core_(core)
    {}

    mmCoreDB* core_;
public:
    void windowsFreezeThaw()
    {
        #ifdef __WXGTK__
        return;
        #endif

        if (this->IsFrozen()) 
            this->Thaw();
        else 
            this->Freeze();
    }
};
//----------------------------------------------------------------------------
#endif // _MM_EX_PANELBASE_H_
//----------------------------------------------------------------------------
