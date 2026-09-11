#include "stdafx.h"

#include "MaterialFractureAperture2_Delegate.h"
#include "MaterialFractureAperture2.h"

CMaterialFractureAperture2_Delegate::CMaterialFractureAperture2_Delegate(
  CMaterialFractureAperture2* materialFractureAperture2)
: CMaterialFractureApertureBase_Delegate(materialFractureAperture2)
, m_materialFractureAperture2(materialFractureAperture2)
{
}
