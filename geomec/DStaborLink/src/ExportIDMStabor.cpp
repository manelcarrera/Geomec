//
// wjrx mantis 3097
//

#include "ExportIDMStabor.h"
#include "GlobalMessage.h"
#include "MaterialServer.h"
#include "ModelBase.h"
#include "NewWellPath.h"
#include "ResultTree.h"
#include "ValueTypes.h"
#include <fstream>
#include <stdafx.h>

std::vector<std::string> CExportIDMStabor::m_fieldNames;
std::vector<std::string> CExportIDMStabor::m_units;
bool CExportIDMStabor::m_classInited = false;

CExportIDMStabor::CExportIDMStabor(const CModelBase &model, const CDepletionStage &stage, const CAnalysisType &antype,
                                   const std::list<CNewWellPoint> *pNewPointList)
    : m_model(model), m_stage(stage), m_antype(antype), m_pNewPointList(pNewPointList) {
  assert(m_pNewPointList != 0);

  if (CExportIDMStabor::m_classInited == true)
    return;
  CExportIDMStabor::m_classInited = true;

  CExportIDMStabor::m_fieldNames.reserve(39);
  CExportIDMStabor::m_units.reserve(39);
  CExportIDMStabor::m_fieldNames.push_back("ValidRow");
  CExportIDMStabor::m_units.push_back("");
  CExportIDMStabor::m_fieldNames.push_back("RowNr");
  CExportIDMStabor::m_units.push_back("");
  CExportIDMStabor::m_fieldNames.push_back("VerticalDepth");
  CExportIDMStabor::m_units.push_back("[m]");
  CExportIDMStabor::m_fieldNames.push_back("BoreholeRadius");
  CExportIDMStabor::m_units.push_back("[m]");
  CExportIDMStabor::m_fieldNames.push_back("Inclination");
  CExportIDMStabor::m_units.push_back("[deg]");
  CExportIDMStabor::m_fieldNames.push_back("AzimuthAngle");
  CExportIDMStabor::m_units.push_back("[deg]");
  CExportIDMStabor::m_fieldNames.push_back("Strsdef");
  CExportIDMStabor::m_units.push_back("");
  CExportIDMStabor::m_fieldNames.push_back("PorePress");
  CExportIDMStabor::m_units.push_back("[MPa]");
  CExportIDMStabor::m_fieldNames.push_back("TotNorthDirStrs");
  CExportIDMStabor::m_units.push_back("[MPa]");
  CExportIDMStabor::m_fieldNames.push_back("TotEastDirStrs");
  CExportIDMStabor::m_units.push_back("[MPa]");
  CExportIDMStabor::m_fieldNames.push_back("TotDepthDirStrs");
  CExportIDMStabor::m_units.push_back("[MPa]");
  CExportIDMStabor::m_fieldNames.push_back("NorthEastShStrs");
  CExportIDMStabor::m_units.push_back("[MPa]");
  CExportIDMStabor::m_fieldNames.push_back("EastDepthShStrs");
  CExportIDMStabor::m_units.push_back("[MPa]");
  CExportIDMStabor::m_fieldNames.push_back("NorthDepthShStrs");
  CExportIDMStabor::m_units.push_back("[MPa]");
  CExportIDMStabor::m_fieldNames.push_back("PorePressure");
  CExportIDMStabor::m_units.push_back("[kPa/m]");
  CExportIDMStabor::m_fieldNames.push_back("VertTSG");
  CExportIDMStabor::m_units.push_back("[kPa/m]");
  CExportIDMStabor::m_fieldNames.push_back("MaxHorTSG");
  CExportIDMStabor::m_units.push_back("[kPa/m]");
  CExportIDMStabor::m_fieldNames.push_back("MinHorTSG");
  CExportIDMStabor::m_units.push_back("[kPa/m]");
  CExportIDMStabor::m_fieldNames.push_back("AzimuthMaxHor");
  CExportIDMStabor::m_units.push_back("[deg]");
  CExportIDMStabor::m_fieldNames.push_back("Undrained");
  CExportIDMStabor::m_units.push_back("Yes/No");
  CExportIDMStabor::m_fieldNames.push_back("BiotAlpha");
  CExportIDMStabor::m_units.push_back("[-]");
  CExportIDMStabor::m_fieldNames.push_back("Skemptoncoef");
  CExportIDMStabor::m_units.push_back("[-]");
  CExportIDMStabor::m_fieldNames.push_back("Usecorrelation");
  CExportIDMStabor::m_units.push_back("Yes/No");
  CExportIDMStabor::m_fieldNames.push_back("SurfaceArea");
  CExportIDMStabor::m_units.push_back("[m2/g]");
  CExportIDMStabor::m_fieldNames.push_back("SonicTT");
  CExportIDMStabor::m_units.push_back("[micro-s/m]");
  CExportIDMStabor::m_fieldNames.push_back("Plasticity");
  CExportIDMStabor::m_units.push_back("Yes/No");
  CExportIDMStabor::m_fieldNames.push_back("Cohesion");
  CExportIDMStabor::m_units.push_back("[MPa]");
  CExportIDMStabor::m_fieldNames.push_back("Friction");
  CExportIDMStabor::m_units.push_back("[deg]");
  CExportIDMStabor::m_fieldNames.push_back("Dilatation");
  CExportIDMStabor::m_units.push_back("[deg]");
  CExportIDMStabor::m_fieldNames.push_back("Isotropy");
  CExportIDMStabor::m_units.push_back("");
  CExportIDMStabor::m_fieldNames.push_back("YoungsMod");
  CExportIDMStabor::m_units.push_back("[GPa]");
  CExportIDMStabor::m_fieldNames.push_back("InPlaneYoungsMod");
  CExportIDMStabor::m_units.push_back("[GPa]");
  CExportIDMStabor::m_fieldNames.push_back("PerpYoungsMod");
  CExportIDMStabor::m_units.push_back("[GPa]");
  CExportIDMStabor::m_fieldNames.push_back("DipAzi");
  CExportIDMStabor::m_units.push_back("[deg]");
  CExportIDMStabor::m_fieldNames.push_back("Dipangle");
  CExportIDMStabor::m_units.push_back("[deg]");
  CExportIDMStabor::m_fieldNames.push_back("PoissonRatio");
  CExportIDMStabor::m_units.push_back("[-]");
  CExportIDMStabor::m_fieldNames.push_back("Hardening");
  CExportIDMStabor::m_units.push_back("[-]");
  CExportIDMStabor::m_fieldNames.push_back("HardeningGrad");
  CExportIDMStabor::m_units.push_back("[GPa]");
  CExportIDMStabor::m_fieldNames.push_back("FractEnergy");
  CExportIDMStabor::m_units.push_back("[Pa.m]");
}

bool CExportIDMStabor::eksport(const std::string &filename) {
  assert(m_pNewPointList != 0);

  std::ofstream stream(filename.c_str(), std::ios_base::out | std::ios_base::trunc);
  if (!stream)
    return false;

  stream << "GEOMEC\n";

  // number of point rows
  stream << m_pNewPointList->size() << std::endl;

  // column heading: field names
  for (int ii = 0; ii < m_fieldNames.size() - 1; ++ii)
    stream << m_fieldNames[ii] << '\t';

  if (m_fieldNames.size() > 0)
    stream << m_fieldNames[m_fieldNames.size() - 1] << std::endl;

  // column heading: units
  for (int ii = 0; ii < m_units.size() - 1; ++ii)
    stream << m_units[ii] << '\t';

  if (m_units.size() > 0)
    stream << m_units[m_units.size() - 1] << std::endl;

  {
    int ii = 0;
    std::list<CNewWellPoint>::const_iterator it;

    for (it = m_pNewPointList->begin(); it != m_pNewPointList->end(); ++it) {
      CNewGeoWellPoint gwp(*const_cast<CModelBase *>(&m_model), *it);
      CExportIDMStaborRow(m_model, m_stage, m_antype, &gwp, ii).eksport(stream);
      ++ii;
    }
  }
  return true;
}

CExportIDMStaborRow::CExportIDMStaborRow(const CModelBase &model, const CDepletionStage &stage,
                                         const CAnalysisType &antype, const CNewGeoWellPoint *pNewWellPoint, int rowNr)
    : m_model(model), m_stage(stage), m_antype(antype), m_pNewWellPoint(pNewWellPoint), m_validRow(true),
      m_rowNr(rowNr + 1), m_verticalDepth(0), m_boreHoleRadius(0.1), m_inclinationAngle(0), m_azimuthAngle(0),
      m_stressDefinitionGeneral(true), m_porePressure(-9999), m_totalNorthDirecStress(-9999),
      m_totalEastDirecStress(-9999), m_totalDepthDirecStress(-9999), m_northEastShearStress(-9999),
      m_eastDepthShearStress(-9999), m_northDepthShearStress(-9999), m_validPorePressure(false),
      m_validTotalNorthDirecStress(false), m_validTotalEastDirecStress(false), m_validTotalDepthDirecStress(false),
      m_validNorthEastShearStress(false), m_validEastDepthShearStress(false), m_validNorthDepthShearStress(false)
      //, m_porePressure_kPa_m(-1)
      //, m_vertTSG(-1)
      //, m_maxHorTSG(-1)
      //, m_minHorTSG(-1)
      //, m_azimuthMaxHor(-1)
      ,
      m_undrained(false), m_biotAlpha(1), m_skemptonCoefficient(0), m_useCorrelationFunction(false), m_surfaceArea(-1),
      m_sonicTransitTime(-1), m_plasticity(false), m_cohesionStrength(0), m_frictionAngle(0), m_dilatationAngle(0),
      m_isotropy(true), m_youngsModulus(0)
      //, m_inPlaneYoungsModulus(-1)
      //, m_YoungsModulusPerpendicularToBeddingPlane(-1)
      //, m_dipAzimuth(-1)
      //, m_dipAngle(-1)
      ,
      m_poissonRatio(0)
//, m_hardeningType(-1)
//, m_hardeningGradient(-1)
//, m_fractureEnergy(-1)
{
  const CMaterialParameter *pParam = 0;
  const CFFMaterial *pMat = 0;

  assert(m_pNewWellPoint != 0);

  const CFormationBase *pFormation = 0;
  pFormation = m_pNewWellPoint->Formation();
  assert(pFormation);

  for (int nElementSet = 0; nElementSet < pFormation->ElementSetSize(); nElementSet++) {
    for (int i = 0; i < pFormation->ElementSet(nElementSet).ElementSet().ElementSize(); i++) {
      // get the element
      const geo::IElement &element = pFormation->ElementSet(nElementSet).ElementSet().Element(i);
      {
        if (element.Contains(*pNewWellPoint, true)) {
          // get the element's material
          pMat = &(pFormation->Material(stage).Material(element));
          break;
        }
      }
    }
  }

  assert(pMat);
  m_youngsModulus = pMat->ParameterValue(IDT_VALUETYPE_YOUNGS_MODULUS) / 1000.0; // GPa
  m_poissonRatio = pMat->ParameterValue(IDT_VALUETYPE_POISSONS_RATIO);
  m_cohesionStrength = pMat->ParameterValue(IDT_VALUETYPE_COHESION); // MPa

  // friction angle is either
  // ID_VALUETYPE_FRICTION_ANGLE
  // or
  // ID_VALUETYPE_INITFRICTION
  if (pMat->IsParameter(IDT_VALUETYPE_FRICTION_ANGLE))
    m_frictionAngle = pMat->ParameterValue(IDT_VALUETYPE_FRICTION_ANGLE);
  else if (pMat->IsParameter(IDT_VALUETYPE_INITFRICTION))
    m_frictionAngle = pMat->ParameterValue(IDT_VALUETYPE_INITFRICTION);
  // remove this assert if a new material is added,
  // where no friction angle exists and we want to use 0
  else
    assert(FALSE);

  // dilatation angle might exist as ID_VALUETYPE_DILATATION,
  // otherwise use 0
  if (pMat->IsParameter(IDT_VALUETYPE_DILATATION))
    m_dilatationAngle = pMat->ParameterValue(IDT_VALUETYPE_DILATATION);
  else
    m_dilatationAngle = 0;

  {
    CAngleQuantity azi, inc;
    pNewWellPoint->AzimuthInclination(azi, inc);
    m_azimuthAngle = azi.Value();
    m_inclinationAngle = inc.Value();

    CNewWellPath &wellpath = (CNewWellPath &)pNewWellPoint->WellPath();
    m_verticalDepth = pNewWellPoint->Z() - wellpath.MudGradRefDepth();
  }

  // Set direct depthstress
  unsigned int uMode = model.ResultTree().TotalStress().Components().Mode(antype, stage.Index());
  const IValueComponentBase *pResult =
      &model.ResultTree().TotalStress().Components().Component(ITensorGroup::CComponentComposite::TC_ZZ, uMode);

  geo::CValue result_value;
  result_value = pResult->ScalarData().ValuePoint(*pNewWellPoint, IQuantityDouble::SI_UNIT);

  if (result_value.Valid()) {
    m_validTotalDepthDirecStress = true;
    m_totalDepthDirecStress = result_value.Value();
  }

  // Set direct east stress stress
  uMode = model.ResultTree().TotalStress().Components().Mode(antype, stage.Index());
  pResult = &model.ResultTree().TotalStress().Components().Component(ITensorGroup::CComponentComposite::TC_YY, uMode);

  result_value = pResult->ScalarData().ValuePoint(*pNewWellPoint, IQuantityDouble::SI_UNIT);

  if (result_value.Valid()) {
    m_validTotalEastDirecStress = true;
    m_totalEastDirecStress = result_value.Value();
  }

  // Set direct north stress
  uMode = model.ResultTree().TotalStress().Components().Mode(antype, stage.Index());
  pResult = &model.ResultTree().TotalStress().Components().Component(ITensorGroup::CComponentComposite::TC_XX, uMode);

  result_value = pResult->ScalarData().ValuePoint(*pNewWellPoint, IQuantityDouble::SI_UNIT);

  if (result_value.Valid()) {
    m_validTotalNorthDirecStress = true;
    m_totalNorthDirecStress = result_value.Value();
  }

  // Set EZ shear stress (YZ)
  uMode = model.ResultTree().TotalStress().Components().Mode(antype, stage.Index());
  pResult = &model.ResultTree().TotalStress().Components().Component(ITensorGroup::CComponentComposite::TC_YZ, uMode);

  result_value = pResult->ScalarData().ValuePoint(*pNewWellPoint, IQuantityDouble::SI_UNIT);

  if (result_value.Valid()) {
    m_validEastDepthShearStress = true;
    m_eastDepthShearStress = result_value.Value();
  }

  // Set NZ shear stress (XZ)
  uMode = model.ResultTree().TotalStress().Components().Mode(antype, stage.Index());
  pResult = &model.ResultTree().TotalStress().Components().Component(ITensorGroup::CComponentComposite::TC_ZX, uMode);

  result_value = pResult->ScalarData().ValuePoint(*pNewWellPoint, IQuantityDouble::SI_UNIT);

  if (result_value.Valid()) {
    m_validNorthDepthShearStress = true;
    m_northDepthShearStress = result_value.Value();
  }

  // Set NE shear stress (XY)
  uMode = model.ResultTree().TotalStress().Components().Mode(antype, stage.Index());
  pResult = &model.ResultTree().TotalStress().Components().Component(ITensorGroup::CComponentComposite::TC_XY, uMode);

  result_value = pResult->ScalarData().ValuePoint(*pNewWellPoint, IQuantityDouble::SI_UNIT);

  if (result_value.Valid()) {
    m_validNorthEastShearStress = true;
    m_northEastShearStress = result_value.Value();
  }

  // Pore pressure
  uMode = model.ResultTree().PorePressure().Mode(antype, stage.Index());
  pResult = &model.ResultTree().PorePressure().Component(0, uMode);

  result_value = pResult->ScalarData().ValuePoint(*pNewWellPoint, IQuantityDouble::SI_UNIT);

  if (result_value.Valid()) {
    m_validPorePressure = true;
    m_porePressure = result_value.Value();
  }

  // Plasticity
  assert(dynamic_cast<const IMaterial *>(&pMat->Material()));
  const IMaterial &mat = static_cast<const IMaterial &>(pMat->Material());
  if (mat.MaterialModel() == MM_CAMCLAY || mat.MaterialModel() == MM_MOHRCOULOMB ||
      mat.MaterialModel() == MM_MODIFIEDMOHRCOULOMB || mat.MaterialModel() == MM_CAMCLAY_CREEP)
    m_plasticity = true;
  else
    m_plasticity = false;

  // Skempton coefficient
  m_skemptonCoefficient =
      1.0 / (1 + pMat->ParameterValue(IDT_VALUETYPE_BULKSTIFFNESS) * pMat->ParameterValue(IDT_VALUETYPE_POROSITY) /
                     pMat->ParameterValue(IDT_VALUETYPE_FLUID_BULK_MOD));

  if (mat.MaterialModel() == MM_UNDRAINED)
    m_undrained = true;
  else
    m_undrained = false;
}

void CExportIDMStaborRow::eksport(std::ostream &stream) {
  if (m_validRow)
    stream << "OK" << '\t';
  else
    stream << "NOK" << '\t';

  stream << m_rowNr << '\t';
  stream << m_verticalDepth << '\t';
  stream << m_boreHoleRadius << '\t';
  stream << m_inclinationAngle << '\t';
  stream << m_azimuthAngle << '\t';

  if (m_stressDefinitionGeneral)
    stream << "general" << '\t';
  else {
    assert(m_stressDefinitionGeneral); // not supported
    stream << '\t';
  }

  if (m_validPorePressure)
    stream << m_porePressure << '\t';
  else
    stream << '\t';

  if (m_validTotalNorthDirecStress)
    stream << m_totalNorthDirecStress << '\t';
  else
    stream << '\t';

  if (m_validTotalEastDirecStress)
    stream << m_totalEastDirecStress << '\t';
  else
    stream << '\t';

  if (m_validTotalDepthDirecStress)
    stream << m_totalDepthDirecStress << '\t';
  else
    stream << '\t';

  if (m_validNorthEastShearStress)
    stream << m_northEastShearStress << '\t';
  else
    stream << '\t';

  if (m_validEastDepthShearStress)
    stream << m_eastDepthShearStress << '\t';
  else
    stream << '\t';

  if (m_validNorthDepthShearStress)
    stream << m_northDepthShearStress << '\t';
  else
    stream << '\t';

  if (m_stressDefinitionGeneral) {
    stream << 0 << '\t'; // m_porePressure_kPa_m
    stream << 0 << '\t'; // m_vertTSG
    stream << 0 << '\t'; // m_maxHorTSG
    stream << 0 << '\t'; // m_minHorTSG
    stream << 0 << '\t'; // m_azimuthMaxHor
  } else {
    assert(m_stressDefinitionGeneral); // not supported
    stream << '\t';                    // m_porePressure_kPa_m
    stream << '\t';                    // m_vertTSG
    stream << '\t';                    // m_maxHorTSG
    stream << '\t';                    // m_minHorTSG
    stream << '\t';                    // m_azimuthMaxHor
  }

  if (m_undrained)
    stream << "yes" << '\t';
  else
    stream << "no" << '\t';
  stream << m_biotAlpha << '\t';
  stream << m_skemptonCoefficient << '\t';
  if (m_useCorrelationFunction) {
    assert(m_useCorrelationFunction == false); // not supported
    stream << "yes" << '\t';
    stream << m_surfaceArea << '\t';
    stream << m_sonicTransitTime << '\t';
  } else {
    stream << "no" << '\t';
    stream << '\t'; // m_surfaceArea
    stream << '\t'; // m_sonicTransitTime
  }
  if (m_plasticity)
    stream << "yes" << '\t';
  else
    stream << "no" << '\t';

  if (m_useCorrelationFunction) {
    assert(m_useCorrelationFunction == false); // not supported
    stream << '\t';                            // m_cohesionStrength
    stream << '\t';                            // m_frictionAngle
    stream << '\t';                            // m_dilatationAngle
  } else {
    stream << m_cohesionStrength << '\t';
    stream << m_frictionAngle << '\t';
    stream << m_dilatationAngle << '\t';
  }

  if (m_isotropy)
    stream << "Isotropy" << '\t';
  else
    stream << "Anisotropy" << '\t';
  if (m_isotropy)
    stream << m_youngsModulus << '\t';
  else
    stream << 10 << '\t'; // GPa
  stream << '\t';         // m_inPlaneYoungsModulus
  stream << '\t';         // m_YoungsModulusPerpendicularToBeddingPlane
  stream << '\t';         // m_dipAzimuth
  stream << '\t';         // m_dipAngle

  if (m_useCorrelationFunction) {
    assert(m_useCorrelationFunction == false); // not supported
    stream << '\t';                            // m_poissonRatio
  } else {
    stream << m_poissonRatio << '\t';
  }

  stream << "none" << '\t'; // m_hardeningType
  stream << '\t';           // m_hardeningGradient
  stream << '\t';           // m_fractureEnergy
  stream << '\n';
}

/////

CStartIDMStaborDlg::CStartIDMStaborDlg(std::list<CNewWellPoint> *pNewPointList, CModelBase &model, CWnd *pParent)
    : CDialog(IDD_EXPORT_IDMSTABOR, pParent), m_model(model), m_pNewPointList(pNewPointList), m_nAnalysisType(0) {
  assert(m_pNewPointList != 0);
}

BOOL CStartIDMStaborDlg::OnInitDialog() {
  CDialog::OnInitDialog();

  if (!m_model.ResultRegister().ResultsAvailable(CAnalysisType::AT_LINEAR)) {
    m_nAnalysisType = 1; // default to nonline
    GetDlgItem(IDC_RAD_LINEAR)->EnableWindow(FALSE);
  }

  if (!m_model.ResultRegister().ResultsAvailable(CAnalysisType::AT_NONLIN)) {
    if (m_nAnalysisType == 1)
      m_nAnalysisType = 2; // default to mixture
    GetDlgItem(IDC_RAD_NONLIN)->EnableWindow(FALSE);
  }

  if (!m_model.ResultRegister().ResultsAvailable(CAnalysisType::AT_MIXTURE)) {
    if (m_nAnalysisType == 2)
      m_nAnalysisType = 3; // default to containment
    GetDlgItem(IDC_RAD_MIXTURE)->EnableWindow(FALSE);
  }

  if (!m_model.ResultRegister().ResultsAvailable(CAnalysisType::AT_MIXTURE_CONTAINMENT)) {
    if (m_nAnalysisType == 3)
      m_nAnalysisType = -1; // default to none
    GetDlgItem(IDC_RAD_MIXTURE_CONTAINMENT)->EnableWindow(FALSE);
  }

  if (m_nAnalysisType != -1) {
    CComboBox *pDeplCombo = (CComboBox *)GetDlgItem(IDC_CBO_STAGE);
    CDepletionStageEntry::const_iterator it;
    for (it = m_model.DepletionStageEntry().begin(); it != m_model.DepletionStageEntry().end(); ++it) {
      if (m_model.ResultRegister().ResultsAvailable(AnalysisType(), *it)) {
        m_vcStages.push_back(&*it);
        pDeplCombo->AddString(it->Name().toStdString().c_str());
      }
    }
    m_nStage = 0;
  } else {
    GetDlgItem(IDC_CBO_STAGE)->EnableWindow(FALSE);
    m_nStage = -1;
  }

  if (m_nAnalysisType == -1)
    _m()->msg("No results are available for export");

  UpdateData(FALSE);

  UpdateExportButton();

  return TRUE;
}

void CStartIDMStaborDlg::DoDataExchange(CDataExchange *pDX) {
  CDialog::DoDataExchange(pDX);

  DDX_Text(pDX, IDC_ED_FILENAME, m_strFileName);
  DDX_Radio(pDX, IDC_RAD_LINEAR, m_nAnalysisType);
  DDX_CBIndex(pDX, IDC_CBO_STAGE, m_nStage);
}

void CStartIDMStaborDlg::OnExport() {
  UpdateData(TRUE);

  assert(m_pNewPointList->empty() == false);

  assert(!m_strFileName.IsEmpty());
  assert(!m_vcStages.empty());
  assert(m_nStage >= 0 && m_nStage < m_vcStages.size());

  if (!m_model.ResultRegister().ResultsAvailable(AnalysisType(), *m_vcStages[m_nStage])) {
    _m()->msg("No results are available for this analysis type and depletion stage");
    return;
  }

  if (CExportIDMStabor(m_model, *m_vcStages[m_nStage], AnalysisType(), m_pNewPointList).eksport(LPCSTR(m_strFileName)))
    _m()->msg("Export successful");
  else
    _m()->msg("Export failed");
}

void CStartIDMStaborDlg::OnBrowse() {
  CFileDialog dlg(FALSE, "txt", m_strFileName, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
                  "Text Files (*.txt)|*.txt|All Files (*.*)|*.*||", this);
  if (dlg.DoModal() == IDOK) {
    m_strFileName = dlg.GetPathName();
    UpdateData(FALSE);
    UpdateExportButton();
  }
}

BEGIN_MESSAGE_MAP(CStartIDMStaborDlg, CDialog)
ON_BN_CLICKED(IDC_EXPORT, OnExport)
ON_BN_CLICKED(IDC_BUT_BROWSE, OnBrowse)
ON_BN_CLICKED(IDC_RAD_LINEAR, UpdateExportButton)
ON_BN_CLICKED(IDC_RAD_NONLIN, UpdateExportButton)
ON_BN_CLICKED(IDC_RAD_MIXTURE, UpdateExportButton)
ON_BN_CLICKED(IDC_RAD_MIXTURE_CONTAINMENT, UpdateExportButton)
ON_CBN_SELCHANGE(IDC_CBO_STAGE, UpdateExportButton)
END_MESSAGE_MAP()

void CStartIDMStaborDlg::UpdateExportButton() {
  UpdateData(TRUE);
  bool bPointsAvailable = (m_pNewPointList && m_pNewPointList->empty() == false);

  bool bValidStage = (m_nStage >= 0 && m_nStage < m_vcStages.size() &&
                      m_model.ResultRegister().ResultsAvailable(AnalysisType(), *m_vcStages[m_nStage]));
  GetDlgItem(IDC_EXPORT)
      ->EnableWindow(m_nAnalysisType != -1 && !m_strFileName.IsEmpty() && !m_vcStages.empty() && bValidStage &&
                     bPointsAvailable);
}

CAnalysisType::TAnalysisType CStartIDMStaborDlg::AnalysisType() const {
  switch (m_nAnalysisType) {
  case 0:
    return CAnalysisType::AT_LINEAR;
  case 1:
    return CAnalysisType::AT_NONLIN;
  case 2:
    return CAnalysisType::AT_MIXTURE;
  }

  assert(m_nAnalysisType == 3);
  return CAnalysisType::AT_MIXTURE_CONTAINMENT;
}
