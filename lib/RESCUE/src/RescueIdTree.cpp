/*************************************************************************

        RescueIdTree.h

 Head of a tree of RescueIdTreeNodes.

        Rod Hanks               May, 1999

****************************************************************************/
#include "myHeaders.h"
#define NUMBER_TO_ALLOCATE 20
#define NUMBER_TO_FILL 15
#include "RescueIdTree.h"
#include "RescueIdTreeNode.h"
#if 0
#include <stdio.h>
#endif

RescueIdTree::RescueIdTree()
{
  count = 0;
  rootNode = new RescueIdTreeNode(0, NUMBER_TO_ALLOCATE);
}

RescueIdTree::~RescueIdTree()
{
  delete rootNode;
}

void RescueIdTree::Add(RescueObject *newObject)
{
  rootNode->Add(newObject, NUMBER_TO_ALLOCATE);
  count++;
}

RescueObject *RescueIdTree::Find(RESCUEINT64 identifier)
{
  return rootNode->Find(identifier);
}

RESCUEBOOL RescueIdTree::Delete(RescueObject *existingObject)
{
  RESCUEBOOL found = FALSE;
  if (rootNode->Delete(existingObject))
  {
    count--;
    found = TRUE;
  }
  return found;
}



