#include "stdafx.h"
#include "DSModelDatabase.h"
#include "DensityProfileTable.h"

CDensityProfileTable::CDensityProfileTable(CDaoDatabase& Database)
: CDSTable(Database)
{
}

void CDensityProfileTable::Open()
{
  if(!RS().IsOpen())
    RS().Open(dbOpenTable, DENSITY_PROFILE_TABLE);
}

void CDensityProfileTable::Create()
{
	CDaoTableDef TableDef(&Database());
	TableDef.Create(DENSITY_PROFILE_TABLE);
	TableDef.CreateField(DENSITY_PROFILE_DEPTH,dbDouble,0);
	TableDef.CreateField(DENSITY_PROFILE_VALUE,dbDouble,0);
	
	TableDef.Append();
	TableDef.Close();
}
