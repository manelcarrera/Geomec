#ifndef _DENSITYPROFILETABLE_H_
#define _DENSITYPROFILETABLE_H_

#include "DSTable.h"

#define DENSITY_PROFILE_TABLE "Density Profile"
#define DENSITY_PROFILE_DEPTH "Depth"
#define DENSITY_PROFILE_VALUE "Value"

class CDensityProfileTable : public CDSTable
{
public:
  CDensityProfileTable(CDaoDatabase& Database);
  virtual void Open();
  virtual void Create();
};

#endif // _DENSITYPROFILETABLE_H_
