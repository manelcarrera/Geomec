#include "stdafx.h"

#include "MaterialLinear_Delegate.h"
#include "MaterialLinear.h"

CMaterialLinear_Delegate::CMaterialLinear_Delegate(
  CMaterialLinear* materialLinear)
: IMaterialRock_Delegate(materialLinear)
, m_materialLinear(materialLinear)
{
}
