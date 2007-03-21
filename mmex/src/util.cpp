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
#include "util.h"
#include "dbwrapper.h"
#include "guiid.h"
#include "fileviewerdialog.h"
#include "mmex.h"
#include "univcsv.h"
#include "mmcoredb.h"

wxString mmGetBaseWorkingPath()
{
   if (wxTheApp->argc > 1)
   {
      wxString path = wxTheApp->argv[1];
      if (wxFileName::DirExists(path))
         return path;
   }
   wxFileName fname(wxTheApp->argv[0]);
   return fname.GetPath(wxPATH_GET_VOLUME);
}

void mmSelectLanguage(wxSQLite3Database* inidb, bool showSelection)
{
	wxFileName fname(wxTheApp->argv[0]);
	mmGUIApp* mmApp = dynamic_cast<mmGUIApp*>(wxTheApp);
	wxASSERT(mmApp);
	/*******************************************************/
	/* Select Language */
    // TODO : Issue with wxWidgets 2.6.2 causes crash when trying to AddCatalog
    // http://cvs.wxwidgets.org/viewcvs.cgi/wxWindows/src/common/intl.cpp.diff?r1=1.166&r2=1.166.2.1
    /*******************************************************/
	wxString langStr = mmDBWrapper::getINISettingValue(inidb, 
		wxT("LANGUAGE"), wxT("")); 

	wxString langPath = fname.GetPath(wxPATH_GET_VOLUME)
		+ wxT("//en//");
#ifdef __WXGTK__
    mmApp->m_locale.AddCatalogLookupPathPrefix(fname.GetPath(wxPATH_GET_VOLUME));
#else
    mmApp->m_locale.AddCatalogLookupPathPrefix(langPath);
#endif
	if (langStr == wxT("") || showSelection)
	{
		wxArrayString langFileArray;
		if (wxDir::Exists(langPath))
		{
			int num = (int)wxDir::GetAllFiles(langPath, &langFileArray, wxT("*.mo"));
			if (num > 0)
			{
				for (int ix = 0; ix < num; ix++)
				{
					wxFileName fname(langFileArray[ix]);
					wxString name = fname.GetName();
					langFileArray[ix] = name;
				}

				langStr = wxGetSingleChoice
					(
					wxT("Please choose language:"),
					wxT("Language"),
					langFileArray
					);

				mmApp->m_locale.AddCatalog(langStr);

				/* Save Language Setting */
				mmDBWrapper::setINISettingValue(inidb, wxT("LANGUAGE"), 
					langStr);
			}
		}
	}
	else
	{
		/* Previous language found */
		mmApp->m_locale.AddCatalog(langStr);
	}
}

wxString mmCleanString(const wxString& orig)
{
    wxString toReturn = orig;
    toReturn.Replace(wxT("'"), wxT("''"));
    return toReturn;
}

wxString mmCleanQuotes(const wxString& orig)
{
    wxString toReturn = orig;
    toReturn.Replace(wxT("'"), wxT("''"));
    toReturn.Replace(wxT("\""), wxT(""));
    toReturn.Trim();
    return toReturn; 
}

wxString mmUnCleanString(const wxString& orig)
{
    wxString toReturn = orig;
    toReturn.Replace(wxT("''"), wxT("'"));
    return toReturn;
}

wxString mmReadyDisplayString(const wxString& orig)
{
    wxString toReturn = orig;
    toReturn.Replace(wxT("&"), wxT("&&"));
    return toReturn;
}

wxString mmGetNiceMonthName(int month)
{
    const wxString gMonthsInYear[12] =
    {
		_("January"), _("February"), _("March"), _("April"), _("May"), _("June"),
		_("July"), _("August"), _("September"), _("October"), _("November"), _("December")
	};

    return gMonthsInYear[month];
}

wxString mmGetNiceDateString(wxDateTime dt)
{
#if 0
    return dt.Format(wxT("%A, %B %d, %Y"));
#endif


    const wxString gDaysInWeek[7] =
	{
		_("Sunday"), _("Monday"), _("Tuesday"), _("Wednesday"),
		_("Thursday"), _("Friday"), _("Saturday")
	};
	
	wxString dts(gDaysInWeek[dt.GetWeekDay()] + wxString(wxT(", ")));
    dts += mmGetNiceMonthName(dt.GetMonth()) + wxString(wxT(" "));
    dts += wxString::Format(wxT("%d"), dt.GetDay()) + wxT(", ") 
        + wxString::Format(wxT("%d"), dt.GetYear());
	return dts;    
}

wxString mmGetNiceDateSimpleString(wxDateTime dt)
{
    wxString dts = wxDateTime::GetMonthName(dt.GetMonth()) + wxString(wxT(" "));
    dts += wxString::Format(wxT("%d"), dt.GetDay()) + wxT(", ") 
        + wxString::Format(wxT("%d"), dt.GetYear());
    return dts;
}

void mmShowErrorMessage(wxWindow* parent, wxString message, wxString messageheader)
{
     wxMessageDialog msgDlg(parent, message,
                                messageheader);
      msgDlg.ShowModal();
}

void mmExportCSV(wxSQLite3Database* db_)
{
 if (mmDBWrapper::getNumAccounts(db_) == 0)
    {
        mmShowErrorMessage(0, _("No Account available! Cannot Export!"), _("Error"));
        return;
    }
    wxArrayString as;
    
    mmBEGINSQL_LITE_EXCEPTION;
    wxSQLite3ResultSet q1 = 
        db_->ExecuteQuery("select * from ACCOUNTLIST_V1 where ACCOUNTTYPE='Checking' order by ACCOUNTNAME;");
    while (q1.NextRow())
    {
        as.Add(q1.GetString(wxT("ACCOUNTNAME")));
    }

    mmENDSQL_LITE_EXCEPTION;

    wxString delimit = mmDBWrapper::getInfoSettingValue(db_, wxT("DELIMITER"), DEFDELIMTER);
    
    wxSingleChoiceDialog* scd = new wxSingleChoiceDialog(0, _("Choose Account to Export from:"), 
        _("CSV Export"), as);
    if (scd->ShowModal() == wxID_OK)
    {
        wxString acctName = scd->GetStringSelection();
        int fromAccountID = mmDBWrapper::getAccountID(db_, acctName);

         wxString fileName = wxFileSelector(_("Choose CSV data file to Export"), 
                wxT(""), wxT(""), wxT(""), wxT("*.csv"), wxSAVE | wxOVERWRITE_PROMPT);
        if ( !fileName.IsEmpty() )
        {
            wxFileOutputStream output( fileName );
            wxTextOutputStream text( output );

            /* Create Log File */
            wxFileName fname(wxTheApp->argv[0]);
            wxFileName csvName(fileName);

            mmBEGINSQL_LITE_EXCEPTION;
            wxString bufSQL = wxString::Format(wxT("SELECT TRANSDATE, \
                TRANSCODE, TRANSAMOUNT,  SUBCATEGID,         \
                CATEGID, PAYEEID, \
                TRANSACTIONNUMBER, NOTES, TOACCOUNTID, ACCOUNTID FROM CHECKINGACCOUNT_V1 \
                where ACCOUNTID = %d OR TOACCOUNTID = %d;"), fromAccountID, fromAccountID );
            wxSQLite3ResultSet q1 = db_->ExecuteQuery(bufSQL);
            int numRecords = 0;
            while (q1.NextRow())
            {
                wxString dateDBString = q1.GetString(wxT("TRANSDATE"));
                wxDateTime dtdt = mmGetStorageStringAsDate(dateDBString);
                wxString dateString = mmGetDateForDisplay(db_, dtdt);

                int sid, cid;
                wxString payee = mmDBWrapper::getPayee(db_, q1.GetInt(wxT("PAYEEID")), sid, cid);
                wxString type = q1.GetString(wxT("TRANSCODE"));
                wxString amount = q1.GetString(wxT("TRANSAMOUNT"));
                wxString categ = mmDBWrapper::getCategoryName(db_, q1.GetInt(wxT("CATEGID")));
                wxString subcateg = mmDBWrapper::getSubCategoryName(db_, 
                    q1.GetInt(wxT("CATEGID")), q1.GetInt(wxT("SUBCATEGID")));
                wxString transNum = q1.GetString(wxT("TRANSACTIONNUMBER"));
                wxString notes = mmUnCleanString(q1.GetString(wxT("NOTES")));
               
                if (type == wxT("Transfer"))
                {
                   int tAccountID = q1.GetInt(wxT("TOACCOUNTID"));
                   int fAccountID = q1.GetInt(wxT("ACCOUNTID"));

                   wxString fromAccount = mmDBWrapper::getAccountName(db_,  fAccountID);
                   wxString toAccount = mmDBWrapper::getAccountName(db_,  tAccountID );

                   if (tAccountID == fromAccountID)
                   {
                      type = wxT("Deposit");
                      payee = fromAccount;
                   }
                   else if (fAccountID == fromAccountID)
                   {
                      type = wxT("Withdrawal");
                      payee = toAccount;
                   }
                }
                text << dateString << delimit << payee << delimit << type << delimit << amount
                     << delimit << categ << delimit << subcateg << delimit << transNum 
                     << delimit << notes << endl;
                numRecords++;
            }
            q1.Finalize();

            wxString msg = wxString::Format(wxT("%d transactions exported"), numRecords);
            mmShowErrorMessage(0, msg, _("Export to CSV"));

            mmENDSQL_LITE_EXCEPTION;
        }// if fileName.empty();

    }// show Modal
    scd->Destroy();
}

int mmImportCSV(mmCoreDB* core)
{
    wxSQLite3Database* db_ = core->db_.get();
    if (mmDBWrapper::getNumAccounts(db_) == 0)
    {
        mmShowErrorMessage(0, _("No Account available! Cannot Import! Create a new account first!"), 
            _("Error"));
        return -1;
    }

    wxArrayString as;
    int fromAccountID = -1;
    
    mmBEGINSQL_LITE_EXCEPTION;

    wxSQLite3ResultSet q1 
        = db_->ExecuteQuery("select * from ACCOUNTLIST_V1 where ACCOUNTTYPE='Checking' order by ACCOUNTNAME;");
    while (q1.NextRow())
    {
        as.Add(q1.GetString(wxT("ACCOUNTNAME")));
    }

    mmENDSQL_LITE_EXCEPTION;

    wxString delimit = mmDBWrapper::getInfoSettingValue(db_, wxT("DELIMITER"), DEFDELIMTER);
    
    wxSingleChoiceDialog* scd = new wxSingleChoiceDialog(0, _("Choose Account to import to:"), 
        _("CSV Import"), as);
    if (scd->ShowModal() == wxID_OK)
    {
        wxString acctName = scd->GetStringSelection();
        fromAccountID = mmDBWrapper::getAccountID(db_, acctName);
        boost::shared_ptr<mmCurrency> pCurrencyPtr = core->accountList_.getCurrencyWeakPtr(fromAccountID).lock();
        wxASSERT(pCurrencyPtr);
     
        wxString fileName = wxFileSelector(wxT("Choose CSV data file to import"), 
                wxT(""), wxT(""), wxT(""), wxT("*.csv"), wxFILE_MUST_EXIST);
        if ( !fileName.empty() )
        {
            wxFileInputStream input( fileName );
            wxTextInputStream text( input );

            /* Create Log File */
            wxFileName fname(wxTheApp->argv[0]);
            wxFileName csvName(fileName);
            wxString logFile = fname.GetPath(wxPATH_GET_VOLUME) + wxT("\\") 
                + csvName.GetName() + wxT(".txt");
            wxFileOutputStream outputLog( logFile );
            wxTextOutputStream log( outputLog );

            // We have a fixed format for now
            // date, payeename, "withdrawal/deposit", amount, category, subcategory, transactionnumber, notes
            int countNumTotal = 0;
            int countImported = 0;
            while ( !input.Eof() )
            {
                wxString line = text.ReadLine();
                if (!line.IsEmpty())
                    countNumTotal++;
                else
                    continue;

                wxString dt = wxDateTime::Now().FormatISODate();
                wxString payee = wxT("");
                wxString type = wxT("");
                wxString amount = wxT("");
                wxString categ = wxT("");
                wxString subcateg = wxT("");
                wxString transNum = wxT("");
                wxString notes = wxT("");

                wxStringTokenizer tkz(line, delimit, wxTOKEN_RET_EMPTY_ALL);
                if (tkz.HasMoreTokens())
                   dt = tkz.GetNextToken();
                else
                {
                    log << _("Line : " ) << countNumTotal 
                        << _(" missing date, skipping.") << endl;
                    continue;
                }

                if (tkz.HasMoreTokens())
                   payee = mmCleanString(tkz.GetNextToken());
                else
                {
                    log << _("Line : " ) << countNumTotal 
                        << _(" missing payee, skipping.") << endl;
                    continue;
                }

                if (tkz.HasMoreTokens())
                    type = mmCleanString(tkz.GetNextToken());
                else
                {
                    log << _("Line : " ) << countNumTotal 
                        << _(" missing transaction type, skipping.") << endl;
                    continue;
                }

                if (tkz.HasMoreTokens())
                   amount = mmCleanString(tkz.GetNextToken());
                else
                {
                    log << _("Line : " ) << countNumTotal 
                        << _(" missing amount, skipping.") << endl;
                    continue;
                }

                if (tkz.HasMoreTokens())
                   categ = mmCleanString(tkz.GetNextToken());
              
                if (tkz.HasMoreTokens())
                   subcateg = mmCleanString(tkz.GetNextToken());
                
                if (tkz.HasMoreTokens())
                   transNum = mmCleanString(tkz.GetNextToken());
                
                if (tkz.HasMoreTokens())
                   notes = mmCleanString(tkz.GetNextToken());
                
                if (dt.Trim().IsEmpty() || payee.Trim().IsEmpty() ||
                    type.Trim().IsEmpty() || amount.Trim().IsEmpty())
                {
                    log << _("Line : " ) << countNumTotal 
                        << _("one of the following fields: date, payee, transaction type, amount strings is empty, skipping") << endl;
                    continue;
                }
                
               if (type.Cmp(wxT("Withdrawal")) && type.Cmp(wxT("Deposit")))
               {
                    log << _("Line : " ) << countNumTotal 
                        << _(" unrecognized transaction type, skipping.") << endl;
                    continue;
               }

               double val = 0.0;
               if (!amount.ToDouble(&val))
               {
                   log << _("Line : " ) << countNumTotal 
                        << _(" invalid amount, skipping.") << endl;

                   continue;
               }
        
               wxDateTime dtdt = mmParseDisplayStringToDate(db_, dt);
               wxString convDate = dtdt.FormatISODate();

               int payeeID, categID , subCategID;
               categID = -1;
               subCategID = -1;

               if (!core->payeeList_.payeeExists(payee))
               {
                   //payee does not exist
                   payeeID = core->payeeList_.addPayee(payee);

                   if (categ.Trim().IsEmpty())
                       categ = wxT("Unknown");

                   categID = core->categoryList_.getCategoryID(categ);
                   if (categID == -1)
                   {
                       categID =  core->categoryList_.addCategory(categ);
                   }
               }
               else
               {
                   payeeID = core->payeeList_.getPayeeID(payee);
                   boost::shared_ptr<mmPayee> pPayee =  core->payeeList_.getPayeeSharedPtr(payeeID);
                   boost::shared_ptr<mmCategory> pCategory = pPayee->category_.lock();
                   if (!pCategory)
                   {
                        // missing category for exisitng payee
                       if (categ.Trim().IsEmpty())
                       {
                           // empty category
                           categ = wxT("Unknown");

                           categID = core->categoryList_.getCategoryID(categ);
                           if (categID == -1)
                           {
                               categID =  core->categoryList_.addCategory(categ);
                           }
                       }
                       else
                       {
                           // non-empty category
                           if (!core->categoryList_.categoryExists(categ))
                           {
                               categID = core->categoryList_.addCategory(categ);
                           }
                           else
                               categID = core->categoryList_.getCategoryID(categ);

                           if (!subcateg.Trim().IsEmpty())
                           {
                               subCategID = core->categoryList_.getSubCategoryID(categID, subcateg);
                               if (subCategID == -1)
                               {
                                   subCategID = core->categoryList_.addSubCategory(categID, subcateg);
                               }
                           }
                       } 
                   }
                   else
                   {
                       if (pCategory->parent_.lock())
                       {
                           categID = pCategory->parent_.lock()->categID_;
                           subCategID = pCategory->categID_; 
                       }
                       else
                       {
                            categID = pCategory->categID_; 
                            subCategID = -1;
                       }
                   }

               }

               wxString status = wxT("F");
               int toAccountID = -1;

               boost::shared_ptr<mmBankTransaction> pTransaction(new mmBankTransaction(core->db_));
               pTransaction->accountID_ = fromAccountID;
               pTransaction->toAccountID_ = toAccountID;
               pTransaction->payee_ = core->payeeList_.getPayeeSharedPtr(payeeID);
               pTransaction->transType_ = type;
               pTransaction->amt_ = val;
               pTransaction->status_ = status;
               pTransaction->transNum_ = transNum;
               pTransaction->notes_ = mmCleanString(notes.c_str());
               pTransaction->category_ = core->categoryList_.getCategorySharedPtr(categID, subCategID);
               pTransaction->date_ = dtdt;
               pTransaction->toAmt_ = 0.0;
               pTransaction->updateAllData(core,fromAccountID,pCurrencyPtr );

               core->bTransactionList_.addTransaction(pTransaction);
               
               countImported++;
               log << _("Line : " ) << countNumTotal << _(" imported OK.") << endl;
            } // while EOF

            wxString msg = wxString::Format(_("Total Lines : %d.\nTotal Imported : %d\nLog file written to : %s.\n\nImported transactions have been flagged so you can review them."), 
                countNumTotal, countImported, logFile.c_str());
            mmShowErrorMessage(0, msg, _("Import from CSV"));
            outputLog.Close();

            fileviewer* dlg = new fileviewer(logFile, 0);
            dlg->ShowModal();
            dlg->Destroy();
        }
    }
    scd->Destroy();
    return fromAccountID;
 }

int mmImportCSVMMNET(mmCoreDB* core)
{
    wxSQLite3Database* db_ = core->db_.get();
    mmShowErrorMessage(0, 
        _("Please verify that the CSV file from MMEX.NET contains only one account!"),
        _("Import from CSV"));

    if (mmDBWrapper::getNumAccounts(db_) == 0)
    {
        mmShowErrorMessage(0, _("No Account available! Cannot Import! Create a new account first!"), 
            _("Error"));
        return -1;
    }

    wxArrayString as;
    int fromAccountID = -1;

    mmBEGINSQL_LITE_EXCEPTION;
    wxSQLite3ResultSet q1 = db_->ExecuteQuery("select * from ACCOUNTLIST_V1 where ACCOUNTTYPE='Checking' order by ACCOUNTNAME;");
    while (q1.NextRow())
    {
        as.Add(q1.GetString(wxT("ACCOUNTNAME")));
    }
    mmENDSQL_LITE_EXCEPTION

    wxString delimit = mmDBWrapper::getInfoSettingValue(db_, wxT("DELIMITER"), DEFDELIMTER);
    
    wxSingleChoiceDialog* scd = new wxSingleChoiceDialog(0, _("Choose Account to import to:"), 
        _("CSV Import"), as);
    if (scd->ShowModal() == wxID_OK)
    {
        wxString acctName = scd->GetStringSelection();
        fromAccountID = mmDBWrapper::getAccountID(db_, acctName);
        
        boost::shared_ptr<mmCurrency> pCurrencyPtr = core->accountList_.getCurrencyWeakPtr(fromAccountID).lock();
        wxASSERT(pCurrencyPtr);
     
        wxString fileName = wxFileSelector(_("Choose MM.NET CSV data file to import"), 
                wxT(""), wxT(""), wxT(""), wxT("*.csv"), wxFILE_MUST_EXIST);
        if ( !fileName.IsEmpty() )
        {
            wxFileInputStream input( fileName );
            wxTextInputStream text( input );

            /* Create Log File */
            wxFileName fname(wxTheApp->argv[0]);
            wxFileName csvName(fileName);
            wxString logFile = fname.GetPath(wxPATH_GET_VOLUME) + wxT("\\") 
                + csvName.GetName() + wxT(".txt");
            wxFileOutputStream outputLog( logFile );
            wxTextOutputStream log( outputLog );

            /* The following is the MM.NET CSV format */
            /* date, payeename, amount(+/-), Number, status, category : subcategory, notes */
            int countNumTotal = 0;
            int countImported = 0;
            while ( !input.Eof() )
            {
                wxString line = text.ReadLine();
                if (!line.IsEmpty())
                    countNumTotal++;
                else
                    continue;

                wxString dt = wxDateTime::Now().FormatISODate();
                wxString payee = wxT("");
                wxString type = wxT("");
                wxString amount = wxT("");
                wxString categ = wxT("");
                wxString subcateg = wxT("");
                wxString transNum = wxT("");
                wxString notes = wxT("");

                wxStringTokenizer tkz(line, delimit, wxTOKEN_RET_EMPTY_ALL);
                if (tkz.HasMoreTokens())
                   dt = tkz.GetNextToken();
                else
                {
                    log << _("Line : " ) << countNumTotal 
                        << _(" missing date, skipping.") << endl;
                    continue;
                }

                if (tkz.HasMoreTokens())
                   payee = tkz.GetNextToken();
                else
                {
                    log << _("Line : " ) << countNumTotal 
                        << _(" missing payee, skipping.") << endl;
                    continue;
                }

                if (tkz.HasMoreTokens())
                   amount = tkz.GetNextToken();
                else
                {
                    log << _("Line : " ) << countNumTotal 
                        << _(" missing amount, skipping.") << endl;
                    continue;
                }

                if (tkz.HasMoreTokens())
                   transNum = tkz.GetNextToken();

                if (tkz.HasMoreTokens())
                   wxString status = tkz.GetNextToken();

                if (tkz.HasMoreTokens())
                   categ = tkz.GetNextToken();
                else
                {
                    log << _("Line : " ) << countNumTotal 
                        << _(" missing category, skipping.") << endl;
                    continue;
                }

                
                if (tkz.HasMoreTokens())
                   notes = tkz.GetNextToken();
                
               double val = 0.0;
               if (!amount.ToDouble(&val))
               {
                   log << _("Line : " ) << countNumTotal 
                        << _(" invalid amount, skipping.") << endl;

                   continue;
               }

               if (val <= 0.0)
                   type = wxT("Withdrawal");
               else
                   type = wxT("Deposit");
               val = fabs(val);

                if (dt.Trim().IsEmpty() || payee.Trim().IsEmpty() ||
                    type.Trim().IsEmpty() || amount.Trim().IsEmpty() ||
                     categ.Trim().IsEmpty())
                {
                    log << _("Line : " ) << countNumTotal 
                        << _(" One of the following fields: date, payee, transaction type, amount, category strings is empty, skipping") << endl;
                    continue;
                }
        
               wxDateTime dtdt;
               const wxChar* chr = dtdt.ParseDate(dt.GetData());
               if (chr == NULL)
               {
                    log << _("Line : " ) << countNumTotal 
                        << _(" invalid date, skipping.") << endl;
                   continue;
               }
               wxString convDate = dtdt.FormatISODate();

               int payeeID, categID, subCategID;
               if (!core->payeeList_.payeeExists(payee))
               {
                   payeeID = core->payeeList_.addPayee(payee);

               }
                
               // category contains ":" to separate categ:subcateg
               wxStringTokenizer cattkz(categ, wxT(":"));
               wxString cat, subcat;
               subcat = wxT("");
                if (cattkz.HasMoreTokens())
                   cat = cattkz.GetNextToken();
                if (cattkz.HasMoreTokens())
                   subcat = cattkz.GetNextToken();

                categID = core->categoryList_.getCategoryID(categ);
                if (categID == -1)
                {
                    categID =  core->categoryList_.addCategory(categ);
                }

               if (!subcat.IsEmpty())
               {
                   subCategID = core->categoryList_.getSubCategoryID(categID, subcateg);
                   if (subCategID == -1)
                   {
                       subCategID = core->categoryList_.addSubCategory(categID, subcateg);
                   }
               }

               wxString status = wxT("F");
               int toAccountID = -1;

               boost::shared_ptr<mmBankTransaction> pTransaction(new mmBankTransaction(core->db_));
               pTransaction->accountID_ = fromAccountID;
               pTransaction->toAccountID_ = toAccountID;
               pTransaction->payee_ = core->payeeList_.getPayeeSharedPtr(payeeID);
               pTransaction->transType_ = type;
               pTransaction->amt_ = val;
               pTransaction->status_ = status;
               pTransaction->transNum_ = transNum;
               pTransaction->notes_ = mmCleanString(notes.c_str());
               pTransaction->category_ = core->categoryList_.getCategorySharedPtr(categID, subCategID);
               pTransaction->date_ = dtdt;
               pTransaction->toAmt_ = 0.0;
               pTransaction->updateAllData(core, fromAccountID, pCurrencyPtr);

               core->bTransactionList_.addTransaction(pTransaction);

               countImported++;
               log << _("Line : " ) << countNumTotal << _(" imported OK.") << endl;
            } // while EOF

            wxString msg = wxString::Format(_("Total Lines : %d \nTotal Imported : %d\n\nLog file written to : %s.\n\nImported transactions have been flagged so you can review them. "), countNumTotal, countImported, logFile.c_str());
            mmShowErrorMessage(0, msg, _("Import from CSV"));
            outputLog.Close();

            fileviewer* dlg = new fileviewer(logFile, 0);
            dlg->ShowModal();
            dlg->Destroy();
        }
    }
    scd->Destroy();
    return fromAccountID;
 }


void mmShowErrorMessageInvalid(wxWindow* parent, wxString message)
{
   wxString msg = _("Entry ") + message + _(" is invalid.");
   wxMessageDialog msgDlg(parent, msg,
                           _("Invalid Entry"));
    msgDlg.ShowModal();
}

/* -------------------------------------------- */
wxString mmGetDateForDisplay(wxSQLite3Database* db, wxDateTime dt)
{
    wxString selection = mmUnCleanString(mmDBWrapper::getInfoSettingValue(db, wxT("DATEFORMAT"), DEFDATEFORMAT));
    return dt.Format(selection);
}

wxDateTime mmParseDisplayStringToDate(wxSQLite3Database* db, const wxString& dtstr)
{
    wxString selection = mmDBWrapper::getInfoSettingValue(db, wxT("DATEFORMAT"), DEFDATEFORMAT);
    wxDateTime dt;
    const wxChar* char1 = dt.ParseFormat(dtstr.GetData(), selection.GetData());
	if (char1 == NULL)
		return wxDateTime::Now();
    return dt;
}

wxString mmGetDateForStorage(wxDateTime dt)
{
    return dt.FormatISODate();
}

wxDateTime mmGetStorageStringAsDate(const wxString& str)
{
    wxDateTime dt;
    dt.ParseDate(str.GetData());
    return dt;
}

wxColour mmGetColourFromString(const wxString& str)
{
    wxStringTokenizer tkz(str, wxT(","),wxTOKEN_RET_EMPTY_ALL);
    unsigned char red = 0xFF;
    unsigned char blue = 0xFF;
    unsigned char green = 0xFF;
    if (tkz.HasMoreTokens())
    {
        long longVal;
        tkz.GetNextToken().ToLong(&longVal);
        red = longVal;
        if (tkz.HasMoreTokens())
        {
            tkz.GetNextToken().ToLong(&longVal);
            green = longVal;
            if (tkz.HasMoreTokens())
            {
                tkz.GetNextToken().ToLong(&longVal);
                blue = longVal;
            }
        }
    }
    return wxColour(red, green, blue);
}

wxString mmGetStringFromColour(wxColour color)
{
    wxString clr = wxString::Format(wxT("%d,%d,%d"), color.Red(), 
                                            color.Green(), 
                                            color.Blue());
    return clr;
}

void mmLoadColorsFromDatabase(wxSQLite3Database* db_)
{
    mmColors::listAlternativeColor0 = mmGetColourFromString(mmDBWrapper::getINISettingValue(db_, 
                                                wxT("LISTALT0"), wxT("225,237,251")));
    mmColors::listAlternativeColor1 = mmGetColourFromString(mmDBWrapper::getINISettingValue(db_, 
                                                wxT("LISTALT1"), wxT("255,255,255")));
    mmColors::listBackColor = mmGetColourFromString(mmDBWrapper::getINISettingValue(db_, 
                                                wxT("LISTBACK"), wxT("255,255,255")));
    mmColors::navTreeBkColor = mmGetColourFromString(mmDBWrapper::getINISettingValue(db_, 
                                                wxT("NAVTREE"), wxT("255,255,255")));
    mmColors::listBorderColor = mmGetColourFromString(mmDBWrapper::getINISettingValue(db_, 
                                                wxT("LISTBORDER"), wxT("0,0,0")));
    mmColors::listDetailsPanelColor = mmGetColourFromString(mmDBWrapper::getINISettingValue(db_, 
                                                wxT("LISTDETAILSPANEL"), wxT("244,247,251")));
}

void mmSaveColorsToDatabase(wxSQLite3Database* db_)
{
    mmDBWrapper::setINISettingValue(db_, wxT("LISTALT0"), 
        mmGetStringFromColour(mmColors::listAlternativeColor0));
    mmDBWrapper::setINISettingValue(db_, wxT("LISTALT1"), 
        mmGetStringFromColour(mmColors::listAlternativeColor1));
    mmDBWrapper::setINISettingValue(db_, wxT("LISTBACK"), 
        mmGetStringFromColour(mmColors::listBackColor));
    mmDBWrapper::setINISettingValue(db_, wxT("NAVTREE"), 
        mmGetStringFromColour(mmColors::navTreeBkColor));
    mmDBWrapper::setINISettingValue(db_, wxT("LISTBORDER"), 
        mmGetStringFromColour(mmColors::listBorderColor));
     mmDBWrapper::setINISettingValue(db_, wxT("LISTDETAILSPANEL"), 
        mmGetStringFromColour(mmColors::listDetailsPanelColor));
}

void mmRestoreDefaultColors()
{
    mmColors::listAlternativeColor0 = wxColour(225, 237, 251);
    mmColors::listAlternativeColor1 = wxColour(255, 255, 255);
    mmColors::listBackColor = wxColour(255, 255, 255);
    mmColors::navTreeBkColor = wxColour(255, 255, 255);
    mmColors::listBorderColor = wxColour(0, 0, 0);
    mmColors::listDetailsPanelColor = wxColour(244, 247, 251);
}

/* Set the default colors */
wxColour mmColors::listAlternativeColor0 = wxColour(225, 237, 251);
wxColour mmColors::listAlternativeColor1 = wxColour(255, 255, 255);
wxColour mmColors::listBackColor = wxColour(255, 255, 255);
wxColour mmColors::navTreeBkColor = wxColour(255, 255, 255);
wxColour mmColors::listBorderColor = wxColour(0, 0, 0);
wxColour mmColors::listDetailsPanelColor = wxColour(244, 247, 251);

/* -------------------------------------------- */
// setup the defaults for US Dollar
wxString  mmCurrencyFormatter::pfx_symbol     = wxT("$");
wxString  mmCurrencyFormatter::sfx_symbol      = wxT("");
wxChar    mmCurrencyFormatter::decimal_point   = wxT('.');
wxChar    mmCurrencyFormatter::group_separator = wxT(',');
wxString  mmCurrencyFormatter::unit_name       = wxT("dollar");
wxString  mmCurrencyFormatter::cent_name       = wxT("cent");
double    mmCurrencyFormatter::scale           = 100; 

double mmRound(const double x)            
{
    double dummy;
    return modf(x + (x<0 ? -.5 : .5),&dummy), dummy;
 }

short mmCents(double value) 
{
    double dummy; 
    return (short)( modf((value + (value < 0 ? -.5 :.5))/ mmCurrencyFormatter::scale, &dummy)* mmCurrencyFormatter::scale);
}

double mmMoneyInt(double value) 
{
    double dummy; return modf(double(value) / mmCurrencyFormatter::scale, &dummy),dummy;
}

void mmCurrencyFormatter::loadSettings(wxString pfx, wxString sfx, wxChar dec, wxChar grp,
                                wxString unit, wxString cent, double scale)
{
    mmCurrencyFormatter::pfx_symbol = pfx;
    mmCurrencyFormatter::sfx_symbol  = sfx;
    mmCurrencyFormatter::decimal_point   = dec;
    mmCurrencyFormatter::group_separator = grp;
    mmCurrencyFormatter::unit_name = unit;
    mmCurrencyFormatter::cent_name = cent;
    mmCurrencyFormatter::scale      = scale; 
}

void mmCurrencyFormatter::loadSettings(boost::shared_ptr<mmCurrency> pCurrencyPtr)
{
    mmCurrencyFormatter::pfx_symbol = pCurrencyPtr->pfxSymbol_;
    mmCurrencyFormatter::sfx_symbol  = pCurrencyPtr->sfxSymbol_;

    if (!pCurrencyPtr->dec_.IsEmpty())
    {
        mmCurrencyFormatter::decimal_point   = pCurrencyPtr->dec_.GetChar(0);
    }
    else
        mmCurrencyFormatter::decimal_point = 0;
    
    if (!pCurrencyPtr->grp_.IsEmpty())
    {
        mmCurrencyFormatter::group_separator = pCurrencyPtr->grp_.GetChar(0);
    }
    else
        mmCurrencyFormatter::group_separator = 0;

        
    mmCurrencyFormatter::unit_name = pCurrencyPtr->unit_;
    mmCurrencyFormatter::cent_name = pCurrencyPtr->cent_;
    mmCurrencyFormatter::scale      = pCurrencyPtr->scaleDl_;
}

void mmCurrencyFormatter::loadDefaultSettings()
{
    mmCurrencyFormatter::pfx_symbol = wxT("$");
    mmCurrencyFormatter::sfx_symbol  = wxT("");
    mmCurrencyFormatter::decimal_point   = wxT('.');
    mmCurrencyFormatter::group_separator = wxT(',');
    mmCurrencyFormatter::unit_name = wxT("dollar");
    mmCurrencyFormatter::cent_name = wxT("cent");
    mmCurrencyFormatter::scale      = 100; 
}

bool mmCurrencyFormatter::formatDoubleToCurrencyEdit(double val, wxString& rdata)
{
  wxString data = wxT("");
  double value = mmRound(val * mmCurrencyFormatter::scale);
  double absx = value > 0 ? value : - value;      //  Get magnitude of argument
  double whole = mmMoneyInt(absx);       //  Isolate whole monetary units
  short  cents = mmCents(absx);     //  Isolate fractional units
  double remdr = absx - ((whole * 100 + cents) / 100);

  int i = 0;
  if  (value < 0) 
     data += '-';    //  Print prefix minus, if needed
 // data = data + pfx_symbol;           //  Insert prefix

//  Print groups of 3 digits separated by punctuation
//  -------------------------------------------------

  const float group_divisors[6] = {1E0f, 1E3f, 1E6f, 1E9f, 1E12f, 1E15f};
  short  grpNum = (whole == 0) ? 0 : short(log10(whole) / 3);
  int    grpVal =  int(whole / group_divisors[grpNum]);

  data += wxString::Format(wxT("%d"), grpVal);  // Print leftmost 3-digits (no leading 0's)
  while (grpNum != 0)          // For remaining 3-digit groups
   {
      if (group_separator)
        data += group_separator;                     // Print group separator
    whole -= grpVal * group_divisors[grpNum--];   // Compute new remainder
    grpVal  = int(whole / group_divisors[grpNum]);// Get next 3-digit value
       
    if   (grpVal < 100) 
        data += '0';               // Insert embedded 0's
    if   (grpVal < 10)  
        data += '0'; //   as needed
    
    wxString newStr;
    // Print leftmost 3-digits (no leading 0's) 
    newStr = wxString::Format(wxT("%d"), grpVal);                
    wxString testString = wxString::Format(wxT("%s%s"), 
        data.c_str(), newStr.c_str());
    data = testString;
   }                                // Print 3-digit value

//  Print cents portion
//  -------------------

  if (decimal_point != 0)
  {
  data += decimal_point;
  if (cents < 10 )
  { 
    //data +=wxT("0"); 
  }

  wxString newStr;
  if ((int)mmCurrencyFormatter::scale == 100)
      newStr = wxString::Format(wxT("%02d"), cents);
  else if ((int)mmCurrencyFormatter::scale == 1000)
      newStr = wxString::Format(wxT("%03d"), cents);
  else if ((int)mmCurrencyFormatter::scale == 10)
      newStr = wxString::Format(wxT("%01d"), cents);

  wxString testString = wxString::Format(wxT("%s%s"), data.c_str(), newStr.c_str());
  data = testString;
  }


//  Append any additional fractional digits
//  ---------------------------------------
  //for (int i = int(mmCurrencyFormatter::scale/100 -1); i&&(remdr>0); i--, remdr/=10)
       //data += wxString(wxT("%d"), mmCents((10 * remdr)));
  //data += sfx_symbol;             //    Insert trailing currency symbol

  rdata = data;
  return true;
}

bool mmCurrencyFormatter::formatDoubleToCurrency(double val, wxString& rdata)
{
  wxString data = wxT("");
  double value = mmRound(val * mmCurrencyFormatter::scale);
  double absx = value > 0 ? value : - value;      //  Get magnitude of argument
  double whole = mmMoneyInt(absx);       //  Isolate whole monetary units
  short  cents = mmCents(absx);     //  Isolate fractional units
  double remdr = absx - ((whole * 100 + cents) / 100);

  int i = 0;
  data = data + pfx_symbol; //  insert prefix
  data = data + wxT(' ');  // insert space
  if  (value < 0) 
     data += '-';    //  print prefix minus, if needed
  

    //  Print groups of 3 digits separated by punctuation
    //  -------------------------------------------------

  const float group_divisors[6] = {1E0f, 1E3f, 1E6f, 1E9f, 1E12f, 1E15f};
  short  grpNum = (whole == 0) ? 0 : short(log10(whole) / 3);
  int    grpVal =  int(whole / group_divisors[grpNum]);

  data += wxString::Format(wxT("%d"), grpVal);                // Print leftmost 3-digits (no leading 0's)
  while (grpNum != 0)          // For remaining 3-digit groups
   {
       if (group_separator)
          data += group_separator;                     // Print group separator
    whole -= grpVal * group_divisors[grpNum--];   // Compute new remainder
    grpVal  = int(whole / group_divisors[grpNum]);// Get next 3-digit value
       
    if   (grpVal < 100) 
        data += '0';               // Insert embedded 0's
    if   (grpVal < 10)  
        data += '0'; //   as needed
    
    wxString newStr;
    newStr = wxString::Format(wxT("%d"), grpVal);                // Print leftmost 3-digits (no leading 0's) 
    wxString testString = wxString::Format(wxT("%s%s"), data.c_str(), newStr.c_str());
    data = testString;
   }                                // Print 3-digit value

//  Print cents portion
//  -------------------

  if (decimal_point != 0)
  {
      data += decimal_point;
      if (cents < 10 )
      { 
      //    data +=wxT("0"); 
      }

      wxString newStr;
      if ((int)mmCurrencyFormatter::scale == 100)
        newStr = wxString::Format(wxT("%02d"), cents);
      else if ((int)mmCurrencyFormatter::scale == 1000)
        newStr = wxString::Format(wxT("%03d"), cents);
      else if ((int)mmCurrencyFormatter::scale == 10)
        newStr = wxString::Format(wxT("%01d"), cents);

      wxString testString = wxString::Format(wxT("%s%s"), data.c_str(), newStr.c_str());
      data = testString;
  }


//  Append any additional fractional digits
//  ---------------------------------------
  //for (int i = int(mmCurrencyFormatter::scale/100 -1); i&&(remdr>0); i--, remdr/=10)
  //     data += wxString(wxT("%d"), mmCents((10 * remdr)));
  data += sfx_symbol;             //    Insert trailing currency symbol

  rdata = data;
  return true;
}

bool mmCurrencyFormatter::formatCurrencyToDouble(const wxString& str, double& val)
{
    wxString newStr;
    for(unsigned int idx = 0; idx < str.Len() ; idx++)
    {
        if (str[idx] == group_separator)
        {
                        
        }
        else if (str[idx] == decimal_point)
        {
            newStr += wxT('.');
        }
        else
        {
            /* if we ever make this intelligent, we need to make it accept neg numbers symbol*/
            newStr += str[idx];
        }
    }
    
    val = 0;
    if (!newStr.IsEmpty())
    {
        return newStr.ToDouble(&val);
    }
    
    return false;
}

