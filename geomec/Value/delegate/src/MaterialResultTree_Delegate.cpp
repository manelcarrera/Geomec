#include "stdafx.h"

#include "MaterialResultTree.h"
#include "MaterialResultTree_Delegate.h"

CMaterialResult_Delegate::CMaterialResult_Delegate(CMaterialResult *materialResult)
    : IResult_Delegate(materialResult), m_materialResult(materialResult) {}

CMaterialResultTree_Delegate::CMaterialResultTree_Delegate(CMaterialResultTree *materialResultTree)
    : CResultGroup_Delegate(materialResultTree), m_materialResultTree(materialResultTree) {}
