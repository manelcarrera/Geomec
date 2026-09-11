#ifndef _FormationVolume_Delegate_h_
#define _FormationVolume_Delegate_h_

class CFormationVolume;

#include "FormationBase_Delegate.h"

class CFormationVolume_Delegate : public IFormationElementSet_Delegate
{
public:
  CFormationVolume_Delegate(CFormationVolume* formationVolume);

  virtual bool Attributes();

private:
  CFormationVolume_Delegate(const CFormationVolume_Delegate& rhs);
  CFormationVolume_Delegate& operator = (const CFormationVolume_Delegate& rhs);

  CFormationVolume* m_formationVolume;

  REGISTER_DELEGATE(CFormationVolume, CFormationVolume_Delegate);
};

#endif  // _FormationVolume_Delegate_h_
