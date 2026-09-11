#ifndef _TetraFormation_Delegate_h_
#define _TetraFormation_Delegate_h_

class CTetraFormationEntry;
class CTetraFormation;
class CTetraFormationVolume;

#include "FormationBase_Delegate.h"
#include "3DFormation_Delegate.h"
#include "FormationVolume_Delegate.h"

class CTetraFormationVolume_Delegate : public CFormationVolume_Delegate
{
public:
  CTetraFormationVolume_Delegate(CTetraFormationVolume* tetraFormationVolume);

  virtual bool Attributes();

  virtual bool CanDestroy() const;

private:
  CTetraFormationVolume_Delegate(const CTetraFormationVolume_Delegate& rhs);
  CTetraFormationVolume_Delegate& operator = (
    const CTetraFormationVolume_Delegate& rhs);

  CTetraFormationVolume* m_tetraFormationVolume;

  REGISTER_DELEGATE(CTetraFormationVolume, CTetraFormationVolume_Delegate);
};

class CTetraFormation_Delegate : public C3DFormation_Delegate
{
public:
  CTetraFormation_Delegate(CTetraFormation* tetraFormation);

  virtual void AppendContextMenu(CContextMenuInvoker& invoker);

  virtual bool Attributes();

private:
  CTetraFormation_Delegate(const CTetraFormation_Delegate& rhs);
  CTetraFormation_Delegate& operator = (const CTetraFormation_Delegate& rhs);

  CTetraFormation* m_tetraFormation;

  REGISTER_DELEGATE(CTetraFormation, CTetraFormation_Delegate);
};

class CTetraFormationEntry_Delegate :
  public CFormationEntryTempl_Delegate <CTetraFormation>
{
public:
  CTetraFormationEntry_Delegate(CTetraFormationEntry* tetraFormationEntry);

  virtual void AppendContextMenu(CContextMenuInvoker& invoker);

  void CreateFormation();
  bool CanCreateFormation() const;

private:
  CTetraFormationEntry_Delegate(const CTetraFormationEntry_Delegate& rhs);
  CTetraFormationEntry_Delegate& operator = (
    const CTetraFormationEntry_Delegate& rhs);

  CTetraFormationEntry* m_tetraFormationEntry;

  REGISTER_DELEGATE(CTetraFormationEntry, CTetraFormationEntry_Delegate);
};

#endif  // _TetraFormation_Delegate_h_
