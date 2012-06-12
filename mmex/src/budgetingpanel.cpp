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
#include "mmex.h"
#include "reportbudget.h"

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
mmBudgetingPanel::mmBudgetingPanel(wxSQLite3Database* db, mmCoreDB* core, mmGUIFrame* mainFrame, int budgetYearID, wxWindow *parent,
            wxWindowID winid, const wxPoint& pos, const wxSize& size, long style,
            const wxString& name 
           ) : 
    mmPanelBase(db, core),
    listCtrlBudget_(),
    budgetYearID_(budgetYearID),
    mainFrame_(mainFrame)
{
    Create(parent, winid, pos, size, style, name);
    currentView_ = ("View All Budget Categories");
}

bool mmBudgetingPanel::Create( wxWindow *parent,
            wxWindowID winid, const wxPoint& pos, 
            const wxSize& size,long style, const wxString& name  )
{
    SetExtraStyle(GetExtraStyle()|wxWS_EX_BLOCK_EVENTS);
    wxPanel::Create(parent, winid, pos, size, style, name);

#ifdef __WXMSW__
    Freeze();
#endif

    CreateControls();
    GetSizer()->Fit(this);
    GetSizer()->SetSizeHints(this);
    
    initVirtualListControl();

#ifdef __WXMSW__
    Thaw();
#endif

    return TRUE;
}

mmBudgetingPanel::~mmBudgetingPanel()
{
    this->save_config(listCtrlBudget_, ("BUDGET"));
}

void mmBudgetingPanel::OnViewPopupSelected(wxCommandEvent& event)
{
    int evt =  event.GetId();
    wxStaticText* header = (wxStaticText*)FindWindow(ID_PANEL_CHECKING_STATIC_PANELVIEW);
    if (evt ==  MENU_VIEW_ALLBUDGETENTRIES) 
    {
        header->SetLabel(_("View All Budget Categories"));
        currentView_ = ("View All Budget Categories");
    } 
    else if (evt == MENU_VIEW_NONZEROBUDGETENTRIES) 
    {
        header->SetLabel(_("View Non-Zero Budget Categories"));
        currentView_ = ("View Non-Zero Budget Categories");
    } 
    else if (evt == MENU_VIEW_INCOMEBUDGETENTRIES) 
    {
        header->SetLabel(_("View Income Budget Categories"));
        currentView_ = ("View Income Budget Categories");
    } 
    else if (evt == MENU_VIEW_EXPENSEBUDGETENTRIES) 
    {
        header->SetLabel(_("View Expense Budget Categories"));
        currentView_ = ("View Expense Budget Categories");
    } 
    else if (evt == MENU_VIEW_SUMMARYBUDGETENTRIES) 
    {
        header->SetLabel(_("View Budget Category Summary"));
        currentView_ = ("View Budget Category Summary");
    } 
    else 
    {
        wxASSERT(false);
    }

    listCtrlBudget_->DeleteAllItems();
    initVirtualListControl();
    listCtrlBudget_->RefreshItems(0, ((int)trans_.size()) - 1);
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
    wxSizerFlags flags;
    flags.Align(wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL).Border(wxLEFT|wxTOP, 4);

    wxBoxSizer* itemBoxSizer2 = new wxBoxSizer(wxVERTICAL);
    this->SetSizer(itemBoxSizer2);

    wxPanel* headerPanel = new wxPanel( this, ID_PANEL_REPORTS_HEADER_PANEL, 
        wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
    itemBoxSizer2->Add(headerPanel, 0, wxGROW|wxALL, 5);

    wxBoxSizer* itemBoxSizerVHeader = new wxBoxSizer(wxVERTICAL);
    headerPanel->SetSizer(itemBoxSizerVHeader);

    wxString yearStr = mmDBWrapper::getBudgetYearForID(db_, budgetYearID_);
    if ((yearStr.length() < 5)) 
    {
        if ( mainFrame_->budgetFinancialYears() ) 
        {
            long year;
            yearStr.ToLong(&year);
            year++;
            yearStr = wxString() << _("Financial Year: ") << yearStr << (" - ") << year;
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
    wxStaticText* itemStaticText9 = new wxStaticText( headerPanel, ID_PANEL_REPORTS_STATIC_HEADER, 
        _("Budget Setup for ") + yearStr);
    int font_size = this->GetFont().GetPointSize();
    itemStaticText9->SetFont(wxFont(font_size+2, wxSWISS, wxNORMAL, wxBOLD, FALSE, ("")));
    itemBoxSizerVHeader->Add(itemStaticText9, flags);

    wxBoxSizer* itemBoxSizerHHeader2 = new wxBoxSizer(wxHORIZONTAL);
    itemBoxSizerVHeader->Add(itemBoxSizerHHeader2, flags);

    wxBitmap itemStaticBitmap3Bitmap(wxBitmap(wxImage(rightarrow_xpm).Scale(16,16)));
    wxStaticBitmap* itemStaticBitmap3 = new wxStaticBitmap( headerPanel, 
        ID_PANEL_BUDGETENTRY_STATIC_BITMAP_VIEW, 
        itemStaticBitmap3Bitmap);
    itemBoxSizerHHeader2->Add(itemStaticBitmap3, flags);
    //itemStaticBitmap3->SetEventHandler( this ); 
    itemStaticBitmap3->Connect(ID_PANEL_BUDGETENTRY_STATIC_BITMAP_VIEW, wxEVT_LEFT_DOWN,
        wxMouseEventHandler(mmBudgetingPanel::OnFilterChanged), NULL, this);

    wxStaticText* itemStaticText18 = new wxStaticText( headerPanel, 
            ID_PANEL_CHECKING_STATIC_PANELVIEW, 
            _("Viewing All Budget Categories"), wxDefaultPosition, wxDefaultSize, 0 );
    itemBoxSizerHHeader2->Add(itemStaticText18, flags);

    wxStaticText* itemStaticText100 = new wxStaticText( headerPanel, wxID_ANY, _("Income......."),
        wxDefaultPosition, wxSize(75, 20), 0);
    itemStaticText100->SetFont(wxFont(font_size, wxSWISS, wxNORMAL, wxNORMAL, FALSE, ""));

    wxStaticText* itemStaticText101 = new wxStaticText( headerPanel,
        ID_DIALOG_BUDGETENTRY_SUMMARY_INCOME_EST, _("Estimated: ") );
    itemStaticText101->SetFont(wxFont(font_size, wxSWISS, wxNORMAL, wxNORMAL, FALSE, ""));

    wxStaticText* itemStaticText102 = new wxStaticText( headerPanel, 
        ID_DIALOG_BUDGETENTRY_SUMMARY_INCOME_ACT, _("Actual: ") );
    itemStaticText102->SetFont(wxFont(font_size, wxSWISS, wxNORMAL, wxNORMAL, FALSE, ""));

    wxStaticText* itemStaticText103 = new wxStaticText( headerPanel, 
        ID_DIALOG_BUDGETENTRY_SUMMARY_INCOME_DIF, _("Difference: ") );
    itemStaticText103->SetFont(wxFont(font_size, wxSWISS, wxNORMAL, wxNORMAL, FALSE, ""));

    wxBoxSizer* itemIncomeSizer = new wxBoxSizer(wxHORIZONTAL);
    itemIncomeSizer->Add(itemStaticText100, flags);
    itemIncomeSizer->Add(itemStaticText101, flags);
    itemIncomeSizer->Add(itemStaticText102, flags);
    itemIncomeSizer->Add(itemStaticText103, flags);
    itemBoxSizerVHeader->Add(itemIncomeSizer, flags);

    wxStaticText* itemStaticText200 = new wxStaticText( headerPanel, wxID_ANY, _("Expenses..."),
        wxDefaultPosition, wxSize(75, 20), 0 );
    itemStaticText200->SetFont(wxFont(font_size, wxSWISS, wxNORMAL, wxNORMAL, FALSE, ("")));

    wxStaticText* itemStaticText201 = new wxStaticText( headerPanel, 
        ID_DIALOG_BUDGETENTRY_SUMMARY_EXPENSES_EST, _("Estimated: ") );
    itemStaticText201->SetFont(wxFont(font_size, wxSWISS, wxNORMAL, wxNORMAL, FALSE, ("")));

    wxStaticText* itemStaticText202 = new wxStaticText( headerPanel, 
        ID_DIALOG_BUDGETENTRY_SUMMARY_EXPENSES_ACT, _("Actual: ") );
    itemStaticText202->SetFont(wxFont(font_size, wxSWISS, wxNORMAL, wxNORMAL, FALSE, ("")));

    wxStaticText* itemStaticText203 = new wxStaticText( headerPanel, 
        ID_DIALOG_BUDGETENTRY_SUMMARY_EXPENSES_DIF, _("Difference: ") );
    itemStaticText203->SetFont(wxFont(font_size, wxSWISS, wxNORMAL, wxNORMAL, FALSE, ("")));

    wxBoxSizer* itemExpenseSizer = new wxBoxSizer(wxHORIZONTAL);
    itemExpenseSizer->Add(itemStaticText200, flags);
    itemExpenseSizer->Add(itemStaticText201, flags);
    itemExpenseSizer->Add(itemStaticText202, flags);
    itemExpenseSizer->Add(itemStaticText203, flags);
    itemBoxSizerVHeader->Add(itemExpenseSizer, flags);
    /* ---------------------- */

    listCtrlBudget_ = new budgetingListCtrl( this, this, 
        ID_PANEL_CHECKING_LISTCTRL_ACCT, wxDefaultPosition, wxDefaultSize, 
        wxLC_REPORT | wxLC_HRULES | wxLC_VRULES | wxLC_VIRTUAL | wxLC_SINGLE_SEL  );
    listCtrlBudget_->InsertColumn(0, _("Category  "));
    listCtrlBudget_->InsertColumn(1, _("Sub Category"));
    listCtrlBudget_->InsertColumn(2, _("Frequency"));
    listCtrlBudget_->InsertColumn(3, _("Amount"), wxLIST_FORMAT_RIGHT);
    listCtrlBudget_->InsertColumn(4, _("Estimated"), wxLIST_FORMAT_RIGHT);
    listCtrlBudget_->InsertColumn(5, _("Actual"), wxLIST_FORMAT_RIGHT);

    wxConfigBase *config = wxConfigBase::Get();

    long col0 = config->ReadLong("BUDGET_COL0_WIDTH", 80);
    long col1 = config->ReadLong("BUDGET_COL1_WIDTH", 80);
    long col2 = config->ReadLong("BUDGET_COL2_WIDTH", 80);
    long col3 = config->ReadLong("BUDGET_COL3_WIDTH", 80);
    long col4 = config->ReadLong("BUDGET_COL4_WIDTH", 80);
    long col5 = config->ReadLong("BUDGET_COL5_WIDTH", 80);

    listCtrlBudget_->SetColumnWidth(0, col0);
    listCtrlBudget_->SetColumnWidth(1, col1);
    listCtrlBudget_->SetColumnWidth(2, col2);
    listCtrlBudget_->SetColumnWidth(3, col3);
    listCtrlBudget_->SetColumnWidth(4, col4);
    listCtrlBudget_->SetColumnWidth(5, col5);

    itemBoxSizer2->Add(listCtrlBudget_, 1, wxGROW | wxALL, 1);
}

bool mmBudgetingPanel::displayEntryAllowed(mmBudgetEntryHolder& budgetEntry)
{
    bool result = false;

    if (currentView_ == ("View Non-Zero Budget Categories")) 
    {
        if ((budgetEntry.estimated_ != 0.0) || (budgetEntry.actual_ != 0.0)) result = true;
    } 
    else if (currentView_ == ("View Income Budget Categories")) 
    {
        if ((budgetEntry.estimated_ > 0.0) || (budgetEntry.actual_ > 0.0)) result = true;
    } 
    else if (currentView_ == ("View Expense Budget Categories")) 
    {
        if ((budgetEntry.estimated_ < 0.0) || (budgetEntry.actual_ < 0.0)) result = true;
    } 
    else if (currentView_ == _("View Budget Category Summary")) 
    {
        if ((budgetEntry.id_ < 0)) result = true;
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
        budgetDetails.initBudgetEntryFields(th, budgetYearID_);
        th.categID_ = q1.GetInt(("CATEGID"));
        th.catStr_ = q1.GetString(("CATEGNAME"));

        mmDBWrapper::getBudgetEntry(db_, budgetYearID_, th.categID_, th.subcategID_, th.period_, th.amt_, th.id_);
        budgetDetails.setBudgetEstimate(th, monthlyBudget);
        if (th.estimated_ < 0)
            estExpenses += th.estimated_;
        else
            estIncome += th.estimated_;

        bool transferAsDeposit = true;
        if (th.amt_ < 0)
        {
            transferAsDeposit = false;
        }
        th.actual_ = core_->getAmountForCategory(th.categID_, th.subcategID_, false,
            dtBegin, dtEnd, evaluateTransfer, transferAsDeposit, mmIniOptions::instance().ignoreFutureTransactions_
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
            budgetDetails.initBudgetEntryFields(thsub, budgetYearID_);
            thsub.categID_ = th.categID_;
            thsub.catStr_ = th.catStr_;
            thsub.subcategID_ = q2.GetInt(("SUBCATEGID"));
            thsub.subCatStr_   = q2.GetString(("SUBCATEGNAME"));

            mmDBWrapper::getBudgetEntry(db_, budgetYearID_, thsub.categID_, thsub.subcategID_, thsub.period_, thsub.amt_,  thsub.id_);
            budgetDetails.setBudgetEstimate(thsub, monthlyBudget);
            if (thsub.estimated_ < 0) 
                estExpenses += thsub.estimated_;
            else
                estIncome += thsub.estimated_;

            transferAsDeposit = true;
            if (thsub.amt_ < 0)
            {
                transferAsDeposit = false;
            }
            thsub.actual_ = core_->getAmountForCategory(thsub.categID_, thsub.subcategID_, false,
                dtBegin, dtEnd, evaluateTransfer, transferAsDeposit, mmIniOptions::instance().ignoreFutureTransactions_
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
            //catTotals.amt_          += thsub.amt_;
            catTotals.estimated_    += thsub.estimated_;
            catTotals.actual_       += thsub.actual_;
            //mmex::formatDoubleToCurrencyEdit(catTotals.amt_,       catTotals.amtString_);
            catTotals.amtString_ = wxEmptyString;
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
            listCtrlBudget_->RefreshItem(transCatTotalIndex);
        }
    }

    st_sub.Finalize();
    st.Finalize();

    listCtrlBudget_->SetItemCount((int)trans_.size());
    
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
    //RefreshItem(selectedIndex_);
}

wxString mmBudgetingPanel::getItem(long item, long column)
{
    if (column == 0) return trans_[item].catStr_;
    if (column == 1) return trans_[item].subCatStr_;
    if (column == 2) return trans_[item].period_;
    if (column == 3) return trans_[item].amtString_;
    if (column == 4) return trans_[item].estimatedStr_;
    if (column == 5) return trans_[item].actualStr_;

    return ("");
}

int budgetingListCtrl::OnGetItemImage(long item) const
{
    if ((cp_->trans_[item].estimated_ == 0.0) && (cp_->trans_[item].actual_ == 0.0)) return ICON_NONE;
    if ((cp_->trans_[item].estimated_ == 0.0) && (cp_->trans_[item].actual_ != 0.0)) return ICON_VOID;
    if (cp_->trans_[item].estimated_ < cp_->trans_[item].actual_) return ICON_RECONCILED;
    if (fabs(cp_->trans_[item].estimated_ - cp_->trans_[item].actual_)  < 0.001) return ICON_RECONCILED;

   return ICON_VOID;
}

wxString budgetingListCtrl::OnGetItemText(long item, long column) const
{
    return cp_->getItem(item, column);
}

wxListItemAttr* budgetingListCtrl::OnGetItemAttr(long item) const
{
    if (cp_->trans_[item].id_ < 0 && cp_->currentView_ != ("View Budget Category Summary"))
    {
        return m_attr3;
    }
    
    /* Returns the alternating background pattern */
    return item % 2 ? this->m_attr2 : this->m_attr1;
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
            cp_->db_, cp_->core_,
            cp_->trans_[selectedIndex_].id_,
            cp_->budgetYearID_,
            cp_->trans_[selectedIndex_].categID_,
            cp_->trans_[selectedIndex_].subcategID_,
            cp_->trans_[selectedIndex_].estimatedStr_,
            cp_->trans_[selectedIndex_].actualStr_, this);

        dlg.ShowModal();
        cp_->initVirtualListControl();
        RefreshItem(selectedIndex_);
    }
}

void mmBudgetingPanel::OnFilterChanged(wxMouseEvent& event)
{

    int e = event.GetEventType();

    wxMenu menu;
    menu.Append(MENU_VIEW_ALLBUDGETENTRIES, _("Viewing All Budget Categories"));
    menu.Append(MENU_VIEW_NONZEROBUDGETENTRIES, _("View Non-Zero Budget Categories"));
    menu.Append(MENU_VIEW_INCOMEBUDGETENTRIES, _("View Income Budget Categories"));
    menu.Append(MENU_VIEW_EXPENSEBUDGETENTRIES, _("View Budget Category Summary"));
    menu.Append(MENU_VIEW_SUMMARYBUDGETENTRIES, _("View Budget Category Summary"));

    PopupMenu(&menu, event.GetPosition());

    event.Skip();
}

