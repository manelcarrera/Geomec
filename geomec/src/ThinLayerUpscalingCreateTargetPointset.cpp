#include "Geomec.h"
#include "stdafx.h"

#include "BaseEntryTypes.h"
#include "BoundaryBase.h"
#include "CoordinateSet.h"
#include "GeoSurface.h"
#include "ModelBase.h"
#include "PointSet.h"
#include "ThinLayerUpscalingCreateTargetPointset.h"

// CDefineRegular2DGridDlg dialog

static const int DEF_NUM_POINTS = 10;

CDefineRegular2DGridDlg::CDefineRegular2DGridDlg(const CModelBase &model, CWnd *pParent)
    : CDialog(CDefineRegular2DGridDlg::IDD, pParent), m_model(model), m_nPointsNorthing(DEF_NUM_POINTS),
      m_nPointsEasting(DEF_NUM_POINTS) {
  geo::CPoint ptMin(model.Boundary().Min());
  geo::CPoint ptMax(model.Boundary().Max());

  m_Corner1Northing.Value(ptMin.X(), CDoubleQuantity::SI_UNIT);
  m_Corner1Easting.Value(ptMin.Y(), CDoubleQuantity::SI_UNIT);
  m_Corner2Northing.Value(ptMax.X(), CDoubleQuantity::SI_UNIT);
  m_Corner2Easting.Value(ptMax.Y(), CDoubleQuantity::SI_UNIT);
}

CDefineRegular2DGridDlg::~CDefineRegular2DGridDlg() {}

geo::CPoint CDefineRegular2DGridDlg::Corner1() const {
  return geo::CPoint(m_Corner1Northing.Value(), m_Corner1Easting.Value());
}

geo::CPoint CDefineRegular2DGridDlg::Corner2() const {
  return geo::CPoint(m_Corner2Northing.Value(), m_Corner2Easting.Value());
}

int CDefineRegular2DGridDlg::NumPointsNorthing() const { return m_nPointsNorthing; }

int CDefineRegular2DGridDlg::NumPointsEasting() const { return m_nPointsEasting; }

void CDefineRegular2DGridDlg::DoDataExchange(CDataExchange *pDX) {
  CDialog::DoDataExchange(pDX);

  CString strLengthUnit = m_Corner1Northing.UnitName(GetGeomecDoc()->UnitNode().Unit()).c_str();
  double dCorner1Northing;
  double dCorner1Easting;
  double dCorner2Northing;
  double dCorner2Easting;

  if (!pDX->m_bSaveAndValidate) {
    dCorner1Northing = m_Corner1Northing.Value(GetGeomecDoc()->UnitNode().Unit());
    dCorner1Easting = m_Corner1Easting.Value(GetGeomecDoc()->UnitNode().Unit());
    dCorner2Northing = m_Corner2Northing.Value(GetGeomecDoc()->UnitNode().Unit());
    dCorner2Easting = m_Corner2Easting.Value(GetGeomecDoc()->UnitNode().Unit());
  }

  DDX_Text(pDX, IDC_UN_CORNER1_NORTHING, strLengthUnit);
  DDX_Text(pDX, IDC_UN_CORNER1_EASTING, strLengthUnit);
  DDX_Text(pDX, IDC_UN_CORNER2_NORTHING, strLengthUnit);
  DDX_Text(pDX, IDC_UN_CORNER2_EASTING, strLengthUnit);

  DDX_Text(pDX, IDC_ED_CORNER1_NORTHING, dCorner1Northing);
  DDX_Text(pDX, IDC_ED_CORNER1_EASTING, dCorner1Easting);
  DDX_Text(pDX, IDC_ED_CORNER2_NORTHING, dCorner2Northing);
  DDX_Text(pDX, IDC_ED_CORNER2_EASTING, dCorner2Easting);

  DDX_Text(pDX, IDC_ED_NUMPOINTS_NORTHING, m_nPointsNorthing);
  DDX_Text(pDX, IDC_ED_NUMPOINTS_EASTING, m_nPointsEasting);

  if (pDX->m_bSaveAndValidate) {
    m_Corner1Northing.Value(dCorner1Northing, GetGeomecDoc()->UnitNode().Unit());
    m_Corner1Easting.Value(dCorner1Easting, GetGeomecDoc()->UnitNode().Unit());
    m_Corner2Northing.Value(dCorner2Northing, GetGeomecDoc()->UnitNode().Unit());
    m_Corner2Easting.Value(dCorner2Easting, GetGeomecDoc()->UnitNode().Unit());
  }
}

ISelectObjectDlg::ISelectObjectDlg(const CModelBase &model, CWnd *pParent)
    : CDialog(IDD_SELECTOBJECT_DLG, pParent), m_model(model) {}

const CModelBase &ISelectObjectDlg::Model() const { return m_model; }

void ISelectObjectDlg::DoDataExchange(CDataExchange *pDX) {
  CDialog::DoDataExchange(pDX);
  DDX_Control(pDX, IDC_LIST_OBJECTS, m_lbObjects);
}

BOOL ISelectObjectDlg::OnInitDialog() {
  CDialog::OnInitDialog();

  SetWindowText(CString("Select ") + ObjectTitle());

  CRect rect;
  GetDlgItem(IDC_LIST_OBJECTS)->GetClientRect(&rect);

  m_lbObjects.InsertColumn(0, ObjectTitle(), LVCFMT_LEFT, rect.Width());
  CreateListObjects(m_lbObjects);

  GetDlgItem(IDOK)->EnableWindow(FALSE);

  return TRUE;
}

// CSelect2DPointsetDlg dialog

CSelect2DPointsetDlg::CSelect2DPointsetDlg(const CModelBase &model, CWnd *pParent) : ISelectObjectDlg(model, pParent) {}

const CPointSet *CSelect2DPointsetDlg::SelectedPointset() const { return m_pSelectedPointset; }

void CSelect2DPointsetDlg::SelectPointset(const CPointSet *pPointset) {
  m_pSelectedPointset = pPointset;
  GetDlgItem(IDOK)->EnableWindow(m_pSelectedPointset != 0);
}

CString CSelect2DPointsetDlg::ObjectTitle() const { return "Pointset"; }

void CSelect2DPointsetDlg::CreateListObjects(CListCtrl &ctrl) {
  const TPointSetEntry &entry = static_cast<const TPointSetEntry &>(*Model().GraphEntry(MD_BASE_POINTSET));
  const TPointSetEntry::TSortedNodeSet &stSortedNodes = entry.SortedEntryNodes();
  TPointSetEntry::TSortedNodeSet::const_iterator it;
  for (it = stSortedNodes.begin(); it != stSortedNodes.end(); ++it) {
    const CPointSet *pPointset = dynamic_cast<const CPointSet *>(*it);
    if (pPointset && pPointset->Dimension() == IPointSet::DIM_2D)
      new CPointsetListObject(*pPointset, *this, ctrl);
  }
}

CSelect2DPointsetDlg::CPointsetListObject::CPointsetListObject(const CPointSet &pointset, CSelect2DPointsetDlg &dlg,
                                                               CListCtrl &ctrl)
    : IListObject(ctrl), m_pointset(pointset), m_dlg(dlg) {
  ctrl.SetItemData(ctrl.GetItemCount() - 1, (DWORD_PTR)this);
  Update();
}

unsigned int CSelect2DPointsetDlg::CPointsetListObject::Icon() const { return m_pointset.IconId(); }

QString CSelect2DPointsetDlg::CPointsetListObject::Text() const { return m_pointset.Name(); }

void CSelect2DPointsetDlg::CPointsetListObject::SelectionStateChanged(bool bSelected) {
  if (bSelected)
    m_dlg.SelectPointset(&m_pointset);
  else if (m_dlg.SelectedPointset() == &m_pointset)
    m_dlg.SelectPointset(0);
}

void CSelect2DPointsetDlg::CPointsetListObject::OnDoubleClick() {
  m_dlg.SelectPointset(&m_pointset);
  m_dlg.OnOK();
}

// CSelectSurfaceDlg

CSelectSurfaceDlg::CSelectSurfaceDlg(const CModelBase &model, CWnd *pParent) : ISelectObjectDlg(model, pParent) {}

const CSurfaceBase *CSelectSurfaceDlg::SelectedSurface() const { return m_pSelectedSurface; }

void CSelectSurfaceDlg::SelectSurface(const CSurfaceBase *pSurface) {
  m_pSelectedSurface = pSurface;
  GetDlgItem(IDOK)->EnableWindow(m_pSelectedSurface != 0);
}

CString CSelectSurfaceDlg::ObjectTitle() const { return "Surface"; }

void CSelectSurfaceDlg::CreateListObjects(CListCtrl &ctrl) {
  const CSurfaceEntry &entry = static_cast<const CSurfaceEntry &>(*Model().GraphEntry(MD_BASE_SURFACE));
  const CSurfaceEntry::TSortedNodeSet &stSortedNodes = entry.SortedEntryNodes();
  CSurfaceEntry::TSortedNodeSet::const_iterator it;
  for (it = stSortedNodes.begin(); it != stSortedNodes.end(); ++it) {
    new CSurfaceListObject(**it, *this, ctrl);
  }
}

CSelectSurfaceDlg::CSurfaceListObject::CSurfaceListObject(const CSurfaceBase &surface, CSelectSurfaceDlg &dlg,
                                                          CListCtrl &ctrl)
    : IListObject(ctrl), m_surface(surface), m_dlg(dlg) {
  ctrl.SetItemData(ctrl.GetItemCount() - 1, (DWORD_PTR)this);
  Update();
}

unsigned int CSelectSurfaceDlg::CSurfaceListObject::Icon() const { return m_surface.IconId(); }

QString CSelectSurfaceDlg::CSurfaceListObject::Text() const { return m_surface.Name(); }

void CSelectSurfaceDlg::CSurfaceListObject::SelectionStateChanged(bool bSelected) {
  if (bSelected)
    m_dlg.SelectSurface(&m_surface);
  else if (m_dlg.SelectedSurface() == &m_surface)
    m_dlg.SelectSurface(0);
}

void CSelectSurfaceDlg::CSurfaceListObject::OnDoubleClick() {
  m_dlg.SelectSurface(&m_surface);
  m_dlg.OnOK();
}
