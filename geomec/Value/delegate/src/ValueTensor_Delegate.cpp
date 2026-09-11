#include "stdafx.h"

#include "ValueTensor_Delegate.h"

TStressTensor_Delegate::TStressTensor_Delegate(TStressTensor *stressTensor)
    : CValueTensorTemp_Delegate<IDT_VALUETYPE_STRESS_TENSOR, IDS_ET_STRESS, IDI_VALUETYPE_STRESS_TENSOR,
                                IDS_VALUENAME_STRESS_TENSOR>(stressTensor),
      m_stressTensor(stressTensor) {}

TStrainTensor_Delegate::TStrainTensor_Delegate(TStrainTensor *strainTensor)
    : CValueTensorTemp_Delegate<IDT_VALUETYPE_STRAIN_TENSOR, IDS_ET_STRAIN, IDI_VALUETYPE_STRAIN_TENSOR,
                                IDS_VALUENAME_STRAIN_TENSOR>(strainTensor),
      m_strainTensor(strainTensor) {}
