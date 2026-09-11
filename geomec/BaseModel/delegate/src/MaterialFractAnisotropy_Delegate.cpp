#include "stdafx.h"

#include "MaterialFractAnisotropy.h"
#include "MaterialFractAnisotropy_Delegate.h"

CMaterialFractAnisotropy_Delegate::CMaterialFractAnisotropy_Delegate(CMaterialFractAnisotropy *materialFractAnisotropy)
    : IMaterialRock_Delegate(materialFractAnisotropy), m_materialFractAnisotropy(materialFractAnisotropy) {}
