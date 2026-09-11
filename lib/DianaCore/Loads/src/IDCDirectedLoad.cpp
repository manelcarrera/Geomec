// IDirectedLoad.cpp: implementation of the IDirectedLoad class.
//
//////////////////////////////////////////////////////////////////////
#include "IDCLoad.h"
#include "IDCDirectedLoad.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

namespace dia {

IDirectedLoad::IDirectedLoad(CLoadCase &loadcase, double size, const geo::IVector &vecDirection)
:	IStructuralLoad(loadcase, size), m_vecDirection(vecDirection)
{
}

IDirectedLoad::IDirectedLoad(CLoadCase &loadcase, const std::vector<double>& vcValue, const geo::IVector &vecDirection)
: IStructuralLoad(loadcase, vcValue), m_vecDirection(vecDirection)
{
}

IDirectedLoad::~IDirectedLoad()
{
}

const geo::IVector &IDirectedLoad::Direction() const
{
  return m_vecDirection;
}

} // namespace dia
