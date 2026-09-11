// TetraBody.h: interface for the CTetraBody class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_TETRABODY_H__CD806CD6_2D69_4BDC_8610_B4C4C7091A95__INCLUDED_)
#define AFX_TETRABODY_H__CD806CD6_2D69_4BDC_8610_B4C4C7091A95__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CTetraBody : public CColorNode  
{
public:
	CTetraBody(CFemAppModel& model);
	virtual ~CTetraBody();

};

typedef CGraphEntryTemp<CTetraBody> TTetraBodyEntry;

#endif // !defined(AFX_TETRABODY_H__CD806CD6_2D69_4BDC_8610_B4C4C7091A95__INCLUDED_)
