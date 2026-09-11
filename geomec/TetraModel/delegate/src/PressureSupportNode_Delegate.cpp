#include "stdafx.h"

#include "PressureSupportNode_Delegate.h"
#include "PressureSupportNode.h"
#include "TetraBoundary.h"
#include "ContextMenuInvoker.h"
#include "GeomecDoc.h"
#include "FieldFactor.h"
#include "ModelBase.h"
#include "DepletionStage.h"
#include "resourceIDP.h"
#include "GlobalMessage.h"
#include "FaceSupportDlg.h"
#include "ResultRegister.h"
#include "ModifiedHint.h"
#include "FemAppMainWindow.h"

CPressureSupportNode_Delegate::CPressureSupportNode_Delegate(
  CPressureSupportNode* pressureSupportNode)
: C3DSupportNode_Delegate(pressureSupportNode)
, m_pressureSupportNode(pressureSupportNode)
{
}

bool CPressureSupportNode_Delegate::MouseRelease(TFrame& frame,
  TKeyboardModifiers state, TMouseButton button, const TScreenPoint& point,
  const TObjectVec& vcHit)
{
  if (m_pressureSupportNode->Boundary().CreateInterfaces())
  {
  return false; // we don't allow application of supports this way when
                  // interface elements are used. (for now)
  }

  if ((button == Qt::LeftButton) || (button == Qt::RightButton))
  {
  for (size_t i = 0; i < vcHit.size(); i++)
  {
      for (size_t j = 0; j < m_pressureSupportNode->getSurfaces().size(); j++)
      {
    if (vcHit[i] == m_pressureSupportNode->getSurfaces()[j].first)
    {
          m_pressureSupportNode->setSelectedSurface(m_pressureSupportNode->getSurfaces()[j].first);
          m_pressureSupportNode->Modified(SelectionChanged);

          if (button == Qt::RightButton)
          {
      // show context menu

      CContextMenuInvoker menuInvoker;

      menuInvoker.AddCommand("Attributes",
              *new CSingleCommandTemplate <
        CPressureSupportNode_Delegate> (
                  *this, &CPressureSupportNode_Delegate::OnShowDialog));

      TScreenPoint global = frame.GlobalScreenPoint(point);

      menuInvoker.TrackMenu(global.x(), global.y(), *FemAppGetMainWnd());
          }

          return true;
    }
      }
  }

  m_pressureSupportNode->setSelectedSurface(0);
  m_pressureSupportNode->Modified(SelectionChanged);
  }

  return false;
}

void CPressureSupportNode_Delegate::AppendContextMenu(
  CContextMenuInvoker &invoker)
{
  invoker.AddCommand(_T("Global stress tensor"),
  *(new CSingleCommandTemplate <CPressureSupportNode_Delegate> (*this,
      &CPressureSupportNode_Delegate::EditGlobalTensor,
      &CPressureSupportNode_Delegate::CanEditTensor)));
}

void CPressureSupportNode_Delegate::OnShowDialog()
{
  // check whether there are multiple depletion stages

  const CModelBase* pModel =
  dynamic_cast <const CModelBase*> (&m_pressureSupportNode->Model());

  assert(pModel != 0);

  if (pModel->InitialDepletionStage().Last())
  {
  _m()->msg(IDP_MULTIPLEDEPLETIONSTAGESNEEDED);

  return;
  }

  assert(m_pressureSupportNode->getSelectedSurface() != 0);

  CPressureSupportNode::TStageSupportMap::iterator its =
  m_pressureSupportNode->getSupports().find(
      CPressureSupportNode::CSurfaceDef(
    *m_pressureSupportNode->getSelectedSurface()));

  assert(its != m_pressureSupportNode->getSupports().end());

  CPressureSupportNode::CFaceSupportDef& def = its->second;
  CPressureSupportNode::CFaceSupportDef* copy =
  new CPressureSupportNode::CFaceSupportDef(def);
  CFaceSupportDlg dlg(*copy, *m_pressureSupportNode);

  // keep track for modified flags

  bool bModified = false;

  if (dlg.DoModal() == IDOK)
  {
  if (dlg.ApplyToAll())
  {
      for (CPressureSupportNode::TStageSupportMap::iterator it =
    m_pressureSupportNode->getSupports().begin();
    it != m_pressureSupportNode->getSupports().end(); it++)
      {
    if (!(it->second == *copy))
    {
          bModified = true;
    }

    it->second = *copy;
      }
  }
  else
  {
      if (!(def == *copy))
      {
    bModified = true;
      }

      def = *copy;
  }

  if (bModified)
  {
      CModelBase& model =
    static_cast <CModelBase&> (m_pressureSupportNode->Model());

      model.ResultRegister().ClearLinear(false);
      model.ResultRegister().ClearNonLinear(false);
      model.ResultRegister().ClearMixture();
      GetGeomecDoc()->SetModifiedFlag();
  }
  }

  delete copy;
}

double CPressureSupportNode_Delegate::UnitFactor() const
{
  const CFemAppDoc* pDoc = GetGeomecDoc();

  assert(pDoc != 0);

  double dRet = 1;

  if (pDoc->UnitNode().Unit() == IQuantityDouble::FIELD_UNIT)
  {
  dRet = FF_FACTOR_STRESS;
  }

  return dRet;
}

QString CPressureSupportNode_Delegate::UnitName() const
{
  const CFemAppDoc* pDoc = GetGeomecDoc();

  assert(pDoc != 0);

  if (pDoc->UnitNode().Unit() == IQuantityDouble::FIELD_UNIT)
  {
  return "psi";
  }

  return "MPa";
}
