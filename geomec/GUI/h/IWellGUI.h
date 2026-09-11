#ifndef _IWellGUI_h_
#define _IWellGUI_h_

class IWellModel;

#include "GUIBase.h"

class IWellGUI : public CGUIBase {
public:
  IWellGUI(CFemAppGUI *parentGUI, IWellModel *wellModel);

  virtual void OnCloseModel();

  IWellModel *model() const;

private:
  IWellGUI(const IWellGUI &rhs);
  IWellGUI &operator=(const IWellGUI &rhs);

  IWellModel *m_wellModel;
};

#endif // _IWellGUI_h_
