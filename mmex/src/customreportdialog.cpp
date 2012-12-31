/*******************************************************
 Copyright (C) 2011 Stefano Giorgio

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

#include "customreportdialog.h"
#include "customreportindex.h"
#include "paths.h"
#include "util.h"
#include <wx/helpbase.h>
#include <wx/help.h>

const int titleTextWidth   = 200; // Determines width of Headings Textbox.
const int sourceTextHeight = 200; // Determines height of Source Textbox.

IMPLEMENT_DYNAMIC_CLASS( mmCustomSQLDialog, wxDialog )

BEGIN_EVENT_TABLE( mmCustomSQLDialog, wxDialog )
    EVT_BUTTON(wxID_OPEN,  mmCustomSQLDialog::OnOpen)
    EVT_BUTTON(wxID_SAVE,  mmCustomSQLDialog::OnSave)
    EVT_BUTTON(wxID_REFRESH, mmCustomSQLDialog::OnRun)
    EVT_BUTTON(wxID_CLEAR, mmCustomSQLDialog::OnClear)
    EVT_BUTTON(wxID_CLOSE, mmCustomSQLDialog::OnClose)
    EVT_CHECKBOX(DIALOG_CUSTOM_SQL_CHKBOX_HEADING_ONLY, mmCustomSQLDialog::OnCheckedHeading)
    EVT_CHECKBOX(DIALOG_CUSTOM_SQL_CHKBOX_SUB_REPORT,   mmCustomSQLDialog::OnCheckedSubReport)
    EVT_TEXT( wxID_FILE, mmCustomSQLDialog::OnTextChangeHeading)
    EVT_TEXT( wxID_VIEW_DETAILS,       mmCustomSQLDialog::OnTextChangeSubReport)
    EVT_TREE_ITEM_RIGHT_CLICK(wxID_ANY, mmCustomSQLDialog::OnItemRightClick)
    EVT_TREE_SEL_CHANGED(wxID_ANY, mmCustomSQLDialog::OnSelChanged)
    EVT_TREE_END_LABEL_EDIT(wxID_ANY, mmCustomSQLDialog::OnLabelChanged)
    //EVT_TREE_ITEM_ACTIVATED(wxID_ANY,  mmCustomSQLDialog::OnDoubleClicked)
END_EVENT_TABLE()

mmCustomSQLDialog::mmCustomSQLDialog(customSQLReportIndex* reportIndex, wxWindow* parent, bool edit,
                                     wxWindowID id, const wxString& caption, const wxPoint& pos, const wxSize& size, long style)
:reportIndex_(reportIndex), sSourceTxtCtrl_(), edit_(edit), navCtrlUpdateRequired_(false), newFileCreated_(true), parent_(parent)
{
    Create(parent_, id, caption, pos, size, style);
}

bool mmCustomSQLDialog::Create( wxWindow* parent, wxWindowID id,
                        const wxString& caption, const wxPoint& pos, const wxSize& size, long style )
{
    SetExtraStyle(GetExtraStyle()|wxWS_EX_BLOCK_EVENTS);
    wxDialog::Create( parent, id, caption, pos, size, style );

    wxAcceleratorEntry entries[2];
    entries[0].Set(wxACCEL_NORMAL, WXK_F9, wxID_REFRESH);
    wxAcceleratorTable accel(1, entries);
    SetAcceleratorTable(accel);

    Connect(wxID_REFRESH, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(mmCustomSQLDialog::OnRun));

    CreateControls();
    GetSizer()->Fit(this);
    GetSizer()->SetSizeHints(this);

    SetIcon(mmex::getProgramIcon());
    Centre();

    iSelectedId_ = reportIndex_->getCustomReportId();
    fillControls();
    return TRUE;
}

void mmCustomSQLDialog::fillControls()
{
    button_Save_->Disable(); // Will be activated if text changes in either text field.

    if (edit_)
    {
        reportTitleTxtCtrl_->ChangeValue(reportIndex_->currentReportTitle());
        if (reportIndex_->currentReportFileName().IsEmpty())
        {
            headingOnlyCheckBox_->SetValue(true);
            SetDialogBoxForHeadings();
        }
        else
        {
            loadedFileName_ = reportIndex_->currentReportFileName(false);
            subMenuCheckBox_->SetValue(reportIndex_->reportIsSubReport());

            wxString sqlData;
            reportIndex_->getSqlFileData(sqlData);
            sSourceTxtCtrl_->ChangeValue(sqlData);

        }
        button_Open_->Disable();
    }
    else
    {
        button_Run_->Disable();
        iSelectedId_ = -1;
    }

    treeCtrl_->DeleteAllItems();
    wxTreeItemId root_ = treeCtrl_->AddRoot(_("Custom Reports"));

    if (reportIndex_->hasActiveSQLReports())
    {
        if (reportIndex_->currentReportFileType() == wxT("LUA"))
        {
            m_radio_box_->SetSelection(1);
        }
        int reportNumber = -1;
        wxString reportNumberStr;
        wxTreeItemId customSqlReportRootItem;
        reportIndex_->resetReportsIndex();
        wxString reportTitle = reportIndex_->nextReportTitle();
        while (reportIndex_->validTitle())
        {
            wxTreeItemId customSqlReportItem;
            if (reportIndex_->reportIsSubReport() && reportNumber >= 0 )
            {
                customSqlReportItem = treeCtrl_->AppendItem(customSqlReportRootItem, reportTitle);
            }
            else
            {
                customSqlReportItem = treeCtrl_->AppendItem(root_, reportTitle);
                customSqlReportRootItem = customSqlReportItem;
            }
            reportNumberStr.Printf(wxT("Custom_Report_%d"), ++reportNumber);
            treeCtrl_->SetItemData(customSqlReportItem, new mmTreeItemData(reportNumberStr));

            if (reportNumber == iSelectedId_) treeCtrl_->SelectItem(customSqlReportItem);

            reportTitle = reportIndex_->nextReportTitle();
        }
    }
    treeCtrl_->ExpandAll();
}

void mmCustomSQLDialog::CreateControls()
{
    wxSizerFlags flags, flagsExpand;
    flags.Align(wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL).Border(wxALL, 5);
    flagsExpand.Align(wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxEXPAND).Border(wxALL, 5).Proportion(1);

    wxBoxSizer* mainBoxSizer = new wxBoxSizer(wxVERTICAL);
    this->SetSizer(mainBoxSizer);

    /****************************************
     Parameters Area
     ***************************************/

    wxBoxSizer* headingPanelSizerH = new wxBoxSizer(wxHORIZONTAL);
    mainBoxSizer->Add(headingPanelSizerH, 5, wxGROW|wxALL, 5);

    wxBoxSizer* headingPanelSizerH2 = new wxBoxSizer(wxVERTICAL);
    headingPanelSizerH2->AddSpacer(15);

    headingPanelSizerH->Add(headingPanelSizerH2, 0, wxEXPAND);
    wxFlexGridSizer* flex_sizer = new wxFlexGridSizer(0, 2, 0, 0);
    //
    flex_sizer->Add(new wxStaticText( this, wxID_ANY, _("Script type:")), flags);
    wxString choices[] = { _("SQL"), _("Lua")};
    int num = sizeof(choices) / sizeof(wxString);
    m_radio_box_ = new wxRadioBox(this, wxID_STATIC, wxT("")
        , wxDefaultPosition, wxDefaultSize, num, choices, 2, wxRA_SPECIFY_COLS);
    flex_sizer->Add(m_radio_box_, flags.Center());

    headingOnlyCheckBox_ = new wxCheckBox(this, DIALOG_CUSTOM_SQL_CHKBOX_HEADING_ONLY, _("Heading"));
    flex_sizer->Add(headingOnlyCheckBox_, flags);

    subMenuCheckBox_ = new wxCheckBox( this, DIALOG_CUSTOM_SQL_CHKBOX_SUB_REPORT, _("Sub-Menu"));
    flex_sizer->Add(subMenuCheckBox_, flags);

    flex_sizer->Add(new wxStaticText( this, wxID_ANY, _("Report Title:")), flags);
    flex_sizer->AddSpacer(1);

    reportTitleTxtCtrl_ = new wxTextCtrl( this, wxID_FILE, wxT(""),
        wxDefaultPosition, wxSize(titleTextWidth,-1));
    reportTitleTxtCtrl_->SetToolTip(_("Report Title is used as the file name of the SQL script."));

    long treeCtrlFlags = wxTR_EDIT_LABELS | wxTR_SINGLE | wxTR_HAS_BUTTONS;
#if defined (__WXWIN__)
    treeCtrlFlags = wxTR_EDIT_LABELS | wxTR_SINGLE | wxTR_HAS_BUTTONS | wxTR_ROW_LINES;
#endif
    treeCtrl_ = new wxTreeCtrl( this, wxID_ANY,
    wxDefaultPosition, wxSize(titleTextWidth, titleTextWidth), treeCtrlFlags );

    headingPanelSizerH2->Add(flex_sizer, flags);
    headingPanelSizerH2->Add(reportTitleTxtCtrl_, flags);
    headingPanelSizerH2->Add(treeCtrl_, flagsExpand);

    /****************************************
     Script Area
     ***************************************/
    // ListBox for source code
    wxBoxSizer* headingPanelSizerV3 = new wxBoxSizer(wxVERTICAL);
    wxBoxSizer* headingPanelSizerH4 = new wxBoxSizer(wxHORIZONTAL);
    headingPanelSizerH->Add(headingPanelSizerV3, flagsExpand);

    headingPanelSizerV3->Add(new wxStaticText( this, wxID_ANY, _("Custom script:")), flags);
    sSourceTxtCtrl_ = new wxTextCtrl( this, wxID_VIEW_DETAILS, wxT(""),
        wxDefaultPosition, wxDefaultSize, wxTE_MULTILINE|wxHSCROLL );
    headingPanelSizerV3->Add(sSourceTxtCtrl_, flagsExpand);
    headingPanelSizerV3->Add(headingPanelSizerH4, flags.Center());

    button_Open_ = new wxButton( this, wxID_OPEN);
    headingPanelSizerH4->Add(button_Open_, flags);
    button_Open_->SetToolTip(_("Locate and load an SQL script file into the script area."));

    button_Save_ = new wxButton( this, wxID_SAVE);
    headingPanelSizerH4->Add(button_Save_, flags);
    button_Save_->SetToolTip(_("Save SQL script to file name set by the Report Title."));

    wxButton* button_Clear = new wxButton( this, wxID_CLEAR);
    headingPanelSizerH4->Add(button_Clear, flags);
    button_Clear->SetToolTip(_("Clear the SQL Source script area"));

    /****************************************
     Bottom Panel
     ***************************************/
    wxPanel* buttonPanel = new wxPanel( this, wxID_STATIC,  wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
    mainBoxSizer->Add(buttonPanel, flags);

    wxBoxSizer* buttonPanelSizer = new wxBoxSizer(wxHORIZONTAL);
    buttonPanel->SetSizer(buttonPanelSizer);

    button_Run_ = new wxButton( buttonPanel, wxID_REFRESH, _("&Run"));
    buttonPanelSizer->Add(button_Run_, flags);
    button_Run_->SetToolTip(_("Test script. Save before running. SQL errors will result in loss of script."));

    wxButton* button_Close = new wxButton( buttonPanel, wxID_CLOSE);
    buttonPanelSizer->Add(button_Close, flags);
    button_Close->SetToolTip(_("Save changes before closing. Changes without Save will be lost."));

}

wxString mmCustomSQLDialog::sScript()
{
    return sQuery_;
}

wxString mmCustomSQLDialog::sReportTitle()
{
    return reportTitleTxtCtrl_->GetValue();
}

wxString mmCustomSQLDialog::sSctiptType()
{
    int i = m_radio_box_->GetSelection();
    if (i == 0)
        return wxT("SQL");
    else
        return wxT("LUA");
}

void mmCustomSQLDialog::OnOpen(wxCommandEvent& /*event*/)
{
    wxString sScriptFileName = wxFileSelector( sSctiptType()==wxT("SQL") ?
        _("Load Custom SQL file:") : _("Load Custom Lua file:"),
        mmex::getPathUser(mmex::DIRECTORY), wxEmptyString, wxEmptyString,
        sSctiptType()==wxT("SQL") ? wxT("SQL File(*.sql)|*.sql") : wxT("Lua File(*.lua)|*.lua")
        , wxFD_FILE_MUST_EXIST);
    if ( !sScriptFileName.empty() )
    {
        wxFileName selectedFileName(sScriptFileName);
        loadedFileName_ = selectedFileName.GetFullName();
        wxString sqlText;

        wxTextFile sqlFile(sScriptFileName);
        if (sqlFile.Open())
        {
            sqlText << sqlFile.GetFirstLine();
            while (! sqlFile.Eof())
            {
                wxString nextLine = sqlFile.GetNextLine();
                if (! nextLine.IsEmpty())
                    sqlText << wxT("\n");
                sqlText << nextLine;
            }
            sSourceTxtCtrl_->SetValue(sqlText);
            newFileCreated_ = false;
            sqlFile.Close();
        }
        else
        {
            wxString msg = wxString() << _("Unable to open file.") << sScriptFileName << wxT("\n\n");
            wxMessageBox(msg,reportIndex_->UserDialogHeading(),wxOK|wxICON_ERROR);
        }
    }
}

void mmCustomSQLDialog::OnSave(wxCommandEvent& /*event*/)
{
    wxString reportfileName = reportTitleTxtCtrl_->GetValue();

    if (reportfileName.IsEmpty())
    {
        wxMessageBox(_("Please supply the Report Title before saving"),reportIndex_->UserDialogHeading(),wxOK|wxICON_WARNING);
        return;
    }

    reportfileName.Replace(wxT(" "),wxT("_"));          // Replace spaces with underscore character
    reportfileName += sSctiptType() == wxT("SQL") ? wxT(".sql") : wxT(".lua");  // Add the file extenstion

    if (reportfileName == loadedFileName_)
    {
        edit_ = true;
    }
    else if (loadedFileName_.IsEmpty())
    {
        loadedFileName_ = reportfileName;
    }

    button_Save_->Disable();

    if (! edit_ && reportIndex_->ReportListHasItems() )
    {
        reportIndex_->getUserTitleSelection(_(" entry location:\n\nOK....: Insert before selected entry\nCancel: Add at bottom of List."));
    }

    if ( headingOnlyCheckBox_->IsChecked() )
    {
        reportIndex_->addReportTitle(reportTitleTxtCtrl_->GetValue(), edit_);
        navCtrlUpdateRequired_ = true;
    }
    else
    {
        // If we have a filename, and we have something to save, save the file.
        wxString sqlSource = sSourceTxtCtrl_->GetValue();
        if ( !reportfileName.empty() && !sqlSource.empty() )
        {
            reportfileName.Replace(wxT(":"),wxT("_")); // else windows 7 will hide the filename.
            wxTextFile tfSourceFile(mmex::getPathUser(mmex::DIRECTORY) + reportfileName);

            // If the file does not exist and cannot be created, throw an error
            if ( !tfSourceFile.Exists() && !tfSourceFile.Create() )
            {
                wxMessageBox(_("Unable to write to file."),reportIndex_->UserDialogHeading(), wxOK|wxICON_ERROR);
                return;
            }

            // if the file does exist, then update the file contents to the new value.
            if (tfSourceFile.Exists())
            {
                // Make sure the file is open
                if (! tfSourceFile.IsOpened())
                {
                    tfSourceFile.Open();
                }
                tfSourceFile.Clear();
                tfSourceFile.AddLine(sqlSource);
            }
            tfSourceFile.Write();
            tfSourceFile.Close();
        }

        // update the index file
        reportIndex_->addReportTitle(reportTitleTxtCtrl_->GetValue(), edit_, reportfileName, subMenuCheckBox_->GetValue());
        navCtrlUpdateRequired_ = true;
        loadedFileName_ = reportfileName;
    }
    if (!edit_ && navCtrlUpdateRequired_) fillControls();
    edit_ = true;
}

void mmCustomSQLDialog::OnRun(wxCommandEvent& /*event*/)
{
   if (sSourceTxtCtrl_->IsEmpty()) return;
   sQuery_ = sSourceTxtCtrl_->GetValue();
   EndModal(wxID_MORE);
}

void mmCustomSQLDialog::OnClear(wxCommandEvent& /*event*/)
{
    sSourceTxtCtrl_->Clear();
    button_Save_->Disable();
    button_Run_->Disable();
    if (! headingOnlyCheckBox_->GetValue())
        button_Open_->Enable();
    sSourceTxtCtrl_->SetFocus();
}

void mmCustomSQLDialog::OnClose(wxCommandEvent& /*event*/)
{
    if (navCtrlUpdateRequired_)
        EndModal(wxID_OK);
    else
        EndModal(wxID_CANCEL);
}

void mmCustomSQLDialog::SetDialogBoxForHeadings()
{
    subMenuCheckBox_->Enable( !headingOnlyCheckBox_->GetValue());
    sSourceTxtCtrl_->Enable(!headingOnlyCheckBox_->GetValue());
    button_Open_->Enable(!headingOnlyCheckBox_->GetValue() && sSourceTxtCtrl_->IsEmpty());
    button_Run_->Enable(!headingOnlyCheckBox_->GetValue() && !sSourceTxtCtrl_->IsEmpty());
}

void mmCustomSQLDialog::OnCheckedHeading(wxCommandEvent& /*event*/)
{
    button_Save_->Enable(!reportTitleTxtCtrl_->IsEmpty());

    SetDialogBoxForHeadings();
}

void mmCustomSQLDialog::OnCheckedSubReport(wxCommandEvent& /*event*/)
{
    button_Save_->Enable();
    headingOnlyCheckBox_->Enable(!subMenuCheckBox_->GetValue());
}

void mmCustomSQLDialog::OnTextChangeHeading(wxCommandEvent& /*event*/)
{
    button_Save_->Enable();
    edit_ = false;          // Allow saving as a new file name.
}

void mmCustomSQLDialog::OnTextChangeSubReport(wxCommandEvent& /*event*/)
{
    button_Save_->Enable(!reportTitleTxtCtrl_->IsEmpty());
    button_Run_->Enable(!sSourceTxtCtrl_->IsEmpty());
    button_Open_->Enable(sSourceTxtCtrl_->IsEmpty());
}

void mmCustomSQLDialog::OnItemRightClick(wxTreeEvent& event)
{
    wxTreeItemId id = event.GetItem();
    treeCtrl_ ->SelectItem(id);

    wxMenu* customReportMenu = new wxMenu;
    customReportMenu->Append(1, _("New Custom Report"));
    customReportMenu->Append(2, _("Delete Custom Report"));
    PopupMenu(&*customReportMenu);

}

void mmCustomSQLDialog::OnSelChanged(wxTreeEvent& event)
{
    mmTreeItemData* iData = dynamic_cast<mmTreeItemData*>(treeCtrl_->GetItemData(event.GetItem()));
    if (!iData) return;
    iSelectedId_ = reportIndex_->getCustomReportId();

    reportIndex_->getSelectedTitleSelection(iData->getString());

    loadedFileName_ = reportIndex_->currentReportFileName(false);
    subMenuCheckBox_->SetValue(reportIndex_->reportIsSubReport());

    wxString sqlData;
    reportIndex_->getSqlFileData(sqlData);
    sSourceTxtCtrl_->ChangeValue(sqlData);
    button_Run_->Enable(!sSourceTxtCtrl_->IsEmpty());
    reportTitleTxtCtrl_->ChangeValue(reportIndex_->currentReportTitle());
    if (reportIndex_->currentReportFileType() == wxT("LUA"))
        m_radio_box_->SetSelection(1);
    else
        m_radio_box_->SetSelection(0);
    edit_ = true;

}

void mmCustomSQLDialog::OnLabelChanged(wxTreeEvent& event)
{
    edit_ = true;
    wxString sOldLabel = reportIndex_->currentReportTitle();
    wxString sLabel = event.GetLabel();
    wxString reportNumberStr = wxString::Format(wxT("Custom_Report_%d"), iSelectedId_);

    if (!sLabel.IsEmpty() && sLabel!=sOldLabel)
    {
        treeCtrl_->SetItemData(event.GetItem(), new mmTreeItemData(reportNumberStr));
        reportTitleTxtCtrl_->ChangeValue(sLabel);
        wxCommandEvent evt(wxEVT_COMMAND_BUTTON_CLICKED, wxID_SAVE);
        OnSave(evt);
    }

}
