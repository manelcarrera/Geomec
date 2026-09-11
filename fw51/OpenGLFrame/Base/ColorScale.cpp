 /* Copyright (c) 2011 TNO DIANA BV                              Confidential */
#include "dimple.h"
#include <qcolor.h>
#include <cassert>
#include "ColorScale.h"


#ifdef _DEBUG
#ifdef _MSC_VER
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#endif  // _MSC_VER
//#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CColorScale::CColorScale()
{

}

CColorScale::~CColorScale()
{

}

CColorScale::CColorScale(const CColorScale& right)
{
	*this = right;
}



CColorScale& CColorScale::operator=(const CColorScale& right)
{
	m_ColorPairSet.clear();
	TColorPairSet::const_iterator it = right.m_ColorPairSet.begin(); 

	while(it != right.m_ColorPairSet.end())
	{
		m_ColorPairSet.insert(*it);
		it++;
	}

	return *this;
}


void CColorScale::Insert(const TColorPair &p)
{
	m_ColorPairSet.insert(p);
}

void CColorScale::Clear()
{
	m_ColorPairSet.clear();
}



CColorScale::TColor CColorScale::GetColor(const double &value) const
{
	assert(m_ColorPairSet.size() > 0);

	if(m_ColorPairSet.size() == 1)
		return m_ColorPairSet.begin()->second;

	DIA_ASSERT(m_ColorPairSet.size() > 1);

	if(value <= m_ColorPairSet.begin()->first)
		return m_ColorPairSet.begin()->second;

	if(value >= m_ColorPairSet.rbegin()->first)
		return m_ColorPairSet.rbegin()->second;

	TColorPairSet::const_iterator it1 = m_ColorPairSet.begin();
	TColorPairSet::const_iterator it2 = it1;
	
	do {
		it2++;
		if(value >= it1->first && value < it2->first) {
			double fac  = (value - it1->first)/(it2->first - it1->first);
			int iRed1   = qRed(it1->second);
			int iGreen1 = qGreen(it1->second);
			int iBlue1  = qBlue(it1->second);

			int iRed2   = qRed(it2->second);
			int iGreen2 = qGreen(it2->second);
			int iBlue2  = qBlue(it2->second);

			int iRed    = iRed1   + int( fac*(iRed2 - iRed1) );
			int iGreen  = iGreen1 + int( fac*(iGreen2 - iGreen1) );
			int iBlue   = iBlue1  + int( fac*(iBlue2 - iBlue1) );
			
			return qRgb(iRed,iGreen,iBlue);
		}
		it1++;
	} while(1);
	
}
