#ifndef _WellCasingModel_Delegate_h_
#define _WellCasingModel_Delegate_h_

class CWellCasingModel;
class CWellCasingFormationEntry;

#include "IWellModel_Delegate.h"
#include "WellFormation_Delegate.h"

class CWellCasingModel_Delegate : public IWellModel_Delegate
{
public:
  CWellCasingModel_Delegate(CWellCasingModel* wellCasingModel);

  virtual void AppendContextMenu(CContextMenuInvoker& invoker);

private:
  CWellCasingModel_Delegate(const CWellCasingModel_Delegate& rhs);
  CWellCasingModel_Delegate& operator = (const CWellCasingModel_Delegate& rhs);

  CWellCasingModel* m_wellCasingModel;

  REGISTER_DELEGATE(CWellCasingModel, CWellCasingModel_Delegate);
};

class CWellCasingFormationEntry_Delegate : public CWellFormationEntry_Delegate
{
public:
  CWellCasingFormationEntry_Delegate(
  CWellCasingFormationEntry* wellCasingFormationEntry);

private:
  CWellCasingFormationEntry_Delegate(
  const CWellCasingFormationEntry_Delegate& rhs);
  CWellCasingFormationEntry_Delegate& operator = (
  const CWellCasingFormationEntry_Delegate& rhs);

  CWellCasingFormationEntry* m_wellCasingFormationEntry;

  REGISTER_DELEGATE(CWellCasingFormationEntry,
  CWellCasingFormationEntry_Delegate);
};

#endif  // _WellCasingModel_Delegate_h_
