#include "stdafx.h"

#include "MaterialFractAnisotropy_Delegate.h"
#include "MaterialFractAnisotropy.h"

CMaterialFractAnisotropy_Delegate::CMaterialFractAnisotropy_Delegate(
  CMaterialFractAnisotropy* materialFractAnisotropy)
: IMaterialRock_Delegate(materialFractAnisotropy)
, m_materialFractAnisotropy(materialFractAnisotropy)
{
}
