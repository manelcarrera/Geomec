#ifndef _MATERIALDEFINES_H_
#define _MATERIALDEFINES_H_

#include <qstring.h>

/*
 * NOTE: Never simply change these names! GEOMEC saves and loads its material parameter values
 * with the parameter names. If the names are changed, the parameters cannot be found anymore
 * when loading an old file.
 *
 * If you need to change a name, have a look at the ConvertMatParamName function and the defines
 * above it in IMaterial.cpp in the GEOMEC project.
 */

// material parameter names
const QString MLD_COHESION                  = "Cohesion";
const QString MLD_FRICTIONANGLE             = "Friction Angle";
const QString MLD_POISSONRATIO              = "Poisson's Ratio";
const QString MLD_YOUNGMODULUS              = "Young's Modulus";
const QString MLD_BULKSTIFFNESS             = "Bulk Stiffness";
const QString MLD_SHEARMODULUS              = "Shear Modulus";
const QString MLD_DYNUNISTIFFNESS           = "Dynamic Uniaxial Stiffness";
const QString MLD_DYNSHEARSTIFFNESS         = "Dynamic Shear Stiffness";
const QString MLD_RHOB                      = "Density";
const QString MLD_VELOCITYP                 = "Vp";
const QString MLD_VELOCITYS                 = "Vs";
const QString MLD_POROSITY                  = "Porosity";
const QString MLD_INITFRICTION              = "Initial Friction Angle";
const QString MLD_HARDENING                 = "Hardening Parameter";
const QString MLD_PRECONSOLIDATION          = "Preconsolidation Stress";
const QString MLD_CAPSHAPE                  = "Cap Shape Factor";
const QString MLD_DILATATION                = "Dilatation Angle";
const QString MLD_LATRATIO_MAX              = "SHtot/Svtot";
const QString MLD_LATRATIO_MIN              = "Shtot/Svtot";
const QString MLD_AZIMUTH                   = "SHtot azimuth";
const QString MLD_INIPORO                   = "Porosity";
const QString MLD_LATRATIO                  = "Lateral Total Stress Ratio";
const QString MLD_INISTRAINRATE             = "Initial Strain Rate";
const QString MLD_VPPROCPARAM               = "Viscoplastic Process Parameter";
const QString MLD_A1                        = "Creep strain rate coefficient 1";
const QString MLD_n1                        = "Creep power coefficient 1";
const QString MLD_A2                        = "Creep strain rate coefficient 2";
const QString MLD_n2                        = "Creep power coefficient 2";
const QString MLD_FLUIDMODULUS              = "Fluid Bulk Modulus";
const QString MLD_UNDRAINEDYOUNG            = "Undrained Young's Modulus";
const QString MLD_UNDRAINEDPOISSON          = "Undrained Poisson's Ratio";
const QString MLD_CAPSHAPEPARAM             = "Cap Shape Parameter";
const QString MLD_HARD_COHESION1            = "Hardening Cohesion 1";
const QString MLD_HARD_COHESION2            = "Hardening Cohesion 2";
const QString MLD_HARD_COHESION3            = "Hardening Cohesion 3";
const QString MLD_EQUIV_PLAST_STRAIN1       = "Equivalent Plastic Strain 1";
const QString MLD_EQUIV_PLAST_STRAIN2       = "Equivalent Plastic Strain 2";
const QString MLD_EQUIV_PLAST_STRAIN3       = "Equivalent Plastic Strain 3";
const QString MLD_HARD_FRICTION1            = "Hardening Friction Angle 1";
const QString MLD_HARD_FRICTION2            = "Hardening Friction Angle 2";
const QString MLD_HARD_FRICTION3            = "Hardening Friction Angle 3";
const QString MLD_ACTIVATION_ENERGY1        = "Activation energy, Q1/R";
const QString MLD_ACTIVATION_ENERGY2        = "Activation energy, Q2/R";
const QString MLD_REFERENCE_STRESS          = "Reference stress";
const QString MLD_GAMMA                     = "Tensile stretch";
const QString MLD_RIGIDITY_PARAM1           = "KNNNN";
const QString MLD_RIGIDITY_PARAM2           = "KNNEE";
const QString MLD_RIGIDITY_PARAM3           = "KNNDD";
const QString MLD_RIGIDITY_PARAM4           = "KNNNE";
const QString MLD_RIGIDITY_PARAM5           = "KNNED";
const QString MLD_RIGIDITY_PARAM6           = "KNNDN";
const QString MLD_RIGIDITY_PARAM7           = "KEEEE";
const QString MLD_RIGIDITY_PARAM8           = "KEEDD";
const QString MLD_RIGIDITY_PARAM9           = "KEENE";
const QString MLD_RIGIDITY_PARAM10          = "KEEED";
const QString MLD_RIGIDITY_PARAM11          = "KEEDN";
const QString MLD_RIGIDITY_PARAM12          = "KDDDD";
const QString MLD_RIGIDITY_PARAM13          = "KDDNE";
const QString MLD_RIGIDITY_PARAM14          = "KDDED";
const QString MLD_RIGIDITY_PARAM15          = "KDDDN";
const QString MLD_RIGIDITY_PARAM16          = "KNENE";
const QString MLD_RIGIDITY_PARAM17          = "KNEED";
const QString MLD_RIGIDITY_PARAM18          = "KNEDN";
const QString MLD_RIGIDITY_PARAM19          = "KEDED";
const QString MLD_RIGIDITY_PARAM20          = "KEDDN";
const QString MLD_RIGIDITY_PARAM21          = "KDNDN";
const QString MLD_ELASTIC_HARDENING         = "Elastic Hardening Parameter";
const QString MLD_SEC_PRECON                = "Secondary Preconsolidation Stress";
const QString MLD_SEC_HARDENING             = "Secondary Hardening Parameter";
const QString MLD_CME                       = "Uniaxial Elastic Compressibility";
const QString MLD_CMP                       = "Uniaxial Plastic Compressibility";
const QString MLD_YOUNGMODULUS_NORM         = "Young's Modulus normal";
const QString MLD_YOUNGMODULUS_TRANS        = "Young's Modulus transverse";
const QString MLD_POISSONRATIO_NORM         = "Poisson's Ratio normal";
const QString MLD_POISSONRATIO_TRANS        = "Poisson's Ratio transverse";
const QString MLD_LAYER_NORMAL_INCL         = "Layer normal dip";
const QString MLD_LAYER_NORMAL_AZI          = "Layer normal azimuth";
const QString MLD_FRACT_STIFF_NORMAL        = "Fracture stiffness normal";
const QString MLD_FRACT_STIFF_SHEAR         = "Fracture stiffness shear";
const QString MLD_HIGH_FRACT_DENS           = "Highest Fracture Density";
const QString MLD_HIGH_FRACT_DENS_AZI       = "Highest Fracture Density azimuth";
const QString MLD_LOW_FRACT_DENS            = "Lowest Fracture Density";
const QString MLD_LOW_FRACT_DENS_AZI        = "Lowest Fracture Density azimuth";
const QString MLD_INTER_FRACT_DENS          = "Intermediate Fracture Density";
const QString MLD_THERM_LIN_EXP_NORM        = "Thermal linear expansion normal";
const QString MLD_THERM_LIN_EXP_LAT         = "Thermal linear expansion lateral";
const QString MLD_THERM_CONDUCT             = "Thermal Conductivity";
const QString MLD_THERM_CAPACI              = "Thermal Capacity";
const QString MLD_PERMEA                    = "Permeability";
const QString MLD_YOUNGMODULUS_DECOMP       = "Young's Modulus in decompaction";
const QString MLD_POISSONRATIO_DECOMP       = "Poisson's Ratio in decompaction";
const QString MLD_VISCOSITY                 = "Viscosity";
const QString MLD_FLUID_DENSITY             = "Fluid Density";
const QString MLD_REFERENCESTRESS           = "Reference Stress";
const QString MLD_APERTUREPARAM             = "Aperture Dependency";
const QString MLD_GRAINSTIFFNESS            = "Grain Stiffness";
const QString MLD_ANISOTROPIC_SHEARMODULUS  = "Anisotropic Shear Modulus";
const QString MLD_THOMSEN_EPSILON           = "Thomsen epsilon";
const QString MLD_THOMSEN_GAMMA             = "Thomsen gamma";
const QString MLD_THOMSEN_DELTA             = "Thomsen delta";
const QString MLD_REFERENCE_APERTURE        = "Reference Aperture";
const QString MLD_DUMMY                     = "Dummy";

// waij TFS 81578, shortened names
const QString MLD_UNDRAINEDTHERMX           = "Undrained Thermal Expansion Coeff.";
const QString MLD_THERMX                    = "Vol. Thermal Expansion Coeff.";
const QString MLD_HIGH_FRACT_DENS_INCL      = "Highest Fracture Density incl.";
const QString MLD_LOW_FRACT_DENS_INCL       = "Lowest Fracture Density incl.";
const QString MLD_YOUNGMODULUS_NORM_DECOMP  = "Young's Mod. normal in decompact.";
const QString MLD_YOUNGMODULUS_TRANS_DECOMP = "Young's Mod. transverse in decompact.";
const QString MLD_POISSONRATIO_NORM_DECOMP  = "Poisson's Ratio normal in decompact.";
const QString MLD_POISSONRATIO_TRANS_DECOMP = "Poisson's Ratio transverse in decompact.";
const QString MLD_ANISOTROPIC_SHEARMODULUS_DECOMP = "Anisotropic Shear Mod. in decompact.";
const QString MLD_V0_HI                     = "V0 in highest fracture density dir.";
const QString MLD_V0_ME                     = "V0 in intermediate fracture density dir.";
const QString MLD_V0_LO                     = "V0 in lowest fracture denstiy dir.";
const QString MLD_FLUIDX                    = "Fluid Thermal Expansion Coeff.";

// Material models
const QString MLD_LINEAR              = "Linear";
const QString MLD_CAMCLAY             = "Cam-clay";
const QString MLD_MOHRCOULOMB         = "Mohr-Coulomb";
const QString MLD_MODIFIEDMOHRCOULOMB = "Modified Mohr-Coulomb";
const QString MLD_CREEP               = "Salt Creep";
const QString MLD_UNDRAINED           = "Undrained Elastic";
const QString MLD_MC_COHESION_HARD1   = "Mohr-Coulomb with Linear Cohesion Hardening/Softening";
const QString MLD_MC_COHESION_HARD2   = "Mohr-Coulomb with Bi-linear Cohesion Hardening/Softening";
const QString MLD_MC_COHESION_HARD3   = "Mohr-Coulomb with Tri-linear Cohesion Hardening/Softening";
const QString MLD_MC_FRICTION_HARD1   = "Mohr-Coulomb with Linear Friction Angle Hardening/Softening";
const QString MLD_MC_FRICTION_HARD2   = "Mohr-Coulomb with Bi-linear Friction Angle Hardening/Softening";
const QString MLD_MC_FRICTION_HARD3   = "Mohr-Coulomb with Tri-linear Friction Angle Hardening/Softening";
const QString MLD_RIGIDITY            = "General Anisotropy";
const QString MLD_DUALCAP_LINELA      = "Dual Cap with linear elasticity";
const QString MLD_FRACTURE_ANISOTROPY = "Fracture Anisotropy";
const QString MLD_UPSCALED_ANISOTROPY = "Upscaled Anisotropy";
const QString MLD_FRACTURE_APERTURE   = "Fracture Aperture";
const QString MLD_ANISOTROPIC_CAMCLAY = "Anisotropic Cam-clay";
const QString MLD_FRACTURE_APERTURE2  = "Fracture Aperture 2";
const QString MLD_CAMCLAY_CREEP       = "Cam-clay creep";

// default parameter values
const double DEF_E = 10000;
const double DEF_NU = 0.2;
const double DEF_CME = (1+DEF_NU)*(1-2*DEF_NU) / (DEF_E * (1-DEF_NU));
const double DEF_G = (DEF_E / (2 * (1 + DEF_NU)));
const double DEF_SALTCREEP_RHOB = 2200;
const double DEF_RHOB = 2300;
const double DEF_COHESION = 0;
const double DEF_FRICTION = 15;
const double DEF_LATRATIO_MAX = 1;
const double DEF_LATRATIO_MIN = 1;
const double DEF_AZIMUTH = 0;
const double DEF_SALTCREEP_PORO = 0;
const double DEF_PORO = 0.2;
const double DEF_INITFRICTION = 15;
const double DEF_HARDENING = 0.05;
const double DEF_PRECONSO = 10;
const double DEF_CAPSHAPE = 1.0;
const double DEF_CAPSHAPEPARAM = (((double) 2) / 9);
const double DEF_DILATATION = 10;
const double DEF_A1 = 2.25E-4;
const double DEF_QR1 = 8000.0;
const double DEF_N1 = 4.5;
const double DEF_A2 = 0.0;
const double DEF_N2 = 1.0;
const double DEF_QR2 = 0.0;
const double DEF_REF_STRESS = 1.0;
const double DEF_INISTRAINRATE = 0.0;
const double DEF_VPPROCPARAM = 0.0;
const double DEF_SALTCREEP_FLUIDMOD = 0;
const double DEF_FLUIDMOD = 1500;
const double DEF_EQUIV_PLAST_STRAIN1 = 0.2;
const double DEF_EQUIV_PLAST_STRAIN2 = 0.4;
const double DEF_EQUIV_PLAST_STRAIN3 = 0.6;
const double DEF_RIGID1 = 1.111E4;
const double DEF_RIGID2 = 2.777E3;
const double DEF_RIGID3 = 3.333E3;
const double DEF_GAMMA = 1.;
const double DEF_THERMX =24E-6;
const double DEF_ELASTICHARD = 0.00225;
const double DEF_SEC_PRECON = 11;
const double DEF_SEC_HARD = 0;
const double DEF_THERM_CONDUCT = 1;
const double DEF_THERM_CAPACI = 1000;
const double DEF_FRACT_INCL = 90;
const double DEF_SALTCREEP_UNDRAINED_PERMEA = 0;
const double DEF_PERMEA = 1;
const double DEF_VISCOSITY = 1;
const double DEF_SALTCREEP_FLUIDX = 0;
const double DEF_FLUIDX = 0.0003;
const double DEF_THERMX_NORMAL = 8E-6;
const double DEF_THERMX_TRANS = 8E-6;
const double DEF_FLUID_DENSITY = 1020;
const double DEF_REFERENCESTRESS = 0;
const double DEF_APERTUREPARAM = 0;
const double DEF_LAYERNORMAL_INCL = 0;
const double DEF_LAYERNORMAL_AZI = 0;
const double DEF_GRAINSTIFFNESS = 100000;
const double DEF_REFERENCE_APERTURE = 0.0001;

// Filos paths (calibration)
const QString YOUNG_FILOSPATH  = "YOUNG";
const QString POISON_FILOSPATH = "POISON";
const QString COHESI_FILOSPATH = "COHESI";
const QString FRICTI_FILOSPATH = "FRICTI";
const QString DILATA_FILOSPATH = "DILATA";
const QString PRECON_FILOSPATH = "PRECON";
const QString LAMBDA_FILOSPATH = "LAMBDA";
const QString ALPHA_FILOSPATH  = "ALPHA";
const QString POROSI_FILOSPATH = "POROSI";
const QString DEPS0_FILOSPATH  = "DEPS0";
const QString VISCOP_FILOSPATH = "VISCOP";
const QString CRPA1_FILOSPATH  = "CRPA1";
const QString CRPA2_FILOSPATH  = "CRPA2";
const QString Q1BYR_FILOSPATH  = "Q1BYR";
const QString Q2BYR_FILOSPATH  = "Q2BYR";
const QString CRPN1_FILOSPATH  = "CRPN1";
const QString CRPN2_FILOSPATH  = "CRPN2";
const QString ELAHAR_FILOSPATH = "ELAHAR";
const QString PRECO1_FILOSPATH = "PRECO1";
const QString MU_FILOSPATH     = "MU";
const QString GAMMA_FILOSPATH  = "GAMMA";
const QString BULKS_FILOSPATH  = "BULKS";

// material model
// don't renumber as they are saved in the model file
typedef enum _MaterialModel
{
  MM_LINEAR              =  0,
  MM_CAMCLAY             =  1,
  MM_MOHRCOULOMB         =  2,
  MM_MODIFIEDMOHRCOULOMB =  3,
  MM_CREEP               =  4,
  MM_UNDRAINED           =  5,
  MM_MC_COHESION_HARD1   =  6,
  MM_MC_COHESION_HARD2   =  7,
  MM_MC_COHESION_HARD3   =  8,
  MM_MC_FRICTION_HARD1   =  9,
  MM_MC_FRICTION_HARD2   = 10,
  MM_MC_FRICTION_HARD3   = 11,
  MM_RIGIDITY            = 13,
  MM_DUALCAP             = 14,  // Not used anymore, maintained for preventing
                // magic numbers.
  MM_DUALCAP_LINELA      = 15,
  MM_FRACTURE_ANISOTROPY = 16,
  MM_UPSCALED_ANISOTROPY = 17,
  MM_FRACTURE_APERTURE   = 18,
  MM_ANISOTROPIC_CAMCLAY = 19,
  MM_FRACTURE_APERTURE2  = 20,
  MM_CAMCLAY_CREEP       = 21
} mlMatModel;

// Enumeration for unit types
typedef enum
{
  MLUT_NONE = 0,
  MLUT_STRESS,
  MLUT_STRESSGRADIENT,
  MLUT_DENSITY,
  MLUT_DEPTH,
  MLUT_FORCE,
  MLUT_VELOCITY,
  MLUT_ANGLE,
  MLUT_PERSTRESS,
  MLUT_PERTIME,
  MLUT_TEMPER,
  MLUT_KELVIN_TEMP,
  MLUT_PERTEMPER,
  MLUT_PERDISTANCE,
  MLUT_THERMCONDUCT,
  MLUT_THERMCAPACITY,
  MLUT_PERMEA,
  MLUT_VISCOSITY
} mlUnitType;

// Enumeration for filtering material types
typedef enum
{
  MLFO_ALL                 = 0x00000000,
  MLFO_LINEAR              = 0x00000001,
  MLFO_CAMCLAY             = 0x00000002,
  MLFO_MOHRCOULOMB         = 0x00000004,
  MLFO_MODIFIEDMOHRCOULOMB = 0x00000008,
  MLFO_CREEP               = 0x00000010,
  MLFO_UNDRAINED           = 0x00000020,
  MLFO_MC_COHESION_HARD1   = 0x00000040,
  MLFO_MC_COHESION_HARD2   = 0x00000080,
  MLFO_MC_COHESION_HARD3   = 0x00000100,
  MLFO_MC_FRICTION_HARD1   = 0x00000200,
  MLFO_MC_FRICTION_HARD2   = 0x00000400,
  MLFO_MC_FRICTION_HARD3   = 0x00000800,
  MLFO_RIGIDITY            = 0x00001000,
  MLFO_DUALCAP_LINELA      = 0x00004000,
  MLFO_FRACTURE_ANISOTROPY = 0x00008000,
  MLFO_UPSCALED_ANISOTROPY = 0x00010000,
  MLFO_FRACTURE_APERTURE   = 0x00020000,
  MLFO_ANISOTROPIC_CAMCLAY = 0x00040000,
  MLFO_FRACTURE_APERTURE2  = 0x00080000,
  MLFO_CAMCLAY_CREEP       = 0x00100000
} mlFilterOption;

// Enumeration for unit definitions
typedef enum
{
  MLUD_SI = 0,
  MLUD_FIELD
} mlUnitDef;

bool FilterContainsMaterialModel(mlMatModel matmodel, int filter);
mlFilterOption MaterialFilter(mlMatModel matmodel);
mlMatModel MaterialModel(mlFilterOption filter);
const QString& MaterialModelName(mlMatModel matmodel);

#endif // _MATERIALDEFINES_H_
