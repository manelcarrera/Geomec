#ifndef _IStressTensorGroup_Delegate_h_
#define _IStressTensorGroup_Delegate_h_

#include "ITensorGroupTemplate_Delegate.h"
#include "IStressTensorGroup.h"

class IStressTensorGroup_Delegate :
  public ITensorGroupTemplate_Delegate <CStressTensor>
{
public:
  class CPrincipalLengthChangeComposite_Delegate :
  public ITensorGroup_Delegate::CVectorComposite_Delegate
  {
  public:
  CPrincipalLengthChangeComposite_Delegate(
      IStressTensorGroup::CPrincipalLengthChangeComposite*
    principalLengthChangeComposite);

  private:
  CPrincipalLengthChangeComposite_Delegate(
      const CPrincipalLengthChangeComposite_Delegate& rhs);
  CPrincipalLengthChangeComposite_Delegate& operator = (
      const CPrincipalLengthChangeComposite_Delegate& rhs);

  IStressTensorGroup::CPrincipalLengthChangeComposite*
      m_principalLengthChangeComposite;

  REGISTER_DELEGATE(IStressTensorGroup::CPrincipalLengthChangeComposite,
      CPrincipalLengthChangeComposite_Delegate);
  };

  class CInvariantComposite_Delegate : public IResult_Delegate
  {
  public:
  CInvariantComposite_Delegate(
      IStressTensorGroup::CInvariantComposite* invariantComposite);

  private:
  CInvariantComposite_Delegate(const CInvariantComposite_Delegate& rhs);
  CInvariantComposite_Delegate& operator = (
      const CInvariantComposite_Delegate& rhs);

  IStressTensorGroup::CInvariantComposite* m_invariantComposite;

  REGISTER_DELEGATE(IStressTensorGroup::CInvariantComposite,
      CInvariantComposite_Delegate);
  };

  class CWellPathStressComposite_Delegate : public IResult_Delegate
  {
  public:
  CWellPathStressComposite_Delegate(
      IStressTensorGroup::CWellPathStressComposite* wellPathStressComposite);

  private:
  CWellPathStressComposite_Delegate(
      const CWellPathStressComposite_Delegate& rhs);
  CWellPathStressComposite_Delegate& operator = (
      const CWellPathStressComposite_Delegate& rhs);

  IStressTensorGroup::CWellPathStressComposite* m_wellPathStressComposite;

  REGISTER_DELEGATE(IStressTensorGroup::CWellPathStressComposite,
      CWellPathStressComposite_Delegate);
  };

  class CVerticalGradientComposite_Delegate : public IResult_Delegate
  {
  public:
  CVerticalGradientComposite_Delegate(
      IStressTensorGroup::CVerticalGradientComposite* verticalGradientComposite);

  private:
  CVerticalGradientComposite_Delegate(
      const CVerticalGradientComposite_Delegate& rhs);
  CVerticalGradientComposite_Delegate& operator=(
      const CVerticalGradientComposite_Delegate& rhs);

  IStressTensorGroup::CVerticalGradientComposite* m_verticalGradientComposite;

  REGISTER_DELEGATE(IStressTensorGroup::CVerticalGradientComposite,
      CVerticalGradientComposite_Delegate);
  };

  IStressTensorGroup_Delegate(IStressTensorGroup* stressTensorGroup);

private:
  IStressTensorGroup_Delegate(const IStressTensorGroup_Delegate& rhs);
  IStressTensorGroup_Delegate& operator = (
  const IStressTensorGroup_Delegate& rhs);

  IStressTensorGroup* m_stressTensorGroup;

  REGISTER_DELEGATE(IStressTensorGroup, IStressTensorGroup_Delegate);
};

#endif  // _IStressTensorGroup_Delegate_h_
