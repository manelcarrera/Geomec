 /* Copyright (c) 2011 TNO DIANA BV                              Confidential */
#ifndef MESHTRIANGLE_H
#define MESHTRIANGLE_H

#include "tet_utils.h"
#include "RBTree.h"
#include "tet_triangle.h"
#include "tet_surface.h"

typedef struct MeshTriangle_t {
  Triangle_t   *triangle;
  int           nTetrahedron;
  Surface_t    *surface;
} MeshTriangle_t;

typedef RBTree_t   MeshTriangleSet_t;


extern MeshTriangleSet_t    *MeshTriangleSetCreate( void );
extern MeshTriangleSet_t    *MeshTriangleSetDelete( MeshTriangleSet_t * );
extern void                  MeshTriangleSetAdd( MeshTriangleSet_t *, int, int, int, int );
extern int                   MeshTriangleSetSize( const MeshTriangleSet_t * );
extern const MeshTriangle_t *MeshTriangleSetFirst( MeshTriangleSet_t *, Iterator_t * );
extern const MeshTriangle_t *MeshTriangleSetNext( MeshTriangleSet_t *, Iterator_t * );
extern MeshTriangle_t       *MeshTriangleSetFind( MeshTriangleSet_t *, int, int, int );
extern Surface_t            *MeshTriangleGetSurface( const MeshTriangle_t * );
extern void                  MeshTriangleSetSurface( MeshTriangle_t *, Surface_t *, int );
extern int                   MeshTriangleSize( const MeshTriangle_t * );


#endif
