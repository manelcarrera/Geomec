#ifndef _ITensorGroupTemplate_Delegate_h_
#define _ITensorGroupTemplate_Delegate_h_

#include "ITensorGroup_Delegate.h"
#include "ITensorGroupTemplate.h"

template <class TENSOR_TYPE>
  class ITensorGroupTemplate_Delegate : public ITensorGroup_Delegate
{
public:
  ITensorGroupTemplate_Delegate(
  ITensorGroupTemplate <TENSOR_TYPE> * tensorGroupTemplate);

private:
  ITensorGroupTemplate_Delegate(const ITensorGroupTemplate_Delegate& rhs);
  ITensorGroupTemplate_Delegate& operator = (
  const ITensorGroupTemplate_Delegate& rhs);

  ITensorGroupTemplate <TENSOR_TYPE> * m_tensorGroupTemplate;

  // TODO REGISTER_DELEGATE(ITensorGroupTemplate, ITensorGroupTemplate_Delegate);
};

template <class TENSOR_TYPE>
  ITensorGroupTemplate_Delegate <TENSOR_TYPE> ::
  ITensorGroupTemplate_Delegate(
      ITensorGroupTemplate <TENSOR_TYPE> * tensorGroupTemplate)
: ITensorGroup_Delegate(tensorGroupTemplate)
, m_tensorGroupTemplate(tensorGroupTemplate)
{
}

#endif  // _ITensorGroupTemplate_Delegate_h_
