#include "stdafx.h"

#include "IStressTensorGroup_Delegate.h"
#include "IStressTensorGroup.h"

IStressTensorGroup_Delegate::CPrincipalLengthChangeComposite_Delegate ::
  CPrincipalLengthChangeComposite_Delegate (
  IStressTensorGroup::CPrincipalLengthChangeComposite*
      principalLengthChangeComposite)
: ITensorGroup_Delegate::CVectorComposite_Delegate(
  principalLengthChangeComposite)
, m_principalLengthChangeComposite(principalLengthChangeComposite)
{
}

IStressTensorGroup_Delegate::CInvariantComposite_Delegate::
  CInvariantComposite_Delegate(
  IStressTensorGroup::CInvariantComposite* invariantComposite)
: IResult_Delegate(invariantComposite)
, m_invariantComposite(invariantComposite)
{
}

IStressTensorGroup_Delegate::CWellPathStressComposite_Delegate::
  CWellPathStressComposite_Delegate(
  IStressTensorGroup::CWellPathStressComposite* wellPathStressComposite)
: IResult_Delegate(wellPathStressComposite)
, m_wellPathStressComposite(wellPathStressComposite)
{
}

IStressTensorGroup_Delegate::CVerticalGradientComposite_Delegate::
  CVerticalGradientComposite_Delegate(
  IStressTensorGroup::CVerticalGradientComposite* verticalGradientComposite)
: IResult_Delegate(verticalGradientComposite)
, m_verticalGradientComposite(verticalGradientComposite)
{
}

IStressTensorGroup_Delegate::IStressTensorGroup_Delegate(
  IStressTensorGroup* stressTensorGroup)
: ITensorGroupTemplate_Delegate <CStressTensor> (stressTensorGroup)
, m_stressTensorGroup(stressTensorGroup)
{
}
