#ifndef _DocumentProxyGUI_h_
#define _DocumentProxyGUI_h_

class CFemAppDoc;

#include "DocumentProxy.h"

class CDocumentProxyGUI : public CDocumentProxy
{
public:
  CDocumentProxyGUI(CFemAppDoc& document);

  virtual ~CDocumentProxyGUI();

  virtual ISceneWrapper* currentScene();
  virtual void currentScene(ISceneWrapper* sceneWrapper);
  virtual void onAnalysisProperties();
  virtual void onInvalidateStorage();
  virtual void setModifiedFlag(bool modified = true);

private:
  CDocumentProxyGUI(const CDocumentProxyGUI& rhs);
  CDocumentProxyGUI& operator = (const CDocumentProxyGUI& rhs);

  CFemAppDoc& m_document;
};

#endif  // _DocumentProxyGUI_h_
