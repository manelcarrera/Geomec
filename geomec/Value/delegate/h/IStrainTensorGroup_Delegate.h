#ifndef _IStrainTensorGroup_Delegate_h_
#define _IStrainTensorGroup_Delegate_h_

#include "ITensorGroupTemplate_Delegate.h"
#include "IStrainTensorGroup.h"

class IStrainTensorGroup_Delegate :
  public ITensorGroupTemplate_Delegate <CStrainTensor>
{
public:
  class CInvariantComposite_Delegate : public IResult_Delegate
  {
  public:
  CInvariantComposite_Delegate(
      IStrainTensorGroup::CInvariantComposite* invariantComposite);

  private:
  CInvariantComposite_Delegate(const CInvariantComposite_Delegate& rhs);
  CInvariantComposite_Delegate& operator = (
      const CInvariantComposite_Delegate& rhs);

  IStrainTensorGroup::CInvariantComposite* m_invariantComposite;

  REGISTER_DELEGATE(IStrainTensorGroup::CInvariantComposite,
      CInvariantComposite_Delegate);
  };

  class CWellPathAxialStrainComposite_Delegate : public IResult_Delegate
  {
  public:
  CWellPathAxialStrainComposite_Delegate(
      IStrainTensorGroup::CWellPathAxialStrainComposite*
    wellPathAxialStrainComposite);

  private:
  CWellPathAxialStrainComposite_Delegate(
      const CWellPathAxialStrainComposite_Delegate& rhs);
  CWellPathAxialStrainComposite_Delegate& operator = (
      const CWellPathAxialStrainComposite_Delegate& rhs);

  IStrainTensorGroup::CWellPathAxialStrainComposite* m_wellPathAxialStrainComposite;

  REGISTER_DELEGATE(IStrainTensorGroup::CWellPathAxialStrainComposite,
      CWellPathAxialStrainComposite_Delegate);
  };

  IStrainTensorGroup_Delegate(IStrainTensorGroup* strainTensorGroup);

private:
  IStrainTensorGroup_Delegate(const IStrainTensorGroup_Delegate& rhs);
  IStrainTensorGroup_Delegate& operator = (
  const IStrainTensorGroup_Delegate& rhs);

  IStrainTensorGroup* m_strainTensorGroup;

  REGISTER_DELEGATE(IStrainTensorGroup, IStrainTensorGroup_Delegate);
};

#endif  // _IStrainTensorGroup_Delegate_h_
