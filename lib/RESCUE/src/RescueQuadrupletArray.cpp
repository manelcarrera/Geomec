/********************************************************************

  RescueQuadrupletArray.cpp

  Adds to RescueTripletArray the ability to hold an MD value for
  each xyz value.  The MD value is never evenly sampled.

  Rod Hanks,  May 2002

*********************************************************************/
#include <string.h>
#include <float.h>
#include "RescueModel.h"
#include "RescueQuadrupletArray.h"
#include "RescueGrid.h" 
#include "RCH2DArray.h"
#include "RCH3DArray.h"  
#include "RescueBuffer.h"

RescueQuadrupletArray::RescueQuadrupletArray(RescueGrid *existingGrid, RESCUEFLOAT missingValueIn, 
                                             RESCUEFLOAT *valueArray, RescueModel *parentModelIn)
                                             :RescueTripletArray(parentModelIn->Context(),
                                                                 existingGrid, 
                                                                 missingValueIn, 
                                                                 parentModelIn)
                                             ,mdValue(0),mdValueLength(0)
{
  isA = R_RescueQuadrupletArray;
  SetValue(missingValueIn, valueArray);
}

RescueQuadrupletArray::~RescueQuadrupletArray()
{
  if (mdValue != 0)
  {
  delete [] mdValue;
  }
}

void RescueQuadrupletArray::MDValues(RESCUEFLOAT *buffer, RESCUEINT64 offset, RESCUEINT64 bufferLength)
{
  SendBuffer(mdValue, buffer, offset, bufferLength);
}

void RescueQuadrupletArray::SetValue(RESCUEFLOAT nullValueIn, RESCUEFLOAT *valueArray)
{
  EnsureLoaded();

  RESCUEINT64 loop;
  RESCUEINT64 ndx = 0;
  RESCUEINT64 items = grid->NodeCount64();
  RESCUEFLOAT *values[4];

  values[3] = new RESCUEFLOAT[(size_t) items];

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
  for (subloop = 0; subloop < 4; subloop++)
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
  AssignMDValue(values[3]);
}

void RescueQuadrupletArray::AssignMDValue(RESCUEFLOAT *valueArray)
{
  EnsureLoaded();
  if (mdValue != 0)
  {
  delete [] mdValue;
  }
  mdValue = valueArray;
  mdValueLength = grid->NodeCount64();
}

void RescueQuadrupletArray::SetMDValue(RESCUEFLOAT *valueArray)
{
  EnsureLoaded();
  mdValueLength = grid->NodeCount64();
  if (mdValue == 0)
  {
  mdValue = new RESCUEFLOAT[(size_t) mdValueLength];
  }
  RESCUEINT64 loop;
  for (loop = 0; loop < mdValueLength; loop++)
  {
  mdValue[loop] = valueArray[loop];
  }
}

RESCUEBOOL RescueQuadrupletArray::IsOfType(_RescueObjectType thisType)
{
  if (thisType == R_RescueQuadrupletArray)
  {
  return TRUE;
  }
  else
  {
  return RescueTripletArray::IsOfType(thisType);
  }
}

void RescueQuadrupletArray::XYZMDAt(RESCUEINT64 i, RESCUEFLOAT &x, RESCUEFLOAT &y, RESCUEFLOAT &z, RESCUEFLOAT &md)
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
  if (mdValue == 0)
  {
  md = FLT_MAX;
  }
  else
  {
  md = mdValue[ndx];
  }
}

RescueQuadrupletArray::RescueQuadrupletArray(RescueContext *context, FILE *archiveFile)
                    :RescueTripletArray(context, archiveFile)
                    ,mdValue(0),mdValueLength(0)
{
  isA = R_RescueQuadrupletArray;
}

void RescueQuadrupletArray::ArchiveData(FILE *archiveFile)
{
  RescueTripletArray::ArchiveData(archiveFile);
  RescueContext *context = parentModel->Context();
  if (context->FileVersion() >= 30)
  {
  if (mdValue == 0)
  {
      myfprintf(context, archiveFile, (RESCUEINT64) 0);
  }
  else
  {
      RESCUEINT64 items = grid->NodeCount64();
      myfprintf(context, archiveFile, items);
      myfprintf(context, archiveFile, mdValue, items, TRUE);
  }
  }
  if (context->FileVersion() >= 37)
  {
  myfprintf(context, archiveFile, "EOD");
  }
}

void RescueQuadrupletArray::UnArchiveData(FILE *archiveFile, RESCUEINT64 fileVersion)
{
  RescueTripletArray::UnArchiveData(archiveFile, fileVersion);
  RescueContext *context = parentModel->Context();
  if (context->ReadFileVersion() >= 30)
  {
  mdValueLength = 0;
  myfscanf(context, archiveFile, &mdValueLength);
  if (mdValueLength != 0)
  {
      mdValue = new RESCUEFLOAT [(size_t) mdValueLength];
      myfscanf(context, archiveFile, mdValue, mdValueLength, TRUE);
  }
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

void RescueQuadrupletArray::DropMemory()
{
  RescueTripletArray::DropMemory();
  if (mdValue != 0)
  {
  delete [] mdValue;
  mdValue = 0;
  mdValueLength = 0;
  }
}

void RescueQuadrupletArray::SwapArraysEndForEnd()
{
  RescueTripletArray::SwapArraysEndForEnd();
  if (mdValue != 0)
  {
  RescueContext::SwapArray(mdValue, mdValueLength);
  }
}

void RescueQuadrupletArray::WriteWITSML(FILE *file, RescueWellbore *wellbore, RCHString *uom, RCHString *csName)
{
  RESCUEBOOL didLoad = FALSE;
  if (IsLoaded() == FALSE)
  {
  if (Load() == FALSE)
  {
      throw "Failed to load wellbore trajectory.";
  }
  didLoad = TRUE;
  }

  RESCUEINT64 howMany = MDValueLength64();
  RESCUEFLOAT *mds = MDValue();
  RESCUEFLOAT *tvds = RescueGetZValue();
  RESCUEFLOAT *easting = RescueGetXValue();
  RESCUEFLOAT *northing = RescueGetYValue();

  if (howMany > 0 && mds != 0)
  {
  fprintf(file, "  <trajectory>\n");
  fprintf(file, "  <nameWell>%s</nameWell>\n", wellbore->WellboreName()->NonNullString());
  fprintf(file, "  <nameWellbore>%s</nameWellbore>\n", wellbore->WellboreName()->NonNullString());
  fprintf(file, "  <name>%s</name>\n", wellbore->WellboreName()->NonNullString());
/*
  These three values are required, but we don't really have them in RESCUE, so we just put the same
  thing three times.
*/
  fprintf(file, "  <mdMn uom=\"%s\">%.2f</mdMn>\n", uom->NonNullString(), mds[0]);
  fprintf(file, "  <mdMx uom=\"%s\">%.2f</mdMx>\n", uom->NonNullString(), mds[howMany - 1]);
  fprintf(file, "  <dispNsVertSectOrig uom=\"%s\">0.00</dispNsVertSectOrig>\n", uom->NonNullString());
  fprintf(file, "  <dispEwVertSectOrig uom=\"%s\">0.00</dispEwVertSectOrig>\n", uom->NonNullString());

  RESCUEINT64 loop;
  for (loop = 0; loop < howMany; loop++)
  {
      fprintf(file, "    <trajectoryStation>\n");
      fprintf(file, "      <typeTrajStation>unknown</typeTrajStation>\n");
      fprintf(file, "      <md uom=\"%s\">%.2f</md>\n", uom->NonNullString(), mds[loop]);
      fprintf(file, "      <tvd uom=\"%s\">%.2f</tvd>\n", uom->NonNullString(), tvds[loop]);
      fprintf(file, "      <dispNs uom=\"%s\">%.2f</dispNs>\n", uom->NonNullString(), (loop == 0) ? 0.0 : northing[loop] - northing[loop - 1]);
      fprintf(file, "      <dispEw uom=\"%s\">%.2f</dispEw>\n", uom->NonNullString(), (loop == 0) ? 0.0 : easting[loop] - easting[loop - 1]);
      fprintf(file, "      <mdDelta uom=\"%s\">%.2f</mdDelta>\n", uom->NonNullString(), (loop == 0) ? 0.0 : mds[loop] - mds[loop - 1]);
      fprintf(file, "      <tvdDelta uom=\"%s\">%.2f</tvdDelta>\n", uom->NonNullString(), (loop == 0) ? 0.0 : tvds[loop] - tvds[loop - 1]);
      fprintf(file, "      <location uid=\"loc-%lld\">\n", loop);
      fprintf(file, "        <wellCRS uidRef=\"grid\">%s</wellCRS>\n", csName->NonNullString());
      fprintf(file, "        <easting uom=\"%s\">%.2f</easting>\n", uom->NonNullString(), easting[loop]);
      fprintf(file, "        <northing uom=\"%s\">%.2f</northing>\n", uom->NonNullString(), northing[loop]);
      fprintf(file, "      </location>\n");
      fprintf(file, "      <commonData>\n");
      fprintf(file, "        <sourceName>Rescue v%d.%d</sourceName>\n", FILE_VERSION, FILE_SUBVERSION);
      fprintf(file, "        <comments>(none)</comments>\n");
      fprintf(file, "      </commonData>\n");
      fprintf(file, "    </trajectoryStation>\n");
  }

  fprintf(file, "%s\n", "  </trajectory>");
  }
  if (didLoad)
  {
  Unload();
  }
}



