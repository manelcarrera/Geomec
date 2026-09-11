/********************************************************************

  RescueTriangulatedSurface.cpp

  A representation of a triangulated surface for RescueSurface.

  Rod Hanks,  June, 1999

*********************************************************************/
#include "RescueTriangleFace.h"
#include "RescueTriangleVertex.h"
#include "RescueModel.h"
#include "RescueTriangulatedSurface.h"
#include "RescueIdTree.h"
#include "RescueProperty.h"
#include "RescueBuffer.h"
#include "RescueBitArray.h"
#include <string.h>

#define TSURF_FILE_VERSION 1

void RescueTriangulatedSurface::RelinkPropertyGrids()
{
  RescueContext *context = parentSurface->ParentModel()->Context();
  RescueGrid *oldFaceGrid = 0;
  RescueGrid *oldNodeGrid = 0;
  if (faceGrid != 0)
  {
    oldFaceGrid = faceGrid;
    faceGrid = new RescueGrid(context, RescueCoordinateSystem::LUB, 0, faces->Count64());
  }
  if (nodeGrid != 0)
  {
    oldNodeGrid = nodeGrid;
    nodeGrid = new RescueGrid(context, RescueCoordinateSystem::LUB, 0, vertices->Count64());
  }
  if (oldFaceGrid != 0 || oldNodeGrid != 0)
  {
    RESCUEINT64 ordinal = 0;
    RescueProperty *property = parentSurface->NthRescueProperty(ordinal++);
    while (property != 0)
    {
      if (property->TriangulatedSurface())
      {
        if (property->PropertyAttachedTo() == RescueProperty::FACE_CENTERED)
        {
          property->Data()->SetGrid(faceGrid);
        }
        else
        {
          property->Data()->SetGrid(nodeGrid);
        }
      }
      property = parentSurface->NthRescueProperty(ordinal++);
    }
    if (oldFaceGrid != 0)
    {
      delete oldFaceGrid;
    }
    if (oldNodeGrid != 0)
    {
      delete oldNodeGrid;
    }
  }
}

RescueGrid *RescueTriangulatedSurface::Grid(RescueProperty::AttachmentPoint howApplies)
{
  RescueContext *context = parentSurface->ParentModel()->Context();
  if (howApplies == RescueProperty::FACE_CENTERED)
  {
    if (faceGrid == 0)
    {
      faceGrid = new RescueGrid(context, RescueCoordinateSystem::LUB, 0, faces->Count64());
    }
    return faceGrid;
  }
  else
  {
    if (nodeGrid == 0)
    {
      nodeGrid = new RescueGrid(context, RescueCoordinateSystem::LUB, 0, vertices->Count64());
    }
    return nodeGrid;
  }
}

RESCUEBOOL RescueTriangulatedSurface::DropVertex(RescueTriangleVertex *toDrop)  
{
  toDrop->DropSelf(faces);
/*
  Cascades the faces.
*/
  return (*vertices) -= toDrop;
}

RESCUEBOOL RescueTriangulatedSurface::DropFace(RescueTriangleFace *toDrop)   
{
  toDrop->DropSelf();
  return (*faces) -= toDrop;
}

RESCUEFLOAT *RescueTriangulatedSurface::Vertices(RESCUEINT32 &vertexCount)
{
  vertexCount = (RESCUEINT32) vertices->Count64();
  RESCUEFLOAT *myReturn = new RESCUEFLOAT[(size_t) (vertexCount * 3)];
  RESCUEINT64 ndx = 0;
  RESCUEINT64 loop;
  for (loop = 0; loop < vertexCount; loop++)
  {
    RescueTriangleVertex *vertex = vertices->NthObject(loop);
    myReturn[ndx++] = vertex->X();
    myReturn[ndx++] = vertex->Y();
    myReturn[ndx++] = vertex->Z();
  }
  return myReturn;
}

RESCUEFLOAT *RescueTriangulatedSurface::Vertices(RESCUEINT64 &vertexCount)
{
  vertexCount = vertices->Count64();
  RESCUEFLOAT *myReturn = new RESCUEFLOAT[(size_t) (vertexCount * 3)];
  RESCUEINT64 ndx = 0;
  RESCUEINT64 loop;
  for (loop = 0; loop < vertexCount; loop++)
  {
    RescueTriangleVertex *vertex = vertices->NthObject(loop);
    myReturn[ndx++] = vertex->X();
    myReturn[ndx++] = vertex->Y();
    myReturn[ndx++] = vertex->Z();
  }
  return myReturn;
}

RESCUEINT32 *RescueTriangulatedSurface::Faces(RESCUEINT32 &faceCount)
{
  faceCount = (RESCUEINT32) faces->Count64();
  RESCUEINT32 *myReturn = new RESCUEINT32[(size_t) (faceCount * 3)];
  RESCUEINT64 ndx = 0;
  RESCUEINT64 loop;
  for (loop = 0; loop < faceCount; loop++)
  {
    RescueTriangleFace *face = faces->NthObject(loop);
    RESCUEINT64 subloop;
    for (subloop = 0; subloop < 3; subloop++)
    {
      RescueTriangleVertex *vertex = face->NthVertex(subloop);
      myReturn[ndx++] = (RESCUEINT32) vertices->IndexOf(vertex);
    }
  }
  return myReturn;
}

RESCUEINT64 *RescueTriangulatedSurface::Faces(RESCUEINT64 &faceCount)
{
  faceCount = faces->Count64();
  RESCUEINT64 *myReturn = new RESCUEINT64[(size_t) (faceCount * 3)];
  RESCUEINT64 ndx = 0;
  RESCUEINT64 loop;
  for (loop = 0; loop < faceCount; loop++)
  {
    RescueTriangleFace *face = faces->NthObject(loop);
    RESCUEINT64 subloop;
    for (subloop = 0; subloop < 3; subloop++)
    {
      RescueTriangleVertex *vertex = face->NthVertex(subloop);
      myReturn[ndx++] = vertices->IndexOf(vertex);
    }
  }
  return myReturn;
}


RescueTriangulatedSurface::~RescueTriangulatedSurface()
{
  delete octTrees;
  delete vertices;
  delete faces;
  if (faceGrid != 0)
  {
    delete faceGrid;
  }
  if (nodeGrid != 0)
  {
    delete nodeGrid;
  }
  if (faceStatus != 0)
  {
    delete faceStatus;
  }
}

RescueTriangulatedSurface::RescueTriangulatedSurface(RescueContext *context, FILE *archiveFile)
                                                    :RescueObject(context)
                                                    ,verticesLookups(0)
                                                    ,vertices(new cSetRescueTriangleVertex)
                                                    ,faces(new cSetRescueTriangleFace)
                                                    ,octTrees(new cSetRescueSOctTreeNode)
                                                    ,faceGrid(0)
                                                    ,nodeGrid(0)
                                                    ,arrayVersion(0)
                                                    ,hasChanged(FALSE)
                                                    ,loaded(FALSE)
                                                    ,faceStatus(0)
{
  ReadId(context, archiveFile);
  if (context->ReadFileVersion() >= 37)
  {
    RESCUECHAR myString[255];

    myfgets(context, myString, 255, archiveFile);
    while (strcmp(myString, "EOD") != 0)
    {
      if (strcmp(myString, "faceStatus") == 0)
      {
        RescueBuffer buf(context, archiveFile);
        if (faceStatus != 0)
        {
          delete faceStatus;
        }
        faceStatus = new RescueBitArray(context, buf);
      }
      else
      {
        RescueBuffer buf(context, archiveFile);
      }
      myfgets(context, myString, 255, archiveFile);
    }
  }
}

void RescueTriangulatedSurface::Archive(FILE *archiveFile)
{
  RescueContext *context = parentSurface->ParentModel()->Context();
  myfprintf(context, archiveFile, "; RescueTriangulatedSurface");
  myfprintf(context, archiveFile, Identifier());
  if (context->FileVersion() >= 37)
  {
    if (faceStatus != 0)
    {
      myfprintf(context, archiveFile, "faceStatus");
      RescueBuffer buf1(context, 100);
      faceStatus->Archive(buf1);
      buf1.Archive(archiveFile);
    }
    myfprintf(context, archiveFile, "EOD");
  }
  if (IsLoaded())
  {
    if (context->delayedGeometryLoad == FALSE
    ||  hasChanged == TRUE
    ||  parentSurface->ParentModel()->geometryActionImmediate == TRUE)
    {
      WriteData();
    }
  }
  else if (parentSurface->ParentModel()->geometryActionImmediate)
  {
    if (ReadData(parentSurface->ParentModel()->oldPathName))
    {
      WriteData();
      DropMemory();
    }
  }
}

void RescueTriangulatedSurface::Relink(RescueObject *parentSurfaceIn)
{
  parentSurface = (RescueSurface *) parentSurfaceIn;
}

RESCUEBOOL RescueTriangulatedSurface::Contains(RescueTriangleVertex *vertex)
{
  RESCUEBOOL myReturn = FALSE;
  RESCUEINT64 ndx = vertices->IndexOf(vertex);
  if (ndx >= 0)
  {
    myReturn = TRUE;
  }
  return myReturn;
}

RescueTriangleVertex *RescueTriangulatedSurface::TriangleVertexIdentifiedBy(RESCUEINT64 id)
{
  RescueTriangleVertex *myReturn = 0;    
  if (verticesLookups != 0)
  {
    myReturn = (RescueTriangleVertex *) verticesLookups->Find(id);
  }
  return myReturn;
}

void RescueTriangulatedSurface::ArchiveData(FILE *archiveFile)
{
  RescueContext *context = parentSurface->ParentModel()->Context();
  vertices->Archive(context, archiveFile);
  faces->Archive(context, archiveFile);
  octTrees->Archive(context, archiveFile);
  if (context->FileVersion() >= 37)
  {
    myfprintf(context, archiveFile, "EOD");
  }
}

void RescueTriangulatedSurface::UnArchiveData(FILE *archiveFile, RESCUEINT64 fileVersion)
{
  RescueContext *context = parentSurface->ParentModel()->Context();
  vertices->UnArchive(context, archiveFile);
  faces->UnArchive(context, archiveFile);

  RescueModel *model = parentSurface->ParentModel();
  if (model->IsWireframeLoaded() == FALSE)
  {
    RESCUECHAR *fileName = (model->oldPathName == 0) ? model->currentPathName : model->oldPathName;
    if (model->ReadWireframeData(fileName))
    {
      model->wireframeHasChanged = FALSE;
    }
  }
  if (context->ReadFileVersion() < 23)
  {
    verticesLookups = new RescueIdTree();
    RESCUEINT64 loop;
    RESCUEINT64 howMany = vertices->Count64();
    for (loop = 0; loop < howMany; loop++)
    {
      verticesLookups->Add(vertices->NthObject(loop));
    }
  }
  vertices->Relink(this);
  faces->Relink(this);
  if (verticesLookups != 0)
  {
    delete verticesLookups;
    verticesLookups = 0;
  }
  loaded = TRUE;
  RelinkPropertyGrids();

  octTrees->UnArchive(context, archiveFile, this);
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

void RescueTriangulatedSurface::DropMemory()
{
  vertices->EmptySelf();
  faces->EmptySelf();
  octTrees->EmptySelf();
  loaded = FALSE;
  hasChanged = FALSE;
}

RESCUEBOOL RescueTriangulatedSurface::Load()
{
  RescueModel *parentModel = parentSurface->ParentModel();
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

RESCUEINT32 RescueTriangulatedSurface::Version(RESCUEBOOL reload)
{
  RescueModel *parentModel = parentSurface->ParentModel();
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

RESCUEBOOL RescueTriangulatedSurface::Unload()
{
  RescueModel *parentModel = parentSurface->ParentModel();
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

RESCUEBOOL RescueTriangulatedSurface::ReadData(RESCUECHAR *basePathName, RESCUEBOOL versionOnly)
{
  RescueContext *context = parentSurface->ParentModel()->Context();
  parentSurface->ParentModel()->LoadContext();
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
      fread(buffer, sizeof(RESCUECHAR), 17, archiveFile);
      if (strncmp(buffer, "escue TSurf File", 16) == 0)
      {
        modelFile = TRUE;
      }
    }
    else
    {
      context->binaryFlag = TRUE;
      fread(buffer, sizeof(RESCUECHAR), 17, archiveFile);
      if (strncmp(buffer, "Rescue TSurf File", 17) == 0)
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
        fseek(archiveFile, 17, SEEK_CUR);
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
        sscanf(myString, "Rescue TSurf File Version %d\n", &fileVersion);
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
      message << "Not a Rescue TSurf File:" << fileName.NonNullString();
      context->SetError(message.NonNullString());
    }
  }
  return myReturn;
}

RESCUEBOOL RescueTriangulatedSurface::WriteData()
{
  RescueModel *parentModel = parentSurface->ParentModel();
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
    RescueContext *context = parentModel->Context(true);
    context->binaryFlag = parentModel->currentBinary;
    if (context->binaryFlag)
    {
      putc((RESCUEUCHAR) TSURF_FILE_VERSION, archiveFile);
      fwrite("Rescue TSurf File", sizeof(RESCUECHAR), 17, archiveFile);
    }
    else
    {
      fprintf(archiveFile, "Rescue TSurf File Version %d\n", TSURF_FILE_VERSION);
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
      message << "Error writing to TSurf file:";
      message << fileName.NonNullString();
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

void RescueTriangulatedSurface::RescueDeleteFile()
{
  if (parentSurface != 0)
  {
    RescueModel *parentModel = parentSurface->ParentModel();
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
}

RESCUEBOOL RescueTriangulatedSurface::FileTruncated()
{
  RCHString fileName;
  fileName << parentSurface->ParentModel()->currentPathName << "." << Identifier();
  return parentSurface->ParentModel()->Context()->FileTruncatedPrimitive(fileName.String());
}

void RescueTriangulatedSurface::SetFaceStatus(RESCUEINT64 i, RescueIJSurface::RescueFaceStatus newStatus)
{
  if (faceStatus == 0)
  {
    faceStatus = new RescueBitArray(parentSurface->ParentModel()->Context(), faces->Count64(), 2);
  }
  faceStatus->SetBitField(i, newStatus);
}

RescueIJSurface::RescueFaceStatus RescueTriangulatedSurface::FaceStatus(RESCUEINT64 i)
{
  if (faceStatus == 0)
  {
    return RescueIJSurface::R_FACE_ACTIVE;
/*
  Default value is R_FACE_ACTIVE, and none have been explicitly set yet, so . . .
*/
  }
  else
  {
    return (RescueIJSurface::RescueFaceStatus) faceStatus->BitField(i);
  }
}

void RescueTriangulatedSurface::SwapAxes(bool swapI, RescueGridAxis *iAxis, 
                                         bool swapJ, RescueGridAxis *jAxis)
{
  if (IsLoaded() == FALSE)
  {
    Load();
  }
  RESCUEFLOAT iAxisBegin = (RESCUEFLOAT) iAxis->LowBound64();
  RESCUEFLOAT iAxisEnd = (RESCUEFLOAT) iAxis->LowBound64() + (iAxis->Count64() - 1);
  RESCUEFLOAT jAxisBegin = (RESCUEFLOAT) jAxis->LowBound64();
  RESCUEFLOAT jAxisEnd = (RESCUEFLOAT) jAxis->LowBound64() + (jAxis->Count64() - 1);
  RESCUEINT64 vOrd = 0;
  RescueTriangleVertex *vertex = vertices->NthObject(vOrd++);
  while (vertex != 0)
  {
    if (swapI)
    {
      vertex->u = iAxisEnd - (vertex->u - iAxisBegin);
    }
    if (swapJ)
    {
      vertex->v = jAxisEnd - (vertex->v - jAxisBegin);
    }
    vertex = vertices->NthObject(vOrd++);
  }

  MarkChanged();
  Unload();
}

RESCUEINT32 RescueTriangulatedSurface::VertexCount(RESCUEBOOL throwIfTooBig)
{
  RESCUEINT64 output = VertexCount64();
  if (throwIfTooBig)
  {
    if (output > 2147483647 || output < -2147483647)
    {
      throw "Model is too large to be read in 32 bit mode.";
    }
  }
  return (RESCUEINT32) output;
}

RESCUEINT32 RescueTriangulatedSurface::FaceCount(RESCUEBOOL throwIfTooBig)
{
  RESCUEINT64 output = FaceCount64();
  if (throwIfTooBig)
  {
    if (output > 2147483647 || output < -2147483647)
    {
      throw "Model is too large to be read in 32 bit mode.";
    }
  }
  return (RESCUEINT32) output;
}

RESCUEINT32 RescueTriangulatedSurface::OctTreeCount(RESCUEBOOL throwIfTooBig)
{
  RESCUEINT64 output = OctTreeCount64();
  if (throwIfTooBig)
  {
    if (output > 2147483647 || output < -2147483647)
    {
      throw "Model is too large to be read in 32 bit mode.";
    }
  }
  return (RESCUEINT32) output;
}



