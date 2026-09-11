#ifndef _DocumentProxy_h_
#define _DocumentProxy_h_

class ISceneWrapper;

class CDocumentProxy
{
public:
  CDocumentProxy();

  virtual ~CDocumentProxy() = 0;

  virtual ISceneWrapper* currentScene() = 0;
  virtual void currentScene(ISceneWrapper* sceneWrapper) = 0;
  virtual void onAnalysisProperties() = 0;
  virtual void onInvalidateStorage() = 0;
  virtual void setModifiedFlag(bool modified = true) = 0;

private:
  CDocumentProxy(const CDocumentProxy& rhs);
  CDocumentProxy& operator = (const CDocumentProxy& rhs);
};

#endif  // _DocumentProxy_h_
