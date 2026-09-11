#include "stdafx.h"

#include "MaterialAnisotropicCamClay_Delegate.h"
#include "MaterialAnisotropicCamClay.h"

CMaterialAnisotropicCamClay_Delegate::CMaterialAnisotropicCamClay_Delegate(
  CMaterialAnisotropicCamClay* materialAnisotropicCamClay)
: IMaterialRock_Delegate(materialAnisotropicCamClay)
, m_materialAnisotropicCamClay(materialAnisotropicCamClay)
{
}
