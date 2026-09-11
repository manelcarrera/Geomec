#ifndef _NONMESHEDSURFACEPRESSURE_DELEGATE_H_
#define _NONMESHEDSURFACEPRESSURE_DELEGATE_H_

#include "IValueComposite_Delegate.h"

class CNonMeshedSurfacePressure;

class CNonMeshedSurfacePressure_Delegate : public IValueComposite_Delegate {
public:
  CNonMeshedSurfacePressure_Delegate(CNonMeshedSurfacePressure *node);

  virtual bool Attributes();

private:
  CNonMeshedSurfacePressure_Delegate(const CNonMeshedSurfacePressure_Delegate &rhs);
  CNonMeshedSurfacePressure_Delegate &operator=(const CNonMeshedSurfacePressure_Delegate &rhs);

  CNonMeshedSurfacePressure *m_node;

  REGISTER_DELEGATE(CNonMeshedSurfacePressure, CNonMeshedSurfacePressure_Delegate);
};

#endif // _NONMESHEDSURFACEPRESSURE_DELEGATE_H_
