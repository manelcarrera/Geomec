#ifndef _ModelProxyConsole_h_
#define _ModelProxyConsole_h_

class CFemAppModel;

#include "ModelProxy.h"

class CModelProxyConsole : public CModelProxy {
public:
  CModelProxyConsole(CFemAppModel &model);

  virtual ~CModelProxyConsole();

  virtual void createContainers();
  virtual void detachModel();
  virtual void onCloseModel();
  virtual void switchToParent();

private:
  CModelProxyConsole(const CModelProxyConsole &rhs);
  CModelProxyConsole &operator=(const CModelProxyConsole &rhs);

  CFemAppModel &m_model;
};

#endif // _ModelProxyConsole_h_
