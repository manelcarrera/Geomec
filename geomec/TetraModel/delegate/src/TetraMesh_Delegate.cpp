#include "stdafx.h"

#include "TetraMesh_Delegate.h"
#include "TetraMesh.h"
#include "ContextMenuInvoker.h"

CTetraMesh_Delegate::CTetraMesh_Delegate(CTetraMesh* tetraMesh)
: CMeshBase_Delegate(tetraMesh)
, m_tetraMesh(tetraMesh)
{
}

void CTetraMesh_Delegate::AppendContextMenu(CContextMenuInvoker &invoker)
{
  typedef CSingleCommandTemplate <CTetraMesh_Delegate>
  TTetraMesh_DelegateCommand;

  invoker.AddCommand(_T("Create mesh"),
  *(new CTetraMesh::TTetraMeshCommand(*m_tetraMesh, &CTetraMesh::CreateMesh,
      &CTetraMesh::CanCreateMesh)));
  invoker.AddSeparator();
  invoker.AddCommand(_T("Export"),
  *(new TTetraMesh_DelegateCommand(*this, &CTetraMesh_Delegate::Export,
      &CTetraMesh_Delegate::IsMesh)));
  invoker.AddSeparator();

  CMeshBase_Delegate::AppendContextMenu(invoker);
}

bool CTetraMesh_Delegate::IsMesh() const
{
  return m_tetraMesh->IsMesh();
}
