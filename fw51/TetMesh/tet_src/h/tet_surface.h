/* Copyright (c) 2011 TNO DIANA BV                              Confidential */
#ifndef SURFACE_H
#define SURFACE_H

#define SURFACE_NO_MARKER 0

#include "BoundBox.h"
#include "Set.h"

typedef enum {
  surfNormal = -1,
  surfTypeFirst = surfNormal,
  surfFrontFault,
  surfBackFault,
  surfTypeLast = 3
} SurfaceType_t;

typedef enum FrontDir_t { frontUnknownDir, frontPosDir, frontNegDir } FrontDir_t;

typedef enum SurfOuterBoundary_t {
  surfOuterUnknown,
  surfOuterPosDir,
  surfOuterNegDir,
  surfOuterNot
} SurfOuterBoundary_t;

typedef struct Surface_t {
  Set_t *triangle;
  void *triangleConnectivity;
  int surfaceID;
  char *userName;
  SurfaceType_t surfaceType;
  FrontDir_t frontDir;               /* Only relevant for faults */
  SurfOuterBoundary_t outerBoundarY; /* On outer boundary of full model */
  bool_t openFault;                  /* Temporary data!! */
  int part;                          /* Original surface may be split in parts! */
} Surface_t;

typedef struct SurfacePair_t {
  const Surface_t *surf1;
  const Surface_t *surf2;
} SurfacePair_t;

#include "tet_triangle.h"

extern Surface_t *SurfaceCreate(int, int, SurfaceType_t, FrontDir_t, const char *, SurfOuterBoundary_t);
extern Surface_t *SurfaceDelete(Surface_t *);
extern Surface_t *SurfacePurge(Surface_t *);
extern Surface_t *SurfaceCopy(Surface_t *);
extern int SurfaceAdd(Surface_t *, Triangle_t *);
extern void SurfaceAddSurface(Surface_t *, Surface_t *);
extern Triangle_t *SurfaceFirst(Surface_t *, int *, Iterator_t *);
extern Triangle_t *SurfaceNext(Surface_t *, int *, Iterator_t *);
extern int SurfaceSize(const Surface_t *);
extern const char *SurfaceUserName(const Surface_t *);
extern void SurfacePrint(Surface_t *, FILE *);
extern void SurfaceInfoPrint(const Surface_t *, FILE *);
extern int SurfaceID(const Surface_t *);
extern int SurfacePartGet(const Surface_t *);
extern SurfaceType_t SurfaceType(const Surface_t *);
extern char *SurfaceTypeString(const Surface_t *, char *);
extern char *SurfaceFrontDirString(const Surface_t *, char *);
extern void SurfaceOrientate(Surface_t *);
extern void SurfaceFlip(Surface_t *);
extern Triangle_t *SurfaceGet(const Surface_t *, int);
extern bool_t SurfaceCheckEdgeOrientation(Surface_t *, int, int, bool_t, Surface_t *, int, int, bool_t);
extern BoundBox_t *SurfaceBoundBox(Surface_t *, PointSet_t *, BoundBox_t *);
extern void *SurfaceEdgePoints(Surface_t *);
extern Set_t *SurfacePoints(Surface_t *, Set_t *);
extern bool_t SurfaceFrontBackOfSameFault(const Surface_t *, const Surface_t *);
extern void SurfaceNormal(Surface_t *, PointSet_t *, double *);
extern void *SurfaceTriangleConnectivity(Surface_t *);
extern FrontDir_t SurfaceGetFrontDir(const Surface_t *);
extern void SurfaceSetFrontDir(Surface_t *, FrontDir_t);
extern bool_t SurfaceActive(const Surface_t *, const bool_t);
extern FrontDir_t SurfaceFaultDir(const Surface_t *, const bool_t);
extern const char *SurfaceUserInfo(char *, const Surface_t *);
extern CompareFunction_t SurfaceComp;
extern CompareFunction_t SurfacePairCompare;
extern void SurfaceOuterBoundarySet(Surface_t *, SurfOuterBoundary_t);
extern SurfOuterBoundary_t SurfaceOuterBoundaryGet(const Surface_t *);
extern bool_t SurfaceOuterOrOpenFault(const Surface_t *);
extern bool_t SurfaceAtOuterBoundary(const Surface_t *);
extern bool_t SurfaceIsOuterBoundary(const Surface_t *, bool_t);
extern void SurfaceSetOpenFault(Surface_t *, bool_t);
extern bool_t SurfaceGetOpenFault(const Surface_t *);
extern void SurfaceToGocad(Surface_t *, PointSet_t *);
extern Surface_t *SurfaceIntersectPlane(Surface_t *, PointSet_t *, const double *, const double *);
extern bool_t SurfaceFrontBackIdenticalTest(Surface_t *, Surface_t *);
extern bool_t SurfaceIsFault(const Surface_t *);
extern SurfacePair_t *SurfacePairInit(SurfacePair_t *, const Surface_t *, const Surface_t *);
extern void SurfaceMeshMessageAdd(void *, const Surface_t *);

#endif
