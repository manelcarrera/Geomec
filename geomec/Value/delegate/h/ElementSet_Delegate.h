#ifndef _ElementSet_Delegate_h_
#define _ElementSet_Delegate_h_

class IElementSet;
class CElementSet;

#include "IPointSet_Delegate.h"

class IElementSet_Delegate : public IPointSet_Delegate {
public:
  IElementSet_Delegate(IElementSet *elementSet);

  // Export functions

  virtual void Export();

private:
  IElementSet_Delegate(const IElementSet_Delegate &rhs);
  IElementSet_Delegate &operator=(const IElementSet_Delegate &rhs);

  IElementSet *m_elementSet;

  REGISTER_DELEGATE(IElementSet, IElementSet_Delegate);
};

class CElementSet_Delegate : public IElementSet_Delegate {
public:
  CElementSet_Delegate(CElementSet *elementSet);

  virtual bool Attributes();

  virtual void AppendContextMenu(CContextMenuInvoker &invoker);

private:
  CElementSet_Delegate(const CElementSet_Delegate &rhs);
  CElementSet_Delegate &operator=(const CElementSet_Delegate &rhs);

  CElementSet *m_elementSet;

  REGISTER_DELEGATE(CElementSet, CElementSet_Delegate);
};

#endif // _ElementSet_Delegate_h_
