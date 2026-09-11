#ifndef _HexaFormation_Delegate_h_
#define _HexaFormation_Delegate_h_

class CHexaFormation;
class CHexaFormationEntry;

#include "3DFormation_Delegate.h"

class CHexaFormation_Delegate : public C3DFormation_Delegate
{
public:
  CHexaFormation_Delegate(CHexaFormation* hexaFormation);

  virtual bool Attributes();

private:
  CHexaFormation_Delegate(const CHexaFormation_Delegate& rhs);
  CHexaFormation_Delegate& operator = (const CHexaFormation_Delegate& rhs);

  CHexaFormation* m_hexaFormation;

  REGISTER_DELEGATE(CHexaFormation, CHexaFormation_Delegate);
};

class CHexaFormationEntry_Delegate :
  public CFormationEntryTempl_Delegate <CHexaFormation>
{
public:
  CHexaFormationEntry_Delegate(CHexaFormationEntry* hexaFormationEntry);

  virtual void AppendContextMenu(CContextMenuInvoker& invoker);

private:
  CHexaFormationEntry_Delegate(const CHexaFormationEntry_Delegate& rhs);
  CHexaFormationEntry_Delegate& operator = (
  const CHexaFormationEntry_Delegate& rhs);

  CHexaFormationEntry* m_hexaFormationEntry;

  REGISTER_DELEGATE(CHexaFormationEntry, CHexaFormationEntry_Delegate);
};

#endif  // _HexaFormation_Delegate_h_
