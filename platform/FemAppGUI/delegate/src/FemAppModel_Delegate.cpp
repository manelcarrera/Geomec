#include "stdafx.h"

#include "FemAppModel.h"
#include "FemAppModel_Delegate.h"

CFemAppModel_Delegate::CFemAppModel_Delegate(CFemAppModel *femAppModel)
    : CGraphNode_Delegate(femAppModel), m_femAppModel(femAppModel) {}
