
#include "IDCStructuralLoad.h"

namespace dia {

IStructuralLoad::IStructuralLoad(CLoadCase &loadcase, double size)
: ILoad(loadcase, size),
  m_LoadCase(loadcase)
{
	loadcase.AddLoad(*this);
}

IStructuralLoad::IStructuralLoad(CLoadCase &loadcase, const std::vector<double>& vcValue)
: ILoad(loadcase, vcValue),
  m_LoadCase(loadcase)
{
	loadcase.AddLoad(*this);
}

const CLoadCase& IStructuralLoad::Case() const
{
  return m_LoadCase;
}

const CLoadCase &IStructuralLoad::LoadCase() const
{
  return Case();
}

}