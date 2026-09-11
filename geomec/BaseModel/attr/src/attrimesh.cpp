// attrimesh.cpp : implementation file
//

#include "attrimesh.h"
#include "GeomecDianaRunner.h"
#include "MeshBase.h"
#include "geomec.h"
#include "stdafx.h"

#ifdef _DEBUG
// #define new DEBUG_NEW
#ifdef _MSC_VER
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif // _MSC_VER
#endif

/////////////////////////////////////////////////////////////////////////////
// CAttriMesh dialog

CAttriMesh::CAttriMesh(CMeshBase &mesh, CWnd *pParent /*=NULL*/) : CDialog(CAttriMesh::IDD, pParent), m_mesh(mesh) {
  //{{AFX_DATA_INIT(CAttriMesh)
  //}}AFX_DATA_INIT
}

void CAttriMesh::DoDataExchange(CDataExchange *pDX) {
  int nElement, nNode;
  if (!pDX->m_bSaveAndValidate) {
    nElement = m_mesh.Mesh().ElementSize();
    nNode = m_mesh.Mesh().PointSize();
  }

  CDialog::DoDataExchange(pDX);
  //{{AFX_DATA_MAP(CAttriMesh)
  DDX_Text(pDX, IDC_ST_ELEMENTS, nElement);
  DDX_Text(pDX, IDC_ST_NODES, nNode);
  //}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CAttriMesh, CDialog)
//{{AFX_MSG_MAP(CAttriMesh)
// NOTE: the ClassWizard will add message map macros here
//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CAttriMesh message handlers
