// VersionTable.cpp: implementation of the CVersionTable class.
//
//////////////////////////////////////////////////////////////////////
#include "stdafx.h"

#ifndef _WIN64

#include "StreamVersion.h"
#include "VersionTable.h"

#ifdef _DEBUG
#ifdef _MSC_VER
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#endif  // _MSC_VER
//#define new DEBUG_NEW
#endif


#define VERSION_TABLE					"VersionInfo"
#define VERSION_NAME					"VersionName"
#define VERSION							"Version"
#define REVISION						"Revision"
#define MINORREVISION					"MinorRevision"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CVersionTable::CVersionTable(CDaoDatabase& Database)
: CDSTable(Database)
{

}

CVersionTable::CVersionTable(CDaoDatabase& Database,const CString& version_name)
: CDSTable(Database),m_VersionName(version_name)
{
  // Construct
}


CVersionTable::~CVersionTable()
{

}

void CVersionTable::SaveVersionInfo(CStreamVersion& VersionInfo)
{
  try
  {
    Create();
    Open();
    RS().AddNew();
    SetField(VERSION_NAME,m_VersionName);
    SetField(VERSION,VersionInfo.majorNr());
    SetField(REVISION,VersionInfo.minorNr());
    SetField(MINORREVISION,VersionInfo.revisionNr());
    RS().Update();
    RS().Close();
    Close();
  }
  catch(...)
  {

  }
}

CStreamVersion CVersionTable::LoadVersionInfo()
{
  try
  {
    if(!TableExists())
      return CStreamVersion(0,0,0);

    Open();
    long version,revision,minorrevision;
    
    GetField(VERSION,version);
    GetField(REVISION,revision);

    if(FieldExists(MINORREVISION))
      GetField(MINORREVISION,minorrevision);
    else
      minorrevision=0;
    Close();

    return CStreamVersion(version,revision,minorrevision);
  }
  catch(...)
  {
    return CStreamVersion(0,0,0);
  }

}

void CVersionTable::Open()
{
  if(RS().IsOpen())
    return;

  assert(!m_VersionName.IsEmpty());

  CString sqlStatement;
  sqlStatement = "SELECT * FROM ";
  sqlStatement += TableName();
  sqlStatement += " WHERE ";
  sqlStatement += VERSION_NAME;
  sqlStatement += " = '";
  sqlStatement += m_VersionName;
  sqlStatement += "'";

  CDaoQueryDef qd(&Database());
  qd.Create();
  qd.SetSQL(sqlStatement);

  RS().Open(&qd,dbOpenDynaset,dbDenyWrite);
}

void CVersionTable::Create()
{

  BOOL exist = TableExists();
  
  CDaoTableDef TableDef(&Database());
  try
  {
    if(!exist)
      TableDef.Create(VERSION_TABLE);

    if(!FieldExists(VERSION_NAME))
      TableDef.CreateField(VERSION_NAME,dbText,255);

    if(!FieldExists(VERSION))
      TableDef.CreateField(VERSION,dbInteger,0);

    if(!FieldExists(REVISION))
      TableDef.CreateField(REVISION,dbInteger,0);

    if(!FieldExists(MINORREVISION))
      TableDef.CreateField(MINORREVISION,dbInteger,0);

    if(!exist)
      TableDef.Append();	

    TableDef.Close();
  }
  catch(...)
  {

  }
}


void  CVersionTable::VersionName(const CString& name)
{
  m_VersionName=name;
}

CString CVersionTable::VersionName() const
{
  return m_VersionName;
}

CString CVersionTable::TableName() const
{
  return VERSION_TABLE;
}

#endif // _WIN64
