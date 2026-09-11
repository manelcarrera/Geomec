/*                                         Copyright (c) 2011 TNO DIANA BV */
#include "tet_surface.h"
#include "Set.h"
#include "tet_edge.h"
#include "tet_graph.h"
#include "tet_mesh.h"
#include "tet_surfaceset.h"
#include "tet_triangle.h"
#include "tet_utils.h"
#include <ctype.h>

extern Surface_t *SurfaceCreate(int surfaceID, int part, SurfaceType_t surfaceType, FrontDir_t frontDir,
                                const char *userName, SurfOuterBoundary_t outerBoundary) {
  Surface_t *surface = DIMALLOC(sizeof(*surface));
  const int len = (int)strlen(userName);
  UTIL_ZERO(surface);
  surface->triangle = SetCreate(0, sizeof(Triangle_t *));
  surface->surfaceID = surfaceID;
  surface->part = part;
  surface->surfaceType = surfaceType;
  surface->frontDir = frontDir;
  surface->userName = DIMALLOC(len + 1);
  strcpy(surface->userName, userName);
  surface->outerBoundarY = outerBoundary;
  return surface;
}

extern Surface_t *SurfaceCopy(Surface_t *surface) {
  Surface_t *result =
      SurfaceCreate(SurfaceID(surface), SurfacePartGet(surface), SurfaceType(surface), SurfaceGetFrontDir(surface),
                    SurfaceUserName(surface), SurfaceOuterBoundaryGet(surface));
  SurfaceAddSurface(result, surface);
  return result;
}

extern Surface_t *SurfaceDelete(Surface_t *surface) {
  if (surface) {
    Iterator_t iter;
    Triangle_t *triangle = SurfaceFirst(surface, NULL, &iter);
    while (triangle) {
      TriangleDelete(triangle);
      triangle = SurfaceNext(surface, NULL, &iter);
    }
    assert(!SurfaceGetOpenFault(surface)); /* Temporary data, should have been cleaned */
    SetDelete(surface->triangle);
    EdgeDelete(surface->triangleConnectivity);
    DIFREE(surface->userName);
    DIFREE(surface);
  }
  return surface;
}

extern Surface_t *SurfacePurge(Surface_t *surface) {
  if (surface) {
    surface->triangleConnectivity = EdgeDelete(surface->triangleConnectivity);
  }
  return surface;
}

/* Return id, -1 if invalid */
extern int SurfaceAdd(Surface_t *surface, Triangle_t *triangle) {
  int triangleId = -1;
  if (triangle) {
    assert(surface != NULL);
    triangleId = SetSize(surface->triangle);
    SetAdd(surface->triangle, &triangle, 1);
    /* Existing connectivity info no longer up-to-date */
    surface->triangleConnectivity = EdgeDelete(surface->triangleConnectivity);
  }
  return triangleId;
}

extern void SurfaceAddSurface(Surface_t *surfaceTo, Surface_t *surfaceFrom) {
  Iterator_t iter;
  Triangle_t *triangle = SurfaceFirst(surfaceFrom, NULL, &iter);
  while (triangle) {
    SurfaceAdd(surfaceTo, TriangleCopy(triangle));
    triangle = SurfaceNext(surfaceFrom, NULL, &iter);
  }
}

extern void *SurfaceTriangleConnectivity(Surface_t *surface) {
  if (!surface->triangleConnectivity) {
    surface->triangleConnectivity = EdgeAdd(EdgeCreate(), surface);
  }
  return surface->triangleConnectivity;
}

extern Triangle_t *SurfaceGet(const Surface_t *surface, int polyId) {
  return *((Triangle_t **)SetElmGet(surface->triangle, polyId));
}

extern Triangle_t *SurfaceFirst(Surface_t *surface, int *polyId, Iterator_t *iter) {
  Triangle_t *triangle = NULL;
  if (polyId)
    *polyId = -1;
  if (surface) {
    iter->current = -1;
    if (SetSize(surface->triangle) > 0) {
      triangle = *((Triangle_t **)SetElmGet(surface->triangle, 0));
      iter->current = 0;
      if (polyId)
        *polyId = iter->current;
    }
  }
  return triangle;
}

extern Triangle_t *SurfaceNext(Surface_t *surface, int *polyId, Iterator_t *iter) {
  Triangle_t *triangle = NULL;
  assert(surface != NULL && iter->current >= 0);
  if (polyId)
    *polyId = -1;
  iter->current += 1;
  if (SetSize(surface->triangle) > iter->current) {
    triangle = *((Triangle_t **)SetElmGet(surface->triangle, iter->current));
    if (polyId)
      *polyId = iter->current;
  }
  if (!triangle) {
    iter->current = -1;
  }
  return triangle;
}

static void SurfaceTriangleFlip(Surface_t *surface, int triangleId) {
  Triangle_t *triangle = SurfaceGet(surface, triangleId);
  assert(SurfaceGetFrontDir(surface) == frontUnknownDir);
  TriangleFlip(triangle);
  /* Edge connectivity is destroyed! */
  surface->triangleConnectivity = EdgeDelete(surface->triangleConnectivity);
}

extern int SurfaceSize(const Surface_t *surface) { return SetSize(surface->triangle); }

extern const char *SurfaceUserName(const Surface_t *surface) { return surface->userName; }

extern int SurfaceID(const Surface_t *surface) { return surface->surfaceID; }

extern SurfaceType_t SurfaceType(const Surface_t *surface) { return surface->surfaceType; }

extern FrontDir_t SurfaceGetFrontDir(const Surface_t *surface) { return surface->frontDir; }

extern void SurfaceSetFrontDir(Surface_t *surface, FrontDir_t frontDir) {
  assert(surface->frontDir == frontUnknownDir || surface->frontDir == frontDir);
  assert(frontDir != frontUnknownDir);
  assert(SurfaceIsFault(surface));
  surface->frontDir = frontDir;
}

extern const char *SurfaceUserInfo(char *buf, const Surface_t *surface) {
  char surfType[BUFSIZ];
  const SurfOuterBoundary_t outer = SurfaceOuterBoundaryGet(surface);
  assert(surface != NULL);
  sprintf(buf, " NAME=%s\tID=%d\tPART=%d\tTYPE=%s%s ", SurfaceUserName(surface), SurfaceID(surface),
          SurfacePartGet(surface), SurfaceTypeString(surface, surfType),
          ((outer == surfOuterPosDir || outer == surfOuterNegDir) ? "\tBOUNDARY" : "\t        "));
  return buf;
}

extern void SurfaceInfoPrint(const Surface_t *surface, FILE *fp) {
  char buf[BUFSIZ];
  fprintf(fp, "SURFACE %s (number of triangles=%d) ", SurfaceUserInfo(buf, surface), SurfaceSize(surface));

  if (SurfaceIsFault(surface)) {
    fprintf(fp, " (%s)\n", SurfaceFrontDirString(surface, buf));
  } else {
    fprintf(fp, "\n");
  }
}

extern void SurfacePrint(Surface_t *surface, FILE *fp) {
  int triangleId;
  Iterator_t iter;
  Triangle_t *triangle = SurfaceFirst(surface, &triangleId, &iter);
  SurfaceInfoPrint(surface, fp);
  while (triangle) {
    const int *pointId = TrianglePoints(triangle);
    const size_t len = TriangleSize(triangle);
    assert(len == 3);
    fprintf(fp, "   triangle  id=%d\t\tpoint=%d %d %d\n", triangleId, pointId[0], pointId[1], pointId[2]);
    triangle = SurfaceNext(surface, &triangleId, &iter);
  }
}

extern bool_t SurfaceCheckEdgeOrientation(Surface_t *surface1, int poly1, int edge1, bool_t flip1, Surface_t *surface2,
                                          int poly2, int edge2, bool_t flip2) {
  int edge1Point1, edge1Point2;
  int edge2Point1, edge2Point2;
  bool_t isOK;

  TriangleSide(SurfaceGet(surface1, poly1), edge1, &edge1Point1, &edge1Point2);
  if (flip1)
    UTIL_SWAP(edge1Point1, edge1Point2, int);
  assert(edge1Point1 != edge1Point2);

  TriangleSide(SurfaceGet(surface2, poly2), edge2, &edge2Point1, &edge2Point2);
  if (flip2)
    UTIL_SWAP(edge2Point1, edge2Point2, int);
  assert(edge2Point1 != edge2Point2);

  isOK = (edge1Point1 == edge2Point2 && edge1Point2 == edge2Point1);
  return isOK;
}

static void SurfaceGetNeighbours(Surface_t *surface, int triangleId, Set_t *neighbours) {
  Triangle_t *triangle = SurfaceGet(surface, triangleId);
  const int numPoints = TriangleSize(triangle);
  Edge_t *edge = SurfaceTriangleConnectivity(surface);
  int n;

  SetClear(neighbours);

  for (n = 0; n < numPoints; n++) {
    int p1, p2;
    const EdgeKey_t *key = NULL;
    const EdgeList_t *list = NULL;
    TriangleSide(triangle, n, &p1, &p2);
    key = EdgeFind(edge, p1, p2);
    list = EdgeListFirst(edge, key);
    while (list) {
      const int eltId = EdgeListTriangleId(list);
      if (eltId != triangleId) {
        SetAdd(neighbours, &eltId, 1);
      }
      list = EdgeListNext(edge, list);
    }
  }
}

extern void *SurfaceEdgePoints(Surface_t *surface) {
  void *edgePoints = RBTreeCreate(sizeof(int), CompareInt);
  Edge_t *edge = SurfaceTriangleConnectivity(surface);
  Iterator_t iter;
  EdgeKey_t *key = EdgeKeyFirst(edge, &iter);

  /* Get list of points on outer edge of fault */
  while (key) {
    if (EdgeListSize(edge, key, NULL) == 1) {
      int p[2];
      EdgeKeyPoints(key, p + 0, p + 1);
      RBTreeSearch(edgePoints, p + 0);
      RBTreeSearch(edgePoints, p + 1);
    }
    key = EdgeKeyNext(edge, &iter);
  }

  return edgePoints;
}

extern Set_t *SurfacePoints(Surface_t *surface, Set_t *work) {
  Set_t *pointSet = work ? SetClear(work) : SetCreate(0, sizeof(int));
  Iterator_t iter;
  Triangle_t *triangle = SurfaceFirst(surface, NULL, &iter);
  while (triangle) {
    int *points = SetNew(pointSet, 3);
    UTIL_COPY(points, TrianglePoints(triangle), 3);
    triangle = SurfaceNext(surface, NULL, &iter);
  }
  /* Sort and remove double entries */
  SetSort(pointSet, CompareInt, TRUE);
  return pointSet;
}

static bool_t SurfaceCommonEdge(Surface_t *surface, int poly1, int poly2, int *edge1, int *edge2) {
  Triangle_t *triangle1 = SurfaceGet(surface, poly1);
  const int numPoints1 = TriangleSize(triangle1);
  bool_t ready = FALSE;
  Edge_t *edge = SurfaceTriangleConnectivity(surface);
  int n;

  for (n = 0; n < numPoints1 && !ready; n++) {
    const EdgeKey_t *key = NULL;
    const EdgeList_t *list = NULL;
    int p1, p2;

    *edge1 = -1;
    *edge2 = -1;

    TriangleSide(triangle1, n, &p1, &p2);
    key = EdgeFind(edge, p1, p2);
    list = EdgeListFirst(edge, key);
    while (list) {
      const int triangleId = EdgeListTriangleId(list);
      if (triangleId == poly1) {
        *edge1 = list->edge;
      } else if (triangleId == poly2) {
        *edge2 = list->edge;
      }
      list = EdgeListNext(edge, list);
    }
    ready = (MIN(*edge1, *edge2) >= 0);
  }
  return ready;
}

static bool_t SurfaceOrientateTriangle(Surface_t *surface, int curPolyId, bool_t curFlip, int ngbpolyId) {
  bool_t ngbFlip = FALSE;
  int curEdge, ngbEdge;
  bool_t found = SurfaceCommonEdge(surface, curPolyId, ngbpolyId, &curEdge, &ngbEdge);
  assert(found);
  ngbFlip = !SurfaceCheckEdgeOrientation(surface, curPolyId, curEdge, curFlip, surface, ngbpolyId, ngbEdge, FALSE);
  return ngbFlip;
}

extern void SurfaceOrientate(Surface_t *surface) {
  const int numPoly = SurfaceSize(surface);
  int *status = DIMALLOC(numPoly * sizeof(*status));
  int *stack = DIMALLOC(numPoly * sizeof(*stack));
  int *stackP = stack;
  Set_t *neighbours = SetCreate(10, sizeof(int));

  enum { surfOrientDoneFlip = -2, surfOrientStackFlip, surfOrientFresh = 0, surfOrientStack, surfOrientDone };

  assert(SurfaceGetFrontDir(surface) == frontUnknownDir);
  UTIL_SET(status, surfOrientFresh, numPoly);

  /* Orientation of first triangle is correct */
  status[0] = surfOrientStack;
  *stackP = 0;

  while (stackP >= stack) {
    /* Pop */
    const int curPolyId = *stackP;
    stackP -= 1;

    if (ABS(status[curPolyId]) < surfOrientDone) {
      int numNgb, n;
      const int *ngb = NULL;
      const bool_t curFlip = status[curPolyId] < 0;

      assert(status[curPolyId] != surfOrientFresh);

      /* Get neighbours */
      SurfaceGetNeighbours(surface, curPolyId, neighbours);
      numNgb = SetSize(neighbours);
      ngb = SetGet(neighbours);

      /* Loop over neighbours and push them on stack after orientation */
      for (n = 0; n < numNgb; n++) {
        const int ngbId = ngb[n];
        if (status[ngbId] == surfOrientFresh) {
          const bool_t ngbFlip = SurfaceOrientateTriangle(surface, curPolyId, curFlip, ngbId);
          status[ngbId] = ngbFlip ? surfOrientStackFlip : surfOrientStack;
          /* Push */
          stackP += 1;
          *stackP = ngbId;
        }
      }
      /* Mark this triangle as done */
      status[curPolyId] = (status[curPolyId] > 0) ? surfOrientDone : surfOrientDoneFlip;
    }
  }

  /* Flip the marked triangles */
  {
    int polyId;
    Iterator_t iter;
    Triangle_t *triangle = SurfaceFirst(surface, &polyId, &iter);
    while (triangle) {
      assert(ABS(status[polyId]) == surfOrientDone);
      if (status[polyId] == surfOrientDoneFlip) {
        SurfaceTriangleFlip(surface, polyId);
      }
      triangle = SurfaceNext(surface, &polyId, &iter);
    }
  }

  DIFREE(stack);
  DIFREE(status);
  SetDelete(neighbours);
}

extern void SurfaceFlip(Surface_t *surface) {
  Iterator_t iter;
  Triangle_t *triangle = SurfaceFirst(surface, NULL, &iter);

  assert(SurfaceGetFrontDir(surface) == frontUnknownDir);
  while (triangle) {
    TriangleFlip(triangle);
    triangle = SurfaceNext(surface, NULL, &iter);
  }
  /* Existing connectivity info no longer up-to-date */
  surface->triangleConnectivity = EdgeDelete(surface->triangleConnectivity);
}

extern BoundBox_t *SurfaceBoundBox(Surface_t *surface, PointSet_t *pointSet, BoundBox_t *boundBox) {
  Iterator_t iter;
  Triangle_t *triangle = SurfaceFirst(surface, NULL, &iter);
  while (triangle) {
    boundBox = TriangleBoundBox(triangle, pointSet, boundBox);
    triangle = SurfaceNext(surface, NULL, &iter);
  }
  return boundBox;
}

extern bool_t SurfaceFrontBackOfSameFault(const Surface_t *surf1, const Surface_t *surf2) {
  bool_t sameFault = FALSE;
  if (surf1 && surf2) {
    if (surf1 != surf2) {
      const int type1 = SurfaceType(surf1);
      const int type2 = SurfaceType(surf2);
      if ((type1 != surfNormal) && (type2 != surfNormal)) {
        if (type1 != type2) {
          sameFault = (SurfaceID(surf1) == SurfaceID(surf2));
        }
      }
    }
  }
  return sameFault;
}

extern char *SurfaceTypeString(const Surface_t *surf, char *buf) {

  switch (SurfaceType(surf)) {
  case surfNormal:
    strcpy(buf, "normal");
    break;
  case surfFrontFault:
    strcpy(buf, "front");
    break;
  case surfBackFault:
    strcpy(buf, "back");
    break;
  default:
    strcpy(buf, "???");
    break;
  }
  return buf;
}

extern char *SurfaceFrontDirString(const Surface_t *surf, char *buf) {

  switch (SurfaceGetFrontDir(surf)) {
  case frontUnknownDir:
    strcpy(buf, "fault direction ???");
    break;
  case frontPosDir:
    strcpy(buf, "front fault positive");
    break;
  case frontNegDir:
    strcpy(buf, "front fault negative");
    break;
  default:
    strcpy(buf, "???");
    break;
  }
  return buf;
}

extern void SurfaceNormal(Surface_t *surface, PointSet_t *pointSet, double *normal) {
  Iterator_t iter;
  Triangle_t *triangle = SurfaceFirst(surface, NULL, &iter);
  double triangleNormal[3];
  UTIL_SET(normal, 0, 3);
  while (triangle) {
    TriangleNormal(triangle, pointSet, triangleNormal);
    VecOperVec(normal, EqlPos, normal, OperPlus, triangleNormal, 3);
    triangle = SurfaceNext(surface, NULL, &iter);
  }
}

extern bool_t SurfaceActive(const Surface_t *surface, const bool_t posDir) {
  bool_t isActive = TRUE;

  if (SurfaceIsFault(surface)) {
    FrontDir_t faultDir = SurfaceFaultDir(surface, posDir);
    isActive = (SurfaceGetFrontDir(surface) == faultDir);
  }
  return isActive;
}

extern FrontDir_t SurfaceFaultDir(const Surface_t *surf, const bool_t posDir) {
  FrontDir_t fdir = frontUnknownDir;
  const SurfaceType_t type = SurfaceType(surf);
  assert(type == surfFrontFault || type == surfBackFault);
  if (type == surfFrontFault) {
    fdir = (posDir ? frontPosDir : frontNegDir);
  } else {
    fdir = (posDir ? frontNegDir : frontPosDir);
  }
  return fdir;
}

extern int SurfaceComp(const void *surf0V, const void *surf1V) {
  const Surface_t *surf0 = surf0V;
  const Surface_t *surf1 = surf1V;
  int result = UTIL_COMPARE(surf0->surfaceID, surf1->surfaceID);
  if (!result) {
    result = UTIL_COMPARE(surf0->part, surf1->part);
    if (!result) {
      /* Seperate front and back */
      const int type0 = SurfaceType(surf0);
      const int type1 = SurfaceType(surf1);
      result = UTIL_COMPARE(type0, type1);
    }
  }
  return result;
}

extern SurfOuterBoundary_t SurfaceOuterBoundaryGet(const Surface_t *surf) { return surf->outerBoundarY; }

extern void SurfaceOuterBoundarySet(Surface_t *surf, SurfOuterBoundary_t outer) {
#if 1
  assert(surf->outerBoundarY == surfOuterUnknown || surf->outerBoundarY == outer || outer == surfOuterUnknown);
#endif
  surf->outerBoundarY = outer;
}

extern bool_t SurfaceGetOpenFault(const Surface_t *surf) { return surf->openFault; }

extern void SurfaceSetOpenFault(Surface_t *surf, bool_t isOuter) { surf->openFault = isOuter; }

extern bool_t SurfaceOuterOrOpenFault(const Surface_t *surf) {
  return SurfaceAtOuterBoundary(surf) || SurfaceGetOpenFault(surf);
}

extern bool_t SurfaceAtOuterBoundary(const Surface_t *surf) {
  const SurfOuterBoundary_t outerBound = SurfaceOuterBoundaryGet(surf);
  assert(outerBound != surfOuterUnknown);
  return (outerBound != surfOuterNot);
}

extern bool_t SurfaceIsOuterBoundary(const Surface_t *surf, bool_t posDir) {
  const SurfOuterBoundary_t outerBound = SurfaceOuterBoundaryGet(surf);
  assert(outerBound != surfOuterUnknown);
  return (posDir ? outerBound == surfOuterPosDir : outerBound == surfOuterNegDir);
}

extern int SurfacePartGet(const Surface_t *surf) { return surf->part; }

static char *SurfaceGocadFileName(const Surface_t *surf, char *buf) {
  char idBuf[64], partBuf[64], typeBuf[64];
  const char *idString = UtilInt2String(idBuf, SurfaceID(surf));
  const char *partString = UtilInt2String(partBuf, SurfacePartGet(surf));
  char *typeString = SurfaceTypeString(surf, typeBuf);

  sprintf(buf, "%c_%s_%s_%s", toupper(*typeString), idString, partString, SurfaceUserName(surf));
  return buf;
}

extern void SurfaceToGocad(Surface_t *surf, PointSet_t *pointSet)

{
  char filenameBuf[1024], *filename = SurfaceGocadFileName(surf, filenameBuf);
  FILE *file = fopen(filename, "w");
  RBTree_t *surfPoints = RBTreeCreate(sizeof(int), CompareInt);
  Iterator_t iter;
  Triangle_t *triangle = NULL;

  /* Print header */
  assert(file != NULL);
  fprintf(file, "GOCAD TSurf\nHEADER {\nname:%s\n}\nTFACE\n", SurfaceUserName(surf));

  /* Pick up active points */
  triangle = SurfaceFirst(surf, NULL, &iter);
  while (triangle) {
    const int *points = TrianglePoints(triangle);
    RBTreeSearch(surfPoints, points + 0);
    RBTreeSearch(surfPoints, points + 1);
    RBTreeSearch(surfPoints, points + 2);
    triangle = SurfaceNext(surf, NULL, &iter);
  }

  /* Print active points */
  {
    const int *point = RBTreeFirst(surfPoints, &iter);
    while (point) {
      char pointBuf[64];
      double cor[3];
      PointSetFind(pointSet, *point, cor + 0, cor + 1, cor + 2);
      fprintf(file, "VRTX %s %lf %lf %lf \n", UtilInt2String(pointBuf, *point), cor[0], cor[1], cor[2]);
      point = RBTreeNext(surfPoints, &iter);
    }
  }
  fprintf(file, "\n");

  /* Print active triangles */
  triangle = SurfaceFirst(surf, NULL, &iter);
  while (triangle) {
    const int *points = TrianglePoints(triangle);
    fprintf(file, "TRGL %d %d %d\n", points[0], points[1], points[2]);
    triangle = SurfaceNext(surf, NULL, &iter);
  }
  fprintf(file, "END");
  fclose(file);
  RBTreeDelete(surfPoints);
}

extern Surface_t *SurfaceIntersectPlane(Surface_t *triangleSet, PointSet_t *pointSet, const double *planePoint,
                                        const double *unitNormal) {
  Surface_t *result = SurfaceCreate(0, 0, surfNormal, frontUnknownDir, "directionSpace", surfOuterUnknown);
  Iterator_t iter;
  const Triangle_t *triangle = SurfaceFirst(triangleSet, NULL, &iter);
  while (triangle) {
    TriangleIntersectPlane(result, triangle, pointSet, planePoint, unitNormal);
    triangle = SurfaceNext(triangleSet, NULL, &iter);
  }
  return result;
}

/* Front and back are identical if they have the same ID, and the same triangles */
extern bool_t SurfaceFrontBackIdenticalTest(Surface_t *front, Surface_t *back) {
  bool_t identical = (SurfaceID(front) == SurfaceID(back));
  if (identical) {
    if (SurfaceType(front) == surfFrontFault && SurfaceType(back) == surfBackFault) {
      identical = (SurfaceSize(front) == SurfaceSize(back));
      if (identical) {
        /* Full test on triangle points */
        RBTree_t *frontPoints = RBTreeCreate(3 * sizeof(int), CompareInt3);
        /* Store front triangles in tree */
        Iterator_t iter;
        const Triangle_t *triangle = SurfaceFirst(front, NULL, &iter);
        while (triangle) {
          const int *points = TrianglePoints(triangle);
          int p[3];
          UTIL_COPY(p, points, 3);
          UTIL_SORT_DIRECT(p, 3, int);
          RBTreeSearch(frontPoints, p);
          triangle = SurfaceNext(front, NULL, &iter);
        }

        /* Test if all back triangles are in front */
        triangle = SurfaceFirst(back, NULL, &iter);
        while (triangle && identical) {
          const int *points = TrianglePoints(triangle);
          int p[3];
          UTIL_COPY(p, points, 3);
          UTIL_SORT_DIRECT(p, 3, int);
          identical = (RBTreeFind(frontPoints, p) >= 0);
          triangle = SurfaceNext(back, NULL, &iter);
        }
        RBTreeDelete(frontPoints);
      }
    } else if (SurfaceType(back) == surfFrontFault && SurfaceType(front) == surfBackFault) {
      /* Swap arguments */
      identical = SurfaceFrontBackIdenticalTest(back, front);
    }
  }
  return identical;
}

extern bool_t SurfaceIsFault(const Surface_t *surf) { return (SurfaceType(surf) != surfNormal); }

extern int SurfacePairCompare(const void *surfPair1V, const void *surfPair2V) {
  const SurfacePair_t *surfPair1 = surfPair1V;
  const SurfacePair_t *surfPair2 = surfPair2V;
  int result = SurfaceComp(surfPair1->surf1, surfPair2->surf1);
  if (!result) {
    result = SurfaceComp(surfPair1->surf2, surfPair2->surf2);
  }
  return result;
}

extern SurfacePair_t *SurfacePairInit(SurfacePair_t *surfPair, const Surface_t *surf1, const Surface_t *surf2) {
  const int cmp = SurfaceComp(surf1, surf2);
  if (cmp >= 0) {
    surfPair->surf1 = surf1;
    surfPair->surf2 = surf2;
  } else {
    surfPair->surf1 = surf2;
    surfPair->surf2 = surf1;
  }
  return surfPair;
}

extern void SurfaceMeshMessageAdd(void *meshV, const Surface_t *surface) {
  char buf[BUFSIZ];
  sprintf(buf, "%s\n", SurfaceUserInfo(buf, surface));
  MeshMessageAdd(meshV, buf);
}
