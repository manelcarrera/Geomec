#ifndef _WellZoomInGUI_h_
#define _WellZoomInGUI_h_

class CWellZoomInModel;

#include "IWellGUI.h"

class CWellZoomInGUI : public IWellGUI
{
public:
  CWellZoomInGUI(CFemAppGUI* parentGUI, CWellZoomInModel* wellZoomInModel);

  virtual void OnCloseModel();

  CWellZoomInModel* model() const;

protected:
  virtual bool LoadPre381Stream(CStorageNode::TSTREAM &stream, CStreamVersion &version, CStorageNode::TPROGRESS &prog);

private:
  CWellZoomInGUI(const CWellZoomInGUI& rhs);
  CWellZoomInGUI& operator = (const CWellZoomInGUI& rhs);

  CWellZoomInModel* m_wellZoomInModel;
};

#endif  // _WellZoomInGUI_h_
