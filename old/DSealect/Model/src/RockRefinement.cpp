// RockRefinement.cpp: implementation of the CRockRefinement class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "RockRefinement.h"

#ifdef _DEBUG
#ifdef _MSC_VER#undef THIS_FILE
static char THIS_FILE[]=__FILE__;#endif  // _MSC_VER
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

//##ModelId=3C8D9DF40293
CRockRefinement::CRockRefinement(int iRefinementLevel)
{
	m_iRockRefinementLevel=iRefinementLevel;

	switch( m_iRockRefinementLevel )
	{
		case 1:
			m_iNrOfTangentialDevisions = 24;
			m_iNrOfRadialDevisionsInFormation = 9;
			m_iNrOfRadialDevisionsInCement = 3;
			m_iNrOfRadialDevisionsInCasing = 2;
			m_iTotalNrOfElements = 336;
			break;

		case 2:
			m_iNrOfTangentialDevisions = 40;
			m_iNrOfRadialDevisionsInFormation = 13;
			m_iNrOfRadialDevisionsInCement = 5;
			m_iNrOfRadialDevisionsInCasing = 3;
			m_iTotalNrOfElements = 840;
			break;

		case 3:
			m_iNrOfTangentialDevisions = 52;
			m_iNrOfRadialDevisionsInFormation = 17;
			m_iNrOfRadialDevisionsInCement = 6;
			m_iNrOfRadialDevisionsInCasing = 4;
			m_iTotalNrOfElements = 1404;
			break;

		default:
			ASSERT(FALSE);
	}
}

//##ModelId=3C8D9DF402A2
CRockRefinement::~CRockRefinement()
{

}
