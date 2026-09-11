#include "stdafx.h"

#include "MatParamsTensor_Delegate.h"

CMatParamsTensor_Delegate::CMatTensorComponent_Delegate::CMatTensorComponent_Delegate(
    CMatParamsTensor::CMatTensorComponent *matTensorComponent)
    : CValueComponent_Delegate(matTensorComponent), m_matTensorComponent(matTensorComponent) {}

CMatParamsTensor_Delegate::CMatParamsTensor_Delegate(CMatParamsTensor *matParamsTensor)
    : CValueType_Delegate(matParamsTensor), m_matParamsTensor(matParamsTensor) {}
