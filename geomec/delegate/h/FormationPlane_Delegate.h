#ifndef _FormationPlane_Delegate_h_
#define _FormationPlane_Delegate_h_

class CFormationPlane;

#include "ColorNode_Delegate.h"

class CFormationPlane_Delegate : public CColorNode_Delegate
{
public:
  CFormationPlane_Delegate(CFormationPlane* formationPlane);

  virtual bool Attributes();

  virtual bool CanDestroy() const;

private:
  CFormationPlane_Delegate(const CFormationPlane_Delegate& rhs);
  CFormationPlane_Delegate& operator = (const CFormationPlane_Delegate& rhs);

  CFormationPlane* m_formationPlane;

  REGISTER_DELEGATE(CFormationPlane, CFormationPlane_Delegate);
};

#endif  // _FormationPlane_Delegate_h_
