 /* Copyright (c) 2011 TNO DIANA BV                              Confidential */
#ifndef XPOINT_H
#define XPOINT_H

#include "tet_utils.h"
#include "tet_surface.h"
#include "tet_point.h"

typedef struct {
   double           shift[3];
   int              pointId;
} XFaultPoint_t;

typedef struct {
  int nDummy;
} XNormal2Fault_t;

typedef enum {
     xpointInvalid
   , xpointFault
   , xpointFaultUnshifted
   , xpointNormal2Fault
   , xpointFaultLinked
   , xpointSelfIntersectOuterBoundary
   , xpointSelfIntersectOuterBoundaryLinked
} XPointType_t;

typedef struct {
   int              orgId;
   int              offset; /* Pointer to self or to xpoint to which this one is linked */
   XPointType_t     type;
   const Surface_t *surf;
   int              triangleId;
   union {
      XNormal2Fault_t  normalPoint;
      XFaultPoint_t    faultPoint;
   } data;
} XPoint_t;

typedef struct XPointSet_t {
   RBTree_t              *extraPoints;
} XPointSet_t;

#define XPointSurface( xp )    ( (xp)->surf )
#define XPointTriangleId( xp ) ( (xp)->triangleId )

extern XPointSet_t       *XPointSetCreate( void );
extern XPointSet_t       *XPointSetDelete( XPointSet_t * );
extern int                XPointSetSize( const XPointSet_t * );
extern int                XPointSetUnshift( XPointSet_t *, int, const Surface_t *, int );
extern void               XPointSetAddFault( XPointSet_t *, int, const Surface_t *, int, const double * ); 
extern int                XPointSetLinkFault( XPointSet_t *, int, const Surface_t *, int, const Surface_t *, int );
extern int                XPointSetLinkNormal( XPointSet_t *, int, const Surface_t *, int, const XPoint_t * );
extern int                XPointSetLinkNormal2( XPointSet_t *, int, const Surface_t *, int, const Surface_t *, int );
extern XPoint_t          *XPointSetFind( const XPointSet_t *, int, const Surface_t *, int ); 
extern int                XPointSetPointId( const XPointSet_t *, const XPoint_t * );
extern bool_t             XPointSetPointInAnyFault( const XPointSet_t *, int );
extern const XPoint_t    *XPointSetFollowLink( const XPointSet_t *, const XPoint_t * );

extern void               XPointSetPrint( XPointSet_t *, FILE * );
extern void               XPointSetPrintPoint( const XPoint_t *, FILE * );

extern XPoint_t          *XPointSetFirst( XPointSet_t  *, Iterator_t * );
extern XPoint_t          *XPointSetNext( XPointSet_t  *, Iterator_t * );

extern XPointType_t       XPointType( const XPoint_t * );
extern int                XPointOrgId( const XPoint_t * );
extern double            *XPointGetShift( PointSet_t *, XPointSet_t *, const XPoint_t *, double, double * );

#endif
