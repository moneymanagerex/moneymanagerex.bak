/////////////////////////////////////////////////////
// Generated By wxActiveRecordGenerator v 1.2.0-rc3
/////////////////////////////////////////////////////

#include "SUBCATEGORY_V1.h"

/** ACTIVE_RECORD **/
SUBCATEGORY_V1::SUBCATEGORY_V1():wxSqliteActiveRecord(){
}

SUBCATEGORY_V1::SUBCATEGORY_V1(const wxString& name,const wxString& server,const wxString& user,const wxString& password,const wxString& table):wxSqliteActiveRecord(name,server,user,password,table){
}

SUBCATEGORY_V1::SUBCATEGORY_V1(DatabaseLayer* database,const wxString& table):wxSqliteActiveRecord(database,table){
}

bool SUBCATEGORY_V1::Create(const wxString& name,const wxString& server,const wxString& user,const wxString& password,const wxString& table){
	return wxSqliteActiveRecord::Create(name,server,user,password,table);
}

SUBCATEGORY_V1Row* SUBCATEGORY_V1::RowFromResult(DatabaseResultSet* result){
	SUBCATEGORY_V1Row* row=new SUBCATEGORY_V1Row(this);
	
	row->GetFromResult(result);
	
	return row;
}

SUBCATEGORY_V1Row* SUBCATEGORY_V1::New(){
	SUBCATEGORY_V1Row* newRow=new SUBCATEGORY_V1Row(this);
	garbageRows.Add(newRow);
	return newRow;
}
bool SUBCATEGORY_V1::Delete(int key){
	try{
		PreparedStatement* pStatement=m_database->PrepareStatement(wxString::Format(wxT("DELETE FROM %s WHERE SUBCATEGID=?"),m_table.c_str()));
		pStatement->SetParamInt(1,key);
		pStatement->ExecuteUpdate();
		return true;
	}
	catch(DatabaseLayerException& e){
		throw(e);
		return false;
	}
}



SUBCATEGORY_V1Row* SUBCATEGORY_V1::SUBCATEGID(int key){
	try{
		PreparedStatement* pStatement=m_database->PrepareStatement(wxString::Format(wxT("SELECT * FROM %s WHERE SUBCATEGID=?"),m_table.c_str()));
		pStatement->SetParamInt(1,key);
		DatabaseResultSet* result= pStatement->ExecuteQuery();

		result->Next();
		SUBCATEGORY_V1Row* row=RowFromResult(result);
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




SUBCATEGORY_V1Row* SUBCATEGORY_V1::Where(const wxString& whereClause){
	try{
		wxString prepStatement = wxString::Format(wxT("SELECT * FROM %s WHERE %s"),m_table.c_str(),whereClause.c_str());
		PreparedStatement* pStatement=m_database->PrepareStatement(prepStatement);
		DatabaseResultSet* result= pStatement->ExecuteQuery();
		
		if(!result->Next())
			return NULL;
		SUBCATEGORY_V1Row* row=RowFromResult(result);
		
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

SUBCATEGORY_V1RowSet* SUBCATEGORY_V1::WhereSet(const wxString& whereClause,const wxString& orderBy){
	SUBCATEGORY_V1RowSet* rowSet=new SUBCATEGORY_V1RowSet();
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


SUBCATEGORY_V1RowSet* SUBCATEGORY_V1::All(const wxString& orderBy){
	SUBCATEGORY_V1RowSet* rowSet=new SUBCATEGORY_V1RowSet();
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

SUBCATEGORY_V1Row::SUBCATEGORY_V1Row():wxActiveRecordRow(){
	bool newRow=true;
}

SUBCATEGORY_V1Row::SUBCATEGORY_V1Row(SUBCATEGORY_V1* activeRecord):wxActiveRecordRow(activeRecord){
	bool newRow=true;
}

SUBCATEGORY_V1Row::SUBCATEGORY_V1Row(const SUBCATEGORY_V1Row& src){
	if(&src==this)
		return;
	newRow=src.newRow;
	
	SUBCATEGNAME=src.SUBCATEGNAME;
	SUBCATEGID=src.SUBCATEGID;
	CATEGID=src.CATEGID;

}

SUBCATEGORY_V1Row::SUBCATEGORY_V1Row(DatabaseLayer* database,const wxString& table):wxActiveRecordRow(database,table){
	newRow=true;
}
	

SUBCATEGORY_V1Row& SUBCATEGORY_V1Row::operator=(const SUBCATEGORY_V1Row& src){
	if(&src==this)
		return *this;
	newRow=src.newRow;
	
	SUBCATEGNAME=src.SUBCATEGNAME;
	SUBCATEGID=src.SUBCATEGID;
	CATEGID=src.CATEGID;


	return *this;
}

bool SUBCATEGORY_V1Row::GetFromResult(DatabaseResultSet* result){
	
	newRow=false;
		SUBCATEGNAME=result->GetResultString(wxT("SUBCATEGNAME"));
	SUBCATEGID=result->GetResultInt(wxT("SUBCATEGID"));
	CATEGID=result->GetResultInt(wxT("CATEGID"));


	return true;
}
	

bool SUBCATEGORY_V1Row::Save(){
	try{
		if(newRow){
			PreparedStatement* pStatement=m_database->PrepareStatement(wxString::Format(wxT("INSERT INTO %s (SUBCATEGNAME,CATEGID) VALUES (?,?)"),m_table.c_str()));
			pStatement->SetParamString(1,SUBCATEGNAME);
			pStatement->SetParamInt(2,CATEGID);
			pStatement->RunQuery();
			m_database->CloseStatement(pStatement);

			
			newRow=false;
		}
		else{
			PreparedStatement* pStatement=m_database->PrepareStatement(wxString::Format(wxT("UPDATE %s SET SUBCATEGNAME=?,CATEGID=? WHERE SUBCATEGID=?"),m_table.c_str()));
			pStatement->SetParamString(1,SUBCATEGNAME);
			pStatement->SetParamInt(3,SUBCATEGID);
			pStatement->SetParamInt(2,CATEGID);
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

bool SUBCATEGORY_V1Row::Delete(){
	try{
		PreparedStatement* pStatement=m_database->PrepareStatement(wxString::Format(wxT("DELETE FROM %s WHERE SUBCATEGID=?"),m_table.c_str()));
		pStatement->SetParamInt(1,SUBCATEGID);
		pStatement->ExecuteUpdate();
		return true;
	}
	catch(DatabaseLayerException& e){
		throw(e);
		return false;
	}
}



CATEGORY_V1Row* SUBCATEGORY_V1Row::GetCATEGORY_V1(){
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



/** END ACTIVE RECORD ROW **/

/** ACTIVE RECORD ROW SET **/

SUBCATEGORY_V1RowSet::SUBCATEGORY_V1RowSet():wxActiveRecordRowSet(){
}

SUBCATEGORY_V1RowSet::SUBCATEGORY_V1RowSet(wxActiveRecord* activeRecord):wxActiveRecordRowSet(activeRecord){
}

SUBCATEGORY_V1RowSet::SUBCATEGORY_V1RowSet(DatabaseLayer* database,const wxString& table):wxActiveRecordRowSet(database,table){
}

SUBCATEGORY_V1Row* SUBCATEGORY_V1RowSet::Item(unsigned long item){
	return (SUBCATEGORY_V1Row*)wxActiveRecordRowSet::Item(item);
}


bool SUBCATEGORY_V1RowSet::SaveAll(){
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


int SUBCATEGORY_V1RowSet::CMPFUNC_SUBCATEGNAME(wxActiveRecordRow** item1,wxActiveRecordRow** item2){
	SUBCATEGORY_V1Row** m_item1=(SUBCATEGORY_V1Row**)item1;
	SUBCATEGORY_V1Row** m_item2=(SUBCATEGORY_V1Row**)item2;
	return (*m_item1)->SUBCATEGNAME.Cmp((*m_item2)->SUBCATEGNAME);
}

int SUBCATEGORY_V1RowSet::CMPFUNC_SUBCATEGID(wxActiveRecordRow** item1,wxActiveRecordRow** item2){
	SUBCATEGORY_V1Row** m_item1=(SUBCATEGORY_V1Row**)item1;
	SUBCATEGORY_V1Row** m_item2=(SUBCATEGORY_V1Row**)item2;
	if((*m_item1)->SUBCATEGID<(*m_item2)->SUBCATEGID)
		return -1;
	else if((*m_item1)->SUBCATEGID>(*m_item2)->SUBCATEGID)
		return 1;
	else
		return 0;
}

int SUBCATEGORY_V1RowSet::CMPFUNC_CATEGID(wxActiveRecordRow** item1,wxActiveRecordRow** item2){
	SUBCATEGORY_V1Row** m_item1=(SUBCATEGORY_V1Row**)item1;
	SUBCATEGORY_V1Row** m_item2=(SUBCATEGORY_V1Row**)item2;
	if((*m_item1)->CATEGID<(*m_item2)->CATEGID)
		return -1;
	else if((*m_item1)->CATEGID>(*m_item2)->CATEGID)
		return 1;
	else
		return 0;
}

CMPFUNC_proto SUBCATEGORY_V1RowSet::GetCmpFunc(const wxString& var) const{
	if(var==wxT("SUBCATEGNAME"))
		return (CMPFUNC_proto)CMPFUNC_SUBCATEGNAME;
	else if(var==wxT("SUBCATEGID"))
		return (CMPFUNC_proto)CMPFUNC_SUBCATEGID;
	else if(var==wxT("CATEGID"))
		return (CMPFUNC_proto)CMPFUNC_CATEGID;
	else 
	return (CMPFUNC_proto)CMPFUNC_default;
}



/** END ACTIVE RECORD ROW SET **/

////@@begin custom implementations



////@@end custom implementations