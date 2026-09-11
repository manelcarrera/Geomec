#ifndef _WellCasingGUI_h_
#define _WellCasingGUI_h_

class CWellCasingModel;

#include "IWellGUI.h"

class CWellCasingGUI : public IWellGUI
{
public:
  CWellCasingGUI(CFemAppGUI* parentGUI, CWellCasingModel* wellCasingModel);

  virtual void SwitchTo();

  virtual void OnCloseModel();

  CWellCasingModel* model() const;

protected:
  virtual bool LoadPre381Stream(CStorageNode::TSTREAM &stream, CStreamVersion &version, CStorageNode::TPROGRESS &prog);

private:
  CWellCasingGUI(const CWellCasingGUI& rhs);
  CWellCasingGUI& operator = (const CWellCasingGUI& rhs);

  CWellCasingModel* m_wellCasingModel;
};

#endif  // _WellCasingGUI_h_
