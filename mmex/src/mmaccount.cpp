#include "mmaccount.h"

mmAccount::mmAccount(wxSQLite3ResultSet& q1)
{
   bool favoriteAcct_;
   double initialBalance_;
   boost::shared_ptr< mmCurrency*> currency_;

    accountID_ = q1.GetInt(wxT("ACCOUNTID"));
    accountName_ = q1.GetString(wxT("ACCOUNTNAME"));
    accountNum_  = q1.GetString(wxT("ACCOUNTNUM"));
    heldAt_ = q1.GetString(wxT("HELDAT"));
    website_ = q1.GetString(wxT("WEBSITE"));
    contactInfo_ = q1.GetString(wxT("CONTACTINFO"));
    accessInfo_ = q1.GetString(wxT("ACCESSINFO"));
    notes_ = q1.GetString(wxT("NOTES"));  
    acctType_ = q1.GetString(wxT("ACCOUNTTYPE"));
    status_ =  mmAccount::MMEX_Open;

    if (q1.GetString(wxT("STATUS")) == wxT("Closed"))
       status_ = mmAccount::MMEX_Closed;

        wxString retVal = q1.GetString(wxT("FAVORITEACCT"));
        if (retVal == wxT("TRUE"))
            favoriteAcct_ = true;
        else
            favoriteAcct_ = false;

        //textCtrl = (wxTextCtrl*)FindWindow(ID_DIALOG_NEWACCT_TEXTCTRL_INITBALANCE);
        //wxString initStr = q1.GetString(wxT("INITIALBAL"), wxT("0"));
        //double initBal = 0.0;
        //initStr.ToDouble(&initBal);
        


}