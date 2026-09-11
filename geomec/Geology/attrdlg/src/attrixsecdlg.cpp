#include <cmath>
#include "stdafx.h"
#include "geomec.h"
#include "modelbase.h"
#include "attrixsecdlg.h"
#include "CrossSection.h"
#include "BoundaryBase.h"
#include "GlobalUnitNode.h"
#include "GlobalMessage.h"

#ifdef _DEBUG
//#define new DEBUG_NEW
#ifdef _MSC_VER
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif  // _MSC_VER
#endif

void KeepInBounds( const char * basePoint, const char * coordinate, double & value, double min, double max);
bool OrientationIsSet (CCrossSection::OrientationType variable, int count, ...);

/////////////////////////////////////////////////////////////////////////////
// CAttriXSecDlg dialog


CAttriXSecDlg::CAttriXSecDlg(CCrossSection &Xsec, CWnd* pParent /*=NULL*/)
  : CDialog(CAttriXSecDlg::IDD, pParent),
  m_xsec(Xsec),
  m_basePointBookmarked(Xsec.IntersectPlaneBookmarked().BasePoint()),
  m_normalBookmarked(Xsec.NormalBookmarked()),
  m_normalBookmarkedSaved(Xsec.NormalBookmarked()),
  m_basePointScreen(Xsec.IntersectPlaneScreen().BasePoint()),
  m_normalScreen(Xsec.NormalScreen()),
  m_normalScreenSaved(Xsec.NormalScreen()),
  m_constraintBookmarked(Xsec.OrientationBookmarked()),
  m_constraintScreen(Xsec.OrientationScreen()),
  m_strName(Xsec.Name().toStdString().c_str()),
  m_bOk(false)
, m_ptFirst()
, m_ptSecond()
, m_dDepth(Xsec.IntersectPlaneScreen().BasePoint().Z())
, m_crossSectionType(Xsec.crossSectionType())
{
  CCrossSection::calculateTwoArbitraryPointsInPlane(m_ptFirst, m_ptSecond,
  Xsec.IntersectPlaneScreen(),
  dynamic_cast <const CModelBase&> (Xsec.Model()));
  //{{AFX_DATA_INIT(CAttriXSecDlg)
    // NOTE: the ClassWizard will add member initialization here
  //}}AFX_DATA_INIT
}

#include <sstream>
void LimitDecimals(double& x, int numDecimals)
{
  std::stringstream ss;
  ss << std::scientific;
  ss.precision(numDecimals); // set # places after decimal
  ss << x;  
  ss >> x;
}

void CAttriXSecDlg::DoDataExchange(CDataExchange* pDX)
{
  CGlobalUnitNode globalUnitNode;

  CDialog::DoDataExchange(pDX);

  CLengthQuantity qnLength;

  CString strUnit;
  CString strName;

  double dBasePointNorthScreen;
  double dBasePointEastScreen;
  double dBasePointDepthScreen;
  double dNormalNorthScreen;
  double dNormalEastScreen;
  double dNormalDepthScreen;

  double dBasePointNorthBookmarked;
  double dBasePointEastBookmarked;
  double dBasePointDepthBookmarked;
  double dNormalNorthBookmarked;
  double dNormalEastBookmarked;
  double dNormalDepthBookmarked;


  if(!pDX->m_bSaveAndValidate)
  {
  switch(globalUnitNode.unitNode().Unit())
  {
  case CQuantity::SI_UNIT:
      strUnit.Format("m");
      break;
  case CQuantity::FIELD_UNIT:
      strUnit.Format("ft");
      break;
  }

  strName = m_strName;

  const int numDecimals = 10;
  dBasePointNorthBookmarked = qnLength.Convert(m_basePointBookmarked.X(), globalUnitNode.unitNode().Unit(), CQuantity::SI_UNIT);
  LimitDecimals(dBasePointNorthBookmarked, numDecimals);
  dBasePointEastBookmarked  = qnLength.Convert(m_basePointBookmarked.Y(), globalUnitNode.unitNode().Unit(), CQuantity::SI_UNIT);
  LimitDecimals(dBasePointEastBookmarked, numDecimals);
  dBasePointDepthBookmarked = qnLength.Convert(m_basePointBookmarked.Z(), globalUnitNode.unitNode().Unit(), CQuantity::SI_UNIT);
  LimitDecimals(dBasePointDepthBookmarked, numDecimals);

  dNormalNorthBookmarked = qnLength.Convert(m_normalBookmarked.X(), globalUnitNode.unitNode().Unit(), CQuantity::SI_UNIT);
  LimitDecimals(dNormalNorthBookmarked, numDecimals);
  dNormalEastBookmarked  = qnLength.Convert(m_normalBookmarked.Y(), globalUnitNode.unitNode().Unit(), CQuantity::SI_UNIT);
  LimitDecimals(dNormalEastBookmarked, numDecimals);
  dNormalDepthBookmarked = qnLength.Convert(m_normalBookmarked.Z(), globalUnitNode.unitNode().Unit(), CQuantity::SI_UNIT);
  LimitDecimals(dNormalDepthBookmarked, numDecimals);

  dBasePointNorthScreen = qnLength.Convert(m_basePointScreen.X(), globalUnitNode.unitNode().Unit(), CQuantity::SI_UNIT);
  LimitDecimals(dBasePointNorthScreen, numDecimals);
  dBasePointEastScreen  = qnLength.Convert(m_basePointScreen.Y(), globalUnitNode.unitNode().Unit(), CQuantity::SI_UNIT);
  LimitDecimals(dBasePointEastScreen, numDecimals);
  dBasePointDepthScreen = qnLength.Convert(m_basePointScreen.Z(), globalUnitNode.unitNode().Unit(), CQuantity::SI_UNIT);
  LimitDecimals(dBasePointDepthScreen, numDecimals);

  dNormalNorthScreen = qnLength.Convert(m_normalScreen.X(), globalUnitNode.unitNode().Unit(), CQuantity::SI_UNIT);
  LimitDecimals(dNormalNorthScreen, numDecimals);
  dNormalEastScreen  = qnLength.Convert(m_normalScreen.Y(), globalUnitNode.unitNode().Unit(), CQuantity::SI_UNIT);
  LimitDecimals(dNormalEastScreen, numDecimals);
  dNormalDepthScreen = qnLength.Convert(m_normalScreen.Z(), globalUnitNode.unitNode().Unit(), CQuantity::SI_UNIT);
  LimitDecimals(dNormalDepthScreen, numDecimals);
  }

  //{{AFX_DATA_MAP(CAttriXSecDlg)
  // NOTE: the ClassWizard will add DDX and DDV calls here
  //}}AFX_DATA_MAP

  DDX_Text(pDX, IDC_EDIT_XSEC_NAME, strName);
  DDX_Radio(pDX, IDC_RADIO_SCREEN_DEPTH, (int &) m_constraintScreen);
  DDX_Radio(pDX, IDC_RADIO_BM_DEPTH, (int &) m_constraintBookmarked);

  DDX_Text(pDX, IDC_EDIT_SCREEN_BASEPOINT_NORTHING, dBasePointNorthScreen);
  DDX_Text(pDX, IDC_EDIT_SCREEN_BASEPOINT_EASTING, dBasePointEastScreen);
  DDX_Text(pDX, IDC_EDIT_SCREEN_BASEPOINT_DEPTH, dBasePointDepthScreen);

  DDX_Text(pDX, IDC_EDIT_SCREEN_NORMAL_NORTHING, dNormalNorthScreen);
  DDX_Text(pDX, IDC_EDIT_SCREEN_NORMAL_EASTING, dNormalEastScreen);
  DDX_Text(pDX, IDC_EDIT_SCREEN_NORMAL_DEPTH, dNormalDepthScreen);

  DDX_Text(pDX, IDC_EDIT_BM_BASEPOINT_NORTHING, dBasePointNorthBookmarked);
  DDX_Text(pDX, IDC_EDIT_BM_BASEPOINT_EASTING, dBasePointEastBookmarked);
  DDX_Text(pDX, IDC_EDIT_BM_BASEPOINT_DEPTH, dBasePointDepthBookmarked);

  DDX_Text(pDX, IDC_EDIT_BM_NORMAL_NORTHING, dNormalNorthBookmarked);
  DDX_Text(pDX, IDC_EDIT_BM_NORMAL_EASTING, dNormalEastBookmarked);
  DDX_Text(pDX, IDC_EDIT_BM_NORMAL_DEPTH, dNormalDepthBookmarked);

  DDX_Text(pDX, IDC_LBL_SCREEN_BASEPOINT_NORTHING_UNIT, strUnit);
  DDX_Text(pDX, IDC_LBL_SCREEN_BASEPOINT_EASTING_UNIT, strUnit);
  DDX_Text(pDX, IDC_LBL_SCREEN_BASEPOINT_DEPTH_UNIT, strUnit);

  DDX_Text(pDX, IDC_LBL_SCREEN_NORMAL_NORTHING_UNIT, strUnit);
  DDX_Text(pDX, IDC_LBL_SCREEN_NORMAL_EASTING_UNIT, strUnit);
  DDX_Text(pDX, IDC_LBL_SCREEN_NORMAL_DEPTH_UNIT, strUnit);

  DDX_Text(pDX, IDC_LBL_BM_BASEPOINT_NORTHING_UNIT, strUnit);
  DDX_Text(pDX, IDC_LBL_BM_BASEPOINT_EASTING_UNIT, strUnit);
  DDX_Text(pDX, IDC_LBL_BM_BASEPOINT_DEPTH_UNIT, strUnit);

  DDX_Text(pDX, IDC_LBL_BM_NORMAL_NORTHING_UNIT, strUnit);
  DDX_Text(pDX, IDC_LBL_BM_NORMAL_EASTING_UNIT, strUnit);
  DDX_Text(pDX, IDC_LBL_BM_NORMAL_DEPTH_UNIT, strUnit);

  dBasePointNorthBookmarked = qnLength.Convert(dBasePointNorthBookmarked, CQuantity::SI_UNIT, globalUnitNode.unitNode().Unit());
  dBasePointEastBookmarked  = qnLength.Convert(dBasePointEastBookmarked,  CQuantity::SI_UNIT, globalUnitNode.unitNode().Unit());
  dBasePointDepthBookmarked = qnLength.Convert(dBasePointDepthBookmarked, CQuantity::SI_UNIT, globalUnitNode.unitNode().Unit());

  dNormalNorthBookmarked = qnLength.Convert(dNormalNorthBookmarked, CQuantity::SI_UNIT, globalUnitNode.unitNode().Unit());
  dNormalEastBookmarked  = qnLength.Convert(dNormalEastBookmarked,  CQuantity::SI_UNIT, globalUnitNode.unitNode().Unit());
  dNormalDepthBookmarked = qnLength.Convert(dNormalDepthBookmarked, CQuantity::SI_UNIT, globalUnitNode.unitNode().Unit());

  dBasePointNorthScreen = qnLength.Convert(dBasePointNorthScreen, CQuantity::SI_UNIT, globalUnitNode.unitNode().Unit());
  dBasePointEastScreen  = qnLength.Convert(dBasePointEastScreen,  CQuantity::SI_UNIT, globalUnitNode.unitNode().Unit());
  dBasePointDepthScreen = qnLength.Convert(dBasePointDepthScreen, CQuantity::SI_UNIT, globalUnitNode.unitNode().Unit());

  dNormalNorthScreen = qnLength.Convert(dNormalNorthScreen, CQuantity::SI_UNIT, globalUnitNode.unitNode().Unit());
  dNormalEastScreen  = qnLength.Convert(dNormalEastScreen,  CQuantity::SI_UNIT, globalUnitNode.unitNode().Unit());
  dNormalDepthScreen = qnLength.Convert(dNormalDepthScreen, CQuantity::SI_UNIT, globalUnitNode.unitNode().Unit());

  if(pDX->m_bSaveAndValidate)
  {
  // Validate your data
  CModelBase *pModel = dynamic_cast<CModelBase*>(&m_xsec.Model());
  assert(pModel);

  double dBoundMinY = pModel->Boundary().Min().Y();
  double dBoundMinX = pModel->Boundary().Min().X();
  double dBoundMaxY = pModel->Boundary().Max().Y();
  double dBoundMaxX = pModel->Boundary().Max().X();
  double dBoundMinZ = pModel->Boundary().Min().Z();
  double dBoundMaxZ = pModel->Boundary().Max().Z();

  // keep the point within the model
  KeepInBounds("Screen", "Northing", dBasePointNorthScreen, dBoundMinX, dBoundMaxX);
  KeepInBounds("Screen", "Easting", dBasePointEastScreen, dBoundMinY, dBoundMaxY);
  KeepInBounds("Screen", "Depth", dBasePointDepthScreen, dBoundMinZ, dBoundMaxZ);

  KeepInBounds("Bookmarked", "Northing", dBasePointNorthBookmarked, dBoundMinX, dBoundMaxX);
  KeepInBounds("Bookmarked", "Easting", dBasePointEastBookmarked, dBoundMinY, dBoundMaxY);
  KeepInBounds("Bookmarked", "Depth", dBasePointDepthBookmarked, dBoundMinZ, dBoundMaxZ);

  m_basePointBookmarked = geo::CPoint(dBasePointNorthBookmarked, dBasePointEastBookmarked, dBasePointDepthBookmarked);
  m_basePointScreen = geo::CPoint(dBasePointNorthScreen, dBasePointEastScreen, dBasePointDepthScreen);

  m_normalBookmarked = geo::CVector(dNormalNorthBookmarked, dNormalEastBookmarked, dNormalDepthBookmarked);
  m_normalScreen = geo::CVector(dNormalNorthScreen, dNormalEastScreen, dNormalDepthScreen);

  m_strName = strName;
  }

  DDX_Radio(pDX, IDC_RADIO_VERTICAL, m_crossSectionType);

  double dFirstNorth = 0;
  double dFirstEast = 0;
  double dSecondNorth = 0;
  double dSecondEast = 0;
  double dDepth = 0;
  int iHorizontal;

  if (!pDX->m_bSaveAndValidate)
  {
  switch (globalUnitNode.unitNode().Unit())
  {
  case CQuantity::SI_UNIT:
      strUnit.Format("m");
      break;
  case CQuantity::FIELD_UNIT:
      strUnit.Format("ft");
      break;
  }

  strName = m_strName;
  iHorizontal = m_crossSectionType;
  dFirstNorth = qnLength.Convert(m_ptFirst.X(), globalUnitNode.unitNode().Unit(), CQuantity::SI_UNIT);
  dFirstEast = qnLength.Convert(m_ptFirst.Y(), globalUnitNode.unitNode().Unit(), CQuantity::SI_UNIT);
  dSecondNorth = qnLength.Convert(m_ptSecond.X(), globalUnitNode.unitNode().Unit(), CQuantity::SI_UNIT);
  dSecondEast = qnLength.Convert(m_ptSecond.Y(), globalUnitNode.unitNode().Unit(), CQuantity::SI_UNIT);
  dDepth = qnLength.Convert(m_dDepth, globalUnitNode.unitNode().Unit(), CQuantity::SI_UNIT);
  }

  //{{AFX_DATA_MAP(CAttriXSecOldDlg)
  // NOTE: the ClassWizard will add DDX and DDV calls here
  //}}AFX_DATA_MAP

  DDX_Text(pDX, IDC_EDIT_XSEC_NAME, strName);

  DDX_Radio(pDX, IDC_RADIO_VERTICAL, iHorizontal);

  DDX_Text(pDX, IDC_EDIT_FIRST_NORTH, dFirstNorth);
  DDX_Text(pDX, IDC_EDIT_FIRST_EAST, dFirstEast);
  DDX_Text(pDX, IDC_EDIT_SECOND_NORTH, dSecondNorth);
  DDX_Text(pDX, IDC_EDIT_SECOND_EAST, dSecondEast);

  DDX_Text(pDX, IDC_STATIC_FIRSTPT_NORTHING_UNIT, strUnit);
  DDX_Text(pDX, IDC_STATIC_FIRSTPT_EASTING_UNIT, strUnit);
  DDX_Text(pDX, IDC_STATIC_SECONDPT_NORTHING_UNIT, strUnit);
  DDX_Text(pDX, IDC_STATIC_SECONDPT_EASTING_UNIT, strUnit);

  DDX_Text(pDX, IDC_EDIT_DEPTH, dDepth);

  DDX_Text(pDX, IDC_UNIT_DEPTH, strUnit);

  dFirstNorth = qnLength.Convert(dFirstNorth, CQuantity::SI_UNIT, globalUnitNode.unitNode().Unit());
  dFirstEast = qnLength.Convert(dFirstEast, CQuantity::SI_UNIT, globalUnitNode.unitNode().Unit());
  dSecondNorth = qnLength.Convert(dSecondNorth, CQuantity::SI_UNIT, globalUnitNode.unitNode().Unit());
  dSecondEast = qnLength.Convert(dSecondEast, CQuantity::SI_UNIT, globalUnitNode.unitNode().Unit());
  dDepth = qnLength.Convert(dDepth, CQuantity::SI_UNIT, globalUnitNode.unitNode().Unit());

  if (pDX->m_bSaveAndValidate)
  {
  // Validate your data
  CModelBase *pModel = dynamic_cast<CModelBase*>(&m_xsec.Model());
  assert(pModel);

  if (m_crossSectionType == 0)
  {
      geo::CPoint first(dFirstNorth, dFirstEast);
      geo::CPoint second(dSecondNorth, dSecondEast);

      if (first == second && m_bOk)
      {
    _m()->msg("Please assign two different reference points.");
    pDX->Fail();
    return;
      }

      double dBoundMinY = pModel->Boundary().Min().Y();
      double dBoundMinX = pModel->Boundary().Min().X();
      double dBoundMaxY = pModel->Boundary().Max().Y();
      double dBoundMaxX = pModel->Boundary().Max().X();

      bool bFirstNorth = (((0 <= dFirstNorth - dBoundMinX) && (dFirstNorth - dBoundMinX < EPS)) || (dFirstNorth > dBoundMinX)) &&
    (((0 <= dFirstNorth - dBoundMaxX) && (dFirstNorth - dBoundMaxX < EPS)) || (dFirstNorth < dBoundMaxX));
      bool bFirstEast = (((0 <= dFirstEast - dBoundMinY) && (dFirstEast - dBoundMinY < EPS)) || (dFirstEast > dBoundMinY)) &&
    (((0 <= dFirstEast - dBoundMaxY) && (dFirstEast - dBoundMaxY < EPS)) || (dFirstEast < dBoundMaxY));
      bool bSecondNorth = (((0 <= dSecondNorth - dBoundMinX) && (dSecondNorth - dBoundMinX < EPS)) || (dSecondNorth > dBoundMinX)) &&
    (((0 <= dSecondNorth - dBoundMaxX) && (dSecondNorth - dBoundMaxX < EPS)) || (dSecondNorth < dBoundMaxX));
      bool bSecondEast = (((0 <= dSecondEast - dBoundMinY) && (dSecondEast - dBoundMinY < EPS)) || (dSecondEast > dBoundMinY)) &&
    (((0 <= dSecondEast - dBoundMaxY) && (dSecondEast - dBoundMaxY < EPS)) || (dSecondEast < dBoundMaxY));

      if ((bFirstNorth && bFirstEast && bSecondNorth && bSecondEast) || !m_bOk)
      {
    m_ptFirst = first;
    m_ptSecond = second;
      }
      else
      {
    if (!bFirstNorth)
          pDX->PrepareEditCtrl(IDC_EDIT_FIRST_NORTH);
    else if (!bFirstEast)
          pDX->PrepareEditCtrl(IDC_EDIT_FIRST_EAST);
    else if (!bSecondNorth)
          pDX->PrepareEditCtrl(IDC_EDIT_SECOND_NORTH);
    else if (!bSecondEast)
          pDX->PrepareEditCtrl(IDC_EDIT_SECOND_EAST);

    _m()->msg("One or both of the reference points is outside the model");
    pDX->Fail();
    return;
      }
  }
  else if (m_crossSectionType == 1)
  {
      double dBoundMinZ = pModel->Boundary().Min().Z();
      double dBoundMaxZ = pModel->Boundary().Max().Z();

      if ((dDepth < dBoundMinZ || dDepth > dBoundMaxZ) && m_bOk)
      {
    pDX->PrepareEditCtrl(IDC_EDIT_DEPTH);
    _m()->msg("The depth value is not within the vertical limits of the model");
    pDX->Fail();
    return;
      }

      m_dDepth = dDepth;
  }

  m_crossSectionType = iHorizontal;
  m_strName = strName;
  }
}

BEGIN_MESSAGE_MAP(CAttriXSecDlg, CDialog)
  //{{AFX_MSG_MAP(CAttriXSecDlg)
  //}}AFX_MSG_MAP
  ON_BN_CLICKED(IDC_BUT_EXPORT, &CAttriXSecDlg::OnCopyFromBookmarked)
  ON_BN_CLICKED(IDC_BUT_IMPORT, &CAttriXSecDlg::OnCopyFromScreen)
  ON_BN_CLICKED(IDC_RADIO_SCREEN_DEPTH, &CAttriXSecDlg::OnBnClickedRadioScreenDepth)
  ON_BN_CLICKED(IDC_RADIO_SCREEN_NE, &CAttriXSecDlg::OnBnClickedRadioScreenNe)
  ON_BN_CLICKED(IDC_RADIO_SCREEN_NORTHING, &CAttriXSecDlg::OnBnClickedRadioScreenNorthing)
  ON_BN_CLICKED(IDC_RADIO_SCREEN_EASTING, &CAttriXSecDlg::OnBnClickedRadioScreenEasting)
  ON_BN_CLICKED(IDC_RADIO_SCREEN_CURRENT, &CAttriXSecDlg::OnBnClickedRadioScreenCurrent)
  ON_BN_CLICKED(IDC_RADIO_SCREEN_ANYDIR, &CAttriXSecDlg::OnBnClickedRadioScreenAnydir)
  ON_BN_CLICKED(IDC_RADIO_BM_DEPTH, &CAttriXSecDlg::OnBnClickedRadioBmDepth)
  ON_BN_CLICKED(IDC_RADIO_BM_NE, &CAttriXSecDlg::OnBnClickedRadioBmNe)
  ON_BN_CLICKED(IDC_RADIO_BM_NORTHING, &CAttriXSecDlg::OnBnClickedRadioBmNorthing)
  ON_BN_CLICKED(IDC_RADIO_BM_EASTING, &CAttriXSecDlg::OnBnClickedRadioBmEasting)
  ON_BN_CLICKED(IDC_RADIO_BM_CURRENT, &CAttriXSecDlg::OnBnClickedRadioBmCurrent)
  ON_BN_CLICKED(IDC_RADIO_BM_ANYDIR, &CAttriXSecDlg::OnBnClickedRadioBmAnydir)
  ON_BN_CLICKED(IDAPPLY, &CAttriXSecDlg::OnApply)
  ON_BN_CLICKED(IDC_RADIO_VERTICAL, &CAttriXSecDlg::OnRadioVertical)
  ON_BN_CLICKED(IDC_RADIO_HORIZONTAL, &CAttriXSecDlg::OnRadioHorizontal)
  ON_BN_CLICKED(IDC_RADIO_MANUAL, &CAttriXSecDlg::OnRadioManual)
  ON_BN_CLICKED(ID_XSEC_OK, &CAttriXSecDlg::OnBnClickedOk)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CAttriXSecDlg message handlers

BOOL CAttriXSecDlg::OnInitDialog()
{
  CDialog::OnInitDialog();

  UpdateControls();

  return TRUE;
}

// no longer used by the ok button, only by the edit controls...
void CAttriXSecDlg::OnOK()
{
  UpdateData(TRUE);
  UpdateData(FALSE);
  UpdateControls();
}

void CAttriXSecDlg::OnApply()
{
   if (TryApply())
   {
       m_normalScreenSaved = m_normalScreen;
       m_normalBookmarkedSaved = m_normalBookmarked;
   }

}

bool CAttriXSecDlg::TryApply()
{
  m_bOk = true;

  bool applied = false;

  UpdateData(TRUE);
  UpdateData(FALSE);
  UpdateControls();

    m_xsec.Name((LPCSTR) m_strName);
  m_xsec.crossSectionType(
      CCrossSection::TCrossSectionType(m_crossSectionType));

  switch (m_crossSectionType)
  {
      case 0:
    m_xsec.SetBookmarked(m_ptFirst, m_ptSecond);
    m_xsec.SetScreenFromBookmarked();
    m_xsec.Clear();
    break;
      case 1:
    m_xsec.SetBookmarked(m_dDepth);
    m_xsec.SetScreenFromBookmarked();
    m_xsec.Clear();
    break;
      case 2:
    m_xsec.SetScreen(m_basePointScreen, m_normalScreen, m_constraintScreen);
    m_xsec.SetBookmarked(m_basePointBookmarked, m_normalBookmarked,
          m_constraintBookmarked);
    m_xsec.Clear();
    break;
      default:
    assert(false);
    break;
  }

  applied = true;


  m_bOk = false;

  return applied;
}

void CAttriXSecDlg::UpdateControls()
{
  GetDlgItem(IDC_LBL_FIRST_NORTHING)->EnableWindow(m_crossSectionType == 0);
  GetDlgItem(IDC_EDIT_FIRST_NORTH)->EnableWindow(m_crossSectionType == 0);
  GetDlgItem(IDC_STATIC_FIRSTPT_NORTHING_UNIT)->EnableWindow(m_crossSectionType == 0);
  GetDlgItem(IDC_LBL_FIRST_EASTING)->EnableWindow(m_crossSectionType == 0);
  GetDlgItem(IDC_EDIT_FIRST_EAST)->EnableWindow(m_crossSectionType == 0);
  GetDlgItem(IDC_STATIC_FIRSTPT_EASTING_UNIT)->EnableWindow(m_crossSectionType == 0);

  GetDlgItem(IDC_EDIT_SECOND_NORTH)->EnableWindow(m_crossSectionType == 0);
  GetDlgItem(IDC_STATIC_SECONDPT_NORTHING_UNIT)->EnableWindow(m_crossSectionType == 0);
  GetDlgItem(IDC_EDIT_SECOND_EAST)->EnableWindow(m_crossSectionType == 0);
  GetDlgItem(IDC_STATIC_SECONDPT_EASTING_UNIT)->EnableWindow(m_crossSectionType == 0);

  GetDlgItem(IDC_LBL_DEPTH)->EnableWindow(m_crossSectionType == 1);
  GetDlgItem(IDC_EDIT_DEPTH)->EnableWindow(m_crossSectionType == 1);
  GetDlgItem(IDC_UNIT_DEPTH)->EnableWindow(m_crossSectionType == 1);

  GetDlgItem(IDC_RADIO_SCREEN_DEPTH)->EnableWindow(m_crossSectionType == 2);
  GetDlgItem(IDC_RADIO_SCREEN_NE)->EnableWindow(m_crossSectionType == 2);
  GetDlgItem(IDC_RADIO_SCREEN_ANYDIR)->EnableWindow(m_crossSectionType == 2);
  GetDlgItem(IDC_RADIO_SCREEN_NORTHING)->EnableWindow(m_crossSectionType == 2);
  GetDlgItem(IDC_RADIO_SCREEN_EASTING)->EnableWindow(m_crossSectionType == 2);
  GetDlgItem(IDC_RADIO_SCREEN_CURRENT)->EnableWindow(m_crossSectionType == 2);

  GetDlgItem(IDC_RADIO_BM_DEPTH)->EnableWindow(m_crossSectionType == 2);
  GetDlgItem(IDC_RADIO_BM_NE)->EnableWindow(m_crossSectionType == 2);
  GetDlgItem(IDC_RADIO_BM_ANYDIR)->EnableWindow(m_crossSectionType == 2);
  GetDlgItem(IDC_RADIO_BM_NORTHING)->EnableWindow(m_crossSectionType == 2);
  GetDlgItem(IDC_RADIO_BM_EASTING)->EnableWindow(m_crossSectionType == 2);
  GetDlgItem(IDC_RADIO_BM_CURRENT)->EnableWindow(m_crossSectionType == 2);

  GetDlgItem(IDC_EDIT_SCREEN_BASEPOINT_NORTHING)->EnableWindow(m_crossSectionType == 2);
  GetDlgItem(IDC_EDIT_SCREEN_BASEPOINT_EASTING)->EnableWindow(m_crossSectionType == 2);
  GetDlgItem(IDC_EDIT_SCREEN_BASEPOINT_DEPTH)->EnableWindow(m_crossSectionType == 2);

  GetDlgItem(IDC_EDIT_BM_BASEPOINT_NORTHING)->EnableWindow(m_crossSectionType == 2);
  GetDlgItem(IDC_EDIT_BM_BASEPOINT_EASTING)->EnableWindow(m_crossSectionType == 2);
  GetDlgItem(IDC_EDIT_BM_BASEPOINT_DEPTH)->EnableWindow(m_crossSectionType == 2);

  GetDlgItem(IDC_EDIT_SCREEN_NORMAL_NORTHING)->EnableWindow((m_crossSectionType == 2) &&
  OrientationIsSet(m_constraintScreen, 4, CCrossSection::ORIENTATION_ANY, CCrossSection::ORIENTATION_CURRENT, CCrossSection::ORIENTATION_NE, CCrossSection::ORIENTATION_NORTHING));
  GetDlgItem(IDC_EDIT_SCREEN_NORMAL_EASTING)->EnableWindow((m_crossSectionType == 2) &&
  OrientationIsSet(m_constraintScreen, 4, CCrossSection::ORIENTATION_ANY, CCrossSection::ORIENTATION_CURRENT, CCrossSection::ORIENTATION_NE, CCrossSection::ORIENTATION_EASTING));
  GetDlgItem(IDC_EDIT_SCREEN_NORMAL_DEPTH)->EnableWindow((m_crossSectionType == 2) &&
  OrientationIsSet(m_constraintScreen, 3, CCrossSection::ORIENTATION_ANY, CCrossSection::ORIENTATION_CURRENT, CCrossSection::ORIENTATION_DEPTH));

  GetDlgItem(IDC_EDIT_BM_NORMAL_NORTHING)->EnableWindow((m_crossSectionType == 2) &&
  OrientationIsSet(m_constraintBookmarked, 4, CCrossSection::ORIENTATION_ANY, CCrossSection::ORIENTATION_CURRENT, CCrossSection::ORIENTATION_NE, CCrossSection::ORIENTATION_NORTHING));
  GetDlgItem(IDC_EDIT_BM_NORMAL_EASTING)->EnableWindow((m_crossSectionType == 2) &&
  OrientationIsSet(m_constraintBookmarked, 4, CCrossSection::ORIENTATION_ANY, CCrossSection::ORIENTATION_CURRENT, CCrossSection::ORIENTATION_NE, CCrossSection::ORIENTATION_EASTING));
  GetDlgItem(IDC_EDIT_BM_NORMAL_DEPTH)->EnableWindow((m_crossSectionType == 2) &&
  OrientationIsSet(m_constraintBookmarked, 3, CCrossSection::ORIENTATION_ANY, CCrossSection::ORIENTATION_CURRENT, CCrossSection::ORIENTATION_DEPTH));

  GetDlgItem(IDC_BUT_IMPORT)->EnableWindow(m_crossSectionType == 2);
  GetDlgItem(IDC_BUT_EXPORT)->EnableWindow(m_crossSectionType == 2);

  UpdateData(FALSE);
}


void CAttriXSecDlg::OnCopyFromBookmarked()
{
  UpdateData(TRUE);

  m_basePointScreen = m_basePointBookmarked;
  m_normalScreen = m_normalBookmarked;
  m_normalScreenSaved = m_normalBookmarked;
  m_constraintScreen = m_constraintBookmarked;
  UpdateData(FALSE);
  UpdateControls();
}

void CAttriXSecDlg::OnCopyFromScreen()
{
  UpdateData(TRUE);

  m_basePointBookmarked = m_basePointScreen;
  m_normalBookmarked = m_normalScreen;
  m_normalBookmarkedSaved = m_normalScreen;
  m_constraintBookmarked = m_constraintScreen;

  UpdateData(FALSE);
  UpdateControls();
}



void CAttriXSecDlg::OnBnClickedRadioScreenDepth()
{
  UpdateData(TRUE);
  m_normalScreen = geo::CVector(0, 0, 1);
  UpdateData(FALSE);
  UpdateControls();
}


void CAttriXSecDlg::OnBnClickedRadioScreenNe()
{
  UpdateData(TRUE);
  m_normalScreen = geo::CVector(m_normalScreenSaved.X(), m_normalScreenSaved.Y(), 0);
  if (m_normalScreen.X() == 0 && m_normalScreen.Y() == 0)
  {
    m_normalScreen = geo::CVector(1, 1, 0);
  }
  UpdateData(FALSE);
  UpdateControls();
}


void CAttriXSecDlg::OnBnClickedRadioScreenNorthing()
{
  UpdateData(TRUE);
  m_normalScreen = geo::CVector(1, 0, 0);
  UpdateData(FALSE);
  UpdateControls();
}


void CAttriXSecDlg::OnBnClickedRadioScreenEasting()
{
  UpdateData(TRUE);
  m_normalScreen = geo::CVector(0, 1, 0);
  UpdateData(FALSE);
  UpdateControls();
}


void CAttriXSecDlg::OnBnClickedRadioScreenCurrent()
{
  UpdateData(TRUE);
  m_normalScreen = m_normalScreenSaved;
  UpdateData(FALSE);
  UpdateControls();
}


void CAttriXSecDlg::OnBnClickedRadioScreenAnydir()
{
  UpdateData(TRUE);
  m_normalScreen = m_normalScreenSaved;
  UpdateData(FALSE);
  UpdateControls();
}


void CAttriXSecDlg::OnBnClickedRadioBmDepth()
{
  UpdateData(TRUE);
  m_normalBookmarked = geo::CVector(0, 0, 1);
  UpdateData(FALSE);
  UpdateControls();
}


void CAttriXSecDlg::OnBnClickedRadioBmNe()
{
  UpdateData(TRUE);
  m_normalBookmarked = geo::CVector(m_normalBookmarkedSaved.X(), m_normalBookmarkedSaved.Y(), 0);
  if (m_normalBookmarked.X() == 0 && m_normalBookmarked.Y() == 0)
  {
    m_normalBookmarked = geo::CVector(1, 1, 0);
  }
  UpdateData(FALSE);
  UpdateControls();
}


void CAttriXSecDlg::OnBnClickedRadioBmNorthing()
{
  UpdateData(TRUE);
  m_normalBookmarked = geo::CVector(1, 0, 0);
  UpdateData(FALSE);
  UpdateControls();
}


void CAttriXSecDlg::OnBnClickedRadioBmEasting()
{
  UpdateData(TRUE);
  m_normalBookmarked = geo::CVector(0, 1, 0);
  UpdateData(FALSE);
  UpdateControls();
}


void CAttriXSecDlg::OnBnClickedRadioBmCurrent()
{
  UpdateData(TRUE);
  m_normalBookmarked = m_normalBookmarkedSaved;
  UpdateData(FALSE);
  UpdateControls();
}


void CAttriXSecDlg::OnBnClickedRadioBmAnydir()
{
  UpdateData(TRUE);
  m_normalBookmarked = m_normalBookmarkedSaved;
  UpdateData(FALSE);
  UpdateControls();
}

void CAttriXSecDlg::OnRadioVertical()
{
  UpdateData(TRUE);
  m_crossSectionType = 0;
  UpdateControls();
}

void CAttriXSecDlg::OnRadioHorizontal()
{
  UpdateData(TRUE);
  m_crossSectionType = 1;
  UpdateControls();
}

void CAttriXSecDlg::OnRadioManual()
{
  UpdateData(TRUE);
  m_crossSectionType = 2;
  UpdateControls();
}

void CAttriXSecDlg::OnBnClickedOk()
{
  // TODO: Add your control notification handler code here
  if (TryApply())
  {
    EndDialog(IDOK);
  }
}


void KeepInBounds(const char * basePoint, const char * coordinate, double &value, double min, double max )
{

  if (value < min || value > max)
  {
    if (std::fabs (value - min) > 0.1 && std::fabs (value - max) > 0.1) // otherwise, adapt quietly
    {
      char message[256];
      sprintf (message, "Coordinate %s of Base Point %s is out of model bounds, moving it in bounds again", coordinate, basePoint);
      _m()->msg(message);
    }

    value = std::max(value, min);
    value = std::min(value, max);
  }
}

bool OrientationIsSet (CCrossSection::OrientationType variable, int count, ...)
{
  va_list vl;
  int i;
  va_start(vl, count);

  for (i = 0; i < count; ++i)
  {
    CCrossSection::OrientationType value = va_arg(vl, CCrossSection::OrientationType);
    if (variable == value) return true;
  }
  return false;
}


