
#include "InterfaceMaterial.h"
#include "ValueTypes.h"
#include "DianaRunController.h"
#include "Material.h"

#include "lbfl.h"

#include "resourceIDI.h"
#include "GeomecDianaRunner.h"

#define DEF_COHESION 10
#define DEF_FRICTION 15
#define DEF_NORMALSTIFF 500
#define DEF_SHEARSTIFF 50
#define DEF_DFLUX 1.E9

CInterfaceMaterial::CInterfaceMaterial(CInterfaceMaterialEntry& entry, const QString& strName)
: IMaterialBase(strName, entry.Model()),
  m_dCohesion(DEF_COHESION),
  m_dFriction(DEF_FRICTION),
  m_dNormalStiffness(DEF_NORMALSTIFF),
  m_dShearStiffness(DEF_SHEARSTIFF),
  m_dDFlux(DEF_DFLUX)
{
  reParent(&entry);
}

CInterfaceMaterial::CInterfaceMaterial(const CInterfaceMaterial& rhs)
: IMaterialBase(rhs),
  m_dCohesion(rhs.m_dCohesion),
  m_dFriction(rhs.m_dFriction),
  m_dNormalStiffness(rhs.m_dNormalStiffness),
  m_dShearStiffness(rhs.m_dShearStiffness),
  m_dDFlux(rhs.m_dDFlux)
{
}

CInterfaceMaterial& CInterfaceMaterial::operator=(const CInterfaceMaterial& rhs)
{
  if(!(*this == rhs))
  {
    IMaterialBase::operator=(rhs);

    m_dCohesion        = rhs.m_dCohesion;
    m_dFriction        = rhs.m_dFriction;
    m_dNormalStiffness = rhs.m_dNormalStiffness;
    m_dShearStiffness  = rhs.m_dShearStiffness;
    m_dDFlux           = rhs.m_dDFlux;

    Modified();
  }

  return *this;
}

bool CInterfaceMaterial::operator==(const CInterfaceMaterial& rhs) const
{
  if(!IMaterialBase::operator==(rhs))
    return false;

  return (
    fabs(m_dCohesion        - rhs.m_dCohesion)        < 1e-6 &&
    fabs(m_dFriction        - rhs.m_dFriction)        < 1e-6 &&
    fabs(m_dNormalStiffness - rhs.m_dNormalStiffness) < 1e-6 &&
    fabs(m_dShearStiffness  - rhs.m_dShearStiffness)  < 1e-6 &&
    fabs(m_dDFlux           - rhs.m_dDFlux)           < 1e-6);
}

unsigned int CInterfaceMaterial::IconId() const
{
  // TODO
  if(ReadOnly())
    return IDI_ROCK_MATERIAL_LOCKED;

	return IDI_ROCK_MATERIAL;
}

unsigned int CInterfaceMaterial::TypeId() const
{
  return 0;
}

double CInterfaceMaterial::ParameterValue(unsigned int ValueTypeID) const
{
  switch(ValueTypeID)
  {
  case IDT_VALUETYPE_COHESION:
    return m_dCohesion;
  case IDT_VALUETYPE_FRICTION_ANGLE:
    return m_dFriction;
  case IDT_VALUETYPE_DFLUX:
    return m_dDFlux;
  case IDT_VALUETYPE_NORMALSTIFFNESS:
    return m_dNormalStiffness;
  case IDT_VALUETYPE_SHEARSTIFFNESS:
    return m_dShearStiffness;
  }

  assert(FALSE);
  return -9999;
}

bool CInterfaceMaterial::IsParameter(unsigned int ValueTypeID) const
{
  return (
    ValueTypeID == IDT_VALUETYPE_COHESION       ||
    ValueTypeID == IDT_VALUETYPE_FRICTION_ANGLE ||
    ValueTypeID == IDT_VALUETYPE_DFLUX          ||
    ValueTypeID == IDT_VALUETYPE_NORMALSTIFFNESS||
    ValueTypeID == IDT_VALUETYPE_SHEARSTIFFNESS);
}

bool CInterfaceMaterial::Write(const CFFMaterial &ffmat, dia::IDianaRunner& diarunner) const
{
  // only in case of user defined slip settings
  if(!ffmat.IsDistributedParameter(IDT_VALUETYPE_NORMALSTIFFNESS) &&
     !ffmat.IsDistributedParameter(IDT_VALUETYPE_SHEARSTIFFNESS))
  {
    // Write cohesion and friction
    SetActive("FRICTI", FTN_TRUE);

    ftn_double_t cohesi = ftn_double_t(ffmat.ParameterValue(IDT_VALUETYPE_COHESION) * 1.e6);
    PutItemLength("COHESI", &cohesi, 1);

    ftn_double_t phi = ftn_double_t(ffmat.ParameterValue(IDT_VALUETYPE_FRICTION_ANGLE) * PI / 180);
    PutItemLength("PHI", &phi, 1);

    ftn_double_t psi = 0.;
    PutItem("PSI", &psi);
  }

  ftn_double_t dsn = ftn_double_t(ffmat.ParameterValue(IDT_VALUETYPE_NORMALSTIFFNESS) * 1e6);
  ftn_double_t dst = ftn_double_t(ffmat.ParameterValue(IDT_VALUETYPE_SHEARSTIFFNESS) * 1e6);

  PutItemLength("DSNZ", &dsn, 1);
  PutItemLength("DSSX", &dst, 1);
  PutItemLength("DSSY", &dst, 1);

  assert(dynamic_cast<CGeomecDianaRunnerBase*>(&diarunner));
  CGeomecDianaRunnerBase& runner = static_cast<CGeomecDianaRunnerBase&>(diarunner);

  if(runner.Controller().AnalysisType() == CAnalysisType::AT_MIXTURE ||
     runner.Controller().AnalysisType() == CAnalysisType::AT_MIXTURE_CONTAINMENT)
  {
    // write very high permeability for faults
    ftn_double_t k_mod = 1;
    PutItemLength("DPERME", &k_mod, 1);
  }

  if(runner.Controller().AnalysisType() == CAnalysisType::AT_HEAT)
  {
    // dflux
    ftn_double_t dflux = ftn_double_t(ffmat.ParameterValue(IDT_VALUETYPE_DFLUX));
    PutItemLength("DFLUX", &dflux, 1);
  }

  return true;
}

int CInterfaceMaterial::Type() const
{
  return 100;
}

int CInterfaceMaterial::WriteFilosParamSize(const CFFMaterial &ffmat, dia::IDianaRunner& diarunner) const
{
  assert(dynamic_cast<CGeomecDianaRunnerBase*>(&diarunner));
  CGeomecDianaRunnerBase& runner = static_cast<CGeomecDianaRunnerBase&>(diarunner);

  int size = 0;

  if (!ffmat.IsDistributedParameter(IDT_VALUETYPE_NORMALSTIFFNESS) && !ffmat.IsDistributedParameter(IDT_VALUETYPE_SHEARSTIFFNESS))
  {
    size += 3; // COHESI/PHI/PSI
  }

  size += 3; // DSNZ/DSSX/DSSY

  if (runner.Controller().AnalysisType() == CAnalysisType::AT_MIXTURE ||
    runner.Controller().AnalysisType() == CAnalysisType::AT_MIXTURE_CONTAINMENT)
  {
    size += 1; // DPERME
  }

  if (runner.Controller().AnalysisType() == CAnalysisType::AT_HEAT)
  {
    size += 1; // DFLUX
  }

  return size;
}

bool CInterfaceMaterial::WriteFilosParamName(const CFFMaterial &ffmat, dia::IDianaRunner& diarunner, int i, char *name) const
{
  assert(dynamic_cast<CGeomecDianaRunnerBase*>(&diarunner));
  CGeomecDianaRunnerBase& runner = static_cast<CGeomecDianaRunnerBase&>(diarunner);

  if (!ffmat.IsDistributedParameter(IDT_VALUETYPE_NORMALSTIFFNESS) && !ffmat.IsDistributedParameter(IDT_VALUETYPE_SHEARSTIFFNESS))
  {
    switch (i)
    {
    case 0:
      strncpy(name, "COHESI", 10);
      return true;
    case 1:
      strncpy(name, "PHI", 10);
      return true;
    case 2:
      strncpy(name, "PSI", 10);
      return true;
    }

    i -= 3;
  }

  switch (i)
  {
  case 0:
    strncpy(name, "DSNZ", 10);
    return true;
  case 1:
    strncpy(name, "DSSX", 10);
    return true;
  case 2:
    strncpy(name, "DSSY", 10);
    return true;
  case 3:
    if (runner.Controller().AnalysisType() == CAnalysisType::AT_MIXTURE ||
      runner.Controller().AnalysisType() == CAnalysisType::AT_MIXTURE_CONTAINMENT)
    {
      strncpy(name, "DPERME", 10);
      return true;
    }

    if (runner.Controller().AnalysisType() == CAnalysisType::AT_HEAT)
    {
      strncpy(name, "DFLUX", 10);
      return true;
    }
  }

  return false;
}

void CInterfaceMaterial::WriteFilosParamValues(const CFFMaterial &ffmat, dia::IDianaRunner& diarunner, double *values, int stride) const
{
  assert(dynamic_cast<CGeomecDianaRunnerBase*>(&diarunner));
  CGeomecDianaRunnerBase& runner = static_cast<CGeomecDianaRunnerBase&>(diarunner);

  if (!ffmat.IsDistributedParameter(IDT_VALUETYPE_NORMALSTIFFNESS) && !ffmat.IsDistributedParameter(IDT_VALUETYPE_SHEARSTIFFNESS))
  {
    *values = ffmat.ParameterValue(IDT_VALUETYPE_COHESION) * 1.e6;
    values += stride;

    *values = ffmat.ParameterValue(IDT_VALUETYPE_FRICTION_ANGLE) * PI / 180;
    values += stride;

    *values = 0;
    values += stride;
  }

  double dsn = ffmat.ParameterValue(IDT_VALUETYPE_NORMALSTIFFNESS) * 1e6;
  double dst = ffmat.ParameterValue(IDT_VALUETYPE_SHEARSTIFFNESS) * 1e6;

  *values = dsn;
  values += stride;

  *values = dst;
  values += stride;

  *values = dst;
  values += stride;

  if (runner.Controller().AnalysisType() == CAnalysisType::AT_MIXTURE ||
    runner.Controller().AnalysisType() == CAnalysisType::AT_MIXTURE_CONTAINMENT)
  {
    *values = 1;
    values += stride;
  }

  if (runner.Controller().AnalysisType() == CAnalysisType::AT_HEAT)
  {
    *values = ffmat.ParameterValue(IDT_VALUETYPE_DFLUX);
    values += stride;
  }

}

double CInterfaceMaterial::Cohesion() const
{
  return m_dCohesion;
}

double CInterfaceMaterial::Friction() const
{
  return m_dFriction;
}

double CInterfaceMaterial::NormalStiffness() const
{
  return m_dNormalStiffness;
}

double CInterfaceMaterial::ShearStiffness() const
{
  return m_dShearStiffness;
}

double CInterfaceMaterial::DFlux() const
{
  return m_dDFlux;
}

bool CInterfaceMaterial::SetCohesion(double dValue)
{
  if(dValue > 0)
  {
    m_dCohesion = dValue;
    return true;
  }

  return false;
}

bool CInterfaceMaterial::SetFriction(double dValue)
{
  if(dValue >= 0 && dValue < 90)
  {
    m_dFriction = dValue;
    return true;
  }

  return false;
}

bool CInterfaceMaterial::SetNormalStiffness(double dValue)
{
  if(dValue > 0)
  {
    m_dNormalStiffness = dValue;
    return true;
  }

  return false;
}

bool CInterfaceMaterial::SetShearStiffness(double dValue)
{
  if(dValue > 0)
  {
    m_dShearStiffness = dValue;
    return true;
  }

  return false;
}

bool CInterfaceMaterial::SetDFlux(double dValue)
{
  if(dValue >= 0)
  {
    m_dDFlux = dValue;
    return true;
  }

  return false;
}

void CInterfaceMaterial::LoadStream(TSTREAM& stream, CStreamVersion& version, TPROGRESS& progress)
{
  IMaterialBase::LoadStream(stream, version, progress);

  stream >> m_dCohesion >> m_dFriction >> m_dNormalStiffness >> m_dShearStiffness;
  progress.Step();
}

void CInterfaceMaterial::SaveStream(TSTREAM& stream, TPROGRESS& progress)
{
  IMaterialBase::SaveStream(stream, progress);

  stream << m_dCohesion << m_dFriction << m_dNormalStiffness << m_dShearStiffness;
  progress.Step();
}

long CInterfaceMaterial::SavedItems() const
{
  long lRet = IMaterialBase::SavedItems();
  lRet += 1;

  return lRet;
}

bool CInterfaceMaterial::Empty() const
{
  return false;
}

bool CInterfaceMaterial::Less(const CGraphNode& node) const
{
  const CInterfaceMaterial* pMat = dynamic_cast<const CInterfaceMaterial*>(&node);
  if(pMat)
  {
    if(m_dCohesion < pMat->m_dCohesion) return true;
    if(m_dCohesion > pMat->m_dCohesion) return false;

    if(m_dFriction < pMat->m_dFriction) return true;
    if(m_dFriction > pMat->m_dFriction) return false;

    if(m_dNormalStiffness < pMat->m_dNormalStiffness) return true;
    if(m_dNormalStiffness > pMat->m_dNormalStiffness) return false;

    if(m_dShearStiffness < pMat->m_dShearStiffness) return true;
    if(m_dShearStiffness > pMat->m_dShearStiffness) return false;

    return (m_dDFlux < pMat->m_dDFlux);
  }

  return IMaterialBase::Less(node);
}


///// CInterfaceMaterialEntry

CInterfaceMaterialEntry::CInterfaceMaterialEntry(int nEntryId, CModelBase& model)
: CGraphEntryTemp<CInterfaceMaterial>(nEntryId, IDI_ROCK_MATERIALS, "Interface Materials", model)
{
}

void CInterfaceMaterialEntry::LoadStream(CFemAppModel& /*model*/, TSTREAM& stream, CStreamVersion& version, TPROGRESS& progress)
{
  int nMat;
  stream >> nMat;

  int i;
  for(i = 0; i < nMat; ++i)
  {
    CInterfaceMaterial* pMat = new CInterfaceMaterial(*this, QString());
    pMat->LoadStream(stream, version, progress);
  }
}

void CInterfaceMaterialEntry::SaveStream(TSTREAM& stream, TPROGRESS& progress)
{
  const TNodeSet& stNodes = EntryNodes();
  stream << int(stNodes.size());

  int nIndex = 0;
  TNodeSet::const_iterator it;
  for(it = stNodes.begin(); it != stNodes.end(); ++it)
  {
    (*it)->Index(++nIndex);
    (*it)->SaveStream(stream, progress);
  }
}

long CInterfaceMaterialEntry::SavedItems() const
{
  long lRet = 0;

  const TNodeSet& stNodes = EntryNodes();
  TNodeSet::const_iterator it;
  for(it = stNodes.begin(); it != stNodes.end(); ++it)
    lRet += (*it)->SavedItems();

  return lRet;
}

namespace
{

const QString DEFAULT_INTERFACE_MATERIAL_NAME =
  QObject::tr("New Interface Material %1");
const size_t DEFAULT_INITIAL_NUMBER = 1;

} // anonymous namespace

CInterfaceMaterial& CInterfaceMaterialEntry::InsertNew()
{
  QString sName = createUniqueName(DEFAULT_INTERFACE_MATERIAL_NAME,
    DEFAULT_INITIAL_NUMBER);
  CInterfaceMaterial* pMat =  new CInterfaceMaterial(*this, sName.toStdString().c_str());
  return *pMat;
}
