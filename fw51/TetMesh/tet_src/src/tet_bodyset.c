 /*                                         Copyright (c) 2011 TNO DIANA BV */
#include "float.h"
#include "tet_utils.h"
#include "tet_mesh.h"
#include "tet_bodyset.h"

extern BodySet_t *BodySetCreate( void )
{
   BodySet_t *result = DIMALLOC( sizeof(*result) );
   UTIL_ZERO( result );
   result->body = SetCreate( 0, sizeof( Body_t * ) );
   return result;
}

/* Clear the set, but do not delete the bodies themselves!  */
extern BodySet_t *BodySetClear( BodySet_t *bodySet )
{
   if ( bodySet ) {
      SetClear( bodySet->body );
   }
   return bodySet;
}

extern BodySet_t *BodySetDelete( BodySet_t *bodySet )
{
   if ( bodySet ) {
      Iterator_t   iter;
      Body_t   *body = BodySetFirst( bodySet, &iter );
      while ( body ) {
         BodyDelete( body );
         body = BodySetNext( bodySet, &iter );
      }
      SetDelete( bodySet->body );
      DIFREE( bodySet );
   }
   return bodySet;
}

extern void BodySetAdd( BodySet_t *bodySet, Body_t *body )
{
   SetAdd( bodySet->body, &body, 1 );
}

extern Body_t *BodySetGet( BodySet_t *bodySet, int id )
{
   Body_t   *result = NULL;
   assert( id >= 0 );
   if ( id < SetSize( bodySet->body ) ) {
      Body_t   **bodyP = SetElmGet( bodySet->body,  id );
      result = *bodyP;
   }
   return result;
}


extern Body_t *BodySetFirst( BodySet_t *bodySet, Iterator_t *iter )
{
   Body_t   **bodyP = SetFirst( bodySet->body, iter );
   return ( bodyP ? *bodyP : NULL );
}


extern Body_t *BodySetNext( BodySet_t *bodySet, Iterator_t *iter )
{
   Body_t   **bodyP = SetNext( bodySet->body, iter );
   return ( bodyP ? *bodyP : NULL );
}

static BodySet_t *BodySetPrintIdSet( BodySet_t *bodySet )
{
   Iterator_t  iter;
   Body_t     *body = BodySetFirst( bodySet, &iter );
   int         printId = 0;
   while ( body ) {
      printId += 1;
      BodyPrintIdSet( body, printId );
      body = BodySetNext( bodySet, &iter );
   }
  return bodySet;
}

extern void BodySetPrint( BodySet_t *bodySet, FILE *fp )
{
   Iterator_t  iter;
   Body_t     *body = BodySetFirst( BodySetPrintIdSet( bodySet ), &iter );
   fprintf( fp, "\nBODYSET number of bodies: %d\n\n", (int) SetSize(bodySet->body ) );
   while ( body ) {
      BodyPrint( body, fp );
      body = BodySetNext( bodySet, &iter );
   }
}

extern int BodySetSize( const BodySet_t *bodyset )
{
	return SetSize(bodyset->body);
}

extern BoundBox_t *BodySetBoundBox( 
               BodySet_t              *bodySet,
               PointSet_t             *pointSet,
               BoundBox_t             *boundBox ) 
{
   Iterator_t  iter;
   Body_t     *body = BodySetFirst( bodySet, &iter );
   while ( body ) {
      boundBox = BodyBoundBox( body, pointSet, boundBox );
      body = BodySetNext( bodySet, &iter );
   }
   return boundBox;
}

static bool_t BodyContainsFullTest(
               Body_t                 *bodyOuter,
               Body_t                 *bodyInner,
               PointSet_t             *pointSet,
               const double            smallLength )
{
   /* Build a tree of bounding boxes for all triangles of surfaces of outer body */
   BoundBoxTree_t *bbTree = BoundBoxTreeCreate();
   Iterator_t      bodyIter;
   BodySurface_t  *bs = BodyFirst( bodyOuter, &bodyIter );
   int             id = 0;
   bool_t          isNested = TRUE;
   double          maxSize = 0;
   Set_t          *intersect = SetCreate( 100, sizeof( int ) );
   Set_t          *outerTriangle = SetCreate( 100, sizeof( Triangle_t * ) );
   while ( bs ) {
      Surface_t   *surf = BodySurfaceGet( bs );
      int          triangleId;
      Iterator_t   surfIter;
      Triangle_t  *triangle = SurfaceFirst( surf, &triangleId, &surfIter );
      while ( triangle ) {
         BoundBox_t   bb;
         SetAdd( outerTriangle, &triangle, 1 );
         TriangleBoundBox( triangle, pointSet, BoundBoxInit( &bb ) );
         BoundBoxTreeAdd( bbTree, id, &bb );
         id += 1;
         triangle = SurfaceNext( surf, NULL, &surfIter );
      }
      bs = BodyNext( bodyOuter, &bodyIter );
   }

   /* Get length scale for the problem: diagonal of outer body bounding box */
   maxSize = 1.1*BoundBoxSize( BoundBoxTreeGetBoundBox( bbTree ) ) + 1;

  /* 
   * Outward normals to all triangles of the inner body should at least intersect 
   * a bounding box of some outer body triangle 
   */
   bs = BodyFirst( bodyInner, &bodyIter );
   while ( bs && isNested ) {
      Surface_t        *surf = BodySurfaceGet( bs );
      Iterator_t        surfIter;
      const Triangle_t *triangle = SurfaceFirst( surf, NULL, &surfIter );
      const int         signDir = ( BodySurfacePosDir( bs ) ? -1 : 1 );
      while ( triangle && isNested ) {
         double             center[3], normal[3], point[3];
         BoundBoxLineData_t lineData;
         TriangleCenter( triangle, pointSet, center );
         TriangleUnitNormal( triangle, pointSet, normal );
         UTIL_COPY( point, center, 3 );
         VecOperScal( point, EqlAdd, normal, OperTimes, signDir*maxSize, 3 );
         BoundBoxLineDataInit( &lineData, center, point );
         BoundBoxTreeFind( intersect, bbTree, &lineData, BoundBoxLineOverlap );
         if ( !SetSize( intersect ) ) {
            isNested = FALSE;
         } else {
            /* Full test for line-triangle intersection */
            bool_t     found = FALSE;
            const int *triangleId = SetGet( intersect );
            int        t;
            for ( t = 0; t < SetSize( intersect ) && !found; t++ ) {
               Triangle_t **outerTriangleP = SetElmGet( outerTriangle, triangleId[t] );
               found = TriangleIntersectLine( *outerTriangleP, pointSet, center, point, smallLength );
            }
            if (!found ) { 
               isNested = FALSE;
            }
         }
         triangle = SurfaceNext( surf, NULL, &surfIter );
      }
      bs = BodyNext( bodyInner, &bodyIter );
   }

   SetDelete( intersect );
   SetDelete( outerTriangle );
   BoundBoxTreeDelete( bbTree );
   return isNested;
}

static Set_t *BodyContainBodies(
               const int               bodyId,
               BoundBoxTree_t         *bbTree,
               BodySet_t              *bodySet,
               PointSet_t             *pointSet,
               const double            smallLength )
{
   Set_t     *result = SetCreate( 0, sizeof( int ) );
   Body_t    *bodyOuter = BodySetGet( bodySet, bodyId );
   if ( BodyTypeGet( bodyOuter ) != bodyDegenerate ) {
      /* Get the bounding box of this body */
      BoundBox_t  bbMem, *bb = BoundBoxTreeGet( bbTree, bodyId, &bbMem );
      /* Which bounding boxes does it contain? */
      Set_t      *work = BoundBoxTreeFind( NULL, bbTree, bb, BoundBoxOverlap );
      const int  *ids = SetGet( work );
      int         n;
      for ( n = 0; n < SetSize( work ); n++ ) {
         if ( ids[n] != bodyId ) {
            BoundBox_t  bbInnerMem, *bbInner = BoundBoxTreeGet( bbTree, ids[n], &bbInnerMem );
            if ( BoundBoxContainsBoundBox( bb, bbInner ) ) {
               Body_t    *bodyInner = BodySetGet( bodySet, ids[n] );
               /* If in same group never nested ?!?!? */
               if( BodyGroupGet( bodyOuter ) != BodyGroupGet( bodyInner ) ) {
                  if ( BodyContainsFullTest( bodyOuter, bodyInner, pointSet, smallLength ) ) {
                     SetAdd( result, ids+n, 1 );
                  }
               }
            }
         }
      }
      SetDelete( work );
   }
   return result;
}

/* Lovely recursive routine to find body nesting */
static void GetHierarchy( 
               void                   *mesh,
               BodySet_t              *bodySet,
               const int               bodyId,
               const int               outerBodyId,
               PointSet_t             *pointSet,
               BoundBoxTree_t         *bbTree,
               void                   *bodyDone )
{
   Body_t *thisBody = BodySetGet( bodySet, bodyId );
   if ( RBTreeFind( bodyDone, &bodyId ) == RBTREE_INVALID_ID ) {
      Set_t     *containSet = BodyContainBodies( bodyId, bbTree, bodySet, pointSet, MeshTolerance( mesh ) );
      const int  nContains = SetSize( containSet );
      const int *contains = SetGet( containSet );

      if ( nContains ) {
         int     n;
         /* Recursively find containment */
         for ( n = 0; n < nContains && !MESH_ERROR( mesh ); n++ ) {
            if ( contains[n] != outerBodyId ) {
               GetHierarchy( mesh, bodySet, contains[n], bodyId, pointSet, bbTree, bodyDone );
            } else {
               MeshMessageOpen( mesh, MeshStatusBodyNesting );
               MeshMessageAdd( mesh, "Failed to determine body nesting" );
               MeshMessageClose( mesh );
            }
         }
         /* All inner bodies that are not yet in another body, are in this body */
         for ( n = 0; n < nContains && !MESH_ERROR( mesh ); n++ ) {
            Body_t *containBody = BodySetGet( bodySet, contains[ n ] );
            if ( !BodyOuterGet( containBody ) ) {
               BodyOuterSet( containBody, thisBody );
            }
         }
      }

      /* Mark as done */
      RBTreeSearch( bodyDone, &bodyId );
      SetDelete( containSet );
   }
}

extern void BodySetHierarchy( 
               void                   *mesh,
               BodySet_t              *bodySet,
               PointSet_t             *pointSet )
{
   const int       nBody = BodySetSize( bodySet );
   RBTree_t       *bodyDone = RBTreeCreate( sizeof( int ), CompareInt );
   BoundBoxTree_t *bbTree = BoundBoxTreeCreate();
   int             n;

   /* Build a tree of all bounding boxes of all bodies */
   for ( n = 0; n < nBody && !MESH_ERROR( mesh ); n++ ) {
      Body_t     *body = BodySetGet( bodySet, n );
      BoundBox_t  bb;
      BodyBoundBox( body, pointSet, BoundBoxInit( &bb ) );
      BoundBoxTreeAdd( bbTree, n, &bb );
      /* Reset outer body */
      BodyOuterSet( body, NULL );
   }

   /* Set up hierarchy */
   for ( n = 0; n < nBody && !MESH_ERROR( mesh ); n++ ) {
      GetHierarchy( mesh, bodySet, n, -1, pointSet, bbTree, bodyDone );
   }

   RBTreeDelete( bodyDone );
   BoundBoxTreeDelete( bbTree );
}
