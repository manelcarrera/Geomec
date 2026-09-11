// ResultKey.cpp: implementation of the CResultKey class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#ifdef _MSC_VER
#pragma warning ( disable : 4786 )
#endif  // _MSC_VER
#include "geomec.h"
#include "ResultKey.h"

#ifdef _DEBUG
#ifdef _MSC_VER#undef THIS_FILE
static char THIS_FILE[]=__FILE__;#endif  // _MSC_VER
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

result_key::result_key()
: m_pStage(0), m_bLinear(TRUE)
{
}
		
result_key::result_key(const CDepletionStage& stage, BOOL bLinear)
: m_pStage(&stage), m_bLinear(bLinear)
{
}
		
result_key::result_key(const result_key& rhs)
: m_pStage(rhs.m_pStage), m_bLinear(rhs.m_bLinear)
{
}

bool result_key::operator<(const result_key& rhs) const
{
	if(m_pStage < rhs.m_pStage)
		return true;
	if(m_pStage == rhs.m_pStage)
	{
		if(!m_bLinear && rhs.m_bLinear)
			return true;
	}

	return false;
}