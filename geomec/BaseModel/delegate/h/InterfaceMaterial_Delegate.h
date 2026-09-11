#ifndef _InterfaceMaterial_Delegate_h_
#define _InterfaceMaterial_Delegate_h_

class CInterfaceMaterial;
class CInterfaceMaterialEntry;

#include "GraphEntry_Delegate.h"
#include "IMaterialBase_Delegate.h"

class CInterfaceMaterial_Delegate : public IMaterialBase_Delegate
{
public:
  CInterfaceMaterial_Delegate(CInterfaceMaterial* interfaceMaterial);

  virtual bool Attributes();

private:
  CInterfaceMaterial_Delegate(const CInterfaceMaterial_Delegate& rhs);
  CInterfaceMaterial_Delegate& operator = (
  const CInterfaceMaterial_Delegate& rhs);

  CInterfaceMaterial* m_interfaceMaterial;

  REGISTER_DELEGATE(CInterfaceMaterial, CInterfaceMaterial_Delegate);
};

class CInterfaceMaterialEntry_Delegate :
  public CGraphEntryTemp_Delegate <CInterfaceMaterial>
{
public:
  CInterfaceMaterialEntry_Delegate(
  CInterfaceMaterialEntry* interfaceMaterialEntry);

  virtual bool Attributes();

  virtual void AppendContextMenu(CContextMenuInvoker& invoker);

  void InsertNewWithProperties();

private:
  CInterfaceMaterialEntry_Delegate(const CInterfaceMaterialEntry_Delegate& rhs);
  CInterfaceMaterialEntry_Delegate& operator = (
  const CInterfaceMaterialEntry_Delegate& rhs);

  CInterfaceMaterialEntry* m_interfaceMaterialEntry;

  REGISTER_DELEGATE(CInterfaceMaterialEntry, CInterfaceMaterialEntry_Delegate);
};

#endif  // _InterfaceMaterial_Delegate_h_
