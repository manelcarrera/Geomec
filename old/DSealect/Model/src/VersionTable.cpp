// VersionTable.cpp: implementation of the CVersionTable class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "VersionTable.h"

#ifdef _DEBUG
#ifdef _MSC_VER#undef THIS_FILE
static char THIS_FILE[]=__FILE__;#endif  // _MSC_VER
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CVersionTable::CVersionTable(CDaoDatabase& Database)
: CDSTable(Database)
{

}

CVersionTable::~CVersionTable()
{

}


void CVersionTable::Open()
{
	CString sqlStatement;
	sqlStatement = "SELECT * FROM ";
	sqlStatement += TableName();
	sqlStatement += " WHERE ";
	sqlStatement += VERSION_NAME;
	sqlStatement += " = '";
	sqlStatement += m_VersionName;
	sqlStatement += "'";

	CDaoQueryDef qd(&Database());
	qd.Create(NULL, sqlStatement);

	if(!RS().IsOpen()){
		// Open the table
		RS().Open(&qd,dbOpenDynaset,dbDenyWrite);
	}
}

void CVersionTable::Create()
{
	CDaoTableDef TableDef(&Database());
	try
	{
		TableDef.Create(VERSION_TABLE);
		TableDef.CreateField(VERSION_NAME,dbText,255);
		TableDef.CreateField(VERSION,dbInteger,0);
		TableDef.CreateField(REVISION,dbInteger,0);
		TableDef.Append();	
		TableDef.Close();
	}
	catch(...)
	{
	}
}


void  CVersionTable::VersionName(const CString name)
{
	m_VersionName=name;
}

CString CVersionTable::VersionName() const
{
	return m_VersionName;
}

CString CVersionTable::TableName()
{
	return VERSION_TABLE;
}