#ifndef _ITensorGroup_Delegate_h_
#define _ITensorGroup_Delegate_h_

#include "ITensorGroup.h"
#include "IVectorResult_Delegate.h"
#include "Result_Delegate.h"

class ITensorGroup_Delegate : public CResultGroup_Delegate {
public:
  class CComponentComposite_Delegate : public IResult_Delegate {
  public:
    CComponentComposite_Delegate(ITensorGroup::CComponentComposite *componentComposite);

  private:
    CComponentComposite_Delegate(const CComponentComposite_Delegate &rhs);
    CComponentComposite_Delegate &operator=(const CComponentComposite_Delegate &rhs);

    ITensorGroup::CComponentComposite *m_componentComposite;

    REGISTER_DELEGATE(ITensorGroup::CComponentComposite, CComponentComposite_Delegate);
  };

  class CVectorComposite_Delegate : public IVectorResult_Delegate {
  public:
    CVectorComposite_Delegate(ITensorGroup::CVectorComposite *vectorComposite);

  private:
    CVectorComposite_Delegate(const CVectorComposite_Delegate &rhs);
    CVectorComposite_Delegate &operator=(const CVectorComposite_Delegate &rhs);

    ITensorGroup::CVectorComposite *m_vectorComposite;

    REGISTER_DELEGATE(ITensorGroup::CVectorComposite, CVectorComposite_Delegate);
  };

  ITensorGroup_Delegate(ITensorGroup *tensorGroup);

private:
  ITensorGroup_Delegate(const ITensorGroup_Delegate &rhs);
  ITensorGroup_Delegate &operator=(const ITensorGroup_Delegate &rhs);

  ITensorGroup *m_tensorGroup;

  REGISTER_DELEGATE(ITensorGroup, ITensorGroup_Delegate);
};

#endif // _ITensorGroup_Delegate_h_
