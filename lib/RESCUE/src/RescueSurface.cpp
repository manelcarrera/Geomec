/********************************************************************

  RescueSurface.cpp

  A surface in RESCUE terms is a parametric 2d grid (i,j), with the
  geometry described by an (x,y,z) triplet for each node of the grid.
  The coordinate system of the triplet is the global coordinate system 
  of the model.

  Rod Hanks,  May 1996

*********************************************************************/
#include "RescueSurface.h"
#include "RescueSurfaceCell.h"
#include "RescueSurfaceToGrid.h"
#include "RescueGridAxis.h"
#include "RescueModel.h"
#include "RescueEdgeSet.h"
#include "RescueEdgeSetStub.h"
#include "RescueColor.h"
#include "RescueVertex.h"
#include "RescueTriangulatedSurface.h"
#include "RescueIJSurface.h"
#include "RescueWireframe.h"
#include "RescueTriangleVertex.h"
#include "RescueTriangleFace.h"
#include "RescueBuffer.h"
#include "cSetRescueDataContainer.h"
#include <string.h>

RescueTriangulatedSurface *RescueSurface::SurfaceTriangulatedIdentifiedBy(RESCUEINT64 identifier)
{
  RescueTriangulatedSurface *myReturn = 0;
  if (triangulatedSurface != 0)
  {
  if (triangulatedSurface->Identifier() == identifier)
  {
      myReturn = triangulatedSurface;
  }
  }
  return myReturn;
}

void RescueSurface::AutoEdgeSet()
{
  RescueEdgeSet *edgeSet = new RescueEdgeSet(this);
  edgeSet->SetRescueBusinessOwner(this);
  edges = new RescueEdgeSetStub(parentModel->Context(), edgeSet);
}

RescueEdgeSetStub *RescueSurface::EdgesObj()
{
  if (ParentModel()->IsWireframeLoaded() == FALSE)
  {
  return 0;
  }
  else
  {
  if (edges == 0)
  {
      AutoEdgeSet();
  }
  return edges;
  }
}

RescueEdgeSet *RescueSurface::Edges()
{
  RescueModel *model = ParentModel();
  if (model->IsWireframeLoaded() == FALSE)
  {
  return 0;
  }
  else
  {
  if (edges == 0)
  {
      AutoEdgeSet();
  }
  RescueEdgeSet *myReturn = edges->EdgeSet(model);
  if (myReturn == 0)
  {
      delete edges;
      AutoEdgeSet();
      myReturn = edges->EdgeSet(model);
  }
  return myReturn;
  }
}

RESCUEBOOL RescueSurface::DropIJSurface(RescueIJSurface *toDrop)
{
  if (ijSurfaces->Count64() == 1)
  {
  return FALSE;
  }
  else
  {
  RESCUEINT64 ndx = 0;
  RescueProperty *property = NthRescueProperty(ndx);
  while (property != 0)
  {
      if (property->ParentIJSurface() == toDrop)
      {
    DropRescueProperty(property);
      }
      else
      {
    ndx++;
      }
      property = NthRescueProperty(ndx);
  }
  toDrop->Dispose();
  return ((*ijSurfaces) -= toDrop);
  }
}

void RescueSurface::Dispose()
{
  RESCUEINT64 ndx = 0;
  RescueIJSurface *ijSurface = NthIJSurface(ndx++);
  {
  ijSurface->Dispose();
  ijSurface = NthIJSurface(ndx++);
  }
}

RescueTriangulatedSurface *RescueSurface::CreateTriangulatedSurface(cSetRescueTriangleVertex *verticesIn,
                                  cSetRescueTriangleFace *facesIn)
{
  if (triangulatedSurface != 0)
  {
  triangulatedSurface->RescueDeleteFile();
  delete triangulatedSurface;
  }
  triangulatedSurface = new RescueTriangulatedSurface(ParentModel()->Context(), this, verticesIn, facesIn);
  return triangulatedSurface;
}

RescueTriangulatedSurface *RescueSurface::CreateTriangulatedSurface(RESCUEINT32 vertexCount,
                                  RESCUEFLOAT **coordinates,
                                  RESCUEINT32 faceCount,
                                  RESCUEINT32 **faceNdxs)
{
  RescueContext *context = ParentModel()->Context();
  cSetRescueTriangleVertex *vertices = new cSetRescueTriangleVertex();
  cSetRescueTriangleFace *faces = new cSetRescueTriangleFace();
  RESCUEINT32 loop;
  for (loop = 0; loop < vertexCount; loop++)
  {
  RESCUEFLOAT x = coordinates[0][loop];
  RESCUEFLOAT y = coordinates[1][loop];
  RESCUEFLOAT z = coordinates[2][loop];

  RescueTriangleVertex *vertex = new RescueTriangleVertex(context, x, y, z);
  (*vertices) += vertex;
  }
  for (loop = 0; loop < faceCount; loop++)
  {
  RescueTriangleVertex *v[3];
  RESCUEBOOL skip = FALSE;
  RESCUEINT32 subloop;
  for (subloop = 0; subloop < 3; subloop++)
  {
      RESCUEINT64 ndx = faceNdxs[subloop][loop];
      v[subloop] = vertices->NthObject(ndx);
      if (v[subloop] == 0)
      {
    skip = TRUE;
      }
  }
  if (skip == FALSE)
  {
      RescueTriangleFace *face = new RescueTriangleFace(v);
      (*faces) += face;
  }
  }
  return CreateTriangulatedSurface(vertices, faces);
}

RescueTriangulatedSurface *RescueSurface::CreateTriangulatedSurface(RESCUEINT64 vertexCount,
                                  RESCUEFLOAT **coordinates,
                                  RESCUEINT64 faceCount,
                                  RESCUEINT64 **faceNdxs)
{
  RescueContext *context = ParentModel()->Context();
  cSetRescueTriangleVertex *vertices = new cSetRescueTriangleVertex();
  cSetRescueTriangleFace *faces = new cSetRescueTriangleFace();
  RESCUEINT64 loop;
  for (loop = 0; loop < vertexCount; loop++)
  {
  RESCUEFLOAT x = coordinates[0][loop];
  RESCUEFLOAT y = coordinates[1][loop];
  RESCUEFLOAT z = coordinates[2][loop];

  RescueTriangleVertex *vertex = new RescueTriangleVertex(context, x, y, z);
  (*vertices) += vertex;
  }
  for (loop = 0; loop < faceCount; loop++)
  {
  RescueTriangleVertex *v[3];
  RESCUEBOOL skip = FALSE;
  RESCUEINT64 subloop;
  for (subloop = 0; subloop < 3; subloop++)
  {
      RESCUEINT64 ndx = faceNdxs[subloop][loop];
      v[subloop] = vertices->NthObject(ndx);
      if (v[subloop] == 0)
      {
    skip = TRUE;
      }
  }
  if (skip == FALSE)
  {
      RescueTriangleFace *face = new RescueTriangleFace(v);
      (*faces) += face;
  }
  }
  return CreateTriangulatedSurface(vertices, faces);
}

RescueTriangulatedSurface *RescueSurface::CreateTriangulatedSurface(RESCUEINT32 vertexCount,
                                  RESCUEFLOAT *coordinates,
                                  RESCUEINT32 faceCount,
                                  RESCUEINT32 *faceNdxs)
{
  RescueContext *context = ParentModel()->Context();
  cSetRescueTriangleVertex *vertices = new cSetRescueTriangleVertex();
  cSetRescueTriangleFace *faces = new cSetRescueTriangleFace();
  RESCUEINT32 ndx = 0;
  RESCUEINT32 loop;
  for (loop = 0; loop < vertexCount; loop++)
  {
  RESCUEFLOAT x = coordinates[ndx++];
  RESCUEFLOAT y = coordinates[ndx++];
  RESCUEFLOAT z = coordinates[ndx++];

  RescueTriangleVertex *vertex = new RescueTriangleVertex(context, x, y, z);
  (*vertices) += vertex;
  }
  ndx = 0;
  for (loop = 0; loop < faceCount; loop++)
  {
  RescueTriangleVertex *v[3];
  RESCUEBOOL skip = FALSE;
  RESCUEINT32 subloop;
  for (subloop = 0; subloop < 3; subloop++)
  {
      RESCUEINT32 faceNdx = faceNdxs[ndx++];
      v[subloop] = vertices->NthObject(faceNdx);
      if (v[subloop] == 0)
      {
    skip = TRUE;
      }
  }
  if (skip == FALSE)
  {
      RescueTriangleFace *face = new RescueTriangleFace(v);
      (*faces) += face;
  }
  }
  return CreateTriangulatedSurface(vertices, faces);
}

RescueTriangulatedSurface *RescueSurface::CreateTriangulatedSurface(RESCUEINT64 vertexCount,
                                  RESCUEFLOAT *coordinates,
                                  RESCUEINT64 faceCount,
                                  RESCUEINT64 *faceNdxs)
{
  RescueContext *context = ParentModel()->Context();
  cSetRescueTriangleVertex *vertices = new cSetRescueTriangleVertex();
  cSetRescueTriangleFace *faces = new cSetRescueTriangleFace();
  RESCUEINT64 ndx = 0;
  RESCUEINT64 loop;
  for (loop = 0; loop < vertexCount; loop++)
  {
  RESCUEFLOAT x = coordinates[ndx++];
  RESCUEFLOAT y = coordinates[ndx++];
  RESCUEFLOAT z = coordinates[ndx++];

  RescueTriangleVertex *vertex = new RescueTriangleVertex(context, x, y, z);
  (*vertices) += vertex;
  }
  ndx = 0;
  for (loop = 0; loop < faceCount; loop++)
  {
  RescueTriangleVertex *v[3];
  RESCUEBOOL skip = FALSE;
  RESCUEINT64 subloop;
  for (subloop = 0; subloop < 3; subloop++)
  {
      RESCUEINT64 faceNdx = faceNdxs[ndx++];
      v[subloop] = vertices->NthObject(faceNdx);
      if (v[subloop] == 0)
      {
    skip = TRUE;
      }
  }
  if (skip == FALSE)
  {
      RescueTriangleFace *face = new RescueTriangleFace(v);
      (*faces) += face;
  }
  }
  return CreateTriangulatedSurface(vertices, faces);
}

RescueSurface::~RescueSurface()
{
  if (color != 0)
  {
  delete color;
  }
  if (ijSurfaces != 0)
  {
  delete ijSurfaces;
  }
  if (edges != 0)
  {
  delete edges;
  }
  if (groups != 0)
  {
  delete groups;
  }
  if (properties != 0)
  {
  delete properties;
  }
  if (triangulatedSurface != 0)
  {
  delete triangulatedSurface;
  }
  if (propertyContainers != 0)
  {
  delete propertyContainers;
  }
}

RescueSurface::RescueSurface(RescueModel *modelIn,
                             RescueCoordinateSystem::Orientation orientation,
                             RescueCoordinateSystemAxis *i_axis,
                             RESCUEFLOAT i_origin, RESCUEFLOAT i_step,
                             RESCUEINT64 i_lowbound, RESCUEINT64 i_count,
                             RescueCoordinateSystemAxis *j_axis,
                             RESCUEFLOAT j_origin, RESCUEFLOAT j_step,
                             RESCUEINT64 j_lowbound, RESCUEINT64 j_count,
                             RESCUEFLOAT missingValue, 
                             SurfaceType typeIn)
                          :RescueWireframeOwner(modelIn->Context())
                          ,edges(0)
                          ,type(typeIn)
                          ,color(0)
                          ,properties(0)
                          ,groups(0)
                          ,triangulatedSurface(0)
                          ,ijSurfaces(0)
                          ,propertyContainers(0)
                          ,propertyContainerId(0)
{
  parentModel = modelIn;
  ijSurfaces = new cSetRescueIJSurface();
  groups = new cSetRescuePropertyGroup();
  properties = new cSetRescueProperty();
  isA = R_RescueSurface;
  new RescueIJSurface(this, orientation, i_axis, i_origin, i_step, i_lowbound,
                      i_count, j_axis, j_origin, j_step, j_lowbound, j_count, 
                      missingValue);
}

RescueSurface::RescueSurface(RescueModel *modelIn,
                             RescueCoordinateSystem::Orientation orientation,
                             RescueCoordinateSystemAxis *i_axis,
                             RESCUEFLOAT i_origin, RESCUEFLOAT i_step,
                             RESCUEINT64 i_lowbound, RESCUEINT64 i_count,
                             RescueCoordinateSystemAxis *j_axis,
                             RESCUEFLOAT j_origin, RESCUEFLOAT j_step,
                             RESCUEINT64 j_lowbound, RESCUEINT64 j_count,
                             RESCUEFLOAT missingValue, 
                             RESCUEFLOAT *values,
                             SurfaceType typeIn)
                          :RescueWireframeOwner(modelIn->Context())
                          ,edges(0)
                          ,type(typeIn)
                          ,color(0)
                          ,properties(0)
                          ,groups(0)
                          ,triangulatedSurface(0)
                          ,ijSurfaces(0)
                          ,propertyContainers(0)
                          ,propertyContainerId(0)
{
  parentModel = modelIn;
  ijSurfaces = new cSetRescueIJSurface();
  groups = new cSetRescuePropertyGroup();
  properties = new cSetRescueProperty();
  isA = R_RescueSurface;
  new RescueIJSurface(this, orientation, i_axis, i_origin, i_step, i_lowbound,
                      i_count, j_axis, j_origin, j_step, j_lowbound, j_count,
                      missingValue, values);
}

RescueSurface::RescueSurface(RescueModel *modelIn,
                             RescueCoordinateSystem::Orientation orientation,
                             RESCUEINT64 i_lowbound, RESCUEINT64 i_count,
                             RESCUEINT64 j_lowbound, RESCUEINT64 j_count,
                             RESCUEFLOAT missingValue,
                             SurfaceType typeIn)
                          :RescueWireframeOwner(modelIn->Context())
                          ,edges(0)
                          ,type(typeIn)
                          ,color(0)
                          ,properties(0)
                          ,groups(0)
                          ,triangulatedSurface(0)
                          ,ijSurfaces(0)
                          ,propertyContainers(0)
                          ,propertyContainerId(0)
{
  parentModel = modelIn;
  ijSurfaces = new cSetRescueIJSurface();
  groups = new cSetRescuePropertyGroup();
  properties = new cSetRescueProperty();
  isA = R_RescueSurface;
  new RescueIJSurface(this, orientation, i_lowbound, i_count,
                      j_lowbound, j_count, missingValue);
}

RescueSurface::RescueSurface(RescueModel *modelIn,
                             RescueCoordinateSystem::Orientation orientation,
                             RESCUEINT64 i_lowbound, RESCUEINT64 i_count,
                             RESCUEINT64 j_lowbound, RESCUEINT64 j_count,
                             RESCUEFLOAT missingValue, 
                             RESCUEFLOAT *tripletArray,
                             SurfaceType typeIn)
                          :RescueWireframeOwner(modelIn->Context())
                          ,edges(0)
                          ,type(typeIn)
                          ,color(0)
                          ,properties(0)
                          ,groups(0)
                          ,triangulatedSurface(0)
                          ,ijSurfaces(0)
                          ,propertyContainers(0)
                          ,propertyContainerId(0)
{
  parentModel = modelIn;
  ijSurfaces = new cSetRescueIJSurface();
  groups = new cSetRescuePropertyGroup();
  properties = new cSetRescueProperty();
  isA = R_RescueSurface;
  new RescueIJSurface(this, orientation, i_lowbound, i_count,
                      j_lowbound, j_count, missingValue, tripletArray);
}

RescueSurface::RescueSurface(RescueContext *context, FILE *archiveFile)
                          :RescueWireframeOwner(context)
                          ,edges(0)
                          ,color(0)
                          ,properties(0)
                          ,groups(0)
                          ,triangulatedSurface(0)
                          ,ijSurfaces(0)
                          ,propertyContainers(0)
                          ,propertyContainerId(0)
{
  ijSurfaces = new cSetRescueIJSurface();
  if (context->ReadFileVersion() < 23)
  {
  RescueIJSurface *oldSurface = new RescueIJSurface(context, archiveFile);
  (*ijSurfaces) += oldSurface;
  }
  if (context->ReadFileVersion() >= 9 && context->ReadFileVersion() <= 12)
  {
  RescueEdgeSet *edgesObj = new RescueEdgeSet(context, archiveFile);
  edges = new RescueEdgeSetStub(context, edgesObj);
  context->unarchivingModel->wireframes->SaveCompatibleEdgeSet(edgesObj, this, edges);
  }
  if (context->ReadFileVersion() >= 10)
  {
  RESCUEINT64 typeFlag;
  myfscanf(context, archiveFile, &typeFlag);
  type = (SurfaceType) typeFlag;
  }
  else
  {
  type = AUXILLIARY;
  }
  if (context->ReadFileVersion() >= 15)
  {
  RESCUEINT64 colorFlag = 0;
  myfscanf(context, archiveFile, &colorFlag);
  if (colorFlag == 1)
  {
      color = new RescueColor(context, archiveFile);
  }
  }
  properties = new cSetRescueProperty();
  groups = new cSetRescuePropertyGroup();
  if (context->ReadFileVersion() >= 20)
  {
  properties->UnArchive(context, archiveFile);
  groups->UnArchive(context, archiveFile);
  RESCUEINT64 flag;
  myfscanf(context, archiveFile, &flag);
  if (flag == 1)
  {
      triangulatedSurface = new RescueTriangulatedSurface(context, archiveFile);
  }
  }
  if (context->ReadFileVersion() >= 23)
  {
  ijSurfaces->UnArchive(context, archiveFile);
  }
  if (context->ReadFileVersion() >= 37)
  {
  RESCUECHAR myString[255];

  myfgets(context, myString, 255, archiveFile);
  while (strcmp(myString, "EOD") != 0)
  {
      if (strcmp(myString, "properties") == 0)
      {
    RescueBuffer buf(context, archiveFile);
    buf >> propertyContainerId;
      }
      else
      {
    RescueBuffer buf(context, archiveFile);
      }
      myfgets(context, myString, 255, archiveFile);
  }
  }
}

void RescueSurface::DropWireframeMemory()
{
  if (edges != 0)
  {
  delete edges;
  edges = 0;
  }
  properties->DropWireframeMemory();
}

void RescueSurface::UnArchiveWireframeData(RescueModel *model, FILE *archiveFile)
{
  RescueContext *context = parentModel->Context();
  if (edges != 0)
  {
  delete edges;
  edges = 0;
  }
  if (context->ReadFileVersion() >= 28)
  {
  RESCUEINT64 flag;
  myfscanf(context, archiveFile, &flag);
  if (flag > 0)
  {
      edges = new RescueEdgeSetStub(context, archiveFile);
  }
  }
  else
  {
  RescueEdgeSet *edgesObj = new RescueEdgeSet(context, archiveFile);
  edges = new RescueEdgeSetStub(context, edgesObj);
  model->wireframes->SaveCompatibleEdgeSet(edgesObj, this, edges);
  }
  if (context->ReadFileVersion() >= 20)
  {
  properties->UnArchiveWireframeData(model, archiveFile);
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

void RescueSurface::RelinkWireframeData(RescueObject *parent)
{
  RescueContext *context = ((RescueModel *) parent)->Context();
                    //RescueModel is passed.
  if (edges != 0)
  {
  if (context->ReadFileVersion() < 28)
  {
      edges->actualEdgeSet->Relink(edges->actualEdgeSet->owner);
  }
  }
  properties->RelinkWireframeData(parent);
}

void RescueSurface::ArchiveWireframeData(FILE *archiveFile)
{
  RescueContext *context = parentModel->Context();
  if (context->FileVersion() >= 28)
  {
  if (edges == 0)
  {
      myfprintf(context, archiveFile, (RESCUEINT64) 0);
  }
  else
  {
      myfprintf(context, archiveFile, (RESCUEINT64) 1);
      edges->ArchiveStub(context, archiveFile);
  }
  }
  else
  {
  RescueEdgeSet *edgesObj = 0;
  if (edges != 0)
  {
       edgesObj = edges->EdgeSet(ParentModel());
  }
  if (edgesObj == 0)
  {
      edgesObj = new RescueEdgeSet(this);
      if (edges != 0)
      {
    delete edges;
      }
      edges = new RescueEdgeSetStub(context, edgesObj);
  }
  edgesObj->Archive(archiveFile);
  }
  if (context->FileVersion() >= 20)
  {
  properties->ArchiveWireframeData(archiveFile);
  }
  if (context->FileVersion() >= 37)
  {
  myfprintf(context, archiveFile, "EOD");
  }
}

void RescueSurface::Relink(RescueObject *parentModelIn)
{
  parentModel = (RescueModel *) parentModelIn;
  ijSurfaces->Relink(this);
  if (triangulatedSurface != 0)
  {
  triangulatedSurface->Relink(this);
  }
  properties->Relink(this);
  groups->Relink(this);
}

void RescueSurface::Archive(FILE *archiveFile)
{
  RescueContext *context = parentModel->Context();
  myfprintf(context, archiveFile, "; Surface");
  RescueIJSurface *surfaceToArchive = 0;
  if (context->FileVersion() < 23)
  {
  surfaceToArchive = ijSurfaces->NthObject(0);
  }
/*
  This routine will seg-fault if they attempt to write an
  old-style file with no representation.  The only thing
  I could do would be to create a totally dummy representation,
  but I don't think I would really be doing anybody a favor
  by doing that.
*/
  if (context->FileVersion() < 23)
  {
  surfaceToArchive->surfaceGrid->Archive(context, archiveFile);
  RESCUEBOOL madeX = FALSE;
  RESCUEBOOL madeY = FALSE;
  RESCUEBOOL madeZ = FALSE;
  if (context->FileVersion() == 9)
  {
      if (surfaceToArchive->surfaceGeometry->xValue == 0)
      {
    surfaceToArchive->surfaceGeometry->RescueGetXValue();
      }
      if (surfaceToArchive->surfaceGeometry->yValue == 0)
      {
    surfaceToArchive->surfaceGeometry->RescueGetYValue();
      }
      if (surfaceToArchive->surfaceGeometry->zValue == 0)
      {
    surfaceToArchive->surfaceGeometry->RescueGetZValue();
      }
  }
/*
  Version 9 did not have equal-axis grids for surfaces,
  so if our triplet array is missing an axis, build one
  first.
*/
  surfaceToArchive->surfaceGeometry->Archive(archiveFile);
  if (context->FileVersion() == 9)
  {
      if (madeX)
      {
    delete [] surfaceToArchive->surfaceGeometry->xValue;
    surfaceToArchive->surfaceGeometry->xValue = 0;
      }
      if (madeY)
      {
    delete [] surfaceToArchive->surfaceGeometry->yValue;
    surfaceToArchive->surfaceGeometry->yValue = 0;
      }
      if (madeZ)
      {
    delete [] surfaceToArchive->surfaceGeometry->zValue;
    surfaceToArchive->surfaceGeometry->zValue = 0;
      }
  }
  }
  if (context->FileVersion() == 9)
  {
  RescueEdgeSet *edgesObj = 0;
  if (edges != 0)
  {
       edgesObj = edges->EdgeSet(ParentModel());
  }
  if (edgesObj == 0)
  {
      edgesObj = new RescueEdgeSet(this);
      if (edges != 0)
      {
    delete edges;
      }
      edges = new RescueEdgeSetStub(context, edgesObj);
  }
  edgesObj->Archive(archiveFile);
  }
  else
  {
  myfprintf(context, archiveFile, (RESCUEINT64) type);
  }
  if (context->FileVersion() >= 15)
  {
  if (color == 0)
  {
      myfprintf(context, archiveFile, (RESCUEINT64) 0);
  }
  else
  {
      myfprintf(context, archiveFile, (RESCUEINT64) 1);
      color->Archive(context, archiveFile);
  }
  }
  if (context->FileVersion() >= 20)
  {
  properties->Archive(context, archiveFile);
  groups->Archive(context, archiveFile);
  if (triangulatedSurface == 0)
  {
      myfprintf(context, archiveFile, (RESCUEINT64) 0);
  }
  else
  {
      myfprintf(context, archiveFile, (RESCUEINT64) 1);
      triangulatedSurface->Archive(archiveFile);
  }
  }
  if (context->FileVersion() >= 23)
  {
  ijSurfaces->Archive(context, archiveFile);
  }
  if (context->FileVersion() >= 37)
  {
  if (propertyContainers == 0 
  &&  propertyContainerId != 0 
  &&  ParentModel()->propertyActionImmediate == TRUE)
  {
      propertyContainers = new cSetRescueDataContainer(ParentModel(), propertyContainerId);  // Will Read.
  }
  if (propertyContainers != 0)
  {
      myfprintf(parentModel->Context(), archiveFile, "properties");
      RescueBuffer buf1(parentModel->Context(), 10);
      buf1 << propertyContainers->Identifier();
      buf1.Archive(archiveFile);

      propertyContainers->Archive(); // Goes into it's own file.
  }
  else if (propertyContainers == 0 && propertyContainerId != 0)
  {
      myfprintf(parentModel->Context(), archiveFile, "properties");
      RescueBuffer buf1(parentModel->Context(), 10);
      buf1 << propertyContainerId;
      buf1.Archive(archiveFile);
  }
  myfprintf(context, archiveFile, "EOD");
  }
}

RESCUEBOOL RescueSurface::DeletePropertyGroup(RescuePropertyGroup *existingPropertyGroup)
{
  ParentModel()->DeletePropertyGroup(existingPropertyGroup);
  return ((*groups) -= existingPropertyGroup);
}

RESCUEBOOL RescueSurface::DropRescueProperty(RescueProperty *unitToDrop)
{
  unitToDrop->Dispose();

  RESCUEINT64 ndx = 0;
  RescuePropertyGroup *group = groups->NthObject(ndx++);
  while (group != 0)
  {
  group->RemoveRescueProperty(unitToDrop);
  group = groups->NthObject(ndx++);
  }
  return ((*properties) -= unitToDrop);
}

RESCUEBOOL RescueSurface::AnyFileTruncated()
{
  RESCUEBOOL myReturn = ijSurfaces->AnyFileTruncated();
  if (myReturn == FALSE)
  {
  myReturn = properties->AnyFileTruncated();
  }
  if (myReturn == FALSE)
  {
  if (triangulatedSurface != 0)
  {
      myReturn = triangulatedSurface->FileTruncated();
  }
  }
  if (myReturn == FALSE)
  {
  if (propertyContainers == 0 && propertyContainerId != 0)
  {
      propertyContainers = new cSetRescueDataContainer(ParentModel(), propertyContainerId);  // Will Read.
  }
  if (propertyContainers != 0)
  {
      myReturn = propertyContainers->AnyFileTruncated();
  }
  }
  return myReturn;
}

RESCUEBOOL RescueSurface::IsOfType(_RescueObjectType thisType)
{
  if (thisType == R_RescueSurface)
  {
  return TRUE;
  }
  else
  {
  return RescueWireframeOwner::IsOfType(thisType);
  }
}

void RescueSurface::SetColor(RescueColor *colorIn)
{
  if (color != 0)
  {
  delete color;
  }
  color = colorIn;
}

void RescueSurface::SetColor(RescueColor &colorIn)
{
  RescueColor *newColor = new RescueColor(colorIn);
  SetColor(newColor);
}

cSetRescueDataContainer *RescueSurface::DataContainers()
{
  if (propertyContainers == 0)
  {
  if (propertyContainerId != 0)
  {
      propertyContainers = new cSetRescueDataContainer(ParentModel(), propertyContainerId);  // Will Read.
  }
  }
  return propertyContainers;
}

cSetRescueDataContainer *RescueSurface::DemandDataContainers()
{
  if (propertyContainers == 0)
  {
  if (propertyContainerId != 0)
  {
      propertyContainers = new cSetRescueDataContainer(ParentModel(), propertyContainerId);  // Will Read.
  }
  else
  {
      propertyContainers = new cSetRescueDataContainer(ParentModel());
  }
  }
  return propertyContainers;
}

void RescueSurface::SetOrientation(RescueOrientationLedger *ledger,
                                   RescueCoordinateSystem::Orientation orientation)
{
  RESCUEINT64 sOrd = 0;
  RescueIJSurface *ijSurf = ijSurfaces->NthObject(sOrd++);
  while (ijSurf != 0)
  {
  if (sOrd == 1 && triangulatedSurface != 0)
  {
      RescueCoordinateSystem::Orientation oNow = ijSurf->Grid()->DisplayOrientation();
      bool swapI = false;
      bool swapJ = false;
      if (oNow == RescueCoordinateSystem::LUF
      ||  oNow == RescueCoordinateSystem::LUB
      ||  oNow == RescueCoordinateSystem::LDF
      ||  oNow == RescueCoordinateSystem::LDB)
      {
    if (orientation == RescueCoordinateSystem::RUF
    ||  orientation == RescueCoordinateSystem::RUB
    ||  orientation == RescueCoordinateSystem::RDF
    ||  orientation == RescueCoordinateSystem::RDB)
    {
          swapI = true;
    }
      }
      else if (orientation == RescueCoordinateSystem::LUF
           ||  orientation == RescueCoordinateSystem::LUB
           ||  orientation == RescueCoordinateSystem::LDF
           ||  orientation == RescueCoordinateSystem::LDB)
      {
    swapI = true;
      }
      if (oNow == RescueCoordinateSystem::LUF
      ||  oNow == RescueCoordinateSystem::LDF
      ||  oNow == RescueCoordinateSystem::RUF
      ||  oNow == RescueCoordinateSystem::RDF)
      {
    if (orientation == RescueCoordinateSystem::LUB
    ||  orientation == RescueCoordinateSystem::LDB
    ||  orientation == RescueCoordinateSystem::RUB
    ||  orientation == RescueCoordinateSystem::RDB)
    {
          swapJ = true;
    }
      }
      else if (orientation == RescueCoordinateSystem::LUF
           ||  orientation == RescueCoordinateSystem::LDF
           ||  orientation == RescueCoordinateSystem::RUF
           ||  orientation == RescueCoordinateSystem::RDF)
      {
    swapJ = true;
      }
      if (swapI == true || swapJ == true)
      {
    triangulatedSurface->SwapAxes(swapI, ijSurf->Grid()->Axis(0), 
                                      swapJ, ijSurf->Grid()->Axis(1));
      }
  }
  ijSurf->SetOrientation(ledger, orientation);
  ijSurf = ijSurfaces->NthObject(sOrd++);
  }
}

void RescueSurface::FindUniquePropertyNames(cSetString *container)
{
  properties->FindUniquePropertyNames(container);
  cSetRescueDataContainer *containers = DataContainers();
  if (containers != 0)
  {
  containers->FindUniquePropertyNames(container);
  }
}




