// DCLineMoment.h: interface for the CLineMoment class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ILINEMOMENT_H__A91B3021_C9BD_4689_A371_FA8FB19DBFD0__INCLUDED_)
#define AFX_ILINEMOMENT_H__A91B3021_C9BD_4689_A371_FA8FB19DBFD0__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "ILine.h"

namespace dia {

class CLoadManager;

class CLineMoment : public IDirectedLoad  
{
	const geo::ILine &m_Line;

public:
	CLineMoment(CLoadCase &loadcase, double size, const geo::IVector &vecDirection, const geo::ILine &line);
	virtual ~CLineMoment();

	virtual bool WriteFilos() const;
	const geo::ILine &Line() const;
};

}

#endif // !defined(AFX_ILINEMOMENT_H__A91B3021_C9BD_4689_A371_FA8FB19DBFD0__INCLUDED_)
