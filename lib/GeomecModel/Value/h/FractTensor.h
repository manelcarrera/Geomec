#if !defined(_VALUEFRACTTENSOR_H_)
#define _VALUEFRACTTENSOR_H_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "MatParamsTensor.h"

class CFractTensor : public CMatParamsTensor
{
public:
  CFractTensor(IPointSet &point_set, const QString &strName);
  virtual void AddComponents();

  ACCEPT_GEOMECMODELVISITORS(VisitFractTensor);
};

#endif // !defined(_VALUEFRACTTENSOR_H_)
