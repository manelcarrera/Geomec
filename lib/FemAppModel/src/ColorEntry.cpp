// ColorEntry.cpp: implementation of the CColorEntry class.
//
//////////////////////////////////////////////////////////////////////

#include "FemAppEntryTypes.h"
#include "StorageNode.h"
#include "OpenGLNodeBase.h"
#include "openglnode.h"
#include "colornode.h"
#include "ColorEntry.h"

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

CColorEntry::CColorEntry( CFemAppModel& model)
: CGraphEntryTemp<CColorNode>(MD_BASE_COLOR_NODE, 0, "ColorEntry", model)
{
	InitColor(qRgb(255, 0  , 0  ));		// Red
	InitColor(qRgb(0 , 255, 255));		// Blue 
	InitColor(qRgb(255, 255, 0  ));		// Yellow
	InitColor(qRgb(0 , 255, 0  ));		// Green
	InitColor(qRgb(255, 0  , 255));		// Pink
	InitColor(qRgb(0  , 0  , 255));		// Blue
	InitColor(qRgb(0  , 128, 0  ));		// Dark Green
	InitColor(qRgb(128, 0  , 0  ));		// Dark Red
	InitColor(qRgb(192, 192, 192));		// Light gray
	InitColor(qRgb(255, 128, 0  ));		// Orange
	InitColor(qRgb(0  , 128, 255));		// Mid Blue
	InitColor(qRgb(255, 128, 128));		// Mid Red
}

CColorEntry::~CColorEntry()
{
}

void CColorEntry::InitColor(TColor color)
{
	m_stColor.insert(color);
}

COLORREF CColorEntry::GetColor() const
{
	assert(m_stColor.size() > 0);
	typedef std::map<TColor, int> TColorMap;
	TColorMap mpColor;

	// Populate colormap
	for(TColorSet::const_iterator it = m_stColor.begin(); it != m_stColor.end(); it++)
		mpColor.insert(TColorMap::value_type(*it, 0));

	// Check used colors
	TNodeSet stColorNode = EntryNodes();
	for(TNodeSet::const_iterator itt = stColorNode.begin(); itt != stColorNode.end(); itt++)
	{
		if(mpColor.find((*itt)->Color()) != mpColor.end())
		{
			mpColor[(*itt)->Color()]++;
		}
	}

	// Find minimum used color
	COLORREF color = mpColor.begin()->first;
	int used = mpColor.begin()->second;
	for(TColorMap::iterator mp_it = mpColor.begin(); mp_it != mpColor.end(); mp_it++)
	{
		if(mp_it->second < used)
		{
			used = mp_it->second;
			color = mp_it->first;
		}
	}

	return color;
}