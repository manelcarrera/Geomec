// IProperty.cpp: implementation of the IProperty class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "geomec.h"
#include "IProperty.h"

#ifdef _DEBUG
#ifdef _MSC_VER#undef THIS_FILE
static char THIS_FILE[]=__FILE__;#endif  // _MSC_VER
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

BOOL IProperty::DefineValueType(UINT uHeaderID, UINT uValueTypeID, UINT uMode)
{
	CString sHeaderTag;
	sHeaderTag.LoadString(uHeaderID);

	// Store in value type map
//	VERIFY(m_mpValueType(TValueTypeMap(sHeaderTag, TValueType(uValueTypeID, uMode))));

	// Store in value

//	TValueTypeMap::iterator it = m_mpValueType.find(CString);

	// Create a new one if this header tag is not defined

	return TRUE;
}


BOOL IProperty::DefineComponent(UINT uHeaderID, UINT uValueTypeID, UINT uComponentID, UINT uMode)
{

	return TRUE;
}
IProperty::IProperty(CGraphMode &model)
: m_model(mode), m_pPointSet(0)
{
}

IProperty::~IProperty()
{
}
