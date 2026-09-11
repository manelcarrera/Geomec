
#include "MaterialModelModifiedMohrCoulomb.h"
#include "MaterialModelLinear.h"

#include "MaterialUnitTypes.h"
#include "mlMatParamDomain.h"
#include "Materials.h"
#include "ValueTypes.h"
#include "IObject.h"
#include "MaterialHelperFactory.h"

namespace {

const QString GROUP_MANUAL     = "Manual";
const QString GROUP_BULK       = "Bulk stiffness";
const QString GROUP_DYNAMIC    = "Dynamic stiffness";
const QString GROUP_VELOCITIES = "Velocities";
const QString GROUP_COMPRESSIBILITY = "Compressibility";

}

void CMaterialModifiedMohrCoulombCreatorV2::OnCreateLibrary(CLibraryMaterial& /*m*/)
{
}

void CMaterialModifiedMohrCoulombCreatorV2::OnCreateLibraryParameters(CLibraryMaterial& m)
{
  const CMaterialHelperFactory *f = CMaterialHelperFactory::Instance();
  assert(f);

  AddNumber(m, 30);

  AddLibParam(MLD_YOUNGMODULUS,        DEF_E ,             m, f->getMPUnitType(CMaterialHelperFactory::UT_STRESS),         IDT_VALUETYPE_YOUNGS_MODULUS,    f->getMPCheckStrategy(CMaterialHelperFactory::MP_GT_0),              f->getMPSetStrategy(CMaterialHelperFactory::MP_SET_LinearYoung),   YOUNG_FILOSPATH,  false, GROUP_MANUAL);
  AddLibParam(MLD_YOUNGMODULUS_DECOMP, DEF_E ,             m, f->getMPUnitType(CMaterialHelperFactory::UT_STRESS),         IDT_VALUETYPE_YOUNG_DECOMP,      f->getMPCheckStrategy(CMaterialHelperFactory::MP_GT_0),              0, QString(), false);
  AddLibParam(MLD_POISSONRATIO,        DEF_NU,             m, f->getMPUnitType(CMaterialHelperFactory::UT_NONE),           IDT_VALUETYPE_POISSONS_RATIO,    f->getMPCheckStrategy(CMaterialHelperFactory::MP_GE_0_LT_5Emin1),    f->getMPSetStrategy(CMaterialHelperFactory::MP_SET_LinearPoisson), POISON_FILOSPATH, false, GROUP_MANUAL);
  AddLibParam(MLD_POISSONRATIO_DECOMP, DEF_NU,             m, f->getMPUnitType(CMaterialHelperFactory::UT_NONE),           IDT_VALUETYPE_POISSON_DECOMP,    f->getMPCheckStrategy(CMaterialHelperFactory::MP_GE_0_LT_5Emin1),    0, QString(), false);
  AddLibParam(MLD_BULKSTIFFNESS,       0,                  m, f->getMPUnitType(CMaterialHelperFactory::UT_STRESS),         IDT_VALUETYPE_BULKSTIFFNESS,     f->getMPCheckStrategy(CMaterialHelperFactory::MP_LinearBulkStiff),   f->getMPSetStrategy(CMaterialHelperFactory::MP_SET_LinearBulkStiff), QString(), false,                        GROUP_BULK);
  AddLibParam(MLD_SHEARMODULUS,        0,                  m, f->getMPUnitType(CMaterialHelperFactory::UT_STRESS),         IDT_VALUETYPE_SHEARMODULUS,      f->getMPCheckStrategy(CMaterialHelperFactory::MP_LinearShearStiff),  f->getMPSetStrategy(CMaterialHelperFactory::MP_SET_LinearShearStiff), QString(), false,                       GROUP_BULK);
  AddLibParam(MLD_DYNUNISTIFFNESS,     0,                  m, f->getMPUnitType(CMaterialHelperFactory::UT_STRESS),         IDT_VALUETYPE_DYNUNISTIFFNESS,   f->getMPCheckStrategy(CMaterialHelperFactory::MP_LinearDynUni),      f->getMPSetStrategy(CMaterialHelperFactory::MP_SET_LinearDynUni), QString(), false,                           GROUP_DYNAMIC);
  AddLibParam(MLD_DYNSHEARSTIFFNESS,   0,                  m, f->getMPUnitType(CMaterialHelperFactory::UT_STRESS),         IDT_VALUETYPE_DYNSHEARSTIFFNESS, f->getMPCheckStrategy(CMaterialHelperFactory::MP_LinearDynShear),    f->getMPSetStrategy(CMaterialHelperFactory::MP_SET_LinearDynShear), QString(), false,                         GROUP_DYNAMIC);
  AddLibParam(MLD_VELOCITYP,           0,                  m, f->getMPUnitType(CMaterialHelperFactory::UT_VELOCITY),       IDT_VALUETYPE_VELOCITYP,         f->getMPCheckStrategy(CMaterialHelperFactory::MP_LinearVelocityP),   f->getMPSetStrategy(CMaterialHelperFactory::MP_SET_LinearVelocityP), QString(), false,                        GROUP_VELOCITIES);
  AddLibParam(MLD_VELOCITYS,           0,                  m, f->getMPUnitType(CMaterialHelperFactory::UT_VELOCITY),       IDT_VALUETYPE_VELOCITYS,         f->getMPCheckStrategy(CMaterialHelperFactory::MP_LinearVelocityS),   f->getMPSetStrategy(CMaterialHelperFactory::MP_SET_LinearVelocityS), QString(), false,                        GROUP_VELOCITIES);
  
  AddLibParam(MLD_CME,                 DEF_CME,            m, f->getMPUnitType(CMaterialHelperFactory::UT_PER_STRESS),     IDT_VALUETYPE_ELASCOMPRES,       f->getMPCheckStrategy(CMaterialHelperFactory::MP_GT_0),              f->getMPSetStrategy(CMaterialHelperFactory::MP_SET_LinearCME), QString(), false, GROUP_COMPRESSIBILITY);
  AddLibParam(MLD_RHOB,                DEF_RHOB,           m, f->getMPUnitType(CMaterialHelperFactory::UT_DENSITY),        IDT_VALUETYPE_RHOB,              f->getMPCheckStrategy(CMaterialHelperFactory::MP_GT_0),              f->getMPSetStrategy(CMaterialHelperFactory::MP_SET_LinearRhoB), QString(), false);
  AddLibParam(MLD_FRICTIONANGLE,       DEF_FRICTION,       m, f->getMPUnitType(CMaterialHelperFactory::UT_ANGLE),          IDT_VALUETYPE_FRICTION_ANGLE,    f->getMPCheckStrategy(CMaterialHelperFactory::MP_GE_0_LT_70),        0,                                     FRICTI_FILOSPATH, false);
  AddLibParam(MLD_COHESION,            DEF_COHESION,       m, f->getMPUnitType(CMaterialHelperFactory::UT_STRESS),         IDT_VALUETYPE_COHESION,          f->getMPCheckStrategy(CMaterialHelperFactory::MP_GE_0),              0,                                     COHESI_FILOSPATH, false);
  AddLibParam(MLD_PRECONSOLIDATION,    DEF_PRECONSO,       m, f->getMPUnitType(CMaterialHelperFactory::UT_STRESS),         IDT_VALUETYPE_PRECONSOLIDATION,  f->getMPCheckStrategy(CMaterialHelperFactory::MP_GT_0),              0,                                     PRECON_FILOSPATH, false);
  AddLibParam(MLD_HARDENING,           DEF_HARDENING,      m, f->getMPUnitType(CMaterialHelperFactory::UT_NONE),           IDT_VALUETYPE_HARDENING,         f->getMPCheckStrategy(CMaterialHelperFactory::MP_GT_0_LE_1),         0,                                     LAMBDA_FILOSPATH, false);
  AddLibParam(MLD_POROSITY,            DEF_PORO,           m, f->getMPUnitType(CMaterialHelperFactory::UT_NONE),           IDT_VALUETYPE_POROSITY,          f->getMPCheckStrategy(CMaterialHelperFactory::MP_GE_0_LT_1),         0,                                     POROSI_FILOSPATH, true);
  AddLibParam(MLD_GRAINSTIFFNESS,      DEF_GRAINSTIFFNESS, m, f->getMPUnitType(CMaterialHelperFactory::UT_STRESS),         IDT_VALUETYPE_GRAINSTIFFNESS,    f->getMPCheckStrategy(CMaterialHelperFactory::MP_GE_10_LE_1E6),      0,                                     BULKS_FILOSPATH,  true);
  AddLibParam(MLD_PERMEA,              DEF_PERMEA,         m, f->getMPUnitType(CMaterialHelperFactory::UT_PERMEA),         IDT_VALUETYPE_PERMEA,            f->getMPCheckStrategy(CMaterialHelperFactory::MP_GE_0),              0, QString(), false);
  AddLibParam(MLD_VISCOSITY,           DEF_VISCOSITY,      m, f->getMPUnitType(CMaterialHelperFactory::UT_VISCOSITY),      IDT_VALUETYPE_VISCOSITY,         f->getMPCheckStrategy(CMaterialHelperFactory::MP_GT_0),              0, QString(), false);
  
  AddLibParam(MLD_CAPSHAPEPARAM,       DEF_CAPSHAPEPARAM,  m, f->getMPUnitType(CMaterialHelperFactory::UT_NONE),           IDT_VALUETYPE_CAPSHAPE,          f->getMPCheckStrategy(CMaterialHelperFactory::MP_GE_1Emin1_LE_2),    0,                                     ALPHA_FILOSPATH,  false);
  AddLibParam(MLD_LATRATIO_MIN,        DEF_LATRATIO_MIN,   m, f->getMPUnitType(CMaterialHelperFactory::UT_NONE),           IDT_VALUETYPE_LATRATIO_MIN,      f->getMPCheckStrategy(CMaterialHelperFactory::MP_LinearLatRatioMin), 0, QString(), false);
  AddLibParam(MLD_LATRATIO_MAX,        DEF_LATRATIO_MAX,   m, f->getMPUnitType(CMaterialHelperFactory::UT_NONE),           IDT_VALUETYPE_LATRATIO_MAX,      f->getMPCheckStrategy(CMaterialHelperFactory::MP_LinearLatRatioMax), 0, QString(), false);
  AddLibParam(MLD_AZIMUTH,             DEF_AZIMUTH,        m, f->getMPUnitType(CMaterialHelperFactory::UT_ANGLE),          IDT_VALUETYPE_AZIMUTH,           f->getMPCheckStrategy(CMaterialHelperFactory::MP_GE_0_LE_360),       0, QString(), false);
  AddLibParam(MLD_THERMX,              DEF_THERMX,         m, f->getMPUnitType(CMaterialHelperFactory::UT_PER_TEMPER),     IDT_VALUETYPE_THERMAL_EXPANSION, f->getMPCheckStrategy(CMaterialHelperFactory::MP_GE_0_LE_1Emin3),    0, QString(), false);
  AddLibParam(MLD_THERM_CONDUCT,       DEF_THERM_CONDUCT,  m, f->getMPUnitType(CMaterialHelperFactory::UT_THERM_CONDUCT),  IDT_VALUETYPE_THERM_CONDUCT,     f->getMPCheckStrategy(CMaterialHelperFactory::MP_GE_0),              0, QString(), false);
  AddLibParam(MLD_THERM_CAPACI,        DEF_THERM_CAPACI,   m, f->getMPUnitType(CMaterialHelperFactory::UT_THERM_CAPACITY), IDT_VALUETYPE_THERM_CAPACI,      f->getMPCheckStrategy(CMaterialHelperFactory::MP_GE_0),              0, QString(), false);
  AddLibParam(MLD_FLUIDMODULUS,        DEF_FLUIDMOD,       m, f->getMPUnitType(CMaterialHelperFactory::UT_STRESS),         IDT_VALUETYPE_FLUID_BULK_MOD,    f->getMPCheckStrategy(CMaterialHelperFactory::MP_GT_0),              0, QString(), false);
  AddLibParam(MLD_FLUIDX,              DEF_FLUIDX,         m, f->getMPUnitType(CMaterialHelperFactory::UT_PER_TEMPER),     IDT_VALUETYPE_FLUIDX,            f->getMPCheckStrategy(CMaterialHelperFactory::MP_GE_0_LE_1Emin1),    0, QString(), false);
  AddLibParam(MLD_FLUID_DENSITY,       DEF_FLUID_DENSITY,  m, f->getMPUnitType(CMaterialHelperFactory::UT_DENSITY),        IDT_VALUETYPE_FLUID_DENSITY,     f->getMPCheckStrategy(CMaterialHelperFactory::MP_GT_0_LE_10000),     0, QString(), false);
}

int CMaterialModifiedMohrCoulombCreatorV2::MaterialModel()
{
  return MM_MODIFIEDMOHRCOULOMB;
}

QString CMaterialModifiedMohrCoulombCreatorV2::MaterialModelName()
{
  return MLD_MODIFIEDMOHRCOULOMB;
}

QString CMaterialModifiedMohrCoulombCreatorV2::CalibrationPath() const
{
  return "MMOHRC";
}

IPQPlotter* CMaterialModifiedMohrCoulombCreatorV2::PQPlotter() const
{
  const CMaterialHelperFactory *f = CMaterialHelperFactory::Instance();
  assert(f);

  return f->getMatPQPlotter(CMaterialHelperFactory::MAT_PQ_ModifiedMohrCoulomb);
}

IPQPlotter* CMaterialModifiedMohrCoulombCreatorV2::CPQPlotter::Clone() const
{
  const CMaterialHelperFactory *f = CMaterialHelperFactory::Instance();
  assert(f);

  return f->getMatPQPlotter(CMaterialHelperFactory::MAT_PQ_ModifiedMohrCoulomb);
}

#ifdef min
#undef min  // To give std::min precedence over #define min(...)
#endif  // min

void CMaterialModifiedMohrCoulombCreatorV2::CPQPlotter::GetPrimaryYieldCurve(const CLibraryMaterial &mat, double /*dPMin*/, double dPMax, CStressStrainArray &values) const
{
  const ml::CMatParam *pPhi      = mat.Parameter(MLD_FRICTIONANGLE);
	const ml::CMatParam *pCohesion = mat.Parameter(MLD_COHESION);
	const ml::CMatParam *pPreConso = mat.Parameter(MLD_PRECONSOLIDATION);
	const ml::CMatParam *pCapShape = mat.Parameter(MLD_CAPSHAPEPARAM);

	double dPhi      = pPhi->Value() * PI / 180; // phi in radians
	double dCohesion = pCohesion->Value();
	double dPreConso = pPreConso->Value();
	double dDeltaP   = dCohesion / tan(dPhi);
	double dCapShape = pCapShape->Value();

	double k = 6 * dCohesion * cos(dPhi) / (3 - sin(dPhi));
	double m = 6 * sin(dPhi) / (3 - sin(dPhi));

	// intersection point of line and circle segment
	double dPInt = ((-dDeltaP / dCapShape) - k * m + sqrt(pow((dDeltaP / dCapShape) + k * m, 2) - (m*m + 1/dCapShape) * (k*k + (dDeltaP*dDeltaP)/dCapShape - (dPreConso*dPreConso)/dCapShape))) /
		(m*m + 1/dCapShape);

  double dPEnd = std::min(dPMax, dPInt);
	double dQEnd = k + m * dPEnd;

	CStressStrain pt1(-dDeltaP, 0);
	CStressStrain pt2(dPEnd, dQEnd);
	values.push_back(pt1);
	values.push_back(pt2);

	if(dPInt < dPMax)
	{
    dPEnd = std::min(dPMax, dPreConso - dDeltaP);
		for(int i=0; i<NUM_PQ_STEPS; i++)
		{
			double p = (double) i / NUM_PQ_STEPS * (dPEnd - dPInt) + dPInt;
			double q = sqrt((dPreConso*dPreConso - (p + dDeltaP)*(p + dDeltaP))/dCapShape);
			CStressStrain pt(p, q);
			values.push_back(pt);
		}

		double p = dPEnd;
		double q = sqrt((dPreConso*dPreConso - (p + dDeltaP)*(p + dDeltaP))/dCapShape);
		CStressStrain pt(p, q);
		values.push_back(pt);
	}
}


void CMaterialModifiedMohrCoulombCreatorDEPRECATED::OnCreateLibrary(CLibraryMaterial& /*m*/)
{
}

void CMaterialModifiedMohrCoulombCreatorDEPRECATED::OnCreateLibraryParameters(CLibraryMaterial& m)
{
  Add(new CLibraryMaterialParameter(MLD_YOUNGMODULUS,        DEF_E ,             m, *new CUnitTypeStress,        IDT_VALUETYPE_YOUNGS_MODULUS,    new MP_GT(0),                                new CMaterialLinearYoungSetStrategy,   YOUNG_FILOSPATH,  false), GROUP_MANUAL);
  Add(new CLibraryMaterialParameter(MLD_YOUNGMODULUS_DECOMP, DEF_E ,             m, *new CUnitTypeStress,        IDT_VALUETYPE_YOUNG_DECOMP,      new MP_GT(0),                                0));
  Add(new CLibraryMaterialParameter(MLD_POISSONRATIO,        DEF_NU,             m, *new CUnitTypeNone,          IDT_VALUETYPE_POISSONS_RATIO,    new MP_GE_LT(0, 0.5),                        new CMaterialLinearPoissonSetStrategy, POISON_FILOSPATH, false), GROUP_MANUAL);
  Add(new CLibraryMaterialParameter(MLD_POISSONRATIO_DECOMP, DEF_NU,             m, *new CUnitTypeNone,          IDT_VALUETYPE_POISSON_DECOMP,    new MP_GE_LT(0, 0.5),                        0));
  Add(new CLibraryMaterialParameter(MLD_BULKSTIFFNESS,       0,                  m, *new CUnitTypeStress,        IDT_VALUETYPE_BULKSTIFFNESS,     new CMaterialLinearBulkStiffCheckStrategy,   new CMaterialLinearBulkStiffSetStrategy),                        GROUP_BULK);
  Add(new CLibraryMaterialParameter(MLD_SHEARMODULUS,        0,                  m, *new CUnitTypeStress,        IDT_VALUETYPE_SHEARMODULUS,      new CMaterialLinearShearStiffCheckStrategy,  new CMaterialLinearShearStiffSetStrategy),                       GROUP_BULK);
  Add(new CLibraryMaterialParameter(MLD_DYNUNISTIFFNESS,     0,                  m, *new CUnitTypeStress,        IDT_VALUETYPE_DYNUNISTIFFNESS,   new CMaterialLinearDynUniCheckStrategy,      new CMaterialLinearDynUniSetStrategy),                           GROUP_DYNAMIC);
  Add(new CLibraryMaterialParameter(MLD_DYNSHEARSTIFFNESS,   0,                  m, *new CUnitTypeStress,        IDT_VALUETYPE_DYNSHEARSTIFFNESS, new CMaterialLinearDynShearCheckStrategy,    new CMaterialLinearDynShearSetStrategy),                         GROUP_DYNAMIC);
  Add(new CLibraryMaterialParameter(MLD_VELOCITYP,           0,                  m, *new CUnitTypeVelocity,      IDT_VALUETYPE_VELOCITYP,         new CMaterialLinearVelocityPCheckStrategy,   new CMaterialLinearVelocityPSetStrategy),                        GROUP_VELOCITIES);
  Add(new CLibraryMaterialParameter(MLD_VELOCITYS,           0,                  m, *new CUnitTypeVelocity,      IDT_VALUETYPE_VELOCITYS,         new CMaterialLinearVelocitySCheckStrategy,   new CMaterialLinearVelocitySSetStrategy),                        GROUP_VELOCITIES);
  Add(new CLibraryMaterialParameter(MLD_CME,                 DEF_CME,            m, *new CUnitTypePerStress,     IDT_VALUETYPE_ELASCOMPRES,       new MP_GT(0),                                new CMaterialLinearCMESetStrategy));
  Add(new CLibraryMaterialParameter(MLD_RHOB,                DEF_RHOB,           m, *new CUnitTypeDensity,       IDT_VALUETYPE_RHOB,              new MP_GT(0),                                new CMaterialLinearRhoBSetStrategy));
  Add(new CLibraryMaterialParameter(MLD_FRICTIONANGLE,       DEF_FRICTION,       m, *new CUnitTypeAngle,         IDT_VALUETYPE_FRICTION_ANGLE,    new MP_GE_LT(0, 70),                         0,                                     FRICTI_FILOSPATH, false));
  Add(new CLibraryMaterialParameter(MLD_COHESION,            DEF_COHESION,       m, *new CUnitTypeStress,        IDT_VALUETYPE_COHESION,          new MP_GE(0),                                0,                                     COHESI_FILOSPATH, false));
  Add(new CLibraryMaterialParameter(MLD_PRECONSOLIDATION,    DEF_PRECONSO,       m, *new CUnitTypeStress,        IDT_VALUETYPE_PRECONSOLIDATION,  new MP_GT(0),                                0,                                     PRECON_FILOSPATH, false));
  Add(new CLibraryMaterialParameter(MLD_HARDENING,           DEF_HARDENING,      m, *new CUnitTypeNone,          IDT_VALUETYPE_HARDENING,         new MP_GT_LE(0, 1),                          0,                                     LAMBDA_FILOSPATH, false));
  Add(new CLibraryMaterialParameter(MLD_POROSITY,            DEF_PORO,           m, *new CUnitTypeNone,          IDT_VALUETYPE_POROSITY,          new MP_GE_LT(0, 1),                          0,                                     POROSI_FILOSPATH, true));
  Add(new CLibraryMaterialParameter(MLD_GRAINSTIFFNESS,      DEF_GRAINSTIFFNESS, m, *new CUnitTypeStress,        IDT_VALUETYPE_GRAINSTIFFNESS,    new MP_GE_LE(10, 1000000),                   0,                                     BULKS_FILOSPATH,  true));
  Add(new CLibraryMaterialParameter(MLD_PERMEA,              DEF_PERMEA,         m, *new CUnitTypePermea,        IDT_VALUETYPE_PERMEA,            new MP_GE(0),                                0));
  Add(new CLibraryMaterialParameter(MLD_VISCOSITY,           DEF_VISCOSITY,      m, *new CUnitTypeViscosity,     IDT_VALUETYPE_VISCOSITY,         new MP_GT(0),                                0));
  Add(new CLibraryMaterialParameter(MLD_CAPSHAPEPARAM,       DEF_CAPSHAPEPARAM,  m, *new CUnitTypeNone,          IDT_VALUETYPE_CAPSHAPE,          new MP_GE_LE(0.1, 2),                        0,                                     ALPHA_FILOSPATH,  false));
  Add(new CLibraryMaterialParameter(MLD_LATRATIO_MIN,        DEF_LATRATIO_MIN,   m, *new CUnitTypeNone,          IDT_VALUETYPE_LATRATIO_MIN,      new CMaterialLinearLatRatioMinCheckStrategy, 0));
  Add(new CLibraryMaterialParameter(MLD_LATRATIO_MAX,        DEF_LATRATIO_MAX,   m, *new CUnitTypeNone,          IDT_VALUETYPE_LATRATIO_MAX,      new CMaterialLinearLatRatioMaxCheckStrategy, 0));
  Add(new CLibraryMaterialParameter(MLD_AZIMUTH,             DEF_AZIMUTH,        m, *new CUnitTypeAngle,         IDT_VALUETYPE_AZIMUTH,           new MP_GE_LE(0, 360),                        0));
  Add(new CLibraryMaterialParameter(MLD_THERMX,              DEF_THERMX,         m, *new CUnitTypePerTemper,     IDT_VALUETYPE_THERMAL_EXPANSION, new MP_GE_LE(0, 1e-3),                       0));
  Add(new CLibraryMaterialParameter(MLD_THERM_CONDUCT,       DEF_THERM_CONDUCT,  m, *new CUnitTypeThermConduct,  IDT_VALUETYPE_THERM_CONDUCT,     new MP_GE(0),                                0));
  Add(new CLibraryMaterialParameter(MLD_THERM_CAPACI,        DEF_THERM_CAPACI,   m, *new CUnitTypeThermCapacity, IDT_VALUETYPE_THERM_CAPACI,      new MP_GE(0),                                0));
  Add(new CLibraryMaterialParameter(MLD_FLUIDMODULUS,        DEF_FLUIDMOD,       m, *new CUnitTypeStress,        IDT_VALUETYPE_FLUID_BULK_MOD,    new MP_GT(0),                                0));
  Add(new CLibraryMaterialParameter(MLD_FLUIDX,              DEF_FLUIDX,         m, *new CUnitTypePerTemper,     IDT_VALUETYPE_FLUIDX,            new MP_GE_LE(0, 0.1),                        0));
  Add(new CLibraryMaterialParameter(MLD_FLUID_DENSITY,       DEF_FLUID_DENSITY,  m, *new CUnitTypeDensity,       IDT_VALUETYPE_FLUID_DENSITY,     new MP_GT_LE(0, 10000),                      0));
}

int CMaterialModifiedMohrCoulombCreatorDEPRECATED::MaterialModel()
{
  return MM_MODIFIEDMOHRCOULOMB;
}

QString CMaterialModifiedMohrCoulombCreatorDEPRECATED::MaterialModelName()
{
  return MLD_MODIFIEDMOHRCOULOMB;
}

QString CMaterialModifiedMohrCoulombCreatorDEPRECATED::CalibrationPath() const
{
  return "MMOHRC";
}

IPQPlotter* CMaterialModifiedMohrCoulombCreatorDEPRECATED::PQPlotter() const
{
  return new CPQPlotter;
}

IPQPlotter* CMaterialModifiedMohrCoulombCreatorDEPRECATED::CPQPlotter::Clone() const
{
  return new CPQPlotter;
}

#ifdef min
#undef min  // To give std::min precedence over #define min(...)
#endif  // min

void CMaterialModifiedMohrCoulombCreatorDEPRECATED::CPQPlotter::GetPrimaryYieldCurve(const CLibraryMaterial &mat, double /*dPMin*/, double dPMax, CStressStrainArray &values) const
{
  const ml::CMatParam *pPhi      = mat.Parameter(MLD_FRICTIONANGLE);
	const ml::CMatParam *pCohesion = mat.Parameter(MLD_COHESION);
	const ml::CMatParam *pPreConso = mat.Parameter(MLD_PRECONSOLIDATION);
	const ml::CMatParam *pCapShape = mat.Parameter(MLD_CAPSHAPEPARAM);

	double dPhi      = pPhi->Value() * PI / 180; // phi in radians
	double dCohesion = pCohesion->Value();
	double dPreConso = pPreConso->Value();
	double dDeltaP   = dCohesion / tan(dPhi);
	double dCapShape = pCapShape->Value();

	double k = 6 * dCohesion * cos(dPhi) / (3 - sin(dPhi));
	double m = 6 * sin(dPhi) / (3 - sin(dPhi));

	// intersection point of line and circle segment
	double dPInt = ((-dDeltaP / dCapShape) - k * m + sqrt(pow((dDeltaP / dCapShape) + k * m, 2) - (m*m + 1/dCapShape) * (k*k + (dDeltaP*dDeltaP)/dCapShape - (dPreConso*dPreConso)/dCapShape))) /
		(m*m + 1/dCapShape);

  double dPEnd = std::min(dPMax, dPInt);
	double dQEnd = k + m * dPEnd;

	CStressStrain pt1(-dDeltaP, 0);
	CStressStrain pt2(dPEnd, dQEnd);
	values.push_back(pt1);
	values.push_back(pt2);

	if(dPInt < dPMax)
	{
    dPEnd = std::min(dPMax, dPreConso - dDeltaP);
		for(int i=0; i<NUM_PQ_STEPS; i++)
		{
			double p = (double) i / NUM_PQ_STEPS * (dPEnd - dPInt) + dPInt;
			double q = sqrt((dPreConso*dPreConso - (p + dDeltaP)*(p + dDeltaP))/dCapShape);
			CStressStrain pt(p, q);
			values.push_back(pt);
		}

		double p = dPEnd;
		double q = sqrt((dPreConso*dPreConso - (p + dDeltaP)*(p + dDeltaP))/dCapShape);
		CStressStrain pt(p, q);
		values.push_back(pt);
	}
}
