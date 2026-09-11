
#include "WellCasingCementInitialMaterial.h"

#include "DepletionStage.h"
#include "DianaRunController.h"
#include "Material.h"

#include "GeomecDianaRunner.h"
#include "lbfl.h"

CWellCasingCementInitialMaterial::CWellCasingCementInitialMaterial(const CFFMaterial &base) : m_base(base) {}

bool CWellCasingCementInitialMaterial::operator<(const dia::IMaterial &rhs) const {
  const CWellCasingCementInitialMaterial *pMat = dynamic_cast<const CWellCasingCementInitialMaterial *>(&rhs);
  if (pMat)
    return m_base < pMat->m_base;

  return dia::IMaterial::operator<(rhs);
}

bool CWellCasingCementInitialMaterial::WriteFilos(dia::IDianaRunner &diarunner) const {
  assert(dynamic_cast<CGeomecDianaRunnerBase *>(&diarunner));
  CGeomecDianaRunnerBase &runner = static_cast<CGeomecDianaRunnerBase &>(diarunner);
  if (runner.Controller().StartStage().Initial())
    PutCharItem("CREEP", "NONE");

  return m_base.WriteFilos(diarunner);
}

// Interface for IElementProperty
int CWellCasingCementInitialMaterial::Type() const { return 104; }

int CWellCasingCementInitialMaterial::WriteFilosParamSize(dia::IDianaRunner &diarunner) const {
  // CREEP=NONE is not a distributed property, so we only need to redirect here
  return m_base.WriteFilosParamSize(diarunner);
}

bool CWellCasingCementInitialMaterial::WriteFilosParamName(dia::IDianaRunner &diarunner, int i, char *name) const {
  return m_base.WriteFilosParamName(diarunner, i, name);
}

void CWellCasingCementInitialMaterial::WriteFilosParamValues(dia::IDianaRunner &diarunner, double *values,
                                                             int stride) const {
  return m_base.WriteFilosParamValues(diarunner, values, stride);
}
