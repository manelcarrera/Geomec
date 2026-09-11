/* Copyright (c) 2011 TNO DIANA BV                              Confidential */
#ifndef EDGE_H
#define EDGE_H

#include "Set.h"
#include "tet_surface.h"
#include "tet_utils.h"

#define EDGE_INVALID_ID (-1)

typedef struct EdgeKey_t {
  int minPoint, maxPoint;
  int list;
} EdgeKey_t;

typedef struct EdgeList_t {
  Surface_t *surface;
  int triangleId;
  int edge;
  int next;
} EdgeList_t;

typedef struct Edge_t {
  RBTree_t *edgeKeyTree;
  Set_t *edgeList;
} Edge_t;

#define EdgeKeyPoints(key, p1, p2) (*(p1) = (key)->minPoint, *(p2) = (key)->maxPoint)
#define EdgeKeyContains(key, p1) (((key)->minPoint == (p1)) || ((key)->maxPoint == (p1)))
#define EdgeListSurface(list) ((list)->surface)
#define EdgeListTriangleId(list) ((list)->triangleId)
#define EdgeListEdge(list) ((list)->edge)

extern Edge_t *EdgeCreate(void);
extern Edge_t *EdgeDelete(Edge_t *);
extern Edge_t *EdgeAdd(Edge_t *, Surface_t *);
extern EdgeKey_t *EdgeFind(Edge_t *, int, int);
extern EdgeKey_t *EdgeKeyFirst(Edge_t *, Iterator_t *);
extern EdgeKey_t *EdgeKeyNext(Edge_t *, Iterator_t *);
extern int EdgeKeyOffset(const Edge_t *, const EdgeKey_t *);
extern int EdgeListSize(Edge_t *, const EdgeKey_t *, int *);
extern EdgeList_t *EdgeListFirst(Edge_t *, const EdgeKey_t *);
extern EdgeList_t *EdgeListNext(Edge_t *, const EdgeList_t *);
extern void EdgeListGet(Edge_t *, const EdgeKey_t *, EdgeList_t *, int *);
extern void EdgeListPrint(const EdgeList_t *, FILE *);
extern void EdgeKeyPrint(Edge_t *, const EdgeKey_t *, FILE *);
extern void EdgePrint(Edge_t *, FILE *);
extern bool_t EdgeIsolatedFault(Edge_t *, const EdgeKey_t *);
extern int EdgeListOtherPoint(const EdgeKey_t *, const EdgeList_t *);
extern bool_t EdgeInternal(Edge_t *, const EdgeKey_t *);
extern bool_t EdgeFaultTouchesOuter(Edge_t *, const EdgeKey_t *);
extern bool_t EdgeKeyDebug(Edge_t *, const EdgeKey_t *);
extern BoundBox_t *EdgeKeyBoundBox(const EdgeKey_t *, PointSet_t *, BoundBox_t *);
extern void EdgeKeyMeshMessageAdd(void *, Edge_t *, EdgeKey_t *, PointSet_t *);

extern CompareFunction_t EdgeListComp;

#endif
