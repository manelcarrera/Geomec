#ifndef _GLOBAL_TEMPERATURE_H_
#define _GLOBAL_TEMPERATURE_H_

#include "GlobalFormationLoad.h"
#include "Temperature.h"

class TTemperature;

class CGlobalTemperature : public CGlobalFormationLoad<TTemperature, CTemperatureGradientComponent>
{
public:
  CGlobalTemperature(CFemAppModel& model);
  CGlobalTemperature(const CGlobalTemperature& rhs);

  virtual unsigned int IconId() const;

  virtual void LoadStream(TSTREAM& stream, CStreamVersion& version, TPROGRESS& progress);

  virtual void OnNewNeighbour(const CGraphNode& node);
  virtual void OnNeighbourDeleted(const CGraphNode& node);

  ACCEPT_GEOMECMODELVISITORS(VisitGlobalTemperature);
};

#endif // _GLOBAL_TEMPERATURE_H_
