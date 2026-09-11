#include "stdafx.h"
#include "DSModelDatabase.h"
#include "TemperatureProfileTable.h"

CTemperatureProfileTable::CTemperatureProfileTable(CDaoDatabase& Database)
: CDSTable(Database)
{
}

void CTemperatureProfileTable::Open()
{
  if(!RS().IsOpen())
    RS().Open(dbOpenTable, TEMPERATURE_PROFILE_TABLE);
}

void CTemperatureProfileTable::Create()
{
	CDaoTableDef TableDef(&Database());
	TableDef.Create(TEMPERATURE_PROFILE_TABLE);
	TableDef.CreateField(TEMPERATURE_PROFILE_DEPTH,dbDouble,0);
	TableDef.CreateField(TEMPERATURE_PROFILE_VALUE,dbDouble,0);
	
	TableDef.Append();
	TableDef.Close();
}
