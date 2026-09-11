// BoundaryInterfaceMaterial.cpp: implementation of the CBoundaryInterfaceMaterial class.
//
//////////////////////////////////////////////////////////////////////

#include "BoundaryInterfaceMaterial.h"
#include "lbfl.h"
#include "dimple.h"
#include "GeomecDianaRunner.h"
#include "DianaRunController.h"

#ifdef _DEBUG
#ifdef _MSC_VER
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#endif  // _MSC_VER
//#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CBoundaryInterfaceMaterial::CBoundaryInterfaceMaterial(const double &krad, const double &ktan):
m_dKrad(krad), m_dKtan(ktan)
{
}

CBoundaryInterfaceMaterial::~CBoundaryInterfaceMaterial()
{
}

bool CBoundaryInterfaceMaterial::WriteFilos(dia::IDianaRunner& diarunner) const
{
  ftn_double_t dsn = (ftn_double_t)(m_dKrad); 
  ftn_double_t dst = (ftn_double_t)(m_dKtan);

  PutItemLength("DSNZ", &dsn, 1);
  PutItemLength("DSSX", &dst, 1);
  PutItemLength("DSSY", &dst, 1);

  assert(dynamic_cast<CGeomecDianaRunnerBase*>(&diarunner));
  CGeomecDianaRunnerBase& runner = static_cast<CGeomecDianaRunnerBase&>(diarunner);

  if(runner.Controller().AnalysisType() == CAnalysisType::AT_MIXTURE ||
     runner.Controller().AnalysisType() == CAnalysisType::AT_MIXTURE_CONTAINMENT)
  {
  // write very low permeability for boundary interfaces
  ftn_double_t k_mod = 1e-8;
  PutItemLength("DPERME", &k_mod, 1);
  }

  return true;
}

bool CBoundaryInterfaceMaterial::operator<(const dia::IMaterial &rhs) const
{
  const CBoundaryInterfaceMaterial *pMat = 
    dynamic_cast<const CBoundaryInterfaceMaterial*>(&rhs);

  if(pMat)
  {
    if(m_dKrad < pMat->m_dKrad)
      return true;
  if(pMat->m_dKrad < m_dKrad)
      return false;

    return (m_dKtan < pMat->m_dKtan);
  }

  return dia::IMaterial::operator<(rhs);
}

// Interface for IElementProperty
int CBoundaryInterfaceMaterial::Type() const
{
  return 102;
}

int CBoundaryInterfaceMaterial::WriteFilosParamSize(dia::IDianaRunner& diarunner) const
{
  int size = 3; // DSNZ/DSSX/DSSY

  assert(dynamic_cast<CGeomecDianaRunnerBase*>(&diarunner));
  CGeomecDianaRunnerBase& runner = static_cast<CGeomecDianaRunnerBase&>(diarunner);

  if (runner.Controller().AnalysisType() == CAnalysisType::AT_MIXTURE ||
  runner.Controller().AnalysisType() == CAnalysisType::AT_MIXTURE_CONTAINMENT)
  {
  size += 1; // DPERME
  }

  return size;
}

bool CBoundaryInterfaceMaterial::WriteFilosParamName(dia::IDianaRunner& diarunner, int i, char *name) const
{
  if (i == 0)
  {
  strncpy(name, "DSNZ", 10);
  return true;
  }
  --i;

  if (i == 0)
  {
  strncpy(name, "DSSX", 10);
  return true;
  }
  --i;

  if (i == 0)
  {
  strncpy(name, "DSSY", 10);
  return true;
  }
  --i;

  assert(dynamic_cast<CGeomecDianaRunnerBase*>(&diarunner));
  CGeomecDianaRunnerBase& runner = static_cast<CGeomecDianaRunnerBase&>(diarunner);

  if (runner.Controller().AnalysisType() == CAnalysisType::AT_MIXTURE ||
  runner.Controller().AnalysisType() == CAnalysisType::AT_MIXTURE_CONTAINMENT)
  {
  if (i == 0)
  {
      strncpy(name, "DPERME", 10);
      return true;
  }
  }

  return false;
}

void CBoundaryInterfaceMaterial::WriteFilosParamValues(dia::IDianaRunner& diarunner, double *values, int stride) const
{
  *values = m_dKrad; // DSNZ
  values += stride;

  *values = m_dKtan; // DSSX
  values += stride;

  *values = m_dKtan; // DSSY
  values += stride;

  assert(dynamic_cast<CGeomecDianaRunnerBase*>(&diarunner));
  CGeomecDianaRunnerBase& runner = static_cast<CGeomecDianaRunnerBase&>(diarunner);

  if (runner.Controller().AnalysisType() == CAnalysisType::AT_MIXTURE ||
  runner.Controller().AnalysisType() == CAnalysisType::AT_MIXTURE_CONTAINMENT)
  {
  // write very low permeability for boundary interfaces
  *values = 1e-8; // DPERME
  }
}


