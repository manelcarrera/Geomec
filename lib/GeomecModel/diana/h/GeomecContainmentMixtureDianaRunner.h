#ifndef _GEOMECCONTAINMENTMIXTUREDIANARUNNER_H_
#define _GEOMECCONTAINMENTMIXTUREDIANARUNNER_H_

#include "GeomecMixtureDianaRunner.h"

class CGeomecContainmentMixtureDianaRunner : public CGeomecMixtureDianaRunner
{
public:
  CGeomecContainmentMixtureDianaRunner(CMeshBase& meshbase,
                                       CModelBase& model,
                                       CDianaRunController& controller);
  virtual ~CGeomecContainmentMixtureDianaRunner();

	virtual const dia::IMaterial& Material(const geo::IElement &element) const;
};

#endif // _GEOMECCONTAINMENTMIXTUREDIANARUNNER_H_
