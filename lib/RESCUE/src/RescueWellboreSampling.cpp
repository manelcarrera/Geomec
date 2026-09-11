/********************************************************************

  RescueWellboreSampling.cpp

  Defines the sampling rate for a group of attached properties,
  in terms of distance along the parent wellbore's geometry.

  Rod Hanks,  January, 1997

*********************************************************************/
#include "RescueWellboreSampling.h"
#include "RescueArray2dVector.h"
#include "RescueArray3dVector.h"
#include "RescueArrayByte.h"
#include "RescueArrayDouble.h"
#include "RescueArrayFloat.h"
#include "RescueArrayInt.h"
#include "RescueArrayShort.h"
#include "RescueBuffer.h"
#include "RescueModel.h"
#include "RescueWellbore.h"
#include "RescueWellboreProperty.h"
#include <string.h>

void RescueWellboreSampling::Dispose() { properties->Dispose(); }

RESCUEBOOL RescueWellboreSampling::DropRescueWellboreProperty(RescueWellboreProperty *unitToDrop) {
  unitToDrop->Dispose();
  return ((*properties) -= unitToDrop);
}

RescueWellboreSampling::~RescueWellboreSampling() {
  if (grid != 0) {
    delete grid;
  }
  if (properties != 0) {
    delete properties;
  }
  if (values != 0) {
    delete[] values;
  }
  if (timeStamp != 0) {
    delete timeStamp;
  }
}

void RescueWellboreSampling::SetValues(RESCUEFLOAT *valuesIn) {
  if (values != 0) {
    delete[] values;
  }
  values = valuesIn;
}

void RescueWellboreSampling::Swap() {
  grid->Axis(0)->Swap();
  if (values != 0) {
    RescueContext::SwapArray(values, grid->Axis(0)->Count64());
  }
  RESCUEINT64 pOrd = 0;
  RescueWellboreProperty *property = NthRescueWellboreProperty(pOrd++);
  while (property != 0) {
    property->Swap();
    property = NthRescueWellboreProperty(pOrd++);
  }
}

RescueWellboreSampling::RescueWellboreSampling(RescueWellbore *parentWellboreIn, RESCUEINT64 countIn,
                                               RESCUEFLOAT originIn, RESCUEFLOAT stepIn)
    : RescueObject(parentWellboreIn->ParentModel()->Context()), properties(0), parentWellbore(parentWellboreIn),
      grid(0), values(0) {
  isA = R_RescueWellboreSampling;
  timeStamp = new RCHString(35);
  RescueContext::CurrentISOTime(*timeStamp);
  grid = new RescueGrid(parentWellboreIn->ParentModel()->Context(), RescueCoordinateSystem::LUB, originIn, stepIn, 0,
                        countIn);
  properties = new cSetRescueWellboreProperty();
  (*parentWellboreIn->sampling) += this;
}

RescueWellboreSampling::RescueWellboreSampling(RescueWellbore *parentWellboreIn, RESCUEINT64 countIn,
                                               RESCUEFLOAT *valuesIn)
    : RescueObject(parentWellboreIn->ParentModel()->Context()), properties(0), parentWellbore(parentWellboreIn),
      grid(0), values(valuesIn) {
  isA = R_RescueWellboreSampling;
  timeStamp = new RCHString(35);
  RescueContext::CurrentISOTime(*timeStamp);
  grid = new RescueGrid(parentWellboreIn->ParentModel()->Context(), RescueCoordinateSystem::LUB, 0, countIn);
  properties = new cSetRescueWellboreProperty();
  (*parentWellboreIn->sampling) += this;
}

RescueWellboreSampling::RescueWellboreSampling(RescueContext *context, FILE *archiveFile)
    : RescueObject(context), properties(0), parentWellbore(0), grid(0), values(0) {
  timeStamp = new RCHString(35);
  RescueContext::CurrentISOTime(*timeStamp);
  if (context->ReadFileVersion() >= 37) {
    RESCUECHAR myString[255];

    myfgets(context, myString, 255, archiveFile);
    while (strcmp(myString, "EOD") != 0) {
      if (strcmp(myString, "timeStamp") == 0) {
        RescueBuffer buf(context, archiveFile);
        buf >> (*timeStamp);
      } else {
        RescueBuffer buf(context, archiveFile);
      }
      myfgets(context, myString, 255, archiveFile);
    }
  }
  grid = new RescueGrid(context, archiveFile);
  properties = new cSetRescueWellboreProperty();
  isA = R_RescueWellboreSampling;
  RESCUEINT64 howMany;
  myfscanf(context, archiveFile, &howMany);
  if (howMany != 0) {
    values = new RESCUEFLOAT[(size_t)howMany];
    myfscanf(context, archiveFile, values, howMany, context->ReadFileVersion() > 15);
  }
  properties->UnArchive(context, archiveFile);
}

void RescueWellboreSampling::Archive(FILE *archiveFile) {
  RescueContext *context = ParentWellbore()->ParentModel()->Context();
  if (context->FileVersion() >= 37) {
    myfprintf(context, archiveFile, "timeStamp");
    RescueBuffer buf(context, timeStamp->length64() + 5);
    buf << (*timeStamp);
    buf.Archive(archiveFile);
    myfprintf(context, archiveFile, "EOD");
  }
  grid->Archive(context, archiveFile);
  if (values == 0) {
    myfprintf(context, archiveFile, (RESCUEINT64)0);
  } else {
    RESCUEINT64 howMany = Count64();
    myfprintf(context, archiveFile, howMany);
    myfprintf(context, archiveFile, values, howMany, context->FileVersion() > 15);
  }
  properties->Archive(context, archiveFile);
}

void RescueWellboreSampling::Relink(RescueObject *parentObject) {
  parentWellbore = (RescueWellbore *)parentObject;
  grid->Relink((RescueObject *)parentWellbore->ParentModel());
  properties->Relink(this);
}

RESCUEFLOAT RescueWellboreSampling::MeasuredDepthAt(RESCUEINT64 ndx) {
  if (values == 0) {
    return grid->Axis(0)->ValueAt(ndx);
  } else {
    return values[ndx];
  }
}

RescueWellboreProperty *RescueWellboreSampling::PropertyDescribedBy(const RESCUECHAR *name, const RESCUECHAR *type,
                                                                    const RESCUECHAR *uom) {
  RescueWellboreProperty *myReturn = 0;
  RESCUEINT64 loop;
  for (loop = 0; loop < properties->Count64() && myReturn == 0; loop++) {
    RescueWellboreProperty *candidate = properties->NthObject(loop);
    RescueArray *data = candidate->Data();
    if (strcmp(name, data->PropertyName()->NonNullString()) == 0) {
      if (strcmp(type, data->PropertyType()->NonNullString()) == 0) {
        if (strcmp(uom, data->UnitOfMeasure()->NonNullString()) == 0) {
          myReturn = candidate;
        }
      }
    }
  }
  return myReturn;
}

void RescueWellboreSampling::WriteWITSML(FILE *file, RescueWellbore *wellbore, RCHString *uom) {
  if (Count64() > 0) {
    fprintf(file, " <wellLog>\n");
    fprintf(file, " <nameWell>%s</nameWell>\n", wellbore->WellboreName()->NonNullString());
    fprintf(file, " <nameWellbore>%s</nameWellbore>\n", wellbore->WellboreName()->NonNullString());
    fprintf(file, " <name>%s - %lld</name>\n", timeStamp->NonNullString(), Identifier());
    fprintf(file, " <indexType>measured depth</indexType>\n");
    fprintf(file, " <minIndex uom=\"%s\">%.2f</minIndex>\n", uom->NonNullString(), MeasuredDepthAt(0));
    fprintf(file, " <maxIndex uom=\"%s\">%.2f</maxIndex>\n", uom->NonNullString(), MeasuredDepthAt(Count64() - 1));
    fprintf(file, " <logCurveInfo uid=\"0\">\n");
    fprintf(file, "   <mnemonic>MDEPTH</mnemonic>\n");
    fprintf(file, "   <classWitsml>measured depth</classWitsml>\n");
    fprintf(file, "   <unit>%s</unit>\n", uom->NonNullString());
    fprintf(file, "   <nullValue>-999.25</nullValue>\n"); // If we ever have null values in MD I'm not aware of it.
    fprintf(file, "   <wellDatum>modelDatum</wellDatum>\n");
    fprintf(file, "   <curveDescription>measured depth</curveDescription>\n");
    fprintf(file, "   <typeLogData>double</typeLogData>\n"); // Really float.
    fprintf(file, " </logCurveInfo>\n");
    int pNdx = 0;
    RescueWellboreProperty *property = NthRescueWellboreProperty(pNdx++);
    while (property != 0) {
      fprintf(file, " <logCurveInfo uid=\"%d\">\n", pNdx);
      RescueArray *data = property->Data();
      fprintf(file, "   <mnemonic>%s</mnemonic>\n", data->PropertyType()->NonNullString());
      fprintf(file, "   <unit>%s</unit>\n", data->UnitOfMeasure()->NonNullString());
      switch (data->IsA()) {
      case R_RescueArrayFloat: {
        RescueArrayFloat *fData = (RescueArrayFloat *)data;
        fprintf(file, "   <nullValue>%f</nullValue>\n", fData->NullValue());
        break;
      } break;
      case R_RescueArrayDouble: {
        RescueArrayDouble *fData = (RescueArrayDouble *)data;
        fprintf(file, "   <nullValue>%lf</nullValue>\n", fData->NullValue());
        break;
      } break;
      case R_RescueArrayInt: {
        RescueArrayInt *fData = (RescueArrayInt *)data;
        fprintf(file, "   <nullValue>%d</nullValue>\n", fData->NullValue());
        break;
      } break;
      case R_RescueArrayShort: {
        RescueArrayShort *fData = (RescueArrayShort *)data;
        fprintf(file, "   <nullValue>%d</nullValue>\n", (int)fData->NullValue());
        break;
      } break;
      case R_RescueArrayByte: {
        RescueArrayByte *fData = (RescueArrayByte *)data;
        fprintf(file, "   <nullValue>%d</nullValue>\n", (int)fData->NullValue());
        break;
      } break;
      case R_RescueArray2dVector: {
        RescueArray2dVector *fData = (RescueArray2dVector *)data;
        Rescue2dVector nullValue = fData->NullValue();
        fprintf(file, "   <nullValue>%f %f</nullValue>\n", nullValue.coord1, nullValue.coord2);
        break;
      } break;
      case R_RescueArray3dVector: {
        RescueArray3dVector *fData = (RescueArray3dVector *)data;
        Rescue3dVector nullValue = fData->NullValue();
        fprintf(file, "   <nullValue>%f %f %f</nullValue>\n", nullValue.coord1, nullValue.coord2, nullValue.coord3);
        break;
      } break;
      default:
        break; // Quiet warning
      }
      fprintf(file, "   <curveDescription>%s</curveDescription>\n", data->PropertyName()->NonNullString());
      switch (data->IsA()) {
      case R_RescueArrayFloat: {
        fprintf(file, "   <typeLogData>double</typeLogData>\n"); // Really float.
        break;
      } break;
      case R_RescueArrayDouble: {
        fprintf(file, "   <typeLogData>double</typeLogData>\n");
        break;
      } break;
      case R_RescueArrayInt: {
        fprintf(file, "   <typeLogData>long</typeLogData>\n");
        break;
      } break;
      case R_RescueArrayShort: {
        fprintf(file, "   <typeLogData>long</typeLogData>\n");
        break;
      } break;
      case R_RescueArrayByte: {
        fprintf(file, "   <typeLogData>long</typeLogData>\n");
        break;
      } break;
      case R_RescueArray2dVector: {
        fprintf(file, "   <typeLogData>string</typeLogData>\n");
        break;
      } break;
      case R_RescueArray3dVector: {
        fprintf(file, "   <typeLogData>string</typeLogData>\n");
        break;
      } break;
      default: {
        fprintf(file, "   <typeLogData>unknown</typeLogData>\n");
      } break;
      }
      fprintf(file, " </logCurveInfo>\n");
      if (data->IsLoaded() == FALSE) {
        data->Load();
      }
      property = NthRescueWellboreProperty(pNdx++);
    }
    fprintf(file, "  <blockInfo uid=\"0\">\n");
    fprintf(file, "    <indexType>measured depth</indexType>\n");
    fprintf(file, "    <blockCurveInfo uid=\"bci-0\">\n");
    fprintf(file, "      <curveId>0</curveId>\n");
    fprintf(file, "      <columnIndex>1</columnIndex>\n");
    fprintf(file, "    </blockCurveInfo>\n");
    pNdx = 0;
    property = NthRescueWellboreProperty(pNdx++);
    while (property != 0) {
      fprintf(file, "    <blockCurveInfo uid=\"bci-%d\">\n", pNdx);
      fprintf(file, "      <curveId>%d</curveId>\n", pNdx);
      fprintf(file, "      <columnIndex>%d</columnIndex>\n", (pNdx + 1));
      fprintf(file, "    </blockCurveInfo>\n");
      property = NthRescueWellboreProperty(pNdx++);
    }
    fprintf(file, "  </blockInfo>\n");
    fprintf(file, " <logData>\n");
    RESCUEINT64 dNdx = 0;
    while (dNdx < Count64()) {
      fprintf(file, " <data id='0'>%f", MeasuredDepthAt(dNdx));
      int pNdx = 0;
      RescueWellboreProperty *property = NthRescueWellboreProperty(pNdx++);
      while (property != 0) {
        fprintf(file, ",");
        RescueArray *data = property->Data();
        switch (data->IsA()) {
        case R_RescueArrayFloat: {
          RescueArrayFloat *fData = (RescueArrayFloat *)data;
          fprintf(file, "%f", fData->Value()[dNdx]);
          break;
        } break;
        case R_RescueArrayDouble: {
          RescueArrayDouble *fData = (RescueArrayDouble *)data;
          fprintf(file, "%lf", fData->Value()[dNdx]);
          break;
        } break;
        case R_RescueArrayInt: {
          RescueArrayInt *fData = (RescueArrayInt *)data;
          fprintf(file, "%d", fData->Value()[dNdx]);
          break;
        } break;
        case R_RescueArrayShort: {
          RescueArrayShort *fData = (RescueArrayShort *)data;
          fprintf(file, "%d", (int)fData->Value()[dNdx]);
          break;
        } break;
        case R_RescueArrayByte: {
          RescueArrayByte *fData = (RescueArrayByte *)data;
          fprintf(file, "%d", (int)fData->Value()[dNdx]);
          break;
        } break;
        case R_RescueArray2dVector: {
          RescueArray2dVector *fData = (RescueArray2dVector *)data;
          Rescue2dVector nullValue = fData->NullValue();
          fprintf(file, "%f %f", nullValue.coord1, nullValue.coord2);
          break;
        } break;
        case R_RescueArray3dVector: {
          RescueArray3dVector *fData = (RescueArray3dVector *)data;
          Rescue3dVector nullValue = fData->NullValue();
          fprintf(file, "%f %f %f", nullValue.coord1, nullValue.coord2, nullValue.coord3);
          break;
        } break;

        default:
          break; // Quiet warning
        }

        property = NthRescueWellboreProperty(pNdx++);
      }
      fprintf(file, "</data>\n");
      dNdx++;
    }
    fprintf(file, " </logData>\n");

    pNdx = 0;
    property = NthRescueWellboreProperty(pNdx++);
    while (property != 0) {
      property->Data()->Unload();
      property = NthRescueWellboreProperty(pNdx++);
    }
    fprintf(file, "  </wellLog>\n");
  }
}

RESCUEBOOL RescueWellboreSampling::AnyFileTruncated() { return properties->AnyFileTruncated(); }

RESCUEBOOL RescueWellboreSampling::IsOfType(_RescueObjectType thisType) {
  if (thisType == R_RescueWellboreSampling) {
    return TRUE;
  } else {
    return RescueObject::IsOfType(thisType);
  }
}
