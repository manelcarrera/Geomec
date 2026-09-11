// FormationMaterialDatabase.h: interface for the CFormationMaterialDatabase class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_FORMATIONMATERIALDATABASE_H__61CB61AD_E834_412D_9F35_F88AF63D0BB9__INCLUDED_)
#define AFX_FORMATIONMATERIALDATABASE_H__61CB61AD_E834_412D_9F35_F88AF63D0BB9__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "FormationMaterial.h"
#include "FormationMaterialTable.h"
#include "MaterialDataBaseTemplate.h"

//##ModelId=3C60D57602DF
class CFormationMaterialDatabase : public CMaterialDatabaseTemplate<CFormationMaterialTable>  
{
public:
	//##ModelId=3C60D576035C
	CFormationMaterialDatabase();
	//##ModelId=3C60D576035D
	virtual ~CFormationMaterialDatabase();
	//##ModelId=3C60D576035F
	virtual BOOL AddMaterial(CDSMaterial &mat);
	//##ModelId=3C60D5760362
	virtual BOOL GetMaterial(CString name,CDSMaterial &mat);

};

#endif // !defined(AFX_FORMATIONMATERIALDATABASE_H__61CB61AD_E834_412D_9F35_F88AF63D0BB9__INCLUDED_)
