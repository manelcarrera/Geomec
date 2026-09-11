#include "stdafx.h"

#include "MaterialLinear.h"
#include "MaterialLinear_Delegate.h"

CMaterialLinear_Delegate::CMaterialLinear_Delegate(CMaterialLinear *materialLinear)
    : IMaterialRock_Delegate(materialLinear), m_materialLinear(materialLinear) {}
