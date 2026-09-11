#include "stdafx.h"

#include "IStrainTensorGroup.h"
#include "IStrainTensorGroup_Delegate.h"

IStrainTensorGroup_Delegate::CInvariantComposite_Delegate::CInvariantComposite_Delegate(
    IStrainTensorGroup::CInvariantComposite *invariantComposite)
    : IResult_Delegate(invariantComposite), m_invariantComposite(invariantComposite) {}

IStrainTensorGroup_Delegate::CWellPathAxialStrainComposite_Delegate::CWellPathAxialStrainComposite_Delegate(
    IStrainTensorGroup::CWellPathAxialStrainComposite *wellPathAxialStrainComposite)
    : IResult_Delegate(wellPathAxialStrainComposite), m_wellPathAxialStrainComposite(wellPathAxialStrainComposite) {}

IStrainTensorGroup_Delegate::IStrainTensorGroup_Delegate(IStrainTensorGroup *strainTensorGroup)
    : ITensorGroupTemplate_Delegate<CStrainTensor>(strainTensorGroup), m_strainTensorGroup(strainTensorGroup) {}
