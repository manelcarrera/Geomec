#include "stdafx.h"

#include "IWellMesh_Delegate.h"
#include "IWellMesh.h"
#include "ContextMenuInvoker.h"

IWellMesh_Delegate::IWellMesh_Delegate(IWellMesh* wellMesh)
: CMeshBase_Delegate(wellMesh)
, m_wellMesh(wellMesh)
{
}

void IWellMesh_Delegate::AppendContextMenu(CContextMenuInvoker &invoker)
{
  typedef CSingleCommandTemplate <IWellMesh> TWellMeshCommand;

  invoker.AddCommand(_T("Generate"),
  *new TWellMeshCommand(*m_wellMesh, &IWellMesh::CreateMesh,
      &IWellMesh::CanCreateMesh));
  invoker.AddCommand(_T("Invalidate"),
  *new TWellMeshCommand(*m_wellMesh, &IWellMesh::InvalidateMesh,
      &IWellMesh::IsMesh));

  COpenGLNode_Delegate::AppendContextMenu(invoker);
}
