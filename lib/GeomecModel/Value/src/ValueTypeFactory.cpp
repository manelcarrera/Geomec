// ValueTypeFactory.cpp: implementation of the CValueTypeFactory class.
//
//////////////////////////////////////////////////////////////////////

#include "ValueTypeFactory.h"
#include "PointSet.h"
#include "ValueTensor.h"
#include "ValueVector.h"
#include "ValueStrain.h"
#include "HexaModel.h"
#include "HexaSurface.h" 
#include "TetraSurface.h" 
#include "FractTensor.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CValueTypeBuilder::~CValueTypeBuilder()
{
}

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

void CValueTypeFactory::AppendValueTypeBuilder(unsigned int uValueType, unsigned int uNameId, unsigned int uExportId, CValueTypeBuilder *pBuilder)
{
	assert(pBuilder);

	CValueTypeIdContainer::instance()->add(uExportId);

	// Create new builder entry
	VERIFY(m_mpBuilder.insert(TBuilderMap::value_type(uValueType, TBuilder(pBuilder, uNameId))).second);

	// Create new import entry
	QString sImportTag;
	sImportTag = getStringTableEntry(uExportId);
	VERIFY(m_mpImport.insert(TImportMap::value_type(sImportTag, uValueType)).second);
  VERIFY(m_mpValueType2ImportTag.insert(
    TValueType2ImportTag::value_type(uValueType, sImportTag)).second);
}

CValueTypeFactory::CValueTypeFactory()
{
	// Pressure
	AppendValueTypeBuilder(IDT_VALUETYPE_PRESSURE,
						   IDS_VALUENAME_PRESSURE,
						   IDS_ET_PRESSURE,
						   new CValueTypeBuilderTemp<TPressure>());

	// Fracture Matrix Pressure
	AppendValueTypeBuilder(IDT_VALUETYPE_FRACTURE_MATRIX_PRESSURE,
						   IDS_VALUENAME_FRACTURE_MATRIX_PRESSURE,
						   IDS_ET_FRACTURE_MATRIX_PRESSURE,
						   new CValueTypeBuilderTemp<TFractureMatrixPressure>());

	// Temperature
	AppendValueTypeBuilder(IDT_VALUETYPE_TEMPERATURE,
						   IDS_VALUENAME_TEMPERATURE,
						   IDS_ET_TEMPERATURE,
						   new CValueTypeBuilderTemp<TTemperature>());

  // Correction strain
  AppendValueTypeBuilder(IDT_VALUETYPE_VOLUMETRICSTRAIN,
                IDS_VALUENAME_VOLUMETRICSTRAIN,
                IDS_ET_VOLUMETRICSTRAIN,
                new CValueTypeBuilderTemp<TVolumetricStrain>());

  // Normal strain
  AppendValueTypeBuilder(IDT_VALUETYPE_NRMSTRAIN,
                IDS_VALUENAME_NRMSTRAIN,
                IDS_ET_NRMSTRAIN,
                new CValueTypeBuilderTemp<TNormalStrain>());

  // Lateral strain
  AppendValueTypeBuilder(IDT_VALUETYPE_LATSTRAIN,
                IDS_VALUENAME_LATSTRAIN,
                IDS_ET_LATSTRAIN,
                new CValueTypeBuilderTemp<TLateralStrain>());

  // Strain
	AppendValueTypeBuilder(IDT_VALUETYPE_STRAIN_TENSOR,
						   IDS_VALUENAME_STRAIN_TENSOR,
						   IDS_ET_STRAIN,
						   new CValueTypeBuilderTemp<TStrainTensor>());

  // Stress
	AppendValueTypeBuilder(IDT_VALUETYPE_STRESS_TENSOR,
						   IDS_VALUENAME_STRESS_TENSOR,
						   IDS_ET_STRESS,
						   new CValueTypeBuilderTemp<TStressTensor>());

/*
  // Change in effective stress
	AppendValueTypeBuilder(IDT_VALUETYPE_CHANGE_EFF_STRESS_TENSOR,
						   IDS_VALUENAME_CHANGE_EFF_STRESS,
						   IDS_ET_CHANGE_EFF_STRESS,
						   new CValueTypeBuilderTemp<TChangeEffectiveStressTensor>());
*/
  //Fracture Intensity
  AppendValueTypeBuilder(IDT_VALUETYPE_FRACT_TENSOR,
						   IDS_VALUENAME_FRACT_TENSOR,
						   IDS_ET_FRACT_TENSOR,
						   new CValueTypeBuilderTemp<CFractTensor>());

	// Displacement
	AppendValueTypeBuilder(IDT_VALUETYPE_DISPLACEMENT,
						   IDS_VALUENAME_DISPLACEMENT,
						   IDS_ET_DISPLACEMENT,
						   new CValueTypeBuilderTemp<TDisplacementVector>());

	// Cohesion
	AppendValueTypeBuilder(IDT_VALUETYPE_COHESION,
						   IDS_VALUENAME_COHESION,
						   IDS_ET_COHESION,
						   new CValueTypeBuilderTemp<TCohesion>());

	// Bulk Stiffness
	AppendValueTypeBuilder(IDT_VALUETYPE_BULKSTIFFNESS,
						   IDS_VALUENAME_BULKSTIFFNESS,
						   IDS_ET_BULKSTIFFNESS,
						   new CValueTypeBuilderTemp<TBulkStiffness>());

	// Shear Modulus
	AppendValueTypeBuilder(IDT_VALUETYPE_SHEARMODULUS,
						   IDS_VALUENAME_SHEARMODULUS,
						   IDS_ET_SHEARMODULUS,
						   new CValueTypeBuilderTemp<TShearModulus>());

  // Vp
  AppendValueTypeBuilder(IDT_VALUETYPE_VELOCITYP,
               IDS_VALUENAME_VELOCITYP,
               IDS_ET_VELOCITYP,
               new CValueTypeBuilderTemp<TVelocityP>());

  // Vs
  AppendValueTypeBuilder(IDT_VALUETYPE_VELOCITYS,
               IDS_VALUENAME_VELOCITYS,
               IDS_ET_VELOCITYS,
               new CValueTypeBuilderTemp<TVelocityS>());

	// Friction angle
	AppendValueTypeBuilder(IDT_VALUETYPE_FRICTION_ANGLE,
						   IDS_VALUENAME_FRICTION_ANGLE,
						   IDS_ET_FRICTION_ANGLE, 
						   new CValueTypeBuilderTemp<TFrictionAngle>());
	
	// Poissons Ratio
	AppendValueTypeBuilder(IDT_VALUETYPE_POISSONS_RATIO, 
						   IDS_VALUENAME_POISSONS_RATIO,
						   IDS_ET_POISSON_RATIO,
						   new CValueTypeBuilderTemp<TPoissonsRatio>());

	// Youngs modulus Ratio
	AppendValueTypeBuilder(IDT_VALUETYPE_YOUNGS_MODULUS,
						   IDS_VALUENAME_YOUNGS_MODULUS,
						   IDS_ET_YOUNG_MODULUS,
						   new CValueTypeBuilderTemp<TYoungsModulus>());

	// Density
	AppendValueTypeBuilder(IDT_VALUETYPE_RHOB,
						   IDS_VALUENAME_RHOB,
						   IDS_ET_RHOB,
						   new CValueTypeBuilderTemp<TDensity>());

	// Porosity
	AppendValueTypeBuilder(IDT_VALUETYPE_POROSITY,
						   IDS_VALUENAME_POROSITY,
						   IDS_ET_POROSITY,
						   new CValueTypeBuilderTemp<TPorosity>());

	// Intial friction
	AppendValueTypeBuilder(IDT_VALUETYPE_INITFRICTION,
						   IDS_VALUENAME_INITFRICTION,
						   IDS_ET_INITFRICTION,
						   new CValueTypeBuilderTemp<TInitFriction>());

	// Hardening
	AppendValueTypeBuilder(IDT_VALUETYPE_HARDENING,
						   IDS_VALUENAME_HARDENING,
						   IDS_ET_HARDENING,
						   new CValueTypeBuilderTemp<THardening>());

	// Preconsolidation
	AppendValueTypeBuilder(IDT_VALUETYPE_PRECONSOLIDATION,
						   IDS_VALUENAME_PRECONSOLIDATION,
						   IDS_ET_PRECONSOLIDATION,
						   new CValueTypeBuilderTemp<TPreConsolidation>());

	// CapShape
	AppendValueTypeBuilder(IDT_VALUETYPE_CAPSHAPE,
						   IDS_VALUENAME_CAPSHAPE,
						   IDS_ET_CAPSHAPE,
						   new CValueTypeBuilderTemp<TCapShape>());

	// Dilatation
	AppendValueTypeBuilder(IDT_VALUETYPE_DILATATION,
						   IDS_VALUENAME_DILATATION,
						   IDS_ET_DILATATION,
						   new CValueTypeBuilderTemp<TDilatation>());

	// Latratio min/max
	AppendValueTypeBuilder(IDT_VALUETYPE_LATRATIO_MIN,
						   IDS_VALUENAME_LATRATIO_MIN,
						   IDS_ET_LATRATIO_MIN,
						   new CValueTypeBuilderTemp<TLatRatioMin>());

	AppendValueTypeBuilder(IDT_VALUETYPE_LATRATIO_MAX,
						   IDS_VALUENAME_LATRATIO_MAX,
						   IDS_ET_LATRATIO_MAX,
							new CValueTypeBuilderTemp<TLatRatioMax>());

	// Azimuth
	AppendValueTypeBuilder(IDT_VALUETYPE_AZIMUTH,
						   IDS_VALUENAME_AZIMUTH,
						   IDS_ET_AZIMUTH,
						   new CValueTypeBuilderTemp<TAzimuth>());
	// A1
	AppendValueTypeBuilder(IDT_VALUETYPE_CREEP_A1,
						   IDS_VALUENAME_CREEP_A1,
						   IDS_ET_CREEP_A1,
						   new CValueTypeBuilderTemp<TCreepA1>());
	// N1
	AppendValueTypeBuilder(IDT_VALUETYPE_CREEP_N1,
						   IDS_VALUENAME_CREEP_N1,
						   IDS_ET_CREEP_N1,
						   new CValueTypeBuilderTemp<TCreepN1>());
	// A2
	AppendValueTypeBuilder(IDT_VALUETYPE_CREEP_A2,
						   IDS_VALUENAME_CREEP_A2,
						   IDS_ET_CREEP_A2,
						   new CValueTypeBuilderTemp<TCreepA2>());
	// N2
	AppendValueTypeBuilder(IDT_VALUETYPE_CREEP_N2,
						   IDS_VALUENAME_CREEP_N2,
						   IDS_ET_CREEP_N2,
						   new CValueTypeBuilderTemp<TCreepN2>());

	// QR1
	AppendValueTypeBuilder(IDT_VALUETYPE_CREEP_QR1,
						   IDS_VALUENAME_CREEP_QR1,
						   IDS_ET_CREEP_QR1,
						   new CValueTypeBuilderTemp<TCreepQR1>());
	// QR1
	AppendValueTypeBuilder(IDT_VALUETYPE_CREEP_QR2,
						   IDS_VALUENAME_CREEP_QR2,
						   IDS_ET_CREEP_QR2,
						   new CValueTypeBuilderTemp<TCreepQR2>());

  // Alpha
  AppendValueTypeBuilder(IDT_VALUETYPE_CREEP_ALPHA,
                IDS_VALUENAME_CREEP_ALPHA,
                IDS_ET_CREEP_ALPHA,
                new CValueTypeBuilderTemp<TCreepAlpha>());

	// Tensile stretch
	AppendValueTypeBuilder(IDT_VALUETYPE_TENSILE_STRETCH,
						   IDS_VALUENAME_TENSILE_STRETCH,
						   IDS_ET_TENSILE_STRETCH,
						   new CValueTypeBuilderTemp<TTensileStretch>());

	// Volumetric thermal expansion coefficient
	AppendValueTypeBuilder(IDT_VALUETYPE_THERMAL_EXPANSION,
						   IDS_VALUENAME_THERMAL_EXPANSION,
						   IDS_ET_THERMAL_EXPANSION,
						   new CValueTypeBuilderTemp<TThermalExpansion>());

	// Surfaces
	AppendValueTypeBuilder(IDT_VALUETYPE_SURFACE,
						   IDS_VALUENAME_SURFACE,
						   IDS_ET_SURFACE,
						   new CValueTypeBuilderTemp<CSurfaceValueType>());	
	// Bulk fluid modulus
	AppendValueTypeBuilder(IDT_VALUETYPE_FLUID_BULK_MOD,
						   IDS_VALUENAME_FLUID_BULK_MOD,
						   IDS_ET_VALUETYPE_FLUID_SHEAR_MOD,
						   new CValueTypeBuilderTemp<TFluidBulkModulus>());
						  
// Cohesion hardening
	AppendValueTypeBuilder(IDT_VALUETYPE_HARD_COHESION1,
						  IDS_VALUENAME_HARD_COHESION1,
						  IDS_ET_HARD_COHESION1, 
						  new CValueTypeBuilderTemp<THardCohesion1>());

	AppendValueTypeBuilder(IDT_VALUETYPE_HARD_COHESION2,
						  IDS_VALUENAME_HARD_COHESION2,
						  IDS_ET_HARD_COHESION2, 
						  new CValueTypeBuilderTemp<THardCohesion2>());

	AppendValueTypeBuilder(IDT_VALUETYPE_HARD_COHESION3,
						  IDS_VALUENAME_HARD_COHESION3,
						  IDS_ET_HARD_COHESION3,
						  new CValueTypeBuilderTemp<THardCohesion3>());

// Friction Angle hardening
	AppendValueTypeBuilder(IDT_VALUETYPE_HARD_FRICTION1,
						 IDS_VALUENAME_HARD_FRICTION1,
						 IDS_ET_HARD_FRICTION1, 
						 new CValueTypeBuilderTemp<THardFriction1>());

	AppendValueTypeBuilder(IDT_VALUETYPE_HARD_FRICTION2,
						 IDS_VALUENAME_HARD_FRICTION2,
						 IDS_ET_HARD_FRICTION2,
						 new CValueTypeBuilderTemp<THardFriction2>());

	AppendValueTypeBuilder(IDT_VALUETYPE_HARD_FRICTION3,
						 IDS_VALUENAME_HARD_FRICTION3,
						 IDS_ET_HARD_FRICTION3,
						 new CValueTypeBuilderTemp<THardFriction3>());

// waij TFS 82017
// Equivalent Plastic Strain
	AppendValueTypeBuilder(IDT_VALUETYPE_EQUIV_PLAST_STRAIN1,
						 IDS_VALUENAME_EQUIV_PLAST_STRAIN1,
						 IDS_ET_EQUIV_PLAST_STRAIN1,
						 new CValueTypeBuilderTemp<TEquivalentPlasticStrain1>());

	AppendValueTypeBuilder(IDT_VALUETYPE_EQUIV_PLAST_STRAIN2,
						 IDS_VALUENAME_EQUIV_PLAST_STRAIN2,
						 IDS_ET_EQUIV_PLAST_STRAIN2,
						 new CValueTypeBuilderTemp<TEquivalentPlasticStrain2>());

	AppendValueTypeBuilder(IDT_VALUETYPE_EQUIV_PLAST_STRAIN3,
						 IDS_VALUENAME_EQUIV_PLAST_STRAIN3,
						 IDS_ET_EQUIV_PLAST_STRAIN3,
						 new CValueTypeBuilderTemp<TEquivalentPlasticStrain3>());

// Homogenization rigidty parameters
	AppendValueTypeBuilder(IDT_VALUETYPE_RIGI_PARAM1,
						 IDS_RIGI_PARAM1,
						 IDS_RIGI_PARAM1,
						 new CValueTypeBuilderTemp<TRigidParam1>());

	AppendValueTypeBuilder(IDT_VALUETYPE_RIGI_PARAM2,
						 IDS_RIGI_PARAM2,
						 IDS_RIGI_PARAM2,
						 new CValueTypeBuilderTemp<TRigidParam2>());

	AppendValueTypeBuilder(IDT_VALUETYPE_RIGI_PARAM3,
						 IDS_RIGI_PARAM3,
						 IDS_RIGI_PARAM3,
						 new CValueTypeBuilderTemp<TRigidParam3>());

	AppendValueTypeBuilder(IDT_VALUETYPE_RIGI_PARAM4,
						 IDS_RIGI_PARAM4,
						 IDS_RIGI_PARAM4,
						 new CValueTypeBuilderTemp<TRigidParam4>());

	AppendValueTypeBuilder(IDT_VALUETYPE_RIGI_PARAM5,
						 IDS_RIGI_PARAM5,
						 IDS_RIGI_PARAM5,
						 new CValueTypeBuilderTemp<TRigidParam5>());

	AppendValueTypeBuilder(IDT_VALUETYPE_RIGI_PARAM6,
						 IDS_RIGI_PARAM6,
						 IDS_RIGI_PARAM6,
						 new CValueTypeBuilderTemp<TRigidParam6>());

	AppendValueTypeBuilder(IDT_VALUETYPE_RIGI_PARAM7,
						 IDS_RIGI_PARAM7,
						 IDS_RIGI_PARAM7,
						 new CValueTypeBuilderTemp<TRigidParam7>());

	AppendValueTypeBuilder(IDT_VALUETYPE_RIGI_PARAM8,
						 IDS_RIGI_PARAM8,
						 IDS_RIGI_PARAM8,
						 new CValueTypeBuilderTemp<TRigidParam8>());

	AppendValueTypeBuilder(IDT_VALUETYPE_RIGI_PARAM9,
						 IDS_RIGI_PARAM9,
						 IDS_RIGI_PARAM9,
						 new CValueTypeBuilderTemp<TRigidParam9>());

	AppendValueTypeBuilder(IDT_VALUETYPE_RIGI_PARAM10,
						 IDS_RIGI_PARAM10,
						 IDS_RIGI_PARAM10,
						 new CValueTypeBuilderTemp<TRigidParam10>());

	AppendValueTypeBuilder(IDT_VALUETYPE_RIGI_PARAM11,
						 IDS_RIGI_PARAM11,
						 IDS_RIGI_PARAM11,
						 new CValueTypeBuilderTemp<TRigidParam11>());

	AppendValueTypeBuilder(IDT_VALUETYPE_RIGI_PARAM12,
						 IDS_RIGI_PARAM12,
						 IDS_RIGI_PARAM12,
						 new CValueTypeBuilderTemp<TRigidParam12>());

	AppendValueTypeBuilder(IDT_VALUETYPE_RIGI_PARAM13,
						 IDS_RIGI_PARAM13,
						 IDS_RIGI_PARAM13,
						 new CValueTypeBuilderTemp<TRigidParam13>());

	AppendValueTypeBuilder(IDT_VALUETYPE_RIGI_PARAM14,
						 IDS_RIGI_PARAM14,
						 IDS_RIGI_PARAM14,
						 new CValueTypeBuilderTemp<TRigidParam14>());

	AppendValueTypeBuilder(IDT_VALUETYPE_RIGI_PARAM15,
						 IDS_RIGI_PARAM15,
						 IDS_RIGI_PARAM15,
						 new CValueTypeBuilderTemp<TRigidParam15>());

	AppendValueTypeBuilder(IDT_VALUETYPE_RIGI_PARAM16,
						 IDS_RIGI_PARAM16,
						 IDS_RIGI_PARAM16,
						 new CValueTypeBuilderTemp<TRigidParam16>());

	AppendValueTypeBuilder(IDT_VALUETYPE_RIGI_PARAM17,
						 IDS_RIGI_PARAM17,
						 IDS_RIGI_PARAM17,
						 new CValueTypeBuilderTemp<TRigidParam17>());

	AppendValueTypeBuilder(IDT_VALUETYPE_RIGI_PARAM18,
						 IDS_RIGI_PARAM18,
						 IDS_RIGI_PARAM18,
						 new CValueTypeBuilderTemp<TRigidParam18>());

	AppendValueTypeBuilder(IDT_VALUETYPE_RIGI_PARAM19,
						 IDS_RIGI_PARAM19,
						 IDS_RIGI_PARAM19,
						 new CValueTypeBuilderTemp<TRigidParam19>());

	AppendValueTypeBuilder(IDT_VALUETYPE_RIGI_PARAM20,
						 IDS_RIGI_PARAM20,
						 IDS_RIGI_PARAM20,
						 new CValueTypeBuilderTemp<TRigidParam20>());

	AppendValueTypeBuilder(IDT_VALUETYPE_RIGI_PARAM21,
						 IDS_RIGI_PARAM21,
						 IDS_RIGI_PARAM21,
						 new CValueTypeBuilderTemp<TRigidParam21>());
		
  AppendValueTypeBuilder(IDT_VALUETYPE_ELASTIC_HARDENING,
             IDS_VALUENAME_ELASTIC_HARDENING,
             IDS_ET_ELASTIC_HARDENING,
             new CValueTypeBuilderTemp<TElasticHardening>());

  AppendValueTypeBuilder(IDT_VALUETYPE_SEC_PRECON,
             IDS_VALUENAME_SEC_PRECON,
             IDS_ET_SEC_PRECON,
             new CValueTypeBuilderTemp<TSecondaryPreconsolidation>());

  AppendValueTypeBuilder(IDT_VALUETYPE_SEC_HARDENING,
             IDS_VALUENAME_SEC_HARDENING,
             IDS_ET_SEC_HARDENING,
             new CValueTypeBuilderTemp<TSecondaryHardening>());

  AppendValueTypeBuilder(IDT_VALUETYPE_ELASCOMPRES,
              IDS_VALUENAME_ELASCOMPRES,
              IDS_ET_ELASCOMPRES,
              new CValueTypeBuilderTemp<TUniaxialElasticCompressibility>());

  AppendValueTypeBuilder(IDT_VALUETYPE_PLASCOMPRES,
              IDS_VALUENAME_PLASCOMPRES,
              IDS_ET_PLASCOMPRES,
              new CValueTypeBuilderTemp<TUniaxialPlasticCompressibility>());

  AppendValueTypeBuilder(IDT_VALUETYPE_YOUNGMODULUS_NORM,
              IDS_VALUENAME_YOUNGMODULUS_NORM,
              IDS_ET_YOUNGMODULUS_NORM,
              new CValueTypeBuilderTemp<TYoungModulusNormal>());

  AppendValueTypeBuilder(IDT_VALUETYPE_YOUNGMODULUS_TRANS,
              IDS_VALUENAME_YOUNGMODULUS_TRANS,
              IDS_ET_YOUNGMODULUS_TRANS,
              new CValueTypeBuilderTemp<TYoungModulusTransverse>());

  AppendValueTypeBuilder(IDT_VALUETYPE_POISSONRATIO_NORM,
              IDS_VALUENAME_POISSONRATIO_NORM,
              IDS_ET_POISSONRATIO_NORM,
              new CValueTypeBuilderTemp<TPoissonRatioNormal>());

  AppendValueTypeBuilder(IDT_VALUETYPE_POISSONRATIO_TRANS,
              IDS_VALUENAME_POISSONRATIO_TRANS,
              IDS_ET_POISSONRATIO_TRANS,
              new CValueTypeBuilderTemp<TPoissonRatioTransverse>());

  AppendValueTypeBuilder(IDT_VALUETYPE_LAYER_NORMAL_INCL,
              IDS_VALUENAME_LAYER_NORMAL_INCL,
              IDS_ET_LAYER_NORMAL_INCL,
              new CValueTypeBuilderTemp<TLayerNormalIncl>());
  
  AppendValueTypeBuilder(IDT_VALUETYPE_LAYER_NORMAL_AZI,
              IDS_VALUENAME_LAYER_NORMAL_AZI,
              IDS_ET_LAYER_NORMAL_AZI,
              new CValueTypeBuilderTemp<TLayerNormalAzi>());

  AppendValueTypeBuilder(IDT_VALUETYPE_FRACT_STIFF_NORMAL,
              IDS_VALUENAME_FRACT_STIFF_NORMAL,
              IDS_ET_FRACT_STIFF_NORMAL,
              new CValueTypeBuilderTemp<TFractStiffNormal>());

  AppendValueTypeBuilder(IDT_VALUETYPE_FRACT_STIFF_SHEAR,
              IDS_VALUENAME_FRACT_STIFF_SHEAR,
              IDS_ET_FRACT_STIFF_SHEAR,
              new CValueTypeBuilderTemp<TFractStiffShear>());

  AppendValueTypeBuilder(IDT_VALUETYPE_THERM_LIN_EXP_NORM,
              IDS_VALUENAME_THERM_LIN_EXP_NORM,
              IDS_ET_THERM_LIN_EXP_NORM,
              new CValueTypeBuilderTemp<TThermLinExpNormal>());

  AppendValueTypeBuilder(IDT_VALUETYPE_THERM_LIN_EXP_LAT,
              IDS_VALUENAME_THERM_LIN_EXP_LAT,
              IDS_ET_THERM_LIN_EXP_LAT,
              new CValueTypeBuilderTemp<TThermLinExpLateral>());

  AppendValueTypeBuilder(IDT_VALUETYPE_THERM_CONDUCT,
              IDS_VALUENAME_THERM_CONDUCT,
              IDS_ET_THERM_CONDUCT,
              new CValueTypeBuilderTemp<TThermalConductivity>());

  AppendValueTypeBuilder(IDT_VALUETYPE_THERM_CAPACI,
              IDS_VALUENAME_THERM_CAPACI,
              IDS_ET_THERM_CAPACI,
              new CValueTypeBuilderTemp<TThermalCapacity>());

  AppendValueTypeBuilder(IDT_VALUETYPE_PERMEA,
              IDS_VALUENAME_PERMEA,
              IDS_ET_PERMEA,
              new CValueTypeBuilderTemp<TPermeability>());

  AppendValueTypeBuilder(IDT_VALUETYPE_YOUNG_DECOMP,
              IDS_VALUENAME_YOUNG_DECOMP,
              IDS_ET_YOUNG_DECOMP,
              new CValueTypeBuilderTemp<TYoungsModulusDecompaction>());

  AppendValueTypeBuilder(IDT_VALUETYPE_POISSON_DECOMP,
              IDS_VALUENAME_POISSON_DECOMP,
              IDS_ET_POISSON_DECOMP,
              new CValueTypeBuilderTemp<TPoissonsRatioDecompaction>());

  AppendValueTypeBuilder(IDT_VALUETYPE_FLUIDX,
              IDS_VALUENAME_FLUIDX,
              IDS_ET_VALUETYPE_FLUIDX,
              new CValueTypeBuilderTemp<TFluidThermalExpansionCoefficient>());

  AppendValueTypeBuilder(IDT_VALUETYPE_FLUID_DENSITY,
              IDS_VALUENAME_FLUID_DENSITY,
              IDS_ET_VALUETYPE_FLUID_DENSITY,
              new CValueTypeBuilderTemp<TFluidDensity>());

  AppendValueTypeBuilder(IDT_VALUETYPE_VISCOSITY,
              IDS_VALUENAME_VISCOSITY,
              IDS_ET_VISCOSITY,
              new CValueTypeBuilderTemp<TViscosity>());

  AppendValueTypeBuilder(IDT_VALUETYPE_REFERENCESTRESS,
              IDS_VALUENAME_REFERENCESTRESS,
              IDS_ET_VALUETYPE_REFERENCESTRESS,
              new CValueTypeBuilderTemp<TReferenceStress>());

  AppendValueTypeBuilder(IDT_VALUETYPE_APERTUREPARAM,
              IDS_VALUENAME_APERTUREPARAM,
              IDS_ET_VALUETYPE_APERTUREPARAM,
              new CValueTypeBuilderTemp<TApertureParameter>());

  AppendValueTypeBuilder(IDT_VALUETYPE_GRAINSTIFFNESS, // wjrx mantis 3314
              IDS_VALUENAME_GRAINSTIFFNESS,
              IDS_ET_VALUETYPE_GRAINSTIFFNESS,
              new CValueTypeBuilderTemp<TGrainStiffnessParameter>());

  AppendValueTypeBuilder(IDT_VALUETYPE_YOUNGMODULUS_NORM_DECOMP,
              IDS_VALUENAME_YOUNGMODULUS_NORM_DECOMP,
              IDS_ET_VALUETYPE_YOUNGMODULUS_NORM_DECOMP,
              new CValueTypeBuilderTemp<TYoungsModulusNormalDecomp>());

  AppendValueTypeBuilder(IDT_VALUETYPE_YOUNGMODULUS_TRANS_DECOMP,
              IDS_VALUENAME_YOUNGMODULUS_TRANS_DECOMP,
              IDS_ET_VALUETYPE_YOUNGMODULUS_TRANS_DECOMP,
              new CValueTypeBuilderTemp<TYoungModulusTransDecomp>());

  AppendValueTypeBuilder(IDT_VALUETYPE_POISSONRATIO_NORM_DECOMP,
              IDS_VALUENAME_POISSONRATIO_NORM_DECOMP,
              IDS_ET_VALUETYPE_POISSONRATIO_NORM_DECOMP,
              new CValueTypeBuilderTemp<TPoissonRatioNormalDecomp>());

  AppendValueTypeBuilder(IDT_VALUETYPE_POISSONRATIO_TRANS_DECOMP,
              IDS_VALUENAME_POISSONRATIO_TRANS_DECOMP,
              IDS_ET_VALUETYPE_POISSONRATIO_TRANS_DECOMP,
              new CValueTypeBuilderTemp<TPoissonRatioTransDecomp>());

  AppendValueTypeBuilder(IDT_VALUETYPE_ANISOTROPIC_SHEARMODULUS,
              IDS_VALUENAME_ANISOTROPIC_SHEARMODULUS,
              IDS_ET_VALUETYPE_ANISOTROPIC_SHEARMODULUS,
              new CValueTypeBuilderTemp<TAnisotropicShearModulus>());

  AppendValueTypeBuilder(IDT_VALUETYPE_ANISOTROPIC_SHEARMODULUS_DECOMP,
              IDS_VALUENAME_ANISOTROPIC_SHEARMODULUS_DECOMP,
              IDS_ET_VALUETYPE_ANISOTROPIC_SHEARMODULUS_DECOMP,
              new CValueTypeBuilderTemp<TAnisotropicShearModulusDecomp>());

  AppendValueTypeBuilder(IDT_VALUETYPE_THOMSEN_EPSILON,
              IDS_VALUENAME_THOMSEN_EPSILON,
              IDS_ET_VALUETYPE_THOMSEN_EPSILON,
              new CValueTypeBuilderTemp<TThomsenEpsilon>());

  AppendValueTypeBuilder(IDT_VALUETYPE_THOMSEN_GAMMA,
              IDS_VALUENAME_THOMSEN_GAMMA,
              IDS_ET_VALUETYPE_THOMSEN_GAMMA,
              new CValueTypeBuilderTemp<TThomsenGamma>());

  AppendValueTypeBuilder(IDT_VALUETYPE_THOMSEN_DELTA,
              IDS_VALUENAME_THOMSEN_DELTA,
              IDS_ET_VALUETYPE_THOMSEN_DELTA,
              new CValueTypeBuilderTemp<TThomsenDelta>());

  // fracture "tensor" components
  AppendValueTypeBuilder(IDT_VALUETYPE_HIGH_FRACT_DENS,
              IDS_VALUENAME_HIGH_FRACT_DENS,
              IDS_ET_HIGH_FRACT_DENS,
              new CValueTypeBuilderTemp<THighFractDens>());

  AppendValueTypeBuilder(IDT_VALUETYPE_HIGH_FRACT_DENS_INCL,
              IDS_VALUENAME_HIGH_FRACT_DENS_INCL,
              IDS_ET_HIGH_FRACT_DENS_INCL,
              new CValueTypeBuilderTemp<THighFractDensIncl>());

  AppendValueTypeBuilder(IDT_VALUETYPE_HIGH_FRACT_DENS_AZI,
              IDS_VALUENAME_HIGH_FRACT_DENS_AZI,
              IDS_ET_HIGH_FRACT_DENS_AZI,
              new CValueTypeBuilderTemp<THighFractDensAzi>());

  AppendValueTypeBuilder(IDT_VALUETYPE_LOW_FRACT_DENS,
              IDS_VALUENAME_LOW_FRACT_DENS,
              IDS_ET_LOW_FRACT_DENS,
              new CValueTypeBuilderTemp<TLowFractDens>());

  AppendValueTypeBuilder(IDT_VALUETYPE_LOW_FRACT_DENS_INCL,
              IDS_VALUENAME_LOW_FRACT_DENS_INCL,
              IDS_ET_LOW_FRACT_DENS_INCL,
              new CValueTypeBuilderTemp<TLowFractDensIncl>());

  AppendValueTypeBuilder(IDT_VALUETYPE_LOW_FRACT_DENS_AZI,
              IDS_VALUENAME_LOW_FRACT_DENS_AZI,
              IDS_ET_LOW_FRACT_DENS_AZI,
              new CValueTypeBuilderTemp<TLowFractDensAzi>());

  AppendValueTypeBuilder(IDT_VALUETYPE_INTER_FRACT_DENS,
              IDS_VALUENAME_INTER_FRACT_DENS,
              IDS_ET_INTER_FRACT_DENS,
              new CValueTypeBuilderTemp<TInterFractDens>());

  AppendValueTypeBuilder(IDT_VALUETYPE_V0_HI,
              IDS_VALUENAME_V0_HI,
              IDS_ET_VALUETYPE_V0_HI,
              new CValueTypeBuilderTemp<TInitialFractureApertureHigh>());

  AppendValueTypeBuilder(IDT_VALUETYPE_V0_ME,
              IDS_VALUENAME_V0_ME,
              IDS_ET_VALUETYPE_V0_ME,
              new CValueTypeBuilderTemp<TInitialFractureApertureInter>());

  AppendValueTypeBuilder(IDT_VALUETYPE_V0_LO,
              IDS_VALUENAME_V0_LO,
              IDS_ET_VALUETYPE_V0_LO,
              new CValueTypeBuilderTemp<TInitialFractureApertureLow>());

  AppendValueTypeBuilder(IDT_VALUETYPE_REFERENCE_APERTURE,
              IDS_VALUENAME_REFERENCE_APERTURE,
              IDS_ET_VALUETYPE_REFERENCE_APERTURE,
              new CValueTypeBuilderTemp<TReferenceAperture>());

  AppendValueTypeBuilder(IDT_VALUETYPE_DYNUNISTIFFNESS,
              IDS_VALUENAME_DYNUNISTIFFNESS,
              IDS_ET_DYNUNISTIFFNESS,
              new CValueTypeBuilderTemp<TDynamicUniaxialStiffness>());

  AppendValueTypeBuilder(IDT_VALUETYPE_DYNSHEARSTIFFNESS,
              IDS_VALUENAME_DYNSHEARSTIFFNESS,
              IDS_ET_DYNSHEARSTIFFNESS,
              new CValueTypeBuilderTemp<TDynamicShearStiffness>());

  AppendValueTypeBuilder(IDT_VALUETYPE_DUMMY,
              IDS_VALUENAME_DUMMY,
              IDS_ET_DUMMY,
              new CValueTypeBuilderTemp<TDummyMaterialParameter>());

  AppendValueTypeBuilder(IDT_VALUETYPE_NRMSTRESS,
    IDS_VALUENAME_NRMSTRESS,
    IDS_ET_NRMSTRESS,
    new CValueTypeBuilderTemp<TNormalStress>());

  AppendValueTypeBuilder(IDT_VALUETYPE_MEANSTRESS,
    IDS_VALUENAME_MEANSTRESS,
    IDS_ET_MEANSTRESS,
    new CValueTypeBuilderTemp<TMeanStress>());

  AppendValueTypeBuilder(IDT_VALUETYPE_TIMESTRAIN,
    IDS_VALUENAME_TIMESTRAIN,
    IDS_ET_TIMESTRAIN,
    new CValueTypeBuilderTemp<TTimeStrain>());

  AppendValueTypeBuilder(IDT_VALUETYPE_DELTAV,
    IDS_VALUENAME_DELTAV,
    IDS_ET_DELTAV,
    new CValueTypeBuilderTemp<TDeltaV>());

  AppendValueTypeBuilder(IDT_VALUETYPE_DELTAT,
    IDS_VALUENAME_DELTAT,
    IDS_ET_DELTAT,
    new CValueTypeBuilderTemp<TDeltaT>());

  AppendValueTypeBuilder(IDT_VALUETYPE_DISPLACEMENTZ,
    IDS_VALUENAME_DISPLACEMENTZ,
    IDS_ET_DISPLACEMENTZ,
    new CValueTypeBuilderTemp<TDisplacementZ>());

  AppendValueTypeBuilder(IDT_VALUETYPE_KRAD,
    IDS_VALUENAME_KRAD,
    IDS_ET_KRAD,
    new CValueTypeBuilderTemp<TBoundaryKradMaterialParameter>());

  AppendValueTypeBuilder(IDT_VALUETYPE_KTAN,
    IDS_VALUENAME_KTAN,
    IDS_ET_KTAN,
    new CValueTypeBuilderTemp<TBoundaryKtanMaterialParameter>());
}

CValueTypeFactory::TValueTypeVec CValueTypeFactory::ValueTypes() const
{
	TValueTypeVec vcRet;
	for(TBuilderMap::const_iterator it = m_mpBuilder.begin(); it != m_mpBuilder.end(); it++)
		vcRet.push_back(TValueTypeIdNamePair(it->first, it->second.second));
	return vcRet;
}

const CValueTypeFactory *CValueTypeFactory::instance()
{
  static CValueTypeFactory *_instance = new CValueTypeFactory;
  return _instance;
}

void CValueTypeFactory::reset()
{
  // We should only remove user defined types, but we don't have any yet, and hence we don't have to do anything (auto-cleanup @ exit)
  //for (TBuilderMap::const_iterator it = instance()->m_mpBuilder.begin(); it != instance()->m_mpBuilder.end(); it++)
	//  delete it->second.first;
}

// Function to build a quantity
bool CValueTypeFactory::ValueTypeAvailable(unsigned int uValueType) const
{
	// Yes, a value type for pointset is available
	if(uValueType == IDT_VALUETYPE_COORDINATE)
		return true;

	TBuilderMap::const_iterator it = m_mpBuilder.find(uValueType);
	
	return it != m_mpBuilder.end();
}

CValueType* CValueTypeFactory::BuildValueType(IPointSet &point_set, const QString &sFileTag, unsigned int uName) const
{
	TImportMap::const_iterator it = m_mpImport.find(sFileTag);
	if(it == m_mpImport.end())
		return 0;

	return BuildValueType(point_set, it->second, uName);
}

CValueType* CValueTypeFactory::BuildValueType(IPointSet &point_set, const QString &sFileTag, const QString &sName) const
{
	TImportMap::const_iterator it = m_mpImport.find(sFileTag);
	if(it == m_mpImport.end())
		return 0;

	return BuildValueType(point_set, it->second, sName);
}

CValueType* CValueTypeFactory::BuildValueType(IPointSet &point_set, unsigned int uValueType, unsigned int uName) const
{
	if(uValueType == IDT_VALUETYPE_COORDINATE)
	{
		CPointSet& ps = dynamic_cast<CPointSet&>(point_set);
		return &ps.Coordinates();
	}

	assert(ValueTypeAvailable(uValueType));
	TBuilderMap::const_iterator it = m_mpBuilder.find(uValueType);
	
	if(it == m_mpBuilder.end())
		return 0;

	// Execut builder
	CValueType *pValueType = it->second.first->Build(point_set, uName);

	pValueType->AssertValid();

	return pValueType;
}

CValueType* CValueTypeFactory::BuildValueType(IPointSet &point_set, unsigned int uValueType, const QString &sName) const
{
	if(uValueType == IDT_VALUETYPE_COORDINATE)
	{
		CPointSet& ps = dynamic_cast<CPointSet&>(point_set);
		return &ps.Coordinates();
	}

	assert(ValueTypeAvailable(uValueType));
	TBuilderMap::const_iterator it = m_mpBuilder.find(uValueType);
	
	if(it == m_mpBuilder.end())
		return 0;

	// Create value type
	CValueType *pValueType = it->second.first->Build(point_set, sName);

	pValueType->AssertValid();

	return pValueType;
}

unsigned int CValueTypeFactory::ValueTypeIconID(unsigned int uValueType) const
{
  assert(ValueTypeAvailable(uValueType));
  TBuilderMap::const_iterator it = m_mpBuilder.find(uValueType);
  // waij: if the icon is not available (which is a programming error),
  // return icon IDI_EMPTY. Failing to return a valid icon gives
  // problems when displaying them in a CListCtrl.
  if(it == m_mpBuilder.end())
    return IDI_EMPTY;

  return it->second.first->IconID();
}

int CValueTypeFactory::NameIndex(unsigned int uValueType) const
{
	TBuilderMap::const_iterator it = m_mpBuilder.find(uValueType);
	
	if(it == m_mpBuilder.end())
		return -1;
	return it->second.second;
}

QString CValueTypeFactory::getImportTag(unsigned int valueType) const
{
  TValueType2ImportTag::const_iterator importTag =
    m_mpValueType2ImportTag.find(valueType);

  if (importTag == m_mpValueType2ImportTag.end())
  {
    return "";
  }

  return importTag->second;
}

CSurfaceValueType::CSurfaceValueType(IPointSet& point_set, const QString &strName)
: TBaseClass(point_set, strName)
{
  new TLengthComponent(*this);
//	assert(point_set.Dimension() == CPointSet::DIM_2D);
	if(dynamic_cast<CHexaModel*>(&Model()))
		m_pSurface = new CHexaSurface(*this);
	else
		m_pSurface = new CTetraSurface(*this);
  if (strName == "Surface")
    m_pSurface->Name(point_set.Name());
  else
	  m_pSurface->Name(strName);
}

CSurfaceValueType::~CSurfaceValueType()
{
}

void CSurfaceValueType::Unit(CQuantity::UNIT unit)
{
	m_pSurface->CoordinateUnit(unit);
}


void CSurfaceValueType::OnNeighbourDeleted(const CGraphNode& node)
{
	if(m_pSurface == &node)
	{
		delete this;
		return;
	}

	TBaseClass::OnNeighbourDeleted(node);
}

void CSurfaceValueType::OnNeighbourModified(const CGraphNode& node, enum ModifiedHint uHint)
{
	TBaseClass::OnNeighbourModified(node, uHint);

	if(m_pSurface == &node)
	{
		if(Name() != m_pSurface->Name())
		{
			CValueType::Name(m_pSurface->Name());
		}
	}
	if (&PointSet() == &node)
	{
		// The component is modified, just propagate to the surface
		m_pSurface->Invalidate();
	}
}
	
const CSurfaceBase& CSurfaceValueType::Surface() const
{
	return *m_pSurface;
}

CSurfaceBase& CSurfaceValueType::Surface()
{
	return *m_pSurface;
}

void CSurfaceValueType::Name(const QString& sName)
{
	m_pSurface->Name(sName);
}

const QString& CSurfaceValueType::Name() const
{
	return CValueType::Name();
}

bool CSurfaceValueType::CanDestroy() const
{
	return !m_pSurface->Used();
}
