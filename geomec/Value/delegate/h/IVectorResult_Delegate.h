#ifndef _IVectorResult_Delegate_h_
#define _IVectorResult_Delegate_h_

class CVectorResult;
class CVectorComponent;

#include "Result_Delegate.h"
#include "IVectorResult.h"

class IVectorResult_Delegate : public IResult_Delegate
{
public:
  class CVectorComponent_Delegate : public IResultComponent_Delegate
  {
  public:
  CVectorComponent_Delegate(
      IVectorResult::CVectorComponent* vectorComponent);

  private:
  CVectorComponent_Delegate(const CVectorComponent_Delegate& rhs);
  CVectorComponent_Delegate& operator = (
      const CVectorComponent_Delegate& rhs);

  IVectorResult::CVectorComponent* m_vectorComponent;

  REGISTER_DELEGATE(IVectorResult::CVectorComponent, CVectorComponent_Delegate);
  };

  IVectorResult_Delegate(IVectorResult* vectorResult);

private:
  IVectorResult_Delegate(const IVectorResult_Delegate& rhs);
  IVectorResult_Delegate& operator = (const IVectorResult_Delegate& rhs);

  IVectorResult* m_vectorResult;

  REGISTER_DELEGATE(IVectorResult, IVectorResult_Delegate);
};

class CVectorResult_Delegate : public IVectorResult_Delegate
{
public:
  CVectorResult_Delegate(CVectorResult* vectorResult);

private:
  CVectorResult_Delegate(const CVectorResult_Delegate& rhs);
  CVectorResult_Delegate& operator = (const CVectorResult_Delegate& rhs);

  CVectorResult* m_vectorResult;

  REGISTER_DELEGATE(CVectorResult, CVectorResult_Delegate);
};

#endif  // _IVectorResult_Delegate_h_
