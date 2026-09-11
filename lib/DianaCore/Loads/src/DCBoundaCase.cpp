// DCLoadCase.cpp: implementation of the CLoadCase class.
//
//////////////////////////////////////////////////////////////////////

#include "DCBoundaCase.h"

#include "DCLoadManager.h"
#include "IDCLoad.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

namespace dia {

CBoundaCase::CBoundaCase(CLoadManager &manager, int index)
:	ICase(manager, index)
{
}

std::string CBoundaCase::FilosDirName() const
{
  return "BOUNDA";
}

} // namespace dia
