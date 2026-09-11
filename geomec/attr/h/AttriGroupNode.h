#pragma once

#include "AttributeTempl.h"
#include "GroupNode.h"

class CAttriGroupNode : public CAttributesTemplate<CGroupNode> {
public:
  CAttriGroupNode(CGroupNode &groupNode, CWnd *pParent = NULL);

protected:
  virtual BOOL OnInitDialog();
  virtual void DoDataExchange(CDataExchange *pDX);

  DECLARE_MESSAGE_MAP();

private:
  void UpdateControls();
};
