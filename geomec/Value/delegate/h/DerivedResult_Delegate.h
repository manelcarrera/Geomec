#ifndef _DerivedResult_Delegate_h_
#define _DerivedResult_Delegate_h_

#include "Result_Delegate.h"
#include "DerivedResult.h"

class CDerivedResult_Delegate : public IResult_Delegate
{
public:
  class CDerivedResultComponent_Delegate : public IResultComponent_Delegate
  {
  public:
    CDerivedResultComponent_Delegate(
      CDerivedResult::CDerivedResultComponent* derivedResultComponent);

    virtual bool Attributes();

    virtual void AppendContextMenu(CContextMenuInvoker& invoker);

  private:
    CDerivedResultComponent_Delegate(
      const CDerivedResultComponent_Delegate& rhs);
    CDerivedResultComponent_Delegate& operator = (
      const CDerivedResultComponent_Delegate& rhs);

    CDerivedResult::CDerivedResultComponent* m_derivedResultComponent;

    REGISTER_DELEGATE(CDerivedResult::CDerivedResultComponent,
      CDerivedResultComponent_Delegate);
  };

  CDerivedResult_Delegate(CDerivedResult* derivedResult);

  virtual bool Attributes();

  virtual void AppendContextMenu(CContextMenuInvoker& invoker);

  // Export functions

  bool CanExport() const;
  void Export();

  virtual bool Destroy();

private:
  CDerivedResult_Delegate(const CDerivedResult_Delegate& rhs);
  CDerivedResult_Delegate& operator = (const CDerivedResult_Delegate& rhs);

  CDerivedResult* m_derivedResult;

  REGISTER_DELEGATE(CDerivedResult, CDerivedResult_Delegate);
};

class CDerivedResultGroup_Delegate : public CResultGroup_Delegate
{
public:
  CDerivedResultGroup_Delegate(CDerivedResultGroup* derivedResultGroup);

  virtual void AppendContextMenu(CContextMenuInvoker& invoker);

  void OnCreateDerivedResult();

private:
  CDerivedResultGroup_Delegate(const CDerivedResultGroup_Delegate& rhs);
  CDerivedResultGroup_Delegate& operator = (
    const CDerivedResultGroup_Delegate& rhs);

  CDerivedResultGroup* m_derivedResultGroup;

  REGISTER_DELEGATE(CDerivedResultGroup, CDerivedResultGroup_Delegate);
};

#endif  // _DerivedResult_Delegate_h_
