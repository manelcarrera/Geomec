#include "stdafx.h"

#include "MaterialUndrained_Delegate.h"
#include "MaterialUndrained.h"

CMaterialUndrained_Delegate::CMaterialUndrained_Delegate(
  CMaterialUndrained* materialUndrained)
: IMaterialRock_Delegate(materialUndrained)
, m_materialUndrained(materialUndrained)
{
}
