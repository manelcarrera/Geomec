
#include <cassert>
#include <cmath>

#include "LibraryMaterial.h"
#include "MaterialCreator.h"

#include "lbfl.h"

CLibraryMaterial::CLibraryMaterial(const QString &strCalibrationPath, IPQPlotter *pPQPlotter, bool bCanCalibrateElastic,
                                   bool bCanCalibratePlastic)
    : m_strCalibrationPath(strCalibrationPath), m_bCanCalibrateElastic(bCanCalibrateElastic),
      m_bCanCalibratePlastic(bCanCalibratePlastic), m_bBehavesLinearly(false), m_pPQPlotter(pPQPlotter),
      m_nMaxIterations(20), m_dConvCriterion(0.001) {}

CLibraryMaterial::CLibraryMaterial(const CLibraryMaterial &rhs)
    : ml::CMaterial(), // we do an assignment later in operator=
      m_pPQPlotter(0) {
  *this = rhs;
}

CLibraryMaterial::~CLibraryMaterial() {
  DestroyParameters();
  // delete m_pPQPlotter;
}

CLibraryMaterial *CLibraryMaterial::Clone() const { return new CLibraryMaterial(*this); }

CLibraryMaterial &CLibraryMaterial::operator=(const CLibraryMaterial &rhs) {
  ml::CMaterial::operator=(rhs);

  // delete m_pPQPlotter;

  m_strCalibrationPath = rhs.m_strCalibrationPath;
  m_bCanCalibrateElastic = rhs.m_bCanCalibrateElastic;
  m_bCanCalibratePlastic = rhs.m_bCanCalibratePlastic;
  m_bBehavesLinearly = rhs.m_bBehavesLinearly;
  m_pPQPlotter = rhs.m_pPQPlotter; // ? rhs.m_pPQPlotter->Clone() : 0);
  m_nMaxIterations = rhs.m_nMaxIterations;
  m_dConvCriterion = rhs.m_dConvCriterion;

  return *this;
}

bool CLibraryMaterial::operator==(const CLibraryMaterial &rhs) const {
  if (!ml::CMaterial::operator==(rhs))
    return false;

  return (m_nMaxIterations == rhs.m_nMaxIterations && fabs(m_dConvCriterion - rhs.m_dConvCriterion) < 1e-10);
}

bool CLibraryMaterial::operator!=(const CLibraryMaterial &rhs) const { return !operator==(rhs); }

int CLibraryMaterial::MaxIterations() const { return m_nMaxIterations; }

double CLibraryMaterial::ConvCriterion() const { return m_dConvCriterion; }

void CLibraryMaterial::MaxIterations(int nMaxIterations) { m_nMaxIterations = nMaxIterations; }

void CLibraryMaterial::ConvCriterion(double dConvCriterion) { m_dConvCriterion = dConvCriterion; }

void CLibraryMaterial::DestroyParameters() {
  for (size_t i = 0; i < m_vcParameters.size(); ++i)
    delete m_vcParameters[i];

  m_vcParameters.clear();
}

void CLibraryMaterial::CopyParameters(const ml::CMaterial &rhs) {
  const CLibraryMaterial &libMat = static_cast<const CLibraryMaterial &>(rhs);

  m_vcParameters.resize(libMat.ParameterSize());

  for (size_t i = 0; i < libMat.ParameterSize(); ++i) {
    m_vcParameters[i] = libMat.m_vcParameters[i]->Clone(*this);
  }
}

size_t CLibraryMaterial::AddParameter(ml::CMatParam *param) {
  m_vcParameters.push_back(static_cast<CLibraryMaterialParameter *>(param));
  return m_vcParameters.size() - 1;
}

size_t CLibraryMaterial::ParameterSize() const { return m_vcParameters.size(); }

const ml::CMatParam &CLibraryMaterial::MatParameter(size_t i) const {
  return *const_cast<const CLibraryMaterialParameter *>(m_vcParameters[i]);
}

ml::CMatParam &CLibraryMaterial::MatParameter(size_t i) { return *m_vcParameters[i]; }

const CLibraryMaterialParameter &CLibraryMaterial::Parameter(size_t i) const {
  return *const_cast<const CLibraryMaterialParameter *>(m_vcParameters[i]);
}

CLibraryMaterialParameter &CLibraryMaterial::Parameter(size_t i) { return *m_vcParameters[i]; }

const CLibraryMaterialParameter *CLibraryMaterial::Parameter(const QString &strName) const {
  const ml::CMatParam *pBase = ml::CMaterial::MatParameter(strName);
  if (!pBase)
    return 0;
  assert(dynamic_cast<const CLibraryMaterialParameter *>(pBase));
  return static_cast<const CLibraryMaterialParameter *>(pBase);
}

CLibraryMaterialParameter *CLibraryMaterial::Parameter(const QString &strName) {
  ml::CMatParam *pBase = ml::CMaterial::MatParameter(strName);
  if (!pBase)
    return 0;
  assert(dynamic_cast<CLibraryMaterialParameter *>(pBase));
  return static_cast<CLibraryMaterialParameter *>(pBase);
}

const CLibraryMaterialParameter *CLibraryMaterial::ParameterByValueTypeID(unsigned int uiValueTypeID) const {
  size_t i;
  for (i = 0; i < ParameterSize(); ++i) {
    if (Parameter(i).ValueTypeID() == uiValueTypeID)
      return &Parameter(i);
  }

  return 0;
}

CLibraryMaterialParameter *CLibraryMaterial::ParameterByValueTypeID(unsigned int uiValueTypeID) {
  return const_cast<CLibraryMaterialParameter *>(
      ((const CLibraryMaterial *)this)->ParameterByValueTypeID(uiValueTypeID));
}

bool CLibraryMaterial::GetPrimaryYieldCurve(double dPMin, double dPMax, CStressStrainArray &values) const {
  if (!m_pPQPlotter)
    return false;

  m_pPQPlotter->GetPrimaryYieldCurve(*this, dPMin, dPMax, values);
  return true;
}

bool CLibraryMaterial::GetSecondaryYieldCurve(double dPMin, double dPMax, CStressStrainArray &values) const {
  if (!m_pPQPlotter)
    return false;

  m_pPQPlotter->GetSecondaryYieldCurve(*this, dPMin, dPMax, values);
  return true;
}

bool CLibraryMaterial::CanCalibrateElastic() const { return !m_strCalibrationPath.isEmpty() && m_bCanCalibrateElastic; }

bool CLibraryMaterial::CanCalibratePlastic() const { return !m_strCalibrationPath.isEmpty() && m_bCanCalibratePlastic; }

bool CLibraryMaterial::BehavesLinearly() const { return m_bBehavesLinearly; }

void CLibraryMaterial::BehavesLinearly(bool bBehavesLinearly) { m_bBehavesLinearly = bBehavesLinearly; }

void CLibraryMaterial::SwitchMaterialModel(ml::CMaterial::CCreator &creator, bool bKeepParamValues) {
  typedef std::map<QString, bool> TNameFixedMap;
  TNameFixedMap mpNameFixed;

  if (bKeepParamValues) {
    for (size_t i = 0; i < ParameterSize(); ++i)
      mpNameFixed.insert(TNameFixedMap::value_type(Parameter(i).Name(), Parameter(i).IsCurrentlyFixed()));
  }

  ml::CMaterial::SwitchMaterialModel(creator, bKeepParamValues);

  if (bKeepParamValues) {
    for (size_t i = 0; i < ParameterSize(); ++i) {
      TNameFixedMap::iterator it = mpNameFixed.find(Parameter(i).Name());
      if (it != mpNameFixed.end())
        Parameter(i).CurrentlyFixed(it->second);
    }
  }

  assert(dynamic_cast<CMaterialCreator *>(&creator));
  CMaterialCreator &matcreator = static_cast<CMaterialCreator &>(creator);

  // switch PQ plotter
  // delete m_pPQPlotter;
  m_pPQPlotter = matcreator.PQPlotter();

  // switch calibration path
  m_strCalibrationPath = matcreator.CalibrationPath();

  m_bCanCalibrateElastic = matcreator.CanCalibrateElastic();
  m_bCanCalibratePlastic = matcreator.CanCalibratePlastic();
}

int CLibraryMaterial::ReadFromFilos() {
  if (!fcisop_())
    return 0;

  std::vector<CLibraryMaterialParameter *> vcParams;

  if (GroupSize() > 0) {
    vcParams.resize(Group(0).ParameterSize());
    size_t i;
    for (i = 0; i < Group(0).ParameterSize(); ++i)
      vcParams[i] = static_cast<CLibraryMaterialParameter *>(&Group(0).Parameter(i));
  } else {
    vcParams.resize(ParameterSize());
    size_t i;
    for (i = 0; i < ParameterSize(); ++i)
      vcParams[i] = &Parameter(i);
  }

  QString strMatCalibPath = QString("/MATCAL/MODEL/") + m_strCalibrationPath + "/";

  std::vector<ftn_double_t> vcValues(vcParams.size());

  for (size_t i = 0; i < vcParams.size(); ++i) {
    CLibraryMaterialParameter *pMP = vcParams[i];
    const QString &strParamCalibPath = pMP->CalibrationPath();
    if (!strParamCalibPath.isEmpty()) {
      QString path = strMatCalibPath + strParamCalibPath + "/VALUE";
      assert(XistIndexed(path.toStdString().c_str(), 0));
      ftn_double_t dValue;
      GetItem(path.toStdString().c_str(), &dValue);
      if (pMP->IsFixedCalibrationParameter() || pMP->IsCurrentlyFixed()) {
        if (fabs(pMP->Value() - dValue) > 1e-8)
          return -1;
      }
      vcValues[i] = dValue;
    }
  }

  for (size_t i = 0; i < vcParams.size(); ++i) {
    const QString &strParamCalibPath = vcParams[i]->CalibrationPath();
    if (!strParamCalibPath.isEmpty())
      vcParams[i]->Value(vcValues[i]);
  }

  return 1;
}

bool CLibraryMaterial::WriteToFilos() const {
  if (!fcisop_())
    return false;

  std::vector<const CLibraryMaterialParameter *> vcParams;

  if (GroupSize() > 0) {
    vcParams.resize(Group(0).ParameterSize());
    size_t i;
    for (i = 0; i < Group(0).ParameterSize(); ++i)
      vcParams[i] = static_cast<const CLibraryMaterialParameter *>(&Group(0).Parameter(i));
  } else {
    vcParams.resize(ParameterSize());
    size_t i;
    for (i = 0; i < ParameterSize(); ++i)
      vcParams[i] = &Parameter(i);
  }

  ftn_double_t dValue;
  ftn_bool_t bFixed;

  QString strMatCalibPath = QString("/MATCAL/MODEL/") + m_strCalibrationPath + "/";

  size_t i;
  for (i = 0; i < vcParams.size(); ++i) {
    PushDir();
    const CLibraryMaterialParameter *pMP = vcParams[i];
    const QString &strParamCalibPath = pMP->CalibrationPath();
    if (!strParamCalibPath.isEmpty()) {
      ChangeDir(strMatCalibPath.toStdString().c_str());
      PushDir();
      ChangeDir(strParamCalibPath.toStdString().c_str());

      dValue = pMP->Value();
      PutItem("VALUE", &dValue);

      bFixed = pMP->IsCurrentlyFixed() || pMP->IsFixedCalibrationParameter();
      SetActive("FIX", bFixed);

      PopDir();
    }
    PopDir();
  }

  return true;
}
