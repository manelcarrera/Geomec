#ifndef _DIANACORE_H_
#define _DIANACORE_H_

// Loads/h
#include "DCBodyFaceLoad.h"
#include "DCConcentrationLoad.h"
#include "DCFaceEdgeLoad.h"
#include "DCFaceLoad.h"
#include "DCFaceMoment.h"
#include "DCInitialNodalPotential.h"
#include "DCInitialStressLoad.h"
#include "DCLineLoad.h"
#include "DCLineMoment.h"
#include "DCNodalTemperature.h"
#include "DCPointLoad.h"
#include "DCPointMoment.h"
#include "DCPrescribedDisplacement.h"
#include "DCPrescribedPressurePotential.h"
#include "DCPrescribedRotation.h"
#include "DCPressureLoad.h"
#include "DCTemperatureLoad.h"
#include "DCWeightLoad.h"
#include "IDCDirectedLoad.h"
#include "IDCFlowLoad.h"
#include "IDCLoad.h"
#include "IDCStructuralLoad.h"
#include "IDCSupportLoad.h"

#include "DCBoundaCase.h"
#include "DCLoadCase.h"
#include "DCLoadCombination.h"
#include "DCLoadManager.h"
#include "IDCCase.h"

// h
#include "DCFixedPotential.h"
#include "DCTimeBoundaTable.h"
#include "DCTimeLoadTable.h"
#include "IDCAnalysisLogger.h"
#include "IDCConcentrationTable.h"
#include "IDCDianaRunner.h"
#include "IDCElementProperty.h"
#include "IDCTemperatureTable.h"
#include "IDCTimeTable.h"
#include "IDCValueTable.h"

#endif // _DIANACORE_H_
