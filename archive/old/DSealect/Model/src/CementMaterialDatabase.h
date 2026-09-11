// CementMaterialDatabase.h: interface for the CCementMaterialDatabase class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CEMENTMATERIALDATABASE_H__85AE4208_F3F3_4B75_B9AF_5856E71E9F76__INCLUDED_)
#define AFX_CEMENTMATERIALDATABASE_H__85AE4208_F3F3_4B75_B9AF_5856E71E9F76__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "CementMaterial.h"
#include "CementMaterialTable.h"
#include "MaterialDataBaseTemplate.h"



class CCementMaterialDatabase  : public CMaterialDatabaseTemplate<CCementMaterialTable>  
{
public:
	CCementMaterialDatabase();
	virtual ~CCementMaterialDatabase();

	virtual BOOL AddMaterial(CDSMaterial &mat);
	virtual BOOL GetMaterial(CString name,CDSMaterial &mat);

};

#endif // !defined(AFX_CEMENTMATERIALDATABASE_H__85AE4208_F3F3_4B75_B9AF_5856E71E9F76__INCLUDED_)
