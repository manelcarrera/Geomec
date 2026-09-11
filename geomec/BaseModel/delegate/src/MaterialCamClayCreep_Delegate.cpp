#include "stdafx.h"

#include "MaterialCamClayCreep_Delegate.h"
#include "MaterialCamClayCreep.h"

CMaterialCamClayCreep_Delegate::CMaterialCamClayCreep_Delegate(
  CMaterialCamClayCreep* materialCamClayCreep)
: IMaterialRock_Delegate(materialCamClayCreep)
, m_materialCamClayCreep(materialCamClayCreep)
{
}
