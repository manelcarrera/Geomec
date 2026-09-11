#ifndef _FractureMatrixPressure_Delegate_h_
#define _FractureMatrixPressure_Delegate_h_

class TFractureMatrixPressure;
class CFractureMatrixComponentRepeater;
class CFractureMatrixComponentGWC;
class CFractureMatrixPressure;

#include "FormationLoad_Delegate.h"
#include "ComponentConstant.h"

typedef CFormationLoad_Delegate <TFractureMatrixPressure,
  CComponentConstant <TFractureMatrixPressure>,
  CFractureMatrixComponentRepeater, CFractureMatrixComponentGWC>
    TFractureMatrixPressureBase_Delegate;

class CFractureMatrixPressure_Delegate :
  public TFractureMatrixPressureBase_Delegate
{
public:
  CFractureMatrixPressure_Delegate(
    CFractureMatrixPressure* fractureMatrixPressure);

  virtual bool Attributes();

private:
  CFractureMatrixPressure_Delegate(const CFractureMatrixPressure_Delegate& rhs);
  CFractureMatrixPressure_Delegate& operator = (
    const CFractureMatrixPressure_Delegate& rhs);

  CFractureMatrixPressure* m_fractureMatrixPressure;

  REGISTER_DELEGATE(CFractureMatrixPressure, CFractureMatrixPressure_Delegate);
};

#endif  // _FractureMatrixPressure_Delegate_h_
