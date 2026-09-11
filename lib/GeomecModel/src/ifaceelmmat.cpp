
#include "ifaceelmmat.h"
#include "ValueTypes.h"
#include "DianaRunController.h"
#include "lbfl.h"
#include "GeomecDianaRunner.h"

CInterfaceElementMaterial::CInterfaceElementMaterial(const ftn_double_t& cohesion, 
		                                                 const ftn_double_t& friction, 
							                                       const ftn_double_t& dstiffnormal,
							                                       const ftn_double_t& dstiffshear,
						                                         CHorizonBase::TSlipType slip_type)
: m_cohesion( cohesion ),
  m_friction( friction ),
  m_dstiffnormal( dstiffnormal ),
  m_dstiffshear( dstiffshear ),
  m_slip_type( slip_type )
{
}


// wjrx mantis 2545
bool CInterfaceElementMaterial::ValidParameterValue(unsigned int ValueTypeID) const
{
	switch(ValueTypeID)
	{
	case IDT_VALUETYPE_FRICTION_ANGLE:
		return true;
	case IDT_VALUETYPE_COHESION:
		return true;
	}

	return false;
}

double CInterfaceElementMaterial::ParameterValue(unsigned int ValueTypeID) const
{
	switch(ValueTypeID)
	{
	case IDT_VALUETYPE_FRICTION_ANGLE:
		return m_friction;
	case IDT_VALUETYPE_COHESION:
		return m_cohesion;
	}

	assert(FALSE);
	return 0;
}

bool CInterfaceElementMaterial::WriteFilos(dia::IDianaRunner& diarunner) const
{
  // the dstif values are user-defined when slip type is USER, otherwise
  // they're hardcoded here
	ftn_double_t dstif[2];

	if( m_slip_type == CHorizonBase::USER )
	{
	  // Write cohesion and friction
	  SetActive("FRICTI", FTN_TRUE);

	  ftn_double_t cohesi = ftn_double_t(m_cohesion * 1.e6);
    PutItemLength("COHESI", &cohesi, 1);

	  ftn_double_t phi = ftn_double_t(m_friction * PI / 180);
    PutItemLength("PHI", &phi, 1);

	  ftn_double_t psi = 0.;
	  PutItem("PSI", &psi);

    dstif[0] = m_dstiffnormal * 1e6;
    dstif[1] = m_dstiffshear * 1e6;
	}
	else if( m_slip_type == CHorizonBase::SLIP )
	{
    dstif[0] = 1e9;
    dstif[1] = 1e3;
	}
  else if(m_slip_type == CHorizonBase::STICK)
  {
    dstif[0] = 1e9;
    dstif[1] = 1e9;
  }
  else
  {
    assert(m_slip_type == CHorizonBase::FRACTURE);
    dstif[0] = m_dstiffnormal * 1e6;
    dstif[1] = m_dstiffshear * 1e6;

    ftn_double_t dustnz[6];
    dustnz[0] = -1000;
    dustnz[1] = -1000 * dstif[0];
    dustnz[2] = 0;
    dustnz[3] = 0;
    dustnz[4] = 1000;
    dustnz[5] = 0;
    PutItemLength("DUSTNZ", dustnz, 6);
  }

  ftn_double_t dsn = dstif[0];
  ftn_double_t dst = dstif[1];

  PutItemLength("DSNZ", &dsn, 1);
  PutItemLength("DSSX", &dst, 1);

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
    ftn_double_t dflux = 1.e9;
    PutItemLength("DFLUX", &dflux, 1);
  }

	return true;
}

bool CInterfaceElementMaterial::operator<(const dia::IMaterial &rhs) const
{
	const CInterfaceElementMaterial *mat = dynamic_cast<const CInterfaceElementMaterial *> (&rhs);

	if(mat)
	{
	  if(m_slip_type < mat->m_slip_type) return true;
	  if(m_slip_type > mat->m_slip_type) return false;

	  if(m_slip_type == CHorizonBase::USER)
    {
      if(m_cohesion < mat->m_cohesion) return true;
	    else if(m_cohesion > mat->m_cohesion) return false;

      if(m_friction < mat->m_friction) return true;
      else if(m_friction > mat->m_friction) return false;

      if(m_dstiffnormal < mat->m_dstiffnormal) return true;
      else if(m_dstiffnormal > mat->m_dstiffnormal) return false;

      if(m_dstiffshear < mat->m_dstiffshear) return true;
	  }

    return false;
  }
	
  return dia::IMaterial::operator<(rhs);
}

// Interface for IElementProperty
int CInterfaceElementMaterial::Type() const
{
  return 103;
}

bool CInterfaceElementMaterial::isInterfaceElementMaterial() const
{
  return true;
}

bool CInterfaceElementMaterial::sameSlip(const CInterfaceElementMaterial& rhs) const
{
  return m_slip_type == rhs.m_slip_type;
}

int CInterfaceElementMaterial::WriteFilosParamSize(dia::IDianaRunner& diarunner) const
{
  int size = 0;

  if (m_slip_type == CHorizonBase::USER)
  {
    size += 3; // COHESI/PHI/PSI
  }
  else if (m_slip_type == CHorizonBase::FRACTURE)
  {
    size += 6; //DUSTNZ(6)
  }

  size += 2; // DSNZ/DSSX

  assert(dynamic_cast<CGeomecDianaRunnerBase*>(&diarunner));
  CGeomecDianaRunnerBase& runner = static_cast<CGeomecDianaRunnerBase&>(diarunner);

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

bool CInterfaceElementMaterial::WriteFilosParamName(dia::IDianaRunner& diarunner, int i, char *name) const
{
  if (m_slip_type == CHorizonBase::USER)
  {
    if (i == 0)
    {
      strncpy(name, "COHESI", 10);
      return true;
    }
    --i;

    if (i == 0)
    {
      strncpy(name, "PHI", 10);
      return true;
    }
    --i;

    if (i == 0)
    {
      strncpy(name, "PSI", 10);
      return true;
    }
    --i;
  }
  else if (m_slip_type == CHorizonBase::FRACTURE)
  {
    if (i < 6)
    {
      QString dustnz = QString("DUSTNZ(%1)").arg(i + 1);
      strncpy(name, dustnz.toStdString().c_str(), 10);
      return true;
    }
    i -= 6;
  }

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
    --i;
  }

  if (runner.Controller().AnalysisType() == CAnalysisType::AT_HEAT)
  {
    if (i == 0)
    {
      strncpy(name, "DFLUX", 10);
      return true;
    }
    --i;
  }

  return false;
}

void CInterfaceElementMaterial::WriteFilosParamValues(dia::IDianaRunner& diarunner, double *values, int stride) const
{
  // the dstif values are user-defined when slip type is USER, otherwise
  // they're hardcoded here
  ftn_double_t dstif[2];

  if (m_slip_type == CHorizonBase::USER)
  {
    *values = m_cohesion * 1.e6; // COHESI
    values += stride;

    *values = m_friction * PI / 180; // PHI
    values += stride;

    *values = 0; // PSI
    values += stride;

    dstif[0] = m_dstiffnormal * 1e6;
    dstif[1] = m_dstiffshear * 1e6;
  }
  else if (m_slip_type == CHorizonBase::SLIP)
  {
    dstif[0] = 1e9;
    dstif[1] = 1e3;
  }
  else if (m_slip_type == CHorizonBase::STICK)
  {
    dstif[0] = 1e9;
    dstif[1] = 1e9;
  }
  else
  {
    assert(m_slip_type == CHorizonBase::FRACTURE);
    dstif[0] = m_dstiffnormal * 1e6;
    dstif[1] = m_dstiffshear * 1e6;

    *values = -1000; // DUSTNZ(1)
    values += stride;

    *values = -1000 * dstif[0]; // DUSTNZ(2)
    values += stride;

    *values = 0; // DUSTNZ(3)
    values += stride;

    *values = 0; // DUSTNZ(4)
    values += stride;

    *values = 1000; // DUSTNZ(5)
    values += stride;

    *values = 0; // DUSTNZ(6)
    values += stride;
  }

  *values = dstif[0]; // DSNZ
  values += stride;

  *values = dstif[1]; // DSSX
  values += stride;

  assert(dynamic_cast<CGeomecDianaRunnerBase*>(&diarunner));
  CGeomecDianaRunnerBase& runner = static_cast<CGeomecDianaRunnerBase&>(diarunner);

  if (runner.Controller().AnalysisType() == CAnalysisType::AT_MIXTURE ||
    runner.Controller().AnalysisType() == CAnalysisType::AT_MIXTURE_CONTAINMENT)
  {
    // write very high permeability for faults
    *values = 1; // DPERME
    values += stride;
  }

  if (runner.Controller().AnalysisType() == CAnalysisType::AT_HEAT)
  {
    // dflux
    *values = 1.e9; // DFLUX
    values += stride;
  }
}