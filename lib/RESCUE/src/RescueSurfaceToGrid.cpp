/********************************************************************

  RescueSurfaceToGrid.cpp

  The record of an intersect between a grid cell and a surface.

  Rod Hanks,  January 1997

*********************************************************************/
#include "RescueSurfaceToGrid.h"
#include "RescueBuffer.h"
#include "RescueGeometry.h"
#include "RescueIJSurface.h"
#include "RescueModel.h"
#include "cSetRescueDataContainer.h"
#include <string.h>

RESCUEINT32 *RescueSurfaceToGrid::IList(RESCUEBOOL throwIfTooBig) {
  if (throwIfTooBig) {
    if (listLength > 2147483647 || listLength < -2147483647) {
      throw "Model is too large to be read in 32 bit mode.";
    }
  }
  return RescueContext::Allocate32For64(iList, (RESCUEINT32)listLength, throwIfTooBig, throwIfTooBig);
}

RESCUEINT32 *RescueSurfaceToGrid::JList(RESCUEBOOL throwIfTooBig) {
  if (throwIfTooBig) {
    if (listLength > 2147483647 || listLength < -2147483647) {
      throw "Model is too large to be read in 32 bit mode.";
    }
  }
  return RescueContext::Allocate32For64(jList, (RESCUEINT32)listLength, throwIfTooBig, throwIfTooBig);
}

RescueSurfaceToGrid::RescueSurfaceToGrid(RescueGeometry *geometryIn, RescueIJSurface *surfaceIn, RESCUEINT64 *iListIn,
                                         RESCUEINT64 *jListIn, RESCUEINT64 listLengthIn)
    : RescueObject(geometryIn->ParentModel()->Context()), geometry(geometryIn), surface(surfaceIn), iList(iListIn),
      jList(jListIn), listLength(listLengthIn), properties(0), propertyContainerId(0) {
  isA = R_RescueSurfaceToGrid;
  if (surface != 0) {
    surface->AddSurfaceToGrid(this);
  }
}

RESCUEBOOL RescueSurfaceToGrid::AnyFileTruncated() {
  RESCUEBOOL myReturn = FALSE;
  if (properties == 0 && propertyContainerId != 0) {
    properties = new cSetRescueDataContainer(geometry->ParentModel(), propertyContainerId); // Will Read.
  }
  if (properties != 0) {
    myReturn = properties->AnyFileTruncated();
  }
  return myReturn;
}

cSetRescueDataContainer *RescueSurfaceToGrid::DataContainers() {
  if (properties == 0) {
    if (propertyContainerId != 0) {
      properties = new cSetRescueDataContainer(geometry->ParentModel(), propertyContainerId); // Will Read.
    }
  }
  return properties;
}

cSetRescueDataContainer *RescueSurfaceToGrid::DemandDataContainers() {
  if (properties == 0) {
    if (propertyContainerId != 0) {
      properties = new cSetRescueDataContainer(geometry->ParentModel(), propertyContainerId); // Will Read.
    } else {
      properties = new cSetRescueDataContainer(geometry->ParentModel());
    }
  }
  return properties;
}

void RescueSurfaceToGrid::SetList(RESCUEINT64 *iListIn, RESCUEINT64 *jListIn, RESCUEINT64 listLengthIn) {
  delete[] iList;
  delete[] jList;
  listLength = listLengthIn;
  iList = iListIn;
  jList = jListIn;
}

void RescueSurfaceToGrid::Archive(FILE *archiveFile) {
  RescueContext *context = geometry->ParentModel()->Context();
  myfprintf(context, archiveFile, "; Surface to grid");
  myfprintf(context, archiveFile, Identifier());
  if (context->FileVersion() >= 23) {
    myfprintf(context, archiveFile, surface->Identifier());
  } else {
    myfprintf(context, archiveFile, surface->ParentSurface()->Identifier());
  }
  myfprintf(context, archiveFile, listLength);
  myfprintf(context, archiveFile, iList, listLength);
  myfprintf(context, archiveFile, jList, listLength);
  if (context->FileVersion() >= 37) {
    if (properties == 0 && propertyContainerId != 0 && geometry->ParentModel()->propertyActionImmediate == TRUE) {
      properties = new cSetRescueDataContainer(geometry->ParentModel(), propertyContainerId); // Will Read.
    }
    if (properties != 0) {
      myfprintf(context, archiveFile, "properties");
      RescueBuffer buf1(context, 10);
      buf1 << properties->Identifier();
      buf1.Archive(archiveFile);

      properties->Archive(); // Goes into it's own file.
    } else if (properties == 0 && propertyContainerId != 0) {
      myfprintf(context, archiveFile, "properties");
      RescueBuffer buf1(context, 10);
      buf1 << propertyContainerId;
      buf1.Archive(archiveFile);
    }
    myfprintf(context, archiveFile, "EOD");
  }
}

RescueSurfaceToGrid::RescueSurfaceToGrid(RescueContext *context, FILE *archiveFile)
    : RescueObject(context), properties(0), propertyContainerId(0) {
  isA = R_RescueSurfaceToGrid;
  ReadId(context, archiveFile);
  myfscanf(context, archiveFile, &surfaceID);
  myfscanf(context, archiveFile, &listLength);
  iList = new RESCUEINT64[(size_t)listLength];
  jList = new RESCUEINT64[(size_t)listLength];
  myfscanf(context, archiveFile, iList, listLength);
  myfscanf(context, archiveFile, jList, listLength);
  if (context->ReadFileVersion() >= 37) {
    RESCUECHAR myString[255];

    myfgets(context, myString, 255, archiveFile);
    while (strcmp(myString, "EOD") != 0) {
      if (strcmp(myString, "properties") == 0) {
        RescueBuffer buf(context, archiveFile);
        buf >> propertyContainerId;
      } else {
        RescueBuffer buf(context, archiveFile);
      }
      myfgets(context, myString, 255, archiveFile);
    }
  }
}

void RescueSurfaceToGrid::Relink(RescueObject *parent) {
  geometry = (RescueGeometry *)parent;
  RescueContext *context = geometry->ParentModel()->Context();
  if (context->ReadFileVersion() < 23) {
    RescueSurface *parentSurface = geometry->ParentModel()->SurfaceIdentifiedBy(surfaceID);
    surface = parentSurface->NthIJSurface(0);
  } else {
    surface = geometry->ParentModel()->IJSurfaceIdentifiedBy(surfaceID);
  }
  surface->AddSurfaceToGrid(this);
}

RescueSurfaceToGrid::~RescueSurfaceToGrid() {
  if (surface != 0) {
    surface->DropSurfaceToGrid(this);
  }
  delete[] iList;
  delete[] jList;
  if (properties != 0) {
    delete properties;
  }
}

RESCUEBOOL RescueSurfaceToGrid::IsOfType(_RescueObjectType thisType) {
  if (thisType == R_RescueSurfaceToGrid) {
    return TRUE;
  } else {
    return RescueObject::IsOfType(thisType);
  }
}

void RescueSurfaceToGrid::FindUniquePropertyNames(cSetString *container) {
  cSetRescueDataContainer *containers = DataContainers();
  if (containers != 0) {
    containers->FindUniquePropertyNames(container);
  }
}
