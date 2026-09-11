// DataTreeView.cpp : implementation file
//

#include "stdafx.h"
#include "geomec.h"
#include "DataTreeView.h"
#include "BaseEntryTypes.h"
#include "PointSet.h"
#include "GeoSurface.h"
#include "MaterialEntry.h"
#include "ValueType.h"
#include "RpnValueset.h"
#include "MeshBase.h"
#include "WellZoomInModel.h"
#include "WellModelEntryTypes.h"
#include "WellCasingSteelMaterial.h"
#include "InterfaceMaterial.h"

#include "PointSetCreateDlg.h"
#include "KeyFile.h"
#include <vector>
#include "WellCasingSteelMaterial_Delegate.h"
#include "InterfaceMaterial_Delegate.h"
#include "MaterialEntry_Delegate.h"
#include "IMaterial_Delegate.h"
#include "geoSurface_Delegate.h"
#include "RpnValueSet_Delegate.h"
#include "WMCommandCommand.h"
#include "FemAppGUI.h"
#include "ContextMenuInvoker.h"
#include "GeoSurfaceObserver.h"
#include "MaterialEntryObserver.h"
#include "resourceIDS.h"
#include "WellCasingSteelMaterialObserver.h"
#include "InterfaceMaterialObserver.h"
#include "GUIBase.h"
#include "ISceneWrapper.h"
#include "OpenInventorSceneWrapper.h"
#include "FemAppMainWindow.h"
#include "ZoominModelEntry.h"
#include "NewWellPathInput.h"
#include "ZoominModelPlaceHolder.h"
#include "NewWellPath.h"

#ifdef _DEBUG
//#define new DEBUG_NEW
#ifdef _MSC_VER
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif  // _MSC_VER
#endif

/////////////////////////////////////////////////////////////////////////////
// CDataBranch

CDataTreeView::CDataBranch::CDataBranch(CGraphTreeView &view)
: IGraphTreeObject(view.GetTreeCtrl(), TVI_ROOT, TVI_LAST), m_pTreeView(&view)
{
  Ctrl().SetItemData(Handle(), (DWORD_PTR)this);
}

QString CDataTreeView::CDataBranch::Text() const
{
  return getStringTableEntry(IDS_TREE_DATA_STORAGE);
}

unsigned int CDataTreeView::CDataBranch::Icon() const
{
  return IDI_DATA_STORE;
}


/* NOT OBSOLETE */ void CDataTreeView::CDataBranch::AppendContextMenu(CContextMenuInvoker &invoker)
{
  CModelBase* pModel = dynamic_cast<CModelBase*>(m_pTreeView->Document()->Model());
  if(pModel)
    invoker.AddCommand( _T("&Import..."),*(new CWMCommandCommand(FemAppGetMainWnd()->m_hWnd,ID_FILE_IMPORT)));
}

/////////////////////////////////////////////////////////////////////////////
// CPointSetBranch

CDataTreeView::CPointSetBranch::CWellPathPointSetBranch::CWellPathPointSetBranch(CPointSetEntry &entry, CTreeCtrl &ctrl, HTREEITEM hInsertAfter)
:TPointSetEntryObserver(entry, ctrl, TRUE, FIXED_ITEM, hInsertAfter)
{
  Update();
}

BOOL CDataTreeView::CPointSetBranch::CWellPathPointSetBranch::OnFilter(const child_type &ps) const
{
  const CPointSet* cast_ps = dynamic_cast <const CPointSet*> (&ps);

  if (cast_ps && cast_ps->pointSetType() == IPointSet::INPUT)
  return TRUE;
  return FALSE;
}

QString CDataTreeView::CPointSetBranch::CWellPathPointSetBranch::Text() const 
{
  return QObject::tr("Wells");
}

void CDataTreeView::CPointSetBranch::CWellPathPointSetBranch::InsertDeviatedWellPathByPointSet()
{
  CGUIBase* pGUI = dynamic_cast <CGUIBase*> (GetGeomecDoc()->GUI());

  assert(pGUI);
  pGUI->InsertDeviatedWellPath(true);
  Update();
}

bool CDataTreeView::CPointSetBranch::CWellPathPointSetBranch::CanInsertNew() const
{
  const CModelBase* pModel = dynamic_cast <const CModelBase *> (GetGeomecDoc()->Model());
  return pModel && pModel->CanInsertNewWellPath();
}


namespace
{

const QString COPY_MESSAGE_MAIN = QObject::tr("Copy to all models");
const QString COPY_MESSAGE_ZOOM = QObject::tr("Copy all relevant wellpaths");

} // anonymous namespace

bool CDataTreeView::CPointSetBranch::CWellPathPointSetBranch::CanCopyAllWellPaths() const
{
  return ObservedChildren().size() > 0;
}

void CDataTreeView::CPointSetBranch::CWellPathPointSetBranch::CopyAllWellPaths()
{
  const CModelBase *model = &static_cast<const CModelBase *>(GetGeomecDoc()->Model())->RootModel();
  const CPointSetEntry *entry = static_cast<const CPointSetEntry *>(model->GraphEntry(MD_BASE_POINTSET));

  const_cast<CPointSetEntry *>(entry)->CopyAllWellpaths();
}

bool CDataTreeView::CPointSetBranch::CWellPathPointSetBranch::CanCopyRelevantWellPaths() const
{
  CModelBase *childModel = static_cast<CModelBase *>(GetGeomecDoc()->Model());
  return !childModel->CanCreateMesh() && ObservedChildren().size() > 0;
}

void CDataTreeView::CPointSetBranch::CWellPathPointSetBranch::CopyRelevantWellPaths()
{
  const CModelBase *childModel = static_cast<const CModelBase *>(GetGeomecDoc()->Model());

  assert(!const_cast<CModelBase *>(childModel)->CanCreateMesh());

  TGraphNodeSet inputs = ObservedChildren();

  for (TGraphNodeSet::iterator it = inputs.begin(); it != inputs.end(); ++it)
  CNewWellPathInput::CreateWellPath(static_cast<CNewWellPathInput *>(*it), *childModel);
}

void CDataTreeView::CPointSetBranch::CWellPathPointSetBranch::AppendContextMenu(CContextMenuInvoker &invoker)
{
  const CModelBase* model = dynamic_cast<const CModelBase *>(GetGeomecDoc()->Model());

  invoker.AddCommand(_T("Import deviated wellpath 3d &point set"),
  *(new CSingleCommandTemplate <CWellPathPointSetBranch> (*this,
      &CWellPathPointSetBranch::InsertDeviatedWellPathByPointSet,
      &CWellPathPointSetBranch::CanInsertNew)));
  invoker.AddCommand(COPY_MESSAGE_MAIN.toStdString(),
  *(new CSingleCommandTemplate <CWellPathPointSetBranch> (*this,
      &CWellPathPointSetBranch::CopyAllWellPaths,
      &CWellPathPointSetBranch::CanCopyAllWellPaths)));
  invoker.AddCommand(COPY_MESSAGE_ZOOM.toStdString(),
  *(new CSingleCommandTemplate <CWellPathPointSetBranch> (*this,
      &CWellPathPointSetBranch::CopyRelevantWellPaths,
      &CWellPathPointSetBranch::CanCopyRelevantWellPaths)));
}

CDataTreeView::CPointSetBranch::CPointSetSubBranch::CPointSetSubBranch
( CPointSetBranch &parent_observer
, const CPointSet::DIMENSION dim
, const CPointSet::TPointSetType psType
, const CString& strName
, const unsigned int uIcon
, HTREEITEM hInsertAfter
)
: TStateEnumerationBranch(parent_observer, strName, uIcon, hInsertAfter, FALSE), m_dim(dim), m_psType(psType)
{
  Update();
}

BOOL CDataTreeView::CPointSetBranch::CPointSetSubBranch::OnFilter(const child_type &ps) const
{
  // only display own mesh
  const CMeshBase* pMesh = dynamic_cast<const CMeshBase*>(&ps);
  if(pMesh && (&pMesh->Model() != GetGeomecDoc()->Model()))
  return FALSE;

  const CPointSet *cast_ps= dynamic_cast<const CPointSet *>(&ps);
  if (cast_ps && cast_ps->pointSetType() == IPointSet::INPUT)
  return FALSE;

  if (cast_ps && m_psType == IPointSet::POINTSET && cast_ps->pointSetType() == IPointSet::TIME_DEPTH && m_dim == ps.Dimension())
  return TRUE;

  return (m_psType == ps.pointSetType() && m_dim == ps.Dimension());
}

CTreeNode* CDataTreeView::CPointSetBranch::CPointSetSubBranch::InsertChild(child_type& child)
{
  CTreeNode* pTreeNode = CEnumerationBranch<IPointSet, TPointSetObserver, TRUE, DELETE_ITEM>::InsertChild(child);
  CModelBase& model = (CModelBase&)child.Model();
  if(&child != &model.Mesh())
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

void CDataTreeView::CPointSetBranch::CPointSetSubBranch::OnChildModified(CTreeNode &child)
{
  CPointSetBranch& parent = dynamic_cast<CPointSetBranch&>(*Parent());
  const IPointSet* pPointSet = dynamic_cast<const IPointSet*>(&child.ObservedItem());
  assert(pPointSet);
  if(pPointSet->Dimension() != m_dim)
    parent.Update();
  else
    CEnumerationBranch<IPointSet, TPointSetObserver, TRUE, DELETE_ITEM>::OnChildModified(child);
}

DROPEFFECT CDataTreeView::CPointSetBranch::CPointSetSubBranch::CanDrop(TCtrlObjectVec &vcDragged, BOOL bMove) const
{
  if (m_dim == IPointSet::DIM_3D && vcDragged.size() == 1)
  {
    CTreeNode *pTreeNode = dynamic_cast<CTreeNode*>(vcDragged[0]);

  if (pTreeNode)
  {
      CPointSet *pPointSet = dynamic_cast<CPointSet *>(&pTreeNode->ObservedItem());

      if (pPointSet && pPointSet->Dimension() == IPointSet::DIM_3D)
      {
    switch (m_psType)
    {
    case CPointSet::WELLPATH:
          if (pPointSet->pointSetType() == IPointSet::POINTSET)
      return DROPEFFECT_COPY;
          break;
    case CPointSet::POINTSET:
          if (pPointSet->pointSetType() == IPointSet::WELLPATH)
      return DROPEFFECT_COPY;
          break;
    default:
          break;
    }
      }
  }
  }

  return DROPEFFECT_NONE;
}

void CDataTreeView::CPointSetBranch::CPointSetSubBranch::Drop(TCtrlObjectVec &vcDragged, BOOL bMove)
{
  assert(CanDrop(vcDragged, bMove));

  CTreeNode *pTreeNode = static_cast<CTreeNode*>(vcDragged[0]);
  CPointSet *pPointSet = static_cast<CPointSet *>(&pTreeNode->ObservedItem());

  pPointSet->pointSetType(m_psType, true);

  pTreeNode->Parent()->Update();
  Update();
}


CDataTreeView::CPointSetBranch::CPointSetBranch
(TPointSetEntry& node
, CModelBase& model
, CTreeCtrl& ctrl
, HTREEITEM hParent
, HTREEITEM hInsertAfter
)
:TPointSetBranchBase(node, ctrl, FALSE, FIXED_ITEM, hParent, hInsertAfter), m_model(model)
{
  new CPointSetSubBranch(*this,
               CPointSet::DIM_3D,
               CPointSet::WELLPATH,
               _T("Wellpath"),
               IDI_POINTSET);

  new CPointSetSubBranch(*this,
               CPointSet::DIM_2D,
               CPointSet::POINTSET,
               _T("2D"),
               IDI_POINTSET);

  new CPointSetSubBranch(*this,
                CPointSet::DIM_3D,
        CPointSet::POINTSET,
                _T("3D"),
                IDI_POINTSET);
}

void CDataTreeView::CPointSetBranch::CreatePointSet()
{
  CPointSetCreateDlg dlg(m_model);
  dlg.DoModal();
}

void CreateHullCallback()
{
  GetGeomecDoc()->GetDataTreeView()->GetTreeCtrl().UpdateTree();
}

/* NOT OBSOLETE */ void CDataTreeView::CPointSetBranch::AppendContextMenu(CContextMenuInvoker &invoker)
{
  static_cast<CPointSetEntry *>(GetGeomecDoc()->Model()->GraphEntry(MD_BASE_POINTSET))->SetCreateHullCallback(CreateHullCallback);
  
  typedef CSingleCommandTemplate<CPointSetBranch> TPointSetBranchCommand;
  invoker.AddCommand( _T("&Import..."),*(new CWMCommandCommand(FemAppGetMainWnd()->m_hWnd,ID_FILE_IMPORT)));
  invoker.AddSeparator();
  invoker.AddCommand( _T("&Create point set ..."),*(new TPointSetBranchCommand(*this, &CPointSetBranch::CreatePointSet)));
  typedef CSingleCommandTemplate<CPointSetEntry> TPointSetEntryCommand;
  invoker.AddCommand(_T("Generate all convex &hulls"), *new TPointSetEntryCommand(m_node, &CPointSetEntry::CreateAllHulls, &CPointSetEntry::CanCreateAllHulls));
  invoker.AddCommand(_T("Delete all unused pointsets"), *new TPointSetEntryCommand(m_node, &CPointSetEntry::DeleteAllUnusedPointsets, &CPointSetEntry::CanDeleteAllUnusedPointsets));
}


/////////////////////////////////////////////////////////////////////////////
// CPointSetObserver

CDataTreeView::CPointSetBranch::CPropertyEnumerator::CPropertyEnumerator(TPointSetObserver &parent_observer, 
                                     const CString& strName,
                                     const unsigned int uIcon,
                                     HTREEITEM hInsertAfter)
: TPropertyEnumerator(parent_observer, strName, uIcon, hInsertAfter)
{
  Update();
}

BOOL CDataTreeView::CPointSetBranch::CPropertyEnumerator::OnFilter(const child_type& t) const
{	
  const CPointSet* pPointSet = dynamic_cast<const CPointSet*>(&ObservedItem());
  if(pPointSet)
    return &pPointSet->Coordinates() != &t;
  return true;
}

/////////////////////////////////////////////////////////////////////////////
// CDataTreeView

IMPLEMENT_DYNCREATE(CDataTreeView, CTreeViewBase)

//##ModelId=3B653D8E037C
CDataTreeView::CDataTreeView()
{
}

//##ModelId=3B653D8E03CB
CDataTreeView::~CDataTreeView()
{

}


BEGIN_MESSAGE_MAP(CDataTreeView, CTreeViewBase)
  //{{AFX_MSG_MAP(CDataTreeView)
  ON_WM_CREATE()
  ON_WM_SIZE()
  ON_COMMAND(ID_EDIT_DELETE, OnEditDelete)
  //}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDataTreeView drawing

//##ModelId=3B653D8E03B0
void CDataTreeView::OnDraw(CDC* pDC)
{
  CDocument* pDoc = GetDocument();
 	// TODO: add draw code here
}

/////////////////////////////////////////////////////////////////////////////
// CDataTreeView diagnostics

#ifdef _DEBUG
//##ModelId=3B653D8E03CD
void CDataTreeView::AssertValid() const
{
  CTreeViewBase::AssertValid();
}

//##ModelId=3B653D8E03DB
void CDataTreeView::Dump(CDumpContext& dc) const
{
  CTreeViewBase::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CDataTreeView message handlers
 
//##ModelId=3B653D8E03AE
void CDataTreeView::OnInitialUpdate() 
{
  CGraphTreeView::OnInitialUpdate();
  
  DeleteTree();

  CDataBranch *pBranch = new CDataBranch(*this);
  CModelBase* pModel = dynamic_cast<CModelBase*>(Document()->Model());
  if(pModel) 
  {
  // Use the root of a child model
  pModel = &pModel->RootModel();
    
    // waij TFS 92430
    // Insert Well Pointset ... 
    if(pModel->GraphEntry(MD_BASE_POINTSET))
      AddRoot(*(new CPointSetBranch::CWellPathPointSetBranch((CPointSetEntry&)(*pModel->GraphEntry(MD_BASE_POINTSET)), GetTreeCtrl(), pBranch->Handle())));

    
    // Insert surface root ...
    if( pModel->GraphEntry(MD_BASE_SURFACE) ) 
      AddRoot(*(new TSurfaceEntryObserver((CSurfaceEntry&)(*pModel->GraphEntry(MD_BASE_SURFACE)), GetTreeCtrl(), TRUE, FIXED_ITEM, pBranch->Handle())));

    // Insert 2D / 3D root ...
    if(pModel->GraphEntry(MD_BASE_POINTSET))
      AddRoot(*(new CPointSetBranch((TPointSetEntry&)(*pModel->GraphEntry(MD_BASE_POINTSET)), *pModel, GetTreeCtrl(), pBranch->Handle())));

    // Insert materials 
    if(pModel->GraphEntry(MD_ROCK_MATERIAL))
      AddRoot(*(new TMaterialEntryObserver((CMaterialEntry&)(*pModel->GraphEntry(MD_ROCK_MATERIAL)), GetTreeCtrl(), TRUE, FIXED_ITEM, pBranch->Handle())));

  if(pModel->GraphEntry(MD_BASE_INTERFACEMATERIAL))
      AddRoot(*(new TInterfaceMaterialEntryObserver((CInterfaceMaterialEntry&)(*pModel->GraphEntry(MD_BASE_INTERFACEMATERIAL)), GetTreeCtrl(), TRUE, FIXED_ITEM, pBranch->Handle())));

  if(pModel->GraphEntry(MD_WELLCASING_STEELMATERIAL_ENTRY))
      AddRoot(*(new TWellCasingSteelMaterialEntryObserver((CWellCasingSteelMaterialEntry&)(*pModel->GraphEntry(MD_WELLCASING_STEELMATERIAL_ENTRY)), GetTreeCtrl(), TRUE, FIXED_ITEM, pBranch->Handle())));
/*
    CGeomecApp *pApp = (CGeomecApp*)AfxGetApp();
    if(pApp->KeyFile().DCasintUnlocked())
    {
      if(pModel->GraphEntry(MD_CASING_GRADE))
        AddRoot(*(new TMaterialEntryObserver((CMaterialEntry&)(*pModel->GraphEntry(MD_CASING_GRADE)), GetTreeCtrl(), TRUE, FIXED_ITEM, pBranch->Handle())));
    }
*/
    
  }
}

//##ModelId=3B653D8E03BD
void CDataTreeView::OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint) 
{
 	switch(lHint)
  {
  case NEW_MODEL:
    DeleteTree();
    OnInitialUpdate();
    break;
  case CURRENT_SCENE_MODIFIED:
  if (pSender != this)
  {
      GetTreeCtrl().UnSelectStateless();
  }
  case NEW_CURRENT_SCENE:
  case SWITCH_TO_OPENINVENTOR:
    GetTreeCtrl().UpdateTree();
    break;
  default:
    assert(TRUE);		// Impossible
    break;
  }

}

void CDataTreeView::OnSelect(ITreeObject &tree_object)
{
  //if(!tree_object.DisplayObject())
  //	return;
  CGeomecDoc& doc = (CGeomecDoc&)(*Document());
  

  BOOL bAccepted = FALSE;

  // Try to view component and value type with the OIV viewer
  TValueComponentObserver *pComponentObserver = dynamic_cast<TValueComponentObserver*>(&tree_object);
  CValueCompositeObserver *pCompositeObserver = dynamic_cast<CValueCompositeObserver*>(&tree_object);
  if((pComponentObserver || pCompositeObserver))
  {

    if(!doc.IsDeleting())
    {
      assert (doc.Model());
      CFemAppModel* pModel = dynamic_cast<CFemAppModel*>(doc.Model());
      CFemAppGUI* pGUI = dynamic_cast <CFemAppGUI*> (doc.GUI());
      if(pModel && pGUI)
      {
        CFemAppModel& model = *pModel;
        CFemAppGUI& gui = *pGUI;

        ISceneWrapper* pCurrent = doc.CurrentScene();
        COpenInventorSceneWrapper * pOIVSceneWrapper = dynamic_cast<COpenInventorSceneWrapper*> (pCurrent);

        if (pOIVSceneWrapper != 0)
        {
          if(pComponentObserver)
          {
            // if (pOIVSceneWrapper->ValueComponent() != &pComponentObserver->ObservedItem())
            bAccepted = pOIVSceneWrapper->ViewComponent(*pComponentObserver);
          }
          else
          {
            IValueComposite& composite = dynamic_cast<IValueComposite&>(pCompositeObserver->ObservedItem());
            // if(pOIVSceneWrapper->ValueComponent() != &composite.Component())
            bAccepted = pOIVSceneWrapper->ViewComposite(*pCompositeObserver);
          }
        }
      }
    }
  }
  if (!bAccepted) // otherwise, this action is redundant and potentially harmful.
    CGraphTreeView::OnSelect(tree_object);

  doc.UpdateAllViews(this, CURRENT_SCENE_MODIFIED, 0);
}

