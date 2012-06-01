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
#include "dbwrapper.h"
#include "util.h" // XXX
//----------------------------------------------------------------------------
/* Include XPM Support */
#include "../resources/exefile.xpm"
#include "../resources/flag.xpm"
#include "../resources/void.xpm"
#include "../resources/reconciled.xpm"
#include "../resources/unreconciled.xpm"
#include "../resources/uparrow.xpm"
#include "../resources/downarrow.xpm"
#include "../resources/rightarrow.xpm"
#include "../resources/duplicate.xpm"
#include "../resources/trans_from.xpm"
#include "../resources/trans_into.xpm"
#include "../resources/tipicon.xpm"
#include "../resources/empty.xpm"
#include "../resources/trans_transfer.xpm"
#include "../resources/assets.xpm"
//----------------------------------------------------------------------------
#define _MM_EX_PANELBASE_H_REVISION_ID    "$Revision$"

class wxSQLite3Database;
class wxListCtrl;

enum EIcons
{
    ICON_RECONCILED,
    ICON_VOID,
    ICON_FOLLOWUP,
    ICON_NONE,
    ICON_UNRECONCILED,
    ICON_DESC,
    ICON_ASC,
    ICON_DUPLICATE,
    ICON_TRANS_WITHDRAWAL,
    ICON_TRANS_DEPOSIT,
    ICON_TRANS_TRANSFER
    , ICON_ASSET
};

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
        , m_imageList(new wxImageList(16, 16))
        , m_attr1(new wxListItemAttr(mmColors::listBorderColor, mmColors::listAlternativeColor0, wxNullFont))
        , m_attr2(new wxListItemAttr(mmColors::listBorderColor, mmColors::listAlternativeColor1, wxNullFont))
    {
        m_imageList->Add(wxBitmap(reconciled_xpm));
        m_imageList->Add(wxBitmap(void_xpm));
        m_imageList->Add(wxBitmap(flag_xpm));
        m_imageList->Add(wxBitmap(empty_xpm));
        m_imageList->Add(wxBitmap(unreconciled_xpm));
        m_imageList->Add(wxBitmap(uparrow_xpm));
        m_imageList->Add(wxBitmap(downarrow_xpm));
        m_imageList->Add(wxBitmap(duplicate_xpm));
        m_imageList->Add(wxBitmap(trans_from_xpm));
        m_imageList->Add(wxBitmap(trans_into_xpm));
        m_imageList->Add(wxBitmap(trans_transfer_xpm));
        m_imageList->Add(wxBitmap(assets_xpm));
        
        SetImageList(m_imageList, wxIMAGE_LIST_SMALL);
    }

    virtual ~mmListCtrl() 
    {
        if (m_imageList) delete m_imageList;
        if (m_attr1) delete m_attr1;
        if (m_attr2) delete m_attr2;
    }

public:
    wxImageList* m_imageList;
    wxListItemAttr* m_attr1;
    wxListItemAttr* m_attr2;
    std::vector<int> m_columns;
    std::vector<wxString> m_column_headers;
    std::vector<int> m_column_width;

public:
    virtual void SetColumnImage(int col, int image)
    {
        for (int i = 0; i < GetColumnCount(); ++i)
        {
            wxListItem item;
            item.SetMask(wxLIST_MASK_IMAGE);
            if (i == col)
                item.SetImage(image);
            else
                item.SetImage(-1);

            SetColumn(i, item);
        }
    }
    virtual wxListItemAttr* OnGetItemAttr(long item) const
    {
        return item % 2 ? m_attr2 : m_attr1;
    }
};

class mmPanelBase : public wxPanel
{
public: 
    virtual wxString getReportText() const { return wxGetEmptyString(); }
    
    mmPanelBase() {}
    ~mmPanelBase() {}
    mmPanelBase(wxSQLite3Database* db, wxSQLite3Database* inidb, mmCoreDB* core = 0): db_(db), inidb_(inidb), core_(core) {}

    wxSQLite3Database* db_; //TODO remove
    wxSQLite3Database* inidb_;
    mmCoreDB* core_;
public:
    virtual void save_config(const mmListCtrl* list_ctrl, const wxString& module = wxT("mmPanelBase"))
    {
        inidb_->Begin();
        for (int i = 0; i < list_ctrl->GetColumnCount(); ++i) 
        {
            int width = list_ctrl->GetColumnWidth(i);
            mmDBWrapper::setINISettingValue(inidb_, wxString::Format(wxT("%s_COL%d_WIDTH"), module.c_str(), i), wxString() << width); 
        }
        inidb_->Commit();
    }

    virtual wxString get_version() const
    {
        return wxT(_MM_EX_PANELBASE_H_REVISION_ID);
    }
};
//----------------------------------------------------------------------------
#endif // _MM_EX_PANELBASE_H_
//----------------------------------------------------------------------------
