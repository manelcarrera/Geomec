#include "stdafx.h"

#include "DisplacementSupportNode_Delegate.h"
#include "DisplacementSupportNode.h"
#include "TetraBoundary.h"
#include "ContextMenuInvoker.h"
#include "GeomecDoc.h"
#include "FieldFactor.h"
#include "ModelBase.h"
#include "DepletionStage.h"
#include "resourceIDP.h"
#include "GlobalMessage.h"
#include "NodalSupportDlg.h"
#include "ResultRegister.h"
#include "ModifiedHint.h"
#include "FemAppMainWindow.h"

CDisplacementSupportNode_Delegate::CDisplacementSupportNode_Delegate(
  CDisplacementSupportNode* displacementSupportNode)
: C3DSupportNode_Delegate(displacementSupportNode)
, m_displacementSupportNode(displacementSupportNode)
{
}

bool CDisplacementSupportNode_Delegate::MouseRelease(TFrame& frame,
  TKeyboardModifiers state, TMouseButton button, const TScreenPoint& point,
  const TObjectVec& vcHit)
{
    if (m_displacementSupportNode->Boundary().CreateInterfaces())
    {
        return false; // we don't allow application of supports this way when
        // interface elements are used. (for now)
    }

    if ((button == Qt::LeftButton) || (button == Qt::RightButton))
    {
        for (size_t i = 0; i < vcHit.size(); i++)
        {
            const geo::IPoint* pPoint = dynamic_cast <const geo::IPoint*> (vcHit[i]);

            if (pPoint)
            {
                for (size_t j = 0; j < m_displacementSupportNode->getCornerNodes().size(); j++)
                {
                    if (pPoint == m_displacementSupportNode->getCornerNodes()[j])
                    {
                        m_displacementSupportNode->setSelectedPoint(pPoint);
                        m_displacementSupportNode->Modified(SelectionChanged);

                        if (button == Qt::RightButton)
                        {
                            CContextMenuInvoker menuInvoker;

                            menuInvoker.AddCommand("Attributes",
                                *new CSingleCommandTemplate <
                                CDisplacementSupportNode_Delegate> (
                                *this, &CDisplacementSupportNode_Delegate::OnShowDialog));

                            TScreenPoint global = frame.GlobalScreenPoint(point);

                            menuInvoker.TrackMenu(global.x(), global.y(), *FemAppGetMainWnd());
                        }

                        return true;
                    }
                }
            }
        }

        m_displacementSupportNode->setSelectedPoint(0);
        m_displacementSupportNode->Modified(SelectionChanged);
    }

    return false;
}

void CDisplacementSupportNode_Delegate::AppendContextMenu(
  CContextMenuInvoker &invoker)
{
  invoker.AddCommand(_T("Global strain tensor"),
    *(new CSingleCommandTemplate <CDisplacementSupportNode_Delegate> (
      *this, &CDisplacementSupportNode_Delegate::EditGlobalTensor,
      &CDisplacementSupportNode_Delegate::CanEditTensor)));
}

void CDisplacementSupportNode_Delegate::OnShowDialog()
{
  // check whether there are multiple depletion stages

  const CModelBase* pModel =
    dynamic_cast <const CModelBase*> (&m_displacementSupportNode->Model());

  assert(pModel != 0);

  if (pModel->InitialDepletionStage().Last())
  {
    _m()->msg(IDP_MULTIPLEDEPLETIONSTAGESNEEDED);

    return;
  }

  assert(m_displacementSupportNode->getSelectedPoint() != 0);

  CDisplacementSupportNode::TStageSupportMap::iterator its =
    m_displacementSupportNode->getSupports().find(
      *m_displacementSupportNode->getSelectedPoint());

  assert(its != m_displacementSupportNode->getSupports().end());

  CDisplacementSupportNode::CNodalSupportDef& def = its->second;
  CDisplacementSupportNode::CNodalSupportDef* copy =
    new CDisplacementSupportNode::CNodalSupportDef(def);
  CNodalSupportDlg dlg(*copy, *m_displacementSupportNode);

  // keep track for modified flags

  bool bModified = false;

  if (dlg.DoModal() == IDOK)
  {
    if (dlg.ApplyToAll())
    {
      for (CDisplacementSupportNode::TStageSupportMap::iterator it =
        m_displacementSupportNode->getSupports().begin();
        it != m_displacementSupportNode->getSupports().end(); it++)
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
        static_cast <CModelBase&> (m_displacementSupportNode->Model());

      model.ResultRegister().ClearLinear(false);
      model.ResultRegister().ClearNonLinear(false);
      model.ResultRegister().ClearMixture();
      GetGeomecDoc()->SetModifiedFlag();
    }
  }

  delete copy;
}

double CDisplacementSupportNode_Delegate::UnitFactor() const
{
  const CFemAppDoc* pDoc = GetGeomecDoc();

  assert(pDoc != 0);

  double dRet = 1;

  if (pDoc->UnitNode().Unit() == IQuantityDouble::FIELD_UNIT)
  {
    dRet = FF_FACTOR_DIMENSIONLESS;
  }

  return dRet;
}

QString CDisplacementSupportNode_Delegate::UnitName() const
{
  return "-";
}
