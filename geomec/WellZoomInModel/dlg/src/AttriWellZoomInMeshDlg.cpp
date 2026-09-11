#include "stdafx.h"
#include "geomec.h"

#include "AttriWellZoomInMeshDlg.h"
#include "GlobalMessage.h"
#include "FieldFactor.h"

static bool IsIntValue(const CString& str)
{
  int i;
  for(i = 0; i < str.GetLength(); ++i)
  {
  if(!isdigit(str[i]))
  {
      return false;
  }
  }

  return true;
}

CAttriWellZoomInMeshDlg::CAttriWellZoomInMeshDlg(CWellZoomInMesh& src, CWnd* pParent)
: CAttributesTemplate<CWellZoomInMesh>::CAttributesTemplate(IDD_ATTRI_WELL_ZOOMIN_MESH, src, pParent)
{
}

BOOL CAttriWellZoomInMeshDlg::OnInitDialog()
{
  CAttributesTemplate<CWellZoomInMesh>::OnInitDialog();
  UpdateControls();
  return TRUE;
}

void CAttriWellZoomInMeshDlg::DoDataExchange(CDataExchange* pDX)
{
  CAttributesTemplate<CWellZoomInMesh>::DoDataExchange(pDX);

  CString strUnit;
  int nType;
  int nRadialElements;
  double dTargetLongElementSize;
  double dRadius;
  double dTargetEdgeSize;
  int nNodes;
  int nElements;

  if(!pDX->m_bSaveAndValidate)
  {
  nType = Copy().isHexaMesh() ? 0 : 1;

  nNodes = Source().Mesh().NodeSize();
  nElements = Source().Mesh().ElementSize();
  BOOL bIsMesh = (Source().IsMesh() != false);

  GetDlgItem(IDC_FRAME_INFO)->EnableWindow(bIsMesh);
  GetDlgItem(IDC_LBL_NUM_NODES)->EnableWindow(bIsMesh);
  GetDlgItem(IDC_VAL_NUM_NODES)->EnableWindow(bIsMesh);
  GetDlgItem(IDC_LBL_NUM_ELEMENTS)->EnableWindow(bIsMesh);
  GetDlgItem(IDC_VAL_NUM_ELEMENTS)->EnableWindow(bIsMesh);

  nRadialElements = Copy().NrOfRadialElements();
  dTargetLongElementSize = Copy().TargetLongitudinalElementSize();
  dRadius = Copy().TetraRadius();
  dTargetEdgeSize = Copy().TetraIdealElementSize();

  if (UnitNode().Unit() == IQuantityDouble::FIELD_UNIT)
  {
      dTargetLongElementSize *= FF_FACTOR_LENGTH;
      dRadius *= FF_FACTOR_LENGTH;
      dTargetEdgeSize *= FF_FACTOR_LENGTH;
  }

  strUnit = CLengthQuantity().UnitName(UnitNode().Unit()).c_str();
  }

  DDX_Text(pDX, IDC_UN_TARGET_LONG_ELEMENT_SIZE, strUnit);
  DDX_Text(pDX, IDC_UN_RADIUS, strUnit);
  DDX_Text(pDX, IDC_UN_TARGET_EDGE_SIZE, strUnit);

  DDX_Text(pDX, IDC_ED_RADIAL_ELEMENTS, nRadialElements);
  DDX_Text(pDX, IDC_ED_TARGET_LONG_ELEMENT_SIZE, dTargetLongElementSize);
  DDX_Text(pDX, IDC_ED_RADIUS, dRadius);
  DDX_Text(pDX, IDC_ED_TARGET_EDGE_SIZE, dTargetEdgeSize);
  DDX_Text(pDX, IDC_VAL_NUM_NODES, nNodes);
  DDX_Text(pDX, IDC_VAL_NUM_ELEMENTS, nElements);

  DDX_Radio(pDX, IDC_RADIO_HEXA, nType);

  if(pDX->m_bSaveAndValidate)
  {
  CString str;
  DDX_Text(pDX, IDC_ED_RADIAL_ELEMENTS, str);
  if(nRadialElements <= 0 || !IsIntValue(str))
  {
      pDX->PrepareEditCtrl(IDC_ED_RADIAL_ELEMENTS);
      _m()->msg("The number of radial elements must be an integer value greater than 0");
      pDX->Fail();
  }

  if (UnitNode().Unit() == IQuantityDouble::FIELD_UNIT)
  {
      dTargetLongElementSize /= FF_FACTOR_LENGTH;
      dRadius /= FF_FACTOR_LENGTH;
      dTargetEdgeSize /= FF_FACTOR_LENGTH;
  }

  if(dTargetLongElementSize <= 0)
  {
      pDX->PrepareEditCtrl(IDC_ED_TARGET_LONG_ELEMENT_SIZE);
      _m()->msg("The target longitudinal element size must be greater than 0");
      pDX->Fail();
  }

  if (dRadius <= 0)
  {
      pDX->PrepareEditCtrl(IDC_ED_RADIUS);
      _m()->msg("The radius must be greater than 0");
      pDX->Fail();
  }

  if (dTargetEdgeSize <= 0)
  {
      pDX->PrepareEditCtrl(IDC_ED_TARGET_EDGE_SIZE);
      _m()->msg("The target edge size must be greater than 0");
      pDX->Fail();
  }

  Copy().NrOfRadialElements(nRadialElements);
  Copy().TargetLongitudinalElementSize(dTargetLongElementSize);
  Copy().TetraRadius(dRadius);
  Copy().TetraIdealElementSize(dTargetEdgeSize);
  }
}

BEGIN_MESSAGE_MAP(CAttriWellZoomInMeshDlg, CDialog)
  ON_BN_CLICKED(IDC_RADIO_HEXA, OnMeshTypeHexa)
  ON_BN_CLICKED(IDC_RADIO_TETRA, OnMeshTypeTetra)
END_MESSAGE_MAP()

void CAttriWellZoomInMeshDlg::OnMeshTypeHexa()
{
  UpdateData(TRUE);
  Copy().setHexaMesh(true);
  UpdateControls();
}

void CAttriWellZoomInMeshDlg::OnMeshTypeTetra()
{
  UpdateData(TRUE);
  Copy().setHexaMesh(false);
  UpdateControls();
}

void CAttriWellZoomInMeshDlg::UpdateControls()
{
  GetDlgItem(IDC_ED_RADIAL_ELEMENTS)->EnableWindow(Copy().isHexaMesh());
  GetDlgItem(IDC_ED_TARGET_LONG_ELEMENT_SIZE)->EnableWindow(Copy().isHexaMesh());
  GetDlgItem(IDC_ED_RADIUS)->EnableWindow(Copy().isTetraMesh());
  GetDlgItem(IDC_ED_TARGET_EDGE_SIZE)->EnableWindow(Copy().isTetraMesh());
}

