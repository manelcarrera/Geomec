#include "stdafx.h"

#include "TetraModel_Delegate.h"
#include "TetraModel.h"

CTetraModel_Delegate::CTetraModel_Delegate(CTetraModel* aTetraModel)
: C3DModel_Delegate(aTetraModel),
  m_TetraModel(aTetraModel)
{
}
