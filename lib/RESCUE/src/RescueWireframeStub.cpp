/********************************************************************

  RescueWireframeStub.cpp

  A RescuePolyLine, or RescueTrimVertex
  object that has been lazily relinked.
  
  You can preview the object here and trade it for a real one
  later.

  Rod Hanks,  March 2001

*********************************************************************/
#include "myHeaders.h"
#include "RescueWireframeStub.h"
#include "RescuePolyLine.h"
#include "RescueTrimVertex.h"
#include "RescuePolyLineNode.h"
#include "RescueWireframe.h"
#include "RescueModel.h"

void RescueWireframeStub::SetWireframe(RescueWireframe *wireframeIn) 
{
  wireframeObj = wireframeIn;
  wireframeId = wireframeObj->Identifier();
  loadNo = wireframeObj->LoadNo();
}

RESCUEBOOL RescueWireframeStub::IsOfType(_RescueObjectType thisType)
{
  if (thisType == R_RescueWireframeStub)
  {
    return TRUE;
  }
  else
  {
    return RescueObject::IsOfType(thisType);
  }
}

RescueWireframe *RescueWireframeStub::Wireframe(RescueModel *model)
{
  if (wireframeObj == 0)
  {
    wireframeObj = model->WireframeIdentifiedBy(wireframeId);
  }
  return wireframeObj;
}

void RescueWireframeStub::ArchiveStub(RescueContext *context, FILE *archiveFile)
{
  myfprintf(context, archiveFile, wireframeId);
  myfprintf(context, archiveFile, objectId);
}

RescueWireframeStub::RescueWireframeStub(RescueContext *context, FILE *archiveFile)
                    :RescueObject(context),loadNo(-1),wireframeObj(0)
{
  myfscanf(context, archiveFile, &wireframeId);
  myfscanf(context, archiveFile, &objectId);
}

RescueWireframeStub::RescueWireframeStub(RescueContext *context, RESCUEINT64 wireframeIdIn, RESCUEINT64 objectIdIn)
                  :RescueObject(context),loadNo(-1),wireframeObj(0)
{
  wireframeId = wireframeIdIn;
  objectId = objectIdIn;
}

RescueWireframeStub::RescueWireframeStub(RescueContext *context, RescueWireframe *wireframeIn, RESCUEINT64 objectIdIn)
                                                :RescueObject(context)
                                                ,wireframeId(0)
                                                ,objectId(objectIdIn)
                                                ,loadNo(-1)
                                                ,wireframeObj(wireframeIn)
{
  if (wireframeObj != 0)
  {
    wireframeId = wireframeObj->Identifier();
  }
}




