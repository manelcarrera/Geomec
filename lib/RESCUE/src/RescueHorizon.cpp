/********************************************************************

  RescueHorizon.cpp

  The horizon object for RESCUE's data model.

  Rod Hanks,  May 1996

*********************************************************************/
#include "RescueHorizon.h"
#include "RescueModel.h"
#include "RescueEventDescr.h"
#include "RescueLogicalOrder.h"
#include "RescueBuffer.h"
#include <string.h>

RescueHorizon::~RescueHorizon()
{
  if (horizonName != 0)
  {
    delete horizonName;
  }
  if (surfaces != 0)
  {
    delete surfaces;
  }
}

RescueHorizon::RescueHorizon(RESCUECHAR *horizonNameIn, RescueModel *parentModelIn)
                                    :RescueLogicalOrderEntry(parentModelIn->Context())
                                    ,horizonName(0)
                                    ,parentModel(parentModelIn)
                                    ,surfaces(0)
                                    ,unitAboveID(0)
                                    ,unitBelowID(0)
                                    ,eventDescrAboveID(0)
                                    ,eventDescrBelowID(0)
{
  isA = R_RescueHorizon;
  horizonName = new RCHString(horizonNameIn);
  surfaces = new cSetRescueBlockUnitHorizonSurface();
  (*parentModel->horizons) += this;
}

RescueHorizon::RescueHorizon(RescueContext *context, FILE *archiveFile)
                              :RescueLogicalOrderEntry(context)
                              ,horizonName(0)
                              ,parentModel(0)
                              ,surfaces(0)
                              ,unitAboveID(0)
                              ,unitBelowID(0)
                              ,eventDescrAboveID(0)
                              ,eventDescrBelowID(0)
{
  RESCUECHAR myString[255];

  isA = R_RescueHorizon;
  ReadId(context, archiveFile);
  myfgets(context, myString, 255, archiveFile);
  horizonName = new RCHString(myString);
  surfaces = new cSetRescueBlockUnitHorizonSurface();
  context->RescueProgress(myString);
  if (context->ReadFileVersion() < 30)
  {
    myfscanf(context, archiveFile, &unitAboveID);
    myfscanf(context, archiveFile, &unitBelowID);
    if (context->ReadFileVersion() >= 28)
    {
      myfscanf(context, archiveFile, &eventDescrAboveID);
      myfscanf(context, archiveFile, &eventDescrBelowID);
    }
  }
  surfaces->UnArchive(context, archiveFile);
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

void RescueHorizon::Relink(RescueObject *parentObject)
{
  parentModel = (RescueModel *) parentObject;
  surfaces->Relink(this);
}

void RescueHorizon::Archive(FILE *archiveFile)
{
  RescueContext *context = parentModel->Context();
  context->RescueProgress(horizonName->String());
  myfprintf(context, archiveFile, "; Horizon");
  myfprintf(context, archiveFile, Identifier());
  myfprintf(context, archiveFile, horizonName->String());
  if (context->FileVersion() < 30)
  {
    RescueLogicalOrder *order = parentModel->LogicalOrder();
    RescueUnit *unitAboveMe = order->UnitAbove(this);
    if (unitAboveMe == 0)
    {
      myfprintf(context, archiveFile, unitAboveID);
    }
    else
    {
      myfprintf(context, archiveFile, unitAboveMe->Identifier());
    }
    RescueUnit *unitBelowMe = order->UnitBelow(this);
    if (unitBelowMe == 0)
    {
      myfprintf(context, archiveFile, (RESCUEINT64) 0);
    }
    else
    {
      myfprintf(context, archiveFile, unitBelowMe->Identifier());
    }
    if (context->FileVersion() >= 28)
    {
      RescueEventDescr *eventDescrAboveMe = order->EventAbove(this);
      if (context->FileVersion() == 25 && eventDescrAboveMe->FullEvents(parentModel) == FALSE)
      {
        myfprintf(context, archiveFile, (RESCUEINT64) 0);
      }
      else if (eventDescrAboveMe == 0)
      {
        myfprintf(context, archiveFile, (RESCUEINT64) 0);
      }
      else
      {
        myfprintf(context, archiveFile, eventDescrAboveMe->Identifier());
      }
      RescueEventDescr *eventDescrBelowMe = order->EventBelow(this);
      if (context->FileVersion() == 25 && eventDescrAboveMe->FullEvents(parentModel) == FALSE)
      {
        myfprintf(context, archiveFile, (RESCUEINT64) 0);
      }
      else if (eventDescrBelowMe == 0)
      {
        myfprintf(context, archiveFile, (RESCUEINT64) 0);
      }
      else
      {
        myfprintf(context, archiveFile, eventDescrBelowMe->Identifier());
      }
    }
  }
  surfaces->Archive(context, archiveFile);
  if (context->FileVersion() >= 37)
  {
    myfprintf(context, archiveFile, "EOD");
  }
}

RescueTriangulatedSurface *RescueHorizon::SurfaceTriangulatedIdentifiedBy(RESCUEINT64 identifier)
{
  RescueTriangulatedSurface *myReturn = 0;
  RESCUEINT64 loop;
  for (loop = 0; loop < surfaces->Count64() && myReturn == 0; loop++)
  {
    RescueBlockUnitHorizonSurface *surface = surfaces->NthObject(loop);
    myReturn = surface->SurfaceTriangulatedIdentifiedBy(identifier);
  }
  return myReturn;
}

RescueIJSurface *RescueHorizon::SurfaceIJIdentifiedBy(RESCUEINT64 identifier)
{
  RescueIJSurface *myReturn = 0;
  RESCUEINT64 loop;
  for (loop = 0; loop < surfaces->Count64() && myReturn == 0; loop++)
  {
    RescueBlockUnitHorizonSurface *surface = surfaces->NthObject(loop);
    myReturn = surface->SurfaceIJIdentifiedBy(identifier);
  }

  return myReturn;
}

RescueProperty *RescueHorizon::PropertyIdentifiedBy(RESCUEINT64 identifier)
{
  RescueProperty *myReturn = 0;
  RESCUEINT64 loop;
  for (loop = 0; loop < surfaces->Count64() && myReturn == 0; loop++)
  {
    RescueBlockUnitHorizonSurface *surface = surfaces->NthObject(loop);
    myReturn = surface->PropertyIdentifiedBy(identifier);
  }

  return myReturn;
}

RESCUEBOOL RescueHorizon::IsNamed(const RESCUECHAR *possibleName)
{
   return ((*horizonName) == possibleName);
}

RESCUEBOOL RescueHorizon::IsOfType(_RescueObjectType thisType)
{
  if (thisType == R_RescueHorizon)
  {
    return TRUE;
  }
  else
  {
    return RescueLogicalOrderEntry::IsOfType(thisType);
  }
}

void RescueHorizon::SetOrientation(RescueOrientationLedger *ledger, 
                                   RescueCoordinateSystem::Orientation orientation)
{
  RESCUEINT64 ndx = 0;
  RescueBlockUnitHorizonSurface *surface = surfaces->NthObject(ndx++);
  while (surface != 0)
  {
    surface->SetOrientation(ledger, orientation);
    surface = surfaces->NthObject(ndx++);
  }
}

void RescueHorizon::FindUniquePropertyNames(cSetString *container)
{
  surfaces->FindUniquePropertyNames(container);
}


