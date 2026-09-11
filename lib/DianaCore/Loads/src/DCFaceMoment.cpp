// DCFaceMoment.cpp: implementation of the CFaceMoment class.
//
//////////////////////////////////////////////////////////////////////
#include "IDCLoad.h"
#include "IDCDirectedLoad.h"
#include "DCFaceMoment.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

namespace dia {

CFaceMoment::CFaceMoment(CLoadCase &loadcase, double size, const geo::IVector &vecDirection, const geo::IFace &face)
:	IDirectedLoad(loadcase, size, vecDirection), m_Face(face)
{
}

CFaceMoment::~CFaceMoment()
{
}

const geo::IFace &CFaceMoment::Face() const
{
	return m_Face;
}

} // namespace dia
