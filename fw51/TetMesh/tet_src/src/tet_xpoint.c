 /*                                         Copyright (c) 2011 TNO DIANA BV */
#include "tet_xpoint.h"

static const char *XPointTypeToString[] = {"Invalid" , "Fault" , "FaultUnshifted" , "Normal2Fault" , "FaultLinked" };

#define MRH_FIND_POINT -1

static int XPointCompare( const void* data0, const void* data1 )
{
   const XPoint_t *p0 = data0;
   const XPoint_t *p1 = data1;
   int                 result = UTIL_COMPARE( p0->orgId, p1->orgId );
  /* 
   * If NULL pointer found then compare equal, introduced to test if
   * if a point is ever in a fault
   */
   if ( p0->surf && p1->surf ) {
      if ( !result ) {
         result = SurfaceComp( p0->surf, p1->surf );
      }
      if ( !result ) {
         result = UTIL_COMPARE( p0->triangleId, p1->triangleId );
      }
   }
   return result;
}

static XPoint_t *XPointInit( 
                  int                         orgId,
                  const Surface_t            *surf,
                  int                         triangleId,
                  XPoint_t                   *xp )
{
  UTIL_ZERO( xp );
  xp->orgId = orgId;
  xp->surf = surf;
  assert( triangleId >= 0 );
  xp->triangleId = triangleId;
  xp->offset = -1;
  return xp;
}

extern XPointSet_t *XPointSetCreate( void )
{
  XPointSet_t *xPointSet = DIMALLOC( sizeof(*xPointSet) );
  UTIL_ZERO( xPointSet );
  xPointSet->extraPoints = RBTreeCreate( sizeof(XPoint_t), XPointCompare );
  return xPointSet;
}

extern XPointSet_t *XPointSetDelete( XPointSet_t *xPointSet )
{
  if ( xPointSet ) {
  RBTreeDelete( xPointSet->extraPoints );
  DIFREE( xPointSet );
  }
  return xPointSet;
}
extern int XPointSetSize( 
                  const XPointSet_t          *xPointSet )
{
  return (int)RBTreeSize( xPointSet->extraPoints );
}

static XFaultPoint_t *XFaultPointInit( 
                  XFaultPoint_t              *xfp,
                  const double               *shift )
{
  UTIL_ZERO( xfp );
  UTIL_COPY( xfp->shift, shift, 3 );
  xfp->pointId = -1; /* Invalid */
  return xfp;
}

extern void XPointSetAddFault( 
              XPointSet_t         *xpointSet,
              int                  orgId, 
              const Surface_t     *fault, 
              int                  triangleId, 
              const double        *shift ) 
{
  XPoint_t     xp, *found = NULL;
  
  assert( SurfaceType( fault ) != surfNormal );

  XPointInit( orgId, fault, triangleId, &xp );
  assert( !RBTreeFindGet( xpointSet->extraPoints, &xp ) );
  found = RBTreeSearchGet( xpointSet->extraPoints, &xp );

  found->offset = XPointSetSize( xpointSet ) - 1;
  assert( found->offset == RBTreeFind( xpointSet->extraPoints, found ) );
  found->type = xpointFault;
  XFaultPointInit( &(found->data.faultPoint), shift );

#if 0
if ( orgId == MRH_FIND_POINT ) {
  XPointSetPrintPoint( found, stderr );
}
#endif

}


extern int XPointSetUnshift(
              XPointSet_t         *xpointSet,
              int                  orgId,
              const Surface_t     *surf,
              int                  triangleId )
{
  int                 numChange = 0;
  if ( SurfaceType( surf ) != surfNormal ) {
  XPoint_t   *found = XPointSetFind( xpointSet, orgId, surf, triangleId );
  /* Take care of links */
  if ( found ) {
      found = (XPoint_t *)XPointSetFollowLink( xpointSet, found );
  }
  if ( found && found->type != xpointFaultUnshifted ) {
      found->type = xpointFaultUnshifted;
      numChange += 1;
#if 0
if ( orgId == MRH_FIND_POINT ) {
fprintf( stderr, "XPointSetUnshift orgId=%d triangleId=%d\n", orgId, triangleId );
SurfaceInfoPrint( surf, stderr );
fprintf( stderr, "\n\n" );
}
#endif
  }
  } else {
  assert( FALSE );
  }
  return numChange;
}


/* Return pointer to pointDataId */
extern XPoint_t *XPointSetFirst( 
                  XPointSet_t                *xpointSet,
                  Iterator_t                 *iter )
{
  return RBTreeFirst( xpointSet->extraPoints, iter );
}

/* Return pointer to pointDataId */
extern XPoint_t *XPointSetNext( 
                  XPointSet_t                *xpointSet,
                  Iterator_t                 *iter )
{
  return RBTreeNext( xpointSet->extraPoints, iter );
}

extern XPoint_t *XPointSetFind(
                  const XPointSet_t          *xpointSet,
                  int                         orgId,
                  const Surface_t            *surf,
                  int                         triangleId )
{
  XPoint_t   xp;
  return RBTreeFindGet( xpointSet->extraPoints, XPointInit( orgId, surf, triangleId, &xp ) );
}

extern XPointType_t XPointType(
                  const XPoint_t             *xp )
{
  return xp->type;
}

extern int XPointOrgId(
                  const XPoint_t             *xp )
{
  return xp->orgId;
}


extern const XPoint_t *XPointSetFollowLink(
                  const XPointSet_t          *xpointSet,
                  const XPoint_t             *xp )
{
  const XPoint_t *result = xp;
  

  switch ( XPointType( result ) ) {
  case xpointFaultUnshifted:
  case xpointFault:
      break;
  case xpointFaultLinked: 
  case xpointNormal2Fault: 
      {
    const XPoint_t  *xpLink = RBTreeGet( xpointSet->extraPoints, xp->offset );
    assert( xpLink != xp );
    result = XPointSetFollowLink( xpointSet, xpLink );
      }
      break;
  default:
      assert( FALSE );
      break;
  }
  return result;
}

extern int XPointSetPointId(
                  const XPointSet_t          *xpointSet,
                  const XPoint_t             *xp )
{
  int       pointId = -1;

  xp = XPointSetFollowLink( xpointSet, xp );

  switch ( XPointType( xp ) ) {
  case xpointFaultUnshifted:
      pointId = xp->orgId;
      break;
  case xpointFault:
      pointId = xp->data.faultPoint.pointId;
      break;
  default:
      assert( FALSE );
      break;
  }
  return pointId;
}

static int LinkFaultPoint(
                  XPointSet_t                *xpointSet,
                  XPoint_t                   *xp1,
                  XPoint_t                   *xp2 )
{
  int                 numChange = 0;
  const XPointType_t  type1 = XPointType( xp1 );
  const XPointType_t  type2 = XPointType( xp2 );
  XPoint_t           *xp1Link = (XPoint_t *)XPointSetFollowLink( xpointSet, xp1 );
  XPoint_t           *xp2Link = (XPoint_t *)XPointSetFollowLink( xpointSet, xp2 );

  assert( type1 == xpointFault || type1 == xpointFaultLinked );
  assert( type2 == xpointFault || type2 == xpointFaultLinked );

#if 0
if ( XPointOrgId(xp1) == MRH_FIND_POINT ) {
  fprintf( stderr, "LinkFaultPoint\n" );
  XPointSetPrintPoint( xp1, stderr );
  XPointSetPrintPoint( xp2, stderr );
  fprintf( stderr, "\n" );
}
#endif

  if ( xp1Link->offset > xp2Link->offset ) {
  /* Link faultpoint1 to faultpoint2 */
  xp1Link->offset = xp2Link->offset;
  xp1Link->type = xpointFaultLinked;
  numChange += 1;
  } else if ( xp1Link->offset < xp2Link->offset ) {
  xp2Link->offset = xp1Link->offset;
  xp2Link->type = xpointFaultLinked;
  } else {
  /* Already linked... */
  }
  return numChange;
}

extern int XPointSetLinkFault(
                  XPointSet_t                *xpointSet,
                  int                         orgId,
                  const Surface_t            *fault1,
                  int                         triangleId1,
                  const Surface_t            *fault2,
                  int                         triangleId2 )
{
  int                 numChange = 0;
  XPoint_t           *xp1 = XPointSetFind( xpointSet, orgId, fault1, triangleId1 );
  XPoint_t           *xp2 = XPointSetFind( xpointSet, orgId, fault2, triangleId2 );
  const XPointType_t  type1 = XPointType( xp1 );
  const XPointType_t  type2 = XPointType( xp2 );

  assert( SurfaceType( fault1 ) != surfNormal );
  assert( SurfaceType( fault2 ) != surfNormal );

  if ( type1 == xpointFaultUnshifted ) {
  assert( XPointOrgId( xp1 ) == XPointOrgId( xp2 ) );
  numChange += XPointSetUnshift( xpointSet, XPointOrgId( xp2 ), fault2, triangleId2 );
  } else if ( type2 == xpointFaultUnshifted ) {
  assert( XPointOrgId( xp1 ) == XPointOrgId( xp2 ) );
  numChange += XPointSetUnshift( xpointSet, XPointOrgId( xp1 ), fault1, triangleId1 );
  } else {
  numChange += LinkFaultPoint( xpointSet, xp1, xp2 );
  }
  return numChange;
}

extern int XPointSetLinkNormal(
                  XPointSet_t                *xpointSet,
                  int                         orgId,
                  const Surface_t            *normal,
                  int                         triangleId,
                  const XPoint_t             *xpFault )
{
  int               numChange = 0;
  XPoint_t          xpMem, *xp = XPointInit( orgId, normal, triangleId, &xpMem );
  XPoint_t         *xpNormal = RBTreeFindGet( xpointSet->extraPoints, xp );

  assert( xpFault && SurfaceType( normal ) == surfNormal );
  if ( !xpNormal ) {
  const int ofset = xpFault->offset; /* Beware of realloc that invalidates xpFault */
  xpNormal = RBTreeSearchGet( xpointSet->extraPoints, xp );
  numChange += 1;
  xpNormal->type = xpointNormal2Fault;
  xpNormal->offset = ofset;
#if 0
if ( orgId == MRH_FIND_POINT ) {
fprintf( stderr, "XPointSetLinkNormal orgId=%d triangleId=%d\n", orgId, triangleId );
SurfaceInfoPrint( normal, stderr );
fprintf( stderr, "\n\n" );
}
#endif
  } else {
  /* Already linked, but also to the same fault point? */
  const XPoint_t  *xp1Link = XPointSetFollowLink( xpointSet, xpFault );
  const XPoint_t  *xp2Link = XPointSetFollowLink( xpointSet, xpNormal );
  if ( xp1Link != xp2Link ) {
      const XPointType_t  type1 = XPointType( xp1Link );
      const XPointType_t  type2 = XPointType( xp2Link );
      if ( ( type1 == xpointFault ) && ( type2 == xpointFault ) ) {
    assert( xp1Link->orgId == xp2Link->orgId );
    /* Two fault points are identical to the same point in a normal surface: link them */
    numChange += XPointSetLinkFault( xpointSet, 
                                         xp1Link->orgId,
                                         xp1Link->surf,
                                         xp1Link->triangleId,
                                         xp2Link->surf,
                                         xp2Link->triangleId );
      } else if ( ( type1 == xpointFaultUnshifted ) && ( type2 == xpointFaultUnshifted ) ) {
    /* ready */
      } else if ( ( type2 == xpointFaultUnshifted ) && ( type1 == xpointFault ) ) {
    /* Unshift 1 */
    numChange += XPointSetUnshift( xpointSet, XPointOrgId(xp1Link), xp1Link->surf, xp1Link->triangleId );
      } else if ( ( type1 == xpointFaultUnshifted ) && ( type2 == xpointFault ) ) {
    /* Unshift 2 */
    numChange += XPointSetUnshift( xpointSet, XPointOrgId(xp2Link), xp2Link->surf, xp2Link->triangleId );
      } else {
    assert( 0 );
      }
  }
  }
  return numChange;
}


static int LinkNormalToLinkedNormal(
                  int                         orgId,
                  const Surface_t            *normal,
                  int                         triangleId,
                  const XPoint_t             *xp,
                  XPointSet_t                *xpointSet )
{
  int               numChange = 0;
  /* Find fault point to which it is linked */
  xp= XPointSetFollowLink( xpointSet, xp );
  switch ( XPointType( xp ) ) {
  case xpointFaultUnshifted:
      /* Ready */
      break;
  case xpointFault:
      /* Link normal to fault point */
      numChange += XPointSetLinkNormal( xpointSet, orgId, normal, triangleId, xp );
      break;
  case xpointFaultLinked: 
  case xpointNormal2Fault: 
  default:
      assert( FALSE );
      break;
  }
  return numChange;
}

/* Link points in 2 normal surfaces */
extern int XPointSetLinkNormal2(
                  XPointSet_t                *xpointSet,
                  int                         orgId,
                  const Surface_t            *normal1,
                  int                         triangleId1,
                  const Surface_t            *normal2,
                  int                         triangleId2 )
{
  int               numChange = 0;

  XPoint_t          xpMem;
  XPoint_t         *xpNormal1 = RBTreeFindGet( xpointSet->extraPoints, XPointInit( orgId, normal1, triangleId1, &xpMem ) );
  XPoint_t         *xpNormal2 = RBTreeFindGet( xpointSet->extraPoints, XPointInit( orgId, normal2, triangleId2, &xpMem ) );

  assert( SurfaceType( normal1 ) == surfNormal );
  assert( SurfaceType( normal2 ) == surfNormal );

  if ( !xpNormal1 && !xpNormal2 ) {
  /* Nothing to be done */
  } else if ( xpNormal1 && !xpNormal2 ) {
  numChange += LinkNormalToLinkedNormal( orgId, normal2, triangleId2, xpNormal1, xpointSet );
  } else if ( xpNormal2 && !xpNormal1 ) {
  numChange += LinkNormalToLinkedNormal( orgId, normal1, triangleId1, xpNormal2, xpointSet );
  } else {
  const XPoint_t  *xp1Link = XPointSetFollowLink( xpointSet, xpNormal1 );
  const XPoint_t  *xp2Link = XPointSetFollowLink( xpointSet, xpNormal2 );

  numChange += XPointSetLinkFault( xpointSet, orgId, XPointSurface(xp1Link), XPointTriangleId(xp1Link),
                                                       XPointSurface(xp2Link), XPointTriangleId(xp2Link) );
  }
  return numChange;
}

static void XPointSetPrintFaultPoint(
                  const XFaultPoint_t        *fault,
                  FILE                       *file )
{
  fprintf( file, " pointId=%d   shift=(%f, %f, %f)", 
     fault->pointId, fault->shift[0], fault->shift[1], fault->shift[2] );
}

static void XPointSetPrintNormalPoint(
                  const XNormal2Fault_t      *normal,
                  FILE                       *file )
{
  UNUSED(normal);
  UNUSED(file);
}

extern void XPointSetPrintPoint(
                  const XPoint_t             *xp,
                  FILE                       *file )
{
  char   buf[ BUFSIZ ];
  fprintf( file, "ExtraPoint orgId=%d offset=%d type=%s surf=%s triangleId=%d", xp->orgId, xp->offset, XPointTypeToString[ xp->type ], SurfaceUserInfo( buf, xp->surf ), xp->triangleId );

  switch ( XPointType( xp ) ) {
  case xpointFault:
  case xpointFaultUnshifted:
  case xpointFaultLinked:
      XPointSetPrintFaultPoint( &xp->data.faultPoint, file );
      break;
  case xpointNormal2Fault:
      XPointSetPrintNormalPoint( &xp->data.normalPoint, file );
      break;
  default:
      assert( FALSE );
      break;
  }
  fprintf( file, "\n" );
}

extern void XPointSetPrint(
                  XPointSet_t                *xpointSet,
                  FILE                       *file )
{
  Iterator_t       iter;
  const XPoint_t  *xp = XPointSetFirst( xpointSet, &iter );
  fprintf( file, "\nExtraPointSet size=%d\n\n", XPointSetSize( xpointSet ) );
  while ( xp ) {
  XPointSetPrintPoint( xp, file );
  xp = XPointSetNext( xpointSet, &iter );
  }
}


extern bool_t XPointSetPointInAnyFault(
                  const XPointSet_t          *xpointSet, 
                  int                         orgId )
{
  return ( XPointSetFind( xpointSet, orgId, NULL, 0 ) != NULL );
}

static bool_t XPointSetTestShift(
                  const double               *shift,
                  PointSet_t                 *pointSet, 
                  XPointSet_t                *xpointSet, 
                  Set_t                      *xpoints )
{
  bool_t               isOK = TRUE;
  Iterator_t           iter;
  const XPoint_t     **xp = SetFirst( xpoints, &iter );
  UNUSED(xpointSet);
  while ( xp ) {
  const XPoint_t      *xpMatch = *xp;
  if ( XPointType( xpMatch ) != xpointNormal2Fault ) {
      const Triangle_t    *triangle = SurfaceGet( xpMatch->surf, xpMatch->triangleId );
      const XFaultPoint_t *xfp = &(xpMatch->data.faultPoint);
      double               normal[3];
      TriangleUnitNormal( triangle, pointSet, normal );
      if ( VecInprod( xfp->shift, shift, 3 ) <= 0 ) {
    isOK = FALSE;    
    break;      
      }
  }
  xp = SetNext( xpoints, &iter );
  }
  return isOK;
}

static void XPointGetCleverShift( 
                  double                     *shift,
                  PointSet_t                 *pointSet, 
                  XPointSet_t                *xpointSet, 
                  Set_t                      *xpoints )
{
  PointSet_t       *directionSet = PointSetCreate( DBL_EPSILON );
  Surface_t        *triangleSet = SurfaceCreate( 0, 0, surfNormal, frontUnknownDir, "directionSpace", surfOuterUnknown);
  double            maxArea = sqrt( DBL_EPSILON ); /* lower bound on area */
  Iterator_t        iter;
  int               maxAreaId = -1, n;
  XPoint_t        **xpp = NULL;

  /* Create cube around origin */
  const int p0 = PointSetAdd( directionSet, -1, -1, -1 );
  const int p1 = PointSetAdd( directionSet,  1, -1, -1 );
  const int p2 = PointSetAdd( directionSet,  1,  1, -1 );
  const int p3 = PointSetAdd( directionSet, -1,  1, -1 );

  const int p4 = PointSetAdd( directionSet, -1, -1,  1 );
  const int p5 = PointSetAdd( directionSet,  1, -1,  1 );
  const int p6 = PointSetAdd( directionSet,  1,  1,  1 );
  const int p7 = PointSetAdd( directionSet, -1,  1,  1 );

  UNUSED(pointSet);
  UNUSED(xpointSet);

  /* Cover with triangles */
  SurfaceAdd( triangleSet, TriangleCreateFromPoints( p0, p1, p2, -1 ) );
  SurfaceAdd( triangleSet, TriangleCreateFromPoints( p0, p3, p2, -1 ) );

  SurfaceAdd( triangleSet, TriangleCreateFromPoints( p4, p5, p6, -1 ) );
  SurfaceAdd( triangleSet, TriangleCreateFromPoints( p4, p7, p6, -1 ) );

  SurfaceAdd( triangleSet, TriangleCreateFromPoints( p0, p3, p7, -1 ) );
  SurfaceAdd( triangleSet, TriangleCreateFromPoints( p0, p4, p7, -1 ) );

  SurfaceAdd( triangleSet, TriangleCreateFromPoints( p1, p2, p6, -1 ) );
  SurfaceAdd( triangleSet, TriangleCreateFromPoints( p1, p5, p6, -1 ) );

  SurfaceAdd( triangleSet, TriangleCreateFromPoints( p1, p5, p0, -1 ) );
  SurfaceAdd( triangleSet, TriangleCreateFromPoints( p4, p5, p0, -1 ) );

  SurfaceAdd( triangleSet, TriangleCreateFromPoints( p2, p3, p6, -1 ) );
  SurfaceAdd( triangleSet, TriangleCreateFromPoints( p7, p3, p6, -1 ) );

  /* Restrict surface in direction space */
  xpp = SetFirst( xpoints, &iter );
  while ( xpp ) {
  XPoint_t            *xp = *xpp;
  double               org[] = { 0, 0, 0 };
  const XFaultPoint_t *xfp = &(xp->data.faultPoint);
  Surface_t           *intersect = SurfaceIntersectPlane( triangleSet, directionSet, org, xfp->shift );
  SurfaceDelete( triangleSet );
  triangleSet = intersect;
  xpp = SetNext( xpoints, &iter );
  }

  /* Find largest intersection triangle */
  for ( n = 0; n < SurfaceSize( triangleSet ); n++ ) {
  const double triangleArea = TriangleArea( SurfaceGet( triangleSet, n ), directionSet );
  if ( triangleArea > maxArea ) {
      maxArea = triangleArea;
      maxAreaId = n;
  }
  }
 
  /* Use mid-point of largest triangle for shift */
  if ( maxAreaId >= 0 ) {
  TriangleCenter( SurfaceGet( triangleSet, maxAreaId ), directionSet, shift );
  UtilNormalize( shift, NULL, 3 );
  } else {
  /* Zero shift, this is an error */
  UTIL_SET( shift, 0, 3 );
  }

  SurfaceDelete( triangleSet );
  PointSetDelete( directionSet );
}

/*
 * XPointGetShift: detrmone the shift of a fault point, given the triangles(+surfaces) connected
 * to it. We require that the shift has a positive inproduct with all triangle shifts(=normal).
 * Inituitively this means that the point is shifted in the 'right' direction with respect to all planes.
 * We achieve this by considering all triangle normal as constraints, and the shift should fullfill all constraints.
 * Each triangle selects a part of the unit globe of 3D directions, thus defining a connected subdomain of the
 * unit globe. The implementation is facilitated by not using a unit globe, but a cube (no periodic boundary conditions,
 * simple plane/triangle intersections only). Unfortunately we cannot be sure if there are no intersecting triangles
 * after shifting, but this seems the best we can do...
 */
extern double *XPointGetShift( 
                  PointSet_t                 *pointSet, 
                  XPointSet_t                *xpointSet, 
                  const XPoint_t             *xp, 
                  double                      shiftScaleFactor,
                  double                     *shift )
{
  const XPoint_t      *xpLink  = XPointSetFollowLink( xpointSet, xp ), *xpMatch = NULL;
  const int            orgId = XPointOrgId( xp );
  XPoint_t             xpSearch;
  Iterator_t           iter;
  double               minimumLength = DBL_MAX;
  Set_t               *xpoints = SetCreate( 10, sizeof(XPoint_t *) );

  /* Loop over linked points */
  XPointInit( orgId, NULL, 0, &xpSearch );

  xpMatch = RBTreeFirstMatch( xpointSet->extraPoints, &xpSearch, &iter );
  while ( xpMatch ) {
  const XPointType_t   type = XPointType( xpMatch );
  if ( type != xpointNormal2Fault && XPointSetFollowLink( xpointSet, xpMatch ) == xpLink ) {
      const XFaultPoint_t *xfp = &(xpMatch->data.faultPoint);
      assert( type == xpointFault || type == xpointFaultLinked );
      SetAdd( xpoints, &xpMatch, 1 );
      minimumLength = MIN( minimumLength, VecL2Norm( xfp->shift, 3 ) );
  }
  xpMatch = RBTreeNextMatch( xpointSet->extraPoints, &xpSearch, &iter );
  }

  /* Determine shift */
  XPointGetCleverShift( shift, pointSet, xpointSet, xpoints );
  if( XPointSetTestShift( shift, pointSet, xpointSet, xpoints ) ) {
  /* Length of shift: factor times minimum */
  assert( minimumLength > DBL_MIN );
  VecOperScal( shift, EqlPos, shift, OperTimes, shiftScaleFactor*minimumLength, 3 ); 
  } else {
  UTIL_SET( shift, 0., 3 );
  }

  SetDelete( xpoints );
  return shift;
}
