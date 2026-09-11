// CementTable.h: interface for the CCementTable class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CEMENTTABLE_H__C34568C1_1087_4412_8E8A_465D6F761F29__INCLUDED_)
#define AFX_CEMENTTABLE_H__C34568C1_1087_4412_8E8A_465D6F761F29__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "DSTable.h"

#define CEMENT_TABLE					"CementScenarios"
#define CEMENT_HEAD_MATERIAL_NAME		"HeadMaterialName"
#define CEMENT_TAIL_MATERIAL_NAME		"TailMaterialName"

class CCementTable :public CDSTable
{
public:
	CCementTable(CDaoDatabase& Database);
	virtual ~CCementTable();
	virtual void Open();
	virtual void Create();
	virtual CString TableName() {return CEMENT_TABLE;}
};

#endif // !defined(AFX_CEMENTTABLE_H__C34568C1_1087_4412_8E8A_465D6F761F29__INCLUDED_)
