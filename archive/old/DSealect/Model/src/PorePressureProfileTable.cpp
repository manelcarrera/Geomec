#include "stdafx.h"
#include "DSModelDatabase.h"
#include "PorePressureProfileTable.h"

CPorePressureProfileTable::CPorePressureProfileTable(CDaoDatabase& Database)
: CDSTable(Database)
{
}

void CPorePressureProfileTable::Open()
{
  if(!RS().IsOpen())
    RS().Open(dbOpenTable, PPP_TABLE);
}

void CPorePressureProfileTable::Create()
{
	CDaoTableDef TableDef(&Database());
	TableDef.Create(PPP_TABLE);
	TableDef.CreateField(PPP_DEPTH,dbDouble,0);
	TableDef.CreateField(PPP_VALUE,dbDouble,0);
	
	TableDef.Append();
	TableDef.Close();
}
