// DSModelDatabase.cpp: implementation of the CDSModelDatabase class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "FormationTable.h"
#include "VersionTable.h"
#include "DrillingDirectionTable.h"
#include "DrillingOperationTable.h"
#include "ModelOverburdenTable.h"
#include "AnalysisPointTable.h"
#include "BatchTableTable.h"
#include "ControlTable.h"
#include "CementTable.h"
#include "CasingMaterialDatabase.h"
#include "CementMaterialDatabase.h"
#include "FormationMaterialDatabase.h"
#include "DSModelDatabase.h"
#include "PorePressureProfileTable.h"
#include "DensityProfileTable.h"
#include "TemperatureProfileTable.h"
#include "ModelTable.h"

#ifdef _DEBUG
#ifdef _MSC_VER#undef THIS_FILE
static char THIS_FILE[]=__FILE__;#endif  // _MSC_VER
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CDSModelDatabase::CDSModelDatabase()
{
	m_pModelDatabase = std::auto_ptr<CDaoDatabase>(new CDaoDatabase( 0 ));

	// Create Tables in Heap using auto_ptr for exception safety and cleanup
	m_pFormationTable		= std::auto_ptr<CDSTable>(new CFormationTable( *m_pModelDatabase ));
	m_pVersionTable			= std::auto_ptr<CDSTable>(new CVersionTable( *m_pModelDatabase ));
	m_pCementTable          = std::auto_ptr<CDSTable>(new CCementTable( *m_pModelDatabase ));

	CVersionTable* tp=dynamic_cast<CVersionTable*>(m_pVersionTable.get());
	tp->VersionName("DSModelDatabase");

	m_pDrillingDirectionTable	= std::auto_ptr<CDSTable>(new CDrillingDirectionTable( *m_pModelDatabase ));
	m_pDrillingOperationTable	= std::auto_ptr<CDSTable>(new CDrillingOperationTable( *m_pModelDatabase ));
	m_pModelOverburdenTable		= std::auto_ptr<CDSTable>(new CModelOverburdenTable( *m_pModelDatabase ));
	m_pAnalysisPointTable		= std::auto_ptr<CDSTable>(new CAnalysisPointTable( *m_pModelDatabase ));
	m_pControlTable				= std::auto_ptr<CDSTable>(new CControlTable( *m_pModelDatabase ));

	m_pBatchTableTable			= std::auto_ptr<CDSTable>(new CBatchTableTable( *m_pModelDatabase ));
  m_pPorePressureProfileTable = std::auto_ptr<CDSTable>(new CPorePressureProfileTable(*m_pModelDatabase));
  m_pDensityProfileTable = std::auto_ptr<CDSTable>(new CDensityProfileTable(*m_pModelDatabase));
  m_pTemperatureProfileTable = std::auto_ptr<CDSTable>(new CTemperatureProfileTable(*m_pModelDatabase));
  m_pModelTable = std::auto_ptr<CDSTable>(new CModelTable(*m_pModelDatabase));
}

CDSModelDatabase::~CDSModelDatabase()
{
	if(m_pModelDatabase->IsOpen())
		m_pModelDatabase->Close();

}


void CDSModelDatabase::Create( LPCTSTR lpszName,CVersionInfo versioninfo)
{
	//Create database
	m_pModelDatabase->Create(lpszName,dbLangGeneral);

	//Create tables
	m_pFormationTable->Create();
	m_pVersionTable->Create();

	m_pDrillingDirectionTable->Create();
	m_pDrillingOperationTable->Create();
	m_pModelOverburdenTable->Create();
	m_pAnalysisPointTable->Create();
	m_pControlTable->Create();
	m_pCementTable->Create();
	m_pBatchTableTable->Create();
  m_pPorePressureProfileTable->Create();
  m_pDensityProfileTable->Create();
  m_pTemperatureProfileTable->Create();
  m_pModelTable->Create();

	//Store VersionInfo
	m_pVersionTable->Open();
	m_pVersionTable->RS().AddNew();
	m_pVersionTable->SetField(VERSION ,versioninfo.Version());
	m_pVersionTable->SetField(REVISION,versioninfo.Revision());

	CVersionTable* tp=dynamic_cast<CVersionTable*>(m_pVersionTable.get());
	m_pVersionTable->SetField(VERSION_NAME,tp->VersionName());

	m_pVersionTable->RS().Update();
	m_pVersionTable->RS().Close();

	m_VersionInfo=versioninfo;

	Database().Close();

	//create formation material tables:
	CFormationMaterialDatabase fmd;
	fmd.Database().Open(lpszName);
	fmd.CreateTables();
	fmd.Database().Close();

	//create casing material tables:
	CCasingMaterialDatabase cmd;
	cmd.Database().Open(lpszName);
	cmd.CreateTables();
	cmd.Database().Close();

	//create cement material tables:
	CCementMaterialDatabase cemd;
	cemd.Database().Open(lpszName);
	cemd.CreateTables();
	cemd.Database().Close();


	Database().Open(lpszName,TRUE);

}

void CDSModelDatabase::Open( LPCTSTR lpszName, BOOL bExclusive, BOOL bReadOnly, LPCTSTR lpszConnect )
{
	m_pModelDatabase->Open(lpszName,bExclusive,bReadOnly,lpszConnect);

	//Get VersionInfo
	try 
	{
		m_pVersionTable->Open();
		long iVer,iRev;
		m_pVersionTable->GetField( VERSION ,iVer );
		m_pVersionTable->GetField( REVISION,iRev );
		m_VersionInfo=CVersionInfo(iVer,iRev);
		m_pVersionTable->Close();

	} 
	catch( CDaoException *e ) 
	{
		//No Version info in database, set version to most historic value
		if ( e->m_pErrorInfo->m_lErrorCode==E_DAO_VtoNameNotFound )
		{
		    e->Delete();
			m_VersionInfo=CVersionInfo(1,0);
		}
		else 
		{
			throw;
		}
	}

}

void CDSModelDatabase::Close()
{
	m_pModelDatabase->Close();
}

CVersionInfo CDSModelDatabase::VersionInfo()
{
	return m_VersionInfo;
}

void CDSModelDatabase::VersionInfo(CVersionInfo VersionInfo)
{
	m_VersionInfo=VersionInfo;
}

BOOL CDSModelDatabase::TableExists(CString strTableName)
{
	CDaoTableDef td( m_pModelDatabase.get() );
	try
	{
		CDaoTableDefInfo tinfo;
		m_pModelDatabase.get()->GetTableDefInfo(strTableName, tinfo);
		
	}
	catch( CDaoException* e )
	{
		// Do nothing. Used to catch security violations opening tables.
		e->Delete( );
		return FALSE;
	}
		
	return TRUE;
}

BOOL CDSModelDatabase::FieldExistsInTable( CString strTable,  CString FieldName)
{
	CDaoTableDef td( m_pModelDatabase.get() );
	td.Open(strTable);
	try
	{
		CDaoFieldInfo cdaoinfo;
		td.GetFieldInfo(FieldName,cdaoinfo);
	}
	catch(CDaoException* e)
	{
		e->Delete();
		td.Close();
		return FALSE;
	}

	td.Close();
	return TRUE;
}
