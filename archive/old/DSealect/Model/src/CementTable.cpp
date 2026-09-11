// CementTable.cpp: implementation of the CCementTable class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "CementTable.h"

#ifdef _DEBUG
#ifdef _MSC_VER#undef THIS_FILE
static char THIS_FILE[]=__FILE__;#endif  // _MSC_VER
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CCementTable::CCementTable(CDaoDatabase& Database)
: CDSTable(Database)
{

}

CCementTable::~CCementTable()
{

}

void CCementTable::Open()
{
	if(!RS().IsOpen()){
		//Open the table
		RS().Open(dbOpenTable, CEMENT_TABLE);
	}
}

void CCementTable::Create()
{
	CDaoTableDef TableDef(&Database());
	TableDef.Create(CEMENT_TABLE);
	CDaoFieldInfo fieldinfo;

	fieldinfo.m_nType = dbText;
	fieldinfo.m_lSize = 255;
	fieldinfo.m_lAttributes = 0;
	fieldinfo.m_bAllowZeroLength = TRUE;

	fieldinfo.m_strName = CEMENT_HEAD_MATERIAL_NAME;
	TableDef.CreateField(fieldinfo);

	fieldinfo.m_strName = CEMENT_TAIL_MATERIAL_NAME;
	TableDef.CreateField(fieldinfo);

	TableDef.Append();
	TableDef.Close();
}
