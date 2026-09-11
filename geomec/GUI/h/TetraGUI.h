#ifndef _TetraGUI_h_
#define _TetraGUI_h_

class CTetraModel;

#include "3DGUI.h"

class CTetraGUI : public C3DGUI
{
public:
  CTetraGUI(CFemAppGUI* parentGUI, CTetraModel* tetraModel);

  virtual void OnCloseModel();

  CTetraModel* model() const;

protected:
  virtual bool LoadPre381Stream(CStorageNode::TSTREAM &stream, CStreamVersion &version, CStorageNode::TPROGRESS &prog);

private:
  CTetraGUI(const CTetraGUI& rhs);
  CTetraGUI& operator = (const CTetraGUI& rhs);

  CTetraModel* m_tetraModel;
};

#endif  // _TetraGUI_h_
