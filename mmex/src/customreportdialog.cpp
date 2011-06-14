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
#include "paths.h"

const int titleTextWidth   = 350; // Determines width of Headings Textbox.
const int sourceTextHeight = 200; // Determines height of Source Textbox.

IMPLEMENT_DYNAMIC_CLASS( mmCustomSQLDialog, wxDialog )

BEGIN_EVENT_TABLE( mmCustomSQLDialog, wxDialog )
    EVT_BUTTON(DIALOG_CUSTOM_SQL_BTN_LOAD,  mmCustomSQLDialog::OnLoad)
    EVT_BUTTON(DIALOG_CUSTOM_SQL_BTN_SAVE,  mmCustomSQLDialog::OnSave)
    EVT_BUTTON(DIALOG_CUSTOM_SQL_BTN_QUERY, mmCustomSQLDialog::OnRun)
    EVT_BUTTON(DIALOG_CUSTOM_SQL_BTN_CLEAR, mmCustomSQLDialog::OnClear)
    EVT_BUTTON(DIALOG_CUSTOM_SQL_BTN_CLOSE, mmCustomSQLDialog::OnClose)
    EVT_CHECKBOX(DIALOG_CUSTOM_SQL_CHKBOX_HEADING_ONLY, mmCustomSQLDialog::OnCheckedHeading)
    EVT_CHECKBOX(DIALOG_CUSTOM_SQL_CHKBOX_SUB_REPORT,   mmCustomSQLDialog::OnCheckedSubReport)
    EVT_TEXT( DIALOG_CUSTOM_SQL_TXTCTRL_REPORT_TITLE, mmCustomSQLDialog::OnTextChangeHeading)
    EVT_TEXT( DIALOG_CUSTOM_SQL_TXTCTRL_SOURCE,       mmCustomSQLDialog::OnTextChangeSubReport)
END_EVENT_TABLE()


mmCustomSQLDialog::mmCustomSQLDialog(customSQLReportIndex* reportIndex, wxWindow* parent, bool edit,
                                     wxWindowID id, const wxString& caption, const wxPoint& pos, const wxSize& size, long style)
:reportIndex_(reportIndex), sqlSourceTxtCtrl_(), edit_(edit), navCtrlUpdateRequired_(false), newFileCreated_(true)
{
    Create(parent, id, caption, pos, size, style);
}

bool mmCustomSQLDialog::Create( wxWindow* parent, wxWindowID id, 
                        const wxString& caption, const wxPoint& pos, const wxSize& size, long style )
{
    SetExtraStyle(GetExtraStyle()|wxWS_EX_BLOCK_EVENTS);
    wxDialog::Create( parent, id, caption, pos, size, style );

    CreateControls();
    GetSizer()->Fit(this);
    GetSizer()->SetSizeHints(this);

    SetIcon(mmex::getProgramIcon());
    Centre();

    return TRUE;
}

void mmCustomSQLDialog::CreateControls()
{    
    mmCustomSQLDialog* customSQLDialog = this;
    wxBoxSizer* mainBoxSizer = new wxBoxSizer(wxVERTICAL);
    customSQLDialog->SetSizer(mainBoxSizer);

    /****************************************
     Heading Panel 
     ***************************************/
    wxPanel* headingPanel = new wxPanel( customSQLDialog, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
    mainBoxSizer->Add(headingPanel, 0, wxALIGN_LEFT|wxALL, 5);

    wxBoxSizer* headingPanelSizerH = new wxBoxSizer(wxHORIZONTAL);
    headingPanel->SetSizer(headingPanelSizerH);

    wxStaticText* headingTitleText = new wxStaticText( headingPanel, wxID_STATIC, _("Report Title:") );
    headingPanelSizerH->Add(headingTitleText, 0,  wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxALL|wxADJUST_MINSIZE, 5);

    reportTitleTxtCtrl_ = new wxTextCtrl( headingPanel, DIALOG_CUSTOM_SQL_TXTCTRL_REPORT_TITLE, wxT(""), wxDefaultPosition, wxSize(titleTextWidth,-1), 0 );
    headingPanelSizerH->Add(reportTitleTxtCtrl_, 0,  wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxALL|wxADJUST_MINSIZE, 5);

    headingOnlyCheckBox_ = new wxCheckBox(headingPanel, DIALOG_CUSTOM_SQL_CHKBOX_HEADING_ONLY, _("Heading"));
    headingPanelSizerH->Add(headingOnlyCheckBox_, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

    subMenuCheckBox_ = new wxCheckBox( headingPanel, DIALOG_CUSTOM_SQL_CHKBOX_SUB_REPORT, _("Sub-Menu"));
    headingPanelSizerH->Add(subMenuCheckBox_, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

    /****************************************
     Center Area
     ***************************************/
    wxStaticText* sourceTitleText = new wxStaticText( customSQLDialog, wxID_STATIC, _("SQL Source Script:"));
    mainBoxSizer->Add(sourceTitleText, 0,  wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxLEFT|wxTOP|wxADJUST_MINSIZE, 9);

    wxBoxSizer* centerHorizontalBoxSizer = new wxBoxSizer(wxHORIZONTAL);
    mainBoxSizer->Add(centerHorizontalBoxSizer, 1, wxGROW|wxALL, 5);

	wxPanel* centerPanel = new wxPanel( customSQLDialog, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
    centerHorizontalBoxSizer->Add(centerPanel, 0, wxALIGN_CENTER_HORIZONTAL|wxALL, 1);

    wxBoxSizer* centerVerticalBoxSizer = new wxBoxSizer(wxVERTICAL);
    centerPanel->SetSizer(centerVerticalBoxSizer);

	// ListBox for source code
    sqlSourceTxtCtrl_ = new wxTextCtrl( customSQLDialog, DIALOG_CUSTOM_SQL_TXTCTRL_SOURCE, _T(""), wxDefaultPosition, wxSize(-1, sourceTextHeight), wxTE_MULTILINE );
    centerHorizontalBoxSizer->Add(sqlSourceTxtCtrl_, 1, wxGROW|wxALL, 1);

    /****************************************
     Bottom Panel
     ***************************************/
    wxPanel* buttonPanel = new wxPanel( customSQLDialog, wxID_ANY,  wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
    mainBoxSizer->Add(buttonPanel, 0, wxALIGN_CENTER_HORIZONTAL|wxALL, 0);

    wxBoxSizer* buttonPanelSizer = new wxBoxSizer(wxHORIZONTAL);
    buttonPanel->SetSizer(buttonPanelSizer);

    button_Load_ = new wxButton( buttonPanel, DIALOG_CUSTOM_SQL_BTN_LOAD, _("&Load"), wxDefaultPosition, wxDefaultSize, 0 );
    buttonPanelSizer->Add(button_Load_, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);
    button_Load_->SetToolTip(_("Locate and load an SQL script file."));

    button_Save_ = new wxButton( buttonPanel, DIALOG_CUSTOM_SQL_BTN_SAVE, _("&Save"), wxDefaultPosition, wxDefaultSize, 0 );
    buttonPanelSizer->Add(button_Save_, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);
    button_Save_->SetToolTip(_("Save Heading details to the Index File, and SQL script file as required."));

    button_Run_ = new wxButton( buttonPanel, DIALOG_CUSTOM_SQL_BTN_QUERY, _("&Run"), wxDefaultPosition, wxDefaultSize, 0 );
    buttonPanelSizer->Add(button_Run_, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);
    button_Run_->SetToolTip(_("Test script. Save before running. SQL errors will result in loss of script details."));

    wxButton* button_Clear = new wxButton( buttonPanel, DIALOG_CUSTOM_SQL_BTN_CLEAR, _("C&lear"), wxDefaultPosition, wxDefaultSize, 0 );
    buttonPanelSizer->Add(button_Clear, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);
    button_Clear->SetToolTip(_("Clear the SQL Source script area"));

    wxButton* button_Close = new wxButton( buttonPanel, DIALOG_CUSTOM_SQL_BTN_CLOSE, _("&Close"), wxDefaultPosition, wxDefaultSize, 0 );
    buttonPanelSizer->Add(button_Close, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);
    button_Close->SetToolTip(_("Save changes before closing. Changes without Saving will be lost."));

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
            sqlSourceTxtCtrl_->ChangeValue(sqlData);
        }
        button_Load_->Disable();
    } 
    else
    {
        button_Run_->Disable();
    }
}

wxString mmCustomSQLDialog::sqlQuery()
{
    return sqlQuery_;
}

void mmCustomSQLDialog::OnLoad(wxCommandEvent& /*event*/)
{
    wxString sqlScriptFileName = wxFileSelector( _("Load Custom SQL file:"), 
                                                 mmex::getPathUser(mmex::DIRECTORY),
                                                 wxT(""), wxT(""),  wxT("SQL File(*.sql)|*.sql"), wxFILE_MUST_EXIST);
    if ( !sqlScriptFileName.empty() )
    {
        wxFileName selectedFileName(sqlScriptFileName);
        loadedFileName_ = selectedFileName.GetFullName();
        wxString sqlText;

        wxTextFile sqlFile(sqlScriptFileName);  
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
            sqlSourceTxtCtrl_->SetValue(sqlText);
            newFileCreated_ = false;
            sqlFile.Close();
        }
        else
        {
            wxString msg = wxString() << _("Unable to open file.") << sqlScriptFileName << wxT("\n\n");
            wxMessageBox(msg,reportIndex_->UserDialogHeading(),wxOK|wxICON_ERROR);
        }
    }
}

void mmCustomSQLDialog::OnSave(wxCommandEvent& /*event*/)
{
    button_Save_->Disable();
    wxString reportfileName;

    if (! edit_ && reportIndex_->ReportListHasItems() )
    {
        reportIndex_->getUserTitleSelection(_(" entry location:\nOK....: Insert before selected entry\nCancel: Add at bottom of List."));
    }

    if ( headingOnlyCheckBox_->GetValue() ) 
    {
        reportIndex_->addReportTitle(reportTitleTxtCtrl_->GetValue(), edit_);
        navCtrlUpdateRequired_ = true;
    }
    else
    {
        if (loadedFileName_.IsEmpty())
        {
            // get the filename from the user
            wxFileDialog fileDialog(this, _("Save Custom SQL file:"),
                                          mmex::getPathUser(mmex::DIRECTORY),
                                          wxT(""),wxT("SQL File(*.sql)|*.sql"),wxSAVE);
            if (fileDialog.ShowModal() == wxID_OK)
            {
                reportfileName = fileDialog.GetFilename();
                wxFileName tempFileName(reportfileName);
                if (tempFileName.GetExt() == wxT(""))
                    reportfileName << wxT(".sql");
            }
        }
        else
        {
            reportfileName = loadedFileName_;
        }

        // If we have a filename, and we have something to save, save the file.
        wxString sqlSource = sqlSourceTxtCtrl_->GetValue();
        if ( !reportfileName.empty() && !sqlSource.empty() )
        {
            wxTextFile sqlSourceFile(mmex::getPathUser(mmex::DIRECTORY) + reportfileName);

            // If the file does not exist and cannot be created, throw an error
            if ( !sqlSourceFile.Exists() && !sqlSourceFile.Create() )
            {
                wxMessageBox(_("Unable to write to file."),reportIndex_->UserDialogHeading(), wxICON_ERROR);
                return;
            }

            // if the file does exist, then update the file contents to the new value.
            if (sqlSourceFile.Exists())
            {
                // Make sure the file is open
                if (! sqlSourceFile.IsOpened())
                    sqlSourceFile.Open();

                sqlSourceFile.Clear();
                sqlSourceFile.AddLine(sqlSource);
            }
            sqlSourceFile.Write();
            sqlSourceFile.Close();
        }

        // update the index file
        reportIndex_->addReportTitle(reportTitleTxtCtrl_->GetValue(), edit_, reportfileName, subMenuCheckBox_->GetValue() );
        navCtrlUpdateRequired_ = true;
    }
}

void mmCustomSQLDialog::OnRun(wxCommandEvent& /*event*/)
{
   wxString lower = sqlSourceTxtCtrl_->GetValue().Lower();
   if ((lower.Find(wxT("update")) != wxNOT_FOUND) || (lower.Find(wxT("delete")) != wxNOT_FOUND) || (lower.Find(wxT("insert")) != wxNOT_FOUND))
   {
		wxMessageDialog msgDlg(this, _("SQL Query will modify your Data. Proceed??"), _("Warning"), wxYES_NO);
        if (msgDlg.ShowModal() == wxID_YES)
		{
            sqlQuery_ = sqlSourceTxtCtrl_->GetValue();
            EndModal(wxID_MORE);
        }
        else
        {
            return;
        }
   }
   else
   {
       sqlQuery_ = sqlSourceTxtCtrl_->GetValue();
       EndModal(wxID_MORE);
   }
}

void mmCustomSQLDialog::OnClear(wxCommandEvent& /*event*/)
{
    sqlSourceTxtCtrl_->Clear();
    button_Save_->Disable();
    button_Run_->Disable();
    if (! headingOnlyCheckBox_->GetValue())
        button_Load_->Enable();
    sqlSourceTxtCtrl_->SetFocus();
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
    sqlSourceTxtCtrl_->Enable(!headingOnlyCheckBox_->GetValue());
    button_Load_->Enable(!headingOnlyCheckBox_->GetValue());
    button_Run_->Enable(!headingOnlyCheckBox_->GetValue());
}

void mmCustomSQLDialog::OnCheckedHeading(wxCommandEvent& /*event*/)
{
    button_Save_->Enable();
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
}

void mmCustomSQLDialog::OnTextChangeSubReport(wxCommandEvent& /*event*/)
{
    button_Save_->Enable();
    if (! sqlSourceTxtCtrl_->IsEmpty())
    {
        button_Run_->Enable();
        button_Load_->Disable();
    }
}

//mmCustomSQLDialog::~mmCustomSQLDialog()
//{
//    wxMessageBox(wxT("Testing that the dialog is being destroyed.\nGoodby.."),wxT("Custom SQL Dialog destructor..."));
//}
