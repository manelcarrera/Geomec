// CPrescribedRotation.cpp: implementation of the CPrescribedRotation class.
//
//////////////////////////////////////////////////////////////////////

#include "IDCLoad.h"
#include "IDCDirectedLoad.h"
#include "IDCSupportLoad.h"
#include "DCPrescribedRotation.h"

#include "ISupport.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

namespace dia {

CPrescribedRotation::CPrescribedRotation(CLoadCase &loadcase, double size, const geo::IVector &vecDirection, const geo::CRotationSupport &support)
:	ISupportLoad(loadcase, size, vecDirection, support)
{
}

CPrescribedRotation::~CPrescribedRotation()
{
}

bool CPrescribedRotation::WriteFilos() const
{
  return ISupportLoad::WriteFilos("RO");
}

} // namespace dia
