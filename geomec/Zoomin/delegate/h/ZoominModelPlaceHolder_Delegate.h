#ifndef _ZOOMINMODELPLACEHOLDER_DELEGATE_H_
#define _ZOOMINMODELPLACEHOLDER_DELEGATE_H_

#include "OpenGLNode_Delegate.h"

#include "ZoominModelPlaceHolder.h"

class CZoominModelPlaceHolder_Delegate : public COpenGLNode_Delegate {
public:
  CZoominModelPlaceHolder_Delegate(CZoominModelPlaceHolder *zoominModelPlaceHolder);

  virtual void AppendContextMenu(CContextMenuInvoker &invoker);
  virtual bool Attributes();

private:
  CZoominModelPlaceHolder *m_zoominModelPlaceHolder;

  CZoominModelPlaceHolder_Delegate(const CZoominModelPlaceHolder_Delegate &);
  CZoominModelPlaceHolder_Delegate &operator=(const CZoominModelPlaceHolder_Delegate &);

  REGISTER_DELEGATE(CZoominModelPlaceHolder, CZoominModelPlaceHolder_Delegate);
};

#endif // _ZOOMINMODELPLACEHOLDER_DELEGATE_H_
