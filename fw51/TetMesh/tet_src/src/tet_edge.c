 /*                                         Copyright (c) 2011 TNO DIANA BV */
#include "tet_edge.h"
#include "Set.h"
#include "RBTree.h"
#include "tet_surfaceset.h"
#include "tet_triangle.h"
#include "tet_mesh.h"

#define EDGE_LIST( edge, id ) ( (EdgeList_t *) ( id >= 0 ? SetElmGet( (edge)->edgeList, (id) ) : NULL ))


static int EdgeDataCompare( const void* data0, const void* data1 )
{
   const EdgeKey_t *e0 = (const EdgeKey_t*)data0;
   const EdgeKey_t *e1 = (const EdgeKey_t*)data1;
   int               result = UTIL_COMPARE( e0->minPoint, e1->minPoint );
   if ( !result ) {
      result = UTIL_COMPARE( e0->maxPoint, e1->maxPoint );
   }
   return result;
}

extern Edge_t *EdgeCreate( void )
{
   Edge_t  *result = DIMALLOC( sizeof(*result) );
   UTIL_ZERO( result );
   result->edgeKeyTree = RBTreeCreate( sizeof( EdgeKey_t ), EdgeDataCompare );
   result->edgeList = SetCreate( 0, sizeof(EdgeList_t) );
   return result;
}

extern Edge_t *EdgeDelete( Edge_t *edge )
{
   if ( edge ) {
      RBTreeDelete( edge->edgeKeyTree );
      SetDelete( edge->edgeList );
      DIFREE( edge );
   }
   return edge;
}

static int EdgeNewList( 
              Edge_t                 *edge,
              Surface_t              *surface,
              int                     triangleId,
              int                     edgeNum )
{
   int            result = SetSize( edge->edgeList );
   EdgeList_t    *list = SetNew( edge->edgeList, 1 );
   list->surface = surface;
   list->triangleId = triangleId;
   list->edge = edgeNum;
   list->next = -1;
   return result;
}

extern Edge_t *EdgeAdd( Edge_t *edge, Surface_t *surface )
{
   int          triangleId;
   Iterator_t   iter;
   Triangle_t  *triangle = SurfaceFirst( surface, &triangleId, &iter );
   while ( triangle ) {
      const int   nPoint = TriangleSize( triangle );
      int         p;
      for ( p = 0; p < nPoint; p++ ) {
         int   p1, p2, newListId;
         EdgeKey_t   edgeKey, *foundKey;
         TriangleSide( triangle, p, &p1, &p2 );
   
         edgeKey.minPoint = MIN( p1, p2 );
         edgeKey.maxPoint = MAX( p1, p2 );
         edgeKey.list     = -1;

         foundKey = RBTreeGet( edge->edgeKeyTree, RBTreeSearch( edge->edgeKeyTree, &edgeKey ) );
         /* Beware of REALLOC ! */
         newListId = EdgeNewList( edge, surface, triangleId, p );
         if ( foundKey->list == -1 ) {
            foundKey->list = newListId;
         } else {
            EdgeList_t   *list = EDGE_LIST( edge, foundKey->list );
            while ( list->next != -1 ) {
               list = EDGE_LIST( edge, list->next );
            }
            list->next = newListId;
         }
      }

      triangle = SurfaceNext( surface, &triangleId, &iter );
   }
   return edge;
}
      
extern bool_t EdgeKeyDebug( Edge_t *edge, const EdgeKey_t *key )
{
  bool_t    found = FALSE;
  UNUSED(edge);
  if ( EdgeKeyContains( key, 3409 ) && EdgeKeyContains( key, 16318 ) ) {
    /* Stop here in debugger */
    found= TRUE;
#if 0
    EdgeKeyPrint( edge, key, stderr );
#endif
  }
  return found;
}

extern int EdgeKeyOffset( const Edge_t *edge, const EdgeKey_t *key )
{
  return RBTreeFind( edge->edgeKeyTree, key );
}

extern EdgeKey_t *EdgeFind( Edge_t *edge, int point1, int point2 )
{
   EdgeKey_t    edgeKey, *found;
   
   edgeKey.minPoint = MIN( point1, point2 );
   edgeKey.maxPoint = MAX( point1, point2 );
   found = RBTreeFindGet( edge->edgeKeyTree, &edgeKey );
   return found;
}

extern EdgeKey_t  *EdgeKeyFirst( Edge_t *edge, Iterator_t *iter )
{
   return RBTreeFirst( edge->edgeKeyTree, iter );
}

extern EdgeKey_t  *EdgeKeyNext( Edge_t *edge, Iterator_t *iter )
{
   return RBTreeNext( edge->edgeKeyTree, iter );
}

extern int EdgeListSize( Edge_t *edge, const EdgeKey_t *key, int *nFaults )
{
   const EdgeList_t *list = EdgeListFirst( edge, key );
   int               len = 0;
   if ( nFaults ) *nFaults = 0;
   while ( list ) {
      len += 1;
      if ( nFaults && ( SurfaceType( EdgeListSurface( list ) ) != surfNormal ) ) {
        *nFaults += 1;
      }
      list = EdgeListNext( edge, list );
   }
   return len;
}

extern EdgeList_t *EdgeListFirst( Edge_t *edge, const EdgeKey_t *key )
{
   return EDGE_LIST( edge, key->list );
}

extern EdgeList_t *EdgeListNext( Edge_t *edge, const EdgeList_t *list )
{
   return EDGE_LIST( edge, list->next );
}

extern int EdgeListComp(const void *edge0V, const void *edge1V )
{
   const EdgeList_t  *edge0 = edge0V;
   const EdgeList_t  *edge1 = edge1V;
   int                result = UTIL_COMPARE( edge0->next, edge1->next );
   if ( !result ) {
      result = SurfaceComp( edge0->surface, edge1->surface );
   }
   if ( !result ) {
      result = UTIL_COMPARE( edge0->triangleId, edge1->triangleId );
   }
   if ( !result ) {
      result = UTIL_COMPARE( edge0->edge, edge1->edge );
   }
   return result;
}

extern void EdgeListGet( Edge_t *edge, const EdgeKey_t *key, EdgeList_t *listOut, int *lenOut )
{
   const EdgeList_t *list = EdgeListFirst( edge, key );
   int               len = 0;
   while ( list ) {
      memcpy( listOut + len, list, sizeof(*list) );
      len += 1;
      list = EdgeListNext( edge, list );
   }
   if ( lenOut ) *lenOut = len;
}

extern void EdgeListPrint( const EdgeList_t *list, FILE *fp )
{
  char   buf[ BUFSIZ ];
  const Surface_t *surf = EdgeListSurface(list);
  fprintf( fp, " surface=%s triangleId=%d edge=%d\n", 
           SurfaceUserInfo( buf, surf),
            EdgeListTriangleId(list), list->edge );
}

extern void EdgeKeyPrint( Edge_t *edge, const EdgeKey_t  *key, FILE *fp )
{
   const EdgeList_t *list = EdgeListFirst( edge, key );
   fprintf( fp, "EdgePrint points: %d %d\n", key->minPoint, key->maxPoint );
   while ( list ) {
      EdgeListPrint( list, fp );
      list = EdgeListNext( edge, list );
   }
}

extern void EdgePrint( Edge_t *edge, FILE *fp )
{
   Iterator_t        iter;
   const EdgeKey_t  *key = EdgeKeyFirst( edge, &iter );
   while ( key ) {
      EdgeKeyPrint( edge, key, fp );
      key = EdgeKeyNext( edge, &iter );
   }
}
extern bool_t EdgeIsolatedFault( Edge_t *edge, const EdgeKey_t *key )
{
   bool_t    isolatedFaultEdge = FALSE;
   if ( EdgeListSize( edge, key, NULL ) == 2 ) {
      EdgeList_t   edges[2];
      EdgeListGet( edge, key, edges, NULL );
      if ( SurfaceFrontBackOfSameFault( edges[0].surface, edges[1].surface ) ) {
         isolatedFaultEdge = TRUE;
      }
   }
   return isolatedFaultEdge;
}


extern bool_t EdgeInternal( Edge_t *edge, const EdgeKey_t *key )
{
   bool_t            isInternal = TRUE;
   const Surface_t  *normal = NULL, *front = NULL, *back = NULL;
   EdgeList_t       *list = EdgeListFirst( edge, key );

   while ( list && isInternal ) {
      const Surface_t    *surf = EdgeListSurface( list );
      const SurfaceType_t type = SurfaceType( surf );
      const Surface_t   **prevSurf = ( type == surfNormal ? &normal : (
                                    ( type == surfFrontFault ? &front : (
                                    ( type == surfBackFault ? &back : (NULL) )
                                    ))));
      assert( prevSurf != NULL );
      if ( *prevSurf == NULL ) {
         *prevSurf = surf;
      } else if ( surf != *prevSurf ) {
          isInternal = FALSE; 
      }
      list = EdgeListNext( edge, list );
   }

   if ( isInternal ) {
      /* Equal surfaces of the same type, analyze further ... */
      if ( normal ) {
         isInternal = ( front == NULL && back == NULL );
      } else if ( front && back ) {
         isInternal = ( SurfaceID( front ) == SurfaceID( back ) ); 
         if ( isInternal ) {
            isInternal = ( EdgeListSize( edge, key, NULL ) == 4 );
         }
      } else {
         isInternal = TRUE;
      }
   }

   return isInternal;
}

extern int EdgeListOtherPoint( const EdgeKey_t *key, const EdgeList_t *list )
{
   const Surface_t  *surf = EdgeListSurface( list );
   const int         triangleId = EdgeListTriangleId( list );
   const Triangle_t *triangle = SurfaceGet( surf, triangleId );
   const int        *points = TrianglePoints( triangle );
   int               point1, point2, point3 = -1, n;

  /* 
   * Find the point that is in the normal surface triangle, 
   * and NOT on the fault joint 
   */
   assert( SurfaceType( surf ) == surfNormal );
   EdgeKeyPoints( key, &point1, &point2 );
   for ( n = 0; n < 3; n++ ) {
      if ( ( points[n] != point1 ) &&  ( points[n] != point2 ) ) {
         point3 = points[n];
         break;
      }
   }
   assert( point3 >= 0 );
   return point3;
}

/* Test if a fault touches the outer boundary in this edge */
extern bool_t EdgeFaultTouchesOuter( 
                  Edge_t                     *edge, 
                  const EdgeKey_t            *key )
{
  bool_t     outerBound = FALSE;
  bool_t     innerFault = FALSE;
  EdgeList_t  *list = EdgeListFirst( edge, key );
  while ( list ) {
    const Surface_t *surf = EdgeListSurface( list );
    if ( SurfaceAtOuterBoundary( surf ) ) {
      outerBound = TRUE;
    } else if ( SurfaceType( surf ) != surfNormal ) {
      innerFault = TRUE;
    }
    list = EdgeListNext( edge, list );
  }
  return ( outerBound && innerFault );
}

extern BoundBox_t *EdgeKeyBoundBox(
                  const EdgeKey_t            *key,
                  PointSet_t                 *pointSet,
                  BoundBox_t                 *bb )
{
   int               point1, point2;
   const double     *cor = NULL;

   EdgeKeyPoints( key, &point1, &point2 );

   BoundBoxInit( bb );
   cor = PointSetGet( pointSet, point1 );
   BoundBoxAdd( bb, cor );
   cor = PointSetGet( pointSet, point2 );
   BoundBoxAdd( bb, cor );

  return bb;
}

extern void EdgeKeyMeshMessageAdd( 
                  void                       *mesh, 
                  Edge_t                     *edge,
                  EdgeKey_t                  *key,
                  PointSet_t                 *pointSet )
{
  SurfaceSet_t   *uniqueSet = SurfaceSetCreate();
  int             p0, p1;
  const EdgeList_t *list = EdgeListFirst( edge, key );

  MeshMessageAdd( mesh, "Surface(s) connected to edge:\n" );
 
  while ( list ) {
    Surface_t   *surf = EdgeListSurface( list );
    if ( !SurfaceSetTest( uniqueSet, surf ) ) {
      SurfaceSetAdd( uniqueSet, surf );
    }
    list = EdgeListNext( edge, list );
  }
  SurfaceSetMeshMessageAdd( mesh, uniqueSet );
  EdgeKeyPoints( key, &p0, &p1 );
  MeshMessageAdd( mesh, "Coordinates:\n" );
  PointMeshMessageAdd( mesh, pointSet, p0 );
  PointMeshMessageAdd( mesh, pointSet, p1 );
  SurfaceSetDelete( SurfaceSetClear( uniqueSet ) );
}
