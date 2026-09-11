#include "stdafx.h"

#include "geoSurface_Delegate.h"
#include "GeoSurface.h"
#include "AttriSurfaceDlg.h"
#include "ContextMenuInvoker.h"
#include "TNOFileDialog.h"
#include "CutSurfaceDlg.h"
#include "SelSurfDlg.h"
#include "GeomecDoc.h"

CSurfaceBase_Delegate::CSurfaceBase_Delegate(CSurfaceBase* surfaceBase)
: IElementSet_Delegate(surfaceBase)
, m_surfaceBase(surfaceBase)
{
}

bool CSurfaceBase_Delegate::Attributes()
{
  CAttriSurfaceDlg dlg(*m_surfaceBase);

  return dlg.DoModal() == IDOK;
}

void CSurfaceBase_Delegate::AppendContextMenu(CContextMenuInvoker &invoker)
{
  // Create the export menu ...

  typedef CSingleCommandTemplate <CSurfaceBase_Delegate> TSurfaceCommand;

  invoker.AddCommand(_T("&Export"),
    *(new TSurfaceCommand(*this, &CSurfaceBase_Delegate::ExportSurface)));
  invoker.AddSeparator();

  IElementSet_Delegate::AppendContextMenu(invoker);
}

bool CSurfaceBase_Delegate::CanDestroy() const
{
  return m_surfaceBase->CanDestroy();
}

void CSurfaceBase_Delegate::ExportSurface()
{
  QString sFileName = m_surfaceBase->Name();
  CTnoFileDialog dlg(FALSE, "ts", sFileName.toStdString().c_str(),
    OFN_OVERWRITEPROMPT, "GoCad surface (*.ts)|*.ts|ASCII Files (*.dat;*.txt)|"
    "*.dat;*.txt|Excel Files (*.xls;*.xls*)|*.xls;*.xls*|||");

  if (dlg.DoModal() == IDOK)
  {
    CFemAppDoc& doc = *GetGeomecDoc();
    QString sExt = dlg.GetPathName().Right(4);

    m_surfaceBase->ExportSurface(sExt, doc.UnitNode(),
      (LPCSTR) dlg.GetPathName());
  }
}

CSurfaceEntry_Delegate::CSurfaceEntry_Delegate(CSurfaceEntry* surfaceEntry)
: CStorageNodeEntry_Delegate <CSurfaceBase> (surfaceEntry)
, m_surfaceEntry(surfaceEntry)
{
}

void CSurfaceEntry_Delegate::AppendContextMenu(CContextMenuInvoker &invoker)
{
  typedef CSingleCommandTemplate <CSurfaceEntry> TEntryCommand;
  typedef CSingleCommandTemplate <CSurfaceEntry_Delegate>
    TEntry_DelegateCommand;

  invoker.AddCommand(_T("&Import..."),
    *(new TEntry_DelegateCommand(*this, &CSurfaceEntry_Delegate::Import,
      &CSurfaceEntry_Delegate::CanImport)));
  invoker.AddSeparator();
  invoker.AddCommand(_T("Export surfaces"),
    *(new TEntry_DelegateCommand(*this, &CSurfaceEntry_Delegate::ExportSurfaces,
      &CSurfaceEntry_Delegate::GetSurfaceCount)));
  invoker.AddCommand(_T("Delete unused surfaces"),
    *(new TEntryCommand(*m_surfaceEntry, &CSurfaceEntry::RemoveUnused,
      &CSurfaceEntry::NonUsedSurface)));
  invoker.AddCommand(_T("Unassign surfaces"),
    *(new TEntry_DelegateCommand(*this,
      &CSurfaceEntry_Delegate::DoSelectAndUnassignSurfaces,
      &CSurfaceEntry_Delegate::CanSelectAndUnassignSurfaces)));
  invoker.AddCommand(_T("Delete surfaces"),
    *(new TEntry_DelegateCommand(*this,
      &CSurfaceEntry_Delegate::DoSelectAndDeleteSurfaces,
      &CSurfaceEntry_Delegate::CanSelectAndDeleteSurfaces)));
  invoker.AddSeparator();
  invoker.AddCommand(_T("Create horizons"),
    *(new TEntry_DelegateCommand(*this,
      &CSurfaceEntry_Delegate::DoSelectAndCreateHorizons,
      &CSurfaceEntry_Delegate::NonUsedSurface)));
  invoker.AddCommand(_T("Create boundary sides"),
    *(new TEntry_DelegateCommand(*this,
      &CSurfaceEntry_Delegate::DoSelectAndCreateSubBoundarySides,
      &CSurfaceEntry_Delegate::CanSelectAndCreateSubBoundarySides)));
  invoker.AddCommand(_T("Auto-assign boundary top and bottom"),
    *(new TEntry_DelegateCommand(*this,
      &CSurfaceEntry_Delegate::DoCreateSubBoundaryTopAndBottom,
      &CSurfaceEntry_Delegate::CanCreateSubBoundaryTopAndBottom)));
  invoker.AddCommand(_T("Create faults"),
    *(new TEntry_DelegateCommand(*this,
      &CSurfaceEntry_Delegate::DoSelectAndCreateFaults,
      &CSurfaceEntry_Delegate::CanSelectAndCreateFaults)));
  invoker.AddSeparator();
  invoker.AddCommand(_T("Merge surfaces in faults"),
    *(new TEntry_DelegateCommand(*this,
      &CSurfaceEntry_Delegate::DoMergeFaultSurfaces,
      &CSurfaceEntry_Delegate::CanMergeFaultSurfaces)));
  invoker.AddCommand(_T("Merge surfaces"),
    *(new TEntry_DelegateCommand(*this,
      &CSurfaceEntry_Delegate::MergeSurfaces,
      &CSurfaceEntry_Delegate::GetSurfaceCount)));

#ifdef ZOOMIN_KEYFILE
  CGeomecApp* pApp = (CGeomecApp*) (AfxGetApp());

  if (pApp->KeyFile().Unlocked())
  {
#endif
    invoker.AddSeparator();
    invoker.AddCommand(_T("Cut surfaces"),
      *(new TEntry_DelegateCommand(*this, &CSurfaceEntry_Delegate::CutSurfaces,
        &CSurfaceEntry_Delegate::NonUsedSurface)));
#ifdef ZOOMIN_KEYFILE
  }
#endif
}

/// \brief menu entry will be enabled or not
bool CSurfaceEntry_Delegate::CanImport() const
{
	return GetGeomecDoc()->Model() != 0;
}

/// \brief Import (surface) files
void CSurfaceEntry_Delegate::Import()
{
	GetGeomecDoc()->OnImportSurface();
}

bool CSurfaceEntry_Delegate::GetSurfaceCount() const
{
  return m_surfaceEntry->GetSurfaceCount();
}

void CSurfaceEntry_Delegate::ExportSurfaces()
{
  CNamedSurfaces slected_surfaces;

  SelectSurfaces(slected_surfaces, NO_CHECK, "Select surfaces for export");

  if (slected_surfaces.size() == 0)
  {
    return;
  }

  CTnoFileDialog dlg(FALSE, "mx", "Surfaces.mx", OFN_OVERWRITEPROMPT,
    "GoCad surface (*.mx)|*.mx|All Files (*.*)|*.*||");

  if (dlg.DoModal() == IDOK)
  {
    CFemAppDoc& doc = *GetGeomecDoc();

    m_surfaceEntry->ExportSurfaces(slected_surfaces, (LPCSTR)dlg.GetPathName(), doc.UnitNode());
  }
}

bool CSurfaceEntry_Delegate::NonUsedSurface() const
{
  return m_surfaceEntry->NonUsedSurface();
}

void CSurfaceEntry_Delegate::DoSelectAndCreateHorizons()
{
  CNamedSurfaces slected_surfaces;

  SelectSurfaces(slected_surfaces, NOT_USED_IN_MODEL, "Select surfaces for new horizons");

  if (slected_surfaces.size() == 0)
  {
    return;
  }

  CGeomecDoc* pDoc = GetGeomecDoc();
  CModelBase* pModel = static_cast<CModelBase*>(pDoc->Model());
  assert(pModel);
  m_surfaceEntry->DoSelectAndCreateHorizons(slected_surfaces, *pModel);
}

bool CSurfaceEntry_Delegate::CanSelectAndCreateSubBoundarySides() const
{
  CGeomecDoc* pDoc = GetGeomecDoc();
  const CModelBase* pModel = static_cast<CModelBase*>(pDoc->Model());
  assert(pModel);
  return m_surfaceEntry->CanSelectAndCreateSubBoundarySides(*pModel);
}

void CSurfaceEntry_Delegate::DoSelectAndCreateSubBoundarySides()
{
  CNamedSurfaces slected_surfaces;

  SelectSurfaces(slected_surfaces, NOT_USED_IN_MODEL, "Select surfaces for new boundary sides");

  if (slected_surfaces.size() == 0)
  {
    return;
  }

  CGeomecDoc* pDoc = GetGeomecDoc();
  CModelBase* pModel = static_cast<CModelBase*>(pDoc->Model());
  assert(pModel);
  m_surfaceEntry->DoSelectAndCreateSubBoundarySides(slected_surfaces, *pModel);
}

bool CSurfaceEntry_Delegate::CanCreateSubBoundaryTopAndBottom() const
{
  CGeomecDoc* pDoc = GetGeomecDoc();
  const CModelBase* pModel = static_cast<CModelBase*>(pDoc->Model());
  assert(pModel);
  return m_surfaceEntry->CanCreateSubBoundaryTopAndBottom(*pModel);
}

void CSurfaceEntry_Delegate::DoCreateSubBoundaryTopAndBottom()
{
  CGeomecDoc* pDoc = GetGeomecDoc();
  CModelBase* pModel = static_cast<CModelBase*>(pDoc->Model());
  assert(pModel);
  return m_surfaceEntry->DoCreateSubBoundaryTopAndBottom(*pModel);
}

bool CSurfaceEntry_Delegate::CanSelectAndCreateFaults() const
{
  CGeomecDoc* pDoc = GetGeomecDoc();
  CModelBase* pModel = static_cast<CModelBase*>(pDoc->Model());
  assert(pModel);
  return m_surfaceEntry->CanSelectAndCreateFaults(*pModel);
}

void CSurfaceEntry_Delegate::DoSelectAndCreateFaults()
{
  CNamedSurfaces slected_surfaces;

  SelectSurfaces(slected_surfaces, NOT_USED_IN_MODEL, "Select surfaces for new faults");

  if (slected_surfaces.size() == 0)
  {
    return;
  }

  CGeomecDoc* pDoc = GetGeomecDoc();
  CModelBase* pModel = static_cast<CModelBase*>(pDoc->Model());
  assert(pModel);
  m_surfaceEntry->DoSelectAndCreateFaults(slected_surfaces, *pModel);
}

bool CSurfaceEntry_Delegate::CanSelectAndUnassignSurfaces() const
{
  return m_surfaceEntry->CanSelectAndUnassignSurfaces();
}

void CSurfaceEntry_Delegate::DoSelectAndUnassignSurfaces()
{
  CNamedSurfaces selected_surfaces;

  SelectSurfaces(selected_surfaces, USED_IN_MODEL, "Select surfaces for unassignment");

  if (selected_surfaces.size() == 0)
  {
    return;
  }

  m_surfaceEntry->DoSelectAndUnassignSurfaces(selected_surfaces);
}

bool CSurfaceEntry_Delegate::CanSelectAndDeleteSurfaces() const
{
  return m_surfaceEntry->CanSelectAndDeleteSurfaces();
}

void CSurfaceEntry_Delegate::DoSelectAndDeleteSurfaces()
{
  CNamedSurfaces selected_surfaces;

  SelectSurfaces(selected_surfaces, NOT_USED_IN_MODEL, "Select unassigned surfaces for deletion ***WARNING: deletion will be permanent***");

  if (selected_surfaces.size() == 0)
  {
    return;
  }

  m_surfaceEntry->DoSelectAndDeleteSurfaces(selected_surfaces);
}

bool CSurfaceEntry_Delegate::CanMergeFaultSurfaces() const
{
  CGeomecDoc* pDoc = GetGeomecDoc();
  const CModelBase* pModel = static_cast<CModelBase*>(pDoc->Model());
  assert(pModel);
  return m_surfaceEntry->CanMergeFaultSurfaces(*pModel);
}

void CSurfaceEntry_Delegate::DoMergeFaultSurfaces()
{
  CGeomecDoc* pDoc = GetGeomecDoc();
  CModelBase* pModel = static_cast<CModelBase*>(pDoc->Model());
  assert(pModel);
  m_surfaceEntry->DoMergeFaultSurfaces(*pModel);
}

void CSurfaceEntry_Delegate::CutSurfaces()
{
	DoCutSurfaces();
}

void CSurfaceEntry_Delegate::MergeSurfaces()
{
  CNamedSurfaces selected_surfaces;

  SelectSurfaces(selected_surfaces, NO_CHECK, "Select surfaces for merging");

  if (selected_surfaces.size() < 2)
  {
    return;
  }

  m_surfaceEntry->MergeSurfaces(selected_surfaces);
}

void CSurfaceEntry_Delegate::CutSurfacesBB(const geo::CBox& box)
{
  DoCutSurfaces(&box);
}

// private

void CSurfaceEntry_Delegate::DoCutSurfaces(const geo::CBox* pbox)
{
  CNamedSurfaces slected_surfaces;

  SelectSurfaces(slected_surfaces, NOT_USED_IN_MODEL, "Select surfaces for cutting");

  if (slected_surfaces.size() == 0)
  {
    return;
  }

  CCutSurfaceDlg* pDlg;

  if (pbox)
  {
    pDlg = new CCutSurfaceDlg(*pbox, GetGeomecDoc()->UnitNode().Unit());
  }
  else
  {
    pDlg = new CCutSurfaceDlg(GetGeomecDoc()->UnitNode().Unit());
  }

  for (size_t i = 0; i < slected_surfaces.size(); i++)
  {
    STNamedSurface& ns = slected_surfaces.at(i);

    pDlg->InsertSurface(ns.Surface, ns.Name.c_str());
  }

  if (pDlg->DoModal() == IDCANCEL)
  {
    delete pDlg;
    return;
  }

  CGeomecDoc* pDoc = GetGeomecDoc();
  CModelBase* pModel = static_cast<CModelBase*>(pDoc->Model());
  assert(pModel);
  m_surfaceEntry->DoCutSurfaces(*pModel, pDlg->CutSurfacesCount(),
    pDlg->getCutSurfaces(), pDlg->getSideSurfaces(), (LPCSTR) pDlg->BaseName(),
    pDlg->GetSides(), pbox);

  delete pDlg;
}

void CSurfaceEntry_Delegate::SelectSurfaces(CNamedSurfaces& ret, eCheckUsed check, const char *caption)
{
  CSurfaceEntry::TNodeSet stSurfaces = m_surfaceEntry->EntryNodes();
  SelSurfDlg dlgsel(NULL, caption);

  for (CSurfaceEntry::TNodeSet::const_iterator it = stSurfaces.begin();
    it != stSurfaces.end(); it++)
  {
    switch(check)
    {
    case USED_IN_MODEL:
      if ((*it)->CanDestroy())
      {
        continue;
      }
      break;
    case NOT_USED_IN_MODEL:
      if (!(*it)->CanDestroy())
      {
        continue;
      }
      break;
    }

    const CSurfaceBase* sb = (*it);
    const geo::ISurface* surf =
      dynamic_cast <const geo::ISurface*> (&((*it)->Surface()));

    if (surf)
    {
      dlgsel.InsertSurface(surf, sb->Name().toStdString().c_str());
    }
  }

  if (dlgsel.DoModal() == IDOK)
  {
    int i;
    int count = dlgsel.SelectedSurfacesCount();

    for (i = 0; i < count; i++)
    {
      const STNamedSurface& ns = dlgsel.GetSelectedSurfaces(i);

      ret.push_back(ns);
    }
  }
}
