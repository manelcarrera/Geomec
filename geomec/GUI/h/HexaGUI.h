#ifndef _HexaGUI_h_
#define _HexaGUI_h_

class CHexaModel;

#include "3DGUI.h"

class CHexaGUI : public C3DGUI
{
public:
  CHexaGUI(CFemAppGUI* parentGUI, CHexaModel* hexaModel);

  bool CanConvertToTetra() const;
  void ConvertToTetraModel(); 

  CGUIBase* ConvertToTetra();

  virtual void OnCloseModel();

  CHexaModel* model() const;

protected:
  virtual bool LoadPre381Stream(CStorageNode::TSTREAM &stream, CStreamVersion &version, CStorageNode::TPROGRESS &prog);

private:
  CHexaGUI(const CHexaGUI& rhs);
  CHexaGUI& operator = (const CHexaGUI& rhs);

  CHexaModel* m_hexaModel;
};

#endif  // _HexaGUI_h_
