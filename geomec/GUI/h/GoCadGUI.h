#ifndef _GoCadGUI_h_
#define _GoCadGUI_h_

class CGoCadModel;
class CHexaModel;

#include "TetraGUI.h"

class CGoCadGUI : public CTetraGUI
{
public:
  CGoCadGUI(CFemAppGUI* parentGUI, CGoCadModel* goCadModel);

  void Convert(const CHexaModel& model);

  virtual void OnCloseModel();

  CGoCadModel* model() const;

private:
  CGoCadGUI(const CGoCadGUI& rhs);
  CGoCadGUI& operator = (const CGoCadGUI& rhs);

  CGoCadModel* m_goCadModel;
};

#endif  // _GoCadGUI_h_
