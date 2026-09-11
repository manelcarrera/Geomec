/********************************************************************

  RescueTriangleFace.cpp

  A face on a triangulated surface.  We have to be a subclass of
  RescueObject, despite the overhead, because we need the unique ID
  feature to maintain relationships during storage and retrieval.

  Rod Hanks,  June, 1999

*********************************************************************/
#include "RescueTriangleFace.h"
#include "RescueTriangleVertex.h"
#include "RescueModel.h"
#include "RescueTriangulatedSurface.h"
#include "RescueBuffer.h"
#include <string.h>

RescueTriangleFace::RescueTriangleFace(RescueTriangleVertex *v1, 
                                       RescueTriangleVertex *v2, 
                                       RescueTriangleVertex *v3)
{
  vertices[0].obj = v1;
  vertices[1].obj = v2;
  vertices[2].obj = v3;
}

RescueTriangleFace::RescueTriangleFace(RescueTriangleVertex **v)
{
  vertices[0].obj = v[0];
  vertices[1].obj = v[1];
  vertices[2].obj = v[2];
}

void RescueTriangleFace::DropSelf()
{
  RESCUEINT64 loop;
  for (loop = 0; loop < 3; loop++)
  {
  vertices[loop].obj->RemoveFromFace(this);
  }
}

RescueTriangleVertex *RescueTriangleFace::NthVertex(RESCUEINT64 ordinal)
{
  RescueTriangleVertex *myReturn = 0;
  if (ordinal >= 0 && ordinal < 3)
  {
  myReturn = vertices[ordinal].obj;
  }
  return myReturn;
}

RescueTriangleFace *RescueTriangleFace::CrossFace(RESCUEINT64 ordinal)
{
  RescueTriangleFace *myReturn = 0;
  RescueTriangleVertex *v1 = vertices[ordinal].obj;
  RescueTriangleVertex *v2 = (ordinal == 2) ? vertices[0].obj : vertices[ordinal + 1].obj;
  RESCUEINT64 howMany = v1->FaceCount64();
  RESCUEINT64 loop;
  for (loop = 0; loop < howMany && myReturn == 0; loop++)
  {
  RescueTriangleFace *otherFace = v1->NthFace(loop);
  if (otherFace != this)
  {
      RESCUEINT64 corner;
      for (corner = 0; corner < 3 && myReturn == 0; corner++)
      {
    if (otherFace->vertices[corner].obj == v2)
    {
          myReturn = otherFace;
    }
      }
  }
  }
  return myReturn;
}

RescueTriangleFace::RescueTriangleFace(RescueContext *context, FILE *archiveFile)
{
  RESCUEINT64 loop;
  for (loop = 0; loop < 3; loop++)
  {
  RESCUEINT64 id;
  myfscanf(context, archiveFile, &id);
  vertices[loop].ndxOrId = (long) id;
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

void RescueTriangleFace::Archive(RescueContext *context, FILE *archiveFile)
{
  RESCUEINT64 loop;
  for (loop = 0; loop < 3; loop++)
  {
  if (context->FileVersion() >= 23)
  {
      myfprintf(context, archiveFile, vertices[loop].obj->ndx);
  }
  else
  {
      myfprintf(context, archiveFile, vertices[loop].obj->Identifier());
  }
  }
  if (context->FileVersion() >= 37)
  {
  myfprintf(context, archiveFile, "EOD");
  }
}

void RescueTriangleFace::Relink(RescueObject *triangulatedSurface)
{
  RescueTriangulatedSurface *surfaceRep = (RescueTriangulatedSurface *) triangulatedSurface;
  RescueContext *context = surfaceRep->ParentSurface()->ParentModel()->Context();
  RESCUEINT64 loop;
  for (loop = 0; loop < 3; loop++)
  {
  if (context->ReadFileVersion() < 23)
  {
      vertices[loop].obj = surfaceRep->TriangleVertexIdentifiedBy((long) vertices[loop].ndxOrId);
  }
  else
  {
      vertices[loop].obj = surfaceRep->NthVertex((long) vertices[loop].ndxOrId);
  }
  }
  for (loop = 0; loop < 3; loop++)
  {
  vertices[loop].obj->AddToFace(this);
  }
}


