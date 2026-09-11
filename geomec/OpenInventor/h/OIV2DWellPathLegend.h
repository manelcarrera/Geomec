#ifndef _OIV2DWellPathLegend_h_
#define _OIV2DWellPathLegend_h_

#include "OIV2DLegend.h"

class OIV2DWellPathLegend : public OIV2DLegend
{
public:
  OIV2DWellPathLegend();
  void setupFormationSections(const CNewWellPath& wellPath);
};

#endif  // _OIV2DWellPathLegend_h_
