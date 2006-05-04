#include "mmextransaction.h"
#include "util.h"

mmTransaction::mmTransaction(mmDataTable* data, long transID)
{
    data_ = data;
    transID_ = transID;
}

mmTransaction::~mmTransaction()
{
    if (data_)
        delete data_;
}

TiXmlNode* mmTransaction::saveToFile()
{
  TiXmlElement* transaction = new TiXmlElement( wxT("mmTransaction"));
  TiXmlElement transactionID( wxT("TransactionID"));

  wxString temp;
  temp.sprintf("%d", transID_);  
  TiXmlText text(temp.GetData());

  transactionID.InsertEndChild(text);
  transaction->InsertEndChild(transactionID);

  TiXmlNode* data = data_->saveToFile();  

  transaction->InsertEndChild(*data);
  delete data;
  return transaction;  
}

void mmTransaction::loadFromXML(TiXmlElement* element)
{
    TiXmlNode* node = 0;
	for( node = element->IterateChildren( 0 );
		 node;
		 node = element->IterateChildren( node ) )
	{
		if (node->ToElement())
		{
			if (!strcmp(node->ToElement()->Value(), wxT("TransactionID")))
            {
                TiXmlNode* child = node->FirstChild();
                if (child)
                {
                    transID_ = atoi(child->ToText()->Value());
                }
            }
            else if (!strcmp(node->ToElement()->Value(), "mmDataTable"))
            {
                data_ = new mmDataTable();
                data_->loadFromXML(node->ToElement());
            }
		}
	}
}

/* ----------------------------- */
mmWithdrawalTransaction::mmWithdrawalTransaction(int transID) 
       : mmTransaction(new mmDataTable(), transID)
{
  
}

double mmWithdrawalTransaction::toAmount()
{
    double data;
    bool retVal = data_->getData(wxString(wxT("AMOUNT")), data);
    if (!retVal)
    {
        assert(false);
    }
    return data;
}

TiXmlNode* mmWithdrawalTransaction::saveToFile()
{
   TiXmlElement* mmWithTrans = new TiXmlElement(wxT("mmWithdrawalTransaction"));
   TiXmlNode* base = mmTransaction::saveToFile(); 
   mmWithTrans->InsertEndChild(*base);

   delete base;

   return mmWithTrans;
}

void mmWithdrawalTransaction::loadFromXML(TiXmlElement* element)
{
    TiXmlNode* node = 0;
	for( node = element->IterateChildren( 0 );
		 node;
		 node = element->IterateChildren( node ) )
	{
		if (node->ToElement())
		{
			if (!strcmp(node->ToElement()->Value(), wxT("mmTransaction")))
            {
                mmTransaction::loadFromXML(node->ToElement());
            }
        }
    }
    
}
/* ------------------------------------------------------ */

mmDepositTransaction::mmDepositTransaction(int transID) 
       : mmTransaction(new mmDataTable(), transID)
{
  
}

double mmDepositTransaction::toAmount()
{
    double data;
    bool retVal = data_->getData(wxString(wxT("AMOUNT")), data);
    if (!retVal)
    {
        assert(false);
    }
    return data;
}

TiXmlNode* mmDepositTransaction::saveToFile()
{
   TiXmlElement* mmWithTrans = new TiXmlElement(wxT("mmDepositTransaction"));
   TiXmlNode* base = mmTransaction::saveToFile(); 
   mmWithTrans->InsertEndChild(*base);

   delete base;

   return mmWithTrans;
}

void mmDepositTransaction::loadFromXML(TiXmlElement* element)
{
    TiXmlNode* node = 0;
	for( node = element->IterateChildren( 0 );
		 node;
		 node = element->IterateChildren( node ) )
	{
		if (node->ToElement())
		{
			if (!strcmp(node->ToElement()->Value(), wxT("mmTransaction")))
            {
                mmTransaction::loadFromXML(node->ToElement());
            }
        }
    }
    
}
/* ------------------------------------------------------ */
mmTransferTransaction::mmTransferTransaction(int transID) 
       : mmTransaction(new mmDataTable(), transID)
{
  
}

double mmTransferTransaction::toAmount()
{
    double data;
    bool retVal = data_->getData(wxString(wxT("AMOUNT")), data);
    if (!retVal)
    {
        assert(false);
    }
    return data;
}

TiXmlNode* mmTransferTransaction::saveToFile()
{
   TiXmlElement* mmWithTrans = new TiXmlElement(wxT("mmTransferTransaction"));
   TiXmlNode* base = mmTransaction::saveToFile(); 
   mmWithTrans->InsertEndChild(*base);

   delete base;

   return mmWithTrans;
}

void mmTransferTransaction::loadFromXML(TiXmlElement* element)
{
    TiXmlNode* node = 0;
	for( node = element->IterateChildren( 0 );
		 node;
		 node = element->IterateChildren( node ) )
	{
		if (node->ToElement())
		{
			if (!strcmp(node->ToElement()->Value(), wxT("mmTransaction")))
            {
                mmTransaction::loadFromXML(node->ToElement());
            }
        }
    }
}

long mmTransferTransaction::getFromAccount()
{
    long fromid;
    getData()->getData(wxT("FROMACCT"), fromid);
    return fromid;
}
   
long mmTransferTransaction::getToAccount()
{
    long toid;
    getData()->getData(wxT("TOACCT"), toid);
    return toid;
}