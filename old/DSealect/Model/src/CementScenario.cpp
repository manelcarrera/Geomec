// CementScenarios.cpp: implementation of the CCementScenarios class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "CementScenario.h"

#ifdef _DEBUG
#ifdef _MSC_VER#undef THIS_FILE
static char THIS_FILE[]=__FILE__;#endif  // _MSC_VER
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

//##ModelId=3C2043F20053
CCementScenario::CCementScenario()
{

}

//##ModelId=3C2043F2005E
CCementScenario::~CCementScenario()
{

}

//##ModelId=3C2043F20041
CString CCementScenario::TailMaterialName()
{
	return m_TailMaterialName;
}

//##ModelId=3C2043F20042
void CCementScenario::TailMaterialName(CString newname)
{
	m_TailMaterialName=newname;
}

//##ModelId=3C2043F20050
CString CCementScenario::HeadMaterialName()
{
	return m_HeadMaterialName;
}

//##ModelId=3C2043F20051
void CCementScenario::HeadMaterialName(CString newname)
{
	m_HeadMaterialName=newname;
}

