/*************************************************************************

    RescueListNode.h

 Node of a list in Rescue

    Rod Hanks               May, 1999

****************************************************************************/
#include "myHeaders.h"
#include "RescueListNode.h"
#include "RescueTree.h"
#include <stdlib.h>
#include <memory.h>
#include <string.h>
#include <stdio.h>

#define NODE_SIZE 20

RescueListNode::RescueListNode(RescueListNode *leftNodeIn)
{
  objectCount = 0;
  objects = (void **) malloc(sizeof(void *) * (size_t) NODE_SIZE);
  leftNode = leftNodeIn;
  rightNode = 0;
}

RescueListNode *RescueListNode::Add(void *object)
{
  RescueListNode *myReturn = this;
  if (objectCount < NODE_SIZE)
  {
  objects[objectCount++] = object;
  }
  else
  {
  rightNode = new RescueListNode(this);
  rightNode->Add(object);
  myReturn = rightNode;
  }
  return myReturn;
}

void RescueListNode::Delete(void *object, RescueTree *tree)
{
  RESCUEINT64 ndx = Find(object);
  if (ndx >= 0)
  {
  if (objectCount > 1)
  {
      RESCUEINT64 numberToMove = objectCount - (ndx + 1);
      size_t bytesToMove = (size_t) numberToMove * sizeof(void *);
      memmove(&objects[ndx], &objects[ndx + 1], bytesToMove);
      objectCount--;
  }
  else if (leftNode != 0)
  {
      leftNode->rightNode = rightNode;
      if (rightNode != 0)
      {
    rightNode->leftNode = leftNode;
      }
      else
      {
    tree->listFoot = leftNode;
      }
      delete this;
  }
  else
  {
      if (rightNode != 0)
      {
    tree->listHead = rightNode;
    rightNode->leftNode = 0;
    delete this;
      }
      else
      {
    objectCount = 0;
      }
  }
  }
}

RescueListNode::~RescueListNode()
{
  free(objects);
}

void RescueListNode::Destroy()
{
  if (rightNode != 0)
  {
  rightNode->Destroy();
  }
  delete this;
}

RESCUEINT64 RescueListNode::Find(void *objectToFind)
{
  RESCUEINT64 myReturn = -1;
  RESCUEINT64 loop;
  for (loop = 0; loop < objectCount && myReturn == -1; loop++)
  {
  if (objects[loop] == objectToFind)
  {
      myReturn = loop;
  }
  }
  return myReturn;
}


