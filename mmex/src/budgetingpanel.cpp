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
 /*******************************************************/
#include "budgetingpanel.h"
#include "budgetentrydialog.h"
#include "util.h"
#include "dbwrapper.h"
#include <algorithm>
#include <vector>
/*******************************************************/
/* Include XPM Support */
#include "../resources/exefile.xpm"
#include "../resources/flag.xpm"
#include "../resources/void.xpm"
#include "../resources/reconciled.xpm"
#include "../resources/unreconciled.xpm"
#include "../resources/uparrow.xpm"
#include "../resources/downarrow.xpm"
#include "../resources/rightarrow.xpm"
/*******************************************************/
static int sortcol = 0;
static bool asc = true;
/*******************************************************/
BEGIN_EVENT_TABLE(mmBudgetingPanel, wxPanel)
    EVT_LEFT_DOWN( mmBudgetingPanel::OnMouseLeftDown ) 

    EVT_MENU(MENU_VIEW_ALLBUDGETENTRIES, mmBudgetingPanel::OnViewPopupSelected)
    EVT_MENU(MENU_VIEW_NONZEROBUDGETENTRIES, mmBudgetingPanel::OnViewPopupSelected)
    EVT_MENU(MENU_VIEW_INCOMEBUDGETENTRIES, mmBudgetingPanel::OnViewPopupSelected)
    EVT_MENU(MENU_VIEW_EXPENSEBUDGETENTRIES, mmBudgetingPanel::OnViewPopupSelected)
END_EVENT_TABLE()
/*******************************************************/
BEGIN_EVENT_TABLE(budgetingListCtrl, wxListCtrl)
    EVT_LIST_ITEM_SELECTED(ID_PANEL_CHECKING_LISTCTRL_ACCT, budgetingListCtrl::OnListItemSelected)
    EVT_LIST_ITEM_ACTIVATED(ID_PANEL_CHECKING_LISTCTRL_ACCT, budgetingListCtrl::OnListItemActivated)

END_EVENT_TABLE()
/*******************************************************/
mmBudgetingPanel::mmBudgetingPanel(wxSQLite3Database* db, wxSQLite3Database* inidb, int budgetYearID, wxWindow *parent,
            wxWindowID winid, const wxPoint& pos, const wxSize& size, long style,
            const wxString& name )
            : db_(db), budgetYearID_(budgetYearID), inidb_(inidb), m_imageList(0)
{
    Create(parent, winid, pos, size, style, name);
    currentView_ = wxT("View All Budget Categories");
}

bool mmBudgetingPanel::Create( wxWindow *parent,
            wxWindowID winid, const wxPoint& pos, 
            const wxSize& size,long style, const wxString& name  )
{
    SetExtraStyle(GetExtraStyle()|wxWS_EX_BLOCK_EVENTS);
    wxPanel::Create(parent, winid, pos, size, style, name);

    this->Freeze();
    CreateControls();
    GetSizer()->Fit(this);
    GetSizer()->SetSizeHints(this);
    
    initVirtualListControl();

    this->Thaw();
    return TRUE;
}

mmBudgetingPanel::~mmBudgetingPanel()
{
    
    if (m_imageList)
        delete m_imageList;

    long col0, col1, col2, col3, col4, col5;
    col0 = listCtrlAccount_->GetColumnWidth(0);
    col1 = listCtrlAccount_->GetColumnWidth(1);
    col2 = listCtrlAccount_->GetColumnWidth(2);
    col3 = listCtrlAccount_->GetColumnWidth(3);
    col4 = listCtrlAccount_->GetColumnWidth(4);
    col5 = listCtrlAccount_->GetColumnWidth(5);

    wxString col0Str = wxString::Format(wxT("%d"), col0);
    wxString col1Str = wxString::Format(wxT("%d"), col1);
    wxString col2Str = wxString::Format(wxT("%d"), col2);
    wxString col3Str = wxString::Format(wxT("%d"), col3);
    wxString col4Str = wxString::Format(wxT("%d"), col4);
    wxString col5Str = wxString::Format(wxT("%d"), col5);

    mmDBWrapper::setINISettingValue(inidb_, wxT("BUDGET_COL0_WIDTH"), col0Str); 
    mmDBWrapper::setINISettingValue(inidb_, wxT("BUDGET_COL1_WIDTH"), col1Str); 
    mmDBWrapper::setINISettingValue(inidb_, wxT("BUDGET_COL2_WIDTH"), col2Str); 
    mmDBWrapper::setINISettingValue(inidb_, wxT("BUDGET_COL3_WIDTH"), col3Str); 
    mmDBWrapper::setINISettingValue(inidb_, wxT("BUDGET_COL4_WIDTH"), col4Str); 
    mmDBWrapper::setINISettingValue(inidb_, wxT("BUDGET_COL5_WIDTH"), col5Str); 
}

void mmBudgetingPanel::OnViewPopupSelected(wxCommandEvent& event)
{
    int evt =  event.GetId();
    wxStaticText* header = (wxStaticText*)FindWindow(ID_PANEL_CHECKING_STATIC_PANELVIEW);
    if (evt ==  MENU_VIEW_ALLBUDGETENTRIES)
    {
        header->SetLabel(_("View All Budget Categories"));
        currentView_ = wxT("View All Budget Categories");
    }
    else if (evt == MENU_VIEW_NONZEROBUDGETENTRIES)
    {
        header->SetLabel(_("View Non-Zero Budget Categories"));
        currentView_ = wxT("View Non-Zero Budget Categories");
    }
    else if (evt == MENU_VIEW_INCOMEBUDGETENTRIES)
    {
        header->SetLabel(_("View Income Budget Categories"));
        currentView_ = wxT("View Income Budget Categories");
    }
    else if (evt == MENU_VIEW_EXPENSEBUDGETENTRIES)
    {
        header->SetLabel(_("View Expense Budget Categories"));
        currentView_ = wxT("View Expense Budget Categories");
    }
    else
    {
        wxASSERT(false);
    }

    listCtrlAccount_->DeleteAllItems();
    initVirtualListControl();
    listCtrlAccount_->RefreshItems(0, ((int)trans_.size()) - 1);
}


void mmBudgetingPanel::OnMouseLeftDown( wxMouseEvent& event )
{
    // depending on the clicked control's window id.
    switch( event.GetId() ) 
    {
    case ID_PANEL_BUDGETENTRY_STATIC_BITMAP_VIEW : 
        {
            wxMenu menu;
            menu.Append(MENU_VIEW_ALLBUDGETENTRIES, _("View All Budget Categories"));
            menu.Append(MENU_VIEW_NONZEROBUDGETENTRIES, _("View Non-Zero Budget Categories"));
            menu.Append(MENU_VIEW_INCOMEBUDGETENTRIES, _("View Income Budget Categories"));
            menu.Append(MENU_VIEW_EXPENSEBUDGETENTRIES, _("View Expense Budget Categories"));
            PopupMenu(&menu, event.GetPosition());
            break;
        }
    }
    event.Skip();
} 

void mmBudgetingPanel::CreateControls()
{    
    mmBudgetingPanel* itemDialog1 = this;

    wxBoxSizer* itemBoxSizer2 = new wxBoxSizer(wxVERTICAL);
    itemDialog1->SetSizer(itemBoxSizer2);

    wxPanel* itemPanel3 = new wxPanel( itemDialog1, ID_PANEL_REPORTS_HEADER_PANEL, 
        wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
    itemBoxSizer2->Add(itemPanel3, 0, wxGROW|wxALL, 5);

    wxBoxSizer* itemBoxSizerVHeader = new wxBoxSizer(wxVERTICAL);
    itemPanel3->SetSizer(itemBoxSizerVHeader);

    wxString yearStr = mmDBWrapper::getBudgetYearForID(db_, budgetYearID_);
    wxStaticText* itemStaticText9 = new wxStaticText( itemPanel3, ID_PANEL_REPORTS_STATIC_HEADER, 
        _("Budget Setup for ") + yearStr, wxDefaultPosition, wxDefaultSize, 0 );
    itemStaticText9->SetFont(wxFont(12, wxSWISS, wxNORMAL, wxBOLD, FALSE, 
        wxT("")));
    itemBoxSizerVHeader->Add(itemStaticText9, 0, wxALL, 1);

     wxBoxSizer* itemBoxSizerHHeader2 = new wxBoxSizer(wxHORIZONTAL);
    itemBoxSizerVHeader->Add(itemBoxSizerHHeader2, 0, wxALL, 1);

    wxBitmap itemStaticBitmap3Bitmap(rightarrow_xpm);
    wxStaticBitmap* itemStaticBitmap3 = new wxStaticBitmap( itemPanel3, 
        ID_PANEL_BUDGETENTRY_STATIC_BITMAP_VIEW, 
        itemStaticBitmap3Bitmap, wxDefaultPosition, wxSize(16, 16), 0 );
    itemBoxSizerHHeader2->Add(itemStaticBitmap3, 0, wxALIGN_CENTER_VERTICAL|wxALL, 1);
    itemStaticBitmap3->SetEventHandler( this ); 

     wxStaticText* itemStaticText18 = new wxStaticText( itemPanel3, 
            ID_PANEL_CHECKING_STATIC_PANELVIEW, 
            _("Viewing All Budget Categories"), wxDefaultPosition, wxDefaultSize, 0 );
    itemBoxSizerHHeader2->Add(itemStaticText18, 0, wxALL, 1);

     wxStaticText* itemStaticText10 = new wxStaticText( itemPanel3, 
         ID_DIALOG_BUDGETENTRY_SUMMARY_INCOME, 
         _("Estimated Income: Actual Income: "), wxDefaultPosition, wxDefaultSize, 0 );
    itemStaticText10->SetFont(wxFont(10, wxSWISS, wxNORMAL, wxNORMAL, FALSE, 
        wxT("")));
    itemBoxSizerVHeader->Add(itemStaticText10, 0, wxALL, 1);

    wxStaticText* itemStaticText11 = new wxStaticText( itemPanel3, 
        ID_DIALOG_BUDGETENTRY_SUMMARY_EXPENSES, 
        _("Estimated Expenses: Actual Expenses: "), wxDefaultPosition, wxDefaultSize, 0 );
    itemStaticText11->SetFont(wxFont(10, wxSWISS, wxNORMAL, wxNORMAL, FALSE, 
        wxT("")));
    itemBoxSizerVHeader->Add(itemStaticText11, 0, wxALL, 1);

    /* ---------------------- */

    wxSize imageSize(16, 16);
    m_imageList = new wxImageList( imageSize.GetWidth(), 
        imageSize.GetHeight() );
    m_imageList->Add(wxBitmap(reconciled_xpm));
    m_imageList->Add(wxBitmap(void_xpm));
    m_imageList->Add(wxBitmap(flag_xpm));
    
    listCtrlAccount_ = new budgetingListCtrl( this, itemDialog1, 
        ID_PANEL_CHECKING_LISTCTRL_ACCT, wxDefaultPosition, wxDefaultSize, 
        wxLC_REPORT | wxLC_HRULES | wxLC_VRULES | wxLC_VIRTUAL | wxLC_SINGLE_SEL  );
    listCtrlAccount_->SetBackgroundColour(mmColors::listBackColor);
    listCtrlAccount_->SetImageList(m_imageList, wxIMAGE_LIST_SMALL);
    listCtrlAccount_->InsertColumn(0, _("Category  "));
    listCtrlAccount_->InsertColumn(1, _("Sub Category"));
    listCtrlAccount_->InsertColumn(2, _("Frequency"));

    wxListItem itemCol;
    itemCol.SetImage(-1);
    itemCol.SetAlign(wxLIST_FORMAT_RIGHT);
    itemCol.SetText(_("Amount"));
    listCtrlAccount_->InsertColumn(3, itemCol);

    itemCol.SetText(_("Estimated"));
    listCtrlAccount_->InsertColumn(4, itemCol);

    itemCol.SetText(_("Actual"));
    listCtrlAccount_->InsertColumn(5, itemCol);

    /* See if we can get data from inidb */
     long col0, col1, col2, col3, col4, col5;
     mmDBWrapper::getINISettingValue(inidb_, 
        wxT("BUDGET_COL0_WIDTH"), wxT("80")).ToLong(&col0); 
     mmDBWrapper::getINISettingValue(inidb_, 
         wxT("BUDGET_COL1_WIDTH"), wxT("80")).ToLong(&col1); 
     mmDBWrapper::getINISettingValue(inidb_, 
         wxT("BUDGET_COL2_WIDTH"), wxT("80")).ToLong(&col2); 
     mmDBWrapper::getINISettingValue(inidb_, 
         wxT("BUDGET_COL3_WIDTH"), wxT("80")).ToLong(&col3); 
     mmDBWrapper::getINISettingValue(inidb_, 
         wxT("BUDGET_COL4_WIDTH"), wxT("80")).ToLong(&col4); 
     mmDBWrapper::getINISettingValue(inidb_, 
         wxT("BUDGET_COL5_WIDTH"), wxT("80")).ToLong(&col5); 

    listCtrlAccount_->SetColumnWidth(0, col0);
    listCtrlAccount_->SetColumnWidth(1, col1);
    listCtrlAccount_->SetColumnWidth(2, col2);
    listCtrlAccount_->SetColumnWidth(3, col3);
    listCtrlAccount_->SetColumnWidth(4, col4);
    listCtrlAccount_->SetColumnWidth(5, col5);

    itemBoxSizer2->Add(listCtrlAccount_, 1, wxGROW | wxALL, 1);
}

void mmBudgetingPanel::initVirtualListControl()
{
    trans_.clear();

    double estIncome = 0.0;
    double estExpenses = 0.0;
    double actIncome = 0.0;
    double actExpenses = 0.0;

    long year = 0;
    mmDBWrapper::getBudgetYearForID(db_, budgetYearID_).ToLong(&year);;
    wxDateTime dtBegin(1, wxDateTime::Jan, year);
    wxDateTime dtEnd(31, wxDateTime::Dec, year);

    mmBEGINSQL_LITE_EXCEPTION;
    mmDBWrapper::loadBaseCurrencySettings(db_);

    wxSQLite3StatementBuffer bufSQL;
    bufSQL.Format("select * from CATEGORY_V1 order by CATEGNAME;");
    wxSQLite3ResultSet q1 = db_->ExecuteQuery(bufSQL);
    while (q1.NextRow())
    {
        mmBudgetEntryHolder th;
        th.categID_ = q1.GetInt(wxT("CATEGID"));
        th.catStr_ = q1.GetString(wxT("CATEGNAME"));
        th.subcategID_ = -1;
        th.subCatStr_ = wxT("");
        th.amt_ = 0.0;
        th.period_ = wxT("None");
        th.estimatedStr_ = wxT("0.0");
        th.estimated_ = 0.0;
        th.actualStr_ = wxT("0.0");
        th.actual_ = 0.0;
        mmDBWrapper::getBudgetEntry(db_, budgetYearID_, th.categID_, th.subcategID_, th.period_, 
            th.amt_);

        if (th.period_ == wxT("Monthly"))
        {
            th.estimated_ = th.amt_ * 12;
        }
        else if (th.period_ == wxT("Yearly"))
        {
            th.estimated_ = th.amt_;
        }
        else if (th.period_ == wxT("Weekly"))
        {
            th.estimated_ = th.amt_ * 52;
        }
        else if (th.period_ == wxT("Bi-Weekly"))
        {
            th.estimated_ = th.amt_ * 26;
        }
        else if (th.period_ == wxT("Bi-Monthly"))
        {
            th.estimated_ = th.amt_ * 6;
        }
         else if (th.period_ == wxT("Quarterly"))
        {
            th.estimated_ = th.amt_ * 4;
        }
           else if (th.period_ == wxT("Half-Yearly"))
        {
            th.estimated_ = th.amt_ * 2;
        }
        else
            wxASSERT(true);

        if (th.estimated_ < 0)
            estExpenses += th.estimated_;
        else
            estIncome += th.estimated_;

        mmCurrencyFormatter::formatDoubleToCurrencyEdit(th.estimated_, th.estimatedStr_);

        th.actual_ = mmDBWrapper::getAmountForCategory(db_, th.categID_, th.subcategID_, 
                    false,  dtBegin, dtEnd);
        mmCurrencyFormatter::formatDoubleToCurrencyEdit(th.actual_, th.actualStr_);

        if (th.actual_ < 0)
            actExpenses += th.actual_;
        else
            actIncome += th.actual_;

        wxString displayAmtString;
        if (mmCurrencyFormatter::formatDoubleToCurrencyEdit(th.amt_, displayAmtString))
            th.amtString_ = displayAmtString;

        if (currentView_ == wxT("View Non-Zero Budget Categories"))
        {
            if ((th.estimated_ == 0.0) && (th.actual_ == 0.0))
            {
            }
            else
                trans_.push_back(th);
        }
        else if (currentView_ == wxT("View Income Budget Categories"))
        {
            if ((th.estimated_ > 0.0) || (th.actual_ > 0.0))
            {
                trans_.push_back(th);
            }
        }
        else if (currentView_ == wxT("View Expense Budget Categories"))
        {
            if ((th.estimated_ < 0.0) || (th.actual_ < 0.0))
                trans_.push_back(th);
        }
        else
            trans_.push_back(th);

        wxSQLite3StatementBuffer bufSQL1;
        bufSQL1.Format("select * from SUBCATEGORY_V1 where CATEGID=%d;", th.categID_);
        wxSQLite3ResultSet q2 = db_->ExecuteQuery(bufSQL1); 
        bool hasSubCateg = false;
        while(q2.NextRow())
        {
            mmBudgetEntryHolder thsub;
            thsub.categID_ = q1.GetInt(wxT("CATEGID"));
            thsub.catStr_ = q1.GetString(wxT("CATEGNAME"));
            thsub.subcategID_ = q2.GetInt(wxT("SUBCATEGID"));
            thsub.subCatStr_   = q2.GetString(wxT("SUBCATEGNAME"));
            thsub.amt_ = 0.0;
            thsub.period_ = wxT("None");
            thsub.estimatedStr_ = wxT("0.0");
            thsub.estimated_ = 0.0;
            thsub.actualStr_ = wxT("0.0");
            thsub.actual_ = 0.0;
            mmDBWrapper::getBudgetEntry(db_, budgetYearID_, thsub.categID_, thsub.subcategID_, thsub.period_, 
                thsub.amt_);

            if (thsub.period_ == wxT("Monthly"))
            {
                thsub.estimated_ = thsub.amt_ * 12;
            }
            else if (thsub.period_ == wxT("Yearly"))
            {
                thsub.estimated_ = thsub.amt_;
            }
            else if (thsub.period_ == wxT("Weekly"))
            {
                thsub.estimated_ = thsub.amt_ * 52;
            }
            else if (thsub.period_ == wxT("Bi-Weekly"))
            {
                thsub.estimated_ = thsub.amt_ * 26;
            }
            else if (thsub.period_ == wxT("Bi-Monthly"))
            {
                thsub.estimated_ = thsub.amt_ * 6;
            }
            else if (thsub.period_ == wxT("Quarterly"))
            {
                thsub.estimated_ = thsub.amt_ * 4;
            }
            else if (thsub.period_ == wxT("Half-Yearly"))
            {
                thsub.estimated_ = thsub.amt_ * 2;
            }
            else
                wxASSERT(true);

            mmCurrencyFormatter::formatDoubleToCurrencyEdit(thsub.estimated_, thsub.estimatedStr_);
            if (thsub.estimated_ < 0)
                estExpenses += thsub.estimated_;
            else
                estIncome += thsub.estimated_;

            thsub.actual_ = mmDBWrapper::getAmountForCategory(db_, thsub.categID_, thsub.subcategID_, 
                false,  dtBegin, dtEnd);
            mmCurrencyFormatter::formatDoubleToCurrencyEdit(thsub.actual_, thsub.actualStr_);
            if (thsub.actual_ < 0)
                actExpenses += thsub.actual_;
            else
                actIncome += thsub.actual_;


            if (mmCurrencyFormatter::formatDoubleToCurrencyEdit(thsub.amt_, displayAmtString))
                thsub.amtString_ = displayAmtString;

            if (currentView_ == wxT("View Non-Zero Budget Categories"))
            {
                if ((thsub.estimated_ == 0.0) && (thsub.actual_ == 0.0))
                {
                }
                else
                    trans_.push_back(thsub);
            }
            else if (currentView_ == wxT("View Income Budget Categories"))
            {
                if ((thsub.estimated_ > 0.0) || (thsub.actual_ > 0.0))
                {
                    trans_.push_back(thsub);
                }
            }
            else if (currentView_ == wxT("View Expense Budget Categories"))
            {
                if ((thsub.estimated_ < 0.0) || (thsub.actual_ < 0.0))
                    trans_.push_back(thsub);
            }
            else
                trans_.push_back(thsub);

        }
        q2.Finalize();
    }
    q1.Finalize();

    listCtrlAccount_->SetItemCount((int)trans_.size());
    
    mmENDSQL_LITE_EXCEPTION;

    wxString estIncomeStr, actIncomeStr,  estExpensesStr, actExpensesStr;
    mmCurrencyFormatter::formatDoubleToCurrency(estIncome, estIncomeStr);
    mmCurrencyFormatter::formatDoubleToCurrency(actIncome, actIncomeStr);
    if (estExpenses < 0.0)
        estExpenses = -estExpenses;
    if (actExpenses < 0.0)
        actExpenses = -actExpenses;
    mmCurrencyFormatter::formatDoubleToCurrency(estExpenses, estExpensesStr);
    mmCurrencyFormatter::formatDoubleToCurrency(actExpenses, actExpensesStr);

    wxStaticText* header = (wxStaticText*)FindWindow(ID_DIALOG_BUDGETENTRY_SUMMARY_INCOME);
    wxString incStr = wxString::Format(_("Estimated Income: %s Actual Income:  %s"), 
        estIncomeStr.c_str(), actIncomeStr.c_str());
    header->SetLabel(incStr);

    header = (wxStaticText*)FindWindow(ID_DIALOG_BUDGETENTRY_SUMMARY_EXPENSES);
    wxString expStr = wxString::Format(_("Estimated Expenses: %s Actual Expenses:  %s"), 
        estExpensesStr.c_str(), actExpensesStr.c_str());
    header->SetLabel(expStr);
}

/*******************************************************/
void budgetingListCtrl::OnListItemSelected(wxListEvent& event)
{
    selectedIndex_ = event.GetIndex();
}

wxString mmBudgetingPanel::getItem(long item, long column)
{
    if (column == 0)
        return trans_[item].catStr_;

    if (column == 1)
        return trans_[item].subCatStr_;

    if (column == 2)
        return trans_[item].period_;

    if (column == 3)
        return trans_[item].amtString_;

    if (column == 4)
        return trans_[item].estimatedStr_;

    if (column == 5)
        return trans_[item].actualStr_;

    return wxT("");
}

int budgetingListCtrl::OnGetItemImage(long item) const
{
    if ((cp_->trans_[item].estimated_ == 0.0) && (cp_->trans_[item].actual_ == 0.0))
       return -1;

    if ((cp_->trans_[item].estimated_ == 0.0) && (cp_->trans_[item].actual_ != 0.0))
       return 2;

   if (cp_->trans_[item].estimated_ < cp_->trans_[item].actual_)
       return 0;

   return 1;
}


wxString budgetingListCtrl::OnGetItemText(long item, long column) const
{
    return cp_->getItem(item, column);
}

wxListItemAttr* budgetingListCtrl::OnGetItemAttr(long item) const
{
    /* Returns the alternating background pattern */
    return item % 2 ? (wxListItemAttr *)&attr2_ : (wxListItemAttr *)&attr1_;
}

void budgetingListCtrl::OnListItemActivated(wxListEvent& event)
{
    selectedIndex_ = event.GetIndex();

    mmBudgetEntryDialog *dlg = new mmBudgetEntryDialog(cp_->db_, cp_->budgetYearID_, 
        cp_->trans_[selectedIndex_].categID_, cp_->trans_[selectedIndex_].subcategID_, this);
    if ( dlg->ShowModal() == wxID_OK )
    {
        cp_->initVirtualListControl();
        RefreshItem(selectedIndex_);
    }
    dlg->Destroy();
}
