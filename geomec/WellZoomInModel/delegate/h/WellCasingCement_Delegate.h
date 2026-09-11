#ifndef _WellCasingCement_Delegate_h_
#define _WellCasingCement_Delegate_h_

class CWellCasingCement;

#include "FormationBase_Delegate.h"

class CWellCasingCement_Delegate : public CFormationBase_Delegate
{
public:
  CWellCasingCement_Delegate(CWellCasingCement* wellCasingCement);

  virtual bool Attributes();

private:
  CWellCasingCement_Delegate(const CWellCasingCement_Delegate& rhs);
  CWellCasingCement_Delegate& operator = (
    const CWellCasingCement_Delegate& rhs);

  CWellCasingCement* m_wellCasingCement;

  REGISTER_DELEGATE(CWellCasingCement, CWellCasingCement_Delegate);
};

#endif  // _WellCasingCement_Delegate_h_
