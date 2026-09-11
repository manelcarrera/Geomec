#ifndef _ModelProxyGUI_h_
#define _ModelProxyGUI_h_

class CFemAppGUI;
class CFemAppDoc;

#include "ModelProxy.h"

class CModelProxyGUI : public CModelProxy
{
public:
  CModelProxyGUI(CFemAppDoc& document, CFemAppModel& model, CFemAppGUI& gui);

  virtual ~CModelProxyGUI();

  virtual void createContainers();
  virtual void detachModel();
  virtual void onCloseModel();
  virtual void switchToParent();

private:
  CModelProxyGUI(const CModelProxyGUI& rhs);
  CModelProxyGUI& operator = (const CModelProxyGUI& rhs);

  CFemAppDoc& m_document;
  CFemAppModel& m_model;
  CFemAppGUI& m_gui;
};

#endif  // _ModelProxyGUI_h_
