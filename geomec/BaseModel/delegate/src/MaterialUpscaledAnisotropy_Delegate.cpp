#include "stdafx.h"

#include "MaterialUpscaledAnisotropy.h"
#include "MaterialUpscaledAnisotropy_Delegate.h"

CMaterialUpscaledAnisotropy_Delegate::CMaterialUpscaledAnisotropy_Delegate(
    CMaterialUpscaledAnisotropy *materialUpscaledAnisotropy)
    : IMaterialRock_Delegate(materialUpscaledAnisotropy), m_materialUpscaledAnisotropy(materialUpscaledAnisotropy) {}
