#ifndef _GocadElementSet_Delegate_h_
#define _GocadElementSet_Delegate_h_

class CGocadElementSet;

#include "ElementSet_Delegate.h"

class CGocadElementSet_Delegate : public CElementSet_Delegate
{
public:
  CGocadElementSet_Delegate(CGocadElementSet* gocadElementSet);

  virtual void AppendContextMenu(CContextMenuInvoker& invoker);

  bool canImportInGocadModel() const;
  void importInGocadModel();

private:
  CGocadElementSet_Delegate(const CGocadElementSet_Delegate& rhs);
  CGocadElementSet_Delegate& operator = (const CGocadElementSet_Delegate& rhs);

  CGocadElementSet* m_gocadElementSet;

  REGISTER_DELEGATE(CGocadElementSet, CGocadElementSet_Delegate);
};

#endif  // _GocadElementSet_Delegate_h_
