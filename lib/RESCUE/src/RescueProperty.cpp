/********************************************************************

  RescueProperty.cpp

  Property for RESCUE's data model.  Attach to 

  Rod Hanks,  May 1996

*********************************************************************/
#include "RescueProperty.h"
#include "RescuePropertyGroup.h"
#include "RescueBlockUnit.h"
#include "RescueArrayFloat.h"
#include "RescueArrayInt.h"
#include "RescueArrayByte.h"
#include "RescueArrayShort.h"
#include "RescueArray2dVector.h"
#include "RescueArray3dVector.h"
#include "RescueModel.h"
#include "RescueTimeStepGroup.h"
#include "RescueIdTree.h"
#include "RescueColorTable.h"
#include "RescueGeobodyPart.h"
#include "RescueGeobody.h"
#include "cSetRescueTrimLoop.h"
#include "RescueTriangulatedSurface.h"
#include "RescueIJSurface.h"
#include "RescueTrimLoopStub.h"
#include "cBagRescueTrimLoop.h"
#include "RescueWireframe.h"
#include "RescueBuffer.h"
#include "RescueDataContainer.h"
#include <string.h>

RescueProperty::RescueProperty(RescueDataContainer *parentContainer,
                               RescueModel *model,
                               const RESCUECHAR *propertyNameIn, 
                               const RESCUECHAR *propertyTypeIn,
          const RESCUECHAR *unitOfMeasureIn,
                               AppliesToSurface applies,
                               RESCUEFLOAT nullValue, RESCUEFLOAT *valueArray, 
                               AttachmentPoint attachedToIn)
                               :RescueHistoryObject(model->Context())
                               ,parentBlockUnit(0)
                               ,parentGeobodyPart(0)
                               ,geometry(0)
                               ,container(parentContainer)
                               ,surface(0)
                               ,loops(0)
                               ,howApplies(applies)
                               ,attachedTo(attachedToIn)
                               ,triangulatedSurface(FALSE)
                               ,ijSurface(0)
                               ,isLocked(FALSE)
                               ,context(model->Context())
                               ,colorTable(0)
{
  RESCUEBOOL cellCentered = TRUE;
  if (attachedTo != CELL_CENTERED && attachedTo != FACE_CENTERED)
  {
  cellCentered = FALSE;
  }
  isA = R_RescueProperty;
  (*container->properties) += this;
  loops = new cSetRescueTrimLoopStub();
  data = new RescueArrayFloat(propertyNameIn, propertyTypeIn, unitOfMeasureIn,
                               container->Grid(), 
                               nullValue, model,
                               cellCentered, valueArray);
}

RescueProperty::RescueProperty(RescueDataContainer *containerIn, RescueModel *modelIn,
                               const RESCUECHAR *propertyNameIn, 
                               const RESCUECHAR *propertyTypeIn, const RESCUECHAR *unitOfMeasureIn,
                               AppliesToSurface applies,
                               RESCUEINT32 nullValue, RESCUEINT32 *valueArray, 
                               AttachmentPoint attachedToIn)
                               :RescueHistoryObject(modelIn->Context())
                               ,parentBlockUnit(0)
                               ,parentGeobodyPart(0)
                               ,geometry(0)
                               ,container(containerIn)
                               ,surface(0)
                               ,loops(0)
                               ,howApplies(applies)
                               ,attachedTo(attachedToIn)
                               ,triangulatedSurface(FALSE)
                               ,ijSurface(0)
                               ,isLocked(FALSE)
                               ,context(modelIn->Context())
                               ,colorTable(0)
{
  RESCUEBOOL cellCentered = TRUE;
  if (attachedTo != CELL_CENTERED && attachedTo != FACE_CENTERED)
  {
  cellCentered = FALSE;
  }
  isA = R_RescueProperty;
  (*container->properties) += this;
  loops = new cSetRescueTrimLoopStub();
  data = new RescueArrayInt(propertyNameIn, propertyTypeIn, unitOfMeasureIn,
                               containerIn->Grid(), 
                               nullValue, modelIn,
                               cellCentered, valueArray);
}


RescueProperty::RescueProperty(RescueDataContainer *containerIn, RescueModel *modelIn,
                               const RESCUECHAR *propertyNameIn, 
                               const RESCUECHAR *propertyTypeIn, 
                               const RESCUECHAR *unitOfMeasureIn,
                               AppliesToSurface applies,
                               RescueLookup *lookupTableIn,
                               RESCUEUCHAR nullValue, 
                               RESCUEUCHAR *valueArray,
                               AttachmentPoint attachedToIn)
                               :RescueHistoryObject(modelIn->Context())
                               ,parentBlockUnit(0)
                               ,parentGeobodyPart(0)
                               ,geometry(0)
                               ,container(containerIn)
                               ,surface(0)
                               ,loops(0)
                               ,howApplies(applies)
                               ,attachedTo(attachedToIn)
                               ,triangulatedSurface(FALSE)
                               ,ijSurface(0)
                               ,isLocked(FALSE)
                               ,context(modelIn->Context())
                               ,colorTable(0)
{
  RESCUEBOOL cellCentered = TRUE;
  if (attachedTo != CELL_CENTERED && attachedTo != FACE_CENTERED)
  {
  cellCentered = FALSE;
  }
  isA = R_RescueProperty;
  (*container->properties) += this;
  loops = new cSetRescueTrimLoopStub();
  data = new RescueArrayByte(propertyNameIn, propertyTypeIn, unitOfMeasureIn,
                             container->Grid(), 
                             nullValue, lookupTableIn, modelIn,
                             cellCentered, valueArray); 
}

RescueProperty::RescueProperty(RescueDataContainer *containerIn, RescueModel *modelIn,
                               const RESCUECHAR *propertyNameIn, 
                               const RESCUECHAR *propertyTypeIn, 
                               const RESCUECHAR *unitOfMeasureIn,
                               AppliesToSurface applies,
                               RescueLookup *lookupTableIn,
                               RESCUEUSHORT nullValue, 
                               RESCUEUSHORT *valueArray,
                               AttachmentPoint attachedToIn)
                               :RescueHistoryObject(modelIn->Context())
                               ,parentBlockUnit(0)
                               ,parentGeobodyPart(0)
                               ,geometry(0)
                               ,container(containerIn)
                               ,surface(0)
                               ,loops(0)
                               ,howApplies(applies)
                               ,attachedTo(attachedToIn)
                               ,triangulatedSurface(FALSE)
                               ,ijSurface(0)
                               ,isLocked(FALSE)
                               ,context(modelIn->Context())
                               ,colorTable(0)
{
  RESCUEBOOL cellCentered = TRUE;
  if (attachedTo != CELL_CENTERED && attachedTo != FACE_CENTERED)
  {
  cellCentered = FALSE;
  }
  isA = R_RescueProperty;
  (*container->properties) += this;
  loops = new cSetRescueTrimLoopStub();
  data = new RescueArrayShort(propertyNameIn, propertyTypeIn, unitOfMeasureIn,
                               container->Grid(), 
                               nullValue, lookupTableIn, 
                               modelIn,
                               cellCentered, valueArray);
}

RescueProperty::RescueProperty(RescueDataContainer *containerIn, RescueModel *modelIn,
                               const RESCUECHAR *propertyNameIn, 
                               const RESCUECHAR *propertyTypeIn, const RESCUECHAR *unitOfMeasureIn,
                               AppliesToSurface applies,
                               Rescue2dVector nullValue, Rescue2dVector *valueArray,
                               AttachmentPoint attachedToIn)
                               :RescueHistoryObject(modelIn->Context())
                               ,parentBlockUnit(0)
                               ,parentGeobodyPart(0)
                               ,geometry(0)
                               ,container(containerIn)
                               ,surface(0)
                               ,loops(0)
                               ,howApplies(applies)
                               ,attachedTo(attachedToIn)
                               ,triangulatedSurface(FALSE)
                               ,ijSurface(0)
                               ,isLocked(FALSE)
                               ,context(modelIn->Context())
                               ,colorTable(0)
{
  RESCUEBOOL cellCentered = TRUE;
  if (attachedTo != CELL_CENTERED && attachedTo != FACE_CENTERED)
  {
  cellCentered = FALSE;
  }
  isA = R_RescueProperty;
  (*container->properties) += this;
  loops = new cSetRescueTrimLoopStub();
  data = new RescueArray2dVector(propertyNameIn, propertyTypeIn, unitOfMeasureIn,
                                 container->Grid(), 
                                 nullValue, modelIn,
                                 cellCentered, valueArray);
}

RescueProperty::RescueProperty(RescueDataContainer *containerIn, RescueModel *modelIn,
                               const RESCUECHAR *propertyNameIn, 
                               const RESCUECHAR *propertyTypeIn, const RESCUECHAR *unitOfMeasureIn,
                               AppliesToSurface applies,
                               Rescue3dVector nullValue, Rescue3dVector *valueArray,
                               AttachmentPoint attachedToIn)
                               :RescueHistoryObject(modelIn->Context())
                               ,parentBlockUnit(0)
                               ,parentGeobodyPart(0)
                               ,geometry(0)
                               ,container(containerIn)
                               ,surface(0)
                               ,loops(0)
                               ,howApplies(applies)
                               ,attachedTo(attachedToIn)
                               ,triangulatedSurface(FALSE)
                               ,ijSurface(0)
                               ,isLocked(FALSE)
                               ,context(modelIn->Context())
                               ,colorTable(0)
{
  RESCUEBOOL cellCentered = TRUE;
  if (attachedTo != CELL_CENTERED && attachedTo != FACE_CENTERED)
  {
  cellCentered = FALSE;
  }
  isA = R_RescueProperty;
  (*container->properties) += this;
  loops = new cSetRescueTrimLoopStub();
  data = new RescueArray3dVector(propertyNameIn, propertyTypeIn, unitOfMeasureIn,
                                 container->Grid(), 
                                 nullValue, modelIn,
                                 cellCentered, valueArray);
}

RescueProperty::RescueProperty(RescueTriangulatedSurface *parentSurface,
                               const RESCUECHAR *propertyNameIn, 
                               const RESCUECHAR *propertyTypeIn, const RESCUECHAR *unitOfMeasureIn,
                               AppliesToSurface applies,
                               RESCUEFLOAT nullValue, RESCUEFLOAT *valueArray, 
                               AttachmentPoint attachedToIn)
                               :RescueHistoryObject(parentSurface->ParentSurface()->ParentModel()->Context())
                               ,parentBlockUnit(0)
                               ,parentGeobodyPart(0)
                               ,geometry(0)
                               ,container(0)
                               ,surface(parentSurface->ParentSurface())
                               ,loops(0)
                               ,howApplies(applies)
                               ,attachedTo(attachedToIn)
                               ,triangulatedSurface(TRUE)
                               ,ijSurface(0)
                               ,isLocked(FALSE)
                               ,context(parentSurface->ParentSurface()->ParentModel()->Context())
                               ,colorTable(0)
{
  isA = R_RescueProperty;
  (*parentSurface->ParentSurface()->properties) += this;
  VetIJSurface();
  loops = new cSetRescueTrimLoopStub();
  data = new RescueArrayFloat(propertyNameIn, propertyTypeIn, unitOfMeasureIn,
                               parentSurface->Grid(attachedTo), 
                               nullValue, parentSurface->ParentSurface()->ParentModel(),
                               TRUE, valueArray);
}

RescueProperty::RescueProperty(RescueTriangulatedSurface *parentSurface,
                               const RESCUECHAR *propertyNameIn, 
                               const RESCUECHAR *propertyTypeIn, const RESCUECHAR *unitOfMeasureIn,
                               AppliesToSurface applies,
                               RESCUEINT32 nullValue, RESCUEINT32 *valueArray, 
                               AttachmentPoint attachedToIn)
                               :RescueHistoryObject(parentSurface->ParentSurface()->ParentModel()->Context())
                               ,parentBlockUnit(0)
                               ,parentGeobodyPart(0)
                               ,geometry(0)
                               ,container(0)
                               ,surface(parentSurface->ParentSurface())
                               ,loops(0)
                               ,howApplies(applies)
                               ,attachedTo(attachedToIn)
                               ,triangulatedSurface(TRUE)
                               ,ijSurface(0)
                               ,isLocked(FALSE)
                               ,context(parentSurface->ParentSurface()->ParentModel()->Context())
                               ,colorTable(0)
{
  isA = R_RescueProperty;
  (*parentSurface->ParentSurface()->properties) += this;
  VetIJSurface();
  loops = new cSetRescueTrimLoopStub();
  data = new RescueArrayInt(propertyNameIn, propertyTypeIn, unitOfMeasureIn,
                               parentSurface->Grid(attachedTo), 
                               nullValue, parentSurface->ParentSurface()->ParentModel(),
                               TRUE, valueArray);
}

RescueProperty::RescueProperty(RescueTriangulatedSurface *parentSurface,
                               const RESCUECHAR *propertyNameIn, 
                               const RESCUECHAR *propertyTypeIn, 
                               const RESCUECHAR *unitOfMeasureIn,
                               AppliesToSurface applies,
                               RescueLookup *lookupTableIn,
                               RESCUEUCHAR nullValue, 
                               RESCUEUCHAR *valueArray,
                               AttachmentPoint attachedToIn)
                               :RescueHistoryObject(parentSurface->ParentSurface()->ParentModel()->Context())
                               ,parentBlockUnit(0)
                               ,parentGeobodyPart(0)
                               ,geometry(0)
                               ,container(0)
                               ,surface(parentSurface->ParentSurface())
                               ,loops(0)
                               ,howApplies(applies)
                               ,attachedTo(attachedToIn)
                               ,triangulatedSurface(TRUE)
                               ,ijSurface(0)
                               ,isLocked(FALSE)
                               ,context(parentSurface->ParentSurface()->ParentModel()->Context())
                               ,colorTable(0)
{
  isA = R_RescueProperty;
  (*parentSurface->ParentSurface()->properties) += this;
  VetIJSurface();
  loops = new cSetRescueTrimLoopStub();
  data = new RescueArrayByte(propertyNameIn, propertyTypeIn, unitOfMeasureIn,
                             parentSurface->Grid(attachedTo), 
                             nullValue, lookupTableIn, parentSurface->ParentSurface()->ParentModel(),
                             TRUE, valueArray); 
}

RescueProperty::RescueProperty(RescueTriangulatedSurface *parentSurface,
                               const RESCUECHAR *propertyNameIn, 
                               const RESCUECHAR *propertyTypeIn, 
                               const RESCUECHAR *unitOfMeasureIn,
                               AppliesToSurface applies,
                               RescueLookup *lookupTableIn,
                               RESCUEUSHORT nullValue, 
                               RESCUEUSHORT *valueArray,
                               AttachmentPoint attachedToIn)
                               :RescueHistoryObject(parentSurface->ParentSurface()->ParentModel()->Context())
                               ,parentBlockUnit(0)
                               ,parentGeobodyPart(0)
                               ,geometry(0)
                               ,container(0)
                               ,surface(parentSurface->ParentSurface())
                               ,loops(0)
                               ,howApplies(applies)
                               ,attachedTo(attachedToIn)
                               ,triangulatedSurface(TRUE)
                               ,ijSurface(0)
                               ,isLocked(FALSE)
                               ,context(parentSurface->ParentSurface()->ParentModel()->Context())
                               ,colorTable(0)
{
  isA = R_RescueProperty;
  (*parentSurface->ParentSurface()->properties) += this;
  VetIJSurface();
  loops = new cSetRescueTrimLoopStub();
  data = new RescueArrayShort(propertyNameIn, propertyTypeIn, unitOfMeasureIn,
                             parentSurface->Grid(attachedTo), 
                             nullValue, lookupTableIn, parentSurface->ParentSurface()->ParentModel(),
                             TRUE, valueArray); 
}

RescueProperty::RescueProperty(RescueTriangulatedSurface *parentSurface,
                               const RESCUECHAR *propertyNameIn, 
                               const RESCUECHAR *propertyTypeIn, const RESCUECHAR *unitOfMeasureIn,
                               AppliesToSurface applies,
                               Rescue2dVector nullValue, Rescue2dVector *valueArray,
                               AttachmentPoint attachedToIn)
                               :RescueHistoryObject(parentSurface->ParentSurface()->ParentModel()->Context())
                               ,parentBlockUnit(0)
                               ,parentGeobodyPart(0)
                               ,geometry(0)
                               ,container(0)
                               ,surface(parentSurface->ParentSurface())
                               ,loops(0)
                               ,howApplies(applies)
                               ,attachedTo(attachedToIn)
                               ,triangulatedSurface(TRUE)
                               ,ijSurface(0)
                               ,isLocked(FALSE)
                               ,context(parentSurface->ParentSurface()->ParentModel()->Context())
                               ,colorTable(0)
{
  isA = R_RescueProperty;
  (*parentSurface->ParentSurface()->properties) += this;
  VetIJSurface();
  loops = new cSetRescueTrimLoopStub();
  data = new RescueArray2dVector(propertyNameIn, propertyTypeIn, unitOfMeasureIn,
                                 parentSurface->Grid(attachedTo), 
                                 nullValue, parentSurface->ParentSurface()->ParentModel(),
                                 TRUE, valueArray);
}

RescueProperty::RescueProperty(RescueTriangulatedSurface *parentSurface,
                               const RESCUECHAR *propertyNameIn, 
                               const RESCUECHAR *propertyTypeIn, const RESCUECHAR *unitOfMeasureIn,
                               AppliesToSurface applies,
                               Rescue3dVector nullValue, Rescue3dVector *valueArray,
                               AttachmentPoint attachedToIn)
                               :RescueHistoryObject(parentSurface->ParentSurface()->ParentModel()->Context())
                               ,parentBlockUnit(0)
                               ,parentGeobodyPart(0)
                               ,geometry(0)
                               ,container(0)
                               ,surface(parentSurface->ParentSurface())
                               ,loops(0)
                               ,howApplies(applies)
                               ,attachedTo(attachedToIn)
                               ,triangulatedSurface(TRUE)
                               ,ijSurface(0)
                               ,isLocked(FALSE)
                               ,context(parentSurface->ParentSurface()->ParentModel()->Context())
                               ,colorTable(0)
{
  isA = R_RescueProperty;
  (*parentSurface->ParentSurface()->properties) += this;
  VetIJSurface();
  loops = new cSetRescueTrimLoopStub();
  data = new RescueArray3dVector(propertyNameIn, propertyTypeIn, unitOfMeasureIn,
                                 parentSurface->Grid(attachedTo), 
                                 nullValue, parentSurface->ParentSurface()->ParentModel(),
                                 TRUE, valueArray);
}

RescueProperty::RescueProperty(RescueSurface *parentSurface,
                               const RESCUECHAR *propertyNameIn, 
                               const RESCUECHAR *propertyTypeIn, const RESCUECHAR *unitOfMeasureIn,
                               AppliesToSurface applies,
                               RESCUEFLOAT nullValue, RESCUEFLOAT *valueArray,
                               AttachmentPoint attachedToIn,
                               RescueIJSurface *surfaceRepresentation)
                               :RescueHistoryObject(parentSurface->ParentModel()->Context())
                               ,parentBlockUnit(0)
                               ,parentGeobodyPart(0)
                               ,geometry(0)
                               ,container(0)
                               ,surface(parentSurface)
                               ,loops(0)
                               ,howApplies(applies)
                               ,attachedTo(attachedToIn)
                               ,triangulatedSurface(FALSE)
                               ,ijSurface(surfaceRepresentation)
                               ,isLocked(FALSE)
                               ,context(parentSurface->ParentModel()->Context())
                               ,colorTable(0)
{
  isA = R_RescueProperty;
  (*parentSurface->properties) += this;
  loops = new cSetRescueTrimLoopStub();
  RESCUEBOOL cellCentered = TRUE;
  VetIJSurface();
  if (attachedTo == NODE_CENTERED)
  {
  cellCentered = FALSE;
  }
  data = new RescueArrayFloat(propertyNameIn, propertyTypeIn, unitOfMeasureIn,
                               ijSurface->Grid(), 
                               nullValue, parentSurface->ParentModel(),
                               cellCentered, valueArray);
}

RescueProperty::RescueProperty(RescueSurface *parentSurface,
                               const RESCUECHAR *propertyNameIn, 
                               const RESCUECHAR *propertyTypeIn, const RESCUECHAR *unitOfMeasureIn,
                               AppliesToSurface applies,
                               RESCUEINT32 nullValue, RESCUEINT32 *valueArray,
                               AttachmentPoint attachedToIn,
                               RescueIJSurface *surfaceRepresentation)
                               :RescueHistoryObject(parentSurface->ParentModel()->Context())
                               ,parentBlockUnit(0)
                               ,parentGeobodyPart(0)
                               ,geometry(0)
                               ,container(0)
                               ,surface(parentSurface)
                               ,loops(0)
                               ,howApplies(applies)
                               ,attachedTo(attachedToIn)
                               ,triangulatedSurface(FALSE)
                               ,ijSurface(surfaceRepresentation)
                               ,isLocked(FALSE)
                               ,context(parentSurface->ParentModel()->Context())
                               ,colorTable(0)
{
  isA = R_RescueProperty;
  (*parentSurface->properties) += this;
  loops = new cSetRescueTrimLoopStub();
  RESCUEBOOL cellCentered = TRUE;
  VetIJSurface();
  if (attachedTo == NODE_CENTERED)
  {
  cellCentered = FALSE;
  }
  data = new RescueArrayInt(propertyNameIn, propertyTypeIn, unitOfMeasureIn,
                               ijSurface->Grid(), 
                               nullValue, parentSurface->ParentModel(),
                               cellCentered, valueArray);
}

RescueProperty::RescueProperty(RescueSurface *parentSurface,
                               const RESCUECHAR *propertyNameIn, 
                               const RESCUECHAR *propertyTypeIn, 
                               const RESCUECHAR *unitOfMeasureIn,
                               AppliesToSurface applies,
                               RescueLookup *lookupTableIn,
                               RESCUEUCHAR nullValue, 
                               RESCUEUCHAR *valueArray,
                               AttachmentPoint attachedToIn,
                               RescueIJSurface *surfaceRepresentation)
                               :RescueHistoryObject(parentSurface->ParentModel()->Context())
                               ,parentBlockUnit(0)
                               ,parentGeobodyPart(0)
                               ,geometry(0)
                               ,container(0)
                               ,surface(parentSurface)
                               ,loops(0)
                               ,howApplies(applies)
                               ,attachedTo(attachedToIn)
                               ,triangulatedSurface(FALSE)
                               ,ijSurface(surfaceRepresentation)
                               ,isLocked(FALSE)
                               ,context(parentSurface->ParentModel()->Context())
                               ,colorTable(0)
{
  isA = R_RescueProperty;
  (*parentSurface->properties) += this;
  loops = new cSetRescueTrimLoopStub();
  VetIJSurface();
  RESCUEBOOL cellCentered = TRUE;
  if (attachedTo == NODE_CENTERED)
  {
  cellCentered = FALSE;
  }
  data = new RescueArrayByte(propertyNameIn, propertyTypeIn, unitOfMeasureIn,
                             ijSurface->Grid(), 
                             nullValue, lookupTableIn, surface->ParentModel(),
                             cellCentered, valueArray); 
}

RescueProperty::RescueProperty(RescueSurface *parentSurface,
                               const RESCUECHAR *propertyNameIn, 
                               const RESCUECHAR *propertyTypeIn, 
                               const RESCUECHAR *unitOfMeasureIn,
                               AppliesToSurface applies,
                               RescueLookup *lookupTableIn,
                               RESCUEUSHORT nullValue, 
                               RESCUEUSHORT *valueArray,
                               AttachmentPoint attachedToIn,
                               RescueIJSurface *surfaceRepresentation)
                               :RescueHistoryObject(parentSurface->ParentModel()->Context())
                               ,parentBlockUnit(0)
                               ,parentGeobodyPart(0)
                               ,geometry(0)
                               ,container(0)
                               ,surface(parentSurface)
                               ,loops(0)
                               ,howApplies(applies)
                               ,attachedTo(attachedToIn)
                               ,triangulatedSurface(FALSE)
                               ,ijSurface(surfaceRepresentation)
                               ,isLocked(FALSE)
                               ,context(parentSurface->ParentModel()->Context())
                               ,colorTable(0)
{
  isA = R_RescueProperty;
  (*parentSurface->properties) += this;
  loops = new cSetRescueTrimLoopStub();
  VetIJSurface();
  RESCUEBOOL cellCentered = TRUE;
  if (attachedTo == NODE_CENTERED)
  {
  cellCentered = FALSE;
  }
  data = new RescueArrayShort(propertyNameIn, propertyTypeIn, unitOfMeasureIn,
                               ijSurface->Grid(), 
                               nullValue, lookupTableIn, 
                               surface->ParentModel(),
                               cellCentered, valueArray);
}

RescueProperty::RescueProperty(RescueSurface *parentSurface,
                               const RESCUECHAR *propertyNameIn, 
                               const RESCUECHAR *propertyTypeIn, 
                               const RESCUECHAR *unitOfMeasureIn,
                               AppliesToSurface applies,
                               Rescue2dVector nullValue, 
                               Rescue2dVector *valueArray,
                               AttachmentPoint attachedToIn,
                               RescueIJSurface *surfaceRepresentation)
                               :RescueHistoryObject(parentSurface->ParentModel()->Context())
                               ,parentBlockUnit(0)
                               ,parentGeobodyPart(0)
                               ,geometry(0)
                               ,container(0)
                               ,surface(parentSurface)
                               ,loops(0)
                               ,howApplies(applies)
                               ,attachedTo(attachedToIn)
                               ,triangulatedSurface(FALSE)
                               ,ijSurface(surfaceRepresentation)
                               ,isLocked(FALSE)
                               ,context(parentSurface->ParentModel()->Context())
                               ,colorTable(0)
{
  isA = R_RescueProperty;
  (*parentSurface->properties) += this;
  loops = new cSetRescueTrimLoopStub();
  VetIJSurface();
  RESCUEBOOL cellCentered = TRUE;
  if (attachedTo == NODE_CENTERED)
  {
  cellCentered = FALSE;
  }
  data = new RescueArray2dVector(propertyNameIn, propertyTypeIn, unitOfMeasureIn,
                                 ijSurface->Grid(), 
                                 nullValue, parentSurface->ParentModel(),
                                 cellCentered, valueArray);
}

RescueProperty::RescueProperty(RescueSurface *parentSurface,
                               const RESCUECHAR *propertyNameIn, 
                               const RESCUECHAR *propertyTypeIn, 
                               const RESCUECHAR *unitOfMeasureIn,
                               AppliesToSurface applies,
                               Rescue3dVector nullValue, 
                               Rescue3dVector *valueArray,
                               AttachmentPoint attachedToIn,
                               RescueIJSurface *surfaceRepresentation)
                               :RescueHistoryObject(parentSurface->ParentModel()->Context())
                               ,parentBlockUnit(0)
                               ,parentGeobodyPart(0)
                               ,geometry(0)
                               ,container(0) 
          ,surface(parentSurface)
                               ,loops(0)
                               ,howApplies(applies)
                               ,attachedTo(attachedToIn)
                               ,triangulatedSurface(FALSE)
                               ,ijSurface(surfaceRepresentation)
                               ,isLocked(FALSE)
                               ,context(parentSurface->ParentModel()->Context())
                               ,colorTable(0)
{
  isA = R_RescueProperty;
  (*parentSurface->properties) += this;
  loops = new cSetRescueTrimLoopStub();
  VetIJSurface();
  RESCUEBOOL cellCentered = TRUE;
  if (attachedTo == NODE_CENTERED)
  {
  cellCentered = FALSE;
  }
  data = new RescueArray3dVector(propertyNameIn, propertyTypeIn, unitOfMeasureIn,
                                 ijSurface->Grid(), 
                                 nullValue, surface->ParentModel(),
                                 cellCentered, valueArray);
}

RescueProperty::RescueProperty(RescueGeometry *geometryIn, 
                               const RESCUECHAR *propertyNameIn, 
                               const RESCUECHAR *propertyTypeIn, const RESCUECHAR *unitOfMeasureIn,
                               RESCUEFLOAT nullValue, RESCUEFLOAT *valueArray,
                               AttachmentPoint attachedToIn)
                               :RescueHistoryObject(geometryIn->ParentModel()->Context())
                               ,parentBlockUnit(0)
                               ,parentGeobodyPart(0)
                               ,geometry(geometryIn)
                               ,container(0)
                               ,surface(0)
                               ,loops(0)
                               ,howApplies(RescueProperty::NA)
                               ,attachedTo(attachedToIn)
                               ,triangulatedSurface(FALSE)
                               ,ijSurface(0)
                               ,isLocked(FALSE)
                               ,context(geometryIn->ParentModel()->Context())
                               ,colorTable(0)
{
  isA = R_RescueProperty;
  (*geometry->properties) += this;
  parentBlockUnit = geometry->ParentBlockUnit();
  parentGeobodyPart = geometry->ParentGeobodyPart();
  data = new RescueArrayFloat(propertyNameIn, propertyTypeIn, unitOfMeasureIn,
                               geometry->Grid(), 
                               nullValue, geometry->ParentModel(),
                               (attachedTo != NODE_CENTERED), valueArray);
}

RescueProperty::RescueProperty(RescueGeometry *geometryIn, 
                               const RESCUECHAR *propertyNameIn, 
                               const RESCUECHAR *propertyTypeIn, const RESCUECHAR *unitOfMeasureIn,
                               RESCUEINT32 nullValue, RESCUEINT32 *valueArray,
                               AttachmentPoint attachedToIn)
                               :RescueHistoryObject(geometryIn->ParentModel()->Context())
                               ,parentBlockUnit(0)
                               ,parentGeobodyPart(0)
                               ,geometry(geometryIn)
                               ,container(0)
                               ,surface(0)
                               ,loops(0)
                               ,howApplies(RescueProperty::NA)
                               ,attachedTo(attachedToIn)
                               ,triangulatedSurface(FALSE)
                               ,isLocked(FALSE)
                               ,context(geometryIn->ParentModel()->Context())
                               ,colorTable(0)
{
  isA = R_RescueProperty;
  (*geometry->properties) += this;
  parentBlockUnit = geometry->ParentBlockUnit();
  parentGeobodyPart = geometry->ParentGeobodyPart();
  data = new RescueArrayInt(propertyNameIn, propertyTypeIn, unitOfMeasureIn,
                               geometry->Grid(), 
                               nullValue, geometry->ParentModel(),
                               (attachedTo != NODE_CENTERED), valueArray);
}

RescueProperty::RescueProperty(RescueGeometry *geometryIn, 
                               const RESCUECHAR *propertyNameIn, 
                               const RESCUECHAR *propertyTypeIn, 
                               const RESCUECHAR *unitOfMeasureIn,
                               Rescue2dVector nullValue, 
                               Rescue2dVector *valueArray,
                               AttachmentPoint attachedToIn)
                               :RescueHistoryObject(geometryIn->ParentModel()->Context())
                               ,parentBlockUnit(0)
                               ,parentGeobodyPart(0)
                               ,geometry(geometryIn)
                               ,container(0)
                               ,surface(0)
                               ,loops(0)
                               ,howApplies(RescueProperty::NA)
                               ,attachedTo(attachedToIn)
                               ,triangulatedSurface(FALSE)
                               ,isLocked(FALSE)
                               ,context(geometryIn->ParentModel()->Context())
                               ,colorTable(0)
{
  isA = R_RescueProperty;
  (*geometry->properties) += this;
  parentBlockUnit = geometry->ParentBlockUnit();
  parentGeobodyPart = geometry->ParentGeobodyPart();
  data = new RescueArray2dVector(propertyNameIn, propertyTypeIn, unitOfMeasureIn,
                                 geometry->Grid(), 
                                 nullValue, geometry->ParentModel(),
                                 (attachedTo != NODE_CENTERED), valueArray);
}

RescueProperty::RescueProperty(RescueGeometry *geometryIn, 
                               const RESCUECHAR *propertyNameIn, 
                               const RESCUECHAR *propertyTypeIn, 
                               const RESCUECHAR *unitOfMeasureIn,
                               Rescue3dVector nullValue, 
                               Rescue3dVector *valueArray,
                               AttachmentPoint attachedToIn)
                               :RescueHistoryObject(geometryIn->ParentModel()->Context())
                               ,parentBlockUnit(0)
                               ,parentGeobodyPart(0)
                               ,geometry(geometryIn)
                               ,container(0)
                               ,surface(0)
                               ,loops(0)
                               ,howApplies(RescueProperty::NA)
                               ,attachedTo(attachedToIn)
                               ,triangulatedSurface(FALSE)
                               ,isLocked(FALSE)
                               ,context(geometryIn->ParentModel()->Context())
                               ,colorTable(0)
{
  isA = R_RescueProperty;
  (*geometry->properties) += this;
  parentBlockUnit = geometry->ParentBlockUnit();
  parentGeobodyPart = geometry->ParentGeobodyPart();
  data = new RescueArray3dVector(propertyNameIn, propertyTypeIn, unitOfMeasureIn,
                                 geometry->Grid(), 
                                 nullValue, geometry->ParentModel(),
                                 (attachedTo != NODE_CENTERED), valueArray);
}

RescueProperty::RescueProperty(RescueGeometry *geometryIn, 
                               const RESCUECHAR *propertyNameIn, 
                               const RESCUECHAR *propertyTypeIn, 
                               const RESCUECHAR *unitOfMeasureIn,
                               RescueLookup *lookupTableIn,
                               RESCUEUCHAR nullValue, 
                               RESCUEUCHAR *valueArray,
                               AttachmentPoint attachedToIn)
                               :RescueHistoryObject(geometryIn->ParentModel()->Context())
                               ,parentBlockUnit(0)
                               ,parentGeobodyPart(0)
                               ,geometry(geometryIn)
                               ,container(0)
                               ,surface(0)
                               ,loops(0)
                               ,howApplies(RescueProperty::NA)
                               ,attachedTo(attachedToIn)
                               ,triangulatedSurface(FALSE)
                               ,isLocked(FALSE)
                               ,context(geometryIn->ParentModel()->Context())
                               ,colorTable(0)
{
  isA = R_RescueProperty;
  (*geometry->properties) += this;
  parentBlockUnit = geometry->ParentBlockUnit();
  parentGeobodyPart = geometry->ParentGeobodyPart();
  data = new RescueArrayByte(propertyNameIn, propertyTypeIn, unitOfMeasureIn,
                               geometry->Grid(), 
                               nullValue, lookupTableIn, geometry->ParentModel(),
                               (attachedTo != NODE_CENTERED), valueArray); 
}

RescueProperty::RescueProperty(RescueGeometry *geometryIn, 
                               const RESCUECHAR *propertyNameIn, 
                               const RESCUECHAR *propertyTypeIn, 
                               const RESCUECHAR *unitOfMeasureIn,
                               RescueLookup *lookupTableIn,
                               RESCUEUSHORT nullValue, 
                               RESCUEUSHORT *valueArray,
                               AttachmentPoint attachedToIn)
                               :RescueHistoryObject(geometryIn->ParentModel()->Context())
                               ,parentBlockUnit(0)
                               ,parentGeobodyPart(0)
                               ,geometry(geometryIn)
                               ,container(0)
                               ,surface(0)
                               ,loops(0)
                               ,howApplies(RescueProperty::NA)
                               ,attachedTo(attachedToIn)
                               ,triangulatedSurface(FALSE)
                               ,isLocked(FALSE)
                               ,context(geometryIn->ParentModel()->Context())
                               ,colorTable(0)
{
  isA = R_RescueProperty;
  (*geometry->properties) += this;
  parentBlockUnit = geometry->ParentBlockUnit();
  parentGeobodyPart = geometry->ParentGeobodyPart();
  data = new RescueArrayShort(propertyNameIn, propertyTypeIn, unitOfMeasureIn,
                               geometry->Grid(), 
                               nullValue, lookupTableIn, 
                               geometry->ParentModel(),
                               (attachedTo != NODE_CENTERED), valueArray);
}

RescueProperty::~RescueProperty()
{
  delete data;
  if (loops != 0)
  {
  delete loops;
  }
  if (colorTable != 0)
  {
  delete colorTable;
  }
}

void RescueProperty::VetIJSurface()
{
  if (ijSurface == 0)
  {
  ijSurface = surface->NthIJSurface(0);
  }
}

RescueProperty::RescueProperty(RescueContext *contextIn, FILE *archiveFile)
                :RescueHistoryObject(contextIn)
                ,parentBlockUnit(0)
                ,parentGeobodyPart(0)
                ,geometry(0)
                ,container(0)
                ,surface(0)
                ,loops(0)
                ,triangulatedSurface(FALSE)
                ,ijSurface(0)
                ,isLocked(FALSE)
                ,context(contextIn)
                ,colorTable(0)
{
  RESCUEINT64 typeFlag = 1;
  if (context->ReadFileVersion() >= 11)
  {
  ReadId(context, archiveFile);
  }
  if (context->ReadFileVersion() >= 21)
  {
  myfscanf(context, archiveFile, &isLocked);
  }
  if (context->ReadFileVersion() >= 20)
  {
  myfscanf(context, archiveFile, &typeFlag);
  if (context->ReadFileVersion() >= 27)
  {
      RESCUEINT64 flag;
      myfscanf(context, archiveFile, &flag);
      attachedTo = (AttachmentPoint) flag;
  }
  if (typeFlag == 3)
  {
      myfscanf(context, archiveFile, &triangulatedSurface);
      if (context->ReadFileVersion() >= 21)
      {
    RESCUEINT64 flag;
    if (context->ReadFileVersion() < 27)
    {
          myfscanf(context, archiveFile, &flag);
          attachedTo = (AttachmentPoint) flag;
    }
    myfscanf(context, archiveFile, &flag);
    howApplies = (AppliesToSurface) flag;
      }
  }
  else
  {
      if (context->ReadFileVersion() < 27)
      {
    attachedTo = CELL_CENTERED;
      }
      howApplies = NA;
  }
  }
  else
  {
  if (context->ReadFileVersion() < 27)
  {
      attachedTo = CELL_CENTERED;
  }
  howApplies = NA;
  }

  if (context->ReadFileVersion() >= 5)
  {
  RESCUEINT64 flag;

  myfscanf(context, archiveFile, &flag);
  if (context->ReadFileVersion() >= 23 && typeFlag == 3)
  {
      myfscanf(context, archiveFile, &ijSurfaceId);
  }
  else
  {
      ijSurfaceId = 0;
  }

  switch (flag)
  {
  case R_RescueArrayByte:
      data = new RescueArrayByte(context, archiveFile);
      break;
  case R_RescueArrayShort:
      data = new RescueArrayShort(context, archiveFile);
      break;
  case R_RescueArray2dVector:
      data = new RescueArray2dVector(context, archiveFile);
      break;
  case R_RescueArray3dVector:
      data = new RescueArray3dVector(context, archiveFile);
      break;
  case R_RescueArrayInt:
      data = new RescueArrayInt(context, archiveFile);
      break;
  default:
      data = new RescueArrayFloat(context, archiveFile);
      break;
  }
  }
  else
  {
  data = new RescueArrayFloat(context, archiveFile);
  }
  isA = R_RescueProperty;

  if (context->ReadFileVersion() >= 18 && typeFlag != 3 && typeFlag != 4)
  {
  myfscanf(context, archiveFile, &geometryId);
  }
  else
  {
  geometryId = 0;
  }
  if (context->ReadFileVersion() >= 20)
  {
  RESCUEINT64 flag;
  myfscanf(context, archiveFile, &flag);
  if (flag == 1)
  {
      colorTable = new RescueColorTable(context, archiveFile);
  }
  }
  if (context->properties != 0)
  {
  context->properties->Add(this);
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

void RescueProperty::Relink(RescueObject *parentObject)
{
/*
  Pass the RescueGeometry, RescueSurface, RescueDataContainer I belong to.
*/
  RescueModel *parentModel = 0;
  if (parentObject->IsA() == R_RescueGeometry)
  {
  geometry = (RescueGeometry *) parentObject;
  data->SetGrid(geometry->Grid());
  parentModel = geometry->ParentModel();
  parentBlockUnit = geometry->ParentBlockUnit();
  parentGeobodyPart = geometry->ParentGeobodyPart();
  }
  else if (parentObject->IsOfType(R_RescueSurface))
  {
  surface = (RescueSurface *) parentObject;
  ijSurface = 0;
  if (ijSurfaceId == 0)
  {
      VetIJSurface();
  }
  else
  {
      ijSurface = surface->SurfaceIJIdentifiedBy(ijSurfaceId);
  }

  if (triangulatedSurface)
  {
      data->SetGrid(surface->TriangulatedSurface()->Grid(attachedTo));
  }
  else
  {
      data->SetGrid(ijSurface->Grid());
  }
  parentModel = surface->ParentModel();
  loops = new cSetRescueTrimLoopStub();
  }
  else if (parentObject->IsA() == R_RescueDataContainer)
  {
  container = (RescueDataContainer *) parentObject;
  data->SetGrid(container->Grid());
  parentModel = container->ParentModel();
  }
  data->Relink(parentModel);
  if (colorTable != 0)
  {
  colorTable->Relink(parentModel);
  }
}

void RescueProperty::Archive(FILE *archiveFile)
{
  RESCUEBOOL archiveGeometry = TRUE;
  if (parentBlockUnit != 0)
  {
  if (context->FileVersion() == 9)
  {
      if (geometry != parentBlockUnit->GridGeometry(0))
      {
    archiveGeometry = FALSE;
      }
  }
/*
  trim properties that relate to any subsequent geometry.
*/
  if (context->FileVersion()  < 20)
  {
      if (data->IsA() == R_RescueArray2dVector
      ||  data->IsA() == R_RescueArray3dVector)
      {
    archiveGeometry = FALSE;
      }
  }
/*
  Don't output types that previous versions don't know how to handle.
*/
  if (archiveGeometry)
  {
      myfprintf(context, archiveFile, "; Property");
      if (context->FileVersion() > 9)
      {
    myfprintf(context, archiveFile, Identifier());
      }
      if (context->FileVersion() >= 21)
      {
    myfprintf(context, archiveFile, isLocked);
      }

      if (context->FileVersion() >= 20)
      {
    myfprintf(context, archiveFile, (RESCUEINT64) 1);
      }
      if (context->FileVersion() >= 27)
      {
    myfprintf(context, archiveFile, (RESCUEINT64) attachedTo);
      }
      myfprintf(context, archiveFile, (RESCUEINT64) data->IsA());
      data->Archive(archiveFile);
      if (context->FileVersion() >= 18)
      {
    myfprintf(context, archiveFile, geometry->Identifier());
      }
  }
  }
  if (context->FileVersion() >= 20)
  {
  if (container != 0)
  {
      myfprintf(context, archiveFile, Identifier());
      myfprintf(context, archiveFile, isLocked);
      myfprintf(context, archiveFile, (RESCUEINT64) 4);
      myfprintf(context, archiveFile, (RESCUEINT64) attachedTo);
      myfprintf(context, archiveFile, (RESCUEINT64) data->IsA());
      data->Archive(archiveFile);
  }
  else if (surface == 0 && parentBlockUnit == 0)       // was if (parentGeobodyPart != 0)
              // now test is if geobodyPart or global to model.
  {
      myfprintf(context, archiveFile, Identifier());
      if (context->FileVersion() >= 21)
      {
    myfprintf(context, archiveFile, isLocked);
      }

      myfprintf(context, archiveFile, (RESCUEINT64) 2);
      if (context->FileVersion() >= 27)
      {
    myfprintf(context, archiveFile, (RESCUEINT64) attachedTo);
      }
      myfprintf(context, archiveFile, (RESCUEINT64) data->IsA());
      data->Archive(archiveFile);
      myfprintf(context, archiveFile, geometry->Identifier());
  }
  else if (surface != 0)
  {
      myfprintf(context, archiveFile, Identifier());
      if (context->FileVersion() >= 21)
      {
    myfprintf(context, archiveFile, isLocked);
      }

      myfprintf(context, archiveFile, (RESCUEINT64) 3);
      if (context->FileVersion() >= 27)
      {
    myfprintf(context, archiveFile, (RESCUEINT64) attachedTo);
      }
      myfprintf(context, archiveFile, triangulatedSurface);
      if (context->FileVersion() >= 21)
      {
    if (context->FileVersion() < 27)
    {
          myfprintf(context, archiveFile, (RESCUEINT64) attachedTo);
    }
    myfprintf(context, archiveFile, (RESCUEINT64) howApplies);
      }
      myfprintf(context, archiveFile, (RESCUEINT64) data->IsA());

      if (context->FileVersion() >= 23)
      {
    if (ijSurface == 0)
    {
          myfprintf(context, archiveFile, (RESCUEINT64) 0);
    }
    else
    {
          myfprintf(context, archiveFile, ijSurface->Identifier());
    }
      }

      if (triangulatedSurface
      &&  data->ParentModel()->propertyActionImmediate)
      {
    RescueTriangulatedSurface *tri = surface->TriangulatedSurface();
    if (tri->IsLoaded() == FALSE)
    {
          tri->ReadData(data->ParentModel()->oldPathName);
          tri->hasChanged = FALSE;
          context->binaryFlag = data->ParentModel()->currentBinary;
    }
      }
/*
  If we will be writing the property back, we need
  to load the triangulation on which it is based.

  Can't use regular load() because that will use the
  new file name, and since triangles come after properties
  in RescueSurface, that won't be established yet.
*/
      data->Archive(archiveFile);
  }
  if (colorTable == 0)
  {
      myfprintf(context, archiveFile, (RESCUEINT64) 0);
  }
  else
  {
      myfprintf(context, archiveFile, (RESCUEINT64) 1);
      colorTable->Archive(archiveFile);
  }
  }
  if (context->FileVersion() >= 37)
  {
  myfprintf(context, archiveFile, "EOD");
  }
}

void RescueProperty::DropWireframeMemory()
{
  if (loops != 0)
  {
  delete loops;
  }
  loops = new cSetRescueTrimLoopStub();
}

void RescueProperty::UnArchiveWireframeData(RescueModel *model, FILE *archiveFile)
{
  if (context->ReadFileVersion() >= 28)
  {
  loops->UnArchive(context, archiveFile);
  }
  else
  {
  loops->EmptySelf();
  RESCUEINT64 howMany;
  myfscanf(context, archiveFile, &howMany);
  RESCUEINT64 loop;
  for (loop = 0; loop < howMany; loop++)
  {
      RescueTrimLoop *loopObj = new RescueTrimLoop(context, archiveFile);
      RescueTrimLoopStub *stub = new RescueTrimLoopStub(context, loopObj);
      model->wireframes->SaveCompatibleTrimLoop(loopObj, this, stub);
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

void RescueProperty::RelinkWireframeData(RescueObject *parent)
{
}

void RescueProperty::ArchiveWireframeData(FILE *archiveFile)
{
  RESCUEBOOL archiveGeometry = TRUE;
  if (context->FileVersion() == 19)
  {
  if (data->IsA() == R_RescueArray2dVector
  ||  data->IsA() == R_RescueArray3dVector)
  {
      archiveGeometry = FALSE;
  }
  }
/*
  Don't output types that previous versions don't know how to handle.
  If we did not archive ourselves, then we won't be there to unarchive
  the wireframe data either.
*/
  if (archiveGeometry)
  {
  if (context->FileVersion() >= 28)
  {
      loops->Archive(context, archiveFile);
  }
  else
  {
      RescueModel *model = ParentSurface()->ParentModel();
      cBagRescueTrimLoop *loopObjs = new cBagRescueTrimLoop();
      RESCUEINT64 ordinal = 0;
      RescueTrimLoopStub *stub = NthTrimLoop(ordinal++);
      while (stub != 0)
      {
    RescueTrimLoop *loopObj = stub->TrimLoop(model);
    if (loopObj != 0)
    {
          (*loopObjs) += loopObj;
    }
    stub = NthTrimLoop(ordinal++);
      }
/*
  Because we don't know if we have any "nonsignifigant trailing branches"
  or not, we have to get the list of objects before we can write out the
  count.
*/
      RESCUEINT64 howMany = loopObjs->Count64();
      myfprintf(context, archiveFile, howMany);
      RESCUEINT64 loop;
      for (loop = 0; loop < howMany; loop++)
      {
    RescueTrimLoop *loopObj = loopObjs->NthObject(loop);
    loopObj->Archive(context, archiveFile);
      }
      delete loopObjs;
  }
/*
  Archive the actual object.
*/
  }
  if (context->FileVersion() >= 37)
  {
  myfprintf(context, archiveFile, "EOD");
  }
}

RESCUEBOOL RescueProperty::IsNamed(const RESCUECHAR *possibleName)
{
   return ((*data->propertyName) == possibleName);
}

RESCUEBOOL RescueProperty::IsOfType(_RescueObjectType thisType)
{
  if (thisType == R_RescueProperty)
  {
  return TRUE;
  }
  else
  {
  return RescueHistoryObject::IsOfType(thisType);
  }
}

void RescueProperty::SetColorTable(RescueColorTable *newTable)
{
  if (colorTable != 0)
  {
  colorTable->RescueDeleteFile();
  delete colorTable;
  }
  colorTable = newTable;
}

void RescueProperty::Dispose() 
{
  data->RescueDeleteFile();
  if (colorTable != 0)
  {
  colorTable->RescueDeleteFile();
  }
}

void RescueProperty::AddTrimLoop(RescueTrimLoop *existingLoop)
{
  if (loops != 0) 
  {
  (*loops) += new RescueTrimLoopStub(context, existingLoop);
  existingLoop->propertyID = this->uniqueID;
  }
}

void RescueProperty::RelinkTrimLoops(RescueModel *model)
{
  if (loops != 0)
  {
  RESCUEINT64 ordinal = 0;
  RescueTrimLoopStub *stub = NthTrimLoop(ordinal++);
  while (stub != 0)
  {
      RescueTrimLoop *loopObj = stub->TrimLoop(model);
      if (loopObj != 0)
      {
    loopObj->propertyID = this->uniqueID;
    RescueWireframe *wireframe = loopObj->Owner();
    if (wireframe != 0)
    {
          wireframe->MarkWireframeChanged();
    }
      }
      stub = NthTrimLoop(ordinal++);
  }
  }
}

void RescueProperty::SwapAxes(bool swapI, RESCUEINT64 iNodes, bool swapJ, RESCUEINT64 jNodes) 
{
  if (data->CellCentered() == TRUE)
  {
  iNodes--;
  jNodes--;
  }
  data->SwapAxes(swapI, iNodes, swapJ, jNodes);
}

void RescueProperty::SwapAxes(bool swapI, RESCUEINT64 iNodes, 
              bool swapJ, RESCUEINT64 jNodes,
              bool swapK, RESCUEINT64 kNodes) 
{
  if (data->CellCentered() == TRUE)
  {
  iNodes--;
  jNodes--;
  kNodes--;
  }
  data->SwapAxes(swapI, iNodes, swapJ, jNodes, swapK, kNodes);
}


