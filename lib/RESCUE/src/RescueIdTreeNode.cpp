/*************************************************************************

        RescueIdTreeNode.h

 Node of an index in Rescue

        Rod Hanks               May, 1999

****************************************************************************/
#include "myHeaders.h"
#include "RescueIdTreeNode.h"
#include "RescueObject.h"
#include <stdlib.h>
#include <memory.h>
#include <string.h>
#include <stdio.h>

RescueIdTreeNode::~RescueIdTreeNode()
{
  free(objects);
  if (branches != 0)
  {
    RESCUEINT64 loop;
    for (loop = 0; loop < branchCount; loop++)
    {
      delete branches[loop];
    }
    free(branches);
  }
}

RescueIdTreeNode::RescueIdTreeNode(RescueIdTreeNode *parentNodeIn,
                               RESCUEINT64 numberToAllocate)
                              :objectCount(0)
          ,objectAllocated(numberToAllocate)
          ,objects(0)
          ,branchCount(0)
          ,branchAllocated(0)
          ,branches(0)
          ,parentNode(parentNodeIn)
{
  objects = (RescueObject **) malloc(sizeof(RescueObject *) * (size_t) objectAllocated);
}

RescueIdTreeNode::RescueIdTreeNode(RescueIdTreeNode *parentNodeIn, RescueObject **objectList, 
                               RESCUEINT64 numberToAllocate, RESCUEINT64 numberSent)
                               :objectCount(numberSent)
          ,objectAllocated(numberToAllocate)
          ,objects(0)
          ,branchCount(0)
          ,branchAllocated(0)
          ,branches(0)
          ,parentNode(parentNodeIn)
{
  objects = (RescueObject **) malloc(sizeof(RescueObject *) * (size_t) objectAllocated);
  size_t bytesToMove = sizeof(RescueObject *) * (size_t) objectCount;
  memcpy(objects, objectList, bytesToMove);
}

RescueObject *RescueIdTreeNode::Find(RESCUEINT64 identifier)
{
  RescueObject *myReturn = 0;
  RESCUEINT64 ndx;
  RESCUEINT64 upperLimit = objectCount;
  RESCUEINT64 lowerLimit = -1;
  RESCUEINT64 result = -1;
  ndx = 0;
 
  while ((upperLimit - lowerLimit) > 1 && result != 0)
  {
    ndx = (lowerLimit + upperLimit) >> 1;
/*
        Calculate middle position.
*/
    if (identifier == objects[ndx]->Identifier())
    {
      result = 0;
    }
    else if (identifier < objects[ndx]->Identifier())
    {
      result = -1;
    }
    else
    {
      result = 1;
    }
/*
        Result of comparision at this point.
*/
    if (result < 0)
    {
      upperLimit = ndx;
    }
    else if (result > 0)
    {
      lowerLimit = ndx;
    }
  }
/*
        Binary search for the named entity.
*/
  if (result > 0)
  {
    ndx++;
  }
/*
  Position to insert it.
*/
  if (result == 0)
  {
    myReturn = objects[ndx];
  }
  else if (branches != 0 && ndx > 0)
  {
    myReturn = branches[ndx - 1]->Find(identifier);
  }
  return myReturn;
}

void RescueIdTreeNode::Add(RescueObject *newObject, RESCUEINT64 numberToFill, RESCUEINT64 memo)
{
  RESCUEINT64 ndx;
  if (objectCount == 0)
  {
    objects[0] = newObject;
    objectCount++;
  }
  else if (Find(newObject, ndx) == FALSE)
  {
    if (branches != 0)
    {
      if (ndx >= objectCount)
      {
        RescueObject *displacedObject = objects[objectCount - 1];
        objects[objectCount - 1] = newObject;
        branches[branchCount - 1]->Add(displacedObject, numberToFill, branchCount - 1);
      }
      else if (ndx == 0)
      {
        RescueObject *displacedObject = objects[0];
        objects[0] = newObject;
        branches[0]->Add(displacedObject, numberToFill, 0);
      }
/*
  This object is smaller than any I have.  I will
  replace it as the new smallest one and move the
  old smallest one down.
*/
      else
      {
        branches[ndx - 1]->Add(newObject, numberToFill, ndx - 1);
      }
    }
/*
  Above, handle being a branch node.
*/
    else
    {
      if (objectCount >= numberToFill)
      {
        RESCUEBOOL handled = FALSE;
        if (parentNode != 0)
        {
          if (ndx == 0)
          {
            handled = parentNode->LayOffToLeft(newObject, numberToFill, memo);
          }
          else
          {
            handled = parentNode->LayOffToLeft(objects[0], numberToFill, memo);
            if (handled == TRUE)
            {
              RESCUEINT64 numberToMove = ndx - 1;
              size_t bytesToMove = (size_t) numberToMove * sizeof(RescueObject *);
              memmove(objects, &objects[1], bytesToMove);
              objects[ndx - 1] = newObject;
            }
          }
          if (handled == FALSE)
          {
            if (ndx >= objectCount)
            {
              handled = parentNode->LayOffToRight(newObject, numberToFill, memo);
            }
            else
            {
              handled = parentNode->LayOffToRight(objects[objectCount - 1], numberToFill, memo);
              if (handled == TRUE)
              {
                RESCUEINT64 numberToMove = objectCount - (ndx + 1);
                size_t bytesToMove = (size_t) numberToMove * sizeof(RescueObject *);
                memmove(&objects[ndx + 1], &objects[ndx], bytesToMove);
                objects[ndx] = newObject;
              }
            }
          }
        }
  /*
    Try to balance the tree by laying off to the left or right before we split.
  */
        if (handled == FALSE)
        {
          if ((parentNode == 0) ? TRUE : (parentNode->objectCount >= objectAllocated
                                      ||  parentNode->objectCount >= numberToFill))
          {
            RESCUEINT64 midNdx = objectCount / 2;
            branchAllocated = objectAllocated - 1;
            branches = (RescueIdTreeNode **) malloc(sizeof(RescueIdTreeNode *) * (size_t) branchAllocated);
            branches[0] = new RescueIdTreeNode(this, &objects[1], objectAllocated, midNdx - 1);
            objects[1] = objects[midNdx];
            branches[1] = new RescueIdTreeNode(this, &objects[midNdx + 1], objectAllocated, 
                                             objectCount - (midNdx + 2));
            objects[2] = objects[objectCount - 1];
            objectCount = 3;
            branchCount = 2;
            Add(newObject, numberToFill);
          }
/*
  Parent is full or we are root. We become a branch node.
*/
          else
          {
            RESCUEINT64 midNdx = objectCount / 2;
            RescueObject *firstKey = objects[midNdx];
            RescueObject **objectList = &objects[midNdx + 1];
            RESCUEINT64 objectsSent = objectCount - (midNdx + 1);
            RescueIdTreeNode *newNode = new RescueIdTreeNode(parentNode, objectList, 
                                                         objectAllocated, objectsSent);
            objectCount = midNdx;
            parentNode->ChildSplit(newNode, firstKey, memo);
            if (newObject->Identifier() < firstKey->Identifier())
            {
              Add(newObject, numberToFill);
            }
            else
            {
              newNode->Add(newObject, numberToFill);
            }
          }
        }
/*
  Parent has room.
*/
      }
/*
  Have to split.
*/
      else
      {
        RESCUEINT64 numberToPush = objectCount - ndx;
        size_t sizeToMove = (size_t) numberToPush * sizeof(RescueObject *);
        memmove(&objects[ndx + 1], &objects[ndx], sizeToMove);
        objects[ndx] = newObject;
        objectCount++;
      }
/*
  Simple insert.
*/
    }
/*
  Above, we are a leaf node.
*/
  }
}

RESCUEBOOL RescueIdTreeNode::LayOffToLeft(RescueObject *keyToMove, RESCUEINT64 numberToFill, RESCUEINT64 ndx)
{
  RESCUEBOOL myReturn = FALSE;
  if (ndx > 0)
  {
    ndx--;
    if (branches[ndx]->objectCount < numberToFill
    &&  branches[ndx]->branches == 0)
    {
      RescueObject *displacedKey = objects[ndx + 1];
      objects[ndx + 1] = keyToMove;
      branches[ndx]->Add(displacedKey, numberToFill, ndx);
      myReturn = TRUE;
    }
  }
  return myReturn;
}

RESCUEBOOL RescueIdTreeNode::LayOffToRight(RescueObject *keyToMove, RESCUEINT64 numberToFill, RESCUEINT64 ndx)
{
  RESCUEBOOL myReturn = FALSE;
  if (ndx < branchCount - 1)
  {
    ndx++;
    if (branches[ndx]->objectCount < numberToFill
    &&  branches[ndx]->branches == 0)
    {
      RescueObject *displacedKey = objects[ndx];
      objects[ndx] = keyToMove;
      branches[ndx]->Add(displacedKey, numberToFill, ndx);
      myReturn = TRUE;
    }
  }
  return myReturn;
}

void RescueIdTreeNode::ChildSplit(RescueIdTreeNode *newChild, RescueObject *firstKey, RESCUEINT64 ndx)
{
/*
  I know that I am not full, and I know that the firstKey lies between my
  first and last keys, and is not one of my keys.
*/
  ndx++;
  RESCUEINT64 numberToPush = objectCount - ndx;
  size_t sizeToMove = (size_t) numberToPush * sizeof(RescueObject *);
  memmove(&objects[ndx + 1], &objects[ndx], sizeToMove);
  objects[ndx] = firstKey;
  objectCount++;
  sizeToMove -= sizeof(RescueObject *);
  memmove(&branches[ndx + 1], &branches[ndx], sizeToMove);
  branches[ndx] = newChild;
  branchCount++;
}

RESCUEBOOL RescueIdTreeNode::Find(RescueObject *objectToFind, RESCUEINT64 &ndx)
{
  RESCUEBOOL myReturn = FALSE;
  RESCUEINT64 upperLimit = objectCount;
  RESCUEINT64 lowerLimit = -1;
  RESCUEINT64 result = -1;
  ndx = 0;
 
  while ((upperLimit - lowerLimit) > 1 && result != 0)
  {
    ndx = (lowerLimit + upperLimit) >> 1;
/*
        Calculate middle position.
*/
    if (objectToFind->Identifier() == objects[ndx]->Identifier())
    {
      result = 0;
    }
    else if (objectToFind->Identifier() < objects[ndx]->Identifier())
    {
      result = -1;
    }
    else
    {
      result = 1;
    }
/*
        Result of comparision at this point.
*/
    if (result < 0)
    {
      upperLimit = ndx;
    }
    else if (result > 0)
    {
      lowerLimit = ndx;
    }
  }
/*
        Binary search for the named entity.
*/
  if (result == 0)
  {
    myReturn = TRUE;
  }
/*
        If we found it, return it.
*/
  else if (result > 0)
  {
    ndx++;
  }
/*
  Position to insert it.
*/
  return myReturn;
}

void RescueIdTreeNode::DeleteLeaf(RESCUEINT64 memo)
{
  RescueObject *displacedKey = objects[memo];
  RESCUEINT64 numberToMove = objectCount - (memo + 1);
  size_t bytesToMove = (size_t) numberToMove * sizeof(RescueObject *);
  memmove(&objects[memo], &objects[memo + 1], bytesToMove);
  objectCount--;
  bytesToMove -= sizeof(RescueObject *);
  memmove(&branches[memo], &branches[memo + 1], bytesToMove);
  branchCount--;

  if (branchCount == 0)
  {
    free(branches);
    branchAllocated = 0;
    branches = 0;
  }
  Add(displacedKey, objectAllocated);
}

RESCUEBOOL RescueIdTreeNode::Delete(RescueObject *objectToDelete, RESCUEINT64 memo)
{
  RESCUEBOOL myReturn = FALSE;
  RESCUEINT64 ndx;
  if (Find(objectToDelete, ndx))
  {
    myReturn = TRUE;
    if (branches != 0)
    {
      if (ndx >= branchCount)
      {
        RescueObject *lowerKey = branches[branchCount - 1]->objects[branches[branchCount - 1]->objectCount - 1];
        objects[ndx] = lowerKey;
        branches[branchCount - 1]->Delete(lowerKey, branchCount - 1);
      }
      else
      {
        RescueObject *lowerKey = branches[ndx]->objects[0];
        objects[ndx] = lowerKey;
        branches[ndx]->Delete(lowerKey, ndx);
      }
    }
    else if (objectCount == 1 && parentNode != 0)
    {
      parentNode->DeleteLeaf(memo);
      objectCount = 0;
      delete this;
    }
    else
    {
      RESCUEINT64 numberToMove = objectCount - (ndx + 1);
      size_t bytesToMove = (size_t) numberToMove * sizeof(void *);
      memmove(&objects[ndx], &objects[ndx + 1], bytesToMove);
      objectCount--;
    }
  }
  else if (branches != 0 && ndx > 0)
  {
    myReturn = branches[ndx - 1]->Delete(objectToDelete, ndx - 1);
  }
  return myReturn;
}




