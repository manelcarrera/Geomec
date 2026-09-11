#ifndef _NEW_WELLPATH_INPUT_DELEGATE_H_
#define _NEW_WELLPATH_INPUT_DELEGATE_H_

#include "PointSet_Delegate.h"

class CNewWellPathInput;

class CNewWellPathInput_Delegate : public CPointSet_Delegate {
public:
  CNewWellPathInput_Delegate(CNewWellPathInput *wellpathInput);

  virtual void AppendContextMenu(CContextMenuInvoker &invoker);

  void FlipDepth();

private:
  CNewWellPathInput_Delegate(const CNewWellPathInput_Delegate &rhs);
  CNewWellPathInput_Delegate &operator=(const CNewWellPathInput_Delegate &rhs);

  CNewWellPathInput *m_wellpathInput;

  REGISTER_DELEGATE(CNewWellPathInput, CNewWellPathInput_Delegate);
};

#endif