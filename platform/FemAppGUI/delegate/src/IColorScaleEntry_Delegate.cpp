#include "stdafx.h"

#include "IColorScaleEntry.h"
#include "IColorScaleEntry_Delegate.h"
#include "IColorScaleNode.h"

IColorScaleEntry_Delegate::IColorScaleEntry_Delegate(IColorScaleEntry *colorScaleEntry)
    : CGraphEntryTemp_Delegate<IColorScaleNode>(colorScaleEntry), m_colorScaleEntry(colorScaleEntry) {}
