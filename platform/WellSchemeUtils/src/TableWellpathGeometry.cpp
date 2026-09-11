// TableWellpathGeometry.cpp: implementation of the TableWellpathGeometry class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"

#ifndef _WIN64

#include "TableWellpathGeometry.h"

#ifdef _DEBUG
#ifdef _MSC_VER
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#endif  // _MSC_VER
//#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

TableWellpathGeometry::TableWellpathGeometry(CDaoDatabase& database)
:CDSTable(database)
{

}

TableWellpathGeometry::~TableWellpathGeometry()
{

}

void TableWellpathGeometry::Open()
{
  if(!RS().IsOpen()){
    //Open the table
    RS().Open(dbOpenTable, WELLPATH_GEO_TABLE_NAME);
  }
}

void TableWellpathGeometry::Create()
{
  if(!TableExists())
  {
    CDaoTableDef TableDef(&Database());
    TableDef.Create(WELLPATH_GEO_TABLE_NAME);
    TableDef.CreateField(WELLPATH_GEO_TMD,dbDouble,0);
    TableDef.CreateField(WELLPATH_GEO_AZI,dbDouble,0);
    TableDef.CreateField(WELLPATH_GEO_INC,dbDouble,0);
    TableDef.Append();
    TableDef.Close();
  }
}

/*virtual*/ CString TableWellpathGeometry::TableName() const
{
  return WELLPATH_GEO_TABLE_NAME; 
}

///////////////////////////////////////////////////////////////////


TableWellpathGlobal::TableWellpathGlobal(CDaoDatabase& database)
:CDSTable(database)
{

}

TableWellpathGlobal::~TableWellpathGlobal()
{

}

void TableWellpathGlobal::Open()
{
  if(!RS().IsOpen()){
    //Open the table
    RS().Open(dbOpenTable, WELLPATH_GLB_TABLE_NAME);
  }
}

void TableWellpathGlobal::Create()
{
  if(!TableExists())
  {
    CDaoTableDef TableDef(&Database());

    


    TableDef.Create(WELLPATH_GLB_TABLE_NAME);

    TableDef.CreateField(WELLPATH_GLB_NORTHING,dbDouble,0);
    TableDef.CreateField(WELLPATH_GLB_EASTING,dbDouble,0);
    TableDef.CreateField(WELLPATH_GLB_DEPTH,dbDouble,0);
    TableDef.CreateField(WELLPATH_GLB_START_NORTHING,dbDouble,0);
    TableDef.CreateField(WELLPATH_GLB_START_EASTING,dbDouble,0);
    TableDef.CreateField(WELLPATH_GLB_START_DEPTH,dbDouble,0);
    TableDef.CreateField(WELLPATH_GLB_SEALEVEL,dbDouble,0);
    TableDef.CreateField(WELLPATH_GLB_GROUND_OR_SEABED_LEVEL,dbDouble,0);

    CDaoFieldInfo fieldinfo;
    fieldinfo.m_strName = WELLPATH_STR_DESCRIPTION;
    fieldinfo.m_nType = dbMemo;
    fieldinfo.m_lSize = 255;
    fieldinfo.m_lAttributes = 0;
    fieldinfo.m_bAllowZeroLength = TRUE;
    TableDef.CreateField(fieldinfo);
    fieldinfo.m_strName = WELLPATH_STR_NAME;
    fieldinfo.m_nType = dbText;
    TableDef.CreateField(fieldinfo);
    fieldinfo.m_strName = WELLPATH_STR_TYPE_NAME;
    TableDef.CreateField(fieldinfo);
    fieldinfo.m_strName = WELLPATH_STR_FIELD_NAME;
    TableDef.CreateField(fieldinfo);
    fieldinfo.m_strName = WELLPATH_STR_LOC_NAME;
    TableDef.CreateField(fieldinfo);
    fieldinfo.m_strName = WELLPATH_STR_COUNTRY;
    TableDef.CreateField(fieldinfo);
    fieldinfo.m_strName = WELLPATH_STR_CUST_NAME;
    TableDef.CreateField(fieldinfo);
    fieldinfo.m_strName = WELLPATH_STR_CUST_CONT;
    TableDef.CreateField(fieldinfo);
    fieldinfo.m_strName = WELLPATH_STR_JOB_NO;
    TableDef.CreateField(fieldinfo);
    
    TableDef.CreateField(WELLPATH_SPUDDATE_DAY,dbLong,0);
    TableDef.CreateField(WELLPATH_SPUDDATE_MONTH,dbLong,0);
    TableDef.CreateField(WELLPATH_SPUDDATE_YEAR,dbLong,0);
    TableDef.Append();
    TableDef.Close();
  }
}


/*virtual*/ CString TableWellpathGlobal::TableName() const
{
  return WELLPATH_GLB_TABLE_NAME; 
}

#endif // _WIN64
