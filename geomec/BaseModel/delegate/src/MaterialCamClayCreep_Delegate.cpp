#include "stdafx.h"

#include "MaterialCamClayCreep.h"
#include "MaterialCamClayCreep_Delegate.h"

CMaterialCamClayCreep_Delegate::CMaterialCamClayCreep_Delegate(CMaterialCamClayCreep *materialCamClayCreep)
    : IMaterialRock_Delegate(materialCamClayCreep), m_materialCamClayCreep(materialCamClayCreep) {}
