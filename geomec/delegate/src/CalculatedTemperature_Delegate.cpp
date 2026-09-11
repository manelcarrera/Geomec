#include "stdafx.h"

#include "CalculatedTemperature.h"
#include "CalculatedTemperature_Delegate.h"

CCalculatedTemperature_Delegate::CCalculatedTemperature_Delegate(CCalculatedTemperature *calculatedTemperature)
    : IValueComposite_Delegate(calculatedTemperature), m_calculatedTemperature(calculatedTemperature) {}
