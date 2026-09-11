 /* Copyright (c) 2011 TNO DIANA BV                              Confidential */
#ifndef TETRAHEDRON_H
#define TETRAHEDRON_H

#include "tet_utils.h"
#include "RBTree.h"
#include "tet_point.h"

typedef struct Tetrahedron_t {
  int   nodes[4];
} Tetrahedron_t;

typedef Set_t   TetrahedronSet_t;


extern TetrahedronSet_t    *TetrahedronSetCreate( void );
extern TetrahedronSet_t    *TetrahedronSetDelete( TetrahedronSet_t * );
extern void                 TetrahedronSetAdd( TetrahedronSet_t *, int, int, int, int );
extern double               TetrahedronSetVolume( TetrahedronSet_t *, PointSet_t * );
extern int                  TetrahedronSetSize( const TetrahedronSet_t * );
extern const Tetrahedron_t *TetrahedronSetFirst( TetrahedronSet_t *, Iterator_t * );
extern const Tetrahedron_t *TetrahedronSetNext( TetrahedronSet_t *, Iterator_t * );
extern const Tetrahedron_t *TetrahedronSetAt( const TetrahedronSet_t *, int );
extern const Tetrahedron_t *TetrahedronSetFind( TetrahedronSet_t *, int, int, int, int );
extern void                 TetrahedronGet( const Tetrahedron_t *, int *, int *, int *, int * );
extern double               TetrahedronVolume( const Tetrahedron_t *, PointSet_t * );
extern double               TetrahedronVolumeFromPoints( const double *, const double *, 
                                                         const double *, const double * );
extern void                 TetrahedronPrint( const Tetrahedron_t *, FILE * );
extern void                 TetrahedronSetPrint( TetrahedronSet_t *, FILE * );
extern TetrahedronSet_t    *TetrahedronSetRenumber( TetrahedronSet_t *, const int * );


#endif
