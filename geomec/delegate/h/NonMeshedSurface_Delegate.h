#ifndef _NONMESHEDSURFACE_DELEGATE_H_
#define _NONMESHEDSURFACE_DELEGATE_H_

#include "ColorNode_Delegate.h"

class CNonMeshedSurface;
class CNonMeshedSurfaceEntry;

class CNonMeshedSurface_Delegate : public CColorNode_Delegate {
public:
  CNonMeshedSurface_Delegate(CNonMeshedSurface *nonMeshedSurface);
  virtual bool Attributes();

private:
  CNonMeshedSurface_Delegate(const CNonMeshedSurface_Delegate &rhs);
  CNonMeshedSurface_Delegate &operator=(const CNonMeshedSurface_Delegate &rhs);

  CNonMeshedSurface *m_nonMeshedSurface;

  REGISTER_DELEGATE(CNonMeshedSurface, CNonMeshedSurface_Delegate);
};

class CNonMeshedSurfaceEntry_Delegate : public CStorageNodeEntry_Delegate<CNonMeshedSurface> {
public:
  CNonMeshedSurfaceEntry_Delegate(CNonMeshedSurfaceEntry *entry);

private:
  CNonMeshedSurfaceEntry_Delegate(const CNonMeshedSurfaceEntry_Delegate &rhs);
  CNonMeshedSurfaceEntry_Delegate &operator=(const CNonMeshedSurfaceEntry_Delegate &rhs);

  CNonMeshedSurfaceEntry *m_entry;

  REGISTER_DELEGATE(CNonMeshedSurfaceEntry, CNonMeshedSurfaceEntry_Delegate);
};

#endif // _NONMESHEDSURFACE_DELEGATE_H_
