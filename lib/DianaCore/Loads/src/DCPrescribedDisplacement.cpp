// CPrescribedDisplacement.cpp: implementation of the CPrescribedDisplacement class.
//
//////////////////////////////////////////////////////////////////////

#include "DCPrescribedDisplacement.h"
#include "IDCDirectedLoad.h"
#include "IDCLoad.h"
#include "IDCSupportLoad.h"

#include "ISupport.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

namespace dia {

CPrescribedDisplacement::CPrescribedDisplacement(CLoadCase &loadcase, double size, const geo::IVector &vecDirection,
                                                 const geo::CTranslationSupport &support)
    : ISupportLoad(loadcase, size, vecDirection, support) {}

CPrescribedDisplacement::~CPrescribedDisplacement() {}

bool CPrescribedDisplacement::WriteFilos() const { return ISupportLoad::WriteFilos("TR"); }

} // namespace dia
