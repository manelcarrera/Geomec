// DCFaceMoment.cpp: implementation of the CFaceMoment class.
//
//////////////////////////////////////////////////////////////////////
#include "DCFaceMoment.h"
#include "IDCDirectedLoad.h"
#include "IDCLoad.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

namespace dia {

CFaceMoment::CFaceMoment(CLoadCase &loadcase, double size, const geo::IVector &vecDirection, const geo::IFace &face)
    : IDirectedLoad(loadcase, size, vecDirection), m_Face(face) {}

CFaceMoment::~CFaceMoment() {}

const geo::IFace &CFaceMoment::Face() const { return m_Face; }

} // namespace dia
