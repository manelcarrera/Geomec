#ifndef _DIANACORE_H_
#define _DIANACORE_H_

// Loads/h
#include "IDCLoad.h"
#include "IDCStructuralLoad.h"
#include "DCPressureLoad.h"
#include "DCTemperatureLoad.h"
#include "DCInitialStressLoad.h"
#include "IDCDirectedLoad.h"
#include "IDCSupportLoad.h"
#include "DCFaceMoment.h"
#include "DCLineMoment.h"
#include "DCPointMoment.h"
#include "DCPrescribedRotation.h"
#include "DCBodyFaceLoad.h"
#include "DCFaceLoad.h"
#include "DCLineLoad.h"
#include "DCPointLoad.h"
#include "DCPrescribedDisplacement.h"
#include "DCWeightLoad.h"
#include "DCFaceEdgeLoad.h"
#include "IDCFlowLoad.h"
#include "DCInitialNodalPotential.h"
#include "DCNodalTemperature.h"
#include "DCPrescribedPressurePotential.h"
#include "DCConcentrationLoad.h"

#include "IDCCase.h"
#include "DCLoadCase.h"
#include "DCLoadCombination.h"
#include "DCLoadManager.h"
#include "DCBoundaCase.h"

// h
#include "IDCElementProperty.h"
#include "IDCDianaRunner.h"
#include "IDCAnalysisLogger.h"
#include "IDCValueTable.h"
#include "IDCTemperatureTable.h"
#include "IDCConcentrationTable.h"
#include "DCFixedPotential.h"
#include "IDCTimeTable.h"
#include "DCTimeBoundaTable.h"
#include "DCTimeLoadTable.h"


#endif // _DIANACORE_H_
