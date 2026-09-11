/* Copyright (c) 2011 TNO DIANA BV                              Confidential */
#ifndef SURFACESET_H
#define SURFACESET_H

#include "Set.h"
#include "tet_bodyset.h"
#include "tet_edge.h"
#include "tet_point.h"
#include "tet_xpoint.h"

typedef struct SurfaceSet_t {
  RBTree_t *surface;
  Edge_t *edge;
} SurfaceSet_t;

typedef struct SurfaceData_t {
  Surface_t *surface;
} SurfaceData_t;

extern Surface_t *SurfaceSetGet(SurfaceSet_t *, int);
extern SurfaceSet_t *SurfaceSetPurge(SurfaceSet_t *);
extern SurfaceSet_t *SurfaceSetCreate(void);
extern SurfaceSet_t *SurfaceSetDelete(SurfaceSet_t *);
extern SurfaceSet_t *SurfaceSetCopy(SurfaceSet_t *);
extern void SurfaceSetPrint(SurfaceSet_t *, FILE *);
extern void SurfaceSetAdd(SurfaceSet_t *, Surface_t *);
extern void SurfaceSetAddSurfaceSet(SurfaceSet_t *, SurfaceSet_t *, bool_t);
extern int SurfaceSetSize(const SurfaceSet_t *);
extern int SurfaceSetTriangleSize(SurfaceSet_t *);
extern int SurfaceSetSurfaceId(SurfaceSet_t *, const Surface_t *);
extern Surface_t *SurfaceSetFirst(SurfaceSet_t *, Iterator_t *);
extern Surface_t *SurfaceSetNext(SurfaceSet_t *, Iterator_t *);
extern SurfaceSet_t *SurfaceSetSplitIntoSimpleSurface(void *, SurfaceSet_t *, bool_t);
extern void SurfaceSetFillBodySet(void *, SurfaceSet_t *, PointSet_t *, BodySet_t *);
extern Edge_t *SurfaceSetEdge(SurfaceSet_t *);
extern SurfaceSet_t *SurfaceSetClear(SurfaceSet_t *);
extern int SurfaceSetSignedSurfId(SurfaceSet_t *, const Surface_t *, bool_t);
extern Set_t *SurfaceSetAllID(SurfaceSet_t *);
extern SurfaceSet_t *SurfaceSetSameID(SurfaceSet_t *, int);
extern SurfaceSet_t *SurfaceSetSameType(SurfaceSet_t *, SurfaceType_t);
extern void SurfaceSetFaultDirection(void *, SurfaceSet_t *, PointSet_t *);
extern int SurfaceSetFreeID(SurfaceSet_t *);
extern void SurfaceSetLinkFaultPoints(void *, PointSet_t *, XPointSet_t *, SurfaceSet_t *, bool_t);
extern void SurfaceSetMarkOuterBoundary(void *, bool_t, PointSet_t *, SurfaceSet_t *);
extern bool_t SurfaceSetClosedFault(void *, SurfaceSet_t *, Edge_t *, EdgeKey_t *);
extern bool_t SurfaceSetTest(SurfaceSet_t *, Surface_t *);
extern void SurfaceSetAddBody(SurfaceSet_t *, Body_t *);
extern void SurfaceSetToGocad(SurfaceSet_t *, PointSet_t *);
extern void SurfaceSetPrintConnectivity(SurfaceSet_t *, FILE *);
extern const Surface_t *SurfaceSetFind(SurfaceSet_t *, int, int, SurfaceType_t);
extern void SurfaceSetFrontBackIdentical(SurfaceSet_t *);
extern void SurfaceSetMeshMessageAdd(void *, SurfaceSet_t *);

#endif
