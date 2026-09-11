// Copyright 2017, Shell Global Solutions International B.V.
// All rights reserved. This document and the data and information contained herein is CONFIDENTIAL.
// Neither the whole nor any part of this document may be copied, modified or distributed in any
// form without the prior written consent of the copyright owner.

#include "RGFailure.h"

using namespace RGFailure;


RGLimitStateFunctionEvaluate::RGLimitStateFunctionEvaluate()
{
  failureMechanism = std::make_unique<RGFailureMechanism>();
  depletionStage = 0; 
  threshold = 0.0;
  limitStateFunctionValue = 0.0;
}

RGLimitStateFunctionEvaluate::RGLimitStateFunctionEvaluate(RGFailureMechanism source)
{
  failureMechanism = std::make_unique<RGFailureMechanism>();
  failureMechanism->objectName = std::string(source.objectName);
  failureMechanism->object = source.object;
  failureMechanism->failureType = source.failureType;
  failureMechanism->lsfType = source.lsfType;
  failureMechanism->value = std::vector<double>(source.value);
  depletionStage = 0;
  threshold = 0.0;
  limitStateFunctionValue = 0.0;
}

RGLimitStateFunctionEvaluate::~RGLimitStateFunctionEvaluate()
{
  RGFailureMechanism* failPtr = failureMechanism.release();
  delete failPtr;
}

RGLimitStateFunctionEvaluate::RGLimitStateFunctionEvaluate(const RGLimitStateFunctionEvaluate & source)
{
  failureMechanism = std::make_unique<RGFailureMechanism>();
  failureMechanism->objectName = std::string(source.failureMechanism->objectName);
  failureMechanism->object = source.failureMechanism->object;
  failureMechanism->failureType = source.failureMechanism->failureType;
  failureMechanism->lsfType = source.failureMechanism->lsfType;
  failureMechanism->value = std::vector<double>(source.failureMechanism->value);

  depletionStage = source.depletionStage;
  threshold = source.threshold;
  limitStateFunctionValue = source.limitStateFunctionValue;	
}

RGLimitStateFunctionEvaluate& RGLimitStateFunctionEvaluate::operator=(const RGLimitStateFunctionEvaluate & source)
{
  failureMechanism = std::make_unique<RGFailureMechanism>();
  failureMechanism->objectName = std::string(source.failureMechanism->objectName);
  failureMechanism->object = source.failureMechanism->object;
  failureMechanism->failureType = source.failureMechanism->failureType;
  failureMechanism->lsfType = source.failureMechanism->lsfType;
  failureMechanism->value = std::vector<double>(source.failureMechanism->value);

  depletionStage = source.depletionStage;
  threshold = source.threshold;
  limitStateFunctionValue = source.limitStateFunctionValue;

  return *this;
}

void RGLimitStateFunctionEvaluate::SetFailureMechanism(RGFailureMechanism & source)
{
  failureMechanism->objectName = std::string(source.objectName);
  failureMechanism->object = source.object;
  failureMechanism->failureType = source.failureType;
  failureMechanism->lsfType = source.lsfType;
  failureMechanism->value = source.value;
}


const std::string c_RGFailureMechamismObject2String[] = { "Fault", "Formation", "Well", "TopSurface", "Property", "UndefinedObject" };
const std::string c_RGFailureMechanismFailureTypes2String[] = { "FaultShearCapacity", "FaultDisplacementL", "ShearCapacity", "EPStress1L",  "EPStress3L", "PStrain1L", "PStrain3L", "StrainInvVolumetric", "PStrain1PlasticL", "PStrain3PlasticL", "CompactionCapacity", "WPAxialStrain", "DisplacementV", "UndefinedFailureType" };
const std::string c_RGFailureMechanismLsfType2String[] = { "Minimum", "Maximum", "MeasureWeightedAverage", "MeasureOfMinimumThresholdExceeded", "MeasureOfMaximumThresholdExceeded", "FractionOfMinimumThresholdExceeded", "FractionOfMaximumThresholdExceeded", "UndefinedLsfType" };


std::string FailureMechanismObjectToString(const RGFailureMechamismObject failureObject)
{
  return c_RGFailureMechamismObject2String[failureObject];
}

RGFailureMechamismObject FailureMechanismObjectFromString(const std:: string& str)
{
  RGFailureMechamismObject result = UndefinedObject;
  for (size_t objType = 0; objType < UndefinedObject; ++objType)
  {
    if (c_RGFailureMechamismObject2String[objType].compare(str) == 0)
    {
      result = static_cast<RGFailureMechamismObject>(objType);
    }
  }
  return result;
}

std::string FailureMechanismFailureTypeToString(const RGFailureMechanismFailureTypes failureType)
{
  return c_RGFailureMechanismFailureTypes2String[failureType];
}

RGFailureMechanismFailureTypes FailureMechanismFailureTypeFromString(const std::string& str)
{
  RGFailureMechanismFailureTypes result = UndefinedFailureType;
  for (size_t objType = 0; objType < UndefinedFailureType; ++objType)
  {
    if (c_RGFailureMechanismFailureTypes2String[objType].compare(str) == 0)
    {
      result = static_cast<RGFailureMechanismFailureTypes>(objType);
    }
  }
  return result;
}

std::string FailureMechanismLsfTypeToString(const RGFailureMechanismLsfType lsfType)
{
  return c_RGFailureMechanismLsfType2String[lsfType];
}

RGFailureMechanismLsfType RGFailureMechanismLsfTypeFromString(const std::string& str)
{
  RGFailureMechanismLsfType result = UndefinedLsfType;
  for (size_t objType = 0; objType < UndefinedLsfType; ++objType)
  {
    if (c_RGFailureMechanismLsfType2String[objType].compare(str) == 0)
    {
      result = static_cast<RGFailureMechanismLsfType>(objType);
    }
  }
  return result;
}
