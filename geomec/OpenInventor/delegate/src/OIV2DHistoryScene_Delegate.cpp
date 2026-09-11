#include "stdafx.h"

#include "OIV2DHistoryScene_Delegate.h"
#include "OIV2DHistoryScene.h"
#include "attri2dscene.h"

OIV2DHistoryScene_Delegate::OIV2DHistoryScene_Delegate(OIV2DHistoryScene * node)
  : OIV2DGraphValueColorScene_Delegate(node)
  , m_Node(node)
{
}

bool OIV2DHistoryScene_Delegate::Attributes()
{
  CAttri2DScene<OIV2DHistoryScene> dlg(*m_Node);
  return dlg.DoModal() == IDOK;
}
