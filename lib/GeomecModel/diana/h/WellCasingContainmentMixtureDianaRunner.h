#ifndef _WELLCASINGCONTAINMENTMIXTUREDIANARUNNER_H_
#define _WELLCASINGCONTAINMENTMIXTUREDIANARUNNER_H_

#include "WellCasingMixtureDianaRunner.h"

class CWellCasingMixtureContainmentDianaRunner : public CWellCasingMixtureDianaRunner
{
public:
  CWellCasingMixtureContainmentDianaRunner(CWellCasingModel& model, CDianaRunController& controller);
  virtual ~CWellCasingMixtureContainmentDianaRunner();

  const dia::IMaterial& Material(const geo::IElement& element) const;
};

#endif // _WELLCASINGCONTAINMENTMIXTUREDIANARUNNER_H_
