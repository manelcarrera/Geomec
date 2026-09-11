#include "stdafx.h"

#include "MaterialCamClay.h"
#include "MaterialCamClay_Delegate.h"

CMaterialCamClay_Delegate::CMaterialCamClay_Delegate(CMaterialCamClay *materialCamClay)
    : IMaterialRock_Delegate(materialCamClay), m_materialCamClay(materialCamClay) {}
