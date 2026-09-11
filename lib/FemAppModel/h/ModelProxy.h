#ifndef _ModelProxy_h_
#define _ModelProxy_h_

class CModelProxy
{
public:
  CModelProxy();

  virtual ~CModelProxy() = 0;

  virtual void createContainers() = 0;
  virtual void detachModel() = 0;
  virtual void onCloseModel() = 0;
  virtual void switchToParent() = 0;

private:
  CModelProxy(const CModelProxy& rhs);
  CModelProxy& operator = (const CModelProxy& rhs);
};

#endif  // _ModelProxy_h_
