#ifndef _TetraSubHorizon_Delegate_h_
#define _TetraSubHorizon_Delegate_h_

class CTetraSubHorizon;
class CTetraSubHorizonEntry;

#include "TetraHorizonBase_Delegate.h"

class CTetraSubHorizon_Delegate : public CTetraHorizonBase_Delegate
{
public:
  CTetraSubHorizon_Delegate(CTetraSubHorizon* tetraSubHorizon);

  virtual bool Attributes();

  virtual void AppendContextMenu(CContextMenuInvoker& invoker);

private:
  CTetraSubHorizon_Delegate(const CTetraSubHorizon_Delegate& rhs);
  CTetraSubHorizon_Delegate& operator = (const CTetraSubHorizon_Delegate& rhs);

  CTetraSubHorizon* m_tetraSubHorizon;

  REGISTER_DELEGATE(CTetraSubHorizon, CTetraSubHorizon_Delegate);
};

class CTetraSubHorizonEntry_Delegate :
  public CStorageNodeEntry_Delegate <CTetraSubHorizon>
{
public:
  CTetraSubHorizonEntry_Delegate(CTetraSubHorizonEntry* tetraSubHorizonEntry);

private:
  CTetraSubHorizonEntry_Delegate(const CTetraSubHorizonEntry_Delegate& rhs);
  CTetraSubHorizonEntry_Delegate& operator = (
  const CTetraSubHorizonEntry_Delegate& rhs);

  CTetraSubHorizonEntry* m_tetraSubHorizonEntry;

  REGISTER_DELEGATE(CTetraSubHorizonEntry, CTetraSubHorizonEntry_Delegate);
};

#endif  // _TetraSubHorizon_Delegate_h_
