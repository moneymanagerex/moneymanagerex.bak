/*******************************************************
 Copyright (C) 2006 Madhan Kanagavel
 Copyright (C) 2012 Stefano Giorgio

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

#include "budgetingpanel.h"
#include "budgetentrydialog.h"
#include "util.h"
#include "dbwrapper.h"
#include <algorithm>
#include "mmex.h"
#include <vector>
#include "reportbudget.h"

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
#include "../resources/empty.xpm"
/*******************************************************/
BEGIN_EVENT_TABLE(mmBudgetingPanel, wxPanel)
    EVT_LEFT_DOWN( mmBudgetingPanel::OnMouseLeftDown ) 

    EVT_MENU(MENU_VIEW_ALLBUDGETENTRIES, mmBudgetingPanel::OnViewPopupSelected)
    EVT_MENU(MENU_VIEW_NONZEROBUDGETENTRIES, mmBudgetingPanel::OnViewPopupSelected)
    EVT_MENU(MENU_VIEW_INCOMEBUDGETENTRIES, mmBudgetingPanel::OnViewPopupSelected)
    EVT_MENU(MENU_VIEW_EXPENSEBUDGETENTRIES, mmBudgetingPanel::OnViewPopupSelected)
    EVT_MENU(MENU_VIEW_SUMMARYBUDGETENTRIES, mmBudgetingPanel::OnViewPopupSelected)
END_EVENT_TABLE()
/*******************************************************/
BEGIN_EVENT_TABLE(budgetingListCtrl, wxListCtrl)
    EVT_LIST_ITEM_SELECTED(ID_PANEL_CHECKING_LISTCTRL_ACCT, budgetingListCtrl::OnListItemSelected)
    EVT_LIST_ITEM_ACTIVATED(ID_PANEL_CHECKING_LISTCTRL_ACCT, budgetingListCtrl::OnListItemActivated)

END_EVENT_TABLE()
/*******************************************************/
mmBudgetingPanel::mmBudgetingPanel(wxSQLite3Database* db, wxSQLite3Database* inidb, mmGUIFrame* mainFrame, int budgetYearID, wxWindow *parent,
            wxWindowID winid, const wxPoint& pos, const wxSize& size, long style,
            const wxString& name 
           ) : 
    mmPanelBase(db, inidb),
    m_imageList(), 
    listCtrlAccount_(),
    mainFrame_(mainFrame),
    budgetYearID_(budgetYearID)
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
    if (m_imageList) delete m_imageList;

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

    mmDBWrapper::begin(inidb_);
    mmDBWrapper::setINISettingValue(inidb_, wxT("BUDGET_COL0_WIDTH"), col0Str); 
    mmDBWrapper::setINISettingValue(inidb_, wxT("BUDGET_COL1_WIDTH"), col1Str); 
    mmDBWrapper::setINISettingValue(inidb_, wxT("BUDGET_COL2_WIDTH"), col2Str); 
    mmDBWrapper::setINISettingValue(inidb_, wxT("BUDGET_COL3_WIDTH"), col3Str); 
    mmDBWrapper::setINISettingValue(inidb_, wxT("BUDGET_COL4_WIDTH"), col4Str); 
    mmDBWrapper::setINISettingValue(inidb_, wxT("BUDGET_COL5_WIDTH"), col5Str); 
    mmDBWrapper::commit(inidb_);
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
    else if (evt == MENU_VIEW_SUMMARYBUDGETENTRIES) 
    {
        header->SetLabel(_("View Budget Category Summary"));
        currentView_ = wxT("View Budget Category Summary");
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
            menu.AppendSeparator();
            menu.Append(MENU_VIEW_SUMMARYBUDGETENTRIES, _("View Budget Category Summary"));
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
    if ((yearStr.length() < 5)) 
    {
        if ( mainFrame_->budgetFinancialYears() ) 
        {
            long year;
            yearStr.ToLong(&year);
            year++;
            yearStr = wxString() << _("Financial Year: ") << yearStr << wxT(" - ") << year;
        } 
        else
        {
            yearStr = wxString() << _("Year: ") << yearStr;
        }
    } 
    else 
    {
        yearStr = wxString() << _("Month: ") << yearStr;
    }
    wxStaticText* itemStaticText9 = new wxStaticText( itemPanel3, ID_PANEL_REPORTS_STATIC_HEADER, 
        _("Budget Setup for ") + yearStr, wxDefaultPosition, wxDefaultSize, 0 );
    itemStaticText9->SetFont(wxFont(12, wxSWISS, wxNORMAL, wxBOLD, FALSE, wxT("")));
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

    wxSize distSize(170, 20);

    wxStaticText* itemStaticText100 = new wxStaticText( itemPanel3, wxID_ANY, _("Income......."),
        wxDefaultPosition, wxSize(75, 20), 0 );
    itemStaticText100->SetFont(wxFont(10, wxSWISS, wxNORMAL, wxNORMAL, FALSE, wxT("")));

    wxStaticText* itemStaticText101 = new wxStaticText( itemPanel3,
        ID_DIALOG_BUDGETENTRY_SUMMARY_INCOME_EST, _("Estimated: "), wxDefaultPosition, distSize, 0);
    itemStaticText101->SetFont(wxFont(10, wxSWISS, wxNORMAL, wxNORMAL, FALSE, wxT("")));

    wxStaticText* itemStaticText102 = new wxStaticText( itemPanel3, 
        ID_DIALOG_BUDGETENTRY_SUMMARY_INCOME_ACT, _("Actual: "), wxDefaultPosition, distSize, 0);
    itemStaticText102->SetFont(wxFont(10, wxSWISS, wxNORMAL, wxNORMAL, FALSE, wxT("")));

    wxStaticText* itemStaticText103 = new wxStaticText( itemPanel3, 
        ID_DIALOG_BUDGETENTRY_SUMMARY_INCOME_DIF, _("Diference: "), wxDefaultPosition, distSize, 0);
    itemStaticText103->SetFont(wxFont(10, wxSWISS, wxNORMAL, wxNORMAL, FALSE, wxT("")));

    wxBoxSizer* itemIncomeSizer = new wxBoxSizer(wxHORIZONTAL);
    itemIncomeSizer->Add(itemStaticText100, 0, wxALL, 0);
    itemIncomeSizer->Add(itemStaticText101, 0, wxALL, 0);
    itemIncomeSizer->Add(itemStaticText102, 0, wxALL, 0);
    itemIncomeSizer->Add(itemStaticText103, 0, wxALL, 0);
    itemBoxSizerVHeader->Add(itemIncomeSizer, 0, wxALL, 1);

    wxStaticText* itemStaticText200 = new wxStaticText( itemPanel3, wxID_ANY, _("Expenses..."),
        wxDefaultPosition, wxSize(75, 20), 0 );
    itemStaticText200->SetFont(wxFont(10, wxSWISS, wxNORMAL, wxNORMAL, FALSE, wxT("")));

    wxStaticText* itemStaticText201 = new wxStaticText( itemPanel3, 
        ID_DIALOG_BUDGETENTRY_SUMMARY_EXPENSES_EST, _("Estimated: "), wxDefaultPosition, distSize, 0);
    itemStaticText201->SetFont(wxFont(10, wxSWISS, wxNORMAL, wxNORMAL, FALSE, wxT("")));

    wxStaticText* itemStaticText202 = new wxStaticText( itemPanel3, 
        ID_DIALOG_BUDGETENTRY_SUMMARY_EXPENSES_ACT, _("Actual: "), wxDefaultPosition, distSize, 0);
    itemStaticText202->SetFont(wxFont(10, wxSWISS, wxNORMAL, wxNORMAL, FALSE, wxT("")));

    wxStaticText* itemStaticText203 = new wxStaticText( itemPanel3, 
        ID_DIALOG_BUDGETENTRY_SUMMARY_EXPENSES_DIF, _("Diference: "), wxDefaultPosition, distSize, 0);
    itemStaticText203->SetFont(wxFont(10, wxSWISS, wxNORMAL, wxNORMAL, FALSE, wxT("")));

    wxBoxSizer* itemExpenseSizer = new wxBoxSizer(wxHORIZONTAL);
    itemExpenseSizer->Add(itemStaticText200, 0, wxALL, 0);
    itemExpenseSizer->Add(itemStaticText201, 0, wxALL, 0);
    itemExpenseSizer->Add(itemStaticText202, 0, wxALL, 0);
    itemExpenseSizer->Add(itemStaticText203, 0, wxALL, 0);
    itemBoxSizerVHeader->Add(itemExpenseSizer, 0, wxALL, 1);
    /* ---------------------- */

    wxSize imageSize(16, 16);
    m_imageList = new wxImageList( imageSize.GetWidth(), imageSize.GetHeight() );
    m_imageList->Add(wxBitmap(reconciled_xpm));
    m_imageList->Add(wxBitmap(void_xpm));
    m_imageList->Add(wxBitmap(flag_xpm));
    m_imageList->Add(wxBitmap(empty_xpm));
    
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
    mmDBWrapper::getINISettingValue(inidb_, wxT("BUDGET_COL0_WIDTH"), wxT("80")).ToLong(&col0); 
    mmDBWrapper::getINISettingValue(inidb_, wxT("BUDGET_COL1_WIDTH"), wxT("80")).ToLong(&col1); 
    mmDBWrapper::getINISettingValue(inidb_, wxT("BUDGET_COL2_WIDTH"), wxT("80")).ToLong(&col2); 
    mmDBWrapper::getINISettingValue(inidb_, wxT("BUDGET_COL3_WIDTH"), wxT("80")).ToLong(&col3); 
    mmDBWrapper::getINISettingValue(inidb_, wxT("BUDGET_COL4_WIDTH"), wxT("80")).ToLong(&col4); 
    mmDBWrapper::getINISettingValue(inidb_, wxT("BUDGET_COL5_WIDTH"), wxT("80")).ToLong(&col5); 

    listCtrlAccount_->SetColumnWidth(0, col0);
    listCtrlAccount_->SetColumnWidth(1, col1);
    listCtrlAccount_->SetColumnWidth(2, col2);
    listCtrlAccount_->SetColumnWidth(3, col3);
    listCtrlAccount_->SetColumnWidth(4, col4);
    listCtrlAccount_->SetColumnWidth(5, col5);

    itemBoxSizer2->Add(listCtrlAccount_, 1, wxGROW | wxALL, 1);
}

bool mmBudgetingPanel::displayEntryAllowed(mmBudgetEntryHolder& budgetEntry)
{
    bool result = false;

    if (currentView_ == wxT("View Non-Zero Budget Categories")) 
    {
        if ((budgetEntry.estimated_ != 0.0) || (budgetEntry.actual_ != 0.0)) result = true;
    } 
    else if (currentView_ == wxT("View Income Budget Categories")) 
    {
        if ((budgetEntry.estimated_ > 0.0) || (budgetEntry.actual_ > 0.0)) result = true;
    } 
    else if (currentView_ == wxT("View Expense Budget Categories")) 
    {
        if ((budgetEntry.estimated_ < 0.0) || (budgetEntry.actual_ < 0.0)) result = true;
    } 
    else if (currentView_ == _("View Budget Category Summary")) 
    {
        if ((budgetEntry.id_ < 0.0)) result = true;
    } 
    else 
    {
        result = true;
    }

    return result;
}

void mmBudgetingPanel::initVirtualListControl()
{
    trans_.clear();
    double estIncome = 0.0;
    double estExpenses = 0.0;
    double actIncome = 0.0;
    double actExpenses = 0.0;
    mmCommonReportDetails  budgetDetails( mainFrame_);

    bool evaluateTransfer = false;
    if (mainFrame_->budgetTransferTotal())
    {
        evaluateTransfer = true;
    }
    wxString budgetYearStr = mmDBWrapper::getBudgetYearForID(db_, budgetYearID_);
    long year = 0;
    budgetYearStr.ToLong(&year);
    wxDateTime dtBegin(1, wxDateTime::Jan, year);
    wxDateTime dtEnd(31, wxDateTime::Dec, year);

    bool monthlyBudget = (budgetYearStr.length() > 5);
  
    if (monthlyBudget)
    {
        budgetDetails.SetBudgetMonth(budgetYearStr, dtBegin, dtEnd);
    }
    else
    {
        int day, month;
        budgetDetails.AdjustYearValues(day, month, dtBegin);
        budgetDetails.AdjustDateForEndFinancialYear(dtEnd);
    }

    mmDBWrapper::loadBaseCurrencySettings(db_);
//TODO: make it easier
/*
    static const char categ_list_sql[] =
    "select c.categname as CATEGNAME, s.subcategname as SUBCATEGNAME, "
    "c.categid as CATEGID, s.subcategid as SUBCATEGID "
    "from subcategory_v1 s "
    "left join  category_v1 c on c.categid=s.categid "
    "union select categname, '', categid, -1  from category_v1 "
    "order by CATEGNAME, SUBCATEGNAME ";
*/
    static const char sql[] =
    "select CATEGID, CATEGNAME "
    "from CATEGORY_V1 "
    "order by CATEGNAME";

    static const char sql_sub[] = 
    "select SUBCATEGID, SUBCATEGNAME "
    "from SUBCATEGORY_V1 "
    "where CATEGID = ? "
    "order by SUBCATEGNAME";

    wxSQLite3Statement st = db_->PrepareStatement(sql);
    wxSQLite3Statement st_sub = db_->PrepareStatement(sql_sub);

    wxSQLite3ResultSet q1 = st.ExecuteQuery();

    while (q1.NextRow())
    {
        mmBudgetEntryHolder th;
        budgetDetails.initBudgetEntryFields(th);
        th.categID_ = q1.GetInt(wxT("CATEGID"));
        th.catStr_ = q1.GetString(wxT("CATEGNAME"));

        mmDBWrapper::getBudgetEntry(db_, budgetYearID_, th.categID_, th.subcategID_, th.period_, th.amt_);
        budgetDetails.setBudgetEstimate(th, monthlyBudget, dtBegin, dtEnd);
        if (th.estimated_ < 0)
            estExpenses += th.estimated_;
        else
            estIncome += th.estimated_;

        bool transferAsDeposit = true;
        if (th.amt_ < 0)
        {
            transferAsDeposit = false;
        }
        th.actual_ = mmDBWrapper::getAmountForCategory(db_, th.categID_, th.subcategID_, false,
            dtBegin, dtEnd, evaluateTransfer, transferAsDeposit, mmIniOptions::ignore_future_transactions_
        );
        if (th.actual_ < 0)
            actExpenses += th.actual_;
        else
            actIncome += th.actual_;

        mmex::formatDoubleToCurrencyEdit(th.amt_, th.amtString_);
        mmex::formatDoubleToCurrencyEdit(th.estimated_, th.estimatedStr_);
        mmex::formatDoubleToCurrencyEdit(th.actual_, th.actualStr_);

        /*************************************************************************** 
         Create a TOTALS entry for the category.
         ***************************************************************************/
        mmBudgetEntryHolder catTotals;
        catTotals.id_ = -1;
        catTotals.categID_ = -1;
        catTotals.catStr_  = th.catStr_;
        catTotals.subcategID_ = -1;
        catTotals.subCatStr_ = wxEmptyString;
        catTotals.period_    = wxEmptyString;
        catTotals.amt_       = th.amt_;
        catTotals.estimated_ = th.estimated_;
        catTotals.actual_    = th.actual_;
        mmex::formatDoubleToCurrencyEdit(catTotals.amt_, catTotals.amtString_);
        mmex::formatDoubleToCurrencyEdit(catTotals.estimated_, catTotals.estimatedStr_);
        mmex::formatDoubleToCurrencyEdit(catTotals.actual_, catTotals.actualStr_);

        if (displayEntryAllowed(th)) {
            trans_.push_back(th);
        }

        st_sub.Bind(1, th.categID_);
        wxSQLite3ResultSet q2 = st_sub.ExecuteQuery(); 

        while(q2.NextRow())
        {
            mmBudgetEntryHolder thsub;
            budgetDetails.initBudgetEntryFields(thsub);
            thsub.categID_ = th.categID_;
            thsub.catStr_ = th.catStr_;
            thsub.subcategID_ = q2.GetInt(wxT("SUBCATEGID"));
            thsub.subCatStr_   = q2.GetString(wxT("SUBCATEGNAME"));

            mmDBWrapper::getBudgetEntry(db_, budgetYearID_, thsub.categID_, thsub.subcategID_, thsub.period_, thsub.amt_);
            budgetDetails.setBudgetEstimate(thsub, monthlyBudget, dtBegin, dtEnd);
            if (thsub.estimated_ < 0) 
                estExpenses += thsub.estimated_;
            else
                estIncome += thsub.estimated_;

            transferAsDeposit = true;
            if (thsub.amt_ < 0)
            {
                transferAsDeposit = false;
            }
            thsub.actual_ = mmDBWrapper::getAmountForCategory(db_, thsub.categID_, thsub.subcategID_, false,
                dtBegin, dtEnd, evaluateTransfer, transferAsDeposit, mmIniOptions::ignore_future_transactions_
            );
            if (thsub.actual_ < 0)
                actExpenses += thsub.actual_;
            else
                actIncome += thsub.actual_;

            mmex::formatDoubleToCurrencyEdit(thsub.amt_, thsub.amtString_);
            mmex::formatDoubleToCurrencyEdit(thsub.estimated_, thsub.estimatedStr_);
            mmex::formatDoubleToCurrencyEdit(thsub.actual_, thsub.actualStr_);

            /*************************************************************************** 
             Update the TOTALS entry for the subcategory.
            ***************************************************************************/
            catTotals.amt_          += thsub.amt_;
            catTotals.estimated_    += thsub.estimated_;
            catTotals.actual_       += thsub.actual_;
            mmex::formatDoubleToCurrencyEdit(catTotals.amt_,       catTotals.amtString_);
            mmex::formatDoubleToCurrencyEdit(catTotals.estimated_, catTotals.estimatedStr_);
            mmex::formatDoubleToCurrencyEdit(catTotals.actual_,    catTotals.actualStr_);

            if (displayEntryAllowed(thsub)) {
                trans_.push_back(thsub);
            }
        }
        st_sub.Reset();

        if (mainFrame_->budgetSetupWithSummary() && displayEntryAllowed(catTotals))
        {
            trans_.push_back(catTotals);
            int transCatTotalIndex = (int)trans_.size()-1;
            listCtrlAccount_->RefreshItem(transCatTotalIndex);
        }
    }

    st_sub.Finalize();
    st.Finalize();

    listCtrlAccount_->SetItemCount((int)trans_.size());
    
    wxString estIncomeStr, actIncomeStr,  estExpensesStr, actExpensesStr;
    mmex::formatDoubleToCurrency(estIncome, estIncomeStr);
    mmex::formatDoubleToCurrency(actIncome, actIncomeStr);
    if (estExpenses < 0.0)
        estExpenses = -estExpenses;
    if (actExpenses < 0.0)
        actExpenses = -actExpenses;

    mmex::formatDoubleToCurrency(estExpenses, estExpensesStr);
    mmex::formatDoubleToCurrency(actExpenses, actExpensesStr);

    wxString differenceStr;
    wxStaticText* header = (wxStaticText*)FindWindow(ID_DIALOG_BUDGETENTRY_SUMMARY_INCOME_EST);
    header->SetLabel( wxString() << _("Estimated: ") << estIncomeStr);
    
    header = (wxStaticText*)FindWindow(ID_DIALOG_BUDGETENTRY_SUMMARY_INCOME_ACT);
    header->SetLabel( wxString() << _("Actual: ") << actIncomeStr);

    mmex::formatDoubleToCurrency(estIncome -actIncome, differenceStr);
    header = (wxStaticText*)FindWindow(ID_DIALOG_BUDGETENTRY_SUMMARY_INCOME_DIF);
    header->SetLabel( wxString() << _("Difference: ") << differenceStr);

    header = (wxStaticText*)FindWindow(ID_DIALOG_BUDGETENTRY_SUMMARY_EXPENSES_EST);
    header->SetLabel( wxString() <<_("Estimated: ") << estExpensesStr);
    
    header = (wxStaticText*)FindWindow(ID_DIALOG_BUDGETENTRY_SUMMARY_EXPENSES_ACT);
    header->SetLabel( wxString() << _("Actual: ") << actExpensesStr);
    
    mmex::formatDoubleToCurrency(estExpenses -actExpenses, differenceStr);
    header = (wxStaticText*)FindWindow(ID_DIALOG_BUDGETENTRY_SUMMARY_EXPENSES_DIF);
    header->SetLabel( wxString() << _("Difference: ") << differenceStr);
}

/*******************************************************/
void budgetingListCtrl::OnListItemSelected(wxListEvent& event)
{
    selectedIndex_ = event.GetIndex();
}

wxString mmBudgetingPanel::getItem(long item, long column)
{
    if (column == 0) return trans_[item].catStr_;
    if (column == 1) return trans_[item].subCatStr_;
    if (column == 2) return trans_[item].period_;
    if (column == 3) return trans_[item].amtString_;
    if (column == 4) return trans_[item].estimatedStr_;
    if (column == 5) return trans_[item].actualStr_;

    return wxT("");
}

int budgetingListCtrl::OnGetItemImage(long item) const
{
    if ((cp_->trans_[item].estimated_ == 0.0) && (cp_->trans_[item].actual_ == 0.0)) return 3;
    if ((cp_->trans_[item].estimated_ == 0.0) && (cp_->trans_[item].actual_ != 0.0)) return 2;
    if (cp_->trans_[item].estimated_ < cp_->trans_[item].actual_) return 0;
    if (fabs(cp_->trans_[item].estimated_ - cp_->trans_[item].actual_)  < 0.001) return 0;

   return 1;
}

wxString budgetingListCtrl::OnGetItemText(long item, long column) const
{
    return cp_->getItem(item, column);
}

wxListItemAttr* budgetingListCtrl::OnGetItemAttr(long item) const
{
    if ((cp_->trans_[item].id_ < 0) && 
        (cp_->currentView_ != wxT("View Budget Category Summary")) )
    {
        return (wxListItemAttr *)&attr3_;
    }
    
    /* Returns the alternating background pattern */
    return item % 2 ? (wxListItemAttr *)&attr2_ : (wxListItemAttr *)&attr1_;
}

void budgetingListCtrl::OnListItemActivated(wxListEvent& event)
{
    selectedIndex_ = event.GetIndex();
    /*************************************************************************** 
     A TOTALS entry does not contain a budget entry, therefore ignore the event.
     ***************************************************************************/
    if (cp_->trans_[selectedIndex_].id_ >= 0)
    {
        mmBudgetEntryDialog dlg(
            cp_->db_, cp_->budgetYearID_,
            cp_->trans_[selectedIndex_].categID_,
            cp_->trans_[selectedIndex_].subcategID_,
            cp_->trans_[selectedIndex_].estimatedStr_,
            cp_->trans_[selectedIndex_].actualStr_, this);
        if ( dlg.ShowModal() == wxID_OK )
        {
            cp_->initVirtualListControl();
            RefreshItem(selectedIndex_);
        }
    }
}
