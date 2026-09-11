
#include <cassert>

#include <qstring.h>

#include "MaterialParameter.h"
#include "ValueTypes.h"
#include "LibraryMaterialParameter.h"
#include "Materials.h"

#include <map>

typedef struct _ParameterTypeMapping
{
  unsigned int uiValueTypeID;
  const QString strMatLibXID;
} TParameterTypeMapping;

static TParameterTypeMapping s_ParamMap[] =
{
  { IDT_VALUETYPE_COHESION,                  MLD_COHESION                  },
  { IDT_VALUETYPE_FRICTION_ANGLE,            MLD_FRICTIONANGLE             },
  { IDT_VALUETYPE_POISSONS_RATIO,            MLD_POISSONRATIO              },
  { IDT_VALUETYPE_YOUNGS_MODULUS,            MLD_YOUNGMODULUS              },
  { IDT_VALUETYPE_POROSITY,                  MLD_POROSITY                  },
  { IDT_VALUETYPE_INITFRICTION,              MLD_INITFRICTION              },
  { IDT_VALUETYPE_HARDENING,                 MLD_HARDENING                 },
  { IDT_VALUETYPE_PRECONSOLIDATION,          MLD_PRECONSOLIDATION          },
  { IDT_VALUETYPE_CAPSHAPE,                  MLD_CAPSHAPE                  },
  { IDT_VALUETYPE_DILATATION,                MLD_DILATATION                },
  { IDT_VALUETYPE_LATRATIO_MAX,              MLD_LATRATIO_MAX              },
  { IDT_VALUETYPE_LATRATIO_MIN,              MLD_LATRATIO_MIN              },
  { IDT_VALUETYPE_AZIMUTH,                   MLD_AZIMUTH                   },
  { IDT_VALUETYPE_INIPORO,                   MLD_INIPORO                   },
  { IDT_VALUETYPE_CREEP_A1,                  MLD_A1                        },
  { IDT_VALUETYPE_CREEP_N1,                  MLD_n1                        },
  { IDT_VALUETYPE_CREEP_A2,                  MLD_A2                        },
  { IDT_VALUETYPE_CREEP_N2,                  MLD_n2                        },
  { IDT_VALUETYPE_BULKSTIFFNESS,             MLD_BULKSTIFFNESS             },
  { IDT_VALUETYPE_SHEARMODULUS,              MLD_SHEARMODULUS              },
  { IDT_VALUETYPE_DYNUNISTIFFNESS,           MLD_DYNUNISTIFFNESS           },
  { IDT_VALUETYPE_DYNSHEARSTIFFNESS,         MLD_DYNSHEARSTIFFNESS         },
  { IDT_VALUETYPE_RHOB,                      MLD_RHOB                      },
  { IDT_VALUETYPE_VELOCITYP,                 MLD_VELOCITYP                 },
  { IDT_VALUETYPE_VELOCITYS,                 MLD_VELOCITYS                 },
  { IDT_VALUETYPE_FLUID_BULK_MOD,            MLD_FLUIDMODULUS              },
  { IDT_VALUETYPE_UNDRAINEDYOUNG,            MLD_UNDRAINEDYOUNG            },
  { IDT_VALUETYPE_UNDRAINEDPOISSON,          MLD_UNDRAINEDPOISSON          },
  { IDT_VALUETYPE_UNDRAINEDTHERMX,           MLD_UNDRAINEDTHERMX           },
  { IDT_VALUETYPE_CAPSHAPEPARAM,             MLD_CAPSHAPEPARAM             },
  { IDT_VALUETYPE_HARD_COHESION1,            MLD_HARD_COHESION1            },
  { IDT_VALUETYPE_HARD_COHESION2,            MLD_HARD_COHESION2            },
  { IDT_VALUETYPE_HARD_COHESION3,            MLD_HARD_COHESION3            },
  { IDT_VALUETYPE_EQUIV_PLAST_STRAIN1,       MLD_EQUIV_PLAST_STRAIN1       },
  { IDT_VALUETYPE_EQUIV_PLAST_STRAIN2,       MLD_EQUIV_PLAST_STRAIN2       },
  { IDT_VALUETYPE_EQUIV_PLAST_STRAIN3,       MLD_EQUIV_PLAST_STRAIN3       },
  { IDT_VALUETYPE_HARD_FRICTION1,            MLD_HARD_FRICTION1            },
  { IDT_VALUETYPE_HARD_FRICTION2,            MLD_HARD_FRICTION2            },
  { IDT_VALUETYPE_HARD_FRICTION3,            MLD_HARD_FRICTION3            },
  { IDT_VALUETYPE_CREEP_QR1,                 MLD_ACTIVATION_ENERGY1        },
  { IDT_VALUETYPE_CREEP_QR2,                 MLD_ACTIVATION_ENERGY2        },
  { IDT_VALUETYPE_CREEP_ALPHA,               MLD_REFERENCE_STRESS          },
  { IDT_VALUETYPE_RIGI_PARAM1,               MLD_RIGIDITY_PARAM1           },
  { IDT_VALUETYPE_RIGI_PARAM2,               MLD_RIGIDITY_PARAM2           },
  { IDT_VALUETYPE_RIGI_PARAM3,               MLD_RIGIDITY_PARAM3           },
  { IDT_VALUETYPE_RIGI_PARAM4,               MLD_RIGIDITY_PARAM4           },
  { IDT_VALUETYPE_RIGI_PARAM5,               MLD_RIGIDITY_PARAM5           },
  { IDT_VALUETYPE_RIGI_PARAM6,               MLD_RIGIDITY_PARAM6           },
  { IDT_VALUETYPE_RIGI_PARAM7,               MLD_RIGIDITY_PARAM7           },
  { IDT_VALUETYPE_RIGI_PARAM8,               MLD_RIGIDITY_PARAM8           },
  { IDT_VALUETYPE_RIGI_PARAM9,               MLD_RIGIDITY_PARAM9           },
  { IDT_VALUETYPE_RIGI_PARAM10,              MLD_RIGIDITY_PARAM10          },
  { IDT_VALUETYPE_RIGI_PARAM11,              MLD_RIGIDITY_PARAM11          },
  { IDT_VALUETYPE_RIGI_PARAM12,              MLD_RIGIDITY_PARAM12          },
  { IDT_VALUETYPE_RIGI_PARAM13,              MLD_RIGIDITY_PARAM13          },
  { IDT_VALUETYPE_RIGI_PARAM14,              MLD_RIGIDITY_PARAM14          },
  { IDT_VALUETYPE_RIGI_PARAM15,              MLD_RIGIDITY_PARAM15          },
  { IDT_VALUETYPE_RIGI_PARAM16,              MLD_RIGIDITY_PARAM16          },
  { IDT_VALUETYPE_RIGI_PARAM17,              MLD_RIGIDITY_PARAM17          },
  { IDT_VALUETYPE_RIGI_PARAM18,              MLD_RIGIDITY_PARAM18          },
  { IDT_VALUETYPE_RIGI_PARAM19,              MLD_RIGIDITY_PARAM19          },
  { IDT_VALUETYPE_RIGI_PARAM20,              MLD_RIGIDITY_PARAM20          },
  { IDT_VALUETYPE_RIGI_PARAM21,              MLD_RIGIDITY_PARAM21          },
  { IDT_VALUETYPE_TENSILE_STRETCH,           MLD_GAMMA                     },
  { IDT_VALUETYPE_THERMAL_EXPANSION,         MLD_THERMX                    },
  { IDT_VALUETYPE_ELASTIC_HARDENING,         MLD_ELASTIC_HARDENING         },
  { IDT_VALUETYPE_SEC_PRECON,                MLD_SEC_PRECON                },
  { IDT_VALUETYPE_SEC_HARDENING,             MLD_SEC_HARDENING             },
  { IDT_VALUETYPE_ELASCOMPRES,               MLD_CME                       },
  { IDT_VALUETYPE_PLASCOMPRES,               MLD_CMP                       },
  { IDT_VALUETYPE_YOUNGMODULUS_NORM,         MLD_YOUNGMODULUS_NORM         },
  { IDT_VALUETYPE_YOUNGMODULUS_TRANS,        MLD_YOUNGMODULUS_TRANS        },
  { IDT_VALUETYPE_POISSONRATIO_NORM,         MLD_POISSONRATIO_NORM         },
  { IDT_VALUETYPE_POISSONRATIO_TRANS,        MLD_POISSONRATIO_TRANS        },
  { IDT_VALUETYPE_LAYER_NORMAL_INCL,         MLD_LAYER_NORMAL_INCL         },
  { IDT_VALUETYPE_LAYER_NORMAL_AZI,          MLD_LAYER_NORMAL_AZI          },
  { IDT_VALUETYPE_FRACT_STIFF_NORMAL,        MLD_FRACT_STIFF_NORMAL        },
  { IDT_VALUETYPE_FRACT_STIFF_SHEAR,         MLD_FRACT_STIFF_SHEAR         },
  { IDT_VALUETYPE_HIGH_FRACT_DENS,           MLD_HIGH_FRACT_DENS           },
  { IDT_VALUETYPE_HIGH_FRACT_DENS_INCL,      MLD_HIGH_FRACT_DENS_INCL      },
  { IDT_VALUETYPE_HIGH_FRACT_DENS_AZI,       MLD_HIGH_FRACT_DENS_AZI       },
  { IDT_VALUETYPE_LOW_FRACT_DENS,            MLD_LOW_FRACT_DENS            },
  { IDT_VALUETYPE_LOW_FRACT_DENS_INCL,       MLD_LOW_FRACT_DENS_INCL       },
  { IDT_VALUETYPE_LOW_FRACT_DENS_AZI,        MLD_LOW_FRACT_DENS_AZI        },
  { IDT_VALUETYPE_INTER_FRACT_DENS,          MLD_INTER_FRACT_DENS          },
  { IDT_VALUETYPE_THERM_LIN_EXP_NORM,        MLD_THERM_LIN_EXP_NORM        },
  { IDT_VALUETYPE_THERM_LIN_EXP_LAT,         MLD_THERM_LIN_EXP_LAT         },
  { IDT_VALUETYPE_THERM_CONDUCT,             MLD_THERM_CONDUCT             },
  { IDT_VALUETYPE_THERM_CAPACI,              MLD_THERM_CAPACI              },
  { IDT_VALUETYPE_PERMEA,                    MLD_PERMEA                    },
  { IDT_VALUETYPE_YOUNG_DECOMP,              MLD_YOUNGMODULUS_DECOMP       },
  { IDT_VALUETYPE_POISSON_DECOMP,            MLD_POISSONRATIO_DECOMP       },
  { IDT_VALUETYPE_VISCOSITY,                 MLD_VISCOSITY                 },
  { IDT_VALUETYPE_FLUIDX,                    MLD_FLUIDX                    },
  { IDT_VALUETYPE_FLUID_DENSITY,             MLD_FLUID_DENSITY             },
  { IDT_VALUETYPE_REFERENCESTRESS,           MLD_REFERENCESTRESS           },
  { IDT_VALUETYPE_APERTUREPARAM,             MLD_APERTUREPARAM             },
  { IDT_VALUETYPE_GRAINSTIFFNESS,            MLD_GRAINSTIFFNESS            }, //wjrx mantis 3314
  { IDT_VALUETYPE_YOUNGMODULUS_NORM_DECOMP,  MLD_YOUNGMODULUS_NORM_DECOMP  },
  { IDT_VALUETYPE_YOUNGMODULUS_TRANS_DECOMP, MLD_YOUNGMODULUS_TRANS_DECOMP },
  { IDT_VALUETYPE_POISSONRATIO_NORM_DECOMP,  MLD_POISSONRATIO_NORM_DECOMP  },
  { IDT_VALUETYPE_POISSONRATIO_TRANS_DECOMP, MLD_POISSONRATIO_TRANS_DECOMP },
  { IDT_VALUETYPE_ANISOTROPIC_SHEARMODULUS,        MLD_ANISOTROPIC_SHEARMODULUS        },
  { IDT_VALUETYPE_ANISOTROPIC_SHEARMODULUS_DECOMP, MLD_ANISOTROPIC_SHEARMODULUS_DECOMP },
  { IDT_VALUETYPE_THOMSEN_EPSILON,                 MLD_THOMSEN_EPSILON                 },
  { IDT_VALUETYPE_THOMSEN_GAMMA,                   MLD_THOMSEN_GAMMA                   },
  { IDT_VALUETYPE_THOMSEN_DELTA,                   MLD_THOMSEN_DELTA                   },
  { IDT_VALUETYPE_V0_HI,                           MLD_V0_HI                           },
  { IDT_VALUETYPE_V0_ME,                           MLD_V0_ME                           },
  { IDT_VALUETYPE_V0_LO,                           MLD_V0_LO                           },
  { IDT_VALUETYPE_REFERENCE_APERTURE,              MLD_REFERENCE_APERTURE              },
  { IDT_VALUETYPE_DUMMY,                           MLD_DUMMY                           },
  { 0,                                             QString()                           }
};

std::map<unsigned int, QString> CMaterialParameter::m_Vt2Str; // get matlibx string id from value type id
std::map<QString, unsigned int> CMaterialParameter::m_Str2Vt; // get value type id from matlibx string id

void CMaterialParameter::BuildValueTypeMaps()
{
  for(int i = 0; s_ParamMap[i].uiValueTypeID; i++)
  {
    std::pair<unsigned int, QString> vt2str;
    vt2str.first  = s_ParamMap[i].uiValueTypeID;
    vt2str.second = s_ParamMap[i].strMatLibXID;
    m_Vt2Str.insert(vt2str);

    std::pair<QString, unsigned int> str2vt;
    str2vt.first  = s_ParamMap[i].strMatLibXID;
    str2vt.second = s_ParamMap[i].uiValueTypeID;
    m_Str2Vt.insert(str2vt);
  }
}

CMaterialParameter::CMaterialParameter(unsigned int uiValueTypeID, CLibraryMaterialParameter& LibraryParameter) :
  m_LibraryParameter(LibraryParameter), m_uiValueTypeID(uiValueTypeID)
{
  // build static value type maps if they don't exist yet
  if(m_Vt2Str.empty())
  {
    assert(m_Str2Vt.empty());
    BuildValueTypeMaps();
  }
}

CMaterialParameter::~CMaterialParameter()
{
}

double CMaterialParameter::Value() const
{
  return m_LibraryParameter.Value();
}
/*
void CMaterialParameter::Value(const double& dValue)
{
  m_dValue = dValue;
}
*/
unsigned int CMaterialParameter::ValueTypeID() const
{
  return m_uiValueTypeID;
}
/*
CString CMaterialParameter::MatLibXString() const
{
  return m_strMatLibXID;
}
*/
bool CMaterialParameter::ValueTypeIDFromMatLibXID(const QString& MatLibXID, unsigned int& ValueTypeID)
{
  // build static value type maps if they don't exist yet
  if(m_Vt2Str.empty())
  {
    assert(m_Str2Vt.empty());
    BuildValueTypeMaps();
  }

  std::map<QString, unsigned int>::iterator it = m_Str2Vt.find(MatLibXID);
  if(it == m_Str2Vt.end()) return false;

  ValueTypeID = it->second;
  return true;
}

bool CMaterialParameter::MatLibXIDFromValueTypeID(unsigned int ValueTypeID, QString& MatLibXID)
{
  // build static value type maps if they don't exist yet
  if(m_Vt2Str.empty())
  {
    assert(m_Str2Vt.empty());
    BuildValueTypeMaps();
  }

  std::map<unsigned int, QString>::iterator it = m_Vt2Str.find(ValueTypeID);
  if(it == m_Vt2Str.end()) return false;

  MatLibXID = it->second;
  return true;
}

bool CMaterialParameter::operator<(const CMaterialParameter& rhs) const
{
  return (m_uiValueTypeID < rhs.m_uiValueTypeID ||
    (!(rhs.m_uiValueTypeID < m_uiValueTypeID) && Value() < rhs.Value())); 
} 

long CMaterialParameter::SavedItems() const
{
  // name and value, fixed is written by parent material class
  return 2;
}