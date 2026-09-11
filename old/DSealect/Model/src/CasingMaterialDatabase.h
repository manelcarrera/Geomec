// CasingMaterialDatabase.h: interface for the CCasingMaterialDatabase class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CASINGMATERIALDATABASE_H__FCC91A58_DB24_4C1E_9780_9EEB10682BAF__INCLUDED_)
#define AFX_CASINGMATERIALDATABASE_H__FCC91A58_DB24_4C1E_9780_9EEB10682BAF__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "CasingMaterial.h"
#include "CasingMaterialTable.h"
#include "MaterialDataBaseTemplate.h"


class CCasingMaterialDatabase  : public CMaterialDatabaseTemplate<CCasingMaterialTable>  
{
public:
	CCasingMaterialDatabase();
	virtual ~CCasingMaterialDatabase();

	virtual BOOL AddMaterial(CDSMaterial &mat);
	virtual BOOL GetMaterial(CString name,CDSMaterial &mat);
};

#endif // !defined(AFX_CASINGMATERIALDATABASE_H__FCC91A58_DB24_4C1E_9780_9EEB10682BAF__INCLUDED_)
