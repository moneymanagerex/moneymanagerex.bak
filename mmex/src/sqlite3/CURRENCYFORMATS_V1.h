/////////////////////////////////////////////////////
// Generated By wxActiveRecordGenerator v 1.2.0-rc3
/////////////////////////////////////////////////////


#ifndef __C_U_R_R_E_N_C_Y_F_O_R_M_A_T_S__V1_
#define __C_U_R_R_E_N_C_Y_F_O_R_M_A_T_S__V1_

////@@begin gen include
#include "wxActiveRecord.h"
#include <wx/wx.h>
#include <wx/string.h>
#include <wx/datetime.h>

////@@end gen include

////@@begin custom include
////@@end custom include

////@@begin gen forward
class CURRENCYFORMATS_V1;
class CURRENCYFORMATS_V1Row;
class CURRENCYFORMATS_V1RowSet;


////@@end gen forward

////@@begin custom forward
////@@end custom forward

////@@begin gen arClass
class CURRENCYFORMATS_V1: public wxSqliteActiveRecord{
protected:
	CURRENCYFORMATS_V1Row* RowFromResult(DatabaseResultSet* result);
public:
	CURRENCYFORMATS_V1();
	CURRENCYFORMATS_V1(const wxString& name,const wxString& server=wxEmptyString,const wxString& user=wxEmptyString,const wxString& password=wxEmptyString,const wxString& table=wxT("CURRENCYFORMATS_V1"));
	CURRENCYFORMATS_V1(DatabaseLayer* database,const wxString& table=wxT("CURRENCYFORMATS_V1"));
	bool Create(const wxString& name,const wxString& server=wxEmptyString,const wxString& user=wxEmptyString,const wxString& password=wxEmptyString,const wxString& table=wxT("CURRENCYFORMATS_V1"));
	
	CURRENCYFORMATS_V1Row* New();
	bool Delete(int key);

	
	CURRENCYFORMATS_V1Row* CURRENCYID(int key);

	CURRENCYFORMATS_V1Row* Where(const wxString& whereClause);
	CURRENCYFORMATS_V1RowSet* WhereSet(const wxString& whereClause,const wxString& orderBy=wxEmptyString);
	CURRENCYFORMATS_V1RowSet* All(const wxString& orderBy=wxEmptyString); 

////@@begin custom arClass
public:


////@@end custom arClass
};
////@@end gen arClass

////@@begin gen arRow
class CURRENCYFORMATS_V1Row: public wxActiveRecordRow{
public:
	CURRENCYFORMATS_V1Row();
	CURRENCYFORMATS_V1Row(const CURRENCYFORMATS_V1Row& src);
	CURRENCYFORMATS_V1Row(CURRENCYFORMATS_V1* activeRecord);
	CURRENCYFORMATS_V1Row(DatabaseLayer* database,const wxString& table=wxT("CURRENCYFORMATS_V1"));
	CURRENCYFORMATS_V1Row& operator=(const CURRENCYFORMATS_V1Row& src);
	bool GetFromResult(DatabaseResultSet* result);
public:
	int BASECONVRATE;
	wxString CURRENCYNAME;
	int SCALE;
	wxString CENT_NAME;
	wxString DECIMAL_POINT;
	int CURRENCYID;
	wxString GROUP_SEPARATOR;
	wxString CURRENCY_SYMBOL;
	wxString SFX_SYMBOL;
	wxString PFX_SYMBOL;
	wxString UNIT_NAME;

public:
	
	
	bool Save();
	bool Delete();
	
	
////@@begin custom arRow
public:


////@@end custom arRow	

};
////@@end gen arRow

////@@begin gen arSet
class CURRENCYFORMATS_V1RowSet: public wxActiveRecordRowSet{
public:
	CURRENCYFORMATS_V1RowSet();
	CURRENCYFORMATS_V1RowSet(wxActiveRecord* activeRecord);
	CURRENCYFORMATS_V1RowSet(DatabaseLayer* database,const wxString& table=wxT("CURRENCYFORMATS_V1"));
	virtual CURRENCYFORMATS_V1Row* Item(unsigned long item);
	
	virtual bool SaveAll();
	
	
protected:
	static int CMPFUNC_BASECONVRATE(wxActiveRecordRow** item1,wxActiveRecordRow** item2);
	static int CMPFUNC_CURRENCYNAME(wxActiveRecordRow** item1,wxActiveRecordRow** item2);
	static int CMPFUNC_SCALE(wxActiveRecordRow** item1,wxActiveRecordRow** item2);
	static int CMPFUNC_CENT_NAME(wxActiveRecordRow** item1,wxActiveRecordRow** item2);
	static int CMPFUNC_DECIMAL_POINT(wxActiveRecordRow** item1,wxActiveRecordRow** item2);
	static int CMPFUNC_CURRENCYID(wxActiveRecordRow** item1,wxActiveRecordRow** item2);
	static int CMPFUNC_GROUP_SEPARATOR(wxActiveRecordRow** item1,wxActiveRecordRow** item2);
	static int CMPFUNC_CURRENCY_SYMBOL(wxActiveRecordRow** item1,wxActiveRecordRow** item2);
	static int CMPFUNC_SFX_SYMBOL(wxActiveRecordRow** item1,wxActiveRecordRow** item2);
	static int CMPFUNC_PFX_SYMBOL(wxActiveRecordRow** item1,wxActiveRecordRow** item2);
	static int CMPFUNC_UNIT_NAME(wxActiveRecordRow** item1,wxActiveRecordRow** item2);
	static int CMPFUNC_global(wxActiveRecordRow** item1,wxActiveRecordRow** item2);
	virtual CMPFUNC_proto GetCmpFunc(const wxString& var) const;

////@@begin custom arSet
public:


////@@end custom arSet
};
////@@end gen arSet

#endif /* __C_U_R_R_E_N_C_Y_F_O_R_M_A_T_S__V1_ */