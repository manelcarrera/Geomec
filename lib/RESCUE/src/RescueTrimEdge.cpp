/********************************************************************

  RescueTrimEdge.cpp

  An edge on a wireframe drawing.

  Rod Hanks,  July 1996

  April, 2005 - Added ownership flags for left and right vertex to
  clean up memory leaks.

*********************************************************************/
#include "RescueTrimEdge.h"
#include "RescueModel.h"
#include "RescueTrimVertex.h"
#include "RescueWireframe.h"
#include "RescueTrimVertexStub.h"
#include "RescueBuffer.h"
#include <string.h>

RescueTrimEdge::~RescueTrimEdge()
{
  delete polyLine;

  if ( leftVertexLocalOwner && leftVertex != NULL )
  {
      delete leftVertex ;
      leftVertex = NULL ;
  }
  if ( rightVertexLocalOwner && rightVertex != NULL )
  {
      delete rightVertex ;
      rightVertex = NULL ;
  }
}

RescueTrimEdge::RescueTrimEdge(RescuePolyLine *polyLineIn, _RescueLineDirection directionIn)
                                  :RescueObject(polyLineIn->ParentModel()->Context())
                                  ,polyLine(0)
                                  ,ownerLoop(0)
                                  ,direction(directionIn)
{
  isA = R_RescueTrimEdge;
  polyLine = new RescuePolyLineStub(polyLineIn->ParentModel()->Context(), polyLineIn);
  leftVertex = polyLineIn->LeftVertexObj();
  rightVertex = polyLineIn->RightVertexObj();
  leftVertexLocalOwner = rightVertexLocalOwner = FALSE ;
}

RescuePolyLine *RescueTrimEdge::PolyLine(RescueModel *model)
{
  RescuePolyLine *myReturn = polyLine->PolyLine(model);
  FixupStubs(myReturn);
  return myReturn;
}

RescueTrimVertex *RescueTrimEdge::StartingPoint(RescueModel *model) 
{
  RescueTrimVertex *myReturn = 0;
  RescuePolyLine *polyLineObj = polyLine->PolyLine(model);
  if (polyLineObj != 0)
  {
  myReturn = (direction == R_LEFT_TO_RIGHT) ? polyLineObj->LeftVertex() 
                                              : polyLineObj->RightVertex();
  FixupStubs(polyLineObj);
  }
  return myReturn;
}

RescueTrimVertexStub *RescueTrimEdge::EndingPointObj()
{
  RescueTrimVertexStub *myReturn = (direction == R_LEFT_TO_RIGHT) ? rightVertex
                                                                  : leftVertex;
  return myReturn;
}
 
RescueTrimVertexStub *RescueTrimEdge::StartingPointObj() 
{
  RescueTrimVertexStub *myReturn = (direction == R_LEFT_TO_RIGHT) ? leftVertex 
                                                                  : rightVertex;
  return myReturn;
}

RescueTrimVertex *RescueTrimEdge::EndingPoint(RescueModel *model)
{
  RescueTrimVertex *myReturn = 0;
  RescuePolyLine *polyLineObj = polyLine->PolyLine(model);
  if (polyLineObj != 0)
  {
  myReturn = (direction == R_LEFT_TO_RIGHT) ? polyLineObj->RightVertex()
                                              : polyLineObj->LeftVertex();
  FixupStubs(polyLineObj);
  }
  return myReturn;
}
 
RESCUEINT64 RescueTrimEdge::NodeCount64()
{
  RESCUEINT64 myReturn = 0;
  RescuePolyLine *polyLineObj = polyLine->PolyLine(0, FALSE);
  if (polyLineObj != 0)
  {
  myReturn = polyLineObj->PolyLineNodes()->Count64();
  FixupStubs(polyLineObj);
  }
  return myReturn;
}

RescuePolyLineNode *RescueTrimEdge::NthNode(RESCUEINT64 zeroBasedOrdinal) 
{
  RescuePolyLineNode *myReturn = 0;
  RescuePolyLine *polyLineObj = polyLine->PolyLine(0, FALSE);
  if (polyLineObj != 0)
  {
  myReturn = polyLineObj->PolyLineNodes()->NthObject((direction == R_LEFT_TO_RIGHT) 
                                  ? zeroBasedOrdinal 
                                  : (polyLineObj->PolyLineNodes()->Count64() - zeroBasedOrdinal) - 1);
  FixupStubs(polyLineObj);
  }
  return myReturn;
}

RescueTrimEdge::RescueTrimEdge(RescueContext *context, FILE *archiveFile)
                              :RescueObject(context)
                              ,polyLine(0)
                              ,leftVertex(0)
                              ,rightVertex(0)
                              ,ownerLoop(0)
{
  RESCUEINT64 directionIn;

  isA = R_RescueTrimEdge;
  if (context->ReadFileVersion() >= 29)
  {
  ReadId(context, archiveFile);
  }
  if (context->ReadFileVersion() >= 28)
  {
  polyLine = new RescuePolyLineStub(context, archiveFile);
  if (context->ReadFileVersion() >= 29)
  {
      leftVertex = new RescueTrimVertexStub(context, archiveFile);
      rightVertex = new RescueTrimVertexStub(context, archiveFile);
      leftVertexLocalOwner = rightVertexLocalOwner = TRUE ;
  }
  }
  else
  {
  RESCUEINT64 polyLineID;
  myfscanf(context, archiveFile, &polyLineID);
  if (context->ReadFileVersion() >= 27)
  {
      RESCUEINT64 polyLineOwner;
      RESCUEINT64 polyLineNdx;

      myfscanf(context, archiveFile, &polyLineOwner);
      myfscanf(context, archiveFile, &polyLineNdx);
  }
  polyLine = new RescuePolyLineStub(context, 0, polyLineID);
  }
  myfscanf(context, archiveFile, &directionIn);
  direction = (_RescueLineDirection) directionIn;
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

void RescueTrimEdge::Archive(RescueContext *context, FILE *archiveFile)
{
  myfprintf(context, archiveFile, "; Trim Edge");
  if (context->FileVersion() >= 29)
  {
  myfprintf(context, archiveFile, Identifier());
  }
  if (context->FileVersion() >= 28)
  {
  polyLine->ArchiveStub(context, archiveFile);
  if (context->FileVersion() >= 29)
  {
      if (leftVertex == 0)
      {
    leftVertex = new RescueTrimVertexStub(context, 0, 0);
    leftVertexLocalOwner = TRUE ;
      }
      if (rightVertex == 0)
      {
    rightVertex = new RescueTrimVertexStub(context, 0, 0);
    rightVertexLocalOwner = TRUE ;
      }
      leftVertex->ArchiveStub(context, archiveFile);
      rightVertex->ArchiveStub(context, archiveFile);
  }
  }
  else
  {
  myfprintf(context, archiveFile, polyLine->BestIdentifier());
  if (context->FileVersion() >= 27)
  {
      myfprintf(context, archiveFile, (RESCUEINT64) 0);
      myfprintf(context, archiveFile, (RESCUEINT64) 0);
  }
  }
  myfprintf(context, archiveFile, (RESCUEINT64) direction);
  if (context->FileVersion() >= 37)
  {
  myfprintf(context, archiveFile, "EOD");
  }
}

void RescueTrimEdge::Relink(RescueObject *parent)
{
}

RESCUEBOOL  RescueTrimEdge::SurfaceOnEdge( RescueIJSurface&  surf )
{                                         // If this edge lies on the given
  RESCUEBOOL  onSurf = FALSE;                  // surface return TRUE

  if (polyLine) 
  {
  RescuePolyLine *actualLine = polyLine->PolyLine(surf.ParentModel());
  if (actualLine != 0)
  {
      FixupStubs(actualLine);
      cSetRescuePolyLineNode*  nodes = actualLine->PolyLineNodes();

      if (nodes) 
      {
    if ( nodes->Count64() > 0 ) 
    {
          RescuePolyLineNode*  node = nodes->NthObject( 0 );

          if ( node ) 
          {
      onSurf = node->SurfaceOnNode( surf );
          }
    }
    else 
    {                           // Use the vertexes
          RescueTrimVertex*  vert1 = actualLine->LeftVertex();
          RescueTrimVertex*  vert2 = actualLine->RightVertex();

          if ( vert1  &&  vert2 ) 
          {
      onSurf = ( vert1->SurfaceOnNode( surf )  &&
                       vert2->SurfaceOnNode( surf )    );
          }
    }
      }
  }
  }

  return  onSurf;
}

void RescueTrimEdge::FixupStubs(RescuePolyLine *line)
{
  RescueContext *context = line->ParentModel()->Context();
  RESCUEBOOL didChange = FALSE;
  if (leftVertex == 0)
  {
  leftVertex = new RescueTrimVertexStub(context, line->LeftVertexObj());
  leftVertexLocalOwner = TRUE ;
  didChange = TRUE;
  }
  else
  {
  RescueTrimVertexStub *other = line->LeftVertexObj();
  if (other->ObjectId() != leftVertex->ObjectId())
  {
      leftVertex->CopyFrom(other);
      didChange = TRUE;
  }
  }
  if (rightVertex == 0)
  {
  rightVertex = new RescueTrimVertexStub(context, line->RightVertexObj());
  rightVertexLocalOwner = TRUE ;
  didChange = TRUE;
  }
  else
  {
  RescueTrimVertexStub *other = line->RightVertexObj();
  if (other->ObjectId() != rightVertex->ObjectId())
  {
      rightVertex->CopyFrom(other);
  }
  }
  if (didChange)
  {
  if (ownerLoop != 0)
  {
      RescueWireframe *wireframe = ownerLoop->Owner();
      if (wireframe == 0)
      {
    RescueEdgeSet *edgeSet = ownerLoop->OwnerSet();
    if (edgeSet != 0)
    {
          wireframe = edgeSet->Owner();
    }
      }
      if (wireframe != 0)
      {
    wireframe->MarkWireframeChanged();
      }
  }
  }
}



RESCUEBOOL RescueTrimEdge::IsOfType(_RescueObjectType thisType)
{
  if (thisType == R_RescueTrimEdge)
  {
  return TRUE;
  }
  else
  {
  return RescueObject::IsOfType(thisType);
  }
}



