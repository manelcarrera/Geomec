#include "stdafx.h"
#include "DSModelDatabase.h"
#include "ModelTable.h"

CModelTable::CModelTable(CDaoDatabase& Database)
: CDSTable(Database)
{
}

void CModelTable::Open()
{
  if(!RS().IsOpen())
    RS().Open(dbOpenTable, MODEL_TABLE);
}

void CModelTable::Create()
{
	CDaoTableDef TableDef(&Database());
	TableDef.Create(MODEL_TABLE);

	CDaoFieldInfo fieldinfo;
	fieldinfo.m_nType = dbText;
	fieldinfo.m_lSize = 255;
	fieldinfo.m_lAttributes = 0;
	fieldinfo.m_bAllowZeroLength = TRUE;

	fieldinfo.m_strName = MODEL_PPP_FILENAME;
	TableDef.CreateField(fieldinfo);

	fieldinfo.m_strName = MODEL_DENSITY_PROFILE_FILENAME;
	TableDef.CreateField(fieldinfo);

	fieldinfo.m_strName = MODEL_TEMPERATURE_PROFILE_FILENAME;
	TableDef.CreateField(fieldinfo);

  fieldinfo.m_strName = MODEL_SURVEY_FILENAME;
  TableDef.CreateField(fieldinfo);

	TableDef.Append();
	TableDef.Close();
}
