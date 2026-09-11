/********************************************************************

  RescuePolyLineNode.cpp

  A point on a polyline.

  The point is described by two kinds of values.  A u,v pair gives the
  address of the point in terms of the i,j values of the some face's
  two dimensional grid.

  The x,y,z triplet gives the address of the point within the model's
  3 dimensional coordinate system.  It is calculated from the u,v pair.

  Rod Hanks,  June 1996

*********************************************************************/
#include "RescueModel.h"
#include "RescuePolyLineNode.h"
#include "RescuePolyLineNodeBUG.h"
#include "RescuePolyLineNodeUVT.h"
#include "RescueIJSurface.h"
#include "RescueTriangulatedSurface.h"
#include <float.h>
#include <limits.h>
#include "RescueBuffer.h"
#include <string.h>

RescuePolyLineNode::~RescuePolyLineNode()
{
  if (surfaceUVs != 0)
  {
  delete surfaceUVs;
  }
  if (layerParams != 0)
  {
  delete layerParams;
  }
  if (tSurfUVs != 0)
  {
  delete tSurfUVs;
  }
}

RescuePolyLineNode::RescuePolyLineNode(RescueContext *context, RESCUEDOUBLE x, RESCUEDOUBLE y, RESCUEDOUBLE z)
              :RescueObject(context)
              ,xValue(x)
              ,yValue(y)
              ,zValue(z)
              ,surfaceUVs(0)
              ,layerParams(0)
              ,tSurfUVs(0)
{
  isA = R_RescuePolyLineNode;
}

RescuePolyLineNode::RescuePolyLineNode(RescueContext *context, RESCUEFLOAT x, RESCUEFLOAT y, RESCUEFLOAT z)
              :RescueObject(context)
              ,xValue((RESCUEDOUBLE) x)
              ,yValue((RESCUEDOUBLE) y)
              ,zValue((RESCUEDOUBLE) z)
              ,surfaceUVs(0)
              ,layerParams(0)
              ,tSurfUVs(0)
{
  isA = R_RescuePolyLineNode;
}

RescuePolyLineNodeUVT *RescuePolyLineNode::UVForFace(RescueTriangulatedSurface *face)
{
  RescuePolyLineNodeUVT *myReturn = 0;
  if (tSurfUVs != 0)
  {
  RESCUEBOOL found = FALSE;
  RESCUEINT64 ordinal = 0;
  myReturn = tSurfUVs->NthObject(ordinal++);

  while (found == FALSE && myReturn != 0)
  {
      if (myReturn->Face(face->ParentSurface()->ParentModel()) == face)
      {
    found = TRUE;
      }
      else
      {
    myReturn = tSurfUVs->NthObject(ordinal++);
      }
  }
  }
  return myReturn;
}

RescuePolyLineNodeUV *RescuePolyLineNode::UVForFace(RescueIJSurface *face)
{
  RescuePolyLineNodeUV *myReturn = 0;
  if (surfaceUVs != 0)
  {
  RESCUEBOOL found = FALSE;
  RESCUEINT64 ordinal = 0;
  myReturn = surfaceUVs->NthObject(ordinal++);

  while (found == FALSE && myReturn != 0)
  {
      if (myReturn->Face() == face)
      {
    found = TRUE;
      }
      else
      {
    myReturn = surfaceUVs->NthObject(ordinal++);
      }
  }
  }
  return myReturn;
}

void RescuePolyLineNode::SetUVValue(RescueTriangulatedSurface *surface, RESCUEINT64 faceNdx, RESCUEFLOAT u, RESCUEFLOAT v)
{
  RescuePolyLineNodeUVT *surfaceIntersect = UVForFace(surface);
  if (surfaceIntersect == 0)
  {
  surfaceIntersect = new RescuePolyLineNodeUVT(surface->ParentSurface()->ParentModel()->Context(), 
                                                  surface, faceNdx, u, v);
  if (tSurfUVs == 0)
  {
      tSurfUVs = new cSetRescuePolyLineNodeUVT();
  }
  (*tSurfUVs) += surfaceIntersect;
  }
  else
  {
  surfaceIntersect->SetUVValue(faceNdx, u, v);
  }
}

RescueTriangulatedSurface *RescuePolyLineNode::NthTSurf(RescueModel *model, RESCUEINT64 zeroBasedOrdinal)
{
  RescueTriangulatedSurface *myReturn = 0;
  if (tSurfUVs != 0)
  {
  RescuePolyLineNodeUVT *nodeUV = tSurfUVs->NthObject(zeroBasedOrdinal);
  if (nodeUV != 0)
  {
      myReturn = nodeUV->Face(model);
  }
  }
  return myReturn;
}

RESCUEFLOAT RescuePolyLineNode::U(RescueTriangulatedSurface *tSurf)
{
  RescuePolyLineNodeUVT *uv = UVForFace(tSurf);
  if (uv == 0)
  {
  return FLT_MAX;
  }
  else
  {
  return uv->U();
  }
}

RESCUEFLOAT RescuePolyLineNode::V(RescueTriangulatedSurface *tSurf)
{
  RescuePolyLineNodeUVT *uv = UVForFace(tSurf);
  if (uv == 0)
  {
  return FLT_MAX;
  }
  else
  {
  return uv->V();
  }
}

RESCUEINT64 RescuePolyLineNode::FaceNdx64(RescueTriangulatedSurface *tSurf)
{
  RescuePolyLineNodeUVT *uv = UVForFace(tSurf);
  if (uv == 0)
  {
  return INT_MAX;
  }
  else
  {
  return uv->FaceNdx64();
  }
}

RESCUEBOOL RescuePolyLineNode::SurfaceOnNode( RescueTriangulatedSurface&  surf )
{
  RESCUEBOOL  onSurf = FALSE;                  // given surface return TRUE
  if (tSurfUVs != 0)
  {
  for ( RESCUEINT64 i = 0; !onSurf  &&  i < tSurfUVs->Count64(); i++ ) 
  {
      RescuePolyLineNodeUVT*  uvNode = tSurfUVs->NthObject( i );
      if ( uvNode ) 
     {
         onSurf = uvNode->CompareFace(&surf);
      }
  }
  }
  return  onSurf;
}

void RescuePolyLineNode::SetUVValue(RescueIJSurface *face, RESCUEFLOAT u, RESCUEFLOAT v)
{
  RescuePolyLineNodeUV *surfaceIntersect = UVForFace(face);
  if (surfaceIntersect == 0)
  {
  surfaceIntersect = new RescuePolyLineNodeUV(face->ParentModel()->Context(), face, u, v);
  if (surfaceUVs == 0)
  {
      surfaceUVs = new cSetRescuePolyLineNodeUV();
  }
  (*surfaceUVs) += surfaceIntersect;
  }
  else
  {
  surfaceIntersect->SetUVValue(u, v);
  }
}


RescuePolyLineNodeBUG *RescuePolyLineNode::ParamOn(RescueGeometry *geometry, RescueLayerType layerType)
{
  RescuePolyLineNodeBUG *myReturn = 0;
  if (layerParams != 0)
  {
  RESCUEINT64 ordinal = 0;
  RescuePolyLineNodeBUG *instance = layerParams->NthObject(ordinal++);

  while (myReturn == 0 && instance != 0)
  {
      if (instance->Geometry() == geometry
      &&  instance->Type()     == layerType)
      {
    myReturn = instance;
      }
      else
      {
    instance = layerParams->NthObject(ordinal++);
      }
  }
  }
  return myReturn;
}

void RescuePolyLineNode::SetLayerValue(RescueGeometry *geometry,
                                       RescueLayerType layerType,
                                       RESCUEFLOAT layerNdxParam)
{
  RescuePolyLineNodeBUG *parameterization = ParamOn(geometry, layerType);
  if (parameterization == 0)
  {
  parameterization = new RescuePolyLineNodeBUG(geometry->ParentModel()->Context(),
                                                 geometry, layerType, layerNdxParam);
  if (layerParams == 0)
  {
      layerParams = new cSetRescuePolyLineNodeBUG();
  }
  (*layerParams) += parameterization;
  }
  else
  {
  parameterization->SetNdxParam(layerNdxParam);
  }
}

RescuePolyLineNode::RescuePolyLineNode(RescueContext *context, FILE *archiveFile)
                  :RescueObject(context),surfaceUVs(0),layerParams(0),tSurfUVs(0)
{
  isA = R_RescuePolyLineNode;
  if (context->ReadFileVersion() >= 7)
  {
  myfscanf(context, archiveFile, &xValue);
  myfscanf(context, archiveFile, &yValue);
  myfscanf(context, archiveFile, &zValue);
  }
  else
  {
  RESCUEFLOAT xStandin, yStandin, zStandin;

  myfscanf(context, archiveFile, &xStandin);
  myfscanf(context, archiveFile, &yStandin);
  myfscanf(context, archiveFile, &zStandin);
  xValue = (RESCUEDOUBLE) xStandin;
  yValue = (RESCUEDOUBLE) yStandin;
  zValue = (RESCUEDOUBLE) zStandin;
  }
  surfaceUVs = new cSetRescuePolyLineNodeUV();
  surfaceUVs->UnArchive(context, archiveFile);
  if (surfaceUVs->Count64() == 0)
  {
  delete surfaceUVs;
  surfaceUVs = 0;
  }
  if (context->ReadFileVersion() >= 23)
  {
  layerParams = new cSetRescuePolyLineNodeBUG();
  layerParams->UnArchive(context, archiveFile);
  if (layerParams->Count64() == 0)
  {
      delete layerParams;
      layerParams = 0;
  }
  }
  if (context->ReadFileVersion() >= 30)
  {
  tSurfUVs = new cSetRescuePolyLineNodeUVT();
  tSurfUVs->UnArchive(context, archiveFile);
  if (tSurfUVs->Count64() == 0)
  {
      delete tSurfUVs;
      tSurfUVs = 0;
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

void RescuePolyLineNode::Archive(RescueContext *context, FILE *archiveFile)
{
  myfprintf(context, archiveFile, "; PolyLineNode");
  myfprintf(context, archiveFile, xValue);
  myfprintf(context, archiveFile, yValue);
  myfprintf(context, archiveFile, zValue);
  if (surfaceUVs == 0)
  {
  surfaceUVs = new cSetRescuePolyLineNodeUV();
  }
  surfaceUVs->Archive(context, archiveFile);
  if (surfaceUVs->Count64() == 0)
  {
  delete surfaceUVs;
  surfaceUVs = 0;
  }
  if (context->FileVersion() >= 23)
  {
  if (layerParams == 0)
  {
      layerParams = new cSetRescuePolyLineNodeBUG();
  }
  layerParams->Archive(context, archiveFile);
  if (layerParams->Count64() == 0)
  {
      delete layerParams;
      layerParams = 0;
  }
  }
  if (context->FileVersion() >= 30)
  {
  if (tSurfUVs == 0)
  {
      tSurfUVs = new cSetRescuePolyLineNodeUVT();
  }
  tSurfUVs->Archive(context, archiveFile);
  if (tSurfUVs->Count64() == 0)
  {
      delete tSurfUVs;
      tSurfUVs = 0;
  }
  }
  if (context->FileVersion() >= 37)
  {
  myfprintf(context, archiveFile, "EOD");
  }
}

void RescuePolyLineNode::Relink(RescueObject *parent)
{
  if (surfaceUVs != 0)
  {
  surfaceUVs->Relink(parent);
  }
  if (layerParams != 0)
  {
  layerParams->Relink(parent);
  }
/*
  tSurfUVs are just-in-time relinked.
*/
}

RESCUEBOOL  RescuePolyLineNode::SurfaceOnNode( RescueIJSurface&  surf )
{                                         // If the node has UV coords on the
  RESCUEBOOL  onSurf = FALSE;                  // given surface return TRUE
  if (surfaceUVs != 0)
  {
  for ( RESCUEINT64 i = 0; !onSurf  &&  i < surfaceUVs->Count64(); i++ ) 
  {
      RescuePolyLineNodeUV*  uvNode = surfaceUVs->NthObject( i );

      if ( uvNode ) 
      {
    onSurf = ( &surf == uvNode->Face() );
      }
  }
  }
  return  onSurf;
  }

RESCUEFLOAT RescuePolyLineNode::U(RescueIJSurface *face)
{
  RescuePolyLineNodeUV *uv = UVForFace(face);
  if (uv == 0)
  {
  return FLT_MAX;
  }
  else
  {
  return uv->U();
  }
}  

RESCUEFLOAT RescuePolyLineNode::V(RescueIJSurface *face)
{
  RescuePolyLineNodeUV *uv = UVForFace(face);
  if (uv == 0)
  {
  return FLT_MAX;
  }
  else
  {
  return uv->V();
  }
}  

RescueIJSurface *RescuePolyLineNode::NthFace(RESCUEINT64 zeroBasedOrdinal) 
{
  RescueIJSurface *myReturn = 0;
  if (surfaceUVs != 0)
  {
  RescuePolyLineNodeUV *nodeUV = surfaceUVs->NthObject(zeroBasedOrdinal);
  if (nodeUV != 0)
  {
      myReturn = nodeUV->Face();
  }
  }
  return myReturn;
}

RESCUEBOOL RescuePolyLineNode::IsOfType(_RescueObjectType thisType)
{
  if (thisType == R_RescuePolyLineNode)
  {
  return TRUE;
  }
  else
  {
  return RescueObject::IsOfType(thisType);
  }
}

RESCUEINT32 RescuePolyLineNode::FaceCount(RESCUEBOOL throwIfTooBig)
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

RESCUEINT32 RescuePolyLineNode::LayerCount(RESCUEBOOL throwIfTooBig)
{
  RESCUEINT64 output = LayerCount64();
  if (throwIfTooBig)
  {
  if (output > 2147483647 || output < -2147483647)
  {
      throw "Model is too large to be read in 32 bit mode.";
  }
  }
  return (RESCUEINT32) output;
}




