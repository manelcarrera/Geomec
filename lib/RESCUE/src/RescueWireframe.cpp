/********************************************************************

  RescueWireframe.cpp

  The EdgeSet, TrimLoop, Polyline and Vertex objects.

  Rod Hanks,  March 2001

*********************************************************************/
#include "RescueWireframe.h"
#include "RCHString.h"
#include "RescueBuffer.h"
#include "RescueGeobody.h"
#include "RescueHistory.h"
#include "RescueModel.h"
#include "RescueWireframeCallBack.h"
#include "RescueWireframeOwner.h"
#include <string.h>

RESCUEINT64 RescueWireframe::PolyLineCount64() {
  if (wireframeLoaded == FALSE) {
    return lastPolyLineCount;
  } else {
    return polyLines->Count64();
  }
}

RESCUEINT64 RescueWireframe::VertexCount64() {
  if (wireframeLoaded == FALSE) {
    return lastTrimVertexCount;
  } else {
    return trimVertexes->Count64();
  }
}

RESCUEINT64 RescueWireframe::EdgeSetCount64() {
  if (wireframeLoaded == FALSE) {
    return lastEdgeSetCount;
  } else {
    return edgeSets->Count64();
  }
}

RESCUEINT64 RescueWireframe::TrimLoopCount64() {
  if (wireframeLoaded == FALSE) {
    return lastTrimLoopCount;
  } else {
    return trimLoops->Count64();
  }
}

RescueWireframe::~RescueWireframe() {
  delete polyLines;
  delete trimVertexes;
  delete edgeSets;
  delete trimLoops;
}

RescueWireframe::RescueWireframe(RescueModel *modelIn, RescueWireframeOwner *associatedBlockUnit)
    : RescueHistoryObject(modelIn->Context()) {
  isA = R_RescueWireframe;
  model = modelIn;
  (*model->wireframes) += this;
  owner = associatedBlockUnit;
  associatedBlockUnit->wireframe = this;
  polyLines = new cSetRescuePolyLine();
  trimVertexes = new cSetRescueTrimVertex();
  edgeSets = new cSetRescueEdgeSet();
  trimLoops = new cSetRescueTrimLoop();
  loadNo = 0;
  wireframeLoaded = TRUE;
  wireframeHasChanged = TRUE;
  isDeleted = FALSE;
  lastTrimVertexCount = 0;
  lastPolyLineCount = 0;
  lastEdgeSetCount = 0;
  lastTrimLoopCount = 0;
  wireframeVersion = 0;
  x1 = 0;
  x2 = 0;
  x3 = 0;
  x4 = 0;
  y1 = 0;
  y2 = 0;
  y3 = 0;
  y4 = 0;
  z1 = 0;
  z2 = 0;
}

void RescueWireframe::AddPolyLine(RescuePolyLine *toAdd) {
  if (IsWireframeLoaded() == FALSE) {
    LoadWireframe();
  }
  (*polyLines) += toAdd;
  wireframeHasChanged = TRUE;
}

void RescueWireframe::AddTrimVertex(RescueTrimVertex *toAdd) {
  if (IsWireframeLoaded() == FALSE) {
    LoadWireframe();
  }
  (*trimVertexes) += toAdd;
  wireframeHasChanged = TRUE;
}

void RescueWireframe::AddEdgeSet(RescueEdgeSet *toAdd) {
  if (IsWireframeLoaded() == FALSE) {
    LoadWireframe();
  }
  (*edgeSets) += toAdd;
  wireframeHasChanged = TRUE;
}

void RescueWireframe::AddTrimLoop(RescueTrimLoop *toAdd) {
  if (IsWireframeLoaded() == FALSE) {
    LoadWireframe();
  }
  (*trimLoops) += toAdd;
  wireframeHasChanged = TRUE;
}

void RescueWireframe::Archive(FILE *archiveFile) {
  RescueContext *context = model->Context();
  if (IsWireframeLoaded()) {
    lastTrimVertexCount = trimVertexes->Count64();
    lastPolyLineCount = polyLines->Count64();
    lastEdgeSetCount = edgeSets->Count64();
    lastTrimLoopCount = trimLoops->Count64();
  }
  myfprintf(context, archiveFile, "; RescueWireframe");
  myfprintf(context, archiveFile, Identifier());
  myfprintf(context, archiveFile, (RESCUEINT64)Owner()->IsA());
  myfprintf(context, archiveFile, Owner()->Identifier());
  myfprintf(context, archiveFile, lastTrimVertexCount);
  myfprintf(context, archiveFile, lastPolyLineCount);
  myfprintf(context, archiveFile, lastEdgeSetCount);
  myfprintf(context, archiveFile, lastTrimLoopCount);
  if (context->FileVersion() >= 29) {
    myfprintf(context, archiveFile, x1);
    myfprintf(context, archiveFile, x2);
    myfprintf(context, archiveFile, x3);
    myfprintf(context, archiveFile, x4);
    myfprintf(context, archiveFile, y1);
    myfprintf(context, archiveFile, y2);
    myfprintf(context, archiveFile, y3);
    myfprintf(context, archiveFile, y4);
    myfprintf(context, archiveFile, z1);
    myfprintf(context, archiveFile, z2);
  }
  if (context->FileVersion() >= 37) {
    myfprintf(context, archiveFile, "EOD");
  }
}

RescueWireframe::RescueWireframe(RescueContext *context, FILE *archiveFile) : RescueHistoryObject(context) {
  isA = R_RescueWireframe;
  ReadId(context, archiveFile);
  myfscanf(context, archiveFile, &ownerIsA);
  myfscanf(context, archiveFile, &ownerId);
  myfscanf(context, archiveFile, &lastTrimVertexCount);
  myfscanf(context, archiveFile, &lastPolyLineCount);
  myfscanf(context, archiveFile, &lastEdgeSetCount);
  myfscanf(context, archiveFile, &lastTrimLoopCount);
  model = 0;
  owner = 0;
  wireframeLoaded = FALSE;
  wireframeHasChanged = FALSE;
  loadNo = 0;
  isDeleted = FALSE;
  polyLines = new cSetRescuePolyLine();
  trimVertexes = new cSetRescueTrimVertex();
  edgeSets = new cSetRescueEdgeSet();
  trimLoops = new cSetRescueTrimLoop();

  if (context->ReadFileVersion() >= 29) {
    myfscanf(context, archiveFile, &x1);
    myfscanf(context, archiveFile, &x2);
    myfscanf(context, archiveFile, &x3);
    myfscanf(context, archiveFile, &x4);
    myfscanf(context, archiveFile, &y1);
    myfscanf(context, archiveFile, &y2);
    myfscanf(context, archiveFile, &y3);
    myfscanf(context, archiveFile, &y4);
    myfscanf(context, archiveFile, &z1);
    myfscanf(context, archiveFile, &z2);
  }
  if (context->ReadFileVersion() >= 37) {
    RESCUECHAR myString[255];

    myfgets(context, myString, 255, archiveFile);
    while (strcmp(myString, "EOD") != 0) {
      RescueBuffer buf(context, archiveFile);
      myfgets(context, myString, 255, archiveFile);
    }
  }
}

void RescueWireframe::DeleteWireframe() {
  isDeleted = TRUE;
  DropWireframeMemory();
  RCHString fileName;
  fileName << model->currentPathName << "." << Identifier();
  model->ScheduleDeleteFile(fileName.String());
}

void RescueWireframe::Relink(RescueObject *parentModel) {
  model = (RescueModel *)parentModel;
  owner = (RescueWireframeOwner *)RescueHistory::FindObject(model, (_RescueObjectType)ownerIsA, ownerId);
  if (owner != 0) {
    owner->wireframe = this;
  }
}

RESCUEINT64 RescueWireframe::WireframeVersion(RESCUEBOOL reload) {
  if (reload) {
    RESCUEINT64 myReturn = wireframeVersion;
    RESCUEINT64 savedArrayVersion = wireframeVersion;
    if (ReadWireframeData(model->currentPathName, TRUE)) {
      myReturn = wireframeVersion;
      wireframeVersion = savedArrayVersion;
    }
    return myReturn;
  } else {
    return wireframeVersion;
  }
}

RESCUEBOOL RescueWireframe::LoadWireframe() {
  RESCUEBOOL myReturn = FALSE;
  if (IsWireframeLoaded()) {
    DropWireframeMemory();
  }
  if (model->ExistingModel()) {
    model->Context()->readFileVersion = model->currentFileVersion;
    model->Context()->readFileMainSoftwareVersion = model->readFileMainSoftwareVersion;
    model->Context()->readFileSubSoftwareVersion = model->readFileSubSoftwareVersion;
    myReturn = ReadWireframeData(model->currentPathName);
  }
  if (myReturn == TRUE) {
    wireframeHasChanged = FALSE;
  }
  return myReturn;
}

RESCUEBOOL RescueWireframe::UnloadWireframe() {
  RESCUEBOOL myReturn = FALSE;

  if (wireframeHasChanged == FALSE || model->desiredFileVersion < 28) {
    DropWireframeMemory();
    myReturn = TRUE;
  }
  /*
    Jonathan's change.  Only write a wireframe if the version we are using has one.
  */
  else if (IsWireframeLoaded() && model->ExistingModel()) {
    model->Context()->fileVersion = model->desiredFileVersion;
    if (WriteWireframeData()) {
      DropWireframeMemory();
      myReturn = TRUE;
    }
  }
  return myReturn;
}

RESCUEBOOL RescueWireframe::ReadWireframeData(RESCUECHAR *basePathName, RESCUEBOOL versionOnly) {
  RescueContext *context = model->Context();
  model->LoadContext();
  RESCUEBOOL myReturn = FALSE;
  RESCUECHAR buffer[30];
  RESCUEBOOL modelFile = FALSE;

  RCHString fileName;
  fileName << basePathName << "." << Identifier();
  FILE *archiveFile = (FILE *)fopen(fileName.String(), "rb");
  if (archiveFile != 0) {
    RESCUEINT64 readFileVersion = getc(archiveFile);
    if (readFileVersion == 'R') {
      context->binaryFlag = FALSE;
      fread(buffer, sizeof(RESCUECHAR), 21, archiveFile);
      if (strncmp(buffer, "escue Wireframe File", 20) == 0) {
        modelFile = TRUE;
      }
    } else {
      context->binaryFlag = TRUE;
      fread(buffer, sizeof(RESCUECHAR), 21, archiveFile);
      if (strncmp(buffer, "Rescue Wireframe File", 21) == 0) {
        modelFile = TRUE;
      }
    }
    fclose(archiveFile);
    if (modelFile) {
      int fileVersion = 0;
      if (context->binaryFlag) {
        archiveFile = (FILE *)fopen(fileName.String(), "rb");
        fileVersion = getc(archiveFile);
        context->readFileVersion = fileVersion;
        fseek(archiveFile, 21, SEEK_CUR);
        if (versionOnly) {
          myfscanf(context, archiveFile, &wireframeVersion);
        } else {
          UnArchiveWireframeData(model, archiveFile);
        }
        fclose(archiveFile);
      } else {
        RESCUECHAR myString[255];

        archiveFile = (FILE *)fopen(fileName.String(), "rt");
        myfgets(context, myString, 255, archiveFile);
        sscanf(myString, "Rescue Wireframe File Version %d\n", &fileVersion);
        context->readFileVersion = fileVersion;
        if (versionOnly) {
          myfscanf(context, archiveFile, &wireframeVersion);
        } else {
          UnArchiveWireframeData(model, archiveFile);
        }
        fclose(archiveFile);
      }
      myReturn = TRUE;
      if (versionOnly == FALSE) {
        wireframeLoaded = TRUE;
        wireframeHasChanged = FALSE;
      }
    } else {
      RCHString message;
      message << "Not a Rescue Wireframe File:" << fileName.String();
      context->SetError(message.NonNullString());
    }
  }
  return myReturn;
}

RescueTrimVertex *RescueWireframe::TrimVertexLocatedBy(RESCUEFLOAT x, RESCUEFLOAT y, RESCUEFLOAT z) {
  RescueTrimVertex *myReturn = 0;
  if (IsDeleted() == FALSE) {
    if (IsWireframeLoaded() == TRUE) {
      myReturn = trimVertexes->TrimVertexLocatedBy(x, y, z);
    }
  }
  return myReturn;
}

RescuePolyLine *RescueWireframe::PolyLineBetween(RescueTrimVertex *end1, RescueTrimVertex *end2) {
  RescuePolyLine *myReturn = 0;
  if (IsDeleted() == FALSE) {
    if (IsWireframeLoaded() == TRUE) {
      myReturn = polyLines->PolyLineBetween(end1, end2);
    }
  }
  return myReturn;
}

RESCUEUINT64 RescueWireframe::WireframeFileSize() {
  RCHString fileName;
  fileName << model->currentPathName << "." << Identifier();
  return model->Context()->FileSizePrimitive(fileName.String());
}

RESCUEBOOL RescueWireframe::WireframeFileTruncated() {
  if (model->Context()->fileVersion < 28 && model->Context()->readFileVersion < 28) {
    return FALSE;
  }
  /*
    If we are dealing with a file before v28 they did not have these types of
    wireframe files.
  */
  else {
    RCHString fileName;
    fileName << model->currentPathName << "." << Identifier();
    return model->Context()->FileTruncatedPrimitive(fileName.String());
  }
}

void RescueWireframe::UpdateWireframeData() {
  if (WireframeHasChanged() || model->geometryActionImmediate == TRUE) {
    if (model->geometryActionImmediate == TRUE) {
      if (IsWireframeLoaded() == false) {
        if (model->ExistingModel()) {
          int savedFileVersion = model->Context()->readFileVersion;
          model->Context()->readFileVersion = model->currentFileVersion;
          model->Context()->readFileMainSoftwareVersion = model->readFileMainSoftwareVersion;
          model->Context()->readFileSubSoftwareVersion = model->readFileSubSoftwareVersion;
          ReadWireframeData(model->oldPathName);
          model->Context()->readFileVersion = savedFileVersion;
        }
      }
    }
    WriteWireframeData();
  }
}

void RescueWireframe::CopyWireframeData(RESCUECHAR *oldPathName) {
  if (IsWireframeLoaded()) {
    WriteWireframeData();
  } else if (ReadWireframeData(oldPathName)) {
    WriteWireframeData();
  }
}

RESCUEBOOL RescueWireframe::WriteWireframeData() {
  RESCUEBOOL myReturn = FALSE;
  RCHString fileName;
  fileName << model->currentPathName << "." << Identifier();
  model->MakeBackupFile(fileName.String());
  FILE *archiveFile;
  if (model->currentBinary) {
    archiveFile = (FILE *)fopen(fileName.String(), "wb");
  } else {
    archiveFile = (FILE *)fopen(fileName.String(), "wt");
  }
  if (archiveFile != 0) {
    model->Context()->fileVersion = model->desiredFileVersion;
    model->Context()->binaryFlag = model->currentBinary;
    if (model->Context()->binaryFlag) {
      putc((RESCUEUCHAR)model->Context()->fileVersion, archiveFile);
      fwrite("Rescue Wireframe File", sizeof(RESCUECHAR), 21, archiveFile);
    } else {
      fprintf(archiveFile, "Rescue Wireframe File Version %d\n", model->Context()->fileVersion);
    }
    ArchiveWireframeData(archiveFile);
    myfprintf(model->Context(), archiveFile, "abracadabra jump jump");
    if (ferror(archiveFile) != 0) {
      RCHString message;
      message << "Error writing Rescue Wireframe File:";
      message << fileName.String();
      message << " (";
      message << (RESCUEINT64)ferror(archiveFile);
      message << ")";
      model->Context()->SetError(message.NonNullString());
    }
    fclose(archiveFile);
    myReturn = TRUE;
  }
  return myReturn;
}

void RescueWireframe::UnarchivePolyLines(FILE *archiveFile) {
  if (IsWireframeLoaded() == FALSE) {
    LoadWireframe();
  }
  RESCUEINT64 howMany;
  myfscanf(model->Context(), archiveFile, &howMany);
  RESCUEINT64 loop;
  for (loop = 0; loop < howMany; loop++) {
    RescuePolyLine *polyLine = new RescuePolyLine(model->Context(), archiveFile);
    (*polyLines) += polyLine;
  }
}

void RescueWireframe::UnarchiveTrimVertexes(FILE *archiveFile) {
  if (IsWireframeLoaded() == FALSE) {
    LoadWireframe();
  }
  RESCUEINT64 howMany;
  myfscanf(model->Context(), archiveFile, &howMany);
  RESCUEINT64 loop;
  for (loop = 0; loop < howMany; loop++) {
    RescueTrimVertex *trimVertex = new RescueTrimVertex(model->Context(), archiveFile);
    (*trimVertexes) += trimVertex;
  }
}

void RescueWireframe::UnArchiveWireframeData(RescueModel *model, FILE *archiveFile) {
  if (model->wireframeCallBackObject != 0) {
    model->wireframeCallBackObject->LoadingWireframe(this);
  }

  myfscanf(model->Context(), archiveFile, &wireframeVersion);
  model->Context()->RescueProgress("Poly Lines");
  polyLines->UnArchive(model->Context(), archiveFile);
  model->Context()->RescueProgress("Trim Vertexes");
  trimVertexes->UnArchive(model->Context(), archiveFile);
  model->Context()->RescueProgress("Edge Sets");
  edgeSets->UnArchive(model->Context(), archiveFile);
  model->Context()->RescueProgress("Trim Loops");
  trimLoops->UnArchive(model->Context(), archiveFile);

  if (model->Context()->ReadFileVersion() >= 37) {
    RESCUECHAR myString[255];

    myfgets(model->Context(), myString, 255, archiveFile);
    while (strcmp(myString, "EOD") != 0) {
      RescueBuffer buf(model->Context(), archiveFile);
      myfgets(model->Context(), myString, 255, archiveFile);
    }
  }

  polyLines->Relink(this);
  trimVertexes->Relink(this);
  edgeSets->Relink(this);
  trimLoops->Relink(this);

  loadNo++;
}

void RescueWireframe::CalculateBox(RESCUEBOOL &firstIteration, RESCUEFLOAT x, RESCUEFLOAT y, RESCUEFLOAT z) {
  if (firstIteration) {
    x1 = x;
    x2 = x;
    x3 = x;
    x4 = x;
    y1 = y;
    y2 = y;
    y3 = y;
    y4 = y;
    z1 = z;
    z2 = z;
    firstIteration = FALSE;
  } else {
    if (x1 > x) {
      x1 = x;
    }
    if (x2 < x) {
      x2 = x;
    }
    if (y1 > y) {
      y1 = y;
    }
    if (y2 < y) {
      y2 = y;
    }
    if (z1 > z) {
      z1 = z;
    }
    if (z2 < z) {
      z2 = z;
    }

    RESCUEFLOAT xPrime = x - y;
    if (x3 > xPrime) {
      x3 = xPrime;
    }
    if (x4 < xPrime) {
      x4 = xPrime;
    }

    RESCUEFLOAT yPrime = y + x;
    if (y3 > yPrime) {
      y3 = yPrime;
    }
    if (y4 < yPrime) {
      y4 = yPrime;
    }
  }
}

void RescueWireframe::ArchiveWireframeData(FILE *archiveFile) {
  RESCUEBOOL firstIteration = TRUE;
  lastTrimVertexCount = trimVertexes->Count64();
  lastPolyLineCount = polyLines->Count64();
  lastEdgeSetCount = edgeSets->Count64();
  lastTrimLoopCount = trimLoops->Count64();
  RESCUEINT64 loop;
  for (loop = 0; loop < lastTrimVertexCount; loop++) {
    RescueTrimVertex *vertex = trimVertexes->NthObject(loop);
    CalculateBox(firstIteration, (RESCUEFLOAT)vertex->X(), (RESCUEFLOAT)vertex->Y(), (RESCUEFLOAT)vertex->Z());
  }
  for (loop = 0; loop < lastPolyLineCount; loop++) {
    RescuePolyLine *pLine = polyLines->NthObject(loop);
    RESCUEINT64 subloop;
    RESCUEINT64 howMany = pLine->InnerNodeCount64();
    for (subloop = 0; subloop < howMany; subloop++) {
      RescuePolyLineNode *node = pLine->NthInnerNode(subloop);
      CalculateBox(firstIteration, (RESCUEFLOAT)node->X(), (RESCUEFLOAT)node->Y(), (RESCUEFLOAT)node->Z());
    }
    /*
      On poly lines, we are not concerned about the vertexes.  If they are part of this
      wireframe we will catch them in the loop above.  If not, they are none of our
      business.
    */
  }
  x3 += y1;
  x4 += y1;
  y3 -= x1;
  y4 -= x1;
/*
  Above, calculate all the stuff we store in the RescueModel wireframe file that will
  help users see what is in this file.
*/
#ifdef TESTING
  myfprintf(model->Context(), archiveFile, wireframeVersion);
#else
  myfprintf(model->Context(), archiveFile, ++wireframeVersion);
#endif
  model->Context()->RescueProgress("Poly Lines");
  polyLines->Archive(model->Context(), archiveFile);
  model->Context()->RescueProgress("Trim Vertexes");
  trimVertexes->Archive(model->Context(), archiveFile);
  model->Context()->RescueProgress("Edge Sets");
  edgeSets->Archive(model->Context(), archiveFile);
  model->Context()->RescueProgress("Trim Loops");
  trimLoops->Archive(model->Context(), archiveFile);
  if (model->Context()->FileVersion() >= 37) {
    myfprintf(model->Context(), archiveFile, "EOD");
  }
}

void RescueWireframe::DropWireframeMemory() {
  if (model->wireframeCallBackObject != 0) {
    model->wireframeCallBackObject->UnloadingWireframe(this);
  }

  if (polyLines != 0) {
    delete polyLines;
    polyLines = new cSetRescuePolyLine();
  }
  if (trimVertexes != 0) {
    delete trimVertexes;
    trimVertexes = new cSetRescueTrimVertex();
  }
  if (edgeSets != 0) {
    delete edgeSets;
    edgeSets = new cSetRescueEdgeSet();
  }
  if (trimLoops != 0) {
    delete trimLoops;
    trimLoops = new cSetRescueTrimLoop();
  }
  wireframeLoaded = FALSE;
  wireframeHasChanged = FALSE;
  loadNo++;
}

RESCUEBOOL RescueWireframe::IsOfType(_RescueObjectType thisType) {
  if (thisType == R_RescueWireframe) {
    return TRUE;
  } else {
    return RescueHistoryObject::IsOfType(thisType);
  }
}

void RescueWireframe::SetOrientation(RescueOrientationLedger *ledger) {
  LoadWireframe();
  RESCUEINT64 ordinal = 0;
  RescueTrimVertex *vertex = NthRescueTrimVertex(ordinal++);
  while (vertex != 0) {
    if (vertex->surfaceUVs != 0) {
      vertex->surfaceUVs->SetOrientation(ledger);
    }
    if (vertex->layerParams != 0) {
      vertex->layerParams->SetOrientation(ledger);
    }
    vertex = NthRescueTrimVertex(ordinal++);
  }
  ordinal = 0;
  RescuePolyLine *line = NthRescuePolyLine(ordinal++);
  while (line != 0) {
    RESCUEINT64 iOrd = 0;
    RescuePolyLineNode *node = line->NthInnerNode(iOrd++);
    while (node != 0) {
      if (node->surfaceUVs != 0) {
        node->surfaceUVs->SetOrientation(ledger);
      }
      if (node->layerParams != 0) {
        node->layerParams->SetOrientation(ledger);
      }
      node = line->NthInnerNode(iOrd++);
    }
    line = NthRescuePolyLine(ordinal++);
  }
  UnloadWireframe();
  MarkWireframeChanged();
}

RESCUEINT32 RescueWireframe::PolyLineCount(RESCUEBOOL throwIfTooBig) {
  RESCUEINT64 output = PolyLineCount64();
  if (throwIfTooBig) {
    if (output > 2147483647 || output < -2147483647) {
      throw "Model is too large to be read in 32 bit mode.";
    }
  }
  return (RESCUEINT32)output;
}

RESCUEINT32 RescueWireframe::VertexCount(RESCUEBOOL throwIfTooBig) {
  RESCUEINT64 output = VertexCount64();
  if (throwIfTooBig) {
    if (output > 2147483647 || output < -2147483647) {
      throw "Model is too large to be read in 32 bit mode.";
    }
  }
  return (RESCUEINT32)output;
}

RESCUEINT32 RescueWireframe::EdgeSetCount(RESCUEBOOL throwIfTooBig) {
  RESCUEINT64 output = EdgeSetCount64();
  if (throwIfTooBig) {
    if (output > 2147483647 || output < -2147483647) {
      throw "Model is too large to be read in 32 bit mode.";
    }
  }
  return (RESCUEINT32)output;
}

RESCUEINT32 RescueWireframe::TrimLoopCount(RESCUEBOOL throwIfTooBig) {
  RESCUEINT64 output = TrimLoopCount64();
  if (throwIfTooBig) {
    if (output > 2147483647 || output < -2147483647) {
      throw "Model is too large to be read in 32 bit mode.";
    }
  }
  return (RESCUEINT32)output;
}
