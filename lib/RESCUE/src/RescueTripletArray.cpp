/********************************************************************

  RescueTripletArray.cpp

  A dynamically sized array handler for three dimensional grids of
  floating point triplets for RESCUE's data model.

  Rod Hanks,  May 1996

*********************************************************************/
#include <string.h>
#include "RescueModel.h"
#include "RescueTripletArray.h"
#include "RescueGrid.h" 
#include "RCH2DArray.h"
#include "RCH3DArray.h"  
#include "RescueSplitPoint.h"
#include "RescueBuffer.h"

#define TRIPLET_FILE_VERSION 3
// File version for separate triplet files.
// 1 - Initial version.
// 2 - Compression.
// 3 - Add RescueSplitPoints.

void RescueTripletArray::Relink(RescueObject *parent)
{
  parentModel = (RescueModel *) parent;
  if (parentModel->geometryActionImmediate && IsLoaded() == FALSE)
  {
  Load();
  }
}

RESCUEBOOL RescueTripletArray::Load()
{
  RESCUEBOOL myReturn = FALSE;
  if (IsLoaded())
  {
  DropMemory();
  }
  if (parentModel->ExistingModel())
  {
  parentModel->Context()->readFileVersion = parentModel->currentFileVersion;
  parentModel->Context()->readFileMainSoftwareVersion = parentModel->readFileMainSoftwareVersion;
  parentModel->Context()->readFileSubSoftwareVersion = parentModel->readFileSubSoftwareVersion;
  myReturn = ReadData(parentModel->currentPathName);
  }
  if (myReturn == TRUE)
  {
  hasChanged = FALSE;
  }
  return myReturn;
}

RESCUEINT32 RescueTripletArray::Version(RESCUEBOOL reload)
{
  if (reload)
  {
  RESCUEINT32 myReturn = arrayVersion;
  RESCUEINT32 savedArrayVersion = arrayVersion;
  if (ReadData(parentModel->currentPathName, TRUE))
  {
      myReturn = arrayVersion;
      arrayVersion = savedArrayVersion;
  }
  return myReturn;
  }
  else
  {
  return arrayVersion;
  }
}

RESCUEBOOL RescueTripletArray::Unload()
{
  RESCUEBOOL myReturn = FALSE;
  if (hasChanged == FALSE)
  {
  DropMemory();
  myReturn = TRUE;
  }
  else if (IsLoaded() && parentModel->ExistingModel())
  {
  if (WriteData())
  {
      DropMemory();
      myReturn = TRUE;
  }
  }
  return myReturn;
}

RESCUEBOOL RescueTripletArray::ReadData(RESCUECHAR *basePathName, RESCUEBOOL versionOnly)
{
  RescueContext *context = parentModel->Context();
  parentModel->LoadContext();
  RESCUEBOOL myReturn = FALSE;
  RESCUECHAR buffer[30];
  RESCUEBOOL modelFile = FALSE;

  RCHString fileName;
  fileName << basePathName << "." << Identifier();
  FILE *archiveFile = (FILE *) fopen(fileName.String(), "rb");
  if (archiveFile != 0)
  {
  RESCUEINT64 fileVersion = getc(archiveFile);
  if (fileVersion == 'R')
  {
      context->binaryFlag = FALSE;
      fread(buffer, sizeof(RESCUECHAR), 19, archiveFile);
      if (strncmp(buffer, "escue Triplet File", 18) == 0)
      {
    modelFile = TRUE;
      }
  }
  else
  {
      context->binaryFlag = TRUE;
      fread(buffer, sizeof(RESCUECHAR), 19, archiveFile);
      if (strncmp(buffer, "Rescue Triplet File", 19) == 0)
      {
    modelFile = TRUE;
      }
  }
  fclose(archiveFile);
  if (modelFile)
  {
      int fileVersion = 0;
      if (context->binaryFlag)
      {
    archiveFile = (FILE *) fopen(fileName.String(), "rb");
    fileVersion = getc(archiveFile);
    fseek(archiveFile, 19, SEEK_CUR);
    myfscanf(context, archiveFile, &arrayVersion);
    if (versionOnly == FALSE)
    {
          UnArchiveData(archiveFile, fileVersion);
    }
    fclose(archiveFile);
    myReturn = TRUE;
      }
      else
      {
    RESCUECHAR myString[255];

    archiveFile = (FILE *) fopen(fileName.String(), "rt");
    myfgets(context, myString, 255, archiveFile);
    sscanf(myString, "Rescue Triplet File Version %d\n", &fileVersion);
    myfscanf(context, archiveFile, &arrayVersion);
    if (versionOnly == FALSE)
    {
          UnArchiveData(archiveFile, fileVersion);
    }
    fclose(archiveFile);
    myReturn = TRUE;
      }
  }
  else
  {
      RCHString message;
      message << "Not a Rescue Triplet File:" << fileName.NonNullString();
      context->SetError(message.NonNullString());
  }
  }
  return myReturn;
}

RESCUEBOOL RescueTripletArray::FileTruncated()
{
  RCHString fileName;
  fileName << parentModel->currentPathName << "." << Identifier();
  return parentModel->Context()->FileTruncatedPrimitive(fileName.String());
}

RESCUEBOOL RescueTripletArray::WriteData()
{
  RescueContext *context = parentModel->Context(true);
  RESCUEBOOL myReturn = FALSE;
  RCHString fileName;
  fileName << parentModel->currentPathName << "." << Identifier();
  parentModel->MakeBackupFile(fileName.String());
  FILE *archiveFile;
  if (parentModel->currentBinary)
  {
  archiveFile = (FILE *) fopen(fileName.String(), "wb");
  }
  else
  {
  archiveFile = (FILE *) fopen(fileName.String(), "wt");
  }
  if (archiveFile != 0)
  {
  context->binaryFlag = parentModel->currentBinary;
  if (context->binaryFlag)
  {
      putc((RESCUEUCHAR) TRIPLET_FILE_VERSION, archiveFile);
      fwrite("Rescue Triplet File", sizeof(RESCUECHAR), 19, archiveFile);
  }
  else
  {
      fprintf(archiveFile, "Rescue Triplet File Version %d\n", TRIPLET_FILE_VERSION);
  }
#ifdef TESTING
  myfprintf(context, archiveFile, arrayVersion);
#else
  myfprintf(context, archiveFile, ++arrayVersion);
#endif
  ArchiveData(archiveFile);
  myfprintf(context, archiveFile, "abracadabra jump jump");
  if (ferror(archiveFile) != 0)
  {
      RCHString message;
      message << "Error writing to Rescue Triplet File:";
      message << fileName.String();
      message << " (";
      message << (RESCUEINT64) ferror(archiveFile);
      message << ")";
      context->SetError(message.NonNullString());
  }
  fclose(archiveFile);
  myReturn = TRUE;
  }
  return myReturn;
}

void RescueTripletArray::ArchiveData(FILE *archiveFile)
{
  RescueContext *context = parentModel->Context();
  if (xValue == 0)
  {
  myfprintf(context, archiveFile, (RESCUEINT64) 0);
  }
  else
  {
  RESCUEINT64 items = grid->NodeCount64();
  myfprintf(context, archiveFile, items);
  myfprintf(context, archiveFile, xValue, items, TRUE);
  }
  if (yValue == 0)
  {
  myfprintf(context, archiveFile, (RESCUEINT64) 0);
  }
  else
  {
  RESCUEINT64 items = grid->NodeCount64();
  myfprintf(context, archiveFile, items);
  myfprintf(context, archiveFile, yValue, items, TRUE);
  }
  if (zValue == 0)
  {
  myfprintf(context, archiveFile, (RESCUEINT64) 0);
  }
  else
  {
  RESCUEINT64 items = grid->NodeCount64();
  myfprintf(context, archiveFile, items);
  myfprintf(context, archiveFile, zValue, items, TRUE);
  }
/*
  Note we always write the vertices info, but it doesn't matter.  If it is
  a version of Rescue < 31 it won't read this far.
*/
  if (vertices == 0)
  {
  myfprintf(context, archiveFile, (RESCUEINT64) 0);
  }
  else
  {
  myfprintf(context, archiveFile, iNodes);
  myfprintf(context, archiveFile, jNodes);
  RESCUEINT64 loop;
  RESCUEINT64 size = iNodes * jNodes;
  for (loop = 0; loop < size; loop++)
  {
      if (vertices[loop] == 0)
      {
    myfprintf(context, archiveFile, (RESCUEINT64) 0);
      }
      else
      {
    myfprintf(context, archiveFile, (RESCUEINT64) 1);
    vertices[loop]->Archive(context, archiveFile, TRUE);
      }
  }
  }
  if (context->FileVersion() >= 37)
  {
  myfprintf(context, archiveFile, "EOD");
  }
}

void RescueTripletArray::UnArchiveData(FILE *archiveFile, RESCUEINT64 fileVersion)
{
  RescueContext *context = parentModel->Context();
  myfscanf(context, archiveFile, &xValueLength);
  if (xValueLength != 0)
  {
  xValue = new RESCUEFLOAT [(size_t) xValueLength];
  myfscanf(context, archiveFile, xValue, xValueLength, fileVersion > 1);
  }
  myfscanf(context, archiveFile, &yValueLength);
  if (yValueLength != 0)
  {
  yValue = new RESCUEFLOAT [(size_t) yValueLength];
  myfscanf(context, archiveFile, yValue, yValueLength, fileVersion > 1);
  }
  myfscanf(context, archiveFile, &zValueLength);
  if (zValueLength != 0)
  {
  zValue = new RESCUEFLOAT [(size_t) zValueLength];
  myfscanf(context, archiveFile, zValue, zValueLength, fileVersion > 1);
  }
  if (fileVersion >= 3)
  {
  myfscanf(context, archiveFile, &iNodes);
  if (iNodes > 0)
  {
      myfscanf(context, archiveFile, &jNodes);
      RESCUEINT64 size = iNodes * jNodes;
      vertices = new RescueSplitPoint*[(int) size];
      RESCUEINT64 loop;
      for (loop = 0; loop < size; loop++)
      {
    RESCUEINT64 items = 0;
    myfscanf(context, archiveFile, &items);
    if (items == 0)
    {
          vertices[loop] = 0;
    }
    else
    {
          vertices[loop] = new RescueSplitPoint(context, archiveFile, fileVersion > 1);
    }
      }
  }
  }
  loaded = TRUE;
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

void RescueTripletArray::DropMemory()
{
  if (xValue != 0)
  {
  delete [] xValue;
  xValue = 0;
  xValueLength = 0;
  }
  if (yValue != 0)
  {
  delete [] yValue;
  yValue = 0;
  yValueLength = 0;
  }
  if (zValue != 0)
  {
  delete [] zValue;
  zValue = 0;
  zValueLength = 0;
  }
  ClearSplitNodes();
  loaded = FALSE;
}

void RescueTripletArray::SwapAxes(bool swapI, bool swapJ)
{
  EnsureLoaded();
  if (xValue != 0)
  {
  RescueContext::SwapAxes(xValue, swapI, iNodes, swapJ, jNodes);
  }
  if (yValue != 0)
  {
  RescueContext::SwapAxes(yValue, swapI, iNodes, swapJ, jNodes);
  }
  if (zValue != 0)
  {
  RescueContext::SwapAxes(zValue, swapI, iNodes, swapJ, jNodes);
  }
}

void RescueTripletArray::SwapArraysEndForEnd()
{
  EnsureLoaded();
  if (xValue != 0)
  {
  RescueContext::SwapArray(xValue, xValueLength);
  }
  if (yValue != 0)
  {
  RescueContext::SwapArray(yValue, yValueLength);
  }
  if (zValue != 0)
  {
  RescueContext::SwapArray(zValue, zValueLength);
  }
}

RescueTripletArray::~RescueTripletArray()
{
  // pmj calls EnsureLoaded that repopulates arrays
   if ( IsLoaded() )
   {
      ClearSplitNodes();
   }

  if (xValue != 0)
  {
  delete [] xValue;
  }
  if (yValue != 0)
  {
  delete [] yValue;
  }
  if (zValue != 0)
  {
  delete [] zValue;
  }
  //ClearSplitNodes();
}

void RescueTripletArray::RescueDeleteFile()
{
  if (parentModel != 0)
  {
  if (parentModel->currentPathName != 0)
  {
      if (parentModel->currentPathName[0] != 0)
      {
    RCHString fileName;
    fileName << parentModel->currentPathName << "." << Identifier();
    parentModel->ScheduleDeleteFile(fileName.String());
      }
  }
  }
}                                     
                                      
RescueTripletArray::RescueTripletArray(RescueGrid *existingGrid, 
                                       RESCUEFLOAT missingValueIn, 
                                       RESCUEFLOAT *valueArray,
                                       RescueModel *parentModelIn)
                                      :RescueObject(parentModelIn->Context())
                                      ,grid(existingGrid)
                                      ,xValue(0)
                                      ,yValue(0)
                                      ,zValue(0)
                                      ,xValueLength(0)
                                      ,yValueLength(0)
                                      ,zValueLength(0)
                                      ,hasChanged(TRUE)
                                      ,loaded(TRUE)
                                      ,parentModel(parentModelIn)
                                      ,arrayVersion(0)
                                      ,iNodes(0)
                                      ,jNodes(0)
                                      ,vertices(0)
{
  isA = R_RescueTripletArray;
  SetValue(missingValueIn, valueArray);
}

RescueTripletArray::RescueTripletArray(RescueContext *context, FILE *archiveFile)
                    :RescueObject(context)
                    ,grid(0)
                    ,xValue(0)
                    ,yValue(0)
                    ,zValue(0)
                    ,xValueLength(0)
                    ,yValueLength(0)
                    ,zValueLength(0)
                    ,hasChanged(FALSE)
                    ,loaded(FALSE)
                    ,parentModel(0)
                    ,arrayVersion(0)
                    ,iNodes(0)
                    ,jNodes(0)
                    ,vertices(0)
{
  isA = R_RescueTripletArray;
  myfscanf(context, archiveFile, &missingValue);
  if (context->ReadFileVersion() < 14)
  {
  myfscanf(context,archiveFile, &xValueLength);
  if (xValueLength != 0)
  {
      xValue = new RESCUEFLOAT [(size_t) xValueLength];
      myfscanf(context,archiveFile, xValue, xValueLength, FALSE);
  }
  myfscanf(context,archiveFile, &yValueLength);
  if (yValueLength != 0)
  {
      yValue = new RESCUEFLOAT [(size_t) yValueLength];
      myfscanf(context,archiveFile, yValue, yValueLength, FALSE);
  }
  myfscanf(context,archiveFile, &zValueLength);
  if (zValueLength != 0)
  {
      zValue = new RESCUEFLOAT [(size_t) zValueLength];
      myfscanf(context,archiveFile, zValue, zValueLength, FALSE);
  }
  loaded = TRUE;
  }
  else
  {
  ReadId(context,archiveFile);
  loaded = FALSE;
  }
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


void RescueTripletArray::Archive(FILE *archiveFile)
{
  myfprintf(parentModel->Context(), archiveFile, "; Triplet Array");
  myfprintf(parentModel->Context(), archiveFile, missingValue);
  if (parentModel->Context()->FileVersion() == 9)
  {
  RESCUEBOOL loadedNow = IsLoaded();
  if (loadedNow == FALSE)
  {
      ReadData(parentModel->oldPathName);
  }
  if (xValue == 0)
  {
      myfprintf(parentModel->Context(), archiveFile, (RESCUEINT64) 0);
  }
  else
  {
      RESCUEINT64 items = grid->NodeCount64();
      myfprintf(parentModel->Context(), archiveFile, items);
      myfprintf(parentModel->Context(), archiveFile, xValue, items, FALSE);
  }
  if (yValue == 0)
  {
      myfprintf(parentModel->Context(), archiveFile, (RESCUEINT64) 0);
  }
  else
  {
      RESCUEINT64 items = grid->NodeCount64();
      myfprintf(parentModel->Context(), archiveFile, items);
      myfprintf(parentModel->Context(), archiveFile, yValue, items, FALSE);
  }
  if (zValue == 0)
  {
      myfprintf(parentModel->Context(), archiveFile, (RESCUEINT64) 0);
  }
  else
  {
      RESCUEINT64 items = grid->NodeCount64();
      myfprintf(parentModel->Context(), archiveFile, items);
      myfprintf(parentModel->Context(), archiveFile, zValue, items, FALSE);
  }
  if (loadedNow == FALSE)
  {
      DropMemory();
  }
  }
  else
  {
  myfprintf(parentModel->Context(), archiveFile, Identifier());
  if (IsLoaded())
  {
      if (parentModel->Context()->delayedGeometryLoad == FALSE
      ||  hasChanged == TRUE
      ||  parentModel->geometryActionImmediate == TRUE)
      {
    WriteData();
      }
  }
  else if (parentModel->geometryActionImmediate)
  {
      if (ReadData(parentModel->oldPathName))
      {
    WriteData();
    DropMemory();
      }
  }
  }
  if (parentModel->Context()->FileVersion() >= 37)
  {
  myfprintf(parentModel->Context(), archiveFile, "EOD");
  }
}

void RescueTripletArray::SetValue(RESCUEFLOAT nullValueIn, RESCUEFLOAT *valueArray)
{
  EnsureLoaded();

  RESCUEINT64 loop;
  RESCUEINT64 ndx = 0;
  RESCUEINT64 items = grid->NodeCount64();
  RESCUEFLOAT *values[3];

  for (loop = 0; loop < 3; loop++)
  {
  if (IsCalculated(loop) == FALSE)
  {
       values[loop] = new RESCUEFLOAT[(size_t) items];
  }
  else
  {
      values[loop] = 0;
  }
  }

  missingValue = nullValueIn;

  RESCUEINT64 subloop;
  for (subloop = 0; subloop < 3; subloop++)
  {
  for (loop = 0; loop < items; loop++)
  {
      if (values[subloop] != 0)
      {
    values[subloop][loop] = valueArray[ndx++];
      }
      else
      {
    ndx++;
      }
  }
  }
/*
  Copy to three separate arrays.
*/
  AssignXValue(values[0]);
  AssignYValue(values[1]);
  AssignZValue(values[2]);
}

RESCUEBOOL RescueTripletArray::IsOfType(_RescueObjectType thisType)
{
  if (thisType == R_RescueTripletArray)
  {
  return TRUE;
  }
  else
  {
  return RescueObject::IsOfType(thisType);
  }
}

RESCUEBOOL RescueTripletArray::IsCalculated(RESCUEINT64 which)
{
  RESCUEBOOL myReturn = FALSE;
  RESCUEINT64 loop;
  for (loop = 0; loop < 2 && myReturn == FALSE; loop++)
  {
  if (IsRegular(loop))
  {
      RescueGridAxis *axis = grid->Axis(loop);
      if (axis->relatedAxis->parentCoordinateSystem->NdxOf(axis->relatedAxis) == which)
      {
    myReturn = TRUE;
      }
  }
  }
  return myReturn;
}

RESCUEBOOL RescueTripletArray::IsRegular(RESCUEINT64 which)
{
  return grid->IsRegular(which);
}

void RescueTripletArray::SetXValue(RESCUEFLOAT *valueArray)
{
  EnsureLoaded();
  if (IsCalculated(0) == FALSE)
  {
  xValueLength = grid->NodeCount64();
  if (xValue == 0)
  {
      xValue = new RESCUEFLOAT[(size_t) xValueLength];
  }
  RESCUEINT64 loop;
  for (loop = 0; loop < xValueLength; loop++)
  {
      xValue[loop] = valueArray[loop];
  }
  }
}

void RescueTripletArray::SetYValue(RESCUEFLOAT *valueArray)
{
  EnsureLoaded();
  if (IsCalculated(1) == FALSE)
  {
  yValueLength = grid->NodeCount64();
  if (yValue == 0)
  {
      yValue = new RESCUEFLOAT[(size_t) yValueLength];
  }
  RESCUEINT64 loop;
  for (loop = 0; loop < yValueLength; loop++)
  {
      yValue[loop] = valueArray[loop];
  }
  }
}

void RescueTripletArray::SetZValue(RESCUEFLOAT *valueArray)
{
  EnsureLoaded();
  if (IsCalculated(2) == FALSE)
  {
  zValueLength = grid->NodeCount64();
  if (zValue == 0)
  {
      zValue = new RESCUEFLOAT[(size_t) zValueLength];
  }
  RESCUEINT64 loop;
  for (loop = 0; loop < zValueLength; loop++)
  {
      zValue[loop] = valueArray[loop];
  }
  }
}

void RescueTripletArray::AssignXValue(RESCUEFLOAT *valueArray)
{
  EnsureLoaded();
  if (IsCalculated(0) == FALSE)
  {
  if (xValue != 0)
  {
      delete [] xValue;
  }
  xValue = valueArray;
  xValueLength = grid->NodeCount64();
  }
}

void RescueTripletArray::AssignYValue(RESCUEFLOAT *valueArray)
{
  EnsureLoaded();
  if (IsCalculated(1) == FALSE)
  {
  if (yValue != 0)
  {
      delete [] yValue;
  }
  yValue = valueArray;
  yValueLength = grid->NodeCount64();
  }
}

void RescueTripletArray::AssignZValue(RESCUEFLOAT *valueArray)
{
  EnsureLoaded();
  if (IsCalculated(2) == FALSE)
  {
  if (zValue != 0)
  {
      delete [] zValue;
  }
  zValue = valueArray;
  zValueLength = grid->NodeCount64();
  }
}

void RescueTripletArray::XYZAt(RESCUEINT64 i, RESCUEFLOAT &x, RESCUEFLOAT &y, RESCUEFLOAT &z)
{
  EnsureLoaded();
  RescueGridAxis *axis = grid->Axis(0);
  RESCUEINT64 ndx = i - axis->LowBound64();
  if (xValue == 0)
  {
  x = axis->Origin() + (axis->Step() * ndx);
  }
  else
  {
  x = xValue[ndx];
  }
  y = yValue[ndx];
  z = zValue[ndx];
}

void RescueTripletArray::XValues(RESCUEFLOAT *buffer, RESCUEINT64 offset, RESCUEINT64 bufferLength)
{
  RESCUEFLOAT *value = RescueGetXValue();
  SendBuffer(value, buffer, offset, bufferLength);
}

void RescueTripletArray::YValues(RESCUEFLOAT *buffer, RESCUEINT64 offset, RESCUEINT64 bufferLength)
{
  RESCUEFLOAT *value = RescueGetYValue();
  SendBuffer(value, buffer, offset, bufferLength);
}

void RescueTripletArray::ZValues(RESCUEFLOAT *buffer, RESCUEINT64 offset, RESCUEINT64 bufferLength)
{
  RESCUEFLOAT *value = RescueGetZValue();
  SendBuffer(value, buffer, offset, bufferLength);
}

void RescueTripletArray::SendBuffer(RESCUEFLOAT *value, RESCUEFLOAT *buffer, RESCUEINT64 offset, RESCUEINT64 bufferLength)
{
  if (value != 0)
  {
  RESCUEFLOAT *pos = &buffer[offset];
  RESCUEFLOAT *src = value;
  while (bufferLength > 0)
  {
      *pos++ = *src++;
      bufferLength--;
  }
  }
}

void RescueTripletArray::EnsureLoaded()
{
  if (IsLoaded() == false)
  {
  Load();
  }
}

RESCUEFLOAT *RescueTripletArray::RescueGetXValue() 
{
  EnsureLoaded();
  if (xValue != 0)
  {
  return xValue;
  }
  else
  {
  return MakeArray(&xValue, 0);
  }
}

RESCUEINT64 RescueTripletArray::RescueGetXValueLength64()
{
  EnsureLoaded();
  if (xValue == 0)
  {
  return 0;
  }
  else
  {
  return xValueLength;
  }
}

RESCUEINT64 RescueTripletArray::RescueGetYValueLength64()
{
  EnsureLoaded();
  if (yValue == 0)
  {
  return 0;
  }
  else
  {
  return yValueLength;
  }
}

RESCUEINT64 RescueTripletArray::RescueGetZValueLength64()
{
  EnsureLoaded();
  if (zValue == 0)
  {
  return 0;
  }
  else
  {
  return zValueLength;
  }
}

RESCUEFLOAT *RescueTripletArray::RescueGetYValue() 
{
  if (yValue != 0)
  {
  return yValue;
  }
  else
  {
  return MakeArray(&yValue, 1);
  }
}

RESCUEFLOAT *RescueTripletArray::RescueGetZValue() 
{
  EnsureLoaded();
  if (zValue != 0)
  {
  return zValue;
  }
  else
  {
  return MakeArray(&zValue, 2);
  }
}

RESCUEFLOAT *RescueTripletArray::MakeArray(RESCUEFLOAT **arrayToMake, RESCUEINT64 axisNdxOfArray)
{
  RESCUEINT64 loop;
  RESCUEBOOL found = FALSE;
  for (loop = 0; loop < 2 && found == FALSE; loop++)
  {
  if (IsRegular(loop))
  {
      RescueGridAxis *axis = grid->Axis(loop);
      if (axis->relatedAxis->parentCoordinateSystem->NdxOf(axis->relatedAxis) == axisNdxOfArray)
      {
    RESCUEINT64 howMany = grid->NodeCount64();
    (*arrayToMake) = new RESCUEFLOAT[(size_t) howMany];
    switch (grid->Dimensions())
    {
    case 1:
          {
      RESCUEINT64 iLoop;
      for (iLoop = axis->LowBound64(); iLoop < axis->LowBound64() + axis->Count64(); iLoop++)
      {
              (*arrayToMake)[iLoop - axis->LowBound64()] = axis->Origin() 
                                                       + (axis->Step() * (iLoop - axis->LowBound64())); 
      }
          }
          break;
    case 2:
          {
      RescueGridAxis *iAxis = grid->Axis(0);
      RescueGridAxis *jAxis = grid->Axis(1);
      RCH2DArray controller = RCH2DArray(iAxis->Count64(),
                                               jAxis->Count64(),
                                               (*arrayToMake));
      RESCUEINT64 iLoop, jLoop;
      for (iLoop = iAxis->LowBound64(); iLoop < iAxis->LowBound64() + iAxis->Count64(); iLoop++)
      {
              for (jLoop = jAxis->LowBound64(); jLoop < jAxis->LowBound64() + jAxis->Count64(); jLoop++)
              {
        if (axis == iAxis)
        {
                  controller.Ndx(iLoop - iAxis->LowBound64(), 
                                 jLoop - jAxis->LowBound64()) = axis->Origin() 
                                                       + (axis->Step() * (iLoop - axis->LowBound64()));
        }
        else
        {
                  controller.Ndx(iLoop - iAxis->LowBound64(), 
                                 jLoop - jAxis->LowBound64()) = axis->Origin() 
                                                       + (axis->Step() * (jLoop - axis->LowBound64()));
        }
              }
      }
          }
          break;
    case 3:
          {
      RescueGridAxis *iAxis = grid->Axis(0);
      RescueGridAxis *jAxis = grid->Axis(1);
      RescueGridAxis *kAxis = grid->Axis(2);
      RCH3DArray controller = RCH3DArray(iAxis->Count64(),
                                               jAxis->Count64(),
                                               kAxis->Count64(),
                                               (*arrayToMake));
      RESCUEINT64 iLoop, jLoop, kLoop;
      for (iLoop = iAxis->LowBound64(); iLoop < iAxis->LowBound64() + iAxis->Count64(); iLoop++)
      {
              for (jLoop = jAxis->LowBound64(); jLoop < jAxis->LowBound64() + jAxis->Count64(); jLoop++)
              {
        for (kLoop = kAxis->LowBound64(); kLoop < kAxis->LowBound64() + kAxis->Count64(); kLoop++)
        {
                  if (axis == iAxis)
                  {
          controller.Ndx(iLoop - iAxis->LowBound64(), 
                                   jLoop - jAxis->LowBound64(),
                                   kLoop - kAxis->LowBound64()) = axis->Origin() 
                                                         + (axis->Step() * (iLoop - axis->LowBound64()));
                  }
                  else if (axis == jAxis)
                  {
          controller.Ndx(iLoop - iAxis->LowBound64(), 
                                   jLoop - jAxis->LowBound64(),
                                   kLoop - kAxis->LowBound64()) = axis->Origin() 
                                                         + (axis->Step() * (jLoop - axis->LowBound64()));
                  }
                  else if (axis == kAxis)
                  {
          controller.Ndx(iLoop - iAxis->LowBound64(), 
                                   jLoop - jAxis->LowBound64(),
                                   kLoop - kAxis->LowBound64()) = axis->Origin() 
                                                         + (axis->Step() * (kLoop - axis->LowBound64()));
                  }
        }
              }
      }
          }
          break;
    }
    found = TRUE;
      }
  }
  }
  return *arrayToMake;
}

void RescueTripletArray::XYZAt(RESCUEINT64 i, RESCUEINT64 j, RESCUEFLOAT &x, RESCUEFLOAT &y, RESCUEFLOAT &z)
{
  EnsureLoaded();
  RescueGridAxis *iAxis = grid->Axis(0);
  RescueGridAxis *jAxis = grid->Axis(1);
  RESCUEINT64 ndx = (j - jAxis->LowBound64()) * iAxis->Count64() + (i- iAxis->LowBound64());

  if (xValue != 0) x = xValue[ndx];
  if (yValue != 0) y = yValue[ndx];
  if (zValue != 0) z = zValue[ndx];
  RESCUEINT64 loop;
  for (loop = 0; loop < 2; loop++)
  {
  if (IsRegular(loop))
  {
      RescueGridAxis *axis = grid->Axis(loop);
      RESCUEDOUBLE temp;
      if (loop == 0) {
    temp = axis->Origin() + (axis->Step() * (i - axis->LowBound64()));
      }
      else {
    temp = axis->Origin() + (axis->Step() * (j - axis->LowBound64()));
      }
      switch (axis->relatedAxis->parentCoordinateSystem->NdxOf(axis->relatedAxis))
      {
      case 0:
    if (xValue == 0)
    {
          x = (RESCUEFLOAT) temp;
    }
    break;
      case 1:
    if (yValue == 0)
    {
          y = (RESCUEFLOAT) temp;
    }
    break;
      case 2:
    if (zValue == 0)
    {
          z = (RESCUEFLOAT) temp;
    }
      }
  }
  }
}

void RescueTripletArray::XYZAt(RESCUEINT64 i, RESCUEINT64 j, RESCUEINT64 k, RESCUEFLOAT &x, RESCUEFLOAT &y, RESCUEFLOAT &z)
{
  EnsureLoaded();
  RescueGridAxis *iAxis = grid->Axis(0);
  RescueGridAxis *jAxis = grid->Axis(1);
  RescueGridAxis *kAxis = grid->Axis(2);
  RESCUEINT64 kCount = kAxis->Count64();
  RESCUEINT64 jCount = jAxis->Count64();
  RESCUEINT64 ndx = ((k - kAxis->LowBound64()) * (kCount * jCount)) 
          + ((j - jAxis->LowBound64()) * jCount) 
          + (i - iAxis->LowBound64());

  if (xValue == 0)
  {
  x = iAxis->Origin() + (iAxis->Step() * (i - iAxis->LowBound64()));
  }
  else
  {
  x = xValue[ndx];
  }
  if (yValue == 0)
  {
  y = jAxis->Origin() + (jAxis->Step() * (j - jAxis->LowBound64()));
  }
  else
  {
  y = yValue[ndx];
  }
  if (zValue == 0)
  {
  z = kAxis->Origin() + (kAxis->Step() * (k - kAxis->LowBound64()));
  }
  else
  {
  z = zValue[ndx];
  }
}

void RescueTripletArray::XYZCornerAt(RESCUEINT64 i, RESCUEINT64 j, RESCUEINT64 faceNumber, RESCUEFLOAT &x, RESCUEFLOAT &y, RESCUEFLOAT &z)
{
  EnsureLoaded();
  RESCUEBOOL handled = FALSE;
  if (vertices != 0)
  {
  if (i >= 0 && i < iNodes && j >= 0 && j < jNodes)
  {
      RESCUEINT64 ndx = (j * iNodes) + i;
      if (vertices[ndx] != 0)
      {
    vertices[ndx]->Values(faceNumber, x, y, z);
    handled = TRUE;
      }
  }
  }
  if (handled == FALSE)
  {
  XYZAt(i, j, x, y, z);
  }
}

RESCUEBOOL RescueTripletArray::HasSplitNodes()
{
  EnsureLoaded();
 RESCUEBOOL myReturn = FALSE;
  if (vertices != 0)
  {
  myReturn = TRUE;
  }
  return myReturn;
}

RESCUEBOOL RescueTripletArray::IsSplit(RESCUEINT64 i, RESCUEINT64 j)
{
  EnsureLoaded();
  RESCUEBOOL myReturn = FALSE;
  if (vertices != 0)
  {
  if (i >= 0 && i < iNodes && j >= 0 && j < jNodes)
  {
      RESCUEINT64 ndx = (j * iNodes) + i;
      if (vertices[ndx] != 0)
      {
    myReturn = TRUE;
      }
  }
  }
  return myReturn;
}

void RescueTripletArray::ClearSplitNode(RESCUEINT64 i, RESCUEINT64 j)
{
  EnsureLoaded();
  if (vertices != 0)
  {
  if (i >= 0 && i < iNodes && j >= 0 && j < jNodes)
  {
      RESCUEINT64 ndx = (j * iNodes) + i;
      if (vertices[ndx] != 0)
      {
    delete vertices[ndx];
    vertices[ndx] = 0;
      }
  }
  }
}

void RescueTripletArray::ClearSplitNodes()
{
  EnsureLoaded();
  if (vertices != 0)
  {
  RESCUEINT64 size = iNodes * jNodes;
  RESCUEINT64 loop;
  for (loop = 0; loop < size; loop++)
  {
      if (vertices[loop] != 0)
      {
    delete vertices[loop];
      }
  }
  }
  delete [] vertices;
  vertices = 0;
  iNodes = 0;
  jNodes = 0;
}

void RescueTripletArray::SetXYZCorner(RESCUEINT64 i, RESCUEINT64 j, RESCUEINT64 faceNumber, RESCUEFLOAT x, RESCUEFLOAT y, RESCUEFLOAT z)
{
  EnsureLoaded();
  if (vertices == 0)
  {
  RescueGridAxis *iAxis = grid->Axis(0);
  RescueGridAxis *jAxis = grid->Axis(1);
  iNodes = iAxis->Count64();
  jNodes = jAxis->Count64();
  RESCUEINT64 size = iNodes * jNodes;
  vertices = new RescueSplitPoint*[(int) size];
  RESCUEINT64 loop;
  for (loop = 0; loop < size; loop++)
  {
      vertices[loop] = 0;
  }
  }
  if (i >= 0 && i < iNodes && j >= 0 && j < jNodes)
  {
  RESCUEINT64 ndx = (j * iNodes) + i;
  if (vertices[ndx] == 0)
  {
      RESCUEFLOAT x1, y1, z1;
      XYZAt(i, j, x1, y1, z1);
      vertices[ndx] = new RescueSplitPoint(x1, y1, z1);
  }
  vertices[ndx]->SetCornerNode(faceNumber, x, y, z);
  }
}




