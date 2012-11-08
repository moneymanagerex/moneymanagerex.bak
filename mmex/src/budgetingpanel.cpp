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
#include "categdialog.h"
#include "mmaccount.h"

/*******************************************************/
BEGIN_EVENT_TABLE(mmBudgetingPanel, wxPanel)
    EVT_LEFT_DOWN( mmBudgetingPanel::OnMouseLeftDown )
    EVT_MENU(wxID_ANY, mmBudgetingPanel::OnViewPopupSelected)
END_EVENT_TABLE()
/*******************************************************/
BEGIN_EVENT_TABLE(budgetingListCtrl, wxListCtrl)
    EVT_LIST_ITEM_SELECTED(ID_PANEL_CHECKING_LISTCTRL_ACCT, budgetingListCtrl::OnListItemSelected)
    EVT_LIST_ITEM_ACTIVATED(ID_PANEL_CHECKING_LISTCTRL_ACCT, budgetingListCtrl::OnListItemActivated)
    EVT_LIST_COL_END_DRAG(ID_PANEL_CHECKING_LISTCTRL_ACCT, budgetingListCtrl::OnItemResize)
END_EVENT_TABLE()
/*******************************************************/
mmBudgetingPanel::mmBudgetingPanel(
    wxSQLite3Database* db, mmCoreDB* core, mmGUIFrame* mainFrame, int budgetYearID,
    wxWindow *parent, wxWindowID winid, const wxPoint& pos, const wxSize& size, long style,const wxString& name)
: mmPanelBase(db, core)
, m_imageList()
, listCtrlBudget_()
, budgetYearID_(budgetYearID)
, mainFrame_(mainFrame)
{
    Create(parent, winid, pos, size, style, name);
}

bool mmBudgetingPanel::Create( wxWindow *parent,
            wxWindowID winid, const wxPoint& pos,
            const wxSize& size,long style, const wxString& name  )
{
    currentView_ = wxT("View All Budget Categories");
    SetExtraStyle(GetExtraStyle()|wxWS_EX_BLOCK_EVENTS);
    wxPanel::Create(parent, winid, pos, size, style, name);

    windowsFreezeThaw(this);
    CreateControls();
    GetSizer()->Fit(this);
    GetSizer()->SetSizeHints(this);

    initVirtualListControl();

    windowsFreezeThaw(this);
    return TRUE;
}

mmBudgetingPanel::~mmBudgetingPanel()
{
    if (m_imageList) delete m_imageList;
    mainFrame_->SetBudgetingPageInactive();
}

void mmBudgetingPanel::save_column_width(const int width)
{
    int col_x = listCtrlBudget_->GetColumnWidth(width);
    core_->iniSettings_->SetIntSetting(wxString::Format(wxT("BUDGET_COL%d_WIDTH"), width), col_x);
}

void mmBudgetingPanel::OnViewPopupSelected(wxCommandEvent& event)
{
    int evt =  event.GetId();
    if (evt ==  MENU_VIEW_ALLBUDGETENTRIES)
        currentView_ = wxTRANSLATE("View All Budget Categories");
    else if (evt == MENU_VIEW_NONZEROBUDGETENTRIES)
        currentView_ = wxTRANSLATE("View Non-Zero Budget Categories");
    else if (evt == MENU_VIEW_INCOMEBUDGETENTRIES)
        currentView_ = wxTRANSLATE("View Income Budget Categories");
    else if (evt == MENU_VIEW_EXPENSEBUDGETENTRIES)
        currentView_ = wxTRANSLATE("View Expense Budget Categories");
    else if (evt == MENU_VIEW_SUMMARYBUDGETENTRIES)
        currentView_ = wxTRANSLATE("View Budget Category Summary");
    else
        wxASSERT(false);

    wxStaticText* header = (wxStaticText*)FindWindow(ID_PANEL_CHECKING_STATIC_PANELVIEW);
    header->SetLabel(wxGetTranslation(currentView_));

    listCtrlBudget_->DeleteAllItems();
    initVirtualListControl();
    if (trans_.size()>0)
        listCtrlBudget_->RefreshItems(0, (static_cast<long>(trans_.size()-1)));
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

void mmBudgetingPanel::UpdateBudgetHeading()
{
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
    budgetReportHeading_->SetLabel(yearStr);
}

void mmBudgetingPanel::CreateControls()
{
    wxSizerFlags flags;
    flags.Align(wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL).Border(wxLEFT|wxTOP, 4);

    wxBoxSizer* itemBoxSizer2 = new wxBoxSizer(wxVERTICAL);
    this->SetSizer(itemBoxSizer2);

    wxPanel* itemPanel3 = new wxPanel( this, ID_PANEL_REPORTS_HEADER_PANEL,
        wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
    itemBoxSizer2->Add(itemPanel3, 0, wxGROW|wxALL, 5);

    wxBoxSizer* itemBoxSizerVHeader = new wxBoxSizer(wxVERTICAL);
    itemPanel3->SetSizer(itemBoxSizerVHeader);

    wxStaticText* budgetReportHeadingText = new wxStaticText( itemPanel3,
        wxID_STATIC, _("Budget Setup for "));
    budgetReportHeading_ = new wxStaticText( itemPanel3,
        ID_PANEL_REPORTS_STATIC_HEADER, wxT("$"), wxDefaultPosition, wxSize(220, -1));

    const int font_size = this->GetFont().GetPointSize();
    budgetReportHeadingText->SetFont(wxFont(font_size+2, wxSWISS, wxNORMAL, wxBOLD, FALSE, wxT("")));
    budgetReportHeading_->SetFont(wxFont(font_size+2, wxSWISS, wxNORMAL, wxBOLD, FALSE, wxT("")));

    wxBoxSizer* budgetReportHeadingSizer = new wxBoxSizer(wxHORIZONTAL);
    budgetReportHeadingSizer->Add(budgetReportHeadingText);
    budgetReportHeadingSizer->Add(budgetReportHeading_);
    itemBoxSizerVHeader->Add(budgetReportHeadingSizer, 0, wxALL, 1);

    UpdateBudgetHeading();

    wxBoxSizer* itemBoxSizerHHeader2 = new wxBoxSizer(wxHORIZONTAL);
    itemBoxSizerVHeader->Add(itemBoxSizerHHeader2, 0, wxALL, 1);

    wxBitmap itemStaticBitmap3Bitmap(wxBitmap(wxImage(rightarrow_xpm).Scale(16,16)));
    wxStaticBitmap* itemStaticBitmap3 = new wxStaticBitmap( itemPanel3,
        ID_PANEL_BUDGETENTRY_STATIC_BITMAP_VIEW,
        itemStaticBitmap3Bitmap, wxDefaultPosition, wxSize(16, 16), 0 );
    itemStaticBitmap3->Connect(ID_PANEL_BUDGETENTRY_STATIC_BITMAP_VIEW, wxEVT_LEFT_DOWN,
        wxMouseEventHandler(mmBudgetingPanel::OnMouseLeftDown), NULL, this);
    itemBoxSizerHHeader2->Add(itemStaticBitmap3, 0, wxALIGN_CENTER_VERTICAL|wxALL, 1);

    wxStaticText* itemStaticText18 = new wxStaticText( itemPanel3,
        ID_PANEL_CHECKING_STATIC_PANELVIEW, _("Viewing All Budget Categories"));
    itemBoxSizerHHeader2->Add(itemStaticText18, 0, wxALL, 1);

    wxFlexGridSizer* itemIncomeSizer = new wxFlexGridSizer(0,7,5,10);
    itemBoxSizerVHeader->Add(itemIncomeSizer);

    income_estimated_ = new wxStaticText( itemPanel3,
        ID_DIALOG_BUDGETENTRY_SUMMARY_INCOME_EST, wxT("$"), wxDefaultPosition, wxSize(120, -1));
    income_actual_ = new wxStaticText( itemPanel3,
        ID_DIALOG_BUDGETENTRY_SUMMARY_INCOME_ACT, wxT("$"), wxDefaultPosition, wxSize(120, -1));
    income_diff_ = new wxStaticText( itemPanel3,
        ID_DIALOG_BUDGETENTRY_SUMMARY_INCOME_DIF, wxT("$"));

    expences_estimated_ = new wxStaticText( itemPanel3,
        ID_DIALOG_BUDGETENTRY_SUMMARY_EXPENSES_EST, wxT("$"), wxDefaultPosition, wxSize(120, -1));
    expences_actual_ = new wxStaticText( itemPanel3,
        ID_DIALOG_BUDGETENTRY_SUMMARY_EXPENSES_ACT, wxT("$"), wxDefaultPosition, wxSize(120, -1));
    expences_diff_ = new wxStaticText( itemPanel3,
        ID_DIALOG_BUDGETENTRY_SUMMARY_EXPENSES_DIF, wxT("$"));

    itemIncomeSizer->Add(new wxStaticText( itemPanel3, wxID_STATIC, _("Income: ")));
    itemIncomeSizer->Add(new wxStaticText( itemPanel3, wxID_STATIC, _("Estimated: ")));
    itemIncomeSizer->Add(income_estimated_);
    itemIncomeSizer->Add(new wxStaticText( itemPanel3, wxID_STATIC, _("Actual: ")));
    itemIncomeSizer->Add(income_actual_);
    itemIncomeSizer->Add(new wxStaticText( itemPanel3, wxID_STATIC, _("Difference: ")));
    itemIncomeSizer->Add(income_diff_);

    itemIncomeSizer->Add(new wxStaticText( itemPanel3, wxID_STATIC, _("Expenses: ")));
    itemIncomeSizer->Add(new wxStaticText( itemPanel3, wxID_STATIC, _("Estimated: ")));
    itemIncomeSizer->Add(expences_estimated_);
    itemIncomeSizer->Add(new wxStaticText( itemPanel3, wxID_STATIC, _("Actual: ")));
    itemIncomeSizer->Add(expences_actual_);
    itemIncomeSizer->Add(new wxStaticText( itemPanel3, wxID_STATIC, _("Difference: ")));
    itemIncomeSizer->Add(expences_diff_);
    /* ---------------------- */

    wxSize imageSize(16, 16);
    m_imageList = new wxImageList( imageSize.GetWidth(), imageSize.GetHeight() );
    m_imageList->Add(wxBitmap(reconciled_xpm));
    m_imageList->Add(wxBitmap(void_xpm));
    m_imageList->Add(wxBitmap(flag_xpm));
    m_imageList->Add(wxBitmap(empty_xpm));

    listCtrlBudget_ = new budgetingListCtrl( this, this,
        ID_PANEL_CHECKING_LISTCTRL_ACCT, wxDefaultPosition, wxDefaultSize,
        wxLC_REPORT | wxLC_HRULES | wxLC_VRULES | wxLC_VIRTUAL | wxLC_SINGLE_SEL  );

    listCtrlBudget_->SetImageList(m_imageList, wxIMAGE_LIST_SMALL);
    listCtrlBudget_->InsertColumn(0, _("Category"));
    listCtrlBudget_->InsertColumn(1, _("Sub Category"));
    listCtrlBudget_->InsertColumn(2, _("Frequency"));
    listCtrlBudget_->InsertColumn(3, _("Amount"), wxLIST_FORMAT_RIGHT);
    listCtrlBudget_->InsertColumn(4, _("Estimated"), wxLIST_FORMAT_RIGHT);
    listCtrlBudget_->InsertColumn(5, _("Actual"), wxLIST_FORMAT_RIGHT);

    /* Get data from inidb */
    for (int i = 0; i < listCtrlBudget_->GetColumnCount(); ++i)
    {
        int col = core_->iniSettings_->GetIntSetting(wxString::Format(wxT("BUDGET_COL%d_WIDTH"), i), 80);
        listCtrlBudget_->SetColumnWidth(i, col);
    }
    itemBoxSizer2->Add(listCtrlBudget_, 1, wxGROW | wxALL, 1);
}

bool mmBudgetingPanel::DisplayEntryAllowed(mmBudgetEntryHolder& budgetEntry)
{
    bool result = false;

    if (currentView_ == wxT("View Non-Zero Budget Categories"))
        result =((budgetEntry.estimated_ != 0.0) || (budgetEntry.actual_ != 0.0));
    else if (currentView_ == wxT("View Income Budget Categories"))
        result = ((budgetEntry.estimated_ > 0.0) || (budgetEntry.actual_ > 0.0));
    else if (currentView_ == wxT("View Expense Budget Categories"))
        result = ((budgetEntry.estimated_ < 0.0) || (budgetEntry.actual_ < 0.0));
    else if (currentView_ == wxT("View Budget Category Summary"))
        result = ((budgetEntry.id_ < 0.0));
    else
        result = true;

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

    core_->currencyList_.LoadBaseCurrencySettings();

    std::pair<mmCategoryList::const_iterator, mmCategoryList::const_iterator> range = core_->categoryList_.Range();
    for (mmCategoryList::const_iterator it = range.first; it != range.second; ++ it)
    {
        const boost::shared_ptr<mmCategory> category = *it;

        mmBudgetEntryHolder th;
        budgetDetails.initBudgetEntryFields(th, budgetYearID_);
        th.categID_ = category->categID_;
        th.catStr_ = category->categName_;

        mmDBWrapper::getBudgetEntry(db_, budgetYearID_, th.categID_, th.subcategID_, th.period_, th.amt_);
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
        th.actual_ = core_->bTransactionList_.getAmountForCategory(core_, th.categID_, th.subcategID_, false,
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

        if (DisplayEntryAllowed(th)) {
            trans_.push_back(th);
        }

        for (std::vector<boost::shared_ptr<mmCategory> >::const_iterator cit =  category->children_.begin();
                cit != category->children_.end();
                ++ cit)
        {
            const boost::shared_ptr<mmCategory> sub_category = *cit;

            mmBudgetEntryHolder thsub;
            budgetDetails.initBudgetEntryFields(thsub, budgetYearID_);
            thsub.categID_ = th.categID_;
            thsub.catStr_ = th.catStr_;
            thsub.subcategID_ = sub_category->categID_;
            thsub.subCatStr_   = sub_category->categName_;

            mmDBWrapper::getBudgetEntry(db_, budgetYearID_,
                thsub.categID_, thsub.subcategID_, thsub.period_, thsub.amt_);
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
            thsub.actual_ = core_->bTransactionList_.getAmountForCategory(core_, thsub.categID_, thsub.subcategID_, false,
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
            catTotals.estimated_    += thsub.estimated_;
            catTotals.actual_       += thsub.actual_;
            catTotals.amtString_ = wxEmptyString;
            mmex::formatDoubleToCurrencyEdit(catTotals.estimated_, catTotals.estimatedStr_);
            mmex::formatDoubleToCurrencyEdit(catTotals.actual_,    catTotals.actualStr_);

            if (DisplayEntryAllowed(thsub)) {
                trans_.push_back(thsub);
            }
        }

        if (mainFrame_->budgetSetupWithSummary() && DisplayEntryAllowed(catTotals))
        {
            trans_.push_back(catTotals);
            int transCatTotalIndex = (int)trans_.size()-1;
            listCtrlBudget_->RefreshItem(transCatTotalIndex);
        }
    }

    listCtrlBudget_->SetItemCount((int)trans_.size());

    wxString est_amount, act_amount, diff_amount;
    mmex::formatDoubleToCurrency(estIncome, est_amount);
    mmex::formatDoubleToCurrency(actIncome, act_amount);
    mmex::formatDoubleToCurrency(estIncome - actIncome, diff_amount);

    income_estimated_->SetLabel(est_amount);
    income_actual_->SetLabel(act_amount);
    income_diff_->SetLabel(diff_amount);

    if (estExpenses < 0.0) estExpenses = -estExpenses;
    if (actExpenses < 0.0) actExpenses = -actExpenses;
    mmex::formatDoubleToCurrency(estExpenses, est_amount);
    mmex::formatDoubleToCurrency(actExpenses, act_amount);
    mmex::formatDoubleToCurrency(estExpenses -actExpenses, diff_amount);

    expences_estimated_->SetLabel(est_amount);
    expences_actual_->SetLabel(act_amount);
    expences_diff_->SetLabel(diff_amount);
}

void mmBudgetingPanel::DisplayBudgetingDetails(int budgetYearID)
{
    windowsFreezeThaw(this);
    budgetYearID_ = budgetYearID;
    initVirtualListControl();
    UpdateBudgetHeading();
    listCtrlBudget_->RefreshItem(0);
    windowsFreezeThaw(this);
}

/*******************************************************/
void budgetingListCtrl::OnItemResize(wxListEvent& event)
{
    cp_->save_column_width(event.GetColumn());
}

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
    return cp_->GetItemImage(item);
}
int mmBudgetingPanel::GetItemImage(long item) const
{
    if ((trans_[item].estimated_ == 0.0) && (trans_[item].actual_ == 0.0)) return 3;
    if ((trans_[item].estimated_ == 0.0) && (trans_[item].actual_ != 0.0)) return 2;
    if (trans_[item].estimated_ < trans_[item].actual_) return 0;
    if (fabs(trans_[item].estimated_ - trans_[item].actual_)  < 0.001) return 0;
    return 1;
}

wxString budgetingListCtrl::OnGetItemText(long item, long column) const
{
    return cp_->getItem(item, column);
}

wxListItemAttr* budgetingListCtrl::OnGetItemAttr(long item) const
{
    if ((cp_->GetTransID(item) < 0) &&
        (cp_->GetCurrentView() != wxT("View Budget Category Summary")) )
    {
        return (wxListItemAttr *)&attr3_;
    }

    /* Returns the alternating background pattern */
    return item % 2 ? (wxListItemAttr *)&attr2_ : (wxListItemAttr *)&attr1_;
}

void budgetingListCtrl::OnListItemActivated(wxListEvent& event)
{
    selectedIndex_ = event.GetIndex();
    cp_->OnListItemActivated(selectedIndex_);
    RefreshItem(selectedIndex_);
}

void mmBudgetingPanel::OnListItemActivated(int selectedIndex)
{
    /***************************************************************************
     A TOTALS entry does not contain a budget entry, therefore ignore the event.
     ***************************************************************************/
    if (trans_[selectedIndex].id_ < 0) return;
    mmBudgetEntryDialog dlg(
        db_, core_, GetBudgetYearID(),
        trans_[selectedIndex].categID_,
        trans_[selectedIndex].subcategID_,
        trans_[selectedIndex].estimatedStr_,
        trans_[selectedIndex].actualStr_, this);
    if ( dlg.ShowModal() == wxID_OK )
    {
        initVirtualListControl();
    }
}
