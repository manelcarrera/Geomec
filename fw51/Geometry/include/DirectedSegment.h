 /* Copyright (c) 2011 TNO DIANA BV                              Confidential */
// directedsegment.h: interface for the CDirectedSegment class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_DIRECTEDSEGMENT_H__97F209B4_5ED3_427F_B602_D3AE5F94F32B__INCLUDED_)
#define AFX_DIRECTEDSEGMENT_H__97F209B4_5ED3_427F_B602_D3AE5F94F32B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <vector>

namespace geo {
class CRingFactory;
class CDirectedSegment  
{
	std::pair<int, int> m_segment;
	const CRingFactory& m_factory;
	std::vector<CDirectedSegment> AdjacentSegments(int nVertex) const;
public:
	typedef std::pair<int, int> TSegment;
	CDirectedSegment(const CRingFactory& factory, int nFirst, int nSecond);
	CDirectedSegment(const CRingFactory& factory, int nFirst, const TSegment& segment);
	CDirectedSegment(const CDirectedSegment& rhs);

	bool operator==(const CDirectedSegment &rhs) const;
	CDirectedSegment& operator=(const CDirectedSegment &rhs);
	
	int First() const;
	int Second() const;

	std::vector<CDirectedSegment> NextSegments() const;
	std::vector<CDirectedSegment> PrevSegments() const;

	double Angle(const CDirectedSegment &rhs) const;
	bool operator<(const CDirectedSegment& rhs) const;
};

}

#endif // !defined(AFX_DIRECTEDSEGMENT_H__97F209B4_5ED3_427F_B602_D3AE5F94F32B__INCLUDED_)
