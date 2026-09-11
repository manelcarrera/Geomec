#include "stdafx.h"

#include "MaterialFractureAperture_Delegate.h"
#include "MaterialFractureAperture.h"

CMaterialFractureAperture_Delegate::CMaterialFractureAperture_Delegate(
  CMaterialFractureAperture* materialFractureAperture)
: CMaterialFractureApertureBase_Delegate(materialFractureAperture)
, m_materialFractureAperture(materialFractureAperture)
{
}
