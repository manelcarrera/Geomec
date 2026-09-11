#include "stdafx.h"
#include "Geomec.h"

#include "AttriWellCasingMeshDlg.h"
#include "WellCasingMesh.h"
#include "FieldFactor.h"
#include "IWellModel.h"
#include "GlobalMessage.h"
#include "WellCasingModel.h"

typedef struct
{
  const char* title;
  const double diameter;
} TCasingType;

static TCasingType casingtypes[] =
{
  { "7\"", 0.1778 },
  { "10\"", 0.254 },
  { 0, 0 } // terminator
};

typedef struct
{
  const char* title;
  const int numelements;
  const bool supported;
} TTangentialElementsType;

static TTangentialElementsType tangentialelementtypes[] =
{
  { "8", 8, true },
  { "16", 16, true },
  { "24", 24, true },
  { "32", 32, false }, // for new casing models, we don't offer this setting: Diana is unlikely to converge
  { 0, 0, true } // terminator
};

CAttriWellCasingMeshDlg::CAttriWellCasingMeshDlg(CWellCasingMesh& mesh, CWnd* pParent /*=NULL*/)
: CAttributesTemplate<CWellCasingMesh>(IDD_ATTRI_CASINGMESH, mesh, pParent)
, m_numberOfSteelElements(Source().numberOfSteelElements())
, m_numberOfCementElements(Source().numberOfCementElements())
, m_spinNumberOfSteelElements()
, m_spinNumberOfCementElements()
, m_outerDiameterRock(Source().outerDiameterRock())
{
}

CAttriWellCasingMeshDlg::~CAttriWellCasingMeshDlg()
{
}

namespace
{

const short MINIMUM_NUMBER_OF_STEEL_ELEMENTS = 1;
const short MAXIMUM_NUMBER_OF_STEEL_ELEMENTS = 4;

const short MINIMUM_NUMBER_OF_CEMENT_ELEMENTS = 1;
const short MAXIMUM_NUMBER_OF_CEMENT_ELEMENTS = 2;

} // anonymous namespace

BOOL CAttriWellCasingMeshDlg::OnInitDialog()
{
  CAttributesTemplate<CWellCasingMesh>::OnInitDialog();

  int i;

  CComboBox* pOuterCasingDiameter = (CComboBox*)GetDlgItem(IDC_CBO_CASINGDIAM);
  for(i = 0; casingtypes[i].title != 0; ++i)
  pOuterCasingDiameter->AddString(FormattedDiameter(i));

  CComboBox* pTangentialElements = (CComboBox*)GetDlgItem(IDC_CBO_NUMTANGENT);
  for(i = 0; tangentialelementtypes[i].title != 0; ++i)
  if (tangentialelementtypes[i].supported || Copy().TangentialElements() == tangentialelementtypes[i].numelements)
      pTangentialElements->AddString(tangentialelementtypes[i].title);

  m_spinNumberOfSteelElements.SetRange(
  MINIMUM_NUMBER_OF_STEEL_ELEMENTS, MAXIMUM_NUMBER_OF_STEEL_ELEMENTS);
  m_spinNumberOfCementElements.SetRange(
  MINIMUM_NUMBER_OF_CEMENT_ELEMENTS, MAXIMUM_NUMBER_OF_CEMENT_ELEMENTS);

  UpdateData(FALSE);

  return TRUE;
}

namespace
{

const QString OUTER_DIAMETER_ROCK_ERROR = QObject::tr("The outer diameter of "
  "rock should be greater or equal than the hole diameter and less or equal "
  "than the well diameter (%1%2)");

} // anonymous namespace

void CAttriWellCasingMeshDlg::DoDataExchange(CDataExchange* pDX)
{
  CAttributesTemplate<CWellCasingMesh>::DoDataExchange(pDX);

  CString strCasingDiameter;
  double dCasingThickness;
  double dHoleDiameter;
  double outerDiameterRock;
  int nNumRadial;
  int nIdxNumTangential = 0;
  double dTargetElementSize;
  int nNodes;
  int nElements;

  CString strUnitLength;
  CLengthQuantity lengthQuantity;

  if(!pDX->m_bSaveAndValidate)
  {
  double dCasingDiameter = Copy().CasingDiameter();
  int i;
  for(i = 0; casingtypes[i].title != 0; ++i)
  {
      if(fabs(dCasingDiameter - casingtypes[i].diameter) < 1e-6)
      {
    strCasingDiameter = FormattedDiameter(i);
    break;
      }
  }

  if(strCasingDiameter.IsEmpty())
      strCasingDiameter.Format("%f", dCasingDiameter);

  for(i = 0; tangentialelementtypes[i].title != 0; ++i)
  {
      if(tangentialelementtypes[i].numelements == Copy().TangentialElements())
      {
    nIdxNumTangential = i;
    break;
      }
  }

  dCasingThickness = Copy().CasingThickness();
  dHoleDiameter = Copy().HoleDiameter();
  outerDiameterRock = Copy().outerDiameterRock();
  nNumRadial = Copy().RadialElements();
  dTargetElementSize = Copy().TargetElementSize();

  if(GetGeomecDoc()->UnitNode().Unit() == IQuantityDouble::FIELD_UNIT)
  {
      dCasingThickness *= FF_FACTOR_LENGTH;
      dHoleDiameter *= FF_FACTOR_LENGTH;
      outerDiameterRock *= FF_FACTOR_LENGTH;
      dTargetElementSize *= FF_FACTOR_LENGTH;
  }

  strUnitLength = lengthQuantity.UnitName(GetGeomecDoc()->UnitNode().Unit()).c_str();

  GetDlgItem(IDC_FRA_CURRENTMESH)->EnableWindow(Source().IsMesh());
  GetDlgItem(IDC_LBL_NUMNODES)->EnableWindow(Source().IsMesh());
  GetDlgItem(IDC_LBL_NUMNODES_VALUE)->EnableWindow(Source().IsMesh());
  GetDlgItem(IDC_LBL_NUMELEMENTS)->EnableWindow(Source().IsMesh());
  GetDlgItem(IDC_LBL_NUMELEMENTS_VALUE)->EnableWindow(Source().IsMesh());

  nNodes = Source().IsMesh() ? Source().Mesh().NodeSize() : 0;
  nElements = Source().IsMesh() ? Source().Mesh().ElementSize() : 0;
  }

  DDX_Text(pDX, IDC_UN_CASINGDIAM, strUnitLength);
  DDX_Text(pDX, IDC_UN_CASINGTHICKNESS, strUnitLength);
  DDX_Text(pDX, IDC_UN_HOLEDIAM, strUnitLength);
  DDX_Text(pDX, IDC_UN_LONGELMSIZE, strUnitLength);

  DDX_CBString(pDX, IDC_CBO_CASINGDIAM, strCasingDiameter);
  DDX_Text(pDX, IDC_ED_CASINGTHICKNESS, dCasingThickness);
  DDX_Text(pDX, IDC_ED_HOLEDIAM, dHoleDiameter);
  DDX_Text(pDX, IDC_ED_NUMRADIAL, nNumRadial);
  DDX_CBIndex(pDX, IDC_CBO_NUMTANGENT, nIdxNumTangential);
  DDX_Text(pDX, IDC_ED_LONGELMSIZE, dTargetElementSize);

  DDX_Text(pDX, IDC_LBL_NUMNODES_VALUE, nNodes);
  DDX_Text(pDX, IDC_LBL_NUMELEMENTS_VALUE, nElements);

  DDX_Text(pDX, IDC_NUMBER_OF_STEEL_ELEMENTS, m_numberOfSteelElements);
  DDX_Text(pDX, IDC_NUMBER_OF_CEMENT_ELEMENTS, m_numberOfCementElements);

  DDX_Control(
  pDX, IDC_SPIN_NUMBER_OF_STEEL_ELEMENTS, m_spinNumberOfSteelElements);
  DDX_Control(
  pDX, IDC_SPIN_NUMBER_OF_CEMENT_ELEMENTS, m_spinNumberOfCementElements);

  DDX_Text(pDX, IDC_OUTER_DIAMETER_ROCK, outerDiameterRock);
  DDX_Text(pDX, IDC_UNIT_OUTER_DIAMETER_ROCK, strUnitLength);

  if(pDX->m_bSaveAndValidate)
  {
  double dCasingDiameter;
  bool bFound = false;
  int i;
  for(i = 0; casingtypes[i].title != 0; ++i)
  {
      if(strCasingDiameter == FormattedDiameter(i))
      {
    dCasingDiameter = casingtypes[i].diameter;
    bFound = true;
    break;
      }
  }

  if(!bFound)
  {
      QString qs(strCasingDiameter);
      bool ok;
      dCasingDiameter = qs.toDouble(&ok);
      if(!ok)
      {
    CString str;
    str.Format("'%s' is not a valid outer casing diameter", strCasingDiameter);
    _m()->msg(str);
    pDX->PrepareEditCtrl(IDC_CBO_CASINGDIAM);
    pDX->Fail();
      }

      if(GetGeomecDoc()->UnitNode().Unit() == IQuantityDouble::FIELD_UNIT)
    dCasingDiameter /= FF_FACTOR_LENGTH;
  }

  const IWellModel& model = static_cast<const IWellModel&>(Copy().Model());

  if(dCasingDiameter >= model.Radius())
  {
      _m()->msg("The outer casing diameter must be smaller than the model radius");
      pDX->PrepareEditCtrl(IDC_CBO_CASINGDIAM);
      pDX->Fail();
  }

  Copy().CasingDiameter(dCasingDiameter);

  if (!tangentialelementtypes[nIdxNumTangential].supported)
  {
      QString str = QString("Warning: %1 tangential elements is a legacy value and will probably lead to divergence in the calculation").arg(tangentialelementtypes[nIdxNumTangential].numelements);
      _m()->msg(str);
  }

  Copy().TangentialElements(tangentialelementtypes[nIdxNumTangential].numelements);

  if(GetGeomecDoc()->UnitNode().Unit() == IQuantityDouble::FIELD_UNIT)
  {
      dCasingThickness /= FF_FACTOR_LENGTH;
      dHoleDiameter /= FF_FACTOR_LENGTH;
      outerDiameterRock /= FF_FACTOR_LENGTH;
      dTargetElementSize /= FF_FACTOR_LENGTH;
  }

  if(dCasingThickness <= 0)
  {
      _m()->msg("The casing thickness must be greater than 0");
      pDX->PrepareEditCtrl(IDC_ED_CASINGTHICKNESS);
      pDX->Fail();
  }

  if(dCasingThickness >= dCasingDiameter / 2)
  {
      _m()->msg("The casing thickness must be less than half the outer casing diameter");
      pDX->PrepareEditCtrl(IDC_ED_CASINGTHICKNESS);
      pDX->Fail();
  }

  Copy().CasingThickness(dCasingThickness);

  if(dHoleDiameter <= dCasingDiameter)
  {
      _m()->msg("The drill hole diameter must be greater than the outer casing diameter");
      pDX->PrepareEditCtrl(IDC_ED_HOLEDIAM);
      pDX->Fail();
  }

  if(dHoleDiameter >= model.Radius())
  {
      _m()->msg("The drill hole diameter must be smaller than the model radius");
      pDX->PrepareEditCtrl(IDC_ED_HOLEDIAM);
      pDX->Fail();
  }

  Copy().HoleDiameter(dHoleDiameter);

  if(nNumRadial < 5 || nNumRadial > 50)
  {
      _m()->msg("The number of rock elements in radial direction must be at least 5 and at most 50");
      pDX->PrepareEditCtrl(IDC_ED_NUMRADIAL);
      pDX->Fail();
  }

  Copy().RadialElements(nNumRadial);

  if(dTargetElementSize <= 0)
  {
      _m()->msg("The target longitudinal element size must be greater than 0");
      pDX->PrepareEditCtrl(IDC_ED_LONGELMSIZE);
      pDX->Fail();
  }

  Copy().TargetElementSize(dTargetElementSize);

  Copy().numberOfSteelElements(m_numberOfSteelElements);
  Copy().numberOfCementElements(m_numberOfCementElements);

  if ((Copy().HoleDiameter() > outerDiameterRock) ||
      (outerDiameterRock >
    (2 * (dynamic_cast <CWellCasingModel&> (Copy().Model()).Radius()))))
  {
      QString errorMessage = QString(OUTER_DIAMETER_ROCK_ERROR).
    arg(CLengthQuantity(2 * (dynamic_cast <CWellCasingModel&> (
          Copy().Model()).Radius())).Value(GetGeomecDoc()->UnitNode().Unit())).
          arg(lengthQuantity.UnitName(GetGeomecDoc()->UnitNode().Unit()).
      c_str());

      _m()->msg(errorMessage);
      pDX->PrepareEditCtrl(IDC_OUTER_DIAMETER_ROCK);
      pDX->Fail();
  }

  Copy().outerDiameterRock(outerDiameterRock);
  }
}


BEGIN_MESSAGE_MAP(CAttriWellCasingMeshDlg, CDialog)
END_MESSAGE_MAP()

CString CAttriWellCasingMeshDlg::FormattedDiameter(int nIndex) const
{
  double dDiameter = casingtypes[nIndex].diameter;
  if(GetGeomecDoc()->UnitNode().Unit() == IQuantityDouble::FIELD_UNIT)
  dDiameter *= FF_FACTOR_LENGTH;

  CString str;
  str.Format("%s: %.4f", casingtypes[nIndex].title, dDiameter);

  return str;
}
