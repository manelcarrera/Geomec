#include "stdafx.h"

#include "MeshSampler_Delegate.h"
#include "MeshSampler.h"
#include "RenameNodeDlg.h"

CPillarMap_Delegate::CPillarMap_Delegate(CPillarMap* pillarMap)
: CColorNode_Delegate(pillarMap)
, m_pillarMap(pillarMap)
{
}

bool CPillarMap_Delegate::Attributes()
{
  CRenameNodeDlg dlg(*m_pillarMap);

  return dlg.DoModal() == IDOK;
}
