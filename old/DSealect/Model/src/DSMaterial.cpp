// DSMaterial.cpp: implementation of the CDSMaterial class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "dsealect.h"
#include "DSMaterial.h"

#ifdef _DEBUG
#ifdef _MSC_VER#undef THIS_FILE
static char THIS_FILE[]=__FILE__;#endif  // _MSC_VER
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

//##ModelId=3C60D563036D
CDSMaterial::CDSMaterial()
{

}

//##ModelId=3C60D563036E
CDSMaterial::~CDSMaterial()
{

}

//##ModelId=3C60D5630370
void CDSMaterial::Name(CString name)
{
	m_Name=name;
	m_Name.TrimLeft();
	m_Name.TrimRight();
}

//##ModelId=3C60D5630373
CString CDSMaterial::Name() const
{
	return m_Name;
}


//##ModelId=3C60D563035E
CString CDSMaterial::GetHardeningName(eHardeningType ht)
{
	CString ret;
	ret.LoadString(int(ht)+HARDENING_IDS_OFFSET);
	return ret;
}
