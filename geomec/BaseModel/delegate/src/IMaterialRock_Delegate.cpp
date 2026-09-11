#include "stdafx.h"

#include "IMaterialRock_Delegate.h"
#include "IMaterialRock.h"

IMaterialRock_Delegate::IMaterialRock_Delegate(IMaterialRock* materialRock)
: IMaterial_Delegate(materialRock)
, m_materialRock(materialRock)
{
}
