#ifndef _3DHorizon_Delegate_h_
#define _3DHorizon_Delegate_h_

class C3DHorizon;

#include "HorizonBase_Delegate.h"

class C3DHorizon_Delegate : public CHorizonBase_Delegate
{
public:
  C3DHorizon_Delegate(C3DHorizon* a3dHorizon);

  virtual bool Attributes();

  virtual void AppendContextMenu(CContextMenuInvoker& invoker);

  void ExportConstantDepthSurface();

private:
  C3DHorizon_Delegate(const C3DHorizon_Delegate& rhs);
  C3DHorizon_Delegate& operator = (const C3DHorizon_Delegate& rhs);

  C3DHorizon* m_3dHorizon;

  REGISTER_DELEGATE(C3DHorizon, C3DHorizon_Delegate);
};

#endif  // _3DHorizon_Delegate_h_
