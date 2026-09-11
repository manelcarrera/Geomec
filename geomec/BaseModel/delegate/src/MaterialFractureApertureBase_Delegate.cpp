#include "stdafx.h"

#include "MaterialFractureApertureBase.h"
#include "MaterialFractureApertureBase_Delegate.h"

CMaterialFractureApertureBase_Delegate::CMaterialFractureApertureBase_Delegate(
    CMaterialFractureApertureBase *materialFractureApertureBase)
    : IMaterialRock_Delegate(materialFractureApertureBase),
      m_materialFractureApertureBase(materialFractureApertureBase) {}
