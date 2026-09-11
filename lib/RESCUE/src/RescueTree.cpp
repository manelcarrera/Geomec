/*************************************************************************

    RescueTree.h

 Head of a tree of RescueTreeNodes.

    Rod Hanks               May, 1999

****************************************************************************/
#include "myHeaders.h"
#define NUMBER_TO_ALLOCATE 20
#define NUMBER_TO_FILL 15
#include "RescueTree.h"
#include "RescueTreeNode.h"
#include "RescueListNode.h"
#if 0
#include <stdio.h>
#endif

RescueTree::RescueTree()
{
  count = 0;
  rootNode = new RescueTreeNode(0, NUMBER_TO_ALLOCATE);
  listHead = new RescueListNode();
  listFoot = listHead;
  traversalNode = 0;
  traversalNdx = 0;
  traversalOrdinal = -99;
  traversal2Node = 0;
  traversal2Ndx = 0;
  traversal2Ordinal = -99;
}

RescueTree::~RescueTree()
{
  delete rootNode;
  listHead->Destroy();
}

void RescueTree::Add(void *newObject)
{
  listFoot = listFoot->Add(newObject);
  Leaf* leaf = new Leaf();
  leaf->listNode = listFoot;
  leaf->object = newObject;
  rootNode->Add(leaf, NUMBER_TO_ALLOCATE);
  count++;
  traversalNode = 0;
  traversalNdx = 0;
  traversalOrdinal = -99;
  traversal2Node = 0;
  traversal2Ndx = 0;
  traversal2Ordinal = -99;
}

RESCUEBOOL RescueTree::Delete(void * existingObject)
{
  RESCUEBOOL found = FALSE;
  if (rootNode->Delete(existingObject, this))
  {
  count--;
  found = TRUE;
  traversalNode = 0;
  traversalNdx = 0;
  traversalOrdinal = -99;
  traversal2Node = 0;
  traversal2Ndx = 0;
  traversal2Ordinal = -99;
  }
  return found;
}

void *RescueTree::NthObject(RESCUEINT64 ordinal)
{
  if (ordinal < 0 || ordinal >= count)
  {
  return 0;
  }
  else
  {
  if (ordinal == 0)
  {
      traversalNode = listHead;
      traversalNdx = 0;
  }
  else if (ordinal == traversalOrdinal)
  {
  }
  else if (ordinal == count - 1)
  {
      traversalNode = listFoot;
      traversalNdx = listFoot->objectCount - 1;
  }
  else if (ordinal == traversalOrdinal + 1)
  {
      if (traversalNdx >= traversalNode->objectCount - 1)
      {
    traversalNode = traversalNode->rightNode;
    traversalNdx = 0;
      }
      else
      {
    traversalNdx++;
      }
  }
  else if (ordinal == traversalOrdinal - 1)
  {
      if (traversalNdx == 0)
      {
    traversalNode = traversalNode->leftNode;
    traversalNdx = traversalNode->objectCount - 1;
      }
      else
      {
    traversalNdx--;
      }
  }
  else
  {
      RESCUEINT64 loop = 0;
      for (loop = 0; loop <= ordinal; loop++)
      {
    NthObject(loop);
      }
  }
  traversalOrdinal = ordinal;
  return traversalNode->objects[traversalNdx];
  }
}

void *RescueTree::TreeTraversal(RESCUEINT64 ordinal)
{
  if (ordinal < 0 || ordinal >= count)
  {
  return 0;
  }
  else
  {
  if (ordinal == 0)
  {
      traversal2Node = rootNode;
      traversal2Ndx = 0;
  }
  else if (ordinal == count - 1)
  {
      traversal2Node = rootNode;
      traversal2Ndx = rootNode->objectCount - 1;
  }
  else if (ordinal == traversal2Ordinal + 1)
  {
      if (traversal2Node->branches != 0)
      {
    if (traversal2Ndx >= traversal2Node->branchCount)
    {
          Leaf *lastKey = traversal2Node->objects[traversal2Node->objectCount - 1];
          traversal2Node = traversal2Node->parentNode;
          traversal2Node->Find(lastKey->object, traversal2Ndx);
    }
    else
    {
          traversal2Node = traversal2Node->branches[traversal2Ndx];
          traversal2Ndx = 0;
    }
      }
      else
      {
    traversal2Ndx++;
    if (traversal2Ndx >= traversal2Node->objectCount)
    {
          Leaf *lastKey = traversal2Node->objects[traversal2Node->objectCount - 1];
          traversal2Node = traversal2Node->parentNode;
          traversal2Node->Find(lastKey->object, traversal2Ndx);
    }
      }
  }
  else if (ordinal == traversal2Ordinal - 1)
  {
      if (traversal2Node->branches != 0)
      {
    traversal2Ndx--;
    if (traversal2Ndx >= 0)
    {
          traversal2Node = traversal2Node->branches[traversal2Ndx];
          traversal2Ndx = traversal2Node->objectCount - 1;
    }
    else
    {
          Leaf *lastKey = traversal2Node->objects[0];
          traversal2Node = traversal2Node->parentNode;
          traversal2Node->Find(lastKey->object, traversal2Ndx);
    }
      }
      else
      {
    traversal2Ndx--;
    if (traversal2Ndx < 0)
    {
          Leaf *lastKey = traversal2Node->objects[0];
          traversal2Node = traversal2Node->parentNode;
          traversal2Node->Find(lastKey->object, traversal2Ndx);
          traversal2Ndx--;
    }
      }
  }
  else
  {
      RESCUEINT64 loop = 0;
      for (loop = 0; loop < ordinal; loop++)
      {
    NthObject(loop);
      }
  }
  traversal2Ordinal = ordinal;
  return traversal2Node->objects[traversal2Ndx]->object;
  }
}





