#ifndef _WellCasingRTCI_Delegate_h_
#define _WellCasingRTCI_Delegate_h_

class CWellCasingRTCIEntry;
class CWellCasingRTCI;

#include "StorageNode_Delegate.h"
#include "ColorNode_Delegate.h"

class CWellCasingRTCI_Delegate : public CColorNode_Delegate
{
public:
  CWellCasingRTCI_Delegate(CWellCasingRTCI* wellCasingRTCI);

  virtual bool Attributes();

private:
  CWellCasingRTCI_Delegate(const CWellCasingRTCI_Delegate& rhs);
  CWellCasingRTCI_Delegate& operator = (const CWellCasingRTCI_Delegate& rhs);

  CWellCasingRTCI* m_wellCasingRTCI;

  REGISTER_DELEGATE(CWellCasingRTCI, CWellCasingRTCI_Delegate);
};

class CWellCasingRTCIEntry_Delegate :
  public CStorageNodeEntry_Delegate <CWellCasingRTCI>
{
public:
  CWellCasingRTCIEntry_Delegate(CWellCasingRTCIEntry* wellCasingRTCIEntry);

  virtual void AppendContextMenu(CContextMenuInvoker& invoker);

private:
  CWellCasingRTCIEntry_Delegate(const CWellCasingRTCIEntry_Delegate& rhs);
  CWellCasingRTCIEntry_Delegate& operator = (
  const CWellCasingRTCIEntry_Delegate& rhs);

  CWellCasingRTCIEntry* m_wellCasingRTCIEntry;

  REGISTER_DELEGATE(CWellCasingRTCIEntry, CWellCasingRTCIEntry_Delegate);
};

#endif  // _WellCasingRTCI_Delegate_h_
