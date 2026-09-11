#include "stdafx.h"

#include "MaterialAnisotropicCamClay.h"
#include "MaterialAnisotropicCamClay_Delegate.h"

CMaterialAnisotropicCamClay_Delegate::CMaterialAnisotropicCamClay_Delegate(
    CMaterialAnisotropicCamClay *materialAnisotropicCamClay)
    : IMaterialRock_Delegate(materialAnisotropicCamClay), m_materialAnisotropicCamClay(materialAnisotropicCamClay) {}
