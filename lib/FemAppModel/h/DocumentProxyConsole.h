#ifndef _DocumentProxyConsole_h_
#define _DocumentProxyConsole_h_

#include "DocumentProxy.h"

class CDocumentProxyConsole : public CDocumentProxy
{
public:
  CDocumentProxyConsole();

  virtual ~CDocumentProxyConsole();

  virtual ISceneWrapper* currentScene();
  virtual void currentScene(ISceneWrapper* sceneWrapper);
  virtual void onAnalysisProperties();
  virtual void onInvalidateStorage();
  virtual void setModifiedFlag(bool modified = true);

private:
  CDocumentProxyConsole(const CDocumentProxyConsole& rhs);
  CDocumentProxyConsole& operator = (const CDocumentProxyConsole& rhs);
};

#endif  // _DocumentProxyConsole_h_
