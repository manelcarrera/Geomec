// MaterialDatabaseTemplate.h: interface for the CMaterialDatabaseTemplate class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MATERIALDATABASETEMPLATE_H__DE25BF0E_DBF4_47FB_B9E2_C90C847F3648__INCLUDED_)
#define AFX_MATERIALDATABASETEMPLATE_H__DE25BF0E_DBF4_47FB_B9E2_C90C847F3648__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <afxdao.h>
#include <memory>
#include "DSTable.h"
#include "VersionInfo.h"
#include "VersionTable.h"
#include "DSMaterial.h"

class CFormationMaterialTable;
class CCementMaterialTable;
class CCasingMaterialTable;

#define MATERIAL_NAME "MaterialName" 

//##ModelId=3C60D57301A6
template<class T>
class CMaterialDatabaseTemplate  
{
protected:
	//##ModelId=3CAAA6650081
	CVersionInfo m_VersionInfo;
	//##ModelId=3CAAA66500A0
	std::auto_ptr<CDaoDatabase>	m_pModelDatabase;
	//##ModelId=3CAAA66500DF
	std::auto_ptr<CDSTable> m_pMaterialTable;	
	//##ModelId=3CAAA665011D
	std::auto_ptr<CDSTable> m_pVersionTable;

public:
	//##ModelId=3C60D5730291
	CMaterialDatabaseTemplate();
	//##ModelId=3C60D5730292
	virtual ~CMaterialDatabaseTemplate();
	//##ModelId=3C60D5730294
	CDaoDatabase& Database() const { return (CDaoDatabase&)*m_pModelDatabase;}
	//##ModelId=3C60D5730296
	CDSTable& MaterialTable() const { return (CDSTable&)*m_pMaterialTable; }
	//##ModelId=3C60D57302A0
	CVersionInfo VersionInfo();
	//##ModelId=3C60D57302A1
	void VersionInfo(CVersionInfo VersionInfo);
	//##ModelId=3C60D57302B0
	void Create( LPCTSTR lpszName,CVersionInfo versioninfo);
	//##ModelId=3C60D57302CF
	void CreateTables();
	//##ModelId=3C60D57302D0
	void Open( LPCTSTR lpszName, BOOL bExclusive, BOOL bReadOnly, LPCTSTR lpszConnect );
	//##ModelId=3C60D57302FE
	BOOL MaterialExists(CString name) const;
	//##ModelId=3C60D573030E
	BOOL RemoveMaterial(CString name);
	//##ModelId=3C8D9E150169
	BOOL HasMaterials() const;


	//##ModelId=3C60D573031D
	virtual BOOL AddMaterial(CDSMaterial &mat)  {ASSERT(FALSE); return FALSE;};
	//##ModelId=3C60D573033D
	virtual BOOL GetMaterial(CString name,CDSMaterial &mat) {ASSERT(FALSE); return FALSE;};
};


//##ModelId=3C60D5730291
template<class T>
CMaterialDatabaseTemplate<T>::CMaterialDatabaseTemplate()
{
	m_pModelDatabase = std::auto_ptr<CDaoDatabase>(new CDaoDatabase( 0 ));
	// Create Tables in Heap using auto_ptr for exception safety and cleanup
	m_pMaterialTable = std::auto_ptr<CDSTable>(new T( *m_pModelDatabase.get() ));
	m_pVersionTable	= std::auto_ptr<CDSTable>(new CVersionTable( *m_pModelDatabase.get() ));
}

//##ModelId=3C60D5730292
template<class T>
CMaterialDatabaseTemplate<T>::~CMaterialDatabaseTemplate()
{

}

//##ModelId=3C60D573030E
template<class T>
BOOL CMaterialDatabaseTemplate<T>::RemoveMaterial(CString name)
{
	CString sqlStatement;

	sqlStatement = "SELECT * FROM ";
	sqlStatement += MaterialTable().TableName();
	sqlStatement += " WHERE ";
	sqlStatement += MATERIAL_NAME;
	sqlStatement += " = '";
	sqlStatement += name;
	sqlStatement += "'";

	CDaoQueryDef qd(&Database());
	qd.Create(NULL, sqlStatement);
	
	MaterialTable().Close();
	MaterialTable().RS().Open(&qd,dbOpenDynaset,dbDenyWrite);

	if(MaterialTable().RS().GetRecordCount() >0)
	{
		qd.Close();
		MaterialTable().RS().MoveFirst();
		MaterialTable().RS().Delete();
		MaterialTable().RS().Close();
		MaterialTable().Close();
		return TRUE; 
	}

	qd.Close();
	MaterialTable().RS().Close();
	MaterialTable().Close();
	return FALSE; 
}

//##ModelId=3C8D9E150169
template<class T>
BOOL CMaterialDatabaseTemplate<T>::HasMaterials() const
{
	MaterialTable().Open();
	return MaterialTable().RS().GetRecordCount();
}

//##ModelId=3C60D57302FE
template<class T>
BOOL CMaterialDatabaseTemplate<T>::MaterialExists(CString name) const
{
	CString sqlStatement;

	sqlStatement = "SELECT * FROM ";
	sqlStatement += MaterialTable().TableName();
	sqlStatement += " WHERE ";
	sqlStatement += MATERIAL_NAME;
	sqlStatement += " = '";
	sqlStatement += name;
	sqlStatement += "'";

	CDaoQueryDef qd(&Database());
	qd.Create(NULL, sqlStatement);
	
	MaterialTable().Close();
	MaterialTable().RS().Open(&qd,dbOpenDynaset,dbReadOnly);

	if(MaterialTable().RS().GetRecordCount() >0)
	{
		qd.Close();
		MaterialTable().RS().Close();
		MaterialTable().Close();
		return TRUE; 
	}

	qd.Close();
	MaterialTable().RS().Close();
	MaterialTable().Close();
	return FALSE; 
}

//##ModelId=3C60D57302A0
template<class T>
CVersionInfo CMaterialDatabaseTemplate<T>::VersionInfo()
{
	return m_VersionInfo;
}

//##ModelId=3C60D57302A1
template<class T>
void CMaterialDatabaseTemplate<T>::VersionInfo(CVersionInfo VersionInfo)
{
	m_VersionInfo=VersionInfo;
}

//##ModelId=3C60D57302CF
template<class T>
void CMaterialDatabaseTemplate<T>::CreateTables()
{
	
	//Create tables
	m_pMaterialTable->Create();
	try
	{
		m_pVersionTable->Create();
	}
	catch(...)
	{

	}

	//Store VersionInfo
	m_pVersionTable->Open();
	m_pVersionTable->RS().AddNew();
	m_pVersionTable->SetField(VERSION ,m_VersionInfo.Version());
	m_pVersionTable->SetField(REVISION,m_VersionInfo.Revision());
	CVersionTable* tp=dynamic_cast<CVersionTable*>(m_pVersionTable.get());
	m_pVersionTable->SetField(VERSION_NAME,tp->VersionName());
	m_pVersionTable->RS().Update();
	m_pVersionTable->RS().Close();
	
}

//##ModelId=3C60D57302B0
template<class T>
void CMaterialDatabaseTemplate<T>::Create( LPCTSTR lpszName,CVersionInfo versioninfo)
{
	//Create database
	m_pModelDatabase->Create(lpszName,dbLangGeneral);
	m_VersionInfo=versioninfo;
	CreateTables();
}


//##ModelId=3C60D57302D0
template<class T>
void CMaterialDatabaseTemplate<T>::Open( LPCTSTR lpszName, BOOL bExclusive, BOOL bReadOnly, LPCTSTR lpszConnect )
{
	m_pModelDatabase->Open(lpszName,bExclusive,bReadOnly,lpszConnect);

	//Get VersionInfo
	try 
	{
		m_pVersionTable->Open();
		int iVer,iRev;
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



#endif // !defined(AFX_MATERIALDATABASETEMPLATE_H__DE25BF0E_DBF4_47FB_B9E2_C90C847F3648__INCLUDED_)
