// RockRefinement.h: interface for the CRockRefinement class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ROCKREFINEMENT_H__C8B07440_3177_4E6B_AA44_918770AC7D9B__INCLUDED_)
#define AFX_ROCKREFINEMENT_H__C8B07440_3177_4E6B_AA44_918770AC7D9B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

//##ModelId=3C8D9DF401D6
class CRockRefinement  
{
private:
	//##ModelId=3C8D9DF40243
	int m_iNrOfTangentialDevisions;
	//##ModelId=3C8D9DF40253
	int m_iNrOfRadialDevisionsInFormation;
	//##ModelId=3C8D9DF40272
	int m_iNrOfRadialDevisionsInCement;
	//##ModelId=3C8D9DF40273
	int m_iNrOfRadialDevisionsInCasing;
	//##ModelId=3C8D9DF40282
	int m_iTotalNrOfElements;
	//##ModelId=3C8D9DF40292
	int m_iRockRefinementLevel;

public:
	//##ModelId=3C8D9DF40293
	CRockRefinement(int iRefinementLevel);
	//##ModelId=3C8D9DF402A2
	virtual ~CRockRefinement();

	//##ModelId=3C8D9DF402B2
	int NrOfTangentialDevisions()        { return m_iNrOfTangentialDevisions;        }
	//##ModelId=3C8D9DF402D0
	int NrOfRadialDevisionsInFormation() { return m_iNrOfRadialDevisionsInFormation; }
	//##ModelId=3C8D9DF402D1
	int NrOfRadialDevisionsInCement()    { return m_iNrOfRadialDevisionsInCement;    }
	//##ModelId=3C8D9DF402D2
	int NrOfRadialDevisionsInCasing()    { return m_iNrOfRadialDevisionsInCasing;    }
	//##ModelId=3C8D9DF402D3
	int TotalNrOfElements()              { return m_iTotalNrOfElements;              }
	//##ModelId=3C8D9DF402E0
	int RockRefinementLevel()            { return m_iRockRefinementLevel;            }
};

#endif // !defined(AFX_ROCKREFINEMENT_H__C8B07440_3177_4E6B_AA44_918770AC7D9B__INCLUDED_)
