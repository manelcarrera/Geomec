#ifndef _MODELTABLE_H_
#define _MODELTABLE_H_

#include "DSTable.h"

#define MODEL_TABLE "Model"
#define MODEL_PPP_FILENAME "Pore Pressure Profile Filename"
#define MODEL_DENSITY_PROFILE_FILENAME "Density Profile Filename"
#define MODEL_TEMPERATURE_PROFILE_FILENAME "Temperature Profile Filename"
#define MODEL_SURVEY_FILENAME "Survey Filename"

class CModelTable : public CDSTable
{
public:
  CModelTable(CDaoDatabase& Database);
  virtual void Open();
  virtual void Create();
};

#endif // _MODELTABLE_H_
