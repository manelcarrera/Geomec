#include "StdAfx.h"
#include "resource.h"

#include "AttriTetraVolume.h"
#include "TetraHorizonBase.h"
#include "MeshBase.h"
#include "SurfaceDesc.h"

CAttriTetraVolume::CSurfaceListObject::CSurfaceListObject(CListCtrl &ctrl, QString sName, int nIconId)
  : IListObject(ctrl), m_sName(sName), m_nIconId(nIconId)
{
  ctrl.SetItemData(ctrl.GetItemCount() - 1, (DWORD_PTR)this);
}

CAttriTetraVolume::CSurfaceListObject::~CSurfaceListObject()
{
}

QString CAttriTetraVolume::CSurfaceListObject::Text() const
{
  return m_sName;
}

unsigned int CAttriTetraVolume::CSurfaceListObject::Icon() const
{
  return m_nIconId;
}


////

CAttriTetraVolume::CAttriTetraVolume(CTetraFormationVolume& volume, CWnd* pParent)
: CAttriFormationElementSet<CTetraFormationVolume>(IDD_ATTRI_FORMATION_VOLUME, volume, pParent)
{
}

BOOL CAttriTetraVolume::OnInitDialog()
{
  CAttriFormationElementSet<CTetraFormationVolume>::OnInitDialog();

  m_lbSurfaces.InsertColumn(0, _T("Name"), LVCFMT_LEFT, -1, 0);
  OnUpdateLb();
  m_lbSurfaces.SetColumnWidth(0, LVSCW_AUTOSIZE);

  return TRUE;
}

void CAttriTetraVolume::DoDataExchange(CDataExchange* pDX)
{
  CAttriFormationElementSet<CTetraFormationVolume>::DoDataExchange(pDX);

  DDX_Control(pDX, IDC_LC_SURFACES, m_lbSurfaces);
}

void CAttriTetraVolume::OnUpdateLb()
{
  m_lbSurfaces.DeleteAllItems();

  CTetraFormationVolume::TEnclosingSurfacesMap mpSurfaces = Copy().GetEnclosingSurfaces();

  CTetraFormationVolume::TEnclosingSurfacesMap::iterator it;
  for (it = mpSurfaces.begin(); it != mpSurfaces.end(); ++it)
  new CSurfaceListObject(m_lbSurfaces, it->first, it->second);

  const CModelBase& model = static_cast<const CModelBase&>(Copy().Model());
  GetDlgItem(IDC_LC_SURFACES)->EnableWindow(model.Mesh().IsMesh());
}

