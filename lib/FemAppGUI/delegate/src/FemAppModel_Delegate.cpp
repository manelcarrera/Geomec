#include "stdafx.h"

#include "FemAppModel_Delegate.h"
#include "FemAppModel.h"

CFemAppModel_Delegate::CFemAppModel_Delegate(CFemAppModel* femAppModel)
: CGraphNode_Delegate(femAppModel)
, m_femAppModel(femAppModel)
{
}
