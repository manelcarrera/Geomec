/* Copyright (c) 2011 TNO DIANA BV                              Confidential */
#ifndef POINT_H
#define POINT_H

#include "RBTree.h"
#include "Set.h"
#include "tet_utils.h"

#define POINT_INVALID_ID (-1)

typedef struct PointSet_t {
  RBTree_t *tree;
  double eps;
} PointSet_t;

extern PointSet_t *PointSetCreate(double);
extern PointSet_t *PointSetCopy(PointSet_t *);
extern PointSet_t *PointSetDelete(PointSet_t *);
extern int PointSetAdd(PointSet_t *, double, double, double);
extern int PointSetSize(const PointSet_t *);
extern const double *PointSetGet(PointSet_t *, int);
extern void PointSetFind(PointSet_t *, int, double *, double *, double *);
extern int PointSetCreateMidPoint(PointSet_t *, int, int);

extern const char *PointSetSPrint(char *, PointSet_t *, const int);
extern void PointSetPrint(PointSet_t *, FILE *);
extern void PointSetPrintPoint(PointSet_t *, int, FILE *);

extern double PointSetMinimumDistance(PointSet_t *, int *, int *, double *);
extern bool_t PointSetTest(PointSet_t *, double, double, double);
extern double PointSetTolerance(const PointSet_t *);

extern void PointMeshMessageAdd(void *, PointSet_t *, int);

#endif
