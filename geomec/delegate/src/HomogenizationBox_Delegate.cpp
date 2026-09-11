#include "stdafx.h"

#include "HomogenizationBox_Delegate.h"
#include "HomogenizationBox.h"
#include "homoboxattridlg.h"
#include "MeshBase.h"
#include "ElementSet_Delegate.h"
#include "GlobalMessage.h"
#include "ContextMenuInvoker.h"
#include "ZoomInCreator.h"
#include "GeoSurface.h"
#include "BaseEntryTypes.h"
#include "DianaXWrapper.h"
#include "SaveModel.h"
#include "SaveModelGUI.h"
#include "RetrieveDianaFileNames.h"
#include "RetrieveDianaFileNamesGUI.h"
#include "geoSurface_Delegate.h"

CHomogenizationBox_Delegate::CHomogenizationBox_Delegate(
  CHomogenizationBox* homogenizationBox)
: IScalingBox_Delegate(homogenizationBox)
, m_homogenizationBox(homogenizationBox)
{
}

bool CHomogenizationBox_Delegate::Attributes()
{
  if (!((CModelBase&) m_homogenizationBox->Model()).Mesh().IsMesh())
  {
  _m()->msg("Please make sure a valid mesh is created");
  return false;
  }

  CHomoBoxAttriDlg dlg(m_homogenizationBox->Model(), *m_homogenizationBox);

  dlg.DoModal();

  return true;
}

void CHomogenizationBox_Delegate::AppendContextMenu(
  CContextMenuInvoker& invoker)
{
  typedef CSingleCommandTemplate <CHomogenizationBox> THomoBoxCommand;
  typedef CSingleCommandTemplate <CHomogenizationBox_Delegate>
  THomoBoxCommand_Delegate;

  invoker.AddCommand(_T("Capture element set"),
  *(new THomoBoxCommand_Delegate(*this,
      &CHomogenizationBox_Delegate::CaptureElSet,
      &CHomogenizationBox_Delegate::CanCaptureElSet)));

#ifdef ZOOMIN_KEYFILE
  CGeomecApp* pApp = (CGeomecApp*) (AfxGetApp());

  if (pApp->KeyFile().Unlocked())
  {
#endif
  invoker.AddCommand(_T("Create zoom-in model"),
      *(new THomoBoxCommand_Delegate(*this,
    &CHomogenizationBox_Delegate::CreateZoomIn)));
  invoker.AddCommand(_T("Cut surfaces with this box"),
      *(new THomoBoxCommand_Delegate(*this,
    &CHomogenizationBox_Delegate::CutSurafceInDatastoreWithBox,
    &CHomogenizationBox_Delegate::CanCutSurafceInDatastoreWithBox)));
#ifdef ZOOMIN_KEYFILE
  }
#endif

  invoker.AddSeparator();
  invoker.AddCommand(_T("&Delete"),
  *(new THomoBoxCommand(*m_homogenizationBox,
      &CHomogenizationBox::Destroy)));

  CColorNode_Delegate::AppendContextMenu(invoker);
}

void CHomogenizationBox_Delegate::CaptureElSet()
{
  CModelBase& model =
  dynamic_cast <CModelBase&> (m_homogenizationBox->Model());
  CElementSet* pElementSet = m_homogenizationBox->CaptureElementSet(model);
  CElementSet_Delegate* pElementSet_Delegate =
  new CElementSet_Delegate(pElementSet);

  if (!pElementSet_Delegate->Attributes())
  {
  delete pElementSet_Delegate;
  delete pElementSet;
  }
}

bool CHomogenizationBox_Delegate::CanCaptureElSet() const
{
  return ((CModelBase&) (m_homogenizationBox->Model())).IsMesh();
}

bool CHomogenizationBox_Delegate::Destroy()
{
  return m_homogenizationBox->Destroy();
}

void CHomogenizationBox_Delegate::CreateZoomIn()
{
  CZoomInCreator zic(*m_homogenizationBox);

  zic.Create();
}

bool CHomogenizationBox_Delegate::CanCutSurafceInDatastoreWithBox() const
{
  return m_homogenizationBox->CanCutSurafceInDatastoreWithBox();
}

void CHomogenizationBox_Delegate::CutSurafceInDatastoreWithBox()
{
  const CModelBase& model =
  dynamic_cast <const CModelBase&> (m_homogenizationBox->Model());
  CSurfaceEntry* entry = ((CSurfaceEntry*) model.GraphEntry(MD_BASE_SURFACE));
  CSurfaceEntry_Delegate* entry_Delegate =
  dynamic_cast <CSurfaceEntry_Delegate*> (entry->getDelegate());

  entry_Delegate->CutSurfacesBB(m_homogenizationBox->Box());
}

CHomoBoxEntry_Delegate::CHomoBoxEntry_Delegate(CHomoBoxEntry* homoBoxEntry)
: CStorageNodeEntry_Delegate <CHomogenizationBox> (homoBoxEntry)
, m_homoBoxEntry(homoBoxEntry)
{
}

void CHomoBoxEntry_Delegate::AppendContextMenu(CContextMenuInvoker& invoker)
{
  typedef CSingleCommandTemplate <CHomoBoxEntry_Delegate>
  THomoBoxEntry_DelegateCommand;

  invoker.AddCommand(_T("New Scaling Box"),
  *(new THomoBoxEntry_DelegateCommand(*this,
      &CHomoBoxEntry_Delegate::NewBox))); 
}

namespace
{

const QString DEFAULT_SCALING_BOX_NAME = QObject::tr("Scaling box%1");

} // anonymous namespace

void CHomoBoxEntry_Delegate::NewBox()
{
  QString name = m_homoBoxEntry->createUniqueName(DEFAULT_SCALING_BOX_NAME,
  m_homoBoxEntry->GraphEntryNodes().size() + 1);
  CHomogenizationBox* pBox =
  new CHomogenizationBox(m_homoBoxEntry->Model(), name);
  CHomoBoxAttriDlg dlg(m_homoBoxEntry->Model(), *pBox);

  if (dlg.DoModal() == IDCANCEL)
  {
  delete pBox;
  }

  m_homoBoxEntry->Modified();
}
