
#include "WellCasingSteelMaterial.h"
#include "DianaRunController.h"
#include "Material.h"
#include "ValueTypes.h"

#include "GeomecDianaRunner.h"
#include "StreamVersion.h"
#include "lbfl.h"
#include "resourceIDI.h"

#include "GlobalConstants.h"

#define DEF_YOUNG 207000
#define DEF_POISSON 0.3
#define DEF_DENSITY 7900
#define DEF_THERMALEXPANSION 35e-6
#define DEF_THERMALCONDUC 30
#define DEF_THERMALCAPACI 500

typedef struct {
  double stress;
  double strain;
} TStressStrainInit;

static TStressStrainInit Default_StressStrain[] = {{360, 0}, {370, 0.0001}, {380, 0.0002}, {400, 0.0010}};

CWellCasingSteelMaterial::CWellCasingSteelMaterial(CWellCasingSteelMaterialEntry &entry, const QString &strName)
    : IMaterialBase(strName, entry.Model()), m_dYoung(DEF_YOUNG), m_dPoisson(DEF_POISSON), m_dDensity(DEF_DENSITY),
      m_dThermalExpansion(DEF_THERMALEXPANSION), m_dThermalConductivity(DEF_THERMALCONDUC),
      m_dThermalCapacity(DEF_THERMALCAPACI), m_bPerforated(false) {
  size_t i;
  for (i = 0; i < sizeof(Default_StressStrain) / sizeof(TStressStrainInit); ++i)
    m_StressStrain.push_back(std::make_pair(Default_StressStrain[i].stress, Default_StressStrain[i].strain));

  reParent(&entry);
}

CWellCasingSteelMaterial::CWellCasingSteelMaterial(const CWellCasingSteelMaterial &rhs)
    : IMaterialBase(rhs), m_dYoung(rhs.m_dYoung), m_dPoisson(rhs.m_dPoisson), m_dDensity(rhs.m_dDensity),
      m_dThermalExpansion(rhs.m_dThermalExpansion), m_dThermalConductivity(rhs.m_dThermalConductivity),
      m_dThermalCapacity(rhs.m_dThermalCapacity), m_bPerforated(rhs.m_bPerforated), m_StressStrain(rhs.m_StressStrain) {
}

CWellCasingSteelMaterial &CWellCasingSteelMaterial::operator=(const CWellCasingSteelMaterial &rhs) {
  if (!(*this == rhs)) {
    IMaterialBase::operator=(rhs);

    m_dYoung = rhs.m_dYoung;
    m_dPoisson = rhs.m_dPoisson;
    m_dDensity = rhs.m_dDensity;
    m_dThermalExpansion = rhs.m_dThermalExpansion;
    m_dThermalConductivity = rhs.m_dThermalConductivity;
    m_dThermalCapacity = rhs.m_dThermalCapacity;
    m_bPerforated = rhs.m_bPerforated;
    m_StressStrain = rhs.m_StressStrain;

    Modified();
  }

  return *this;
}

bool CWellCasingSteelMaterial::operator==(const CWellCasingSteelMaterial &rhs) const {
  if (!IMaterialBase::operator==(rhs))
    return false;

  return (fabs(m_dYoung - rhs.m_dYoung) < 1e-6 && fabs(m_dPoisson - rhs.m_dPoisson) < 1e-8 &&
          fabs(m_dDensity - rhs.m_dDensity) < 1e-6 && fabs(m_dThermalExpansion - rhs.m_dThermalExpansion) < 1e-6 &&
          fabs(m_dThermalConductivity - rhs.m_dThermalConductivity) < 1e-6 &&
          fabs(m_dThermalCapacity - rhs.m_dThermalCapacity) < 1e-6 && m_StressStrain == rhs.m_StressStrain &&
          m_bPerforated == rhs.m_bPerforated);
}

unsigned int CWellCasingSteelMaterial::IconId() const {
  // TODO
  if (ReadOnly())
    return IDI_ROCK_MATERIAL_LOCKED;

  return IDI_ROCK_MATERIAL;
}

unsigned int CWellCasingSteelMaterial::TypeId() const { return 0; }

double CWellCasingSteelMaterial::ParameterValue(unsigned int ValueTypeID) const {
  switch (ValueTypeID) {
  case IDT_VALUETYPE_YOUNGS_MODULUS:
    return m_dYoung;
  case IDT_VALUETYPE_POISSONS_RATIO:
    return m_dPoisson;
  case IDT_VALUETYPE_RHOB:
    return m_dDensity;
  case IDT_VALUETYPE_THERMAL_EXPANSION:
    return m_dThermalExpansion;
  case IDT_VALUETYPE_THERM_CONDUCT:
    return m_dThermalConductivity;
  case IDT_VALUETYPE_THERM_CAPACI:
    return m_dThermalCapacity;
  }

  assert(FALSE);
  return -9999;
}

bool CWellCasingSteelMaterial::IsParameter(unsigned int ValueTypeID) const {
  return (ValueTypeID == IDT_VALUETYPE_YOUNGS_MODULUS || ValueTypeID == IDT_VALUETYPE_POISSONS_RATIO ||
          ValueTypeID == IDT_VALUETYPE_RHOB || ValueTypeID == IDT_VALUETYPE_THERMAL_EXPANSION ||
          ValueTypeID == IDT_VALUETYPE_THERM_CONDUCT || ValueTypeID == IDT_VALUETYPE_THERM_CAPACI);
}

bool CWellCasingSteelMaterial::Write(const CFFMaterial &ffmat, dia::IDianaRunner &diarunner) const {
  assert(dynamic_cast<CGeomecDianaRunnerBase *>(&diarunner));
  CGeomecDianaRunnerBase &runner = static_cast<CGeomecDianaRunnerBase &>(diarunner);

  ftn_double_t dYoung = ffmat.ParameterValue(IDT_VALUETYPE_YOUNGS_MODULUS) * 1e6;
  ftn_double_t dPoisson = ffmat.ParameterValue(IDT_VALUETYPE_POISSONS_RATIO);
  ftn_double_t dDensity = ffmat.ParameterValue(IDT_VALUETYPE_RHOB);
  ftn_double_t dThermX = ffmat.ParameterValue(IDT_VALUETYPE_THERMAL_EXPANSION) /
                         3.0; // go from volumetric to linear thermal expansion coefficient

  PutCharItem("YIELD", "VMISES");
  PutItemLength("YOUNG", &dYoung, 1);
  PutItemLength("POISON", &dPoisson, 1);
  PutItem("DENSIT", &dDensity);
  PutItemLength("THERMX", &dThermX, 1);

  ftn_double_t *pKapsig = new ftn_double_t[2 * m_StressStrain.size() + 2];
  CStressStrain::const_iterator it;
  size_t i;
  for (i = 0, it = m_StressStrain.begin(); it != m_StressStrain.end(); ++i, ++it) {
    assert(i < m_StressStrain.size());
    pKapsig[2 * i] = (ftn_double_t)it->second;
    pKapsig[2 * i + 1] = (ftn_double_t)it->first * 1e6;
  }

  pKapsig[2 * m_StressStrain.size()] = 1.;
  pKapsig[2 * m_StressStrain.size() + 1] = 2.e9;

  PutItemLength("KAPSIG", pKapsig, 2 * m_StressStrain.size() + 2);
  delete[] pKapsig;

  //// temporary
  /*
    ftn_double_t K0[2];
    K0[0] = 1;
    K0[1] = 1;
    PutItemLength("K0", K0, 2);
    ftn_int_t iDir = 1;
    PutItem("K0DIR", &iDir);
  */
  //// end temporary

  if ((runner.Controller().AnalysisType() == CAnalysisType::AT_MIXTURE ||
       runner.Controller().AnalysisType() == CAnalysisType::AT_MIXTURE_CONTAINMENT) &&
      m_bPerforated) {
    // write modified permeability k' = k / mu
    ftn_double_t mu = (ftn_double_t)ffmat.ParameterValue(IDT_VALUETYPE_VISCOSITY); // in cP (= 1e-3 Pa.s)
    mu *= 1e-3;                                                                    // Pa.s
    ftn_double_t k = (ftn_double_t)ffmat.ParameterValue(IDT_VALUETYPE_PERMEA);     // in mD (= 1e-15 m2)
    k *= 1e-15;                                                                    // m2
    ftn_double_t k_mod = k / mu;
    //    if(!m_bPerforated)
    //      k_mod *= 1e-15; // very low permeability
    PutItemLength("PERMEA", &k_mod, 1);
  }

  if (runner.Controller().AnalysisType() == CAnalysisType::AT_HEAT) {
    ftn_double_t ddum = (ftn_double_t)ffmat.ParameterValue(IDT_VALUETYPE_THERM_CONDUCT);
    // use months as time unit instead of seconds
    ddum *= SECONDS_PER_MONTH;
    PutItemLength("CONDUC", &ddum, 1);

    ddum = (ftn_double_t)ffmat.ParameterValue(IDT_VALUETYPE_THERM_CAPACI);
    ddum *= (ftn_double_t)ffmat.ParameterValue(IDT_VALUETYPE_RHOB);
    PutItem("CAPACI", &ddum);

    // according to heat flow design document by Chantal Frissen the POROSI
    // material parameter can not be present in heat flow calculation
    if (XistIndexed("POROSI", 0))
      RemoveIndexedItem("POROSI", 0);
  }

  return true;
}

int CWellCasingSteelMaterial::Type() const { return 101; }

int CWellCasingSteelMaterial::WriteFilosParamSize(const CFFMaterial & /*ffmat*/, dia::IDianaRunner &diarunner) const {
  assert(dynamic_cast<CGeomecDianaRunnerBase *>(&diarunner));
  CGeomecDianaRunnerBase &runner = static_cast<CGeomecDianaRunnerBase &>(diarunner);

  int size = 4; // YOUNG/POISON/DENSIT/THERMX

  size += 2 * m_StressStrain.size() + 2; // KAPSIG

  if ((runner.Controller().AnalysisType() == CAnalysisType::AT_MIXTURE ||
       runner.Controller().AnalysisType() == CAnalysisType::AT_MIXTURE_CONTAINMENT) &&
      m_bPerforated) {
    size += 1; // PERMEA
  }

  if (runner.Controller().AnalysisType() == CAnalysisType::AT_HEAT) {
    size += 2; // CONDUC/CAPACI
  }

  return size;
}

bool CWellCasingSteelMaterial::WriteFilosParamName(const CFFMaterial & /*ffmat*/, dia::IDianaRunner &diarunner, int i,
                                                   char *name) const {
  assert(dynamic_cast<CGeomecDianaRunnerBase *>(&diarunner));
  CGeomecDianaRunnerBase &runner = static_cast<CGeomecDianaRunnerBase &>(diarunner);

  if (i < 4) {
    switch (i) {
    case 0:
      strncpy(name, "YOUNG", 10);
      break;
    case 1:
      strncpy(name, "POISON", 10);
      break;
    case 2:
      strncpy(name, "DENSIT", 10);
      break;
    case 3:
      strncpy(name, "THERMX", 10);
      break;
    default:
      assert(false);
    }
    return true;
  } else if (i < 4 + 2 * m_StressStrain.size() + 2) {
    QString kapsig = QString("KAPSIG(%1)").arg(i - 4 + 1);
    strncpy(name, kapsig.toStdString().c_str(), kapsig.length());
    return true;
  } else {
    if ((runner.Controller().AnalysisType() == CAnalysisType::AT_MIXTURE ||
         runner.Controller().AnalysisType() == CAnalysisType::AT_MIXTURE_CONTAINMENT) &&
        m_bPerforated) {
      if (i == 4 + 2 * m_StressStrain.size() + 2) {
        strncpy(name, "PERMEA", 10);
        return true;
      } else {
        assert(false);
        return false;
      }
    }

    if (runner.Controller().AnalysisType() == CAnalysisType::AT_HEAT) {
      if (i == 4 + 2 * m_StressStrain.size() + 2) {
        strncpy(name, "CONDUC", 10);
        return true;
      } else if (i == 4 + 2 * m_StressStrain.size() + 2 + 1) {
        strncpy(name, "CAPACI", 10);
        return true;
      } else {
        assert(false);
        return false;
      }
    }

    assert(false);
    return false;
  }
}

void CWellCasingSteelMaterial::WriteFilosParamValues(const CFFMaterial &ffmat, dia::IDianaRunner &diarunner,
                                                     double *values, int stride) const {
  assert(dynamic_cast<CGeomecDianaRunnerBase *>(&diarunner));
  CGeomecDianaRunnerBase &runner = static_cast<CGeomecDianaRunnerBase &>(diarunner);

  *values = ffmat.ParameterValue(IDT_VALUETYPE_YOUNGS_MODULUS) * 1e6;
  values += stride;

  *values = ffmat.ParameterValue(IDT_VALUETYPE_POISSONS_RATIO);
  values += stride;

  *values = ffmat.ParameterValue(IDT_VALUETYPE_RHOB);
  values += stride;

  *values = ffmat.ParameterValue(IDT_VALUETYPE_THERMAL_EXPANSION) / 3.0;
  values += stride;

  for (CStressStrain::const_iterator it = m_StressStrain.begin(); it != m_StressStrain.end(); ++it) {
    *values = it->second;
    values += stride;

    *values = it->first * 1e6;
    values += stride;
  }

  *values = 1;
  values += stride;

  *values = 2e9;
  values += stride;

  if ((runner.Controller().AnalysisType() == CAnalysisType::AT_MIXTURE ||
       runner.Controller().AnalysisType() == CAnalysisType::AT_MIXTURE_CONTAINMENT) &&
      m_bPerforated) {
    // write modified permeability k' = k / mu
    ftn_double_t mu = (ftn_double_t)ffmat.ParameterValue(IDT_VALUETYPE_VISCOSITY); // in cP (= 1e-3 Pa.s)
    mu *= 1e-3;                                                                    // Pa.s
    ftn_double_t k = (ftn_double_t)ffmat.ParameterValue(IDT_VALUETYPE_PERMEA);     // in mD (= 1e-15 m2)
    k *= 1e-15;                                                                    // m2
    ftn_double_t k_mod = k / mu;

    *values = k_mod;
    values += stride;
  }

  if (runner.Controller().AnalysisType() == CAnalysisType::AT_HEAT) {
    ftn_double_t ddum = (ftn_double_t)ffmat.ParameterValue(IDT_VALUETYPE_THERM_CONDUCT);
    // use months as time unit instead of seconds
    ddum *= SECONDS_PER_MONTH;

    *values = ddum;
    values += stride;

    ddum = (ftn_double_t)ffmat.ParameterValue(IDT_VALUETYPE_THERM_CAPACI);
    ddum *= (ftn_double_t)ffmat.ParameterValue(IDT_VALUETYPE_RHOB);

    *values = ddum;
    values += stride;
  }
}

double CWellCasingSteelMaterial::YoungsModulus() const { return m_dYoung; }

double CWellCasingSteelMaterial::PoissonsRatio() const { return m_dPoisson; }

double CWellCasingSteelMaterial::Density() const { return m_dDensity; }

double CWellCasingSteelMaterial::ThermalExpansion() const { return m_dThermalExpansion; }

double CWellCasingSteelMaterial::ThermalConductivity() const { return m_dThermalConductivity; }

double CWellCasingSteelMaterial::ThermalCapacity() const { return m_dThermalCapacity; }

bool CWellCasingSteelMaterial::Perforated() const { return m_bPerforated; }

bool CWellCasingSteelMaterial::SetYoungsModulus(double dValue) {
  if (dValue > 0) {
    m_dYoung = dValue;
    return true;
  }

  return false;
}

bool CWellCasingSteelMaterial::SetPoissonsRatio(double dValue) {
  if (dValue >= 0 && dValue < 0.5) {
    m_dPoisson = dValue;
    return true;
  }

  return false;
}

bool CWellCasingSteelMaterial::SetDensity(double dValue) {
  if (dValue >= 0) {
    m_dDensity = dValue;
    return true;
  }

  return false;
}

bool CWellCasingSteelMaterial::SetThermalExpansion(double dValue) {
  if (dValue >= 0) {
    m_dThermalExpansion = dValue;
    return true;
  }

  return false;
}

bool CWellCasingSteelMaterial::SetThermalConductivity(double dValue) {
  if (dValue >= 0) {
    m_dThermalConductivity = dValue;
    return true;
  }

  return false;
}

bool CWellCasingSteelMaterial::SetThermalCapacity(double dValue) {
  if (dValue >= 0) {
    m_dThermalCapacity = dValue;
    return true;
  }

  return false;
}

void CWellCasingSteelMaterial::SetPerforated(bool bValue) { m_bPerforated = bValue; }

CWellCasingSteelMaterial::CStressStrain &CWellCasingSteelMaterial::StressStrain() { return m_StressStrain; }

const CWellCasingSteelMaterial::CStressStrain &CWellCasingSteelMaterial::StressStrain() const { return m_StressStrain; }

void CWellCasingSteelMaterial::LoadStream(TSTREAM &stream, CStreamVersion &version, TPROGRESS &progress) {
  IMaterialBase::LoadStream(stream, version, progress);

  stream >> m_dYoung >> m_dPoisson >> m_dDensity >> m_dThermalExpansion;
  if (version >= CStreamVersion(3, 7, 12)) {
    int nPerforated;
    stream >> nPerforated;
    m_bPerforated = (nPerforated != 0);
  }
  progress.Step();

  m_StressStrain.LoadStream(stream, version, progress);
}

void CWellCasingSteelMaterial::SaveStream(TSTREAM &stream, TPROGRESS &progress) {
  IMaterialBase::SaveStream(stream, progress);

  stream << m_dYoung << m_dPoisson << m_dDensity << m_dThermalExpansion;
  stream << int(m_bPerforated ? 1 : 0);
  progress.Step();

  m_StressStrain.SaveStream(stream, progress);
}

long CWellCasingSteelMaterial::SavedItems() const {
  long lRet = IMaterialBase::SavedItems();
  lRet += 1 + m_StressStrain.SavedItems();

  return lRet;
}

bool CWellCasingSteelMaterial::Empty() const { return false; }

bool CWellCasingSteelMaterial::Less(const CGraphNode &node) const {
  const CWellCasingSteelMaterial *pMat = dynamic_cast<const CWellCasingSteelMaterial *>(&node);
  if (pMat) {
    if (m_dYoung < pMat->m_dYoung)
      return true;
    if (m_dYoung > pMat->m_dYoung)
      return false;

    if (m_dPoisson < pMat->m_dPoisson)
      return true;
    if (m_dPoisson > pMat->m_dPoisson)
      return false;

    if (m_dDensity < pMat->m_dDensity)
      return true;
    if (m_dDensity > pMat->m_dDensity)
      return false;

    if (m_dThermalExpansion < pMat->m_dThermalExpansion)
      return true;
    if (m_dThermalExpansion > pMat->m_dThermalExpansion)
      return false;

    if (m_dThermalConductivity < pMat->m_dThermalConductivity)
      return true;
    if (m_dThermalConductivity > pMat->m_dThermalConductivity)
      return false;

    if (m_dThermalCapacity < pMat->m_dThermalCapacity)
      return true;
    if (m_dThermalCapacity > pMat->m_dThermalCapacity)
      return false;

    if (m_bPerforated < pMat->m_bPerforated)
      return true;
    if (m_bPerforated > pMat->m_bPerforated)
      return false;

    return m_StressStrain.Less(pMat->m_StressStrain);
  }

  return IMaterialBase::Less(node);
}

///// CWellCasingSteelMaterial::CStressStrain

CWellCasingSteelMaterial::CStressStrain::CStressStrain() {}

CWellCasingSteelMaterial::CStressStrain::CStressStrain(const CStressStrain &rhs)
    : m_lstStressStrain(rhs.m_lstStressStrain) {}

CWellCasingSteelMaterial::CStressStrain &CWellCasingSteelMaterial::CStressStrain::operator=(const CStressStrain &rhs) {
  m_lstStressStrain = rhs.m_lstStressStrain;
  return *this;
}

bool CWellCasingSteelMaterial::CStressStrain::operator==(const CStressStrain &rhs) const {
  return m_lstStressStrain == rhs.m_lstStressStrain;
}

bool CWellCasingSteelMaterial::CStressStrain::empty() const { return m_lstStressStrain.empty(); }

CWellCasingSteelMaterial::CStressStrain::size_type CWellCasingSteelMaterial::CStressStrain::size() const {
  return m_lstStressStrain.size();
}

void CWellCasingSteelMaterial::CStressStrain::clear() { m_lstStressStrain.clear(); }

CWellCasingSteelMaterial::CStressStrain::iterator CWellCasingSteelMaterial::CStressStrain::begin() {
  return m_lstStressStrain.begin();
}

CWellCasingSteelMaterial::CStressStrain::iterator CWellCasingSteelMaterial::CStressStrain::end() {
  return m_lstStressStrain.end();
}

CWellCasingSteelMaterial::CStressStrain::const_iterator CWellCasingSteelMaterial::CStressStrain::begin() const {
  return m_lstStressStrain.begin();
}

CWellCasingSteelMaterial::CStressStrain::const_iterator CWellCasingSteelMaterial::CStressStrain::end() const {
  return m_lstStressStrain.end();
}

CWellCasingSteelMaterial::CStressStrain::reverse_iterator CWellCasingSteelMaterial::CStressStrain::rbegin() {
  return m_lstStressStrain.rbegin();
}

CWellCasingSteelMaterial::CStressStrain::reverse_iterator CWellCasingSteelMaterial::CStressStrain::rend() {
  return m_lstStressStrain.rend();
}

CWellCasingSteelMaterial::CStressStrain::const_reverse_iterator
CWellCasingSteelMaterial::CStressStrain::rbegin() const {
  return m_lstStressStrain.rbegin();
}

CWellCasingSteelMaterial::CStressStrain::const_reverse_iterator CWellCasingSteelMaterial::CStressStrain::rend() const {
  return m_lstStressStrain.rend();
}

void CWellCasingSteelMaterial::CStressStrain::push_back(const TStressStrainPair &_Val) {
  m_lstStressStrain.push_back(_Val);
}

void CWellCasingSteelMaterial::CStressStrain::push_front(const TStressStrainPair &_Val) {
  m_lstStressStrain.push_front(_Val);
}

void CWellCasingSteelMaterial::CStressStrain::pop_back() { m_lstStressStrain.pop_back(); }

void CWellCasingSteelMaterial::CStressStrain::pop_front() { m_lstStressStrain.pop_front(); }

CWellCasingSteelMaterial::CStressStrain::iterator
CWellCasingSteelMaterial::CStressStrain::insert(iterator _Where, const TStressStrainPair &_Val) {
  return m_lstStressStrain.insert(_Where, _Val);
}

CWellCasingSteelMaterial::CStressStrain::iterator CWellCasingSteelMaterial::CStressStrain::erase(iterator _Where) {
  return m_lstStressStrain.erase(_Where);
}

CWellCasingSteelMaterial::CStressStrain::iterator CWellCasingSteelMaterial::CStressStrain::erase(iterator _First,
                                                                                                 iterator _Last) {
  return m_lstStressStrain.erase(_First, _Last);
}

void CWellCasingSteelMaterial::CStressStrain::sort() { m_lstStressStrain.sort(CLess()); }

void CWellCasingSteelMaterial::CStressStrain::LoadStream(CStorageNode::TSTREAM &stream,
                                                         CStorageNode::TSTREAMVERSION & /*version*/,
                                                         CStorageNode::TPROGRESS &progress) {
  clear();

  int n;
  stream >> n;

  int i;
  for (i = 0; i < n; ++i) {
    double dFirst;
    double dSecond;
    stream >> dFirst >> dSecond;
    m_lstStressStrain.push_back(std::make_pair(dFirst, dSecond));
    progress.Step();
  }
}

void CWellCasingSteelMaterial::CStressStrain::SaveStream(CStorageNode::TSTREAM &stream,
                                                         CStorageNode::TPROGRESS &progress) {
  stream << int(m_lstStressStrain.size());

  iterator it;
  for (it = m_lstStressStrain.begin(); it != m_lstStressStrain.end(); ++it) {
    stream << it->first << it->second;
    progress.Step();
  }
}

long CWellCasingSteelMaterial::CStressStrain::SavedItems() const { return m_lstStressStrain.size(); }

bool CWellCasingSteelMaterial::CStressStrain::Less(const CStressStrain &rhs) const {
  return (m_lstStressStrain < rhs.m_lstStressStrain);
}

///// CWellCasingSteelMaterialEntry

CWellCasingSteelMaterialEntry::CWellCasingSteelMaterialEntry(int nEntryId, CModelBase &model)
    : CGraphEntryTemp<CWellCasingSteelMaterial>(nEntryId, IDI_ROCK_MATERIALS, "Casing Steel Materials", model) {}

void CWellCasingSteelMaterialEntry::LoadStream(CFemAppModel & /*model*/, TSTREAM &stream, CStreamVersion &version,
                                               TPROGRESS &progress) {
  int nMat;
  stream >> nMat;

  int i;
  for (i = 0; i < nMat; ++i) {
    CWellCasingSteelMaterial *pMat = new CWellCasingSteelMaterial(*this, QString());
    pMat->LoadStream(stream, version, progress);
  }
}

void CWellCasingSteelMaterialEntry::SaveStream(TSTREAM &stream, TPROGRESS &progress) {
  const TNodeSet &stNodes = EntryNodes();
  stream << int(stNodes.size());

  int nIndex = 0;
  TNodeSet::const_iterator it;
  for (it = stNodes.begin(); it != stNodes.end(); ++it) {
    (*it)->Index(++nIndex);
    (*it)->SaveStream(stream, progress);
  }
}

long CWellCasingSteelMaterialEntry::SavedItems() const {
  long lRet = 0;

  const TNodeSet &stNodes = EntryNodes();
  TNodeSet::const_iterator it;
  for (it = stNodes.begin(); it != stNodes.end(); ++it)
    lRet += (*it)->SavedItems();

  return lRet;
}

namespace {

const QString DEFAULT_STEEL_MATERIAL_NAME = QObject::tr("New Steel Material %1");
const size_t DEFAULT_INITIAL_NUMBER = 1;

} // anonymous namespace

CWellCasingSteelMaterial &CWellCasingSteelMaterialEntry::InsertNew() {
  QString sName = createUniqueName(DEFAULT_STEEL_MATERIAL_NAME, DEFAULT_INITIAL_NUMBER);
  CWellCasingSteelMaterial *pMat = new CWellCasingSteelMaterial(*this, sName.toStdString().c_str());
  return *pMat;
}