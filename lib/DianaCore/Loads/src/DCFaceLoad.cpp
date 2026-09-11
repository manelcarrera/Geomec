// DCFaceLoad.cpp: implementation of the CFaceLoad class.
//
//////////////////////////////////////////////////////////////////////
#include "DCFaceLoad.h"
#include "IDCDirectedLoad.h"
#include "IDCLoad.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

namespace dia {

CFaceLoad::CFaceLoad(CLoadCase &loadcase, double size, const geo::IVector &vecDirection, const geo::IFace &face)
    : IDirectedLoad(loadcase, size, vecDirection), m_Face(face) {}

CFaceLoad::~CFaceLoad() {}

const geo::IFace &CFaceLoad::Face() const { return m_Face; }

} // namespace dia
