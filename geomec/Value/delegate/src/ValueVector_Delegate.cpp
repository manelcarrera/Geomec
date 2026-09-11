#include "stdafx.h"

#include "ValueVector_Delegate.h"

TDisplacementVector_Delegate::TDisplacementVector_Delegate(
  TDisplacementVector* displacementVector)
: CValueVectorTemp_Delegate <IDT_VALUETYPE_DISPLACEMENT, IDS_ET_DISPLACEMENT,
    IDI_VALUETYPE_DISPLACEMENT, IDS_VALUENAME_DISP_VECTOR> (displacementVector)
, m_displacementVector(displacementVector)
{
}
