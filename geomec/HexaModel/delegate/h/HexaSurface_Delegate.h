#ifndef _HexaSurface_Delegate_h_
#define _HexaSurface_Delegate_h_

class CHexaSurface;

#include "geoSurface_Delegate.h"

class CHexaSurface_Delegate : public CSurfaceBase_Delegate
{
public:
  CHexaSurface_Delegate(CHexaSurface* hexaSurface);

  virtual void AppendContextMenu(CContextMenuInvoker& invoker);

private:
  CHexaSurface_Delegate(const CHexaSurface_Delegate& rhs);
  CHexaSurface_Delegate& operator = (const CHexaSurface_Delegate& rhs);

  CHexaSurface* m_hexaSurface;

  REGISTER_DELEGATE(CHexaSurface, CHexaSurface_Delegate);
};

#endif  // _HexaSurface_Delegate_h_
