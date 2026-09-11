#ifndef _EclipseGUI_h_
#define _EclipseGUI_h_

class CEclipseModel;

#include "TetraGUI.h"

class CEclipseGUI : public CTetraGUI {
public:
  CEclipseGUI(CFemAppGUI *parentGUI, CEclipseModel *eclipseModel);

  virtual bool NewModel(bool bAttachToDocument = true);

  virtual void OnCloseModel();

  bool EditSubModel();

  CEclipseModel *model() const;

protected:
  virtual bool LoadPre381Stream(CStorageNode::TSTREAM &stream, CStreamVersion &version, CStorageNode::TPROGRESS &prog);

private:
  CEclipseGUI(const CEclipseGUI &rhs);
  CEclipseGUI &operator=(const CEclipseGUI &rhs);

  CEclipseModel *m_eclipseModel;
};

#endif // _EclipseGUI_h_
