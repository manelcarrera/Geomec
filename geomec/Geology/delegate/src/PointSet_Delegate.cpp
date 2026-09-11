#include "stdafx.h"

#include "PointSet_Delegate.h"
#include "AttriPointSetDlg.h"
#include "ContextMenuInvoker.h"
#include "ModelBase.h"
#include "ExportDlg.h"
#include "TNOFileDialog.h"
#include "SampledPointsetDlg.h"
#include "GeomecDoc.h"
#include "FemAppMainWindow.h"

CPointSet_Delegate::CPointSet_Delegate(CPointSet* pointSet)
: IPointSet_Delegate(pointSet)
, m_pointSet(pointSet)
{
}

bool CPointSet_Delegate::Attributes()
{
  m_pointSet->AssertValid();

  CAttriPointSetTemplate <CPointSet> dlg(*m_pointSet);

  if (dlg.DoModal() == IDOK)
  {
  m_pointSet->AssertValid();
  m_pointSet->Modified(); // wjrx mantis 3335

  return true;
  }

  return false;
}

void CPointSet_Delegate::AppendContextMenu(CContextMenuInvoker &invoker)
{
  typedef CSingleCommandTemplate <CPointSet> TPointSetCommand;
  typedef CSingleCommandTemplate <CPointSet_Delegate> TPointSet_DelegateCommand;

  // Export all ...

  invoker.AddCommand(_T("&Export"),
  *(new TPointSet_DelegateCommand(*this, &CPointSet_Delegate::Export)));

  if (m_pointSet->Dimension() == 2 || m_pointSet->Dimension() == 3)
  {
  // wjrx mantis 3186

  invoker.AddCommand(_T("&Sample"),
      *(new TPointSet_DelegateCommand(*this, &CPointSet_Delegate::Sample)));

#if _DEBUG
  invoker.AddCommand(_T("Delete Convex &Hull"), *(new TPointSet_DelegateCommand(*this, &CPointSet_Delegate::DestroyConvexHull, &CPointSet_Delegate::CanDestroyConvexHull)));
#endif
  }

  invoker.AddSeparator();

  IPointSet_Delegate::AppendContextMenu(invoker);
}

bool CPointSet_Delegate::CanDestroy() const
{
  return m_pointSet->CanDestroy();
}

void CPointSet_Delegate::Export()
{
  CGeomecDoc& doc = *GetGeomecDoc();
  IExportFormat::CExportArg arg(doc.UnitNode().Unit());
  const CModelBase& model = (CModelBase&) m_pointSet->Model();

  CTnoFileDialog dlg(FALSE, "dat", m_pointSet->Name().toStdString().c_str(),
  OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT | OFN_NOREADONLYRETURN,
  "ASCII Files (*.dat;*.txt;*.xyz)|*.dat;*.txt;*.xyz|"
  "Excel Files (*.xls;*.xls*)|*.xls;*.xls*||", FemAppGetMainWnd());

  if (dlg.DoModal() == IDOK)
  {
  m_pointSet->Export(arg, (LPCSTR) dlg.GetPathName());
  }
}

void CPointSet_Delegate::Sample()
{
  // Dimension() is caught in the GUI

  if (m_pointSet->Dimension() < 2 )
  {
  return;
  }

  CGeomecDoc& doc = *GetGeomecDoc();
  CSampledPointSetDlg sampleDlg(*m_pointSet, doc.UnitNode().Unit());

  if (sampleDlg.DoModal() == IDOK)
  {
  m_pointSet->Sample((LPCSTR) sampleDlg.getName(), sampleDlg.getSizeNorth(),
      sampleDlg.getSizeEast(), sampleDlg.getSizeDepth());
  }
}


bool CPointSet_Delegate::CanDestroyConvexHull() const
{
  return m_pointSet->HasHull();
}

void CPointSet_Delegate::DestroyConvexHull()
{
  m_pointSet->DestroyHull();
}

CPointSet_Delegate::CCoordinateType_Delegate::CCoordinateType_Delegate(
  CPointSet::CCoordinateType* coordinateType)
: CValueType_Delegate(coordinateType)
, m_coordinateType(coordinateType)
{
}

bool CPointSet_Delegate::CCoordinateType_Delegate::CanDestroy() const
{
  return m_coordinateType->CanDestroy();
}
