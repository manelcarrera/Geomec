#include "stdafx.h"

#include "MaterialFractureApertureBase_Delegate.h"
#include "MaterialFractureApertureBase.h"

CMaterialFractureApertureBase_Delegate::CMaterialFractureApertureBase_Delegate(
  CMaterialFractureApertureBase* materialFractureApertureBase)
: IMaterialRock_Delegate(materialFractureApertureBase)
, m_materialFractureApertureBase(materialFractureApertureBase)
{
}
