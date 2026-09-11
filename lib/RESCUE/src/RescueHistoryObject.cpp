/********************************************************************

  RescueHistoryObject.h

  This abstract object contains the behaviour for RescueObjects that
  can be added to a RescueHistor record.

  This abstract superclass can be instantiated as one of it's subclasses.

  Rod Hanks,  June, 1999

*********************************************************************/
#include "myHeaders.h"
#include "RescueHistoryObject.h"
#include "cBagRescueHistory.h"
#include "RescueHistory.h"

RescueHistoryObject::~RescueHistoryObject()
{
  if (objectChanges != 0)
  {
  RescueHistory *history = objectChanges->NthObject(0);
  while (history != 0)
  {
      history->RemoveObjectChanged(this);
      history = objectChanges->NthObject(0);
  }
  delete objectChanges;
  }
  if (objectRelated != 0)
  {
  RescueHistory *history = objectRelated->NthObject(0);
  while (history != 0)
  {
      history->RemoveRelatedObject(this);
      history = objectRelated->NthObject(0);
  }
  delete objectRelated;
  }
}

RescueHistory *RescueHistoryObject::NthObjectChanges(RESCUEINT64 zeroBasedOrdinal)
{
  if (objectChanges == 0)
  {
  return 0;
  }
  else
  {
  return objectChanges->NthObject(zeroBasedOrdinal);
  }
}

RescueHistory *RescueHistoryObject::NthRelatedChanges(RESCUEINT64 zeroBasedOrdinal)
{
  if (objectRelated == 0)
  {
  return 0;
  }
  else
  {
  return objectRelated->NthObject(zeroBasedOrdinal);
  }
}

void RescueHistoryObject::AddObjectChanged(RescueHistory *history)
{
  if (objectChanges == 0)
  {
  objectChanges = new cBagRescueHistory();
  }
  (*objectChanges) += history;
}

void RescueHistoryObject::AddRelatedChange(RescueHistory *history)
{
  if (objectRelated == 0)
  {
  objectRelated = new cBagRescueHistory();
  }
  (*objectRelated) += history;
}

void RescueHistoryObject::RemoveObjectChanged(RescueHistory *history)
{
  if (objectChanges != 0)
  {
  (*objectChanges) -= history;
  }
}

void RescueHistoryObject::RemoveRelatedChange(RescueHistory *history)
{
  if (objectRelated != 0)
  {
  (*objectRelated) -= history;
  }
}

RESCUEBOOL RescueHistoryObject::IsOfType(_RescueObjectType thisType)
{
  if (thisType == R_RescueHistoryObject)
  {
  return TRUE;
  }
  else
  {
  return RescueObject::IsOfType(thisType);
  }
}




