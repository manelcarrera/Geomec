// IColorScaleEntry.cpp: implementation of the IColorScaleEntry class.
//
//////////////////////////////////////////////////////////////////////
#include "IColorScaleEntry.h"
#include "FemAppEntryTypes.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

IColorScaleEntry::IColorScaleEntry(CFemAppModel &model)
    : CGraphEntryTemp<IColorScaleNode>(MD_BASE_COLOR_SCALE, 0, 0, model) {}

IColorScaleEntry::~IColorScaleEntry() {}
