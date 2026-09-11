#include "stdafx.h"

#include "FractTensor.h"
#include "FractTensor_Delegate.h"

CFractTensor_Delegate::CFractTensor_Delegate(CFractTensor *fractTensor)
    : CMatParamsTensor_Delegate(fractTensor), m_fractTensor(fractTensor) {}
