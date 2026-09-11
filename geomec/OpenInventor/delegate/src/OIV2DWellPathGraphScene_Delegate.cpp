#include "stdafx.h"

#include "OIV2DWellPathGraphScene_Delegate.h"
#include "OIV2DWellPathGraphScene.h"
#include "attri2dscene.h"

OIV2DWellPathGraphScene_Delegate::OIV2DWellPathGraphScene_Delegate(OIV2DWellPathGraphScene* node)
  : OIV2DGraphValueColorScene_Delegate(node)
  , m_Node(node)
{
}

bool OIV2DWellPathGraphScene_Delegate::Attributes()
{
  CAttri2DScene<OIV2DWellPathGraphScene> dlg(*m_Node);
  return dlg.DoModal() == IDOK;
}
