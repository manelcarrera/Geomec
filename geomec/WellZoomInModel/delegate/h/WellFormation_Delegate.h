#ifndef _WellFormation_Delegate_h_
#define _WellFormation_Delegate_h_

class CWellFormation;
class CWellFormationEntry;

#include "3DFormation_Delegate.h"
#include "FormationBase_Delegate.h"

class CWellFormation_Delegate : public C3DFormation_Delegate {
public:
  CWellFormation_Delegate(CWellFormation *wellFormation);

  virtual bool Attributes();

private:
  CWellFormation_Delegate(const CWellFormation_Delegate &rhs);
  CWellFormation_Delegate &operator=(const CWellFormation_Delegate &rhs);

  CWellFormation *m_wellFormation;

  REGISTER_DELEGATE(CWellFormation, CWellFormation_Delegate);
};

class CWellFormationEntry_Delegate : public CFormationEntryTempl_Delegate<CWellFormation> {
public:
  CWellFormationEntry_Delegate(CWellFormationEntry *wellFormationEntry);

  virtual void AppendContextMenu(CContextMenuInvoker &invoker);

private:
  CWellFormationEntry_Delegate(const CWellFormationEntry_Delegate &rhs);
  CWellFormationEntry_Delegate &operator=(const CWellFormationEntry_Delegate &rhs);

  CWellFormationEntry *m_wellFormationEntry;

  REGISTER_DELEGATE(CWellFormationEntry, CWellFormationEntry_Delegate);
};

#endif // _WellFormation_Delegate_h_
