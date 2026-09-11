/********************************************************************

  RescueModelPropertyGroup.cpp

  A grouping of RescuePropertyGroups for some application
  purpose.  The grouping may cross RescueBlockUnit boundaries.

  Rod Hanks,  June 1998

*********************************************************************/
#include "RescueModelPropertyGroup.h"
#include "RescuePropertyGroup.h"
#include "RescueModel.h"
#include "RescueIdTree.h"
#include "RescueHistory.h"
#include "RescueBuffer.h"
#include <string.h>

RescueModelPropertyGroup::RescueModelPropertyGroup(RESCUECHAR *groupNameIn,
                                                   RescueModel *modelIn)
                                                  :RescueHistoryObject(modelIn->Context())
                                                  ,parentModel(modelIn)
                                                  ,groupName(0)
                                                  ,groups(0)
                                                  ,isLocked(FALSE)
                                                  ,propertyGroupsID(0)
{
  groupName = new RCHString(groupNameIn);
  isA = R_RescueModelPropertyGroup;
  (*parentModel->propertyGroup) += this;
  groups = new cBagRescuePropertyGroup();
}

RescueModelPropertyGroup::~RescueModelPropertyGroup()
{
  if (groupName != 0)
  {
    delete groupName;
  }
  if (groups != 0)
  {
    delete groups;
  }
}

void RescueModelPropertyGroup::SetLock()
{
  isLocked = TRUE;
  RESCUEINT64 loop;
  for (loop = 0; loop < (*groups).Count64(); loop++)
  {
    RescuePropertyGroup *group = (RescuePropertyGroup *) 
                                                    (*groups).NthObject(loop);
    group->SetLock();
  }
}

void RescueModelPropertyGroup::Archive(FILE *archiveFile)
{
  RescueContext *context = parentModel->Context();
  context->RescueProgress((*groupName).String());
  myfprintf(context, archiveFile, "; RescueModelPropertyGroup");
  myfprintf(context, archiveFile, Identifier());
  myfprintf(context, archiveFile, (*groupName).String());

  if (context->FileVersion() >= 21)
  {
    myfprintf(context, archiveFile, isLocked);
  }

  if (context->FileVersion() == 19)
  {
    RESCUEINT64 howMany = (*groups).Count64();
    RESCUEINT64 count = 0;
    RESCUEINT64 loop;
    for (loop = 0; loop < howMany; loop++)
    {
      RescuePropertyGroup *group = (RescuePropertyGroup *) 
                                                    (*groups).NthObject(loop);
      if (group->ParentBlockUnit() != 0)
      {
        count++;
      }
    }
    myfprintf(context, archiveFile, count);
    for (loop = 0; loop < howMany; loop++)
    {
      RescuePropertyGroup *group = (RescuePropertyGroup *) 
                                                    (*groups).NthObject(loop);
      if (group->ParentBlockUnit() != 0)
      {
        myfprintf(context, archiveFile, group->Identifier());
      }
    }
  }
  else
  {
    myfprintf(context, archiveFile, (*groups).Count64());
    RESCUEINT64 loop;
    for (loop = 0; loop < (*groups).Count64(); loop++)
    {
      RescuePropertyGroup *group = (RescuePropertyGroup *) 
                              (*groups).NthObject(loop);
      myfprintf(context, archiveFile, group->Identifier());
    }
  }
  if (context->FileVersion() >= 37)
  {
    myfprintf(context, archiveFile, "EOD");
  }
}

RescueModelPropertyGroup::RescueModelPropertyGroup(RescueContext *context, FILE *archiveFile)
                                                  :RescueHistoryObject(context)
                                                  ,parentModel(0)
                                                  ,groupName(0)
                                                  ,groups(0)
                                                  ,isLocked(FALSE)
                                                  ,propertyGroupsID(0)
{
  RESCUECHAR myString[255];
  RESCUEINT64 count;

  groups = new cBagRescuePropertyGroup();
  isA = R_RescueModelPropertyGroup;
  ReadId(context, archiveFile);
  myfgets(context, myString, 255, archiveFile);
  groupName = new RCHString(myString);
  context->RescueProgress(myString);

  if (context->ReadFileVersion() >= 21)
  {
    myfscanf(context, archiveFile, &isLocked);
  }

  myfscanf(context, archiveFile, &count);
  if (count > 0)
  {
    propertyGroupsID = new cBagInt();

    RESCUEINT64 loop;
    for (loop = 0; loop < count; loop++)
    {
      RESCUEINT64 id;

      myfscanf(context, archiveFile, &id);
      (*propertyGroupsID) += id;
    }
  }
  context->modelPropertyGroups->Add(this);
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

void RescueModelPropertyGroup::Relink(RescueObject *parent)
{
// pass RescueModel.
  parentModel = (RescueModel *) parent;
  if (propertyGroupsID != 0)
  {
    RESCUEINT64 loop;
    for (loop = 0; loop < propertyGroupsID->Count64(); loop++)
    {
      RESCUEINT64 groupID = propertyGroupsID->NthObject(loop);
      RescuePropertyGroup *group = 
        (RescuePropertyGroup *) RescueHistory::FindObject(parentModel, 
                                                                    R_RescuePropertyGroup, 
                                                                     groupID);
      if (group != 0) 
      {
        (*groups) += group;
      }
    }
    delete propertyGroupsID;
    propertyGroupsID = 0;
  }
}

RESCUEBOOL RescueModelPropertyGroup::IsOfType(_RescueObjectType thisType)
{
  if (thisType == R_RescueModelPropertyGroup)
  {
    return TRUE;
  }
  else
  {
    return RescueHistoryObject::IsOfType(thisType);
  }
}



