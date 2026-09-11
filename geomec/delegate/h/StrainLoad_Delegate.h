#ifndef _StrainLoad_Delegate_h_
#define _StrainLoad_Delegate_h_

#include "StrainLoad.h"
#include "ValueTypeFactory.h"

#include "FormationLoad_Delegate.h"

typedef CFormationLoad_Delegate<TStrain, CStrainLoadGradientComponent, CStrainLoadRepeaterComponent,
                                CStrainLoadGWCDummyComponent>
    TStrainLoadBase_Delegate;

class CStrainLoad_Delegate : public TStrainLoadBase_Delegate {
public:
  CStrainLoad_Delegate(CStrainLoad *strainLoad);

  virtual bool Attributes();

private:
  CStrainLoad_Delegate(const CStrainLoad_Delegate &rhs);
  CStrainLoad_Delegate &operator=(const CStrainLoad_Delegate &rhs);

  CStrainLoad *m_StrainLoad;

  REGISTER_DELEGATE(CStrainLoad, CStrainLoad_Delegate);
};

#endif // _VolumetricStrainLoad_Delegate_h_
