#include "stdafx.h"

#include "MeshBase_Delegate.h"
#include "MeshBase.h"
#include "AttriMesh.h"
#include "ContextMenuInvoker.h"
#include "MainFrm.h"
#include "ModelBase.h"
#include "PointSet.h"
#include "FemAppMainWindow.h"

CMeshBase_Delegate::CMeshBase_Delegate(CMeshBase* meshBase)
: IElementSet_Delegate(meshBase)
, m_meshBase(meshBase)
{
}

bool CMeshBase_Delegate::Attributes()
{
  CAttriMesh dlg(*m_meshBase);

  return dlg.DoModal() == IDOK;
}

bool CMeshBase_Delegate::IsMesh() const
{
  return m_meshBase->IsMesh();
}

void CMeshBase_Delegate::AppendContextMenu(CContextMenuInvoker &invoker)
{
  typedef CSingleCommandTemplate <CMeshBase_Delegate> TCommand;

  invoker.AddCommand(_T("Create pointset"),
  *(new TCommand(*this, &CMeshBase_Delegate::CreatePointSet,
      &CMeshBase_Delegate::IsMesh)));

  IElementSet_Delegate::AppendContextMenu(invoker);
}

void CMeshBase_Delegate::CreatePointSet()
{
  assert(m_meshBase->IsMesh());

  CMainFrame* pMainFrame = (CMainFrame*) FemAppGetMainWnd();

  pMainFrame->StartProgress("Creating pointset",
  m_meshBase->Mesh().PointSize());

  // Create a new pointset and add the points

  IPointSet::DIMENSION dim = IPointSet::DIM_3D;

  if (((CModelBase&) m_meshBase->Model()).Dimension() == 2)
  {
  dim = IPointSet::DIM_2D;
  }

  CPointSet* pPointSet = new CPointSet(m_meshBase->Name().toStdString().c_str(),
  m_meshBase->Model(), 0, dim);

  for (int i = 0; i < m_meshBase->Mesh().PointSize(); i++)
  {
  pPointSet->PushBack(m_meshBase->Mesh().Point(i), std::vector <double> ());
  pMainFrame->ProgressStep();
  }

  pMainFrame->EndProgress();
}

bool CMeshBase_Delegate::CanDestroy() const
{
  return m_meshBase->CanDestroy();
}
