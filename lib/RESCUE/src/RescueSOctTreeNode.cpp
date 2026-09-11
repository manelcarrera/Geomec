/*************************************************************************

    RescueSOctTreeNode.cpp

 Node of a tree of RescueOctTreeNodes, used for triangulated surfaces.

    Rod Hanks               January, 2000

****************************************************************************/
#include "RescueSOctTreeNode.h"
#include "RescueBuffer.h"
#include "RescueModel.h"
#include "RescueTriangleVertex.h"
#include "RescueTriangulatedSurface.h"
#include "cBagRescueTriangleVertex.h"
#include "myHeaders.h"
#include <memory.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void RescueSOctTreeNode::CommonInitialization(RESCUEFLOAT minXin, RESCUEFLOAT maxXin, RESCUEFLOAT minYin,
                                              RESCUEFLOAT maxYin, RESCUEFLOAT minZin, RESCUEFLOAT maxZin) {
  minX = minXin;
  maxX = maxXin;
  minY = minYin;
  maxY = maxYin;
  minZ = minZin;
  maxZ = maxZin;
  RESCUEINT64 loop;
  for (loop = 0; loop < 8; loop++) {
    leaf[loop] = 0;
  }
  vertices = 0;
}

RescueSOctTreeNode::RescueSOctTreeNode(RescueTriangulatedSurface *parent, RESCUEFLOAT minXin, RESCUEFLOAT maxXin,
                                       RESCUEFLOAT minYin, RESCUEFLOAT maxYin, RESCUEFLOAT minZin, RESCUEFLOAT maxZin,
                                       RESCUEINT64 xDivisions, RESCUEINT64 yDivisions, RESCUEINT64 zDivisions)
    : vertices(0) {
  CommonInitialization(minXin, maxXin, minYin, maxYin, minZin, maxZin);
  vertices = new cBagRescueTriangleVertex();
  (*parent->octTrees) += this;
  RESCUEINT64 ndx = 0;
  RescueTriangleVertex *vertex = parent->NthVertex(ndx++);
  while (vertex != 0) {
    if (vertex->X() >= minX && vertex->X() <= maxX) {
      if (vertex->Y() >= minY && vertex->Y() <= maxY) {
        if (vertex->Z() >= minZ && vertex->Z() <= maxZ) {
          (*vertices) += vertex;
        }
      }
    }
    vertex = parent->NthVertex(ndx++);
  }
  Split(xDivisions, yDivisions, zDivisions);
}

void RescueSOctTreeNode::Split(RESCUEINT64 xDivisions, RESCUEINT64 yDivisions, RESCUEINT64 zDivisions) {
  if (xDivisions > 1 || yDivisions > 1 || zDivisions > 1) {
    RESCUEFLOAT midX = maxX;
    if (xDivisions > 1) {
      midX = (RESCUEFLOAT)((minX + maxX) / 2.0);
    }
    RESCUEFLOAT midY = maxY;
    if (yDivisions > 1) {
      midY = (RESCUEFLOAT)((minY + maxY) / 2.0);
    }
    RESCUEFLOAT midZ = maxZ;
    if (zDivisions > 1) {
      midZ = (RESCUEFLOAT)((minZ + maxZ) / 2.0);
    }
    xDivisions--;
    yDivisions--;
    zDivisions--;

    leaf[0] = LeafNode(this, minX, midX, minY, midY, minZ, midZ, xDivisions, yDivisions, zDivisions);
    if (midX < maxX) {
      leaf[1] = LeafNode(this, midX, maxX, minY, midY, minZ, midZ, xDivisions, yDivisions, zDivisions);
      if (midY < maxY) {
        leaf[2] = LeafNode(this, midX, maxX, midY, maxY, minZ, midZ, xDivisions, yDivisions, zDivisions);
      }
    }
    if (midY < maxY) {
      leaf[3] = LeafNode(this, minX, midX, midY, maxY, minZ, midZ, xDivisions, yDivisions, zDivisions);
    }
    if (midZ < maxZ) {
      leaf[4] = LeafNode(this, minX, midX, minY, midY, midZ, maxZ, xDivisions, yDivisions, zDivisions);
      if (midX < maxX) {
        leaf[5] = LeafNode(this, midX, maxX, minY, midY, midZ, maxZ, xDivisions, yDivisions, zDivisions);
        if (midY < maxY) {
          leaf[6] = LeafNode(this, midX, maxX, midY, maxY, midZ, maxZ, xDivisions, yDivisions, zDivisions);
        }
      }
      if (midY < maxY) {
        leaf[7] = LeafNode(this, minX, midX, midY, maxY, midZ, maxZ, xDivisions, yDivisions, zDivisions);
      }
    }
    delete vertices;
    vertices = 0;
  }
}

RescueSOctTreeNode *RescueSOctTreeNode::LeafNode(FILE *archiveFile,
                                                 RescueTriangulatedSurface *parentTriangulatedSurface) {
  RescueSOctTreeNode *myReturn = new RescueSOctTreeNode(archiveFile, parentTriangulatedSurface);
  return myReturn;
}

RescueSOctTreeNode *RescueSOctTreeNode::LeafNode(RescueSOctTreeNode *parent, RESCUEFLOAT minX, RESCUEFLOAT maxX,
                                                 RESCUEFLOAT minY, RESCUEFLOAT maxY, RESCUEFLOAT minZ, RESCUEFLOAT maxZ,
                                                 RESCUEINT64 xDivisions, RESCUEINT64 yDivisions,
                                                 RESCUEINT64 zDivisions) {
  RescueSOctTreeNode *myReturn =
      new RescueSOctTreeNode(this, minX, maxX, minY, maxY, minZ, maxZ, xDivisions, yDivisions, zDivisions);
  return myReturn;
}

RescueSOctTreeNode::RescueSOctTreeNode(RescueSOctTreeNode *parent, RESCUEFLOAT minXin, RESCUEFLOAT maxXin,
                                       RESCUEFLOAT minYin, RESCUEFLOAT maxYin, RESCUEFLOAT minZin, RESCUEFLOAT maxZin,
                                       RESCUEINT64 xDivisions, RESCUEINT64 yDivisions, RESCUEINT64 zDivisions)
    : vertices(0) {
  CommonInitialization(minXin, maxXin, minYin, maxYin, minZin, maxZin);
  vertices = new cBagRescueTriangleVertex();
  RESCUEINT64 ndx = 0;
  RescueTriangleVertex *vertex = parent->NthVertex(ndx++);
  while (vertex != 0) {
    if (vertex->X() >= minX && vertex->X() <= maxX) {
      if (vertex->Y() >= minY && vertex->Y() <= maxY) {
        if (vertex->Z() >= minZ && vertex->Z() <= maxZ) {
          (*vertices) += vertex;
        }
      }
    }
    vertex = parent->NthVertex(ndx++);
  }
  Split(xDivisions, yDivisions, zDivisions);
}

RescueSOctTreeNode::RescueSOctTreeNode(FILE *archiveFile, RescueTriangulatedSurface *parentTriangulatedSurface) {
  RescueContext *context = parentTriangulatedSurface->ParentSurface()->ParentModel()->Context();
  myfscanf(context, archiveFile, &minX);
  myfscanf(context, archiveFile, &maxX);
  myfscanf(context, archiveFile, &minY);
  myfscanf(context, archiveFile, &maxY);
  myfscanf(context, archiveFile, &minZ);
  myfscanf(context, archiveFile, &maxZ);
  CommonInitialization(minX, maxX, minY, maxY, minZ, maxZ);
  RESCUEINT64 flag;
  myfscanf(context, archiveFile, &flag);
  if (flag == 1) {
    vertices = new cBagRescueTriangleVertex();
    vertices->UnArchive(context, archiveFile, parentTriangulatedSurface);
  }
  RESCUEINT64 loop;
  for (loop = 0; loop < 8; loop++) {
    myfscanf(context, archiveFile, &flag);
    if (flag == 1) {
      leaf[loop] = LeafNode(archiveFile, parentTriangulatedSurface);
    }
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

RESCUEBOOL RescueSOctTreeNode::TerminalNode() {
  RESCUEBOOL myReturn = TRUE;
  RESCUEINT64 loop;
  for (loop = 0; loop < 8 && myReturn == TRUE; loop++) {
    if (leaf[loop] != 0) {
      myReturn = FALSE;
    }
  }
  return myReturn;
}

void RescueSOctTreeNode::Archive(RescueContext *context, FILE *archiveFile) {
  myfprintf(context, archiveFile, minX);
  myfprintf(context, archiveFile, maxX);
  myfprintf(context, archiveFile, minY);
  myfprintf(context, archiveFile, maxY);
  myfprintf(context, archiveFile, minZ);
  myfprintf(context, archiveFile, maxZ);
  if (vertices == 0) {
    myfprintf(context, archiveFile, (RESCUEINT64)0);
  } else {
    myfprintf(context, archiveFile, (RESCUEINT64)1);
    vertices->Archive(context, archiveFile);
  }
  /*
    The RescueTriangleVertex objects are identified by ndx not id.
    Therefore, they can only be archived AFTER the cSetRescueTriangleVertex
    which owns them has been archived.
  */
  RESCUEINT64 loop;
  for (loop = 0; loop < 8; loop++) {
    if (leaf[loop] == 0) {
      myfprintf(context, archiveFile, (RESCUEINT64)0);
    } else {
      myfprintf(context, archiveFile, (RESCUEINT64)1);
      leaf[loop]->Archive(context, archiveFile);
    }
  }
  if (context->FileVersion() >= 37) {
    myfprintf(context, archiveFile, "EOD");
  }
}

RescueSOctTreeNode::~RescueSOctTreeNode() {
  if (vertices != 0) {
    delete vertices;
  }
  RESCUEINT64 loop;
  for (loop = 0; loop < 8; loop++) {
    if (leaf[loop] != 0) {
      delete leaf[loop];
    }
  }
}
