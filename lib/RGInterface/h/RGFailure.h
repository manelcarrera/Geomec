// Copyright 2017, Shell Global Solutions International B.V.
// All rights reserved. This document and the data and information contained herein is CONFIDENTIAL.
// Neither the whole nor any part of this document may be copied, modified or distributed in any
// form without the prior written consent of the copyright owner.


#ifndef RG_FAILURE_MECHANISM
#define RG_FAILURE_MECHANISM

#include "RGInterfaceExport.h"

#include <string>
#include <vector>
#include <memory>

// if we want to set the LSF at the
// last depletion stage we have to use
// the following flag
#define LAST_DEPLETION_STAGE -1

namespace RGFailure
{
	// The object is the actual physical object 
	enum RGFailureMechamismObject
	{
		Fault = 0,
		Formation,
		Well,
		TopSurface,
		Property,
		UndefinedObject
	};

	// this is the failure mechanism type
	// in contrast to the LSF type below
	enum RGFailureMechanismFailureTypes
	{
		FaultShearCapacity = 0,
		FaultDisplacementL,
		ShearCapacity,
		EPStress1L,
		EPStress3L,
		PStrain1L,
		PStrain3L,
		StrainInvVolumetric,
		PStrain1PlasticL,
		PStrain3PlasticL,
		CompactionCapacity,
		WPAxialStrain,
		DisplacementV,
		UndefinedFailureType
	};

	// The LSF type
	enum RGFailureMechanismLsfType
	{
		Minimum = 0,
		Maximum,

		MeasureWeightedAverage,

		MeasureOfMinimumThresholdExceeded,
		MeasureOfMaximumThresholdExceeded,
		FractionOfMinimumThresholdExceeded,
		FractionOfMaximumThresholdExceeded,

		UndefinedLsfType
	};
}

class INTERFACE_RGINTERFACE RGFailureMechanism
{
public:
	std::string                                objectName;
	RGFailure::RGFailureMechamismObject        object;
	RGFailure::RGFailureMechanismFailureTypes  failureType;
	RGFailure::RGFailureMechanismLsfType       lsfType;
	std::vector<double>                         value;
};

// Evaluate for a specified instance and threshold
class INTERFACE_RGINTERFACE RGLimitStateFunctionEvaluate
{
public:
	std::unique_ptr<RGFailureMechanism> failureMechanism;
	int                       depletionStage; // std::numeric_limits<int>::max() means last one?
	double                    threshold;       // Optional: failureMechanism.lsfType >= MeasureOfMinimumThresholdExceeded
	double                    limitStateFunctionValue; // the value of the LSF

	RGLimitStateFunctionEvaluate();
	RGLimitStateFunctionEvaluate(RGFailureMechanism );
	~RGLimitStateFunctionEvaluate();
	RGLimitStateFunctionEvaluate(const RGLimitStateFunctionEvaluate &);
	RGLimitStateFunctionEvaluate& operator=(const RGLimitStateFunctionEvaluate &);
	void SetFailureMechanism(RGFailureMechanism  &);
};

// utility methods for conversion
std::string INTERFACE_RGINTERFACE FailureMechanismObjectToString(const RGFailure::RGFailureMechamismObject);
RGFailure::RGFailureMechamismObject INTERFACE_RGINTERFACE FailureMechanismObjectFromString(const std::string&);

std::string INTERFACE_RGINTERFACE FailureMechanismFailureTypeToString(const RGFailure::RGFailureMechanismFailureTypes);
RGFailure::RGFailureMechanismFailureTypes INTERFACE_RGINTERFACE FailureMechanismFailureTypeFromString(const std::string&);

std::string INTERFACE_RGINTERFACE FailureMechanismLsfTypeToString(const RGFailure::RGFailureMechanismLsfType);
RGFailure::RGFailureMechanismLsfType INTERFACE_RGINTERFACE RGFailureMechanismLsfTypeFromString(const std::string&);

#endif
