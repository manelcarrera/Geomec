#ifndef _GUIFactory_h_
#define _GUIFactory_h_

class CFemAppGUI;
class CGUIBase;
class CModelBase;

class CGUIFactory
{
public:
  CGUIFactory();

  static CGUIBase* createGUI(CFemAppGUI* parentGUI, CFemAppModel* model);

private:
  CGUIFactory(const CGUIFactory& rhs);
  CGUIFactory& operator = (const CGUIFactory& rhs);
};

#endif  // _GUIFactory_h_
