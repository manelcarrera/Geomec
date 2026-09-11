/*                                         Copyright (c) 2011 TNO DIANA BV */
#include "tet_point.h"
#include "BoundBox.h"
#include "RBTree.h"
#include "float.h"
#include "tet_mesh.h"

/* Always set before calling RB* routines!!! */
static double smallDistance;

static int PointCompare(const void *cor0V, const void *cor1V) {
  int result = 0, d;
  const double *cor0 = cor0V;
  const double *cor1 = cor1V;
  for (d = 0; d < 3 && !result; d++) {
    if (cor0[d] - cor1[d] < -smallDistance) {
      result = -1;
    } else if (cor0[d] - cor1[d] > smallDistance) {
      result = 1;
    }
  }
  return result;
}

extern PointSet_t *PointSetCreate(double eps) {
  PointSet_t *result = DIMALLOC(sizeof(*result));
  result->tree = RBTreeCreate(3 * sizeof(double), PointCompare);
  result->eps = eps;
  return result;
}

extern PointSet_t *PointSetDelete(PointSet_t *pointSet) {
  if (pointSet) {
    RBTreeDelete(pointSet->tree);
    DIFREE(pointSet);
  }
  return NULL;
}

extern PointSet_t *PointSetCopy(PointSet_t *pointSet) {
  PointSet_t *result = PointSetCreate(PointSetTolerance(pointSet));
  const int size = PointSetSize(pointSet);
  int n;
  for (n = 0; n < size; n++) {
    const double *cor = PointSetGet(pointSet, n);
    PointSetAdd(result, cor[0], cor[1], cor[2]);
  }
  return result;
}
extern int PointSetAdd(PointSet_t *pointSet, double x, double y, double z) {
  double cor[3];
  cor[0] = x;
  cor[1] = y;
  cor[2] = z;
  smallDistance = pointSet->eps;
  return RBTreeSearch(pointSet->tree, cor);
}

extern bool_t PointSetTest(PointSet_t *pointSet, double x, double y, double z) {
  double cor[3];
  cor[0] = x;
  cor[1] = y;
  cor[2] = z;
  smallDistance = pointSet->eps;
  return (RBTreeFind(pointSet->tree, cor) != RBTREE_INVALID_ID);
}

extern const double *PointSetGet(PointSet_t *pointSet, int id) { return RBTreeGet(pointSet->tree, id); }

extern void PointSetFind(PointSet_t *pointSet, int id, double *x, double *y, double *z) {
  const double *cor = PointSetGet(pointSet, id);
  *x = cor[0];
  *y = cor[1];
  *z = cor[2];
}

extern int PointSetSize(const PointSet_t *pointSet) { return (int)RBTreeSize(pointSet->tree); }

extern void PointSetPrintPoint(PointSet_t *pointSet, int id, FILE *fp) {
  char buf[BUFSIZ];
  fprintf(fp, "%d\t%s\n", id, PointSetSPrint(buf, pointSet, id));
}

extern void PointSetPrint(PointSet_t *pointSet, FILE *fp) {
  const int len = PointSetSize(pointSet);
  int n;
  fprintf(fp, "\nPOINTSET   size=%d  eps=%f\n", len, pointSet->eps);
  for (n = 0; n < len; n++) {
    PointSetPrintPoint(pointSet, n, fp);
  }
}

extern const char *PointSetSPrint(char *buf, PointSet_t *pointSet, const int pointId) {
  const double *cor = PointSetGet(pointSet, pointId);
  sprintf(buf, "( %lf, %lf, %lf )", cor[0], cor[1], cor[2]);
  return buf;
}

static double PointSetDistance(PointSet_t *pointSet, int p1, int p2) {
  const double *cor1 = PointSetGet(pointSet, p1);
  const double *cor2 = PointSetGet(pointSet, p2);
  double cor[3];
  cor[0] = cor1[0] - cor2[0];
  cor[1] = cor1[1] - cor2[1];
  cor[2] = cor1[2] - cor2[2];
  return VecL2Norm(cor, 3);
}

extern double PointSetMinimumDistance(PointSet_t *pointSet, int *id1, int *id2, double *measure) {
  double result = 0;
  const int nPoint = PointSetSize(pointSet);
  *id1 = RBTREE_INVALID_ID;
  *id2 = RBTREE_INVALID_ID;
  if (nPoint > 1) {
    /* Build a tree of bounding boxes around points */
    BoundBoxTree_t *bbTree = BoundBoxTreeCreate();
    BoundBox_t bb;
    Set_t *box = NULL;
    int n;
    for (n = 0; n < nPoint; n++) {
      const double *cor = PointSetGet(pointSet, n);
      BoundBoxTreeAdd(bbTree, n, BoundBoxAdd(BoundBoxInit(&bb), cor));
    }

    result = BoundBoxSize(BoundBoxTreeGetBoundBox(bbTree));
    *measure = result;
    for (n = 0; n < nPoint; n++) {
      double corMin[3], corMax[3];
      int d;
      bool_t selfFound = FALSE;
      const double *cor = PointSetGet(pointSet, n);
      for (d = 0; d < 3; d++) {
        corMin[d] = cor[d] - result;
        corMax[d] = cor[d] + result;
      }
      BoundBoxInit(&bb);
      BoundBoxAdd(&bb, corMin);
      BoundBoxAdd(&bb, corMax);
      box = BoundBoxTreeFind(box, bbTree, &bb, BoundBoxOverlap);
      for (d = 0; d < SetSize(box); d++) {
        const int *id = SetElmGet(box, d);
        if (*id != n) {
          const double dist = PointSetDistance(pointSet, n, *id);
          if (dist < result) {
            result = dist;
            *id1 = n;
            *id2 = *id;
          }
        } else {
          selfFound = TRUE;
        }
      }
      assert(selfFound);
    }
    BoundBoxTreeDelete(bbTree);
    SetDelete(box);
  }
  return result;
}

extern double PointSetTolerance(const PointSet_t *pointSet) { return pointSet->eps; }

extern int PointSetCreateMidPoint(PointSet_t *pointSet, int p0, int p1) {
  double pointX[3], point0[3], point1[3];

  /* Get coordinates of mid-point, notice that the point is not shifted! */
  PointSetFind(pointSet, p0, point0 + 0, point0 + 1, point0 + 2);
  PointSetFind(pointSet, p1, point1 + 0, point1 + 1, point1 + 2);
  VecOperVec(pointX, EqlPos, point0, OperPlus, point1, 3);
  VecOperScal(pointX, EqlPos, pointX, OperTimes, 0.5, 3);
  /* Add the midpoint to the point set */
  return PointSetAdd(pointSet, pointX[0], pointX[1], pointX[2]);
}

extern void PointMeshMessageAdd(void *meshV, PointSet_t *pointSet, int pointId) {
  char buf[BUFSIZ], pointBuf[BUFSIZ];
  sprintf(buf, "   %s\n", PointSetSPrint(pointBuf, pointSet, pointId));
  MeshMessageAdd(meshV, buf);
}
