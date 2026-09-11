#ifndef _WellCasingSteelMaterial_Delegate_h_
#define _WellCasingSteelMaterial_Delegate_h_

class CWellCasingSteelMaterial;
class CWellCasingSteelMaterialEntry;

#include "GraphEntry_Delegate.h"
#include "IMaterialBase_Delegate.h"

class CWellCasingSteelMaterial_Delegate : public IMaterialBase_Delegate
{
public:
  CWellCasingSteelMaterial_Delegate(
  CWellCasingSteelMaterial* wellCasingSteelMaterial);

  virtual bool Attributes();

private:
  CWellCasingSteelMaterial_Delegate(
  const CWellCasingSteelMaterial_Delegate& rhs);
  CWellCasingSteelMaterial_Delegate& operator = (
  const CWellCasingSteelMaterial_Delegate& rhs);

  CWellCasingSteelMaterial* m_wellCasingSteelMaterial;

  REGISTER_DELEGATE(CWellCasingSteelMaterial,
  CWellCasingSteelMaterial_Delegate);
};

class CWellCasingSteelMaterialEntry_Delegate :
  public CGraphEntryTemp_Delegate <CWellCasingSteelMaterial>
{
public:
  CWellCasingSteelMaterialEntry_Delegate(
  CWellCasingSteelMaterialEntry* wellCasingSteelMaterialEntry);

  virtual bool Attributes();

  virtual void AppendContextMenu(CContextMenuInvoker& invoker);

  void InsertNewAndProperties();

private:
  CWellCasingSteelMaterialEntry_Delegate(
  const CWellCasingSteelMaterialEntry_Delegate& rhs);
  CWellCasingSteelMaterialEntry_Delegate& operator = (
  const CWellCasingSteelMaterialEntry_Delegate& rhs);

  CWellCasingSteelMaterialEntry* m_wellCasingSteelMaterialEntry;

  REGISTER_DELEGATE(CWellCasingSteelMaterialEntry,
  CWellCasingSteelMaterialEntry_Delegate);
};

#endif  // _WellCasingSteelMaterial_Delegate_h_
