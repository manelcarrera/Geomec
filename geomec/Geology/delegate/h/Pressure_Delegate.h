#ifndef _Pressure_Delegate_h_
#define _Pressure_Delegate_h_

class TPressure;
class CComponentRepeater;
class CComponentGWC;
class CPressure;

#include "ComponentConstant.h"
#include "FormationLoad_Delegate.h"

typedef CFormationLoad_Delegate<TPressure, CComponentConstant<TPressure>, CComponentRepeater, CComponentGWC>
    TPressureBase_Delegate;

class CPressure_Delegate : public TPressureBase_Delegate {
public:
  CPressure_Delegate(CPressure *pressure);

  virtual bool Attributes();

private:
  CPressure_Delegate(const CPressure_Delegate &rhs);
  CPressure_Delegate &operator=(const CPressure_Delegate &rhs);

  CPressure *m_pressure;

  REGISTER_DELEGATE(CPressure, CPressure_Delegate);
};

#endif // _Pressure_Delegate_h_
