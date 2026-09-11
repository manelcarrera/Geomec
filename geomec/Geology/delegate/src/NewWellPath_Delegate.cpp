#include "stdafx.h"

#include "Geomec.h"
#include "NewWellPath_Delegate.h"
#include "NewWellPath.h"
#include "NewWellPathInput.h"
#include "ContextMenuInvoker.h"
#include "GUIBase.h"
#include "ResultExportDlg.h"
#include "AttriNewWellPathDlg.h"
#include "GeomecDoc.h"
#include "BoundaryBase.h"
#include "3DGUI.h"
#include "3DModel.h"
#include "fistexportdlg.h"
#include "fistpointpickDlg.h"
#include "TNOFileDialog.h"
#include "DsbPickPointsDlg.h"
#include "ExportIDMStabor.h"
#include "OpenInventorSceneNode.h"
#include "FemAppMainWindow.h"
#include "GlobalMessage.h"
#include "TreeNode.h"
#include "GroupNode.h"
#include "GroupNode_Delegate.h"
#include "NewWellPathGroupNodeObserver.h"


CNewWellPath_Delegate::CNewWellPath_Delegate(CNewWellPath* newWellPath)
: CColorNode_Delegate(newWellPath)
, m_pNewWellPath(newWellPath)
{
}

void CNewWellPath_Delegate::AppendContextMenu(CContextMenuInvoker& invoker)
{
  typedef CSingleCommandTemplate<CNewWellPath_Delegate>
  TNewWellPath_DelegateCommand;
  invoker.AddCommand(_T("Show wellpath information"),
  *(new CSingleCommandTemplate<CNewWellPath>(*m_pNewWellPath,
  &CNewWellPath::ToggleDrawWellInfo, 0, &CNewWellPath::DrawWellInfo)));
  invoker.AddSeparator();

  if (m_pNewWellPath->IsVertical())
  {
  invoker.AddCommand(_T("Change &Position"),
      *(new TNewWellPath_DelegateCommand(*this, &CNewWellPath_Delegate::Edit)));
  invoker.AddSeparator();
  }

  invoker.AddCommand(_T("&Flip Depth"), *(new TNewWellPath_DelegateCommand(*this, &CNewWellPath_Delegate::FlipDepth)));
  invoker.AddSeparator();

  // wjrx mantis 3647 3680
  //
  invoker.AddCommand(_T("Generate IDM input"),
  *(new TNewWellPath_DelegateCommand(*this,
  &CNewWellPath_Delegate::GenerateIDMInput)));

  invoker.AddSeparator();
  // wjrx mantis 3564
  //
  invoker.AddCommand(_T("Select points for &FIST export"),
  *(new TNewWellPath_DelegateCommand(*this,
      &CNewWellPath_Delegate::DefineFistPoints,
      &CNewWellPath_Delegate::CanDefineFistPoints)));
  invoker.AddCommand(_T("&Export to FIST"),
  *(new TNewWellPath_DelegateCommand(*this,
          &CNewWellPath_Delegate::ExportToFist,
    &CNewWellPath_Delegate::CanExportToFist)));
  
  invoker.AddCommand(_T("Create zoom-in model"),
  *new TNewWellPath_DelegateCommand(*this,
    &CNewWellPath_Delegate::CreateZoomInModel,
    &CNewWellPath_Delegate::CanZoomIn));

  invoker.AddCommand(_T("Export Log ASCII Standard (LAS) file"),
  *new TNewWellPath_DelegateCommand(*this, &CNewWellPath_Delegate::ExportLAS,
  &CNewWellPath_Delegate::CanExportLAS));

  invoker.AddCommand(_T("Import Log ASCII Standard (LAS) file"),
  *new TNewWellPath_DelegateCommand(*this, &CNewWellPath_Delegate::ImportLAS,
      &CNewWellPath_Delegate::CanImportLAS));

  invoker.AddSeparator();
  CColorNode_Delegate::AppendContextMenu(invoker);
}

bool CNewWellPath_Delegate::Edit()
{
  CNewWellPathEntry* pEntry =
  (CNewWellPathEntry*) (m_pNewWellPath->Model().GraphEntry(MD_NEW_WELLPATH));
  CNewWellPathEntry_Delegate* pEntry_Delegate =
  static_cast <CNewWellPathEntry_Delegate*> (pEntry->getDelegate());

  pEntry_Delegate->EditVerticalWellPath(m_pNewWellPath);

  return true;
}

void CNewWellPath_Delegate::FlipDepth()
{
  const_cast<CNewWellPathInput *>(m_pNewWellPath->WellPathInput())->FlipDepth();
}

/// \brief Enable zoom-in menu item
bool CNewWellPath_Delegate::CanZoomIn() const
{
  return m_pNewWellPath->CanZoomIn();
}

/// \brief Create Zoom-in model
void CNewWellPath_Delegate::CreateZoomInModel()
{
  m_pNewWellPath->CreateZoomInModel();
}

void CNewWellPath_Delegate::ExportLAS()
{
  TResultExportConfiguration resultExportConfiguration(false, true);

  resultExportConfiguration.enableNewWellPathSelection = true;
  resultExportConfiguration.selectedNewWellPath = m_pNewWellPath;

  CModelBase& model = static_cast <CModelBase&> (m_pNewWellPath->Model());
  CResultExportDlg dlg(model, resultExportConfiguration);

  dlg.SelectAllNewWellpaths(true);
  dlg.DoModal();
}

bool CNewWellPath_Delegate::CanExportLAS() const
{
  return true;
}

bool CNewWellPath_Delegate::CanImportLAS() const
{
  return m_pNewWellPath->CanImportLAS();
}


void CNewWellPath_Delegate::ImportLAS()
{
  QString sFilter =
  "Log ASCII Standard files (*.las)|*.las|All Files (*.*)|*.*||";
  CTnoFileDialog dlg(true, "las", 0, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
  sFilter.toStdString().c_str(), FemAppGetMainWnd());

  if (dlg.DoModal() == IDOK)
  {
  m_pNewWellPath->ImportLAS((LPCSTR) dlg.GetPathName());
  }
}

bool CNewWellPath_Delegate::Attributes()
{
  CAttriNewWellPathDlg dlg(*const_cast<CNewWellPathInput *>(m_pNewWellPath->WellPathInput()));

  return dlg.DoModal() == IDOK;
}

CNewWellPathEntry_Delegate::CNewWellPathEntry_Delegate(
  CNewWellPathEntry* newWellPathEntry)
: CStorageNodeEntry_Delegate <CNewWellPath> (newWellPathEntry)
, m_newWellPathEntry(newWellPathEntry)
{
}

bool CNewWellPath_Delegate::CanDefineFistPoints() const
{
# ifdef FIST_KEYFILE
  const CGeomecApp* pApp = dynamic_cast <const CGeomecApp*> (AfxGetApp());

  assert(pApp);

  if (pApp->KeyFile().Unlocked() == false ) return false;
  return true;
#else
  return true;
# endif
}

void CNewWellPath_Delegate::DefineFistPoints()
{
  CModelBase* pModel = dynamic_cast <CModelBase*> (&m_pNewWellPath->Model());

  if (!pModel->IsMesh())
  {
  if (pModel->CanCreateMesh())
  {
      pModel->CreateMesh();
  }
  }

  if (!pModel->IsMesh())
  {
  _m()->msg("First create a mesh");
  return;
  }

  CUnitNode unitNode;
  std::list<std::list<INewWellSection *> >Sections;
  Sections.push_back(m_pNewWellPath->FormationSectionPointers());

  CFistPointPickDlg dlg(&m_pNewWellPath->FistExportPoints(),
  *m_pNewWellPath, Sections, unitNode.Unit());

  dlg.WellPointListCtrl().ShowTMDOnly();
  dlg.DoModal();

  m_pNewWellPath->Modified();
}

/// \brief Enable Export to Fist menu entry, partly based on Keyfile
bool CNewWellPath_Delegate::CanExportToFist() const
{
# ifdef FIST_KEYFILE
  const CGeomecApp* pApp = dynamic_cast <const CGeomecApp*> (AfxGetApp());

  assert(pApp);

  if (pApp->KeyFile().Unlocked() == false ) return false;
  return m_pNewWellPath->CanExportToFist();
#else
  return m_pNewWellPath->CanExportToFist();
# endif
}

void CNewWellPath_Delegate::ExportToFist()
{ 
  CFistExportDlg
  dlg(m_pNewWellPath,(const CModelBase*)(&m_pNewWellPath->Model()));

  dlg.DoModal();
}

//----
void CNewWellPath_Delegate::GenerateIDMInput()
{
  CModelBase* pModel = dynamic_cast <CModelBase*> (&m_pNewWellPath->Model());

  if (!pModel->IsMesh())
  {
  if (pModel->CanCreateMesh())
  {
      pModel->CreateMesh();
  }
  }

  if (!pModel->IsMesh())
  {
  _m()->msg("First create a mesh");
  return;
  }

  CUnitNode unitNode;
  std::list<std::list<INewWellSection *> >Sections;

  Sections.push_back(m_pNewWellPath->FormationSectionPointers());

  CDsbPickPointsDlg dlg(&m_pNewWellPath->DStaborPoints(),
  *m_pNewWellPath, Sections, unitNode.Unit());

  dlg.WellPointListCtrl().ShowTMDOnly();
  if ( dlg.DoModal() == IDOK
      && m_pNewWellPath->DStaborPoints().empty() == false )
  {
  // wjrx mantis 3680
  CStartIDMStaborDlg
      StartDlg(&m_pNewWellPath->DStaborPoints(),*pModel);
  StartDlg.DoModal();
  }

  //--- wjrx mantis 3647: display of dstabor points not supported
  //--- here is the code for the old wellpath:
  //
  // m_wellPath->UpdateDsbAnalysisPointsInScene(); //wjrx mantis 2542
  // m_wellPath->DisplayList().ClearAndDelete();
  // m_wellPath->Modified();
}
//----

void CNewWellPathEntry_Delegate::AppendContextMenu(CContextMenuInvoker &invoker)
{
  invoker.AddCommand(_T("Show wellpath information"),
  *(new CSingleCommandTemplate<CNewWellPathEntry>(*m_newWellPathEntry,
  &CNewWellPathEntry::ToggleDrawWellInfo, 0, &CNewWellPathEntry::DrawWellInfo)));
  invoker.AddSeparator();

  invoker.AddCommand(_T("Import deviated wellpath 3d &point set"),
  *(new CSingleCommandTemplate <CNewWellPathEntry_Delegate> (*this,
      &CNewWellPathEntry_Delegate::InsertDeviatedWellPathByPointSet,
      &CNewWellPathEntry_Delegate::CanInsertNew))); //wjrx mantis 3185

#if 1
  // The vertical wellpath // wjrx mantis 3549
  CContextMenuInvoker* pSubMenu = new CContextMenuInvoker;


  pSubMenu->AddCommand(_T("&Graphically"),
  *(new CSingleCommandTemplate <CNewWellPathEntry_Delegate> (*this,
      &CNewWellPathEntry_Delegate::InsertVerticalWellPathGraphical,
      &CNewWellPathEntry_Delegate::CanInsertNew)));
  pSubMenu->AddCommand(_T("&By value"),
  *(new CSingleCommandTemplate <CNewWellPathEntry_Delegate> (*this,
      &CNewWellPathEntry_Delegate::InsertVerticalWellPathByValue,
      &CNewWellPathEntry_Delegate::CanInsertNew)));
  invoker.AddInvoker("Insert &vertical wellpath", *pSubMenu);
#else
  invoker.AddCommand(_T("&Insert vertical wellpath"),
      *(new CSingleCommandTemplate <CNewWellPathEntry_Delegate> (*this,
      &CNewWellPathEntry_Delegate::InsertVerticalWellPathByValue,
      &CNewWellPathEntry_Delegate::CanInsertNew)));
#endif

  invoker.AddCommand(_T("&Export"),
  *new CSingleCommandTemplate <CNewWellPathEntry_Delegate> (*this,
      &CNewWellPathEntry_Delegate::Export,
      &CNewWellPathEntry_Delegate::CanExport));

  invoker.AddCommand(_T("Export to &FIST"),
  *(new CSingleCommandTemplate <CNewWellPathEntry_Delegate> (*this,
      &CNewWellPathEntry_Delegate::ExportToFist,
      &CNewWellPathEntry_Delegate::CanExportToFist)));

  invoker.AddSeparator();

  invoker.AddCommand(_T("Add group"),
  *(new CSingleCommandTemplate<CNewWellPathEntry_Delegate>(*this,
      &CNewWellPathEntry_Delegate::InsertGroup)));
}

void CNewWellPathEntry_Delegate::InsertGroup()
{
  if (TreeNode())
  {
  CGroupNodeEntry *entry = static_cast<CGroupNodeEntry *>(m_newWellPathEntry->Model().GraphEntry(MD_BASE_GROUPNODE));
  // TODO: use CGraphNode::createUniqueName, but that one crashes
  QString name("New Group");
  CGroupNode *groupNode = new CGroupNode(name, m_newWellPathEntry->Model(), m_newWellPathEntry);
  new TNewWellPathGroupNodeObserver(*groupNode, *TreeNode());
  }
}

bool CNewWellPathEntry_Delegate::CanExportToFist() const
{
  return m_newWellPathEntry->CanExportToFist();
}

void CNewWellPathEntry_Delegate::ExportToFist()
{
  CFistExportDlg
  dlg(true,(const CModelBase*)(&m_newWellPathEntry->Model()));

  dlg.DoModal();
}

bool CNewWellPathEntry_Delegate::CanInsertNew() const
{
  return m_newWellPathEntry->CanInsertNew();
}

// wjrx mantis 3185

void CNewWellPathEntry_Delegate::InsertDeviatedWellPathByPointSet()
{
  CGUIBase* pGUI = dynamic_cast <CGUIBase*> (GetGeomecDoc()->GUI());

  assert(pGUI);
  pGUI->InsertDeviatedWellPath(true);
}

// wjrx mantis 3549
//
void CNewWellPathEntry_Delegate::InsertVerticalWellPathByValue()
{
  CModelBase& model = (CModelBase&) (m_newWellPathEntry->Model());

  if (!model.IsMesh())
  {
  _m()->msg("First create a mesh",0,0);
  return;
  }

  // Just create a well in the middle of the model

  geo::CPoint point(
  (model.Boundary().Max().X() + model.Boundary().Min().X()) / 2,
  (model.Boundary().Max().Y() + model.Boundary().Min().Y()) / 2);

  // Create unique name

  QString sName = m_newWellPathEntry->createUniqueName(
  CNewWellPathEntry::DEFAULT_NEWWELL_NAME,
  CNewWellPathEntry::DEFAULT_INITIAL_NUMBER);

  CNewWellPathInput *input = new CNewWellPathInput(sName.toStdString().c_str(), model, point);

  CAttriNewWellPathDlg dlg(*input);

  if (dlg.DoModal() == IDCANCEL)
  {
  input->Destroy();
  input = 0;
  }
  else
  {
  std::vector<std::string> messages;

  const CNewWellPath *pWellPath = CNewWellPathInput::CreateWellPath(input, model, &messages);

  if (messages.size())
      _m()->msg(messages);
  }
}

void CNewWellPathEntry_Delegate::InsertVerticalWellPathGraphical()
{
  CModelBase& model = (CModelBase&) (m_newWellPathEntry->Model());

  if (!model.IsMesh())
  {
  _m()->msg("First create a mesh", 0, 0);

  return;
  }

  EditVerticalWellPath();
}

void CNewWellPathEntry_Delegate::EditVerticalWellPath(CNewWellPath* pWellPath)
{
  C3DModel& model = (C3DModel&) (m_newWellPathEntry->Model());
  if (!model.IsMesh())
  {
    _m()->msg("First create a mesh", 0, 0);
    return;
  }

  CGeomecDoc* pDoc = GetGeomecDoc();
  C3DGUI& guiModel = (C3DGUI&) *(dynamic_cast <C3DGUI*> (pDoc->GUI()));
  COpenInventorSceneNode& oivNode = guiModel.OpenInventorScene();
  pDoc->CurrentScene(&oivNode);
  oivNode.EditVerticalNewWellPath (pWellPath);
}
//===

bool CNewWellPathEntry_Delegate::CanExport() const
{
  return m_newWellPathEntry->CanExport();
}

void CNewWellPathEntry_Delegate::Export()
{
  CModelBase& model = static_cast <CModelBase&> (m_newWellPathEntry->Model());
  CResultExportDlg dlg(model);

  dlg.SelectAllNewWellpaths(true);
  dlg.DoModal();
}
