#include "stdafx.h"

#include "MaterialUpscaledAnisotropy_Delegate.h"
#include "MaterialUpscaledAnisotropy.h"

CMaterialUpscaledAnisotropy_Delegate::CMaterialUpscaledAnisotropy_Delegate(
  CMaterialUpscaledAnisotropy* materialUpscaledAnisotropy)
: IMaterialRock_Delegate(materialUpscaledAnisotropy)
, m_materialUpscaledAnisotropy(materialUpscaledAnisotropy)
{
}
