/********************************************************************

  RescueWellboreProperty.cpp

  Property for wellbores RESCUE's data model.  Attach to 

  Rod Hanks,  May 1996

*********************************************************************/
#include "RescueGrid.h"
#include "RescueWellboreProperty.h"
#include "RescueWellboreSampling.h"
#include "RescueArrayFloat.h"
#include "RescueArrayInt.h"
#include "RescueArrayByte.h"
#include "RescueArrayShort.h"
#include "RescueModel.h"
#include "RescueIdTree.h"
#include "RescueBuffer.h"
#include <string.h>

RescueWellboreProperty::RescueWellboreProperty(RescueWellboreSampling *parentSamplingIn,
                                               const RESCUECHAR *propertyName,
                                               const RESCUECHAR *propertyType,
                                               const RESCUECHAR *unitOfMeasure,
                                               RESCUEUCHAR nullValue,
                                               RescueLookup *lookupIn,
                                               RESCUEUCHAR *values)
                                               :RescueHistoryObject(parentSamplingIn->ParentWellbore()->ParentModel()->Context())
                                               ,parentSampling(parentSamplingIn)
{
  isA = R_RescueWellboreProperty;
  (*parentSampling->properties) += this;
  data = new RescueArrayByte(propertyName, propertyType, unitOfMeasure,
                               parentSampling->SampleGrid(), 
                               nullValue, lookupIn,
                               parentSampling->ParentWellbore()->ParentModel(),
                               FALSE, values); 
}

RescueWellboreProperty::RescueWellboreProperty(RescueWellboreSampling *parentSamplingIn,
                                               const RESCUECHAR *propertyName,
                                               const RESCUECHAR *propertyType,
                                               const RESCUECHAR *unitOfMeasure,
                                               RESCUEUSHORT nullValue,
                                               RescueLookup *lookupIn,
                                               RESCUEUSHORT *values)
                                               :RescueHistoryObject(parentSamplingIn->ParentWellbore()->ParentModel()->Context())
                                               ,parentSampling(parentSamplingIn)
{
  isA = R_RescueWellboreProperty;
  (*parentSampling->properties) += this;
  data = new RescueArrayShort(propertyName, propertyType, unitOfMeasure,
                               parentSampling->SampleGrid(), 
                               nullValue, lookupIn, 
                               parentSampling->ParentWellbore()->ParentModel(),
                               FALSE, values); 
}

RescueWellboreProperty::RescueWellboreProperty(RescueWellboreSampling *parentSamplingIn,
                                               const RESCUECHAR *propertyName,
                                               const RESCUECHAR *propertyType,
                                               const RESCUECHAR *unitOfMeasure,
                                               RESCUEFLOAT nullValue,
                                               RESCUEFLOAT *values)
                                               :RescueHistoryObject(parentSamplingIn->ParentWellbore()->ParentModel()->Context())
                                               ,parentSampling(parentSamplingIn)
{
  isA = R_RescueWellboreProperty;
  (*parentSampling->properties) += this;
  data = new RescueArrayFloat(propertyName, propertyType, unitOfMeasure,
                               parentSamplingIn->SampleGrid(), 
                               nullValue, 
                               parentSampling->ParentWellbore()->ParentModel(),
                               FALSE, values); 
}

RescueWellboreProperty::RescueWellboreProperty(RescueWellboreSampling *parentSamplingIn,
                                               const RESCUECHAR *propertyName,
                                               const RESCUECHAR *propertyType,
                                               const RESCUECHAR *unitOfMeasure,
                                               RESCUEINT32 nullValue,
                                               RESCUEINT32 *values)
                                               :RescueHistoryObject(parentSamplingIn->ParentWellbore()->ParentModel()->Context())
                                               ,parentSampling(parentSamplingIn)
{
  isA = R_RescueWellboreProperty;
  (*parentSampling->properties) += this;
  data = new RescueArrayInt(propertyName, propertyType, unitOfMeasure,
                               parentSamplingIn->SampleGrid(), 
                               nullValue, 
                               parentSampling->ParentWellbore()->ParentModel(),
                               FALSE, values); 
}

RescueWellboreProperty::RescueWellboreProperty(RescueContext *context, FILE *archiveFile)
                                      :RescueHistoryObject(context)
                                      ,parentSampling(0)
{
  if (context->ReadFileVersion() >= 5)
  {
    RESCUEINT64 flag;
    if (context->ReadFileVersion() >= 20)
    {
      ReadId(context, archiveFile);
    }
    myfscanf(context, archiveFile, &flag);
    switch (flag)
    {
    case R_RescueArrayByte:
      data = new RescueArrayByte(context, archiveFile);
      break;
    case R_RescueArrayShort:
      data = new RescueArrayShort(context, archiveFile);
      break;
    case R_RescueArrayInt:
      data = new RescueArrayInt(context, archiveFile);
      break;
    default:
      data = new RescueArrayFloat(context, archiveFile);
      break;
    }
  }
  else
  {
    data = new RescueArrayFloat(context, archiveFile);
  }
  isA = R_RescueWellboreProperty;
  context->wellboreProperties->Add(this);
  if (context->ReadFileVersion() >= 37)
  {
    RESCUECHAR myString[255];

    myfgets(context, myString, 255, archiveFile);
    while (strcmp(myString, "EOD") != 0)
    {
      RescueBuffer buf(context, archiveFile);
      myfgets(context, myString, 255, archiveFile);
    }
  }
}

void RescueWellboreProperty::Relink(RescueObject *parentObject)
{
  parentSampling = (RescueWellboreSampling *) parentObject;
  data->SetGrid(parentSampling->SampleGrid());
  data->Relink(parentSampling->ParentWellbore()->ParentModel());
}

void RescueWellboreProperty::Archive(FILE *archiveFile)
{
  RescueContext *context = ParentWellbore()->ParentModel()->Context();
  myfprintf(context, archiveFile, "; Wellbore Property");
  if (context->FileVersion() >= 20)
  {
    myfprintf(context, archiveFile, Identifier());
  }
  myfprintf(context, archiveFile, (RESCUEINT64) data->IsA());
  data->Archive(archiveFile);
  if (context->FileVersion() >= 37)
  {
    myfprintf(context, archiveFile, "EOD");
  }
}

RESCUEBOOL RescueWellboreProperty::IsOfType(_RescueObjectType thisType)
{
  if (thisType == R_RescueWellboreProperty)
  {
    return TRUE;
  }
  else
  {
    return RescueHistoryObject::IsOfType(thisType);
  }
}



