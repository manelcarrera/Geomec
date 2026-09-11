// DCLoadCase.cpp: implementation of the CLoadCase class.
//
//////////////////////////////////////////////////////////////////////

#include "DCLoadCase.h"

#include "DCLoadManager.h"
#include "IDCLoad.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

namespace dia {

CLoadCase::CLoadCase(CLoadManager &manager, int index) : ICase(manager, index) {}

std::string CLoadCase::FilosDirName() const { return "LOADS"; }

} // namespace dia
