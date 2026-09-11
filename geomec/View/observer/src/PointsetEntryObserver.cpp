// DataTreeView.cpp : implementation file
//

#include "stdafx.h"
#include "geomec.h"
#include "PointSetEntryObserver.h"
#include "BaseEntryTypes.h"
#include "PointSet.h"
#include "GeoSurface.h"
#include "MaterialEntry.h"
#include "ValueType.h"
#include "RpnValueset.h"
#include "MeshBase.h"
#include "PointSetCreateDlg.h"
#include "KeyFile.h"
#include <vector>
#include "RpnValueSet_Delegate.h"
#include "WMCommandCommand.h"
#include "ContextMenuInvoker.h"
#include "IPointSet_Delegate.h"
#include "resourceIDI.h"
#include "FemAppMainWindow.h"

#ifdef _DEBUG
//#define new DEBUG_NEW
#ifdef _MSC_VER
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif  // _MSC_VER
#endif

BOOL CPointSetEntryObserver::OnFilterPointSet(const IPointSet& point_set) const
{
  return TRUE;
}

/*!
  Default two branches are placed under the poinset
*/
CTreeNode* CPointSetEntryObserver::OnInsertPointSetObserver(CPointSetSubBranch& branch, IPointSet& point_set)
{
  CTreeNode* pTreeNode = branch.CEnumerationBranch<IPointSet, TPointSetObserver, TRUE, DELETE_ITEM>::InsertChild(point_set);
  CModelBase& model = (CModelBase&)point_set;
  if(&point_set != &model.Mesh())
  {
    TPointSetObserver* pObserver = (TPointSetObserver*)pTreeNode;
    typedef CNodeObserver_Delegate<CRpnValueSet, CRpnValueSet_Delegate, CDummyNode, CDummyObserver, FALSE, FIXED_ITEM> TRpnValueSetObs;
    typedef CEnumerationBranch<CRpnValueSet, TRpnValueSetObs, TRUE, DELETE_ITEM> TFormulaEnumerator;
    new CPropertyEnumerator(*pObserver,
                _T("Properties"),
                IDI_PROPERTY);

    new TFormulaEnumerator(*pObserver,
                 _T("Formulas"),
                 IDI_RPN_FORMULAS);
  }
  return pTreeNode;
}

CPointSetEntryObserver::CPointSetSubBranch::CPointSetSubBranch(CPointSetEntryObserver &parent_observer, 
                                     const CPointSet::DIMENSION dim,
                                     const CString& strName,
                                     const unsigned int uIcon,
                                     HTREEITEM hInsertAfter)
: TStateEnumerationBranch(parent_observer, strName, uIcon, hInsertAfter, FALSE), m_dim(dim)
{
  Update();
}

/*!
  Only allows point sets matching the dimension and the parent filter
*/
BOOL CPointSetEntryObserver::CPointSetSubBranch::OnFilter(const child_type &ps) const
{
  const CPointSetEntryObserver& parent = dynamic_cast<const CPointSetEntryObserver&>(*Parent());
  return m_dim == ps.Dimension() && parent.OnFilterPointSet(ps);
}

/*!
  The function OnInsertPointSetObserver on the parent is called.
*/
CTreeNode* CPointSetEntryObserver::CPointSetSubBranch::InsertChild(child_type& child)
{
  // Call function by parent 
  CPointSetEntryObserver& parent = dynamic_cast<CPointSetEntryObserver&>(*Parent());
  return parent.OnInsertPointSetObserver( *this, child );
}


void CPointSetEntryObserver::CPointSetSubBranch::OnChildModified(CTreeNode &child)
{
  CPointSetEntryObserver& parent = dynamic_cast<CPointSetEntryObserver&>(*Parent());
  const IPointSet* pPointSet = dynamic_cast<const IPointSet*>(&child.ObservedItem());
  assert(pPointSet);
  if(pPointSet->Dimension() != m_dim)
    parent.Update();
  else
    CEnumerationBranch<IPointSet, TPointSetObserver, TRUE, DELETE_ITEM>::OnChildModified(child);
}

CPointSetEntryObserver::CPointSetEntryObserver(TPointSetEntry& node,
                        CModelBase& model,
                        CTreeCtrl& ctrl,
                        HTREEITEM hParent,
                        HTREEITEM hInsertAfter)
:CNodeObserver_Delegate<TPointSetEntry, TPointSetEntry_Delegate, CDummyNode, CDummyObserver, FALSE, FIXED_ITEM>(node, ctrl, FALSE, FIXED_ITEM, hParent, hInsertAfter)
{CPointSetSubBranch* p =
  new CPointSetSubBranch(*this,
               CPointSet::DIM_2D,
               _T("2D"),
               IDI_POINTSET);
p->StateIcon();
  new CPointSetSubBranch(*this,
               CPointSet::DIM_3D,
               _T("3D"),
               IDI_POINTSET);
}

void CPointSetEntryObserver::CreatePointSet()
{
  CPointSetCreateDlg dlg((CModelBase&)((TPointSetEntry&)ObservedItem()).Model());
  dlg.DoModal();
}

void CPointSetEntryObserver::AppendContextMenu(CContextMenuInvoker &invoker)
{
  typedef CSingleCommandTemplate<CPointSetEntryObserver> TPointSetBranchCommand;
  invoker.AddCommand( _T("&Import..."),*(new CWMCommandCommand(FemAppGetMainWnd()->m_hWnd,ID_FILE_IMPORT)));
  invoker.AddSeparator();
  invoker.AddCommand( _T("&Create point set ..."),*(new TPointSetBranchCommand(*this, &CPointSetEntryObserver::CreatePointSet)));
  typedef CSingleCommandTemplate<CPointSetEntry> TPointSetEntryCommand;
  invoker.AddCommand(_T("Generate all convex &hulls"), *new TPointSetEntryCommand(m_node, &CPointSetEntry::CreateAllHulls, &CPointSetEntry::CanCreateAllHulls));
  invoker.AddCommand(_T("Delete all unused pointsets"), *new TPointSetEntryCommand(m_node, &CPointSetEntry::DeleteAllUnusedPointsets, &CPointSetEntry::CanDeleteAllUnusedPointsets));
}


/////////////////////////////////////////////////////////////////////////////
// CPointSetObserver

CPointSetEntryObserver::CPropertyEnumerator::CPropertyEnumerator(TPointSetObserver &parent_observer, 
                                     const CString& strName,
                                     const unsigned int uIcon,
                                     HTREEITEM hInsertAfter)
: TPropertyEnumerator(parent_observer, strName, uIcon, hInsertAfter)
{
  Update();
}

BOOL CPointSetEntryObserver::CPropertyEnumerator::OnFilter(const child_type& t) const
{	
  const CPointSet* pPointSet = dynamic_cast<const CPointSet*>(&ObservedItem());
  if(pPointSet)
    return &pPointSet->Coordinates() != &t;
  return true;
}

