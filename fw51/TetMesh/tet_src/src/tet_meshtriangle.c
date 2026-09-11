/*                                         Copyright (c) 2011 TNO DIANA BV */
#include "tet_meshtriangle.h"

static int MeshTriangleComp(const void *mt0V, const void *mt1V) {
  const MeshTriangle_t *mt0 = mt0V;
  const MeshTriangle_t *mt1 = mt1V;
  return TriangleComp(mt0->triangle, mt1->triangle);
}

static MeshTriangle_t *MeshTriangleCreate(int p0, int p1, int p2) {
  MeshTriangle_t *mt = DIMALLOC(sizeof(*mt));
  int trianglePoints[3];
  UTIL_ZERO(mt);
  trianglePoints[0] = p0;
  trianglePoints[1] = p1;
  trianglePoints[2] = p2;
  mt->triangle = TriangleCreate(trianglePoints, -1);
  return mt;
}

static MeshTriangle_t *MeshTriangleClear(MeshTriangle_t *mt) {
  if (mt) {
    mt->triangle = TriangleDelete(mt->triangle);
  }
  return mt;
}

static MeshTriangle_t *MeshTriangleDelete(MeshTriangle_t *mt) {
  if (mt) {
    MeshTriangleClear(mt);
    DIFREE(mt);
  }
  return mt;
}

extern MeshTriangleSet_t *MeshTriangleSetCreate(void) { return RBTreeCreate(sizeof(MeshTriangle_t), MeshTriangleComp); }

extern MeshTriangleSet_t *MeshTriangleSetDelete(MeshTriangleSet_t *mtSet) {
  if (mtSet) {
    Iterator_t iter;
    MeshTriangle_t *mt = RBTreeFirst(mtSet, &iter);
    while (mt) {
      MeshTriangleClear(mt);
      mt = RBTreeNext(mtSet, &iter);
    }
  }
  return RBTreeDelete(mtSet);
}
extern void MeshTriangleSetAdd(MeshTriangleSet_t *mtSet, int p0, int p1, int p2, int tetPoint) {
  MeshTriangle_t *mt = MeshTriangleSetFind(mtSet, p0, p1, p2);
  UNUSED(tetPoint);
  if (!mt) {
    int trianglePoints[3];
    MeshTriangle_t mtNew;
    UTIL_ZERO(&mtNew);
    trianglePoints[0] = p0;
    trianglePoints[1] = p1;
    trianglePoints[2] = p2;
    mtNew.triangle = TriangleCreate(trianglePoints, -1);
    mt = RBTreeSearchGet(mtSet, &mtNew);
  }

  mt->nTetrahedron += 1;
}

extern int MeshTriangleSetSize(const MeshTriangleSet_t *mtSet) { return (int)RBTreeSize(mtSet); }

extern const MeshTriangle_t *MeshTriangleSetFirst(MeshTriangleSet_t *mtSet, Iterator_t *iter) {
  return RBTreeFirst(mtSet, iter);
}

extern const MeshTriangle_t *MeshTriangleSetNext(MeshTriangleSet_t *mtSet, Iterator_t *iter) {
  return RBTreeNext(mtSet, iter);
}

extern MeshTriangle_t *MeshTriangleSetFind(MeshTriangleSet_t *mtSet, int p0, int p1, int p2) {
  MeshTriangle_t *mt = MeshTriangleCreate(p0, p1, p2);
  MeshTriangle_t *result = RBTreeFindGet(mtSet, mt);
  MeshTriangleDelete(mt);
  return result;
}

extern Surface_t *MeshTriangleGetSurface(const MeshTriangle_t *mt) { return mt->surface; }

extern void MeshTriangleSetSurface(MeshTriangle_t *mt, Surface_t *surf, int interfaceIdx) {
  assert(!mt->surface || !SurfaceComp(mt->surface, surf));
  mt->surface = surf;
  mt->triangle->interfaceIdx = interfaceIdx;
}

extern int MeshTriangleSize(const MeshTriangle_t *mt) { return mt->nTetrahedron; }
