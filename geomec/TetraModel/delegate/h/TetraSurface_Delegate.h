#ifndef _TetraSurface_Delegate_h_
#define _TetraSurface_Delegate_h_

class CTetraSurface;

#include "geoSurface_Delegate.h"

class CTetraSurface_Delegate : public CSurfaceBase_Delegate
{
public:
  typedef CSingleCommandTemplate<CTetraSurface_Delegate> TTetraSurfaceDelegateCommand;

  CTetraSurface_Delegate(CTetraSurface* tetraSurface);

  virtual void AppendContextMenu(CContextMenuInvoker& invoker);

private:
  CTetraSurface_Delegate(const CTetraSurface_Delegate& rhs);
  CTetraSurface_Delegate& operator = (const CTetraSurface_Delegate& rhs);

  CTetraSurface* m_tetraSurface;

  void CreateSideSurface();
  void CreateSuperHorizon();
  void CreateSubHorizon();
  void CreateSubFault();

  REGISTER_DELEGATE(CTetraSurface, CTetraSurface_Delegate);
};

#endif  // _TetraSurface_Delegate_h_
