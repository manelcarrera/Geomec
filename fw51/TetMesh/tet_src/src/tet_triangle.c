 /*                                         Copyright (c) 2011 TNO DIANA BV */
#include "tet_point.h"
#include "tet_triangle.h"
#include "tet_mesh.h"
#include "float.h"

enum {
   posUndefined = -1,
   posLeft,
   posMiddle,
   posRight,
   posLast
};

static double Norm3D(
                     const double           *p )
{
   return sqrt( p[0]*p[0] + p[1]*p[1] + p[2]*p[2] );
}

static  Triangle_t *TriangleStandardize( Triangle_t *triangle )
{
   int  v[3], minVertex, minPos=-1, d;
   UTIL_COPY( v, triangle->vertex, 3 );
   /* Find position of smallest coordinate */
   UTIL_GET_MINIMUM( v, 3, &minVertex );
   for ( d = 0; d < 3; d++ ) {
      if ( v[d] == minVertex ) {
         minPos = d;
      }
   }
   assert( minPos >= 0 );

   /* Put smallest coordinate first */
   for ( d = 0; d < 3; d++ ) {
      triangle->vertex[d] = v[(d + minPos)%3];
   }
   return triangle;
}

extern int TriangleInterfaceIdx( const Triangle_t *triangle )
{
  return triangle->interfaceIdx;
}

extern Triangle_t *TriangleCreate( const int *pointIdList, int interfaceIdx )
{
   Triangle_t *result = DIMALLOC( sizeof(*result) );
   result->interfaceIdx = interfaceIdx;
   TriangleChange( result, pointIdList );
   return result;
}

extern Triangle_t  *TriangleCreateFromPoints( 
                  int                         p0, 
                  int                         p1, 
                  int                         p2,
                  int                         interfaceIdx )
{
  int list[3];
  list[0] = p0;
  list[1] = p1;
  list[2] = p2;
  return TriangleCreate( list, interfaceIdx );
}

extern Triangle_t *TriangleChange( Triangle_t *triangle, const int *pointIdList )
{
   UTIL_COPY( triangle->vertex, pointIdList, 3 );
   TriangleStandardize( triangle );
   return triangle;
}

extern Triangle_t *TriangleDelete( Triangle_t *triangle )
{
   DIFREE( triangle );
   return NULL;
}

extern int TriangleSize( const Triangle_t *triangle )
{
   UNUSED(triangle);
   return 3;
}

extern const int *TrianglePoints( const Triangle_t *triangle )
{
   return triangle->vertex;
}

extern Triangle_t *TriangleCopy( const Triangle_t *triangle )
{
   return TriangleCreate( TrianglePoints( triangle ), TriangleInterfaceIdx( triangle ) );
}

extern int TriangleComp( const void *triangle0V, const void *triangle1V )
{
   const Triangle_t *triangle0 = triangle0V;
   const Triangle_t *triangle1 = triangle1V;
   int               result = UTIL_COMPARE( triangle0->vertex[0], triangle1->vertex[0] );
   if ( !result ) {
      const int min0 = MIN( triangle0->vertex[1], triangle0->vertex[2] );
      const int min1 = MIN( triangle1->vertex[1], triangle1->vertex[2] );
      /* Triangle must be standardized! */
      assert( triangle0->vertex[0] < min0 );
      assert( triangle1->vertex[0] < min1 );
      result = UTIL_COMPARE( min0, min1 );
      if ( !result ) {
         const int max0 = MAX( triangle0->vertex[1], triangle0->vertex[2] );
         const int max1 = MAX( triangle1->vertex[1], triangle1->vertex[2] );
         result = UTIL_COMPARE( max0, max1 );
      }
   }
   return result;
}

extern void TrianglePrint( const Triangle_t *triangle, FILE *fp )
{
   const int     *pointId = TrianglePoints( triangle );
   const size_t   len = TriangleSize( triangle );
   size_t         n;
   fprintf( fp, "TRIANGLE: " );
   for ( n = 0; n < len; n++ ) {
      fprintf( fp, "  %d", pointId[n] );
   }
   fprintf( fp, "\n" );
}


extern const char *TriangleSPrint( char *buf, const Triangle_t *triangle, PointSet_t *pointSet )
{
   const int     *pointId = TrianglePoints( triangle );
   char           pointBuf0[ BUFSIZ ], pointBuf1[ BUFSIZ ], pointBuf2[ BUFSIZ ];
   sprintf( buf, "%s\n%s\n%s\n", 
                   PointSetSPrint( pointBuf0,  pointSet, pointId[ 0 ] ),
                   PointSetSPrint( pointBuf1,  pointSet, pointId[ 1 ] ),
                   PointSetSPrint( pointBuf2,  pointSet, pointId[ 2 ] ));
   return buf;
}

extern Triangle_t *TriangleFlip( Triangle_t *triangle )
{
   UTIL_REVERSE( triangle->vertex, 3, int );
   TriangleStandardize( triangle );
   return triangle;
}

extern void TriangleSide( const Triangle_t *triangle, int s, int *p1, int *p2 )
{
   const int  nPoint = TriangleSize( triangle );
   const int *points = TrianglePoints( triangle );
   assert( s >= 0 && s < nPoint );
   *p1 = points[s];
   *p2 = ( s+1 < nPoint ) ? points[s+1] : points[0];
}

extern void TriangleNormal( const Triangle_t *triangle, PointSet_t *pointSet, double *normal )
{
   int      p1, p2, p3, p4;
   double   c1[3], c2[3], c3[3], c4[3];
   double   d1[3], d2[3];
   TriangleSide( triangle, 0, &p1, &p2 );
   TriangleSide( triangle, 1, &p3, &p4 );
   assert( p2 == p3 );
   PointSetFind( pointSet, p1, c1+0, c1+1, c1+2 );
   PointSetFind( pointSet, p2, c2+0, c2+1, c2+2 );
   PointSetFind( pointSet, p3, c3+0, c3+1, c3+2 );
   PointSetFind( pointSet, p4, c4+0, c4+1, c4+2 );
   VecOperVec( d1, EqlPos, c2, OperMinus, c1, 3 );
   VecOperVec( d2, EqlPos, c4, OperMinus, c3, 3 );
   OuterProduct( d1, d2, normal );
}


extern bool_t TriangleUnitNormal( const Triangle_t *triangle, PointSet_t *pointSet, double *unitNormal )
{
   TriangleNormal( triangle, pointSet, unitNormal );
   return UtilNormalize( unitNormal, NULL, 3 );
}

extern void TriangleCenter( const Triangle_t *triangle, 
                           PointSet_t *pointSet, double *center )
{
   const int *pointId = TrianglePoints( triangle );
   const int  len = TriangleSize( triangle );
   const double invLen = 1./len;
   int        n;

   UTIL_SET( center, 0, 3 );
   for ( n = 0; n < len; n++ ) {
      double    p[3];
      PointSetFind( pointSet, pointId[n], p+0, p+1, p+2 );
      VecOperScal( center, EqlAdd, p, OperTimes, invLen, 3 );
   }
}

extern int TrianglePointPosition( const Triangle_t *triangle, 
             PointSet_t *pointSet, const double eps, const double *point )
{
   double   unitNormal[3], center[3], dif[3], inp;
   int      result;
   TriangleUnitNormal( triangle, pointSet, unitNormal );
   TriangleCenter( triangle, pointSet, center );
   VecOperVec( dif, EqlPos, point, OperMinus, center, 3 );
   inp = VecInprod( dif, unitNormal, 3 );
   assert( eps >= -0.01 );
   if ( inp >= eps ) {
      result = 1;
   } else if ( inp < -eps ) {
      result = -1;
   } else {
      result = 0;
   }
   return result;
}

extern BoundBox_t *TriangleBoundBox( 
                     const Triangle_t       *triangle,
                     PointSet_t             *pointSet,
                     BoundBox_t             *boundBox )
{
   const int *id = TrianglePoints( triangle );
   int        p;
   for ( p = 0; p < 3; p++ ) {
      double   point[3];
      PointSetFind( pointSet, id[p], point+0, point+1, point+2 );
      BoundBoxAdd( boundBox, point );
   }
   return boundBox;
}

extern double TriangleArea( 
                     const Triangle_t       *triangle,
                     PointSet_t             *pointSet )
{
   double    normal[ 3 ];
   TriangleNormal( triangle, pointSet, normal );
   return ( 0.5*VecL2Norm( normal, 3 ) );
}


/*
 * Determine the distance of a point to the line determined by normal vector
 * and reference point in 2D
 */
static double PointDistancePlane2D(
                     const double           *refPoint,
                     const double           *normal,
                     const double           *point )
{
   return normal[0]*(point[0]-refPoint[0]) + 
          normal[1]*(point[1]-refPoint[1]);
}

/*
 * Determine the distance of a point to the plane determined by normal vector
 * and reference point in 3D
 */
static double PointDistancePlane3D(
                     const double           *refPoint,
                     const double           *normal,
                     const double           *point )
{
   return normal[0]*(point[0]-refPoint[0]) + 
          normal[1]*(point[1]-refPoint[1]) +
          normal[2]*(point[2]-refPoint[2]);
}

#define DistToPos( dist, eps ) ( ABS((dist)) <= (eps) ? posMiddle : ( dist > 0 ? posRight : posLeft ) )
#define PointOnMiddle( dist, eps ) ( DistToPos( dist, eps ) == posMiddle )

static int LineIntersectPlane(
                     const double           *planePoint,
                     const double           *unitNormal,
                     const double           *linePoint1,
                     const double           *linePoint2,
                     const double            eps,
                     double                 *pointInPlane )
{
   /* Distance of linePoints to plane */
   const double dist1 = PointDistancePlane3D( planePoint, unitNormal, linePoint1 ),
                dist2 = PointDistancePlane3D( planePoint, unitNormal, linePoint2 );
   int          nInPlane = 0;

   /* Point1 on plane ? */
   if ( PointOnMiddle( dist1, eps ) ) {
      UTIL_COPY( pointInPlane, linePoint1, 3 );
      nInPlane += 1;
   }

   /* Point2 on plane ? */
   if ( PointOnMiddle( dist2, eps ) ) {
      UTIL_COPY( pointInPlane, linePoint2, 3 );
      nInPlane += 1;
   }

   if ( !nInPlane ) {
      if ( dist1 * dist2 < 0 ) {
         /* Points on opposite side of plane */
         const double difDist = dist1 - dist2;
         const double lambda = ABS(dist1)/ABS(difDist);
         UTIL_COPY( pointInPlane, linePoint1, 3 );
         VecTimesScal2( pointInPlane, 1-lambda, linePoint2, lambda, 3 );
         nInPlane += 1;
      }
   }

   return nInPlane;
}

static void OrthoNormalAxes(
                     const double           *normal0,
                     double                 *normal1,
                     double                 *normal2 )
{
   int     d;
   int     dMin = -1;
   double  inpMin = 1.;
   double  inp;

   /* Find a vector that is not on line with normal0 */
   for ( d= 0; d < 3; d++ )
   {
      inp = ABS( normal0[ d ] );

      if ( inp < inpMin )
      {
         dMin = d;
         inpMin = inp;
      }
   }
   assert( dMin >= 0 );

   /* First unit normal vector */
   UTIL_SET( normal1, 0, 3 );
   normal1[ dMin ] = 1.0;

   inp = normal0[ dMin ];

   VecOperScal( normal1, EqlAdd, normal0, OperTimes, -inp, 3 );
   UtilNormalize( normal1, NULL, 3 );

   /* The second one is outer product; always unit length */
   OuterProduct( normal0, normal1, normal2 );
}

static void ProjectPointOnPlane2D(
                     const double           *refPoint,
                     const double           *normal1,
                     const double           *normal2,
                     const double           *point3D,
                     double                 *point2D )
{
   double p[3];
   /* Shift */
   VecOperVec( p, EqlPos, point3D, OperMinus, refPoint, 3 );
   /* Get coordinates */
   point2D[ 0 ] = VecInprod( p, normal1, 3 );
   point2D[ 1 ] = VecInprod( p, normal2, 3 );
}

/* 
 *  Return normal to line in 2D, return value indicates degenaracy
 */
static bool_t LineNormal2D(
                     const double           *p0,
                     const double           *p1,
                     double                 *normal,
                     const double            smallLength )
{
   double  rvec[ 2 ], dist;
   bool_t  isDegenerate;
   /* Direction vector from l0 to l1 */
   rvec[ 0 ] = p1[ 0 ] - p0[ 0 ];
   rvec[ 1 ] = p1[ 1 ] - p0[ 1 ];
   normal[ 0 ] = -rvec[ 1 ];
   normal[ 1 ] =  rvec[ 0 ];
   /* Normalize for proper distance computaion */
   isDegenerate = UtilNormalize( normal, &dist, 2 );
   if ( !isDegenerate ) {
      isDegenerate = PointOnMiddle( dist, smallLength );
   }
   return isDegenerate;
}
  

/*
 * Test if point lies between line0 and line1.
 * The point **must** lie on the line thorugh line0 and line1;
 * this is not tested.
 */
static bool_t PointOnLine2D(
                     const double           *line0,
                     const double           *line1,
                     const double           *point )
{
   bool_t        isBetween = FALSE;
   double        l0[2], l1[2], p[2], lambda;
   VecOperVec( l0, EqlPos, line0, OperMinus, line0, 2 );
   VecOperVec( l1, EqlPos, line1, OperMinus, line0, 2 );
   VecOperVec( p,  EqlPos, point, OperMinus, line0, 2 );
   lambda = ( VecInprod( l1, p, 2 ) / VecInprod( l1, l1, 2 ) );
   isBetween = ( lambda >= 0 && lambda <= 1 );
   return isBetween;
}

/*
 * Determine the position of point wrt to the line through line0 to line1.
 * If on the same size as refPoint, then return posRight else posLeft.
 */
static int PointPositionLine2D(
                     const double           *line0,
                     const double           *line1,
                     const double           *refPoint,
                     const double           *point,
                     const double            smallLength )
{
   int position = posUndefined;
   double  normal[ 2 ], dist;
   bool_t  isDegenerate = LineNormal2D( line0, line1, normal, smallLength );

   assert( !isDegenerate );
   dist = PointDistancePlane2D( line0, normal, point );
   if ( PointOnMiddle( dist, smallLength ) ) {
      position = posMiddle;
   } else {
      const double   distRef = PointDistancePlane2D( line0, normal, refPoint );
      assert( distRef != 0. );
      if ( distRef*dist >= 0. ) {
         position = posRight;
      } else {
         position = posLeft;
      }
   }
   return position;
}


static bool_t PointInTriangle2D( 
                     const double           *t0,
                     const double           *t1,
                     const double           *t2,
                     const double           *point,
                     const double            smallLength )
{
   int pos;
   bool_t   intersect = TRUE;

   if ( intersect ) {
      pos = PointPositionLine2D( t0, t1, t2, point, smallLength );
      intersect = ( pos != posLeft );
   }
   if ( intersect ) {
      pos = PointPositionLine2D( t0, t2, t1, point, smallLength );
      intersect = ( pos != posLeft );
   }
   if ( intersect ) {
      pos = PointPositionLine2D( t1, t2, t0, point, smallLength );
      intersect = ( pos != posLeft );
   }
   return intersect;
}

/*
 * Test if 2 non-degenerate lines in 2D have at least 1 point in common.
 */
static bool_t LineIntersectLine2D(
                     const double           *p0,
                     const double           *p1,
                     const double           *q0,
                     const double           *q1,
                     const double            smallLength )
{
   bool_t         intersect = FALSE;
   double         normal[ 2 ];
   bool_t         isDegenerate = LineNormal2D( p0, p1, normal, smallLength );
   const double   dist0 = PointDistancePlane2D( p0, normal, q0 ),
                  dist1 = PointDistancePlane2D( p0, normal, q1 );
   const int      pos0 = DistToPos( dist0, smallLength),
                  pos1 = DistToPos( dist1, smallLength);

   assert( !isDegenerate );
   if ( pos0 == posMiddle && pos1 == posMiddle ) {
      /* The 2 lines are colinear... test on bounding boxes */
      double      cor[3];
      BoundBox_t  bbp, bbq;
      
      BoundBoxInit( &bbp );
      BoundBoxInit( &bbq );
      cor[2] = 1.; /* Enlarge BB in 3rtd direction */
      UTIL_COPY( cor, p0, 2 ); BoundBoxAdd( &bbp, cor );
      UTIL_COPY( cor, q0, 2 ); BoundBoxAdd( &bbq, cor );
      cor[2] = -1.;
      UTIL_COPY( cor, p1, 2 ); BoundBoxAdd( &bbp, cor );
      UTIL_COPY( cor, q1, 2 ); BoundBoxAdd( &bbq, cor );
      
      intersect = BoundBoxOverlap( &bbp, &bbq );
   } else if ( pos0 == posMiddle ) {
      intersect = PointOnLine2D( p0, p1, q0 );
   } else if ( pos1 == posMiddle ) {
      intersect = PointOnLine2D( p0, p1, q1 );
   } else if ( pos0 != pos1 ){
         const double difDist = dist0 - dist1;
         double       lambda = ABS(dist0)/ABS(difDist);
         double       pp[2], qq[2], pointOnLine[2];
         UTIL_COPY( pointOnLine, q0, 2 );
         VecTimesScal2( pointOnLine, 1-lambda, q1, lambda, 2 );
         VecOperVec( pp, EqlPos, p1, OperMinus, p0, 2 );
         VecOperVec( qq, EqlPos, pointOnLine, OperMinus, p0, 2 );
         lambda = VecInprod( qq, pp, 2 ) / VecInprod( pp, pp, 2 );
         intersect = ( lambda >= 0. && lambda <= 1. );
   } else {
      /* Both q0 and q1 on same side of line p0->p1, no intersection possible */
   }
   return intersect;
}

/*
 * Test if line intersects with triangle in 3D. 
 * Line may be degenerated, if the triangle is degenerate no intersection is assumed!
 */
extern bool_t TriangleIntersectLine(
                     const Triangle_t       *triangle,
                     PointSet_t             *pointSet,
                     const double           *p1,
                     const double           *p2,
                     const double            smallLength )
{
   double       pa[3], unitNormal[ 3 ], pointInPlane[ 3 ];
   const int   *tPoints = TrianglePoints( triangle );
   int          nInPlane = 0;
   bool_t       intersect = FALSE;
   

   /* Get triangle point */
   PointSetFind( pointSet, tPoints[0], pa+0, pa+1, pa+2 );

   if ( !TriangleUnitNormal( triangle, pointSet, unitNormal ) ) {
      /* Does the line intersect with the plane through the triangle? */
      nInPlane = LineIntersectPlane( pa, unitNormal, p1, p2, smallLength, pointInPlane );
      if ( nInPlane ) {
         double       pb[3], pc[3], xAxis2D[ 3 ], yAxis2D[3 ],
                      pa2D[2], pb2D[2], pc2D[2];
   
         PointSetFind( pointSet, tPoints[1], pb+0, pb+1, pb+2 );
         PointSetFind( pointSet, tPoints[2], pc+0, pc+1, pc+2 );
   
         /* Orthonormal axis in the plane through triangle */
         OrthoNormalAxes( unitNormal, xAxis2D, yAxis2D );
            
         /* Reduce whole problem to 2D */
         ProjectPointOnPlane2D( pa, xAxis2D, yAxis2D, pa, pa2D );
         ProjectPointOnPlane2D( pa, xAxis2D, yAxis2D, pb, pb2D );
         ProjectPointOnPlane2D( pa, xAxis2D, yAxis2D, pc, pc2D );

         if ( nInPlane == 1 ) {
            /* Single intersection point between line and plane */
            double    point2D[2];
            /* Test if the intersectsection point is in 2D triangle */
            ProjectPointOnPlane2D( pa, xAxis2D, yAxis2D, pointInPlane, point2D );
            intersect = PointInTriangle2D( pa2D, pb2D, pc2D, point2D, smallLength );
         } else if ( nInPlane == 2 ) {
            double    la2D[2], lb2D[2];
            /* Whole line  is in plane through triangle, exceptional case */
            ProjectPointOnPlane2D( pa, xAxis2D, yAxis2D, p1, la2D );
            ProjectPointOnPlane2D( pa, xAxis2D, yAxis2D, p2, lb2D );
            /* Test if either of the 2 line points is inside the 2D triangle */
            intersect = PointInTriangle2D( pa2D, pb2D, pc2D, la2D, smallLength );
            if ( !intersect ) {
               intersect = PointInTriangle2D( pa2D, pb2D, pc2D, lb2D, smallLength );
            }
          
            if ( !intersect ) {
               /* Intersection only possible if 2D line intersect with any of the 3 triangle edges */
               double   dif[ 3 ], dist;
               VecOperVec( dif, EqlPos, p1, OperMinus, p2, 3 );
               dist = Norm3D( dif );
               if ( !PointOnMiddle( dist,  smallLength ) ) {
                  /* Line is not degenerate */
                  if ( !intersect ) {
                     intersect = LineIntersectLine2D( pa2D, pb2D, la2D, lb2D, smallLength );
                  }
                  if ( !intersect ) {
                     intersect = LineIntersectLine2D( pb2D, pc2D, la2D, lb2D, smallLength );
                  }
                  if ( !intersect ) {
                     intersect = LineIntersectLine2D( pc2D, pa2D, la2D, lb2D, smallLength );
                  }
               }
            }
         }
      }
   } else {
      /* Triangle is degenerate, no intersection */
   }
    
   return intersect;
}

extern double TriangleAspectRatio( 
                     const Triangle_t       *triangle,
                     PointSet_t             *pointSet )
{
   int    side;
   double minSide = DBL_MAX, maxSide = -1;
   for ( side = 0; side < 3; side++ ) {
      int    p1, p2;
      double c1[ 3 ], c2[ 3 ], dif[ 3 ], sideLength;
      TriangleSide( triangle, side, &p1, &p2 );
      PointSetFind( pointSet, p1, c1+0, c1+1, c1+2 );
      PointSetFind( pointSet, p2, c2+0, c2+1, c2+2 );
      VecOperVec( dif, EqlPos, c2, OperMinus, c1, 3 );
      sideLength = Norm3D( dif );
      minSide = MIN( minSide, sideLength );
      maxSide = MAX( maxSide, sideLength );
   }
   return ( (maxSide > DBL_MIN) ? minSide/maxSide : 0 );
}

static double TriangleAngle(
                     const Triangle_t       *triangle,
                     PointSet_t             *pointSet,
                     int                     corner )
{
  const int   c1 = ( corner > 0 ? corner-1 : 2 );
  const int   c2 = ( corner < 2 ? corner+1 : 0 );
  double      p1[3], p2[3], pc[3], d1[3], d2[3], inprod, len1, len2;

  UNUSED(triangle);

  PointSetFind( pointSet, c1,     p1+0, p1+1, p1+2 );
  PointSetFind( pointSet, c2,     p2+0, p2+1, p2+2 );
  PointSetFind( pointSet, corner, pc+0, pc+1, pc+2 );
  VecOperVec( d1, EqlPos, p1, OperMinus, pc, 3 );
  VecOperVec( d2, EqlPos, p2, OperMinus, pc, 3 );
  inprod = VecInprod( d1, d2, 3 );
  len1 = Norm3D( d1 ); len1 = MAX( len1, DBL_MIN );
  len2 = Norm3D( d2 ); len2 = MAX( len2, DBL_MIN );
  return ArcCosinus( ABS( inprod / len1 / len2 ) );
}

/* Return smallest angle in radians */
extern double TriangleSmallestAngle( 
                     const Triangle_t       *triangle,
                     PointSet_t             *pointSet )
{
  double minAngle = M_PI;
  int    corner;
  for ( corner = 0; corner < 3; corner++ ) {
    const double angle = TriangleAngle( triangle, pointSet, corner );
    minAngle = MIN( angle, minAngle );
  }
  return minAngle;
}

/* Find common side between 2 triangles, return TRUE if found, else FALSE */
extern bool_t TriangleCommonEdge( 
                     const Triangle_t       *triangle1,
                     const Triangle_t       *triangle2,
                     int                    *side1,
                     int                    *side2 )
{
  bool_t found = FALSE;
  for ( *side1 = 0; *side1 < 3; (*side1)++ ) {
    int p1_1, p1_2;
    TriangleSide( triangle1, *side1, &p1_1, &p1_2 );
    for ( *side2 = 0; *side2 < 3; (*side2)++ ) {
      int p2_1, p2_2;
      TriangleSide( triangle2, *side2, &p2_1, &p2_2 );
      found = MIN( p1_1, p1_2 ) == MIN( p2_1, p2_2 ) &&
              MAX( p1_1, p1_2 ) == MAX( p2_1, p2_2 );
      if ( found ) break;
    }
    if ( found ) break;
  }
  return found;
}

extern bool_t TriangleCommonPoint(
                     const Triangle_t       *triangle1,
                     const Triangle_t       *triangle2,
                     int                    *commonP )
{
  int               common = -1;
  const int        *points1 = TrianglePoints( triangle1 );
  const int        *points2 = TrianglePoints( triangle2 );
  int               n1, n2;

  for ( n1 = 0; n1 < 3 && common < 0; n1++ ) {
    for ( n2 = 0; n2 < 3 && common < 0; n2++ ) {
      if ( points1[n1] == points2[n2] ) {
        common = points1[n1];
      }
    }
  }
  if ( commonP ) *commonP = common;
  return ( common >= 0 );
}

/* Return angle between 2 triangles with common edge */
extern double TriangleAngleBetween( 
                     const Triangle_t       *triangle1,
                     const Triangle_t       *triangle2,
                     PointSet_t             *pointSet )
{
  int            side1, side2;
  int            p1, p2, other1, other2;
  double         xAxis2D[3], yAxis2D[3], point1[3], point2[3], unitEdge[3], refPoint[3], 
                 corOther1_3D[3], corOther1_2D[2], corOther2_3D[3], corOther2_2D[2];
  bool_t         isDegenerate;

  /* Find the common edge between the 2 triangles */
  const bool_t   commonEdge = TriangleCommonEdge( triangle1, triangle2, &side1, &side2 );
  assert( commonEdge );
  TriangleSide( triangle1, side1, &p1, &p2 );
  PointSetFind( pointSet, p1, point1+0, point1+1, point1+2 );
  PointSetFind( pointSet, p2, point2+0, point2+1, point2+2 );
   
  /* Orthonormal basis for plane perpendicular to edge */ 
  VecOperVec( unitEdge, EqlPos, point2, OperMinus, point1, 3 );
  isDegenerate = UtilNormalize( unitEdge, NULL, 3 );
  assert( !isDegenerate ); /* Triangle cannot be degenerate */
  OrthoNormalAxes( unitEdge, xAxis2D, yAxis2D );
            
  /* Reference point */
  VecOperVec( refPoint, EqlPos, point2, OperPlus, point1, 3 );
  VecOperScal( refPoint, EqlPos, refPoint, OperTimes, 0.5, 3 );

  /* Project other point from triangle 1 */
  TriangleLocate2( triangle1, p1, p2, &other1, NULL );
  PointSetFind( pointSet, other1, corOther1_3D+0, corOther1_3D+1, corOther1_3D+2 );
  ProjectPointOnPlane2D( refPoint, xAxis2D, yAxis2D, corOther1_3D, corOther1_2D );
  isDegenerate = UtilNormalize( corOther1_2D, NULL, 2 ); assert( !isDegenerate );

  /* Project other point from triangle 2 */
  TriangleLocate2( triangle2, p1, p2, &other2, NULL );
  PointSetFind( pointSet, other2, corOther2_3D+0, corOther2_3D+1, corOther2_3D+2 );
  ProjectPointOnPlane2D( refPoint, xAxis2D, yAxis2D, corOther2_3D, corOther2_2D );
  isDegenerate = UtilNormalize( corOther2_2D, NULL, 2 ); assert( !isDegenerate );

  return ArcCosinus( VecInprod( corOther1_2D, corOther2_2D, 2 ) );
}

extern void TriangleLocate2(
                  const Triangle_t           *triangle,
                  int                         p0,
                  int                         p1,
                  int                        *other,
                  bool_t                     *orient )
{
  const int        *points = TrianglePoints( triangle );
  int               n;

  *other = -1;

  for ( n = 0; n < 3; n++ ) {
    int   t0, t1;
    if ( other && points[ n ] != p0 && points[ n ] != p1 ) {
      *other = points[ n ];
    }
    if ( orient ) {
      TriangleSide( triangle, n, &t0, &t1 );
      if ( t0 == p0 && t1 == p1 ) {
        *orient = TRUE;
      } else if ( t0 == p1 && t1 == p0 ) {
        *orient = FALSE;
      }
    }
  }
}

/* Check if 2 triangles intersect: intersection if any side of either triangle intersects the other triangle */
static bool_t TriangleIntersect12( 
                  const Triangle_t           *triangle1, 
                  const Triangle_t           *triangle2, 
                  PointSet_t                 *pointSet )
{
  bool_t intersect = FALSE;
  int side2;
  for ( side2 = 0; side2 < 3; side2++ ) {
    int p1, p2;
    TriangleSide( triangle2, side2, &p1, &p2 );
    if ( TriangleIntersectLine( triangle1, pointSet, PointSetGet( pointSet, p1 ), 
                                                     PointSetGet( pointSet, p2 ), 
                                                     PointSetTolerance( pointSet ) / 10 ) ) {
      intersect = TRUE;
      break;
    }
  }
  return intersect;
}


extern bool_t TriangleIntersect( 
                  const Triangle_t           *triangle1, 
                  const Triangle_t           *triangle2, 
                  PointSet_t                 *pointSet )
{
  bool_t intersect = TriangleIntersect12( triangle1, triangle2, pointSet );
  if ( !intersect ) intersect = TriangleIntersect12( triangle2, triangle1, pointSet );
  return intersect;
}


extern bool_t TriangleContains( 
                  const Triangle_t           *triangle, 
                  int                         point )
{
  return ( triangle->vertex[0] == point || triangle->vertex[1] == point || triangle->vertex[2] == point );
}

extern bool_t TriangleDebug(
                  const Triangle_t           *triangle )
{
  bool_t   found = FALSE;
  if ( TriangleContains( triangle, 16863 ) && TriangleContains( triangle, 25524 )  && TriangleContains( triangle, 25519 ) ) {
    found = TRUE;
  }
  return found;
}

static void IntersectAddTriangle(
                  Surface_t                  *triangleSet, 
                  int                         p0,
                  int                         p1,
                  int                         p2,
                  int                         interfaceIdx )
{
  if ( p0 != p1 && p0 != p2 && p1 != p2 ) {
    SurfaceAdd( triangleSet, TriangleCreateFromPoints( p0, p1, p2, interfaceIdx ) );
  }
}

extern void TriangleIntersectPlane(
                  Surface_t                  *triangleSet, 
                  const Triangle_t           *triangle,
                  PointSet_t                 *pointSet,
                  const double               *planePoint,
                  const double               *unitNormal )
{
  int           n;
  const int    *points = TrianglePoints( triangle );
  const double *cor[3];
  int           pos[3];
  const double  eps = PointSetTolerance( pointSet );
  int           posCount[ posLast ];
  int           pointRight[3], pointLeft[3], pointMiddle[3];

  UTIL_SET( posCount, 0, posLast );
  for ( n = 0; n < TriangleSize( triangle ); n++ ) {
    double   dist;
    cor[n] = PointSetGet( pointSet, (TrianglePoints( triangle ))[n] );
    dist = PointDistancePlane3D( planePoint, unitNormal, cor[n] );
    pos[n] = DistToPos( dist, eps );
    
    switch ( pos[n] ) {
      case posMiddle:
        pointMiddle[posCount[pos[n]]] = n;
        break;
      case posLeft:
        pointLeft[posCount[pos[n]]] = n;
        break;
      case posRight:
        pointRight[posCount[pos[n]]] = n;
        break;
      default:
        assert( FALSE );
        break;
    }
    posCount[pos[n]] += 1;
  }

  if ( posCount[posMiddle] == 3 ) {
    /* Whole triangle in intersection */
    SurfaceAdd( triangleSet, TriangleCopy( triangle ) );
  } else if ( posCount[posMiddle] + posCount[posRight] == 3 ) {
    /* All on right side */
    SurfaceAdd( triangleSet, TriangleCopy( triangle ) );
  } else if ( posCount[posMiddle] + posCount[posLeft] == 3 ) {
    /* All on wrong side, ready */
  } else if ( posCount[posMiddle] == 1 ) {
    double       inter0[3];
    int          i0;
    assert( posCount[posLeft] == 1 && posCount[posRight] == 1 );
    LineIntersectPlane( planePoint, unitNormal, cor[pointRight[0]], cor[pointLeft[0]], eps, inter0 );
    i0 = PointSetAdd( pointSet, inter0[0], inter0[1], inter0[2] );
    IntersectAddTriangle( triangleSet, points[pointRight[0]], points[pointMiddle[0]], i0, TriangleInterfaceIdx( triangle ) );
  } else if ( posCount[posLeft]  == 2 ) {
    /* Single triangle on right side */
    double       inter0[3], inter1[3];
    int          i0, i1;
    LineIntersectPlane( planePoint, unitNormal, cor[pointRight[0]], cor[pointLeft[0]], eps, inter0 );
    LineIntersectPlane( planePoint, unitNormal, cor[pointRight[0]], cor[pointLeft[1]], eps, inter1 );
    i0 = PointSetAdd( pointSet, inter0[0], inter0[1], inter0[2] );
    i1 = PointSetAdd( pointSet, inter1[0], inter1[1], inter1[2] );
    IntersectAddTriangle( triangleSet, points[pointRight[0]], i0, i1, TriangleInterfaceIdx( triangle )  );
  } else if ( posCount[posRight]  == 2 ) {
    /* Two triangle on right side */
    double       inter0[3], inter1[3];
    int          i0, i1;
    LineIntersectPlane( planePoint, unitNormal, cor[pointRight[0]], cor[pointLeft[0]], eps, inter0 );
    LineIntersectPlane( planePoint, unitNormal, cor[pointRight[1]], cor[pointLeft[0]], eps, inter1 );
    i0 = PointSetAdd( pointSet, inter0[0], inter0[1], inter0[2] );
    i1 = PointSetAdd( pointSet, inter1[0], inter1[1], inter1[2] );
    IntersectAddTriangle( triangleSet, points[pointRight[0]], i1,  points[pointRight[1]], TriangleInterfaceIdx( triangle ) );
    IntersectAddTriangle( triangleSet, points[pointRight[0]], i1,  i0, TriangleInterfaceIdx( triangle ) );

  } else {
    assert( 0 );
  }
}

extern void TriangleMeshMessageAdd( 
                  void                       *meshV, 
                  PointSet_t                 *pointSet,
                  const Triangle_t           *triangle )
{
   char     buf[BUFSIZ];
   TriangleSPrint( buf, triangle, pointSet ); 
   MeshMessageAdd( meshV, buf );
}
