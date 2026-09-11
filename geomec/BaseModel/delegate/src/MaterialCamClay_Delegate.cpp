#include "stdafx.h"

#include "MaterialCamClay_Delegate.h"
#include "MaterialCamClay.h"

CMaterialCamClay_Delegate::CMaterialCamClay_Delegate(
  CMaterialCamClay* materialCamClay)
: IMaterialRock_Delegate(materialCamClay)
, m_materialCamClay(materialCamClay)
{
}
