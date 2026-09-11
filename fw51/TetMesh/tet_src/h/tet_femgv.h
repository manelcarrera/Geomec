/* Copyright (c) 2011 TNO DIANA BV                              Confidential */
#ifndef FEMGV_H
#define FEMGV_H

#include "tet_bodyset.h"
#include "tet_surfaceset.h"
#include "tet_tetrahedron.h"
#include "tet_utils.h"

typedef struct FemGvPath_t {
  const char *sDianaPath;
  const char *sDiaSharePath;
  const char *sDianaLibPath;
  const char *sDianaSLibPath;
  const char *sFgvExeName;
} FemGvPath_t;

extern TetrahedronSet_t *FemGvMeshBody(PointSet_t *, SurfaceSet_t *, Body_t *, BodySet_t *, const FemGvPath_t *);

#endif
