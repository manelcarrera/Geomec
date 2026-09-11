#ifndef _MatParamsTensor_Delegate_h_
#define _MatParamsTensor_Delegate_h_

//class CMatParamsTensor;
//class CMatTensorComponent;

#include "ValueType_Delegate.h"
#include "ValueComponent_Delegate.h"
#include "MatParamsTensor.h"

class CMatParamsTensor_Delegate : public CValueType_Delegate
{
public:
  class CMatTensorComponent_Delegate : public CValueComponent_Delegate
  {
  public:
  CMatTensorComponent_Delegate(
      CMatParamsTensor::CMatTensorComponent* matTensorComponent);

  private:
  CMatTensorComponent_Delegate(const CMatTensorComponent_Delegate& rhs);
  CMatTensorComponent_Delegate& operator = (
      const CMatTensorComponent_Delegate& rhs);

  CMatParamsTensor::CMatTensorComponent* m_matTensorComponent;

  REGISTER_DELEGATE(CMatParamsTensor::CMatTensorComponent,
      CMatTensorComponent_Delegate);
  };

  CMatParamsTensor_Delegate(CMatParamsTensor* matParamsTensor);

private:
  CMatParamsTensor_Delegate(const CMatParamsTensor_Delegate& rhs);
  CMatParamsTensor_Delegate& operator = (const CMatParamsTensor_Delegate& rhs);

  CMatParamsTensor* m_matParamsTensor;

  REGISTER_DELEGATE(CMatParamsTensor, CMatParamsTensor_Delegate);
};

#endif  // _MatParamsTensor_Delegate_h_
