#include "stdafx.h"

#include "AnalysisPoint_Delegate.h"
#include "AnalysisPoint.h"
#include "AttriAnalysisPoint.h"
#include "ContextMenuInvoker.h"
#include "ModelBase.h"
#include "BoundaryBase.h"

CAnalysisPoint_Delegate::CAnalysisPoint_Delegate(CAnalysisPoint* analysisPoint)
: CColorNode_Delegate(analysisPoint)
, m_analysisPoint(analysisPoint)
{
}

bool CAnalysisPoint_Delegate::Attributes()
{
  CAttriAnalysisPoint dlg(*m_analysisPoint);

  if (dlg.DoModal() == IDOK)
  {
  return true;
  }

  return false;
}

CAnalysisPointEntry_Delegate::CAnalysisPointEntry_Delegate(
  CAnalysisPointEntry* analysisPointEntry)
: CStorageNodeEntry_Delegate <CAnalysisPoint> (analysisPointEntry)
, m_analysisPointEntry(analysisPointEntry)
{
}

void CAnalysisPointEntry_Delegate::AppendContextMenu(
  CContextMenuInvoker& invoker)
{
  typedef CSingleCommandTemplate <CAnalysisPointEntry_Delegate> TMyCommand;

  invoker.AddCommand("Create &Analysis point", *(new TMyCommand(*this,
  &CAnalysisPointEntry_Delegate::CreateAnalysisPoint)));
}

namespace
{

const QString DEFAULT_ANALYSIS_POINT_NAME = QObject::tr("New point%1");
const size_t DEFAULT_INITIAL_NUMBER = 0;

} // anonymous namespace

void CAnalysisPointEntry_Delegate::CreateAnalysisPoint()
{
  QString sName =
  m_analysisPointEntry->createUniqueName(DEFAULT_ANALYSIS_POINT_NAME,
      DEFAULT_INITIAL_NUMBER);
  CModelBase& model = (CModelBase&) m_analysisPointEntry->Model();
  geo::CPoint point = model.Boundary().Min() +
  (model.Boundary().Max() - model.Boundary().Min()) / 2;

  assert(!point.Empty());

  CAnalysisPoint* pPoint = new CAnalysisPoint(sName.toStdString().c_str(), point, model);
  CAnalysisPoint_Delegate* pPoint_Delegate = new CAnalysisPoint_Delegate(pPoint);

  if (!pPoint_Delegate->Attributes())
  {
  delete pPoint_Delegate;
  delete pPoint;
  }
}
