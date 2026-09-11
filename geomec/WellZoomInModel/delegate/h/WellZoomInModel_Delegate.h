#ifndef _WellZoomInModel_Delegate_h_
#define _WellZoomInModel_Delegate_h_

class CWellZoomInModel;

#include "IWellModel_Delegate.h"

class CWellZoomInModel_Delegate : public IWellModel_Delegate {
public:
  CWellZoomInModel_Delegate(CWellZoomInModel *wellZoomInModel);

  virtual void AppendContextMenu(CContextMenuInvoker &invoker);

private:
  CWellZoomInModel_Delegate(const CWellZoomInModel_Delegate &rhs);
  CWellZoomInModel_Delegate &operator=(const CWellZoomInModel_Delegate &rhs);

  CWellZoomInModel *m_wellZoomInModel;

  REGISTER_DELEGATE(CWellZoomInModel, CWellZoomInModel_Delegate);
};

#endif // _WellZoomInModel_Delegate_h_
