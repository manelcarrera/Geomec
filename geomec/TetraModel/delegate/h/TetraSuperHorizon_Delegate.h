#ifndef _TetraSuperHorizon_Delegate_h_
#define _TetraSuperHorizon_Delegate_h_

class CTetraSuperHorizon;
class CTetraSuperHorizonEntry;

#include "StorageNode_Delegate.h"
#include "TetraHorizonBase_Delegate.h"

class CTetraSuperHorizon_Delegate : public CTetraHorizonBase_Delegate
{
public:
  CTetraSuperHorizon_Delegate(
  CTetraSuperHorizon* tetraSuperHorizon);

  virtual bool Attributes();

private:
  CTetraSuperHorizon_Delegate(const CTetraSuperHorizon_Delegate& rhs);
  CTetraSuperHorizon_Delegate& operator = (
  const CTetraSuperHorizon_Delegate& rhs);

  CTetraSuperHorizon* m_tetraSuperHorizon;

  REGISTER_DELEGATE(CTetraSuperHorizon, CTetraSuperHorizon_Delegate);
};

class CTetraSuperHorizonEntry_Delegate :
  public CStorageNodeEntry_Delegate <CTetraSuperHorizon>
{
public:
  CTetraSuperHorizonEntry_Delegate(
  CTetraSuperHorizonEntry* tetraSuperHorizonEntry);

  virtual void AppendContextMenu(CContextMenuInvoker& invoker);

  bool CanCreateHorizon() const;
  void CreateHorizon();

private:
  CTetraSuperHorizonEntry_Delegate(
  const CTetraSuperHorizonEntry_Delegate& rhs);
  CTetraSuperHorizonEntry_Delegate& operator = (
  const CTetraSuperHorizonEntry_Delegate& rhs);

  CTetraSuperHorizonEntry* m_tetraSuperHorizonEntry;

  REGISTER_DELEGATE(CTetraSuperHorizonEntry, CTetraSuperHorizonEntry_Delegate);
};

#endif  // _TetraSuperHorizon_Delegate_h_
