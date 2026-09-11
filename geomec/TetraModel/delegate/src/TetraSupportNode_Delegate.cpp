#include "stdafx.h"

#include "TetraSupportNode_Delegate.h"
#include "3DSupportNode.h"
#include "ModelBase.h"
#include "DepletionStage.h"
#include "resourceIDP.h"
#include "GlobalMessage.h"
#include "GlobalTensorDlg.h"

C3DSupportNode_Delegate::C3DSupportNode_Delegate(
  C3DSupportNode* tetraSupportNode)
: CBaseSupportNode_Delegate(tetraSupportNode)
, m_tetraSupportNode(tetraSupportNode)
{
}

double C3DSupportNode_Delegate::UnitFactor() const
{
  assert(false);

  return 0.0;
}

QString C3DSupportNode_Delegate::UnitName() const
{
  assert(false);

  return QString();
}

bool C3DSupportNode_Delegate::CanEditTensor() const
{
  return m_tetraSupportNode->CanEditTensor();
}

void C3DSupportNode_Delegate::EditGlobalTensor()
{
  // check whether there are multiple depletion stages

  const CModelBase* pModel =
    dynamic_cast <const CModelBase*> (&m_tetraSupportNode->Model());

  assert(pModel != 0);

  if (pModel->InitialDepletionStage().Last())
  {
    _m()->msg(IDP_MULTIPLEDEPLETIONSTAGESNEEDED);

    return;
  }

  CGlobalTensorDlg dlg(*m_tetraSupportNode);

  if (dlg.DoModal() == IDOK)
  {
    m_tetraSupportNode->EditGlobalTensor();
  }
}
