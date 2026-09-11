#ifndef _VALUETENSOR_DELEGATE_H_
#define _VALUETENSOR_DELEGATE_H_

#include "ValueType_Delegate.h"
#include "ValueComponent_Delegate.h"
#include "ValueTensor.h"

template<unsigned int uComponentName,			// Resource string ID of the component name
     unsigned int uComponentIndex>			// Index of the component in the value set
class CTensorComponentTemp_Delegate :
  public CComponentTemp_Delegate<uComponentName, uComponentIndex>
{
public:
  CTensorComponentTemp_Delegate(CTensorComponentTemp<uComponentName,
  uComponentIndex>* tensorComponentTemp)
  : CComponentTemp_Delegate<uComponentName,
                              uComponentIndex>(tensorComponentTemp),
      m_tensorComponentTemp(tensorComponentTemp)
  {
  ACTIVATE_TEMPLATE_DELEGATE(CTensorComponentTempTemplate,
      CTensorComponentTemp_DelegateTemplate);
  }

private:
  CTensorComponentTemp_Delegate(const CTensorComponentTemp_Delegate& rhs);
  CTensorComponentTemp_Delegate& operator=(
  const CTensorComponentTemp_Delegate& rhs);

  CTensorComponentTemp<uComponentName, uComponentIndex>* m_tensorComponentTemp;

  typedef CTensorComponentTemp<uComponentName, uComponentIndex>
  CTensorComponentTempTemplate;
  typedef CTensorComponentTemp_Delegate<uComponentName, uComponentIndex>
  CTensorComponentTemp_DelegateTemplate;

  REGISTER_DELEGATE(CTensorComponentTempTemplate,
  CTensorComponentTemp_DelegateTemplate);
};

template<unsigned int uTypeId, 
         unsigned int uExport,
         unsigned int uIconId,
         unsigned int uValueNameId>
class CValueTensorTemp_Delegate :
  public CValueTypeTemp_Delegate<uTypeId, uIconId, uValueNameId, uExport>
{
public:
  CValueTensorTemp_Delegate(CValueTensorTemp<uTypeId, uExport, uIconId,
  uValueNameId>* valueTensorTemp)
  : CValueTypeTemp_Delegate<uTypeId,
                              uIconId,
                              uValueNameId,
                              uExport>(valueTensorTemp),
      m_valueTensorTemp(valueTensorTemp)
  {
  ACTIVATE_TEMPLATE_DELEGATE(CValueTensorTempTemplate,
      CValueTensorTemp_DelegateTemplate);
  }

private:
  CValueTensorTemp_Delegate(const CValueTensorTemp_Delegate& rhs);
  CValueTensorTemp_Delegate& operator=(
  const CValueTensorTemp_Delegate& rhs);

  CValueTensorTemp<uTypeId, uExport, uIconId, uValueNameId>*
  m_valueTensorTemp;

  typedef CValueTensorTemp<uTypeId, uIconId, uValueNameId, uExport>
  CValueTensorTempTemplate;
  typedef CValueTensorTemp_Delegate<uTypeId, uIconId, uValueNameId, uExport>
  CValueTensorTemp_DelegateTemplate;

  REGISTER_DELEGATE(CValueTensorTempTemplate,
  CValueTensorTemp_DelegateTemplate);
};

class TStressTensor_Delegate :
  public CValueTensorTemp_Delegate<IDT_VALUETYPE_STRESS_TENSOR,
                                   IDS_ET_STRESS,
                                   IDI_VALUETYPE_STRESS_TENSOR,
                                   IDS_VALUENAME_STRESS_TENSOR>
{
public:
  TStressTensor_Delegate(TStressTensor* stressTensor);

private:
  TStressTensor_Delegate(TStressTensor_Delegate& rhs);
  TStressTensor_Delegate& operator=(TStressTensor_Delegate& rhs);

  TStressTensor* m_stressTensor;

  REGISTER_DELEGATE(TStressTensor, TStressTensor_Delegate);
};

class TStrainTensor_Delegate :
  public CValueTensorTemp_Delegate<IDT_VALUETYPE_STRAIN_TENSOR,
                                   IDS_ET_STRAIN,
                                   IDI_VALUETYPE_STRAIN_TENSOR,
                                   IDS_VALUENAME_STRAIN_TENSOR>
{
public:
  TStrainTensor_Delegate(TStrainTensor* strainTensor);

private:
  TStrainTensor_Delegate(TStrainTensor_Delegate& rhs);
  TStrainTensor_Delegate& operator=(TStrainTensor_Delegate& rhs);

  TStrainTensor* m_strainTensor;

  REGISTER_DELEGATE(TStrainTensor, TStrainTensor_Delegate);
};

#endif // _VALUETENSOR_DELEGATE_H_
