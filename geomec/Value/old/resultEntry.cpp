// resultEntry.cpp: implementation of the CresultEntry class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#ifdef _MSC_VER
#pragma warning ( disable : 4786 )
#endif  // _MSC_VER
#include "geomec.h"
#include "resultEntry.h"
#include "FieldFactor.h"
#include "BaseEntryTypes.h"
#include "resultserver.h"
#include "AttriResultEntry.h"
#include "ValueType.h"

#ifdef _DEBUG
#ifdef _MSC_VER#undef THIS_FILE
static char THIS_FILE[]=__FILE__;#endif  // _MSC_VER
#define new DEBUG_NEW
#endif


#define RS_ARG_NONE  0

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CResultEntry::CResultEntry(CResultServer& server)
: CGraphEntryTemp<IResult>(MD_BASE_RESULT, IDI_RESULT, IDS_TREE_RESULTS), m_server(server), m_weighting_type(CValueType::NO_WEIGHTING)
{
	// Total stress
	CreateProgramResult(CResultServer::RT_TS_TOT_VERT_STRESS,
						IDS_RN_TOT_VERT_STRESS,
						IDS_ET_TOT_VERT_STRESS,
						IDS_RL_TOT_STRESS,
						RS_ARG_NONE,
						IDS_UNIT_SI_STRESS,
						IDS_UNIT_FIELD_STRESS,
						FF_FACTOR_STRESS);

	CreateProgramResult(CResultServer::RT_TS_TOT_NN_HORZ_STRESS,
						IDS_RN_TOT_NN_HORZ_STRESS,
						IDS_ET_TOT_NN_HORZ_STRESS,
						IDS_RL_TOT_STRESS,
						RS_ARG_NONE,
						IDS_UNIT_SI_STRESS,
						IDS_UNIT_FIELD_STRESS,
						FF_FACTOR_STRESS);

	CreateProgramResult(CResultServer::RT_TS_TOT_EE_HORZ_STRESS,
						IDS_RN_TOT_EE_HORZ_STRESS,
						IDS_ET_TOT_EE_HORZ_STRESS,
						IDS_RL_TOT_STRESS,
						RS_ARG_NONE,
						IDS_UNIT_SI_STRESS,
						IDS_UNIT_FIELD_STRESS,
						FF_FACTOR_STRESS);

	CreateProgramResult(CResultServer::RT_TS_TOT_MAX_HORZ_STRESS,
						IDS_RN_TOT_MAX_HORZ_STRESS,
						IDS_ET_TOT_MAX_HORZ_STRESS,
						IDS_RL_TOT_STRESS,
						RS_ARG_NONE,
						IDS_UNIT_SI_STRESS,
						IDS_UNIT_FIELD_STRESS,
						FF_FACTOR_STRESS);

	CreateProgramResult(CResultServer::RT_TS_TOT_MIN_HORZ_STRESS,
						IDS_RN_TOT_MIN_HORZ_STRESS,
						IDS_ET_TOT_MIN_HORZ_STRESS,
						IDS_RL_TOT_STRESS,
						RS_ARG_NONE,
						IDS_UNIT_SI_STRESS,
						IDS_UNIT_FIELD_STRESS,
						FF_FACTOR_STRESS);

	CreateProgramResult(CResultServer::RT_TS_DELTA_TOT_VERT_STRESS,
						IDS_RN_DELTA_TOT_VERT_STRESS,
						IDS_ET_DELTA_TOT_VERT_STRESS,
						IDS_RL_TOT_STRESS,
						RS_ARG_NONE,
						IDS_UNIT_SI_STRESS,
						IDS_UNIT_FIELD_STRESS,
						FF_FACTOR_STRESS);

	CreateProgramResult(CResultServer::RT_TS_DELTA_TOT_NN_HORZ_STRESS,
						IDS_RN_DELTA_TOT_NN_HORZ_STRESS,
						IDS_ET_DELTA_TOT_NN_HORZ_STRESS,
						IDS_RL_TOT_STRESS,
						RS_ARG_NONE,
						IDS_UNIT_SI_STRESS,
						IDS_UNIT_FIELD_STRESS,
						FF_FACTOR_STRESS);

	CreateProgramResult(CResultServer::RT_TS_DELTA_TOT_EE_HORZ_STRESS,
						IDS_RN_DELTA_TOT_EE_HORZ_STRESS,
						IDS_ET_DELTA_TOT_EE_HORZ_STRESS,
						IDS_RL_TOT_STRESS,
						RS_ARG_NONE,
						IDS_UNIT_SI_STRESS,
						IDS_UNIT_FIELD_STRESS,
						FF_FACTOR_STRESS);

	CreateProgramResult(CResultServer::RT_TS_DELTA_TOT_MAX_HORZ_STRESS,
						IDS_RN_DELTA_TOT_MAX_HORZ_STRESS,
						IDS_ET_DELTA_TOT_MAX_HORZ_STRESS,
						IDS_RL_TOT_STRESS,
						RS_ARG_NONE,
						IDS_UNIT_SI_STRESS,
						IDS_UNIT_FIELD_STRESS,
						FF_FACTOR_STRESS);

	CreateProgramResult(CResultServer::RT_TS_DELTA_TOT_MIN_HORZ_STRESS,
						IDS_RN_DELTA_TOT_MIN_HORZ_STRESS,
						IDS_ET_DELTA_TOT_MIN_HORZ_STRESS,
						IDS_RL_TOT_STRESS,
						RS_ARG_NONE,
						IDS_UNIT_SI_STRESS,
						IDS_UNIT_FIELD_STRESS,
						FF_FACTOR_STRESS);

// Effective stress
	CreateProgramResult(CResultServer::RT_ES_EFF_VERT_STRESS,
						IDS_RN_EFF_VERT_STRESS,
						IDS_ET_EFF_VERT_STRESS,
						IDS_RL_EFF_STRESS,
						RS_ARG_NONE,
						IDS_UNIT_SI_STRESS,
						IDS_UNIT_FIELD_STRESS,
						FF_FACTOR_STRESS);

	CreateProgramResult(CResultServer::RT_ES_EFF_NN_HORZ_STRESS,
						IDS_RN_EFF_NN_HORZ_STRESS,
						IDS_ET_EFF_NN_HORZ_STRESS,
						IDS_RL_EFF_STRESS,
						RS_ARG_NONE,
						IDS_UNIT_SI_STRESS,
						IDS_UNIT_FIELD_STRESS,
						FF_FACTOR_STRESS);

	CreateProgramResult(CResultServer::RT_ES_EFF_EE_HORZ_STRESS,
						IDS_RN_EFF_EE_HORZ_STRESS,
						IDS_ET_EFF_EE_HORZ_STRESS,
						IDS_RL_EFF_STRESS,
						RS_ARG_NONE,
						IDS_UNIT_SI_STRESS,
						IDS_UNIT_FIELD_STRESS,
						FF_FACTOR_STRESS);

	CreateProgramResult(CResultServer::RT_ES_EFF_MAX_HORZ_STRESS,
						IDS_RN_EFF_MAX_HORZ_STRESS,
						IDS_ET_EFF_MAX_HORZ_STRESS,
						IDS_RL_EFF_STRESS,
						RS_ARG_NONE,
						IDS_UNIT_SI_STRESS,
						IDS_UNIT_FIELD_STRESS,
						FF_FACTOR_STRESS);

	CreateProgramResult(CResultServer::RT_ES_EFF_MIN_HORZ_STRESS,
						IDS_RN_EFF_MIN_HORZ_STRESS,
						IDS_ET_EFF_MIN_HORZ_STRESS,
						IDS_RL_EFF_STRESS,
						RS_ARG_NONE,
						IDS_UNIT_SI_STRESS,
						IDS_UNIT_FIELD_STRESS,
						FF_FACTOR_STRESS);

	CreateProgramResult(CResultServer::RT_ES_DELTA_EFF_VERT_STRESS,
						IDS_RN_DELTA_EFF_VERT_STRESS,
						IDS_ET_DELTA_EFF_VERT_STRESS,
						IDS_RL_EFF_STRESS,
						RS_ARG_NONE,
						IDS_UNIT_SI_STRESS,
						IDS_UNIT_FIELD_STRESS,
						FF_FACTOR_STRESS);

	CreateProgramResult(CResultServer::RT_ES_DELTA_EFF_NN_HORZ_STRESS,
						IDS_RN_DELTA_EFF_NN_HORZ_STRESS,
						IDS_ET_DELTA_EFF_NN_HORZ_STRESS,
						IDS_RL_EFF_STRESS,
						RS_ARG_NONE,
						IDS_UNIT_SI_STRESS,
						IDS_UNIT_FIELD_STRESS,
						FF_FACTOR_STRESS);

	CreateProgramResult(CResultServer::RT_ES_DELTA_EFF_EE_HORZ_STRESS,
						IDS_RN_DELTA_EFF_EE_HORZ_STRESS,
						IDS_ET_DELTA_EFF_EE_HORZ_STRESS,
						IDS_RL_EFF_STRESS,
						RS_ARG_NONE,
						IDS_UNIT_SI_STRESS,
						IDS_UNIT_FIELD_STRESS,
						FF_FACTOR_STRESS);

	CreateProgramResult(CResultServer::RT_ES_DELTA_EFF_MAX_HORZ_STRESS,
						IDS_RN_DELTA_EFF_MAX_HORZ_STRESS,
						IDS_ET_DELTA_EFF_MAX_HORZ_STRESS,
						IDS_RL_EFF_STRESS,
						RS_ARG_NONE,
						IDS_UNIT_SI_STRESS,
						IDS_UNIT_FIELD_STRESS,
						FF_FACTOR_STRESS);

	CreateProgramResult(CResultServer::RT_ES_DELTA_EFF_MIN_HORZ_STRESS,
						IDS_RN_DELTA_EFF_MIN_HORZ_STRESS,
						IDS_ET_DELTA_EFF_MIN_HORZ_STRESS,
						IDS_RL_EFF_STRESS,
						RS_ARG_NONE,
						IDS_UNIT_SI_STRESS,
						IDS_UNIT_FIELD_STRESS,
						FF_FACTOR_STRESS);

// Shear stress
	CreateProgramResult(CResultServer::RT_SS_VERT_N_SHEAR_STRESS,
						IDS_RN_VERT_N_SHEAR_STRESS,
						IDS_ET_VERT_N_SHEAR_STRESS,
						IDS_RL_SHEAR_STRESS,
						RS_ARG_NONE,
						IDS_UNIT_SI_STRESS,
						IDS_UNIT_FIELD_STRESS,
						FF_FACTOR_STRESS);

	CreateProgramResult(CResultServer::RT_SS_VERT_E_SHEAR_STRESS,
						IDS_RN_VERT_E_SHEAR_STRESS,
						IDS_ET_VERT_E_SHEAR_STRESS,
						IDS_RL_SHEAR_STRESS,
						RS_ARG_NONE,
						IDS_UNIT_SI_STRESS,
						IDS_UNIT_FIELD_STRESS,
						FF_FACTOR_STRESS);

	CreateProgramResult(CResultServer::RT_SS_N_E_SHEAR_STRESS,
						IDS_RN_N_E_SHEAR_STRESS,
						IDS_ET_N_E_SHEAR_STRESS,
						IDS_RL_SHEAR_STRESS,
						RS_ARG_NONE,
						IDS_UNIT_SI_STRESS,
						IDS_UNIT_FIELD_STRESS,
						FF_FACTOR_STRESS);

	CreateProgramResult(CResultServer::RT_SS_MAX_HORZ_SHEAR_STRESS,
						IDS_RN_MAX_HORZ_SHEAR_STRESS,
						IDS_ET_MAX_HORZ_SHEAR_STRESS,
						IDS_RL_SHEAR_STRESS,
						RS_ARG_NONE,
						IDS_UNIT_SI_STRESS,
						IDS_UNIT_FIELD_STRESS,
						FF_FACTOR_STRESS);

	CreateProgramResult(CResultServer::RT_SS_MIN_HORZ_SHEAR_STRESS,
						IDS_RN_MIN_HORZ_SHEAR_STRESS,
						IDS_ET_MIN_HORZ_SHEAR_STRESS,
						IDS_RL_SHEAR_STRESS,
						RS_ARG_NONE,
						IDS_UNIT_SI_STRESS,
						IDS_UNIT_FIELD_STRESS,
						FF_FACTOR_STRESS);

	CreateProgramResult(CResultServer::RT_SS_DELTA_VERT_N_SHEAR_STRESS,
						IDS_RN_DELTA_VERT_N_SHEAR_STRESS,
						IDS_ET_DELTA_VERT_N_SHEAR_STRESS,
						IDS_RL_SHEAR_STRESS,
						RS_ARG_NONE,
						IDS_UNIT_SI_STRESS,
						IDS_UNIT_FIELD_STRESS,
						FF_FACTOR_STRESS);

	CreateProgramResult(CResultServer::RT_SS_DELTA_VERT_E_SHEAR_STRESS,
						IDS_RN_DELTA_VERT_E_SHEAR_STRESS,
						IDS_ET_DELTA_VERT_E_SHEAR_STRESS,
						IDS_RL_SHEAR_STRESS,
						RS_ARG_NONE,
						IDS_UNIT_SI_STRESS,
						IDS_UNIT_FIELD_STRESS,
						FF_FACTOR_STRESS);

	CreateProgramResult(CResultServer::RT_SS_DELTA_N_E_SHEAR_STRESS,
						IDS_RN_DELTA_N_E_SHEAR_STRESS,
						IDS_ET_DELTA_N_E_SHEAR_STRESS,
						IDS_RL_SHEAR_STRESS,
						RS_ARG_NONE,
						IDS_UNIT_SI_STRESS,
						IDS_UNIT_FIELD_STRESS,
						FF_FACTOR_STRESS);

	CreateProgramResult(CResultServer::RT_SS_DELTA_MAX_HORZ_SHEAR_STRESS,
						IDS_RN_DELTA_MAX_HORZ_SHEAR_STRESS,
						IDS_ET_DELTA_MAX_HORZ_SHEAR_STRESS,
						IDS_RL_SHEAR_STRESS,
						RS_ARG_NONE,
						IDS_UNIT_SI_STRESS,
						IDS_UNIT_FIELD_STRESS,
						FF_FACTOR_STRESS);

	CreateProgramResult(CResultServer::RT_SS_DELTA_MIN_HORZ_SHEAR_STRESS,
						IDS_RN_DELTA_MIN_HORZ_SHEAR_STRESS,
						IDS_ET_DELTA_MIN_HORZ_SHEAR_STRESS,
						IDS_RL_SHEAR_STRESS,
						RS_ARG_NONE,
						IDS_UNIT_SI_STRESS,
						IDS_UNIT_FIELD_STRESS,
						FF_FACTOR_STRESS);

	CreateProgramResult(CResultServer::RT_SC_SHEAR_CAPACITY,
						IDS_RN_SHEAR_CAPACITY,
						IDS_ET_SHEAR_CAPACITY,
						IDS_RL_SHEAR_CAPACITY,
						RS_ARG_NONE,
						IDS_UNIT_DIMENSIONLESS,
						IDS_UNIT_DIMENSIONLESS,
						FF_FACTOR_DIMENSIONLESS);
						
// Total princial stress
	CreateProgramResult(CResultServer::RT_TPS_TOT_MAX_PRINC_STRESS,
						IDS_RN_TOT_MAX_PRINC_STRESS,
						IDS_ET_TOT_MAX_PRINC_STRESS,
						IDS_RL_TOT_PRINC_STRESS,
						RS_ARG_NONE,
						IDS_UNIT_SI_STRESS,
						IDS_UNIT_FIELD_STRESS,
						FF_FACTOR_STRESS);

	CreateProgramResult(CResultServer::RT_TPS_TOT_MED_PRINC_STRESS,
						IDS_RN_TOT_MED_PRINC_STRESS,
						IDS_ET_TOT_MED_PRINC_STRESS,
						IDS_RL_TOT_PRINC_STRESS,
						RS_ARG_NONE,
						IDS_UNIT_SI_STRESS,
						IDS_UNIT_FIELD_STRESS,
						FF_FACTOR_STRESS);

	CreateProgramResult(CResultServer::RT_TPS_TOT_MIN_PRINC_STRESS,
						IDS_RN_TOT_MIN_PRINC_STRESS,
						IDS_ET_TOT_MIN_PRINC_STRESS,
						IDS_RL_TOT_PRINC_STRESS,
						RS_ARG_NONE,
						IDS_UNIT_SI_STRESS,
						IDS_UNIT_FIELD_STRESS,
						FF_FACTOR_STRESS);

	CreateProgramResult(CResultServer::RT_TPS_DELTA_TOT_MAX_PRINC_STRESS,
						IDS_RN_DELTA_TOT_MAX_PRINC_STRESS,
						IDS_ET_DELTA_TOT_MAX_PRINC_STRESS,
						IDS_RL_TOT_PRINC_STRESS,
						RS_ARG_NONE,
						IDS_UNIT_SI_STRESS,
						IDS_UNIT_FIELD_STRESS,
						FF_FACTOR_STRESS);

	CreateProgramResult(CResultServer::RT_TPS_DELTA_TOT_MED_PRINC_STRESS,
						IDS_RN_DELTA_TOT_MED_PRINC_STRESS,
						IDS_ET_DELTA_TOT_MED_PRINC_STRESS,
						IDS_RL_TOT_PRINC_STRESS,
						RS_ARG_NONE,
						IDS_UNIT_SI_STRESS,
						IDS_UNIT_FIELD_STRESS,
						FF_FACTOR_STRESS);

	CreateProgramResult(CResultServer::RT_TPS_DELTA_TOT_MIN_PRINC_STRESS,
						IDS_RN_DELTA_TOT_MIN_PRINC_STRESS,
						IDS_ET_DELTA_TOT_MIN_PRINC_STRESS,
						IDS_RL_TOT_PRINC_STRESS,
						RS_ARG_NONE,
						IDS_UNIT_SI_STRESS,
						IDS_UNIT_FIELD_STRESS,
						FF_FACTOR_STRESS);

// Effective principal stress
	CreateProgramResult(CResultServer::RT_EPS_EFF_MAX_PRINC_STRESS,
						IDS_RN_EFF_MAX_PRINC_STRESS,
						IDS_ET_EFF_MAX_PRINC_STRESS,
						IDS_RL_EFF_PRINC_STRESS,
						RS_ARG_NONE,
						IDS_UNIT_SI_STRESS,
						IDS_UNIT_FIELD_STRESS,
						FF_FACTOR_STRESS);

	CreateProgramResult(CResultServer::RT_EPS_EFF_MED_PRINC_STRESS,
						IDS_RN_EFF_MED_PRINC_STRESS,
						IDS_ET_EFF_MED_PRINC_STRESS,
						IDS_RL_EFF_PRINC_STRESS,
						RS_ARG_NONE,
						IDS_UNIT_SI_STRESS,
						IDS_UNIT_FIELD_STRESS,
						FF_FACTOR_STRESS);

	CreateProgramResult(CResultServer::RT_EPS_EFF_MIN_PRINC_STRESS,
						IDS_RN_EFF_MIN_PRINC_STRESS,
						IDS_ET_EFF_MIN_PRINC_STRESS,
						IDS_RL_EFF_PRINC_STRESS,
						RS_ARG_NONE,
						IDS_UNIT_SI_STRESS,
						IDS_UNIT_FIELD_STRESS,
						FF_FACTOR_STRESS);

	CreateProgramResult(CResultServer::RT_EPS_DELTA_EFF_MAX_PRINC_STRESS,
						IDS_RN_DELTA_EFF_MAX_PRINC_STRESS,
						IDS_ET_DELTA_EFF_MAX_PRINC_STRESS,
						IDS_RL_EFF_PRINC_STRESS,
						RS_ARG_NONE,
						IDS_UNIT_SI_STRESS,
						IDS_UNIT_FIELD_STRESS,
						FF_FACTOR_STRESS);

	CreateProgramResult(CResultServer::RT_EPS_DELTA_EFF_MED_PRINC_STRESS,
						IDS_RN_DELTA_EFF_MED_PRINC_STRESS,
						IDS_ET_DELTA_EFF_MED_PRINC_STRESS,
						IDS_RL_EFF_PRINC_STRESS,
						RS_ARG_NONE,
						IDS_UNIT_SI_STRESS,
						IDS_UNIT_FIELD_STRESS,
						FF_FACTOR_STRESS);

	CreateProgramResult(CResultServer::RT_EPS_DELTA_EFF_MIN_PRINC_STRESS,
						IDS_RN_DELTA_EFF_MIN_PRINC_STRESS,
						IDS_ET_DELTA_EFF_MIN_PRINC_STRESS,
						IDS_RL_EFF_PRINC_STRESS,
						RS_ARG_NONE,
						IDS_UNIT_SI_STRESS,
						IDS_UNIT_FIELD_STRESS,
						FF_FACTOR_STRESS);

// Pore pressure
	CreateProgramResult(CResultServer::RT_TS_PORE_PRESSURE,
						IDS_RN_PORE_PRESSURE,
						IDS_ET_PORE_PRESSURE,
						IDS_RL_POREPRESSURE,
						RS_ARG_NONE,
						IDS_UNIT_SI_STRESS,
						IDS_UNIT_FIELD_STRESS,
						FF_FACTOR_PRESSURE);
// wed:TODO Stress in fault
CreateProgramResult(CResultServer::RT_SF_SHEAR_STRESS_FAULT,
						IDS_RN_SHEAR_STRESS_FAULT,
						IDS_ET_SHEAR_STRESS_FAULT,
						IDS_RL_FAULT,
						RS_ARG_NONE,
						IDS_UNIT_SI_STRESS,
						IDS_UNIT_FIELD_STRESS,
						FF_FACTOR_STRESS);

CreateProgramResult(CResultServer::RT_SF_NORM_STRESS_FAULT,
						IDS_RN_NORM_STRESS_FAULT,
						IDS_ET_NORM_STRESS_FAULT,
						IDS_RL_FAULT,
						RS_ARG_NONE,
						IDS_UNIT_SI_STRESS,
						IDS_UNIT_FIELD_STRESS,
						FF_FACTOR_STRESS);

CreateProgramResult(CResultServer::RT_SF_DELTA_SHEAR_STRESS_FAULT,
						IDS_RN_DELTA_SHEAR_STRESS_FAULT,
						IDS_ET_DELTA_SHEAR_STRESS_FAULT,
						IDS_RL_FAULT,
						RS_ARG_NONE,
						IDS_UNIT_SI_STRESS,
						IDS_UNIT_FIELD_STRESS,
						FF_FACTOR_STRESS);

CreateProgramResult(CResultServer::RT_SF_DELTA_NORM_STRESS_FAULT,
						IDS_RN_DELTA_NORM_STRESS_FAULT,
						IDS_ET_DELTA_NORM_STRESS_FAULT,
						IDS_RL_FAULT,
						RS_ARG_NONE,
						IDS_UNIT_SI_STRESS,
						IDS_UNIT_FIELD_STRESS,
						FF_FACTOR_STRESS);

CreateProgramResult(CResultServer::RT_SF_SHEAR_CAPACITY_FAULT,
						IDS_RN_SHEAR_CAPACITY_FAULT,
						IDS_ET_SHEAR_CAPACITY_FAULT,
						IDS_RL_FAULT,
						RS_ARG_NONE,
						IDS_UNIT_DIMENSIONLESS,
						IDS_UNIT_DIMENSIONLESS,
						FF_FACTOR_DIMENSIONLESS);

CreateProgramResult(CResultServer::RT_SF_SLIP_FAULT,
						IDS_RN_SLIP_FAULT,
						IDS_ET_SLIP_FAULT,
						IDS_RL_FAULT,
						RS_ARG_NONE,
						IDS_UNIT_SI_LENGTH,
						IDS_UNIT_FIELD_LENGTH,
						FF_FACTOR_LENGTH);

// Gamma  // Temporary disabled
/*
	CreateProgramResult(CResultServer::RT_GM_GAMMA_VERT,
						IDS_RN_GAMMA_VERT,
						IDS_ET_GAMMA_VERT,
						IDS_RL_GAMMA,
						RS_ARG_NONE,
						IDS_UNIT_DIMENSIONLESS,
						IDS_UNIT_DIMENSIONLESS,
						FF_FACTOR_DIMENSIONLESS);

	CreateProgramResult(CResultServer::RT_GM_GAMMA_MAX_HORZ,
						IDS_RN_GAMMA_MAX_HORZ,
						IDS_ET_GAMMA_MAX_HORZ,
						IDS_RL_GAMMA,
						RS_ARG_NONE,
						IDS_UNIT_DIMENSIONLESS,
						IDS_UNIT_DIMENSIONLESS,
						FF_FACTOR_DIMENSIONLESS);

	CreateProgramResult(CResultServer::RT_GM_GAMMA_MIN_HORZ,
						IDS_RN_GAMMA_MIN_HORZ,
						IDS_ET_GAMMA_MIN_HORZ,
						IDS_RL_GAMMA,
						RS_ARG_NONE,
						IDS_UNIT_DIMENSIONLESS,
						IDS_UNIT_DIMENSIONLESS,
						FF_FACTOR_DIMENSIONLESS);

	CreateProgramResult(CResultServer::RT_GM_GAMMA_MAX_PRINC_STRESS,
						IDS_RN_GAMMA_MAX_PRINC_STRESS,
						IDS_ET_GAMMA_MAX_PRINC_STRESS,
						IDS_RL_GAMMA,
						RS_ARG_NONE,
						IDS_UNIT_DIMENSIONLESS,
						IDS_UNIT_DIMENSIONLESS,
						FF_FACTOR_DIMENSIONLESS);

	CreateProgramResult(CResultServer::RT_GM_GAMMA_MED_PRINC_STRESS,
						IDS_RN_GAMMA_MED_PRINC_STRESS,
						IDS_ET_GAMMA_MED_PRINC_STRESS,
						IDS_RL_GAMMA,
						RS_ARG_NONE,
						IDS_UNIT_DIMENSIONLESS,
						IDS_UNIT_DIMENSIONLESS,
						FF_FACTOR_DIMENSIONLESS);

	CreateProgramResult(CResultServer::RT_GM_GAMMA_MIN_PRINC_STRESS,
						IDS_RN_GAMMA_MIN_PRINC_STRESS,
						IDS_ET_GAMMA_MIN_PRINC_STRESS,
						IDS_RL_GAMMA,
						RS_ARG_NONE,
						IDS_UNIT_DIMENSIONLESS,
						IDS_UNIT_DIMENSIONLESS,
						FF_FACTOR_DIMENSIONLESS);
*/
// Compaction strain
	CreateProgramResult(CResultServer::RT_CS_VERT_COMP_STRN,
						IDS_RN_VERT_COMP_STRN,
						IDS_ET_VERT_COMP_STRN,
						IDS_RL_COMP_STRAIN,
						RS_ARG_NONE,
						IDS_UNIT_DIMENSIONLESS,
						IDS_UNIT_DIMENSIONLESS,
						FF_FACTOR_DIMENSIONLESS);

	CreateProgramResult(CResultServer::RT_CS_N_COMP_STRN,
						IDS_RN_N_COMP_STRN,
						IDS_ET_N_COMP_STRN,
						IDS_RL_COMP_STRAIN,
						RS_ARG_NONE,
						IDS_UNIT_DIMENSIONLESS,
						IDS_UNIT_DIMENSIONLESS,
						FF_FACTOR_DIMENSIONLESS);

	CreateProgramResult(CResultServer::RT_CS_E_COMP_STRN,
						IDS_RN_E_COMP_STRN,
						IDS_ET_E_COMP_STRN,
						IDS_RL_COMP_STRAIN,
						RS_ARG_NONE,
						IDS_UNIT_DIMENSIONLESS,
						IDS_UNIT_DIMENSIONLESS,
						FF_FACTOR_DIMENSIONLESS);

	CreateProgramResult(CResultServer::RT_CS_MAX_HORZ_COMP_STRN,
						IDS_RN_MAX_HORZ_COMP_STRN,
						IDS_ET_MAX_HORZ_COMP_STRN,
						IDS_RL_COMP_STRAIN,
						RS_ARG_NONE,
						IDS_UNIT_DIMENSIONLESS,
						IDS_UNIT_DIMENSIONLESS,
						FF_FACTOR_DIMENSIONLESS);

	CreateProgramResult(CResultServer::RT_CS_MIN_HORZ_COMP_STRN,
						IDS_RN_MIN_HORZ_COMP_STRN,
						IDS_ET_MIN_HORZ_COMP_STRN,
						IDS_RL_COMP_STRAIN,
						RS_ARG_NONE,
						IDS_UNIT_DIMENSIONLESS,
						IDS_UNIT_DIMENSIONLESS,
						FF_FACTOR_DIMENSIONLESS);

	CreateProgramResult(CResultServer::RT_CS_MAX_PRINC_COMP_STRN,
						IDS_RN_MAX_PRINC_COMP_STRN,
						IDS_ET_MAX_PRINC_COMP_STRN,
						IDS_RL_COMP_STRAIN,
						RS_ARG_NONE,
						IDS_UNIT_DIMENSIONLESS,
						IDS_UNIT_DIMENSIONLESS,
						FF_FACTOR_DIMENSIONLESS);

	CreateProgramResult(CResultServer::RT_CS_MED_PRINC_COMP_STRN,
						IDS_RN_MED_PRINC_COMP_STRN,
						IDS_ET_MED_PRINC_COMP_STRN,
						IDS_RL_COMP_STRAIN,
						RS_ARG_NONE,
						IDS_UNIT_DIMENSIONLESS,
						IDS_UNIT_DIMENSIONLESS,
						FF_FACTOR_DIMENSIONLESS);

	CreateProgramResult(CResultServer::RT_CS_MIN_PRINC_COMP_STRN,
						IDS_RN_MIN_PRINC_COMP_STRN,
						IDS_ET_MIN_PRINC_COMP_STRN,
						IDS_RL_COMP_STRAIN,
						RS_ARG_NONE,
						IDS_UNIT_DIMENSIONLESS,
						IDS_UNIT_DIMENSIONLESS,
						FF_FACTOR_DIMENSIONLESS);

	CreateProgramResult(CResultServer::RT_CS_VOL_COMP_STRN,
						IDS_RN_VOL_COMP_STRN,
						IDS_ET_VOL_COMP_STRN,
						IDS_RL_COMP_STRAIN,
						RS_ARG_NONE,
						IDS_UNIT_DIMENSIONLESS,
						IDS_UNIT_DIMENSIONLESS,
						FF_FACTOR_DIMENSIONLESS);

// Shear strain
	CreateProgramResult(CResultServer::RT_CS_VERT_N_SHEAR_STRAIN,
						IDS_RN_VERT_N_SHEAR_STRN,
						IDS_ET_VERT_N_SHEAR_STRN,
						IDS_RL_SHEAR_STRAIN,
						RS_ARG_NONE,
						IDS_UNIT_DIMENSIONLESS,
						IDS_UNIT_DIMENSIONLESS,
						FF_FACTOR_DIMENSIONLESS);

	CreateProgramResult(CResultServer::RT_CS_VERT_E_SHEAR_STRAIN,
						IDS_RN_VERT_E_SHEAR_STRN,
						IDS_ET_VERT_E_SHEAR_STRN,
						IDS_RL_SHEAR_STRAIN,
						RS_ARG_NONE,
						IDS_UNIT_DIMENSIONLESS,
						IDS_UNIT_DIMENSIONLESS,
						FF_FACTOR_DIMENSIONLESS);

	CreateProgramResult(CResultServer::RT_CS_N_E_SHEAR_STRAIN,
						IDS_RN_N_E_SHEAR_STRN,
						IDS_ET_N_E_SHEAR_STRN,
						IDS_RL_SHEAR_STRAIN,
						RS_ARG_NONE,
						IDS_UNIT_DIMENSIONLESS,
						IDS_UNIT_DIMENSIONLESS,
						FF_FACTOR_DIMENSIONLESS);

// wed:TODO find solution for availability of nonlin results in linear mode
// Plastic strain

	CreateProgramResult(CResultServer::RT_PS_PLAS_STRN_I,
						IDS_RN_PLAS_STRN_I,
						IDS_ET_PLAS_STRN_I,
						IDS_RL_PLASTIC_STRAIN,
						RS_ARG_NONE,
						IDS_UNIT_DIMENSIONLESS,
						IDS_UNIT_DIMENSIONLESS,
						FF_FACTOR_DIMENSIONLESS);

	CreateProgramResult(CResultServer::RT_PS_PLAS_STRN_II,
						IDS_RN_PLAS_STRN_II,
						IDS_ET_PLAS_STRN_II,
						IDS_RL_PLASTIC_STRAIN,
						RS_ARG_NONE,
						IDS_UNIT_DIMENSIONLESS,
						IDS_UNIT_DIMENSIONLESS,
						FF_FACTOR_DIMENSIONLESS);

// Subsidence
	CreateProgramResult(CResultServer::RT_SD_SUBSIDENCE,
						IDS_RN_SUBSIDENCE,
						IDS_ET_SUBSIDENCE,
						IDS_RL_DISPLACEMENTS,
						RS_ARG_NONE,
						IDS_UNIT_SI_LENGTH,
						IDS_UNIT_FIELD_LENGTH,
						FF_FACTOR_LENGTH);

// Horizontal displacement
	CreateProgramResult(CResultServer::RT_HD_HOR_DISP,
						IDS_RN_HOR_DISP,
						IDS_ET_HOR_DISP,
						IDS_RL_DISPLACEMENTS,
						RS_ARG_NONE,
						IDS_UNIT_SI_LENGTH,
						IDS_UNIT_FIELD_LENGTH,
						FF_FACTOR_LENGTH);

	LinkTo(server);
}

CResultEntry::~CResultEntry()
{

}

void CResultEntry::CreateProgramResult(CResultServer::TResultType uResultID,
									   UINT uResultName,
									   UINT uResultLabel,
									   UINT uBranchName,
									   UINT uClassType,
									   UINT uSIUnit,
									   UINT uFieldUnit,
									   const double &dFieldFactor)
{
	ASSERT((int) uResultID < 1000);
	CProgramResult *pResult = 0;

	switch(uClassType)
	{
	case RS_ARG_NONE:
		pResult = new CProgramResult(*this,
									 uResultID,
									 uResultName,
									 uResultLabel,
									 uBranchName,
									 uSIUnit,
									 uFieldUnit,
									 dFieldFactor);
		break;
	default:
		ASSERT(FALSE);
		break;
	}

	m_mpResult.insert(TResultMap::value_type(uResultID, pResult));

}

const CProgramResult& CResultEntry::Result(UINT uResultID) const
{
	TResultMap::const_iterator it = m_mpResult.find(uResultID);
	ASSERT(it != m_mpResult.end());
	return *it->second;
}

CResultServer& CResultEntry::Server()
{
	return m_server;
}

const CResultServer& CResultEntry::Server() const
{
	return m_server;
}

const CModelBase& CResultEntry::Model() const
{
	return (const CModelBase&) Server().Model();
}

CModelBase& CResultEntry::Model()
{
	return (CModelBase&) Server().Model();
}

CResultEntry::TWeightingType CResultEntry::WeightingType() const
{
	return m_weighting_type;
}

void CResultEntry::WeightingType(TWeightingType type)
{
	if(m_weighting_type != type)
	{
		m_weighting_type = type;
		Modified();
	}
}

void CResultEntry::AppendMenu(CMenu& menu)
{
	menu.AppendMenu(MF_STRING, ID_CONTEXT_COPY, _T("&Copy"));
	menu.AppendMenu(MF_STRING, ID_CONTEXT_PASTE, _T("&Past"));
	// Append export
	menu.AppendMenu(MF_SEPARATOR);
	// Append attributes ...
	menu.AppendMenu(MF_STRING, ID_ITEM_ATTRIBUTES, _T("&Attributes"));
} 
	
void CResultEntry::Properties()
{
	CAttriResultEntry dlg(*this);
	dlg.DoModal();
}

void CResultEntry::OnNeighbourModified(const CGraphNode& node, UINT uHint)
{
	if(&node == &m_server)
		Modified();

	CGraphEntryTemp<IResult>::OnNeighbourModified(node, uHint);
}

const CProgramResult& CResultEntry::Result(CResultServer::TResultType uResultID) const
{
	const CProgramResult* pResult = 0;
	CResultEntry::TNodeSet stNode = EntryNodes();
	for(CResultEntry::TNodeSet::const_iterator it = stNode.begin(); it != stNode.end(); it++)
	{
		pResult = dynamic_cast<const CProgramResult*>(*it);
		if(pResult)
		{
			if(pResult->ResultID() == uResultID)
				return *pResult;
		}
	}

	ASSERT(FALSE);

	return *pResult;
}