#include "stdafx.h"

#include "GoCadModel_Delegate.h"
#include "GoCadModel.h"
#include "ContextMenuInvoker.h"
#include "TNOFileDialog.h"
#include "AttriHorizon.h"
#include "SelectGocadSolidDlg.h"
#include "resourceIDS.h"
#include "GeomecStringTable.h"
#include "TetraSubHorizon.h"

#include <QFileDialog>
#include <QtWidgets\QMessageBox>

CGoCadModel_Delegate::CGoCadModel_Delegate(CGoCadModel* aGoCadModel)
: CTetraModel_Delegate(aGoCadModel),
  m_GoCadModel(aGoCadModel)
{
}


/////

CGoCadSubModelEntry_Delegate::CGoCadSubModelEntry_Delegate(
  CGoCadSubModelEntry* goCadSubModelEntry)
: TSubModelEntry_Delegate(goCadSubModelEntry)
, m_goCadSubModelEntry(goCadSubModelEntry)
{
}

void CGoCadSubModelEntry_Delegate::AppendContextMenu(
  CContextMenuInvoker &invoker)
{
  invoker.AddCommand(_T("Import &Mesh"),
  *(new CGoCadSubModelEntry_Delegate::TEntryCommand(*this,
      &CGoCadSubModelEntry_Delegate::ImportMesh,
      &CGoCadSubModelEntry_Delegate::CanImportMesh)));
  invoker.AddSeparator();
  invoker.AddCommand(_T("Insert &Horizon"),
  *(new CGoCadSubModelEntry_Delegate::TEntryCommand(*this,
      &CGoCadSubModelEntry_Delegate::InsertHorizon,
      &CGoCadSubModelEntry_Delegate::CanInsertSurfaceContainer)));

#ifdef TETRA_FAULTS_KEYFILE
  const CKeyFile& keyfile = (dynamic_cast <const CGeomecApp*> (AfxGetApp()))->KeyFile();

  if (keyfile.Unlocked())
  {
#endif
  invoker.AddCommand(_T("Insert &Fault"),
      *(new CGoCadSubModelEntry_Delegate::TEntryCommand(*this,
    &CGoCadSubModelEntry_Delegate::InsertFault,
    &CGoCadSubModelEntry_Delegate::CanInsertSurfaceContainer)));
#ifdef TETRA_FAULTS_KEYFILE
  }
#endif
}

bool CGoCadSubModelEntry_Delegate::CanImportMesh() const
{
  return dynamic_cast <const CGoCadModel*> (
  &m_goCadSubModelEntry->Model())->CanImportMesh();
}

namespace
{

#ifdef SKUA_NEW
bool selectGocadSolidDlg(
  const std::vector<const gm_skua::SKUAParseData *>& solids,
  std::vector<int>& vcSolids)
#else
  bool selectGocadSolidDlg(
  const std::vector <QSharedPointer <CGocadData::CTSolid> >& solids,
  std::vector<int>& vcSolids)
#endif
{
  CSelectGocadSolidDlg dlg(solids);

  if (dlg.DoModal() != IDOK)
  {
  return false;
  }

  vcSolids.clear();
  for(int i = 0; i < dlg.SelectedSize(); ++i)
  vcSolids.push_back(dlg.SelectedEntry(i));

  return true;
}

} // anonymous namespace

void CGoCadSubModelEntry_Delegate::ImportMesh()
{
  QString sFilter;

  sFilter = getStringTableEntry(IDS_MESH_IMPORT_FILTER);

  CTnoFileDialog dlg(TRUE, NULL, NULL, OFN_ALLOWMULTISELECT,
  sFilter.toStdString().c_str());

  if (dlg.DoModal() == IDOK)
  {
  static_cast <CGoCadModel*> (
      &m_goCadSubModelEntry->Model())->ImportMesh((LPCSTR) dlg.GetPathName(),
    selectGocadSolidDlg);
  }
}

void CGoCadSubModelEntry_Delegate::ImportMesh(const QString& fileName)
{
  static_cast <CGoCadModel*>(&m_goCadSubModelEntry->Model())->ImportMesh(fileName, selectGocadSolidDlg);
}

bool CGoCadSubModelEntry_Delegate::CanInsertSurfaceContainer() const
{
  return m_goCadSubModelEntry->CanInsertSurfaceContainer();
}

void InsertHorizon_Generic( CFemAppModel& model, bool bSlip )
{
  CTetraSubHorizon* pHorizon =
  new CTetraSubHorizon( bSlip ? "New Fault " : "New Horizon", bSlip, model, FALSE /*attach to entry*/);

  typedef CAttriHorizon <CTetraSubHorizon> THorizonDlg;

  THorizonDlg dlg( bSlip ? IDD_ATTRI_FAULT : IDD_ATTRI_NON_SLIP_HORIZON, *pHorizon, false);

  if (dlg.DoModal() == IDCANCEL)
  {
  delete pHorizon;
  }
  else
  {
    pHorizon->LinkToEntry();
    pHorizon->AttachToEntry();
  }
}

void CGoCadSubModelEntry_Delegate::InsertFault()
{ 
  InsertHorizon_Generic( m_goCadSubModelEntry->Model(), TRUE );
}

void CGoCadSubModelEntry_Delegate::InsertHorizon()
{
  InsertHorizon_Generic( m_goCadSubModelEntry->Model(), FALSE );
}
