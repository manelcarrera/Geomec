/*************************************************************************

    RescueTreeNode.h

 Node of an index in Rescue

    Rod Hanks               May, 1999

****************************************************************************/
#include "RescueTreeNode.h"
#include "RescueListNode.h"
#include "RescueTree.h"
#include "myHeaders.h"
#include <memory.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

RescueTreeNode::~RescueTreeNode() {
  if (objectCount > 0) {
    RESCUEINT64 loop;
    for (loop = 0; loop < objectCount; loop++) {
      delete objects[loop];
    }
  }
  free(objects);
  if (branches != 0) {
    RESCUEINT64 loop;
    for (loop = 0; loop < branchCount; loop++) {
      delete branches[loop];
    }
    free(branches);
  }
}

RescueTreeNode::RescueTreeNode(RescueTreeNode *parentNodeIn, RESCUEINT64 numberToAllocate)
    : objectCount(0), objectAllocated(numberToAllocate), objects(0), branchCount(0), branchAllocated(0), branches(0),
      parentNode(parentNodeIn) {
  objects = (Leaf **)malloc(sizeof(void *) * (size_t)objectAllocated);
}

RescueTreeNode::RescueTreeNode(RescueTreeNode *parentNodeIn, Leaf **objectList, RESCUEINT64 numberToAllocate,
                               RESCUEINT64 numberSent)
    : objectCount(numberSent), objectAllocated(numberToAllocate), objects(0), branchCount(0), branchAllocated(0),
      branches(0), parentNode(parentNodeIn) {
  objects = (Leaf **)malloc(sizeof(Leaf *) * (size_t)objectAllocated);
  size_t bytesToMove = sizeof(Leaf *) * (size_t)objectCount;
  memcpy(objects, objectList, bytesToMove);
}

RESCUEBOOL RescueTreeNode::Delete(void *objectToDelete, RescueTree *tree, RESCUEINT64 memo) {
  RESCUEBOOL myReturn = FALSE;
  RESCUEINT64 ndx;
  if (Find(objectToDelete, ndx)) {
    myReturn = TRUE;
    if (tree != 0) {
      objects[ndx]->listNode->Delete(objectToDelete, tree);
      delete objects[ndx];
    }
    if (branches != 0) {
      if (ndx >= branchCount) {
        Leaf *lowerKey = branches[branchCount - 1]->objects[branches[branchCount - 1]->objectCount - 1];
        objects[ndx] = lowerKey;
        branches[branchCount - 1]->Delete(lowerKey->object, 0, branchCount - 1);
      } else {
        Leaf *lowerKey = branches[ndx]->objects[0];
        objects[ndx] = lowerKey;
        branches[ndx]->Delete(lowerKey->object, 0, ndx);
      }
    } else if (objectCount == 1 && parentNode != 0) {
      parentNode->DeleteLeaf(memo);
      objectCount = 0;
      delete this;
    } else {
      RESCUEINT64 numberToMove = objectCount - (ndx + 1);
      size_t bytesToMove = (size_t)numberToMove * sizeof(void *);
      memmove(&objects[ndx], &objects[ndx + 1], bytesToMove);
      objectCount--;
    }
  } else if (branches != 0 && ndx > 0) {
    myReturn = branches[ndx - 1]->Delete(objectToDelete, tree, ndx - 1);
  }
  return myReturn;
}

void RescueTreeNode::DeleteLeaf(RESCUEINT64 memo) {
  Leaf *displacedKey = objects[memo];
  RESCUEINT64 numberToMove = objectCount - (memo + 1);
  size_t bytesToMove = (size_t)numberToMove * sizeof(void *);
  memmove(&objects[memo], &objects[memo + 1], bytesToMove);
  objectCount--;
  bytesToMove -= sizeof(void *);
  memmove(&branches[memo], &branches[memo + 1], bytesToMove);
  branchCount--;

  if (branchCount == 0) {
    free(branches);
    branchAllocated = 0;
    branches = 0;
  }
  Add(displacedKey, objectAllocated);
}

void RescueTreeNode::Add(Leaf *newObject, RESCUEINT64 numberToFill, RESCUEINT64 memo) {
  RESCUEINT64 ndx;
  if (objectCount == 0) {
    objects[0] = newObject;
    objectCount++;
  } else if (Find(newObject->object, ndx) == FALSE) {
    if (branches != 0) {
      if (ndx >= objectCount) {
        Leaf *displacedObject = objects[objectCount - 1];
        objects[objectCount - 1] = newObject;
        branches[branchCount - 1]->Add(displacedObject, numberToFill, branchCount - 1);
      } else if (ndx == 0) {
        Leaf *displacedObject = objects[0];
        objects[0] = newObject;
        branches[0]->Add(displacedObject, numberToFill, 0);
      }
      /*
        This object is smaller than any I have.  I will
        replace it as the new smallest one and move the
        old smallest one down.
      */
      else {
        branches[ndx - 1]->Add(newObject, numberToFill, ndx - 1);
      }
    }
    /*
      Above, handle being a branch node.
    */
    else {
      if (objectCount >= numberToFill) {
        RESCUEBOOL handled = FALSE;
        if (parentNode != 0) {
          if (ndx == 0) {
            handled = parentNode->LayOffToLeft(newObject, numberToFill, memo);
          } else {
            handled = parentNode->LayOffToLeft(objects[0], numberToFill, memo);
            if (handled == TRUE) {
              RESCUEINT64 numberToMove = ndx - 1;
              size_t bytesToMove = (size_t)numberToMove * sizeof(Leaf *);
              memmove(objects, &objects[1], bytesToMove);
              objects[ndx - 1] = newObject;
            }
          }
          if (handled == FALSE) {
            if (ndx >= objectCount) {
              handled = parentNode->LayOffToRight(newObject, numberToFill, memo);
            } else {
              handled = parentNode->LayOffToRight(objects[objectCount - 1], numberToFill, memo);
              if (handled == TRUE) {
                RESCUEINT64 numberToMove = objectCount - (ndx + 1);
                size_t bytesToMove = (size_t)numberToMove * sizeof(Leaf *);
                memmove(&objects[ndx + 1], &objects[ndx], bytesToMove);
                objects[ndx] = newObject;
              }
            }
          }
        }
        /*
        Try to balance the tree by laying off to the left or right before we split.
        */
        if (handled == FALSE) {
          if ((parentNode == 0)
                  ? TRUE
                  : (parentNode->objectCount >= objectAllocated || parentNode->objectCount >= numberToFill)) {
            RESCUEINT64 midNdx = objectCount / 2;
            branchAllocated = objectAllocated - 1;
            branches = (RescueTreeNode **)malloc(sizeof(RescueTreeNode *) * (size_t)branchAllocated);
            branches[0] = new RescueTreeNode(this, &objects[1], objectAllocated, midNdx - 1);
            objects[1] = objects[midNdx];
            branches[1] = new RescueTreeNode(this, &objects[midNdx + 1], objectAllocated, objectCount - (midNdx + 2));
            objects[2] = objects[objectCount - 1];
            objectCount = 3;
            branchCount = 2;
            Add(newObject, numberToFill);
          }
          /*
            Parent is full or we are root. We become a branch node.
          */
          else {
            RESCUEINT64 midNdx = objectCount / 2;
            Leaf *firstKey = objects[midNdx];
            Leaf **objectList = &objects[midNdx + 1];
            RESCUEINT64 objectsSent = objectCount - (midNdx + 1);
            RescueTreeNode *newNode = new RescueTreeNode(parentNode, objectList, objectAllocated, objectsSent);
            objectCount = midNdx;
            parentNode->ChildSplit(newNode, firstKey, memo);
            if (newObject->object < firstKey->object) {
              Add(newObject, numberToFill);
            } else {
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
      else {
        RESCUEINT64 numberToPush = objectCount - ndx;
        size_t sizeToMove = (size_t)numberToPush * sizeof(Leaf *);
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
  } else {
    delete newObject;
  }
  /*
    If the object already exists, then we delete the Leaf structure
    that was made to point to it.  We aren't going to index it
    anywhere.
  */
}

RESCUEBOOL RescueTreeNode::LayOffToLeft(Leaf *keyToMove, RESCUEINT64 numberToFill, RESCUEINT64 ndx) {
  RESCUEBOOL myReturn = FALSE;
  if (ndx > 0) {
    ndx--;
    if (branches[ndx]->objectCount < numberToFill && branches[ndx]->branches == 0) {
      Leaf *displacedKey = objects[ndx + 1];
      objects[ndx + 1] = keyToMove;
      branches[ndx]->Add(displacedKey, numberToFill, ndx);
      myReturn = TRUE;
    }
  }
  return myReturn;
}

RESCUEBOOL RescueTreeNode::LayOffToRight(Leaf *keyToMove, RESCUEINT64 numberToFill, RESCUEINT64 ndx) {
  RESCUEBOOL myReturn = FALSE;
  if (ndx < branchCount - 1) {
    ndx++;
    if (branches[ndx]->objectCount < numberToFill && branches[ndx]->branches == 0) {
      Leaf *displacedKey = objects[ndx];
      objects[ndx] = keyToMove;
      branches[ndx]->Add(displacedKey, numberToFill, ndx);
      myReturn = TRUE;
    }
  }
  return myReturn;
}

void RescueTreeNode::ChildSplit(RescueTreeNode *newChild, Leaf *firstKey, RESCUEINT64 ndx) {
  /*
    I know that I am not full, and I know that the firstKey lies between my
    first and last keys, and is not one of my keys.
  */
  ndx++;
  RESCUEINT64 numberToPush = objectCount - ndx;
  size_t sizeToMove = (size_t)numberToPush * sizeof(Leaf *);
  memmove(&objects[ndx + 1], &objects[ndx], sizeToMove);
  objects[ndx] = firstKey;
  objectCount++;
  sizeToMove -= sizeof(Leaf *);
  memmove(&branches[ndx + 1], &branches[ndx], sizeToMove);
  branches[ndx] = newChild;
  branchCount++;
}

RESCUEBOOL RescueTreeNode::Find(void *objectToFind, RESCUEINT64 &ndx) {
  RESCUEBOOL myReturn = FALSE;
  RESCUEINT64 upperLimit = objectCount;
  RESCUEINT64 lowerLimit = -1;
  RESCUEINT64 result = -1;
  ndx = 0;

  while ((upperLimit - lowerLimit) > 1 && result != 0) {
    ndx = (lowerLimit + upperLimit) >> 1;
    /*
        Calculate middle position.
    */
    if (objectToFind == objects[ndx]->object) {
      result = 0;
    } else if (objectToFind < objects[ndx]->object) {
      result = -1;
    } else {
      result = 1;
    }
    /*
        Result of comparision at this point.
    */
    if (result < 0) {
      upperLimit = ndx;
    } else if (result > 0) {
      lowerLimit = ndx;
    }
  }
  /*
      Binary search for the named entity.
  */
  if (result == 0) {
    myReturn = TRUE;
  }
  /*
      If we found it, return it.
  */
  else if (result > 0) {
    ndx++;
  }
  /*
    Position to insert it.
  */
  return myReturn;
}
