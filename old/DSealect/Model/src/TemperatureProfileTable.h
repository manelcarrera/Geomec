#ifndef _TEMPERATUREPROFILETABLE_H_
#define _TEMPERATUREPROFILETABLE_H_

#include "DSTable.h"

#define TEMPERATURE_PROFILE_TABLE "Temperature Profile"
#define TEMPERATURE_PROFILE_DEPTH "Depth"
#define TEMPERATURE_PROFILE_VALUE "Value"

class CTemperatureProfileTable : public CDSTable
{
public:
  CTemperatureProfileTable(CDaoDatabase& Database);
  virtual void Open();
  virtual void Create();
};

#endif // _TEMPERATUREPROFILETABLE_H_
