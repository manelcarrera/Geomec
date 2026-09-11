#pragma once


#include "Result_Delegate.h"

class CGVTResultGroup;

class CGVTResultGroup_Delegate : public CResultGroup_Delegate
{
public:
  CGVTResultGroup_Delegate(CGVTResultGroup* gvtResultGroup);

  virtual void AppendContextMenu(CContextMenuInvoker& invoker);

private:
  CGVTResultGroup_Delegate(const CGVTResultGroup_Delegate& rhs);
  CGVTResultGroup_Delegate& operator=(const CGVTResultGroup_Delegate& rhs);

  CGVTResultGroup* m_gvtResultGroup;

  REGISTER_DELEGATE(CGVTResultGroup, CGVTResultGroup_Delegate);
};