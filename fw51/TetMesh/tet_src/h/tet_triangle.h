 /* Copyright (c) 2011 TNO DIANA BV                              Confidential */
#ifndef TRIANGLE_H
#define TRIANGLE_H

#include "tet_utils.h"
#include "tet_point.h"
#include "BoundBox.h"

typedef struct Triangle_t {
  int            vertex[3];
  int            interfaceIdx;
} Triangle_t;

extern Triangle_t  *TriangleCopy( const Triangle_t * );
extern Triangle_t  *TriangleCreate( const int *, int );
extern Triangle_t  *TriangleCreateFromPoints( int, int, int, int );
extern Triangle_t  *TriangleChange( Triangle_t *, const int * );
extern Triangle_t  *TriangleDelete( Triangle_t * );
extern int         TriangleSize( const Triangle_t * );
extern const int  *TrianglePoints( const Triangle_t * );
extern void        TrianglePrint( const Triangle_t *, FILE * );
extern void        TriangleSide( const Triangle_t *, int, int *, int * );
extern Triangle_t *TriangleFlip( Triangle_t * );
extern bool_t      TriangleUnitNormal( const Triangle_t *, PointSet_t *, double *);
extern void        TriangleCenter( const Triangle_t *, PointSet_t *, double * );
extern int         TrianglePointPosition( const Triangle_t *, 
                             PointSet_t *, const double, const double *);
extern BoundBox_t *TriangleBoundBox( const Triangle_t *, PointSet_t *, BoundBox_t * );
extern void TriangleNormal( const Triangle_t *, PointSet_t *, double * );
extern double TriangleArea( const Triangle_t *, PointSet_t * ); 
extern bool_t TriangleIntersectLine( const Triangle_t *, PointSet_t *, const double *, const double *, const double ); 
extern bool_t TriangleIntersect( const Triangle_t *, const Triangle_t *, PointSet_t * ); 
extern double TriangleAspectRatio( const Triangle_t *, PointSet_t * );
extern const char *TriangleSPrint( char *, const Triangle_t *, PointSet_t * );
extern void TriangleLocate2( const Triangle_t *, int, int, int *, bool_t * );
extern double TriangleSmallestAngle( const Triangle_t *, PointSet_t * );
extern double TriangleAngleBetween( const Triangle_t *, const Triangle_t *, PointSet_t * );
extern bool_t TriangleCommonEdge( const Triangle_t *, const Triangle_t *, int *, int * ) ;
extern bool_t TriangleCommonPoint( const Triangle_t *, const Triangle_t *, int * );
extern bool_t TriangleContains( const Triangle_t *, int );
extern bool_t TriangleDebug( const Triangle_t * );
extern CompareFunction_t TriangleComp;
extern void TriangleMeshMessageAdd( void *, PointSet_t *, const Triangle_t * );
extern int TriangleInterfaceIdx( const Triangle_t * );


#include "tet_surface.h"
extern void   TriangleIntersectPlane( Surface_t *, const Triangle_t *, PointSet_t *, const double *, const double * );



#endif
