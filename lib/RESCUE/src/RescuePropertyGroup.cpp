/********************************************************************

  RescuePropertyGroup.cpp

  block unit for RESCUE's data model.

  Rod Hanks,  May 1996

*********************************************************************/
#include "RescuePropertyGroup.h"
#include "RescueBlockUnit.h"
#include "RescueBuffer.h"
#include "RescueDataContainer.h"
#include "RescueGeobody.h"
#include "RescueGeobodyPart.h"
#include "RescueIdTree.h"
#include "RescueModel.h"
#include "RescueSurface.h"
#include "RescueTimeStepManager.h"
#include "RescueUnit.h"
#include <string.h>

RescueTimeStepManager *RescuePropertyGroup::GetTimeStepManager(RESCUEBOOL refresh) {
  if (refresh && tsManager != 0) {
    delete tsManager;
    tsManager = 0;
  }
  if (tsManager == 0) {
    tsManager = new RescueTimeStepManager(this);
  }
  return tsManager;
}

RescueTimeStepGroup *RescuePropertyGroup::GetStaticGroup() {
  RescueTimeStepGroup *defaultGroup = 0;
  RESCUEINT64 ndx = 0;
  RescueTimeStepGroup *group = timeSteps->NthObject(ndx++);
  while (group != 0 && defaultGroup == 0) {
    if (*(group->TimeStepName()) == "Default Group") {
      defaultGroup = group;
    } else {
      group = timeSteps->NthObject(ndx++);
    }
  }
  if (defaultGroup == 0) {
    defaultGroup = new RescueTimeStepGroup("Default Group", this);
  }
  return defaultGroup;
}

RescueTimeStepGroup *RescuePropertyGroup::GetInitializationGroup() {
  RescueTimeStepGroup *defaultGroup = 0;
  RESCUEINT64 ndx = 0;
  RescueTimeStepGroup *group = timeSteps->NthObject(ndx++);
  while (group != 0 && defaultGroup == 0) {
    if (*(group->TimeStepName()) == "Initialization Group") {
      defaultGroup = group;
    } else {
      group = timeSteps->NthObject(ndx++);
    }
  }
  if (defaultGroup == 0) {
    defaultGroup = new RescueTimeStepGroup("Initialization Group", this);
  }
  return defaultGroup;
}

void RescuePropertyGroup::AddProperty(RescueProperty *toAdd) {
  RescueTimeStepGroup *defaultGroup = GetStaticGroup();
  defaultGroup->AddProperty(toAdd);
}

RESCUEBOOL RescuePropertyGroup::RemoveRescueProperty(RescueProperty *unitToDrop) {
  RESCUEBOOL myReturn = FALSE;
  RESCUEINT64 ndx = 0;
  RescueTimeStepGroup *group = timeSteps->NthObject(ndx++);
  while (group != 0) {
    if (group->RemoveRescueProperty(unitToDrop)) {
      myReturn = TRUE;
    }
    group = timeSteps->NthObject(ndx++);
  }
  return myReturn;
}

RescuePropertyGroup::~RescuePropertyGroup() {
  if (groupName != 0) {
    delete groupName;
  }
  if (timeSteps != 0) {
    delete timeSteps;
  }
  if (tsManager != 0) {
    delete tsManager;
  }
}

void RescuePropertyGroup::SetGroupName(const RESCUECHAR *newGroupName) { (*groupName) = newGroupName; }

RescuePropertyGroup *RescuePropertyGroup::MakeRescuePropertyGroup(const RESCUECHAR *groupNameIn, RescueModel *modelIn,
                                                                  RescueDataContainer *containerIn) {
  RescuePropertyGroup *myReturn = 0;
  if (containerIn->propertyGroups->ObjectNamed(groupNameIn) == 0) {
    myReturn = new RescuePropertyGroup(groupNameIn, modelIn, containerIn);
  }
  return myReturn;
}

RescuePropertyGroup::RescuePropertyGroup(const RESCUECHAR *groupNameIn, RescueModel *modelIn,
                                         RescueDataContainer *containerIn)
    : RescueHistoryObject(modelIn->Context()), groupName(0), model(modelIn), unit(0), blockUnit(0), blockUnitID(0),
      body(0), bodyPart(0), surface(0), container(containerIn), timeSteps(0), isLocked(FALSE),
      context(modelIn->Context()), tsManager(0), properties(0) {
  groupName = new RCHString(groupNameIn);
  timeSteps = new cSetRescueTimeStepGroup();

  (*container->propertyGroups) += this;
  isA = R_RescuePropertyGroup;
}

RescuePropertyGroup *RescuePropertyGroup::MakeRescuePropertyGroup(const RESCUECHAR *groupNameIn,
                                                                  RescueSurface *surfaceIn) {
  RescuePropertyGroup *myReturn = 0;
  if (surfaceIn->groups->ObjectNamed(groupNameIn) == 0) {
    myReturn = new RescuePropertyGroup(groupNameIn, surfaceIn);
  }
  return myReturn;
}

RescuePropertyGroup::RescuePropertyGroup(const RESCUECHAR *groupNameIn, RescueSurface *surfaceIn)
    : RescueHistoryObject(surfaceIn->ParentModel()->Context()), groupName(0), model(0), unit(0), blockUnit(0),
      blockUnitID(0), body(0), bodyPart(0), surface(surfaceIn), container(0), timeSteps(0), isLocked(FALSE),
      context(surfaceIn->ParentModel()->Context()), tsManager(0), properties(0) {
  groupName = new RCHString(groupNameIn);
  timeSteps = new cSetRescueTimeStepGroup();

  (*surface->groups) += this;
  isA = R_RescuePropertyGroup;
}

RescuePropertyGroup *RescuePropertyGroup::MakeRescuePropertyGroup(const RESCUECHAR *groupNameIn,
                                                                  RescueGeobodyPart *bodyPartIn) {
  RescuePropertyGroup *myReturn = 0;
  RescueGeobody *body = bodyPartIn->Body();
  if (body->groups->ObjectNamed(groupNameIn) == 0) {
    myReturn = new RescuePropertyGroup(groupNameIn, bodyPartIn);
  }
  return myReturn;
}

RescuePropertyGroup::RescuePropertyGroup(const RESCUECHAR *groupNameIn, RescueGeobodyPart *bodyPartIn)
    : RescueHistoryObject(bodyPartIn->Body()->ParentModel()->Context()), groupName(0), model(0), unit(0), blockUnit(0),
      blockUnitID(0), body(0), bodyPart(bodyPartIn), surface(0), container(0), timeSteps(0), isLocked(FALSE),
      context(bodyPartIn->Body()->ParentModel()->Context()), tsManager(0), properties(0) {
  groupName = new RCHString(groupNameIn);
  timeSteps = new cSetRescueTimeStepGroup();

  body = bodyPart->Body();
  (*body->groups) += this;
  bodyPart->AddPropertyGroup(this);
  isA = R_RescuePropertyGroup;
}

RescuePropertyGroup *RescuePropertyGroup::MakeRescuePropertyGroup(const RESCUECHAR *groupNameIn,
                                                                  RescueBlockUnit *blockUnitIn) {
  RescuePropertyGroup *myReturn = 0;
  if (blockUnitIn->Unit()->groups->ObjectNamed(groupNameIn) == 0) {
    myReturn = new RescuePropertyGroup(groupNameIn, blockUnitIn);
  }
  return myReturn;
}

RescuePropertyGroup::RescuePropertyGroup(const RESCUECHAR *groupNameIn, RescueBlockUnit *blockUnitIn)
    : RescueHistoryObject(blockUnitIn->ParentModel()->Context()), groupName(0), model(0), unit(0),
      blockUnit(blockUnitIn), blockUnitID(0), body(0), bodyPart(0), surface(0), container(0), timeSteps(0),
      isLocked(FALSE), context(blockUnitIn->ParentModel()->Context()), tsManager(0), properties(0) {
  groupName = new RCHString(groupNameIn);
  timeSteps = new cSetRescueTimeStepGroup();

  unit = blockUnit->Unit();
  (*unit->groups) += this;
  blockUnit->AddPropertyGroup(this);
  isA = R_RescuePropertyGroup;
}

RescuePropertyGroup *RescuePropertyGroup::MakeRescuePropertyGroup(const RESCUECHAR *groupNameIn, RescueModel *modelIn) {
  RescuePropertyGroup *myReturn = 0;
  if (modelIn->localGroups->ObjectNamed(groupNameIn) == 0) {
    myReturn = new RescuePropertyGroup(groupNameIn, modelIn);
  }
  return myReturn;
}

RescuePropertyGroup::RescuePropertyGroup(const RESCUECHAR *groupNameIn, RescueModel *modelIn)
    : RescueHistoryObject(modelIn->Context()), groupName(0), model(modelIn), unit(0), blockUnit(0), blockUnitID(0),
      body(0), bodyPart(0), surface(0), container(0), timeSteps(0), isLocked(FALSE), context(modelIn->Context()),
      tsManager(0), properties(0) {
  groupName = new RCHString(groupNameIn);
  timeSteps = new cSetRescueTimeStepGroup();

  (*model->localGroups) += this;
  isA = R_RescuePropertyGroup;
}

RescuePropertyGroup::RescuePropertyGroup(RescueContext *contextIn, FILE *archiveFile)
    : RescueHistoryObject(contextIn), groupName(0), model(0), unit(0), blockUnit(0), blockUnitID(0), body(0),
      bodyPart(0), surface(0), container(0), timeSteps(0), isLocked(FALSE), context(contextIn), tsManager(0),
      properties(0) {
  RESCUECHAR myString[255];

  isA = R_RescuePropertyGroup;
  ReadId(context, archiveFile);
  myfgets(context, myString, 255, archiveFile);
  groupName = new RCHString(myString);

  if (context->ReadFileVersion() >= 21) {
    myfscanf(context, archiveFile, &isLocked);
  }

  timeSteps = new cSetRescueTimeStepGroup();
  myfscanf(context, archiveFile, &blockUnitID);
  blockUnit = 0;
  if (context->ReadFileVersion() >= 11) {
    timeSteps->UnArchive(context, archiveFile);
  } else {
    new RescueTimeStepGroup("Default Group", this);
    /*
      This gives us one group for compatibility.
    */
    properties = new cSetRescueProperty();
    properties->UnArchive(context, archiveFile);
    /*
      Unarchive the old properties.  We'll pass them to RescueBlockUnit
      later.
    */
  }
  if (context->propertyGroups != 0) {
    context->propertyGroups->Add(this);
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

void RescuePropertyGroup::Relink(RescueObject *parent) {
  /*
    Pass the owning object (RescueModel, RescueUnit, RescueGeobody, or RescueSurface)
  */
  if (parent->IsA() == R_RescueUnit) {
    unit = (RescueUnit *)parent;
    blockUnit = unit->ParentModel()->BlockUnitIdentifiedBy(blockUnitID);
    timeSteps->Relink(this);
    if (properties != 0) {
      RescueTimeStepGroup *timeStep = timeSteps->NthObject(0);
      if (timeStep != 0) {
        RESCUEINT64 howMany = properties->Count64();
        RESCUEINT64 loop;
        for (loop = 0; loop < howMany; loop++) {
          RescueProperty *property = properties->NthObject(loop);
          (*blockUnit->GridGeometry()->properties) += property;
          timeStep->AddProperty(property);
        }
      }
      properties->count = 0;
      /*
        Necessary to keep the properties from being deleted with the set.
      */
      delete properties;
      properties = 0;
    }
    /*
      For compatibility with older files, hand these properties we gathered over
      to our block unit, but add them to our time step group.
    */
  } else if (parent->IsA() == R_RescueGeobody) {
    body = (RescueGeobody *)parent;
    bodyPart = body->bodyParts->ObjectIdentifiedBy(blockUnitID);
    timeSteps->Relink(this);
  } else if (parent->IsOfType(R_RescueSurface)) {
    surface = (RescueSurface *)parent;
    timeSteps->Relink(this);
  } else if (parent->IsA() == R_RescueModel) {
    model = (RescueModel *)parent;
    timeSteps->Relink(this);
  } else if (parent->IsA() == R_RescueDataContainer) {
    container = (RescueDataContainer *)parent;
    timeSteps->Relink(this);
  }
}

void RescuePropertyGroup::Archive(FILE *archiveFile) {
  myfprintf(context, archiveFile, "; Block Unit Property Group");
  myfprintf(context, archiveFile, Identifier());
  myfprintf(context, archiveFile, groupName->String());
  if (context->FileVersion() >= 21) {
    myfprintf(context, archiveFile, isLocked);
  }
  if (blockUnit != 0) {
    myfprintf(context, archiveFile, blockUnit->Identifier());
  } else if (bodyPart != 0) {
    myfprintf(context, archiveFile, bodyPart->Identifier());
  } else if (container != 0) {
    myfprintf(context, archiveFile, container->Identifier());
  } else {
    myfprintf(context, archiveFile, (RESCUEINT64)0);
  }

  if (context->FileVersion() == 9) {
    properties = new cSetRescueProperty();
    RESCUEINT64 timeStepOrdinal = 0;
    RescueTimeStepGroup *timeStep = timeSteps->NthObject(timeStepOrdinal++);
    while (timeStep != 0) {
      RESCUEINT64 propertyCount = timeStep->RescuePropertyCount64();
      RESCUEINT64 loop;
      for (loop = 0; loop < propertyCount; loop++) {
        RescueProperty *property = timeStep->NthRescueProperty(loop);
        (*properties) += property;
      }
      timeStep = timeSteps->NthObject(timeStepOrdinal++);
    }
    properties->Archive(context, archiveFile);
    properties->count = 0;
    /*
      Necessary to keep the properties from being deleted with the set.
    */
    delete properties;
    properties = 0;
  } else {
    timeSteps->Archive(context, archiveFile);
  }
  if (context->FileVersion() >= 37) {
    myfprintf(context, archiveFile, "EOD");
  }
}

void RescuePropertyGroup::SetLock() {
  isLocked = TRUE;
  RESCUEINT64 loop;
  for (loop = 0; loop < (*timeSteps).Count64(); loop++) {
    RescueTimeStepGroup *step = (RescueTimeStepGroup *)(*timeSteps).NthObject(loop);
    step->SetLock();
  }
}

RESCUEBOOL RescuePropertyGroup::IsOfType(_RescueObjectType thisType) {
  if (thisType == R_RescuePropertyGroup) {
    return TRUE;
  } else {
    return RescueHistoryObject::IsOfType(thisType);
  }
}
