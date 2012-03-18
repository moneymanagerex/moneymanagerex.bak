/////////////////////////////////////////////////////
// Generated By wxActiveRecordGenerator v 1.2.0-rc3
/////////////////////////////////////////////////////

#include "BUDGETSPLITTRANSACTIONS_V1.h"

/** ACTIVE_RECORD **/
BUDGETSPLITTRANSACTIONS_V1::BUDGETSPLITTRANSACTIONS_V1():wxSqliteActiveRecord(){
}

BUDGETSPLITTRANSACTIONS_V1::BUDGETSPLITTRANSACTIONS_V1(const wxString& name,const wxString& server,const wxString& user,const wxString& password,const wxString& table):wxSqliteActiveRecord(name,server,user,password,table){
}

BUDGETSPLITTRANSACTIONS_V1::BUDGETSPLITTRANSACTIONS_V1(DatabaseLayer* database,const wxString& table):wxSqliteActiveRecord(database,table){
}

bool BUDGETSPLITTRANSACTIONS_V1::Create(const wxString& name,const wxString& server,const wxString& user,const wxString& password,const wxString& table){
	return wxSqliteActiveRecord::Create(name,server,user,password,table);
}

BUDGETSPLITTRANSACTIONS_V1Row* BUDGETSPLITTRANSACTIONS_V1::RowFromResult(DatabaseResultSet* result){
	BUDGETSPLITTRANSACTIONS_V1Row* row=new BUDGETSPLITTRANSACTIONS_V1Row(this);
	
	row->GetFromResult(result);
	
	return row;
}

BUDGETSPLITTRANSACTIONS_V1Row* BUDGETSPLITTRANSACTIONS_V1::New(){
	BUDGETSPLITTRANSACTIONS_V1Row* newRow=new BUDGETSPLITTRANSACTIONS_V1Row(this);
	garbageRows.Add(newRow);
	return newRow;
}
bool BUDGETSPLITTRANSACTIONS_V1::Delete(int key){
	try{
		PreparedStatement* pStatement=m_database->PrepareStatement(wxString::Format(wxT("DELETE FROM %s WHERE SPLITTRANSID=?"),m_table.c_str()));
		pStatement->SetParamInt(1,key);
		pStatement->ExecuteUpdate();
		return true;
	}
	catch(DatabaseLayerException& e){
		throw(e);
		return false;
	}
}



BUDGETSPLITTRANSACTIONS_V1Row* BUDGETSPLITTRANSACTIONS_V1::SPLITTRANSID(int key){
	try{
		PreparedStatement* pStatement=m_database->PrepareStatement(wxString::Format(wxT("SELECT * FROM %s WHERE SPLITTRANSID=?"),m_table.c_str()));
		pStatement->SetParamInt(1,key);
		DatabaseResultSet* result= pStatement->ExecuteQuery();

		result->Next();
		BUDGETSPLITTRANSACTIONS_V1Row* row=RowFromResult(result);
		garbageRows.Add(row);
		m_database->CloseResultSet(result);
		m_database->CloseStatement(pStatement);
		return row;
	}
	catch (DatabaseLayerException& e)
	{
		ProcessException(e);
		return NULL;
	}
}




BUDGETSPLITTRANSACTIONS_V1Row* BUDGETSPLITTRANSACTIONS_V1::Where(const wxString& whereClause){
	try{
		wxString prepStatement = wxString::Format(wxT("SELECT * FROM %s WHERE %s"),m_table.c_str(),whereClause.c_str());
		PreparedStatement* pStatement=m_database->PrepareStatement(prepStatement);
		DatabaseResultSet* result= pStatement->ExecuteQuery();
		
		if(!result->Next())
			return NULL;
		BUDGETSPLITTRANSACTIONS_V1Row* row=RowFromResult(result);
		
		garbageRows.Add(row);
		m_database->CloseResultSet(result);
		m_database->CloseStatement(pStatement);						
		return row;
	}
	catch (DatabaseLayerException& e)
	{
		ProcessException(e);
		return 0;
	}
}

BUDGETSPLITTRANSACTIONS_V1RowSet* BUDGETSPLITTRANSACTIONS_V1::WhereSet(const wxString& whereClause,const wxString& orderBy){
	BUDGETSPLITTRANSACTIONS_V1RowSet* rowSet=new BUDGETSPLITTRANSACTIONS_V1RowSet();
	try{
		wxString prepStatement=wxString::Format(wxT("SELECT * FROM %s WHERE %s"),m_table.c_str(),whereClause.c_str());
		if(!orderBy.IsEmpty())
			prepStatement+=wxT(" ORDER BY ")+orderBy;
		PreparedStatement* pStatement=m_database->PrepareStatement(prepStatement);
		DatabaseResultSet* result= pStatement->ExecuteQuery();
		
		if(result){
			while(result->Next()){
				rowSet->Add(RowFromResult(result));
			}
		}
		
		garbageRowSets.Add(rowSet);
		m_database->CloseResultSet(result);
		m_database->CloseStatement(pStatement);	
		return rowSet;
		
	}
	catch (DatabaseLayerException& e)
	{
		ProcessException(e);
		return 0;
	}
}


BUDGETSPLITTRANSACTIONS_V1RowSet* BUDGETSPLITTRANSACTIONS_V1::All(const wxString& orderBy){
	BUDGETSPLITTRANSACTIONS_V1RowSet* rowSet=new BUDGETSPLITTRANSACTIONS_V1RowSet();
	try{
		wxString prepStatement=wxString::Format(wxT("SELECT * FROM %s"),m_table.c_str());
		if(!orderBy.IsEmpty())
			prepStatement+=wxT(" ORDER BY ")+orderBy;
		PreparedStatement* pStatement=m_database->PrepareStatement(prepStatement);
		
		DatabaseResultSet* result= pStatement->ExecuteQuery();
		
		if(result){
			while(result->Next()){
				rowSet->Add(RowFromResult(result));
			}
		}
		garbageRowSets.Add(rowSet);
		m_database->CloseResultSet(result);
		m_database->CloseStatement(pStatement);	
		return rowSet;
		
	}
	catch (DatabaseLayerException& e)
	{
		ProcessException(e);
		return 0;
	}
}

/** END ACTIVE RECORD **/

/** ACTIVE RECORD ROW **/

BUDGETSPLITTRANSACTIONS_V1Row::BUDGETSPLITTRANSACTIONS_V1Row():wxActiveRecordRow(){
	bool newRow=true;
}

BUDGETSPLITTRANSACTIONS_V1Row::BUDGETSPLITTRANSACTIONS_V1Row(BUDGETSPLITTRANSACTIONS_V1* activeRecord):wxActiveRecordRow(activeRecord){
	bool newRow=true;
}

BUDGETSPLITTRANSACTIONS_V1Row::BUDGETSPLITTRANSACTIONS_V1Row(const BUDGETSPLITTRANSACTIONS_V1Row& src){
	if(&src==this)
		return;
	newRow=src.newRow;
	
	SUBCATEGID=src.SUBCATEGID;
	SPLITTRANSAMOUNT=src.SPLITTRANSAMOUNT;
	SPLITTRANSID=src.SPLITTRANSID;
	CATEGID=src.CATEGID;
	TRANSID=src.TRANSID;

}

BUDGETSPLITTRANSACTIONS_V1Row::BUDGETSPLITTRANSACTIONS_V1Row(DatabaseLayer* database,const wxString& table):wxActiveRecordRow(database,table){
	newRow=true;
}
	

BUDGETSPLITTRANSACTIONS_V1Row& BUDGETSPLITTRANSACTIONS_V1Row::operator=(const BUDGETSPLITTRANSACTIONS_V1Row& src){
	if(&src==this)
		return *this;
	newRow=src.newRow;
	
	SUBCATEGID=src.SUBCATEGID;
	SPLITTRANSAMOUNT=src.SPLITTRANSAMOUNT;
	SPLITTRANSID=src.SPLITTRANSID;
	CATEGID=src.CATEGID;
	TRANSID=src.TRANSID;


	return *this;
}

bool BUDGETSPLITTRANSACTIONS_V1Row::GetFromResult(DatabaseResultSet* result){
	
	newRow=false;
		SUBCATEGID=result->GetResultInt(wxT("SUBCATEGID"));
	SPLITTRANSAMOUNT=result->GetResultString(wxT("SPLITTRANSAMOUNT"));
	SPLITTRANSID=result->GetResultInt(wxT("SPLITTRANSID"));
	CATEGID=result->GetResultInt(wxT("CATEGID"));
	TRANSID=result->GetResultInt(wxT("TRANSID"));


	return true;
}
	

bool BUDGETSPLITTRANSACTIONS_V1Row::Save(){
	try{
		if(newRow){
			PreparedStatement* pStatement=m_database->PrepareStatement(wxString::Format(wxT("INSERT INTO %s (SUBCATEGID,SPLITTRANSAMOUNT,CATEGID,TRANSID) VALUES (?,?,?,?)"),m_table.c_str()));
			pStatement->SetParamInt(1,SUBCATEGID);
			pStatement->SetParamString(2,SPLITTRANSAMOUNT);
			pStatement->SetParamInt(3,CATEGID);
			pStatement->SetParamInt(4,TRANSID);
			pStatement->RunQuery();
			m_database->CloseStatement(pStatement);

			
			newRow=false;
		}
		else{
			PreparedStatement* pStatement=m_database->PrepareStatement(wxString::Format(wxT("UPDATE %s SET SUBCATEGID=?,SPLITTRANSAMOUNT=?,CATEGID=?,TRANSID=? WHERE SPLITTRANSID=?"),m_table.c_str()));
			pStatement->SetParamInt(1,SUBCATEGID);
			pStatement->SetParamString(2,SPLITTRANSAMOUNT);
			pStatement->SetParamInt(5,SPLITTRANSID);
			pStatement->SetParamInt(3,CATEGID);
			pStatement->SetParamInt(4,TRANSID);
			pStatement->RunQuery();
			m_database->CloseStatement(pStatement);

		}
		
		return true;
	}
	catch (DatabaseLayerException& e)
	{
		wxActiveRecord::ProcessException(e);
		return false;
	}
}

bool BUDGETSPLITTRANSACTIONS_V1Row::Delete(){
	try{
		PreparedStatement* pStatement=m_database->PrepareStatement(wxString::Format(wxT("DELETE FROM %s WHERE SPLITTRANSID=?"),m_table.c_str()));
		pStatement->SetParamInt(1,SPLITTRANSID);
		pStatement->ExecuteUpdate();
		return true;
	}
	catch(DatabaseLayerException& e){
		throw(e);
		return false;
	}
}



CHECKINGACCOUNT_V1Row* BUDGETSPLITTRANSACTIONS_V1Row::GetCHECKINGACCOUNT_V1(){
	CHECKINGACCOUNT_V1Row* owner= new CHECKINGACCOUNT_V1Row(m_database,wxT("CHECKINGACCOUNT_V1"));
	PreparedStatement* pStatement=m_database->PrepareStatement(wxT("SELECT * FROM CHECKINGACCOUNT_V1 WHERE TRANSID=?"));
	pStatement->SetParamInt(1,TRANSID);
	pStatement->SetParamInt(1,TRANSID);
	DatabaseResultSet* result= pStatement->ExecuteQuery();

	result->Next();
	owner->GetFromResult(result);
	garbageRows.Add(owner);
	m_database->CloseResultSet(result);
	m_database->CloseStatement(pStatement);
	return owner;
}

CATEGORY_V1Row* BUDGETSPLITTRANSACTIONS_V1Row::GetCATEGORY_V1(){
	CATEGORY_V1Row* owner= new CATEGORY_V1Row(m_database,wxT("CATEGORY_V1"));
	PreparedStatement* pStatement=m_database->PrepareStatement(wxT("SELECT * FROM CATEGORY_V1 WHERE CATEGID=?"));
	pStatement->SetParamInt(1,CATEGID);
	pStatement->SetParamInt(1,CATEGID);
	DatabaseResultSet* result= pStatement->ExecuteQuery();

	result->Next();
	owner->GetFromResult(result);
	garbageRows.Add(owner);
	m_database->CloseResultSet(result);
	m_database->CloseStatement(pStatement);
	return owner;
}

SUBCATEGORY_V1Row* BUDGETSPLITTRANSACTIONS_V1Row::GetSUBCATEGORY_V1(){
	SUBCATEGORY_V1Row* owner= new SUBCATEGORY_V1Row(m_database,wxT("SUBCATEGORY_V1"));
	PreparedStatement* pStatement=m_database->PrepareStatement(wxT("SELECT * FROM SUBCATEGORY_V1 WHERE SUBCATEGID=?"));
	pStatement->SetParamInt(1,SUBCATEGID);
	pStatement->SetParamInt(1,SUBCATEGID);
	DatabaseResultSet* result= pStatement->ExecuteQuery();

	result->Next();
	owner->GetFromResult(result);
	garbageRows.Add(owner);
	m_database->CloseResultSet(result);
	m_database->CloseStatement(pStatement);
	return owner;
}



/** END ACTIVE RECORD ROW **/

/** ACTIVE RECORD ROW SET **/

BUDGETSPLITTRANSACTIONS_V1RowSet::BUDGETSPLITTRANSACTIONS_V1RowSet():wxActiveRecordRowSet(){
}

BUDGETSPLITTRANSACTIONS_V1RowSet::BUDGETSPLITTRANSACTIONS_V1RowSet(wxActiveRecord* activeRecord):wxActiveRecordRowSet(activeRecord){
}

BUDGETSPLITTRANSACTIONS_V1RowSet::BUDGETSPLITTRANSACTIONS_V1RowSet(DatabaseLayer* database,const wxString& table):wxActiveRecordRowSet(database,table){
}

BUDGETSPLITTRANSACTIONS_V1Row* BUDGETSPLITTRANSACTIONS_V1RowSet::Item(unsigned long item){
	return (BUDGETSPLITTRANSACTIONS_V1Row*)wxActiveRecordRowSet::Item(item);
}


bool BUDGETSPLITTRANSACTIONS_V1RowSet::SaveAll(){
	try{
		m_database->BeginTransaction();
		for(unsigned long i=0;i<Count();i++)
			Item(i)->Save();
		m_database->Commit();
		return true;
	}
	catch (DatabaseLayerException& e)
	{
		m_database->RollBack();
		wxActiveRecord::ProcessException(e);
		return false;
	}
}


int BUDGETSPLITTRANSACTIONS_V1RowSet::CMPFUNC_SUBCATEGID(wxActiveRecordRow** item1,wxActiveRecordRow** item2){
	BUDGETSPLITTRANSACTIONS_V1Row** m_item1=(BUDGETSPLITTRANSACTIONS_V1Row**)item1;
	BUDGETSPLITTRANSACTIONS_V1Row** m_item2=(BUDGETSPLITTRANSACTIONS_V1Row**)item2;
	if((*m_item1)->SUBCATEGID<(*m_item2)->SUBCATEGID)
		return -1;
	else if((*m_item1)->SUBCATEGID>(*m_item2)->SUBCATEGID)
		return 1;
	else
		return 0;
}

int BUDGETSPLITTRANSACTIONS_V1RowSet::CMPFUNC_SPLITTRANSAMOUNT(wxActiveRecordRow** item1,wxActiveRecordRow** item2){
	BUDGETSPLITTRANSACTIONS_V1Row** m_item1=(BUDGETSPLITTRANSACTIONS_V1Row**)item1;
	BUDGETSPLITTRANSACTIONS_V1Row** m_item2=(BUDGETSPLITTRANSACTIONS_V1Row**)item2;
	return (*m_item1)->SPLITTRANSAMOUNT.Cmp((*m_item2)->SPLITTRANSAMOUNT);
}

int BUDGETSPLITTRANSACTIONS_V1RowSet::CMPFUNC_SPLITTRANSID(wxActiveRecordRow** item1,wxActiveRecordRow** item2){
	BUDGETSPLITTRANSACTIONS_V1Row** m_item1=(BUDGETSPLITTRANSACTIONS_V1Row**)item1;
	BUDGETSPLITTRANSACTIONS_V1Row** m_item2=(BUDGETSPLITTRANSACTIONS_V1Row**)item2;
	if((*m_item1)->SPLITTRANSID<(*m_item2)->SPLITTRANSID)
		return -1;
	else if((*m_item1)->SPLITTRANSID>(*m_item2)->SPLITTRANSID)
		return 1;
	else
		return 0;
}

int BUDGETSPLITTRANSACTIONS_V1RowSet::CMPFUNC_CATEGID(wxActiveRecordRow** item1,wxActiveRecordRow** item2){
	BUDGETSPLITTRANSACTIONS_V1Row** m_item1=(BUDGETSPLITTRANSACTIONS_V1Row**)item1;
	BUDGETSPLITTRANSACTIONS_V1Row** m_item2=(BUDGETSPLITTRANSACTIONS_V1Row**)item2;
	if((*m_item1)->CATEGID<(*m_item2)->CATEGID)
		return -1;
	else if((*m_item1)->CATEGID>(*m_item2)->CATEGID)
		return 1;
	else
		return 0;
}

int BUDGETSPLITTRANSACTIONS_V1RowSet::CMPFUNC_TRANSID(wxActiveRecordRow** item1,wxActiveRecordRow** item2){
	BUDGETSPLITTRANSACTIONS_V1Row** m_item1=(BUDGETSPLITTRANSACTIONS_V1Row**)item1;
	BUDGETSPLITTRANSACTIONS_V1Row** m_item2=(BUDGETSPLITTRANSACTIONS_V1Row**)item2;
	if((*m_item1)->TRANSID<(*m_item2)->TRANSID)
		return -1;
	else if((*m_item1)->TRANSID>(*m_item2)->TRANSID)
		return 1;
	else
		return 0;
}

CMPFUNC_proto BUDGETSPLITTRANSACTIONS_V1RowSet::GetCmpFunc(const wxString& var) const{
	if(var==wxT("SUBCATEGID"))
		return (CMPFUNC_proto)CMPFUNC_SUBCATEGID;
	else if(var==wxT("SPLITTRANSAMOUNT"))
		return (CMPFUNC_proto)CMPFUNC_SPLITTRANSAMOUNT;
	else if(var==wxT("SPLITTRANSID"))
		return (CMPFUNC_proto)CMPFUNC_SPLITTRANSID;
	else if(var==wxT("CATEGID"))
		return (CMPFUNC_proto)CMPFUNC_CATEGID;
	else if(var==wxT("TRANSID"))
		return (CMPFUNC_proto)CMPFUNC_TRANSID;
	else 
	return (CMPFUNC_proto)CMPFUNC_default;
}



/** END ACTIVE RECORD ROW SET **/

////@@begin custom implementations



////@@end custom implementations