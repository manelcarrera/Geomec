 /*                                         Copyright (c) 2011 TNO DIANA BV */
#include "float.h"
#include "tet_utils.h"
#include "tet_point.h"
#include "tet_triangle.h"
#include "tet_surface.h"
#include "tet_surfaceset.h"
#include "tet_edge.h"
#include "RBTree.h"
#include "tet_graph.h"
#include "Set.h"
#include "tet_bodyset.h"
#include "tet_xpoint.h"
#include "tet_mesh.h"


static int SurfaceSetCompareSurface( const void* data0, const void* data1 )
{
   const SurfaceData_t *s0 = data0;
   const SurfaceData_t *s1 = data1;
   return SurfaceComp( s0->surface, s1->surface );
}

static SurfaceSet_t *SurfaceSetInit( SurfaceSet_t *surfaceSet )
{
   UTIL_ZERO( surfaceSet );
   surfaceSet->surface = RBTreeCreate( sizeof(SurfaceData_t), 
                                       SurfaceSetCompareSurface );
   return surfaceSet;
}

extern bool_t SurfaceSetTest(
                  SurfaceSet_t               *surfaceSet,
                  Surface_t                  *surface )
{
  SurfaceData_t  data;
  UTIL_ZERO( &data );
  data.surface = surface;
  return ( RBTreeFindGet( surfaceSet->surface, &data ) != NULL );
}

static SurfaceSet_t *SurfaceSetDeleteData( SurfaceSet_t *surfaceSet )
{
   if ( surfaceSet ) {
      Iterator_t iter;
      Surface_t *surface = SurfaceSetFirst( surfaceSet, &iter );
      surfaceSet->edge = EdgeDelete( surfaceSet->edge );
      while ( surface ) {
         SurfaceDelete( surface );
         surface = SurfaceSetNext( surfaceSet, &iter );
      }
      surfaceSet->surface = RBTreeDelete( surfaceSet->surface );
   }
   return surfaceSet;
}


extern SurfaceSet_t *SurfaceSetCreate( void )
{
   SurfaceSet_t     *result = DIMALLOC( sizeof(*result) );
   return SurfaceSetInit( result );
}

extern SurfaceSet_t *SurfaceSetDelete( SurfaceSet_t *surfaceSet )
{
   if ( surfaceSet ) {
      SurfaceSetDeleteData( surfaceSet );
      DIFREE( surfaceSet );
   }
   return surfaceSet;
}

extern SurfaceSet_t *SurfaceSetCopy(  
                  SurfaceSet_t               *orgSurfSet )
{
  SurfaceSet_t     *result = SurfaceSetCreate();
  SurfaceSetAddSurfaceSet( result, orgSurfSet, TRUE );
  return result;
}

extern SurfaceSet_t *SurfaceSetPurge( SurfaceSet_t *surfaceSet )
{
   if ( surfaceSet ) {
      Iterator_t iter;
      Surface_t *surface = SurfaceSetFirst( surfaceSet, &iter );
      while ( surface ) {
         SurfacePurge( surface );
         surface = SurfaceSetNext( surfaceSet, &iter );
      }
      surfaceSet->edge = EdgeDelete( surfaceSet->edge );
   }
   return surfaceSet;
}

/* Clear the surface-set,  but do not delete the surfaces themselves! */
extern SurfaceSet_t *SurfaceSetClear( SurfaceSet_t *surfaceSet )
{
   if ( surfaceSet ) {
      surfaceSet->surface = RBTreeDelete( surfaceSet->surface );
      surfaceSet->edge = EdgeDelete( surfaceSet->edge );
      SurfaceSetInit( surfaceSet );
   }
   return surfaceSet;
}

extern void  SurfaceSetAdd( SurfaceSet_t *surfaceSet, Surface_t *surface )
{
   if ( surface ) {
      SurfaceData_t  data;
      UTIL_ZERO( &data );
      data.surface = surface;
      assert( !RBTreeFindGet( surfaceSet->surface, &data ) );
      RBTreeSearch( surfaceSet->surface, &data );
      surfaceSet->edge = EdgeDelete( surfaceSet->edge );
   }
}

static int SurfaceSetDataId( SurfaceSet_t *surfaceSet, const Surface_t *surface )
{
   SurfaceData_t data;
   int           result = -1;
   UTIL_ZERO( &data );
   data.surface = (Surface_t *)surface;
   result =  RBTreeFind( surfaceSet->surface, &data );
   assert( result >= 0 && result < (int)RBTreeSize( surfaceSet->surface ) );
   return result;
}

extern Surface_t *SurfaceSetFirst( SurfaceSet_t *surfaceSet, Iterator_t *iter )
{
   Surface_t *surface = NULL;
   if ( surfaceSet )  {
      SurfaceData_t *data = RBTreeFirst( surfaceSet->surface, iter );
      if ( data ) surface = data->surface;
   }
   return surface;
}


extern Surface_t *SurfaceSetNext( SurfaceSet_t *surfaceSet, Iterator_t *iter )
{
   SurfaceData_t *data = RBTreeNext( surfaceSet->surface, iter );
   Surface_t     *surface = NULL;
      if ( data ) surface = data->surface;
   return surface;
}

/* Return number of surfaces in set */
extern int   SurfaceSetSize( const SurfaceSet_t *surfaceSet )
{
   return (int)RBTreeSize( surfaceSet->surface );
}

/* Return number of triangles in all surfaces in set */
extern int   SurfaceSetTriangleSize( 
                  SurfaceSet_t               *surfaceSet )
{
  int                size = 0;
  Iterator_t         iter;
  const Surface_t   *surface = SurfaceSetFirst( surfaceSet, &iter );
  while ( surface ) {
    size += SurfaceSize( surface );
    surface = SurfaceSetNext( surfaceSet, &iter );
  }
  return size;
}

extern Edge_t *SurfaceSetEdge( SurfaceSet_t *surfSet )
{
   if ( !surfSet->edge ) {
      Edge_t    *edge = EdgeCreate( );
      Iterator_t iter;
      Surface_t *surface = SurfaceSetFirst( surfSet, &iter );
      while ( surface ) {
         EdgeAdd( edge, surface );
         surface = SurfaceSetNext( surfSet, &iter );
      }
      surfSet->edge = edge;
   }
   return surfSet->edge;
}

/* Set ofset for every surface */
static int *SurfaceSetTriangleOffset( SurfaceSet_t *surfaceSet )
{
   const int     numSurf = SurfaceSetSize( surfaceSet );
   int          *result = DIMALLOC( numSurf*sizeof(*result) );
   Iterator_t    iter;
   Surface_t   *surface = SurfaceSetFirst( surfaceSet, &iter );
   int          lofset = 0;
   UTIL_SET( result, INT_MAX, numSurf );
   while ( surface ) {
      result[SurfaceSetSurfaceId(surfaceSet, surface)] = lofset;
      lofset += SurfaceSize( surface );
      surface = SurfaceSetNext( surfaceSet, &iter );
   }
   return result;
}

static int *GroupTriangles( 
              SurfaceSet_t          *surfaceSet, 
              int                   *numGroups ) 
{
   Edge_t       *edge = SurfaceSetEdge( surfaceSet );
   int          *groupId = NULL;
   TetGraph_t      *graph = TetGraphCreate();
   int          *ofset = SurfaceSetTriangleOffset( surfaceSet );
   Iterator_t    iter;
   EdgeKey_t    *key = EdgeKeyFirst( edge, &iter );
   Set_t        *edgeListSet = SetCreate( 100, sizeof(EdgeList_t) );

   /* Fill the connection table with edges that connect 2 triangles */
   while ( key ) {
      int          p0, p1;
      const int    numPolygon = EdgeListSize( edge, key, NULL );
      EdgeList_t *edges = SetNew( SetClear(edgeListSet), numPolygon );
      EdgeListGet( edge, key, edges, NULL );
     /* 
      * Always add self-reference, otherwise dimension wrong if 
      * surface consists of single triangle
      */
      for ( p0 = 0; p0 < numPolygon; p0++ ) {
         Surface_t *surf0 = EdgeListSurface( edges+p0 );
         const int  surfId0 = SurfaceSetSurfaceId( surfaceSet, surf0 );
         const int  id0 = ofset[surfId0] + EdgeListTriangleId( edges+p0 );
         TetGraphAdd( graph, id0, id0 );
      }
      if ( EdgeInternal( edge, key ) ) {
         for ( p0 = 0; p0 < numPolygon; p0++ ) {
            for ( p1 = p0+1; p1 < numPolygon; p1++ ) {
               Surface_t *surf0 = EdgeListSurface( edges+p0 );
               Surface_t *surf1 = EdgeListSurface( edges+p1 );
               if ( surf0 == surf1 ) {
                  const int surfId0 = SurfaceSetSurfaceId( surfaceSet, surf0 );
                  const int surfId1 = SurfaceSetSurfaceId( surfaceSet, surf1 );
                  const int id0 = ofset[surfId0] + EdgeListTriangleId( edges+p0 );
                  const int id1 = ofset[surfId1] + EdgeListTriangleId( edges+p1 );
                  TetGraphAdd( graph, id0, id1 );
               }
            }
         }
      }
      key = EdgeKeyNext( edge, &iter );
   }

   /* Seperate disconnected parts */
   groupId = TetGraphGetConnectivity( graph, numGroups );

   DIFREE( ofset );
   TetGraphDelete( graph );
   SetDelete( edgeListSet );

   return groupId;
}

typedef struct IdPart_t {
  const Surface_t    *surf;
  int                 part;
} IdPart_t;

static int IdPartCompare( const void* data0, const void* data1 )
{
   const IdPart_t *ip0 = data0;
   const IdPart_t *ip1 = data1;
   int             result = UTIL_COMPARE( SurfaceID(ip0->surf), SurfaceID(ip1->surf) );
   if ( !result ) {
     /* Front/back have their own numbering */
     result = UTIL_COMPARE( SurfaceType(ip0->surf), SurfaceType(ip1->surf) );
   }
   return result;
}

static SurfaceSet_t *SynchronizeFault(
                  SurfaceSet_t               *faultSet )
{
  SurfaceSet_t          *result = SurfaceSetCreate( );
  SurfaceSet_t          *frontSet = SurfaceSetSameType( faultSet, surfFrontFault );
  SurfaceSet_t          *backSet  = SurfaceSetSameType( faultSet, surfBackFault );
  Iterator_t            iterBack, iterFront;
  Surface_t            *front = SurfaceSetFirst( frontSet, &iterFront );
  while ( front ) {
    /* Find matching back */
    Surface_t      *back = SurfaceSetFirst( backSet, &iterBack );
    Surface_t      *newBack = NULL;
    bool_t          found = FALSE;
    while ( back ) {
      if ( SurfaceFrontBackIdenticalTest( front, back ) ) {
        found = TRUE;
        break;
      }
      back = SurfaceSetNext( backSet, &iterBack );
    }
    assert( found );
    newBack = SurfaceCreate(
                             SurfaceID(back),
                             SurfacePartGet(front),  /* This is what we are doing */ 
                             SurfaceType(back), 
                             SurfaceGetFrontDir( back ),
                             SurfaceUserName( back ),
                             SurfaceOuterBoundaryGet( back ) );
    SurfaceAddSurface( newBack, back );
    SurfaceSetAdd( result, SurfaceCopy( front ) );
    SurfaceSetAdd( result, newBack );
    front = SurfaceSetNext( frontSet, &iterFront );
  }
  SurfaceSetDelete( SurfaceSetClear( frontSet ) );
  SurfaceSetDelete( SurfaceSetClear( backSet  ) );
  return result;
}

static  SurfaceSet_t *SynchronizeFrontBack(
                  SurfaceSet_t               *surfaceSet )
{
  RBTree_t           *idDone = RBTreeCreate( sizeof(int), CompareInt );
  Iterator_t          iter;
  Surface_t          *surface = SurfaceSetFirst( surfaceSet, &iter );
  SurfaceSet_t       *result = SurfaceSetCreate();
  while ( surface ) {
    const int   id = SurfaceID( surface );
    if ( !RBTreeFindGet( idDone, &id ) ) {
      /* Pick up fault surfaces with same ID */
      SurfaceSet_t  *idSet = SurfaceSetSameID( surfaceSet, id );
      RBTreeSearch( idDone, &id ); /* Mark as done */

      if ( SurfaceIsFault( surface ) ) {
        SurfaceSet_t *newIdSet = SynchronizeFault( idSet );
        SurfaceSetAddSurfaceSet( result, newIdSet, FALSE );
        SurfaceSetDelete( SurfaceSetClear( newIdSet ) );
      } else {
        SurfaceSetAddSurfaceSet( result, idSet, TRUE );
      }
      SurfaceSetDelete( SurfaceSetClear( idSet ) );
    }
    surface = SurfaceSetNext( surfaceSet, &iter );
  }
  RBTreeDelete( idDone );
  return result;
}

extern SurfaceSet_t *SurfaceSetSplitIntoSimpleSurface( 
                  void                       *mesh, 
                  SurfaceSet_t               *surfaceSet,
                  bool_t                      synchronizeFrontBack )
{
   SurfaceSet_t  *simpleSurfaceSet = SurfaceSetCreate();
   int           *groupId = NULL;
   int            numGroups;
   RBTree_t      *idPartTree= RBTreeCreate( sizeof(IdPart_t), IdPartCompare );

   UNUSED(mesh);

   groupId = GroupTriangles( surfaceSet, &numGroups );

   {
      Iterator_t  surfSetIter;
      Surface_t  *surface = SurfaceSetFirst( surfaceSet, &surfSetIter );
      Surface_t **newSurf = DIMALLOC( numGroups*sizeof(*newSurf) );
      int    triangleId = 0;
      UTIL_SET( newSurf, NULL, numGroups );

      while ( surface ) {
         Iterator_t     iter;
         Triangle_t    *triangle = SurfaceFirst( surface, NULL, &iter );

         /* Loop over the triangles and add them to the new surfaces */
         while ( triangle ) {
            Surface_t *newSurface = newSurf[groupId[triangleId]];
            if ( !newSurface ) {
               IdPart_t  idPart, *idPartFound = NULL;
               memset ( &idPart, 0, sizeof idPart );
               idPart.surf = surface;
               idPart.part = 0;
               idPartFound = RBTreeSearchGet( idPartTree, &idPart );
               idPartFound->part += 1;
               newSurf[groupId[triangleId]] = SurfaceCreate( 
                                                 SurfaceID(surface),
                                                 idPartFound->part,
                                                 SurfaceType(surface), 
                                                 SurfaceGetFrontDir( surface ),
                                                 SurfaceUserName( surface ),
                                                 SurfaceOuterBoundaryGet( surface ) );
               newSurface = newSurf[groupId[triangleId]];
               SurfaceSetAdd( simpleSurfaceSet, newSurface );
            }
            SurfaceAdd( newSurface, TriangleCopy( triangle ) );
            triangle = SurfaceNext( surface, NULL, &iter );
            triangleId += 1;
         }
         surface = SurfaceSetNext( surfaceSet, &surfSetIter );
      }

      DIFREE( newSurf );
   }

   /* If front and back are identical, then make sure that they have equal part number */
   if ( synchronizeFrontBack ) {
     SurfaceSet_t   *tmp = simpleSurfaceSet;
     simpleSurfaceSet = SynchronizeFrontBack( tmp );
     SurfaceSetDelete( tmp );
   }

   DIFREE( groupId );
   RBTreeDelete( idPartTree );

   return simpleSurfaceSet;
}

extern void SurfaceSetPrint(  SurfaceSet_t *surfaceSet, FILE *fp )
{
   Iterator_t          iter;
   Surface_t          *surface = SurfaceSetFirst( surfaceSet, &iter );
   fprintf( fp, "SURFACE_SET size=%d \n\n", SurfaceSetSize(surfaceSet) );
   while ( surface ) {
      SurfaceInfoPrint( surface, fp );
      surface = SurfaceSetNext( surfaceSet, &iter );
   }
}

extern int SurfaceSetSignedSurfId(  SurfaceSet_t *surfaceSet, const Surface_t *surface, bool_t posDir )
{
   const int id = 2*SurfaceSetDataId( surfaceSet, surface ) + ( posDir ? 1 : 0 );
   return id;
}

static bool_t IsActiveSurf( 
               const Surface_t         *surf,
               const bool_t             posDir )
{
   bool_t    isActive = TRUE;

   /* Here we treat the exception that the frontDir is unknown */
   if ( SurfaceType( surf ) != surfNormal && SurfaceGetFrontDir( surf ) != frontUnknownDir ) {
      isActive = SurfaceActive( surf, posDir );
   }
   return isActive;
}

typedef struct JointData_t {
   const EdgeList_t *edge1;
   const EdgeList_t *edge2;
   double           angle;
   int              surfId1;
   int              surfId2;
   bool_t           posDir1;
   bool_t           posDir2;
} JointData_t;


static int JointDataCompare( const void* data0, const void* data1 )
{
   const JointData_t *e0 = (const JointData_t*)data0;
   const JointData_t *e1 = (const JointData_t*)data1;
   int                result = EdgeListComp( e0->edge1, e1->edge1 );
   if ( !result ) {
      result = UTIL_COMPARE( e0->posDir1, e1->posDir1 );
   }
   if ( !result ) {
      result = UTIL_COMPARE( e0->angle, e1->angle );
   }
   return result;
}

static bool_t JointDataFill( 
               JointData_t            *jd, 
               bool_t                  useBothSidesFault,
               SurfaceSet_t           *surfaceSet, 
               const EdgeList_t       *edge1,
               const bool_t            posDir1,
               const EdgeList_t       *edge2,
               const bool_t            posDir2,
               const double            angle )
{
   Surface_t   *surf1 = EdgeListSurface( edge1 );
   Surface_t   *surf2 = EdgeListSurface( edge2 );
   const bool_t       isActive = ( useBothSidesFault || 
                    ( IsActiveSurf( surf1, posDir1 ) && IsActiveSurf( surf2, posDir2 ) ) );

   /* Always fill jd, even if not active */
   jd->edge1 = edge1;
   jd->edge2 = edge2;
   jd->surfId1 = SurfaceSetSignedSurfId( surfaceSet, EdgeListSurface( edge1 ), posDir1 );
   jd->surfId2 = SurfaceSetSignedSurfId( surfaceSet, EdgeListSurface( edge2 ), posDir2 );
   jd->posDir1 = posDir1;
   jd->posDir2 = posDir2;
   jd->angle = angle;
   return isActive;
}

static void AnalyzeSinglePair(
               const EdgeList_t       *edge1, 
               const EdgeList_t       *edge2, 
               bool_t                  useBothSidesFault,
               SurfaceSet_t           *surfaceSet, 
               PointSet_t             *pointSet, 
               Set_t                  *jointData )
{
   Surface_t        *surface1 = EdgeListSurface( edge1 );
   Surface_t        *surface2 = EdgeListSurface( edge2 );
   const Triangle_t *triangle1 = SurfaceGet( surface1, EdgeListTriangleId( edge1 ) );
   const Triangle_t *triangle2 = SurfaceGet( surface2, EdgeListTriangleId( edge2 ) );
   bool_t            orientOK = SurfaceCheckEdgeOrientation(
                       surface1, EdgeListTriangleId(edge1), edge1->edge, FALSE, 
                       surface2, EdgeListTriangleId(edge2), edge2->edge, FALSE );
   double            normal1[3], normal2[3], center2[3];
   bool_t            errNorm1 = TriangleUnitNormal( triangle1, pointSet, normal1 );
   bool_t            errNorm2 = TriangleUnitNormal( triangle2, pointSet, normal2 );
   double            cosAngle = orientOK ?  VecInprod( normal1, normal2, 3 )
                                         : -VecInprod( normal1, normal2, 3 );
   double            angle1, angle2;
   bool_t            isActive[4];
   JointData_t       jd[4];
   int               n;

   assert( !errNorm1 );
   assert( !errNorm2 );
   
   /* Get the center of the second triangle */
   TriangleCenter( triangle2, pointSet, center2 );

   /* Determine position of center wrt first triangle */ 
   if ( TrianglePointPosition( triangle1, pointSet, 0., center2 ) >= 0 ) {
      angle1 = M_PI - ArcCosinus( cosAngle );
   } else {
      angle1 = M_PI + ArcCosinus( cosAngle );
   }
   angle2 = 2*M_PI - angle1;

   if ( orientOK ) {
      isActive[0] = JointDataFill( jd+0, useBothSidesFault, surfaceSet, edge1, TRUE,  edge2, TRUE,  angle1 );
      isActive[1] = JointDataFill( jd+1, useBothSidesFault, surfaceSet, edge1, FALSE, edge2, FALSE, angle2 );
   } else {
      isActive[0] = JointDataFill( jd+0, useBothSidesFault, surfaceSet, edge1, TRUE,  edge2, FALSE, angle1 );
      isActive[1] = JointDataFill( jd+1, useBothSidesFault, surfaceSet, edge1, FALSE, edge2, TRUE,  angle2 );
   }

   /* Add reverse pairs */
   isActive[2] = JointDataFill( jd+2, useBothSidesFault, surfaceSet, jd[0].edge2, jd[0].posDir2,  
                                                                     jd[0].edge1, jd[0].posDir1,  jd[0].angle );
   isActive[3] = JointDataFill( jd+3, useBothSidesFault, surfaceSet, jd[1].edge2, jd[1].posDir2,  
                                                                     jd[1].edge1, jd[1].posDir1,  jd[1].angle );

   /* Add the active ones to the set */
   for ( n = 0; n < 4; n++ ) {
     if ( isActive[n] ) {
        SetAdd( jointData, jd+n, 1 );
      }
   }
}

static void AnalyzeSingleJoint( 
               void                    *mesh,
               bool_t                   useBothSidesFault,
               Edge_t                  *edge,
               const EdgeKey_t         *key,
               const EdgeList_t        *edges,
               const int                nEdges, 
               PointSet_t              *pointSet, 
               Set_t                   *jointWork,
               SurfaceSet_t            *surfaceSet,
               Set_t                   *pairs )
{
   int     n;
   int          nFault;
   const int    nTriangle = EdgeListSize( edge, key, &nFault );
   const bool_t isolatedFaultEdge = ( nFault == nTriangle ? EdgeIsolatedFault( edge, key ) : FALSE );

   SetClear( jointWork );
   SetClear( pairs );

   for ( n = 0; n < nEdges; n++ ) {
      int       ngb;
      for ( ngb = n+1; ngb < nEdges; ngb++ ) {
         bool_t  validPair = TRUE;
           const Surface_t *surf1 = EdgeListSurface( edges+n );
           const Surface_t *surf2 = EdgeListSurface( edges+ngb );

         if ( nFault && !useBothSidesFault ) {
           if ( SurfaceFrontBackOfSameFault( surf1, surf2 ) ) {
             validPair = isolatedFaultEdge;
           }
         }
         if ( validPair ) {
            AnalyzeSinglePair( edges+n, edges+ngb, useBothSidesFault, surfaceSet, pointSet, jointWork );
         }
      }
   }

   /* Sort on first edge and angle */
   SetSort( jointWork, JointDataCompare, FALSE );

   /* Pick out pairs with minimum angle */
   {
      const int          nJoint = SetSize( jointWork );
      const JointData_t *joint = SetGet( jointWork );
      int                prevSurfId = -1;
      for ( n = 0; n < nJoint && !MESH_ERROR( mesh ); n++ ) {
         const int  id1 = joint[n].surfId1;
         if ( id1 != prevSurfId ) {
            /* First of sub-list: smallest angle */
                  SetAdd( pairs, joint+n, 1 );
                  prevSurfId = id1;
         }
      }
   }
}

static void ProcessPairs( 
               const Set_t             *pairs,
               TetGraph_t                 *graph )
{
   const int nPairs = SetSize( pairs );
   int       n;
   for ( n = 0; n < nPairs; n++ ) {
      const JointData_t *pair = SetElmGet( pairs, n );
      if ( ( pair->surfId1 != pair->surfId2 ) && !TetGraphExists( graph, pair->surfId1, pair->surfId2 ) ) {
        const Surface_t *surf1 = EdgeListSurface( pair->edge1 );
        const Surface_t *surf2 = EdgeListSurface( pair->edge2 );
        const SurfaceType_t type1 = SurfaceType( surf1 );
        const SurfaceType_t type2 = SurfaceType( surf2 );
        bool_t              isActive = TRUE;
        /* Never connect inactive face of fault to a normal surface, that is not part of the outer boundary */
        if ( type1 == surfNormal && type2 != surfNormal ) {
          isActive = ( SurfaceAtOuterBoundary( surf1 ) || SurfaceActive( surf2, pair->posDir2 ) );
        } else if ( type2 == surfNormal && type1 != surfNormal ) {
          isActive = ( SurfaceAtOuterBoundary( surf2 ) || SurfaceActive( surf1, pair->posDir1 ) );
        }
        if ( isActive ) {
          TetGraphAdd( graph, pair->surfId1, pair->surfId2 );
        }
     }
   }
}

static void SurfaceSetFrontDirSet( 
               void                    *mesh,
               SurfaceSet_t            *surfaceSet,
               const int                ID,
               const FrontDir_t         fdir )
{
   SurfaceSet_t  *faultSet = SurfaceSetSameID( surfaceSet, ID );
   Iterator_t     iter;
   Surface_t     *fault = SurfaceSetFirst( faultSet, &iter );
   while ( fault ) {
      if ( SurfaceType( fault ) != surfNormal ) {
         const FrontDir_t surfFrontDir = SurfaceGetFrontDir( fault );
         if ( surfFrontDir == frontUnknownDir ) {
            SurfaceSetFrontDir( fault, fdir );
         } else if ( surfFrontDir != fdir ) {
            MeshMessageOpen( mesh, MeshStatusFaultWarped );
            MeshMessageAdd( mesh, "Fault seems to be warped" );
            SurfaceMeshMessageAdd( mesh, fault );
            MeshMessageClose( mesh );
         }
      }
      fault = SurfaceSetNext( faultSet, &iter );
   }
   SurfaceSetDelete( SurfaceSetClear( faultSet ) );
}

static void SurfaceSetAnalyzeJoints( 
               void                    *mesh,
               PointSet_t              *pointSet, 
               SurfaceSet_t            *surfaceSet,
               TetGraph_t                 *graph )
{
   Edge_t       *edge = SurfaceSetEdge( surfaceSet );
   Iterator_t    iter;
   EdgeKey_t    *key = EdgeKeyFirst( edge, &iter );
   Set_t        *edgeListSet = SetCreate( 0, sizeof(EdgeList_t) );
   Set_t        *jointWork = SetCreate( 0, sizeof(JointData_t) );
   Set_t        *pairs = SetCreate( 0, sizeof(JointData_t) );

   while ( key && !MESH_ERROR( mesh ) ) {
      if ( !EdgeInternal( edge, key ) ) {
         const int   listLen = EdgeListSize( edge, key, NULL );
         EdgeList_t *edges = SetNew( SetClear(edgeListSet), listLen );
         EdgeListGet( edge, key, edges, NULL );
         if ( !MESH_ERROR( mesh ) ) {
            AnalyzeSingleJoint( mesh, TRUE, edge, key, edges, listLen, pointSet, 
                           jointWork, surfaceSet, pairs );
            ProcessPairs( pairs, graph );
         }
      }
      key = EdgeKeyNext( edge, &iter );
   }
   SetDelete( edgeListSet );
   SetDelete( jointWork );
   SetDelete( pairs );
}

static void SetIndistinguishableFault(
               void                   *mesh,
               SurfaceSet_t           *surfaceSet )
{
  Iterator_t  iter;
  Surface_t  *surface = SurfaceSetFirst( surfaceSet, &iter );
  while ( surface ) {
    if ( SurfaceType( surface ) == surfFrontFault ) {
      SurfaceSetFrontDirSet( mesh, surfaceSet, SurfaceID(surface), frontPosDir );
    }   
    surface = SurfaceSetNext( surfaceSet, &iter );
  } 
}

/* If a surface set only contains faults, then it must be degenerate */
static bool_t SurfaceSetDegenerate( 
               SurfaceSet_t           *surfaceSet )
{
   bool_t      isDegenerate = TRUE;
   Iterator_t  iter;
   Surface_t  *surface = SurfaceSetFirst( surfaceSet, &iter );
   while ( isDegenerate && surface ) {
      if ( SurfaceType( surface ) == surfNormal ) {
         isDegenerate = FALSE;
      }
      surface = SurfaceSetNext( surfaceSet, &iter );
   } 
   return isDegenerate;
}

static int GetOppositeId(
                  const int                   id )
{
  int  oppositeId;
  if ( id % 2 ) {
    oppositeId = id - 1;  /* Odd */
  } else {
    oppositeId = id + 1;  /* Even */
  }
  return oppositeId;
}

#if 0
static void printen(
               SurfaceSet_t           *surfaceSet, 
               bool_t                 *outerBound )
{
   Iterator_t      surfSetIter;
   Surface_t      *surf = SurfaceSetFirst( surfaceSet, &surfSetIter );
    while ( surf ) {
      const int          posId = SurfaceSetSignedSurfId( surfaceSet, surf, TRUE  );
      const int          negId = SurfaceSetSignedSurfId( surfaceSet, surf, FALSE );
      char buf[BUFSIZ];
      if ( outerBound[posId] ) fprintf( stderr, "OUTER pos id=%d: %s\n", posId, SurfaceUserInfo(buf, surf) );
      if ( outerBound[negId] ) fprintf( stderr, "OUTER neg id=%d: %s\n", negId, SurfaceUserInfo(buf, surf) );
      surf = SurfaceSetNext( surfaceSet, &surfSetIter );
   }
}
#endif

static void SetOuterBound(
                  void                       *mesh,
                  SurfaceSet_t               *surfaceSet, 
                  bool_t                     *outerBound,
                  int                         id,
                  int                        *numChange )
{
  if ( !outerBound[id] && !MESH_ERROR( mesh ) ) {
    const int          oppositeId = GetOppositeId( id );
    if ( outerBound[oppositeId] ) {
      MeshMessageOpen( mesh, MeshStatusOuterBoundary );
      MeshMessageAdd( mesh, "Failed to determine the outer boundary of formation defined by\n" );
      SurfaceSetMeshMessageAdd( mesh, surfaceSet );
      MeshMessageClose( mesh );
    } else {
      outerBound[id] = TRUE;
      *numChange += 1;
    }
  }
}
static int ProcessOuterBoundPairs( 
               void                   *mesh,
               SurfaceSet_t           *surfaceSet, 
               Set_t                  *pairs, 
               bool_t                  faultsOpen,
               bool_t                 *outerBound )
{
  int       numChange = 0;
  const int nPairs = SetSize( pairs );
  int       n;
  for ( n = 0; n < nPairs && !MESH_ERROR( mesh ); n++ ) {
    const JointData_t *pair = SetElmGet( pairs, n );
    const Surface_t   *surf1 = EdgeListSurface(pair->edge1);
    const Surface_t   *surf2 = EdgeListSurface(pair->edge2);
    const int          id1 = pair->surfId1;
    const int          id2 = pair->surfId2;
    const int          oppositeId1 = GetOppositeId( id1 );
    const int          oppositeId2 = GetOppositeId( id2 );

    /* Normal case */
    if ( IsActiveSurf( surf1, pair->posDir1 ) && IsActiveSurf( surf2, pair->posDir2 ) ) {
      if ( !outerBound[id1] && outerBound[id2] ) {
        SetOuterBound( mesh, surfaceSet, outerBound, id1, &numChange );
      } 
      if ( outerBound[id1] && !outerBound[id2] ) {
        SetOuterBound( mesh, surfaceSet, outerBound, id2, &numChange );
      } 
    } 
   /* 
    * Special treatment needed for fault/normal if the normal surface is on the 
    * outer boundary: the fault will open here
    */
    if ( faultsOpen && !MESH_ERROR( mesh ) ) {
      if ( outerBound[oppositeId1] && !outerBound[oppositeId2] && ( SurfaceType( surf2 ) != surfNormal ) ) {
        if ( IsActiveSurf( surf2, pair->posDir2 ) ) {
          SetOuterBound( mesh, surfaceSet, outerBound, oppositeId2, &numChange );
        }
      }
      if ( outerBound[oppositeId2] && !outerBound[oppositeId1] && ( SurfaceType( surf1 ) != surfNormal ) ) {
        if ( IsActiveSurf( surf1, pair->posDir1 ) ) {
          SetOuterBound( mesh, surfaceSet, outerBound, oppositeId1, &numChange );
        }
      }
    }
  }
  return numChange;
}

static int MarkOuterBoundaryLoop( 
               void                   *mesh,
               bool_t                  faultsOpen,
               SurfaceSet_t           *surfaceSet, 
               PointSet_t             *pointSet,
               bool_t                 *outerBound )
{
   int           numChange = 0;
   Edge_t       *edge = SurfaceSetEdge( surfaceSet );
   Iterator_t    iter;
   EdgeKey_t    *key = EdgeKeyFirst( edge, &iter );
   Set_t        *edgeListSet = SetCreate( 0, sizeof(EdgeList_t) );
   Set_t        *jointDataSet = SetCreate( 0, sizeof(JointData_t) );
   Set_t        *pairs = SetCreate( 0, sizeof(JointData_t) );

   while ( key && !MESH_ERROR( mesh ) ) {
      if ( !EdgeInternal( edge, key ) ) {
         const int   listLen = EdgeListSize( edge, key, NULL );
         EdgeList_t *edges = SetNew( SetClear(edgeListSet), listLen );
         EdgeListGet( edge, key, edges, NULL );
         if ( !MESH_ERROR( mesh ) ) {
            bool_t     useBothSidesFault = faultsOpen;
            AnalyzeSingleJoint( mesh, useBothSidesFault, edge, key, edges, listLen, pointSet, 
                           jointDataSet, surfaceSet, pairs );
             
            numChange += ProcessOuterBoundPairs( mesh, surfaceSet, pairs, faultsOpen, outerBound );
         }
      }
      key = EdgeKeyNext( edge, &iter );
   }
   SetDelete( edgeListSet );
   SetDelete( jointDataSet );
   SetDelete( pairs );

   return numChange;
}

static void MarkExtremalSurfaces(
               const double            smallLength,
               SurfaceSet_t           *surfaceSet, 
               PointSet_t             *pointSet,
               bool_t                 *outerBound )
{
   Iterator_t      surfSetIter;
   Surface_t      *surf = SurfaceSetFirst( surfaceSet, &surfSetIter );
   bool_t          anyOuterFound = FALSE;

   /* Check if model outer boundary in set */
   while ( surf ) {
      const SurfOuterBoundary_t outer = SurfaceOuterBoundaryGet( surf );
      if ( outer == surfOuterPosDir || outer == surfOuterNegDir ) {
        const int sngSurfId = SurfaceSetSignedSurfId(  surfaceSet, surf, ( outer == surfOuterPosDir ) );
        anyOuterFound = TRUE;
        outerBound[ sngSurfId ] = TRUE;
      }
      surf = SurfaceSetNext( surfaceSet, &surfSetIter );
   }

   if ( !anyOuterFound ) {
      BoundBoxTree_t *bbTree = BoundBoxTreeCreate();
      Set_t          *intersect = SetCreate( 100, sizeof( int ) );
      Set_t          *triangles = SetCreate( 100, sizeof( Triangle_t * ) );
      double          maxSize;
      int             triangleId = 0;
      int             attempt;

      /* Build tree of triangle bounding boxes */
      surf = SurfaceSetFirst( surfaceSet, &surfSetIter );
      while ( surf ) {
         Iterator_t    iter;
         Triangle_t   *triangle = SurfaceFirst( surf, NULL, &iter );
         while ( triangle ) {
            BoundBox_t   bb;
            TriangleBoundBox( triangle, pointSet, BoundBoxInit( &bb ) );
            BoundBoxTreeAdd( bbTree, triangleId, &bb );
            SetAdd( triangles, &triangle, 1 );
            triangleId += 1;
            triangle = SurfaceNext( surf, NULL, &iter );
         }
         surf = SurfaceSetNext( surfaceSet, &surfSetIter );
      }


      /* Get length scale for the problem: diagonal of outer body bounding box */
      maxSize = 1.1*BoundBoxSize( BoundBoxTreeGetBoundBox( bbTree ) ) + 1;

      for ( attempt = 0; attempt <= 1 && !anyOuterFound; attempt++ ) {
        /* A surface is outer if at least 1 normal to a triangle does not intersect any other triangle */
        surf = SurfaceSetFirst( surfaceSet, &surfSetIter );
        while ( surf ) {
           Iterator_t    iter;
           Triangle_t   *triangle = SurfaceFirst( surf, NULL, &iter );
           while ( triangle ) {
              int                posDir;
              double             center[3], normal[3], point[3];
              BoundBoxLineData_t lineData;
              TriangleCenter( triangle, pointSet, center );
              TriangleUnitNormal( triangle, pointSet, normal );
                 for ( posDir = 0; posDir <= 1; posDir++ ) {
                 const int *triangleIds = NULL;
                 int        t;
                 bool_t     otherIntersect = FALSE;
                 UTIL_COPY( point, center, 3 );
                 VecOperScal( point, EqlAdd, normal, OperTimes, (posDir ? 1. : -1.)*maxSize, 3 );
                 BoundBoxLineDataInit( &lineData, center, point );
                 BoundBoxTreeFind( intersect, bbTree, &lineData, BoundBoxLineOverlap );
                 /* Full test for line-triangle intersection */
                 triangleIds = SetGet( intersect );
                 for ( t = 0; t < SetSize( intersect ) && !otherIntersect; t++ ) {
                    Triangle_t **otherTriangleP = SetElmGet( triangles, triangleIds[t] );
                    /* Skip self */
                    if ( TriangleComp( triangle, *otherTriangleP ) ) {
                       otherIntersect = TriangleIntersectLine( *otherTriangleP, pointSet, center, point, smallLength );
                    }
                 }
                 if ( !otherIntersect ) {
                    const int sngSurfId = SurfaceSetSignedSurfId(  surfaceSet, surf, posDir );
                    outerBound[ sngSurfId ] = TRUE;
                    anyOuterFound = TRUE;
                    /* Break loop over triangles, and go to next surface */
                    break;
                 }
              }
              /* Avoid carrying out a full loop (very expensive): in first run only first triangle */
              if ( attempt ) {
                triangle = SurfaceNext( surf, NULL, &iter );
              } else {
                triangle = NULL;
              }
           }
           surf = SurfaceSetNext( surfaceSet, &surfSetIter );
         }
      }
   
      BoundBoxTreeDelete( bbTree );
      SetDelete( intersect );
      SetDelete( triangles );
   }
}

static int ExtendBoundaryFaults(
               void                   *mesh,
               SurfaceSet_t           *surfaceSet, 
               bool_t                 *outerBound )
{
  int            numChange = 0;
  Set_t         *posFaultOuterID = SetCreate( 0, sizeof(int) );
  Set_t         *negFaultOuterID = SetCreate( 0, sizeof(int) );
  SurfaceType_t  type;

  for ( type = surfFrontFault; type <= surfBackFault && !MESH_ERROR( mesh ); type++ ) {
    Iterator_t         iter;
    const Surface_t   *surf = NULL;

    SetClear( posFaultOuterID );
    SetClear( negFaultOuterID );

    surf = SurfaceSetFirst( surfaceSet, &iter );
    while ( surf ) {
      if ( SurfaceType( surf ) == type ) {
        const int          posId = SurfaceSetSignedSurfId( surfaceSet, surf, TRUE  );
        const int          negId = SurfaceSetSignedSurfId( surfaceSet, surf, FALSE );
        const int          ID = SurfaceID( surf );
        if ( outerBound[posId] ) {
          SetAdd( posFaultOuterID, &ID, 1 );
        } else if ( outerBound[negId] ) {
          SetAdd( negFaultOuterID, &ID, 1 );
        }
      }
      surf = SurfaceSetNext( surfaceSet, &iter );
    }
  
    /* Sort and remove double entries */
    SetSort( posFaultOuterID, CompareInt, TRUE );
    SetSort( negFaultOuterID, CompareInt, TRUE );
  
    /* Extend faults */
    surf = SurfaceSetFirst( surfaceSet, &iter );
    while ( surf && !MESH_ERROR( mesh ) ) {
      if ( SurfaceType( surf ) == type ) {
        const int          ID = SurfaceID( surf );
        if ( SetFind( posFaultOuterID, ID, CompareInt ) ) {
          const int          posId = SurfaceSetSignedSurfId( surfaceSet, surf, TRUE  );
          SetOuterBound( mesh, surfaceSet, outerBound, posId, &numChange );
        } else if ( SetFind( negFaultOuterID, ID, CompareInt ) ) {
          const int          negId = SurfaceSetSignedSurfId( surfaceSet, surf, FALSE );
          SetOuterBound( mesh, surfaceSet, outerBound, negId, &numChange );
        }
      }
      surf = SurfaceSetNext( surfaceSet, &iter );
    }
  }

  SetDelete( posFaultOuterID );
  SetDelete( negFaultOuterID );
  return numChange;
}

/* Mark surfaces that are on the outer boundary of this bodyset */
static bool_t *SurfaceSetOuterBound( 
               void                   *mesh,
               bool_t                  faultsOpen,
               SurfaceSet_t           *surfaceSet, 
               PointSet_t             *pointSet )
{
  const int nSignedSurf = 2*SurfaceSetSize( surfaceSet );
  bool_t   *outerBound = DIMALLOC( nSignedSurf*sizeof(*outerBound) );

  UTIL_SET( outerBound, FALSE, nSignedSurf );

  /* Ignore degenerate bodies */
  if ( !SurfaceSetDegenerate( surfaceSet ) && !MESH_ERROR( mesh )  ) {
    int       numChange = 1;
    /* Surfaces with largest and smallest coordinates are on outer boundary */
    MarkExtremalSurfaces( MeshTolerance( mesh ), surfaceSet, pointSet, outerBound );
    while ( numChange && !MESH_ERROR( mesh ) ) {
      /* Surfaces connected with outer bound surface are also on outer boundary */
      numChange = MarkOuterBoundaryLoop( mesh, faultsOpen, surfaceSet, pointSet, outerBound );
      /* 
      * Because faults amust be simply connected, this implies that if part of a fault is connected
      * to the outer boundary, then the wole fault is connected to the outer boundary.
      * Also the side cannot change.
      */
      if ( faultsOpen && !MESH_ERROR( mesh ) ) {
        numChange += ExtendBoundaryFaults( mesh, surfaceSet, outerBound );
      }
    }

    /* Check if outer boundary is closed */
    if ( !MESH_ERROR( mesh )) {
      Edge_t       *edge = SurfaceSetEdge( surfaceSet );
      Iterator_t    iter;
      EdgeKey_t    *key = EdgeKeyFirst( edge, &iter );
      while ( key ) {
        if ( !EdgeInternal( edge, key ) ) {
          int   nOuter = 0;
          EdgeList_t   *list = EdgeListFirst( edge, key );
          while ( list ) {
            const Surface_t   *surf = EdgeListSurface( list );
            const int          posId = SurfaceSetSignedSurfId( surfaceSet, surf, TRUE  );
            const int          negId = SurfaceSetSignedSurfId( surfaceSet, surf, FALSE );
            if ( outerBound[posId] || outerBound[negId] ) {
              nOuter += 1;
            }
            list = EdgeListNext( edge, list );
          }
          assert( nOuter%2 == 0 );  /* Must be even */
        }
        key = EdgeKeyNext( edge, &iter );
      }
    }
  }
  return outerBound;
}

static void SearchDistinguishableFault(
                  void                       *mesh,
                  Edge_t                     *edge,
                  EdgeKey_t                  *key,
               SurfaceSet_t           *surfaceSet, 
                  PointSet_t                 *pointSet, 
                  Set_t                      *idSetFront,
                  Set_t                      *idSetBack,
                  Set_t                      *edgeSet,
                  Set_t                      *jointWork,
                  Set_t                      *pairs )
{
  EdgeList_t   *list = EdgeListFirst( edge, key );

  /* Are there distinguishable faults here? */
  while ( list ) {
    Surface_t   *surf = EdgeListSurface( list );
    if ( SurfaceGetFrontDir( surf ) == frontUnknownDir ) {
      const int  surfID = SurfaceID( surf );
      switch ( SurfaceType( surf ) ) {
        case surfNormal:
          break;
        case surfFrontFault:
          SetAdd( idSetFront, &surfID, 1 );
          break;
        case surfBackFault:
          SetAdd( idSetBack, &surfID, 1 );
          break;
        default:
          assert( FALSE );
          break;
      }
    }
    list = EdgeListNext( edge, list );
  }

  /* Get sorted list of fault IDs */
  SetSort( idSetFront, CompareInt, TRUE );
  SetSort( idSetBack, CompareInt, TRUE );
  if ( SetSize(idSetFront ) || SetSize(idSetBack ) ) {
    const int    numPolygon = EdgeListSize( edge, key, NULL );
    EdgeList_t  *edges = SetNew( SetClear(edgeSet), numPolygon );
    EdgeListGet( edge, key, edges, NULL );
    AnalyzeSingleJoint( mesh, FALSE, edge, key, edges, numPolygon, pointSet, jointWork, surfaceSet, pairs );
    {
      const int nPairs = SetSize( pairs );
      int       n;

      for ( n = 0; n < nPairs && !MESH_ERROR( mesh ) ; n++ ) {
        const JointData_t *pair = SetElmGet( pairs, n );
        Surface_t   *surf1 = EdgeListSurface( pair->edge1 );
        Surface_t   *surf2 = EdgeListSurface( pair->edge2 );
        Surface_t   *fault = NULL;
        bool_t       posDir = 0;
        /* Normal/Fault pair? */
        if ( SurfaceType( surf1 ) == surfNormal &&
             SurfaceType( surf2 ) != surfNormal ) {
          if ( SurfaceGetFrontDir( surf2 ) == frontUnknownDir ) {
            fault = surf2;
            posDir = pair->posDir2;
          }
        } else if ( SurfaceType( surf1 ) != surfNormal &&
                    SurfaceType( surf2 ) == surfNormal ) {
          if ( SurfaceGetFrontDir( surf1 ) == frontUnknownDir ) {
            fault = surf1;
            posDir = pair->posDir1;
          }
        }
        if ( fault ) {
          const int    faultID = SurfaceID( fault );
          const bool_t inFront = ( SetFind( idSetFront, faultID, CompareInt ) != NULL );
          const bool_t inBack  = ( SetFind( idSetBack,  faultID, CompareInt ) != NULL );
          /* Either front of back of distinguishable fault */
          if ( UTIL_XOR( inFront, inBack ) ) {
            FrontDir_t    fDir = SurfaceFaultDir( fault, posDir );

            /* Smallest angle is interior ?!?! */
	    if ( pair->angle > M_PI ) {
               /* Flip */
               if ( fDir == frontPosDir ) {
                 fDir = frontNegDir;
               } else {
                 fDir = frontPosDir;
               }
            }
            SurfaceSetFrontDirSet( mesh, surfaceSet, SurfaceID(fault), fDir );
          }
        }
      }
    }
  }
}

static void SetDistinguishableFault(
               void                   *mesh,
               SurfaceSet_t           *surfaceSet,
               PointSet_t             *pointSet )
{
   bool_t     faultDirUnknown = FALSE;
   Set_t     *idSetFront = SetCreate( 64, sizeof( int ) );
   Set_t     *idSetBack  = SetCreate( 64, sizeof( int ) );
   Set_t     *edgeListWork = SetCreate( 64, sizeof(EdgeList_t) );
   Set_t     *jointWork = SetCreate( 64, sizeof(JointData_t) );
   Set_t     *pairs = SetCreate( 64, sizeof(JointData_t) );
   Iterator_t surfSetIter;

   /* Quick check on unknown fault directions */
   Surface_t *surf = SurfaceSetFirst( surfaceSet, &surfSetIter );
   while ( surf ) {
      if ( SurfaceType( surf ) != surfNormal ) {
         if ( SurfaceGetFrontDir( surf ) == frontUnknownDir ) {
           faultDirUnknown = TRUE;
           break;
         }
      }
      surf = SurfaceSetNext( surfaceSet, &surfSetIter );
   }

   if ( faultDirUnknown ) {
      Edge_t       *edge = SurfaceSetEdge( surfaceSet );
      Iterator_t    iter;
      EdgeKey_t    *key = EdgeKeyFirst( edge, &iter );
      while ( key && !MESH_ERROR( mesh ) ) {
         int       nFault;
         const int nTriangle = EdgeListSize( edge, key, &nFault );
         if ( nFault && ( nTriangle - nFault ) && !EdgeInternal( edge, key ) ) {
            /* Normal/Fault crossing */
            SearchDistinguishableFault( mesh, edge, key, surfaceSet, pointSet,
                                        SetClear(idSetFront),  SetClear(idSetBack),
                                        SetClear(edgeListWork), SetClear(jointWork),
                                        SetClear(pairs) );
         }
         key = EdgeKeyNext( edge, &iter );
      }
   }

   /* Do something for the hard cases */
   surf = SurfaceSetFirst( surfaceSet, &surfSetIter );
   while ( surf && !MESH_ERROR( mesh ) ) {
      if ( SurfaceType( surf ) != surfNormal ) {
         if ( SurfaceGetFrontDir( surf ) == frontUnknownDir ) {
            MeshMessageOpen( mesh, MeshStatusFaultDirection );
            MeshMessageAdd( mesh, "Direction of fault\n" );
            SurfaceMeshMessageAdd( mesh, surf );
            MeshMessageAdd( mesh, "cannot be determined.\nErrors in the meshing stage can be remedied by the use of the Flip Fault flag.\n" );
            MeshMessageClose( mesh );
            SurfaceSetFrontDirSet( mesh, surfaceSet, SurfaceID(surf), frontPosDir );
            /* Loops over surfaces cannot be nested, so start all over again */
            surf = SurfaceSetFirst( surfaceSet, &surfSetIter );
            continue;
         }
      }
      surf = SurfaceSetNext( surfaceSet, &surfSetIter );
   }
   SetDelete( idSetFront );
   SetDelete( idSetBack );
   SetDelete( edgeListWork );
   SetDelete( jointWork );
   SetDelete( pairs );
}

extern void SurfaceSetFaultDirection( 
               void                   *mesh,
               SurfaceSet_t           *surfaceSet,
               PointSet_t             *pointSet )
{

   /* Set the indistinguishable faults */
   if ( !MESH_ERROR( mesh ) ) {
      SetIndistinguishableFault( mesh, surfaceSet );
   }

   /* Set the distinguishable fault directions: simple criterion: smallest angle */
   if ( !MESH_ERROR( mesh ) ) {
      SetDistinguishableFault( mesh, surfaceSet, pointSet );
   }
}

static bool_t BodyCorrect( 
               SurfaceSet_t           *surfaceSet,
               PointSet_t             *pointSet,
               double                  smallLength,
               Body_t                 *body )
{
   const int  bodySize = BodySize( body );
   Iterator_t iter;
   bool_t     isCorrect = TRUE;
   const int  nSignedSurf = 2*SurfaceSetSize( surfaceSet );
   bool_t    *outerBound = DIMALLOC( nSignedSurf*sizeof(*outerBound) );

   UTIL_SET( outerBound, FALSE, nSignedSurf );
   MarkExtremalSurfaces( smallLength, surfaceSet, pointSet, outerBound );

   if ( !bodySize ) {
     isCorrect = FALSE;
   } else if ( bodySize == 1 ) {
      /* Reject if only front or back */
      BodySurface_t       *bs = BodyFirst( body, &iter );
      const Surface_t     *surface = BodySurfaceGet( bs );
      if ( SurfaceType( surface ) != surfNormal ) {
         isCorrect = FALSE;
      }
   } else if ( BodyTypeGet( body ) == bodyDegenerate ) {
      /* All surfaces should have wrong direction */
      BodySurface_t       *bs = BodyFirst( body, &iter );
      while ( bs && isCorrect ) {
        const Surface_t  *fault = BodySurfaceGet( bs );
        const bool_t      posDir = BodySurfacePosDir( bs );
#if 0
fprintf( stderr, "\nBodyCorrect posDir=%d active=%d\n", posDir, SurfaceActive( fault, posDir ) );
SurfaceInfoPrint( fault, stderr );
#endif
        isCorrect = !SurfaceActive( fault, posDir );
        bs = BodyNext( body, &iter );
     }
   } else if ( BodyTypeGet( body ) == bodyNormal ) {
      /* None of the surfaces on outer boundary */
      BodySurface_t       *bs = BodyFirst( body, &iter );
      while ( bs && isCorrect ) {
        const Surface_t  *surf = BodySurfaceGet( bs );
        if ( SurfaceType( surf ) == surfNormal ) {
           const bool_t      posDir = BodySurfacePosDir( bs );
           isCorrect = !outerBound[ SurfaceSetSignedSurfId( surfaceSet, surf, posDir  ) ];
        }
        bs = BodyNext( body, &iter );
    }
  }
  DIFREE( outerBound );
  return isCorrect;
}

static void GenerateBodies( 
               void                   *mesh,
               TetGraph_t                *graph,
               SurfaceSet_t           *surfaceSet,
               PointSet_t             *pointSet,
               BodySet_t              *bodySet )
{

   /* Generate bodies from surface connectivity */
   int            n, numBody;
   int           *bodyId = TetGraphGetConnectivity( graph, &numBody );
   Body_t       **bodyList = DIMALLOC( numBody*sizeof(*bodyList) );
   Iterator_t     iter;
   SurfaceData_t *data = RBTreeFirst( surfaceSet->surface, &iter );

   for ( n = 0; n < numBody; n++ ) {
      bodyList[n] = BodyCreate( mesh );
   }

   while( data ) {
      Surface_t   *surf = data->surface;
      const bool_t posDir = TRUE;
      const bool_t negDir = FALSE;
      const int    posId = SurfaceSetSignedSurfId( surfaceSet, surf, posDir  );
      const int    negId = SurfaceSetSignedSurfId( surfaceSet, surf, negDir );
         BodyAdd( bodyList[bodyId[posId]], surf, posDir );
         BodyAdd( bodyList[bodyId[negId]], surf, negDir );
      data = RBTreeNext( surfaceSet->surface, &iter );
   }

   for ( n = 0; n < numBody; n++ ) {
      bool_t isCorrect;
      BodyTypeSet( bodyList[n] );
      isCorrect =  BodyCorrect( surfaceSet, pointSet, MeshTolerance( mesh ), bodyList[ n ] );
      if ( isCorrect ) {
         BodySetAdd( bodySet, bodyList[n] );
      } else {
         /* Delete empty bodies (wrong fault faces) */
         BodyDelete( bodyList[n] );
      }
   }

   /* Check if all surfaces are now in a body */
   if ( !MESH_ERROR( mesh ) ) {
      Iterator_t       surfSetIter;
      Surface_t       *surface = SurfaceSetFirst( surfaceSet, &surfSetIter );
      SurfaceSet_t    *checkSet = SurfaceSetCreate( );
      Iterator_t       bodyIter;
      Body_t          *body = BodySetFirst( bodySet, &bodyIter );
      while ( body ) {
         SurfaceSetAddBody( checkSet, body );
         body = BodySetNext( bodySet, &bodyIter  );
      }
 
      while ( surface ) {
         if ( !SurfaceSetTest( checkSet, surface ) ) {
            MeshMessageOpen( mesh, MeshStatusSurfaceNotInBody );
            MeshMessageAdd( mesh, "Surface\n" );
            SurfaceMeshMessageAdd( mesh, surface );
            MeshMessageAdd( mesh, "not in body.\nProbably due to poor matching between surfaces." );
            MeshMessageClose( mesh );
         }
         surface = SurfaceSetNext( surfaceSet, &surfSetIter );
      }

      SurfaceSetClear( checkSet );
      SurfaceSetDelete( checkSet );
   }

   DIFREE( bodyList );
   DIFREE( bodyId );
}


static void SurfaceSetFillSingleBodySet( 
               void                   *mesh,
               SurfaceSet_t           *surfaceSet, 
               PointSet_t             *pointSet, 
               BodySet_t              *bodySet )
{
   TetGraph_t  *graph = TetGraphInit( TetGraphCreate(), 2*SurfaceSetSize( surfaceSet ) );
   bool_t    isModelOuter = FALSE;

   /* Check if this set contains the outer edge of the model */
   {
      Iterator_t         iter;
      const Surface_t   *surf = SurfaceSetFirst( surfaceSet, &iter );
      while ( surf && !isModelOuter ) {
         isModelOuter =  SurfaceAtOuterBoundary( surf );
         surf = SurfaceSetNext( surfaceSet, &iter );
     }
   }

   /* Derive surface connectivity from common edges */
   if ( !MESH_ERROR( mesh ) ) {
      SurfaceSetAnalyzeJoints( mesh, pointSet, surfaceSet, graph );
   }

   /* Generate bodies from surface connectivity */
   if ( !MESH_ERROR( mesh ) ) {
      GenerateBodies( mesh, graph, surfaceSet, pointSet, bodySet );
   }

   TetGraphDelete( graph );
}

static Set_t *SurfaceSetDisconnect( void *mesh, SurfaceSet_t   *surfaceSet )
{
   Edge_t         *edge = SurfaceSetEdge( surfaceSet );
   TetGraph_t        *graph = TetGraphCreate();
   Set_t          *result = SetCreate( 0, sizeof( SurfaceSet_t * ) );

   UNUSED(mesh);

   /* Get connectivity of surfaces in surfaceSet */
   {
      Iterator_t    iter;
      EdgeKey_t    *key = EdgeKeyFirst( edge, &iter );
      Set_t        *edgeListSet = SetCreate( 0, sizeof(EdgeList_t) );
      while ( key ) {
         const int   listLen = EdgeListSize( edge, key, NULL );
         EdgeList_t *edges = SetNew( SetClear(edgeListSet), listLen );
         int         e0, e1;
   
         EdgeListGet( edge, key, edges, NULL );
         for ( e0 = 0; e0 < listLen; e0++ ) {
            int    surfId0 = SurfaceSetSurfaceId( surfaceSet, EdgeListSurface( edges+e0 ) );
            TetGraphAdd( graph, surfId0, surfId0 );
            for ( e1 = 0; e1 < listLen; e1++ ) {
               int    surfId1 = SurfaceSetSurfaceId( surfaceSet, EdgeListSurface( edges+e1 ) );
               TetGraphAdd( graph, surfId0, surfId1 );
            }
         }
         key = EdgeKeyNext( edge, &iter );
      }
      SetDelete( edgeListSet );
   }

   /* Output a list of surface sets, that will be analyzed separately */
   {
      int            nConnectSurf, n;
      int           *connectSurf = TetGraphGetConnectivity( graph, &nConnectSurf );
      SurfaceSet_t **list = SetNew( result, nConnectSurf );
      Iterator_t     iter;
      Surface_t     *surface = SurfaceSetFirst( surfaceSet, &iter );
      for ( n = 0; n < nConnectSurf; n++ ) {
         list[ n ] = SurfaceSetCreate( );
      }
      while ( surface ) {
         int    surfId = SurfaceSetSurfaceId( surfaceSet, surface );
         SurfaceSetAdd( list[connectSurf[surfId]], surface );
         surface = SurfaceSetNext( surfaceSet, &iter );
      }
      DIFREE( connectSurf );
   }

   TetGraphDelete( graph );

   return result;
} 

extern void SurfaceSetFillBodySet( 
               void                   *mesh,
               SurfaceSet_t           *surfaceSet, 
               PointSet_t             *pointSet, 
               BodySet_t              *bodySet )
{
   int        n;
   Set_t     *disjointSurfaceSet = SurfaceSetDisconnect( mesh, surfaceSet );
   const int  nDisjointSurf = SetSize( disjointSurfaceSet );

   for ( n = 0; n  < nDisjointSurf && !MESH_ERROR( mesh ); n++ ) {
      SurfaceSet_t **surfSetP = SetElmGet( disjointSurfaceSet, n );
      SurfaceSet_t  *surfSet = *surfSetP;
      BodySet_t *disjointBodySet = BodySetCreate();

      if ( !MESH_ERROR( mesh ) ) {
         SurfaceSetFillSingleBodySet( mesh, surfSet, pointSet, disjointBodySet );
      }

      /* Add the created bodies to the full set */
      {
         Iterator_t iter;
         Body_t    *body = BodySetFirst( disjointBodySet, &iter );
         while ( body && !MESH_ERROR( mesh ) ) {
            BodyGroupSet( body, n );
            BodySetAdd( bodySet, BodyCopy( mesh, body ) );
            body = BodySetNext( disjointBodySet, &iter );
         }
      }
      BodySetDelete( disjointBodySet );
   }

   /* Cleanup */
   for ( n = 0; n  < nDisjointSurf; n++ ) {
      SurfaceSet_t **surfSetP = SetElmGet( disjointSurfaceSet, n );
      SurfaceSetClear( *surfSetP );
      SurfaceSetDelete( *surfSetP );
   }
   SetDelete( disjointSurfaceSet );
}

extern int SurfaceSetSurfaceId( SurfaceSet_t *surfaceSet, const Surface_t *surface )
{
   int            id = SurfaceSetDataId( surfaceSet, surface );
   assert( id >= 0 );
   return id;
}

extern Surface_t *SurfaceSetGet( SurfaceSet_t *surfaceSet, int indx )
{
   SurfaceData_t *data = RBTreeGet(surfaceSet->surface, indx);
   return data->surface;
}

static int LinkFaultPoints( 
               XPointSet_t            *xpointSet,
               const EdgeKey_t        *key,
               const Set_t            *pairSet,
               int                     numFault,
               bool_t                  skipOuter2Outer )
{
  int       n;
  int       numChange = 0;
  for ( n = 0; n < SetSize( pairSet ); n++ ) {
    const JointData_t *joint = SetElmGet( pairSet, n );
    const Surface_t   *surf1 = EdgeListSurface( joint->edge1 );
    const Surface_t   *surf2 = EdgeListSurface( joint->edge2 );

    if ( !skipOuter2Outer || !( SurfaceIsOuterBoundary(surf1, joint->posDir1) && SurfaceIsOuterBoundary(surf2, joint->posDir2 ) ) ) {
      const int id1 = EdgeListTriangleId( joint->edge1 );
      const int id2 = EdgeListTriangleId( joint->edge2 );
      int       p1, p2;
      EdgeKeyPoints( key, &p1, &p2 );
      if ( ( SurfaceType( surf1 ) != surfNormal ) && ( SurfaceType( surf2 ) != surfNormal )  ) {
        if ( !SurfaceFrontBackOfSameFault( surf1, surf2 ) ) {
          numChange += XPointSetLinkFault( xpointSet, p1, surf1, id1, surf2, id2 );
          numChange += XPointSetLinkFault( xpointSet, p2, surf1, id1, surf2, id2 );
        }
      } else if ( ( SurfaceType( surf1 ) == surfNormal ) != ( SurfaceType( surf2 ) == surfNormal ) ) {
        /* 1 normal surface and 1 fault: link normal to fault */
#if 0
        if ( !(SurfaceOuterOrOpenFault( surf1 ) && SurfaceOuterOrOpenFault( surf2 ) ) ) {
#else
    {
#endif
          if ( SurfaceType( surf1 ) == surfNormal ) {
            numChange += XPointSetLinkNormal( xpointSet, p1, surf1, id1, XPointSetFind( xpointSet, p1, surf2, id2 ) );
            numChange += XPointSetLinkNormal( xpointSet, p2, surf1, id1, XPointSetFind( xpointSet, p2, surf2, id2 ) );
          } else {
            numChange += XPointSetLinkNormal( xpointSet, p1, surf2, id2, XPointSetFind( xpointSet, p1, surf1, id1 ) );
            numChange += XPointSetLinkNormal( xpointSet, p2, surf2, id2, XPointSetFind( xpointSet, p2, surf1, id1 ) );
          }
        }
      } else {
        /* Close if there is no fault or if both not on outer boundary */
        if ( !numFault || !(SurfaceOuterOrOpenFault( surf1 ) && SurfaceOuterOrOpenFault( surf2 ) ) ) {
          numChange += XPointSetLinkNormal2( xpointSet, p1, surf1, id1, surf2, id2 );
          numChange += XPointSetLinkNormal2( xpointSet, p2, surf1, id1, surf2, id2 );
        }
      }
    }
  }
  return numChange;
}

extern void SurfaceSetLinkFaultPoints( 
               void                   *mesh,
               PointSet_t             *pointSet,
               XPointSet_t            *xpointSet,
               SurfaceSet_t           *surfaceSet,
               bool_t                  skipOuter2Outer )
{
   Set_t        *edgeListWork = SetCreate( 100, sizeof(EdgeList_t) );
   Set_t        *jointWork = SetCreate( 100, sizeof(JointData_t) );
   Set_t        *pairs = SetCreate( 100, sizeof(JointData_t) );
   int           numChange = 1; /* Enter loop first time */

   while ( !MESH_ERROR( mesh ) && numChange ) {
     Edge_t       *edge = SurfaceSetEdge( surfaceSet );
     Iterator_t    iter;
     EdgeKey_t    *key = EdgeKeyFirst( edge, &iter );
     numChange = 0;
     while ( key && !MESH_ERROR( mesh ) ) {
        int          p1, p2, numFault;
        const int    numPolygon = EdgeListSize( edge, key, &numFault );

        EdgeKeyPoints( key, &p1, &p2 );
        if ( ( numFault || XPointSetPointInAnyFault( xpointSet, p1 ) || 
                           XPointSetPointInAnyFault( xpointSet, p2 ) ) && 
               !MESH_ERROR( mesh ) ) {
          /* Fault is split, find pairs */
          EdgeList_t  *edges = SetNew( SetClear(edgeListWork), numPolygon );
          EdgeListGet( edge, key, edges, NULL );
          AnalyzeSingleJoint( mesh, FALSE, edge, key, edges, numPolygon, pointSet,
                              jointWork, surfaceSet, pairs );
          numChange += LinkFaultPoints( xpointSet, key, pairs, numFault, skipOuter2Outer );
      }
      key = EdgeKeyNext( edge, &iter );
   }
   }

   SetDelete( edgeListWork );
   SetDelete( jointWork );
   SetDelete( pairs );
}

extern Set_t *SurfaceSetAllID( SurfaceSet_t *surfaceSet )
{
   Set_t     *idSet = SetCreate( 0, sizeof(int) );
   Iterator_t iter;
   Surface_t *surface = SurfaceSetFirst( surfaceSet, &iter );
   /* Get id's of surfaces */
   while ( surface ) {
      const int   surfId = SurfaceID( surface );
      SetAdd( idSet, &surfId, 1 );
      surface = SurfaceSetNext( surfaceSet, &iter );
   }
   /* Remove double entries */
   SetSort( idSet, CompareInt, TRUE );
   return idSet;
}


extern SurfaceSet_t *SurfaceSetSameID( SurfaceSet_t *surfaceSet, int ID )
{
   SurfaceSet_t  *result = SurfaceSetCreate();
   Iterator_t     iter;
   Surface_t     *surface = SurfaceSetFirst( surfaceSet, &iter );
   /* Get id's of surfaces */
   while ( surface ) {
      if ( SurfaceID( surface ) == ID ) {
         SurfaceSetAdd( result, surface );
      }
      surface = SurfaceSetNext( surfaceSet, &iter );
   }
   return result;
}

extern int SurfaceSetFreeID( SurfaceSet_t *surfaceSet )
{
  int  id = 12345;
  bool_t ready = FALSE;
  while( !ready ) {
    SurfaceSet_t *surfSet = SurfaceSetSameID( surfaceSet, id );
    if ( !SurfaceSetSize(surfSet) ) {
      ready = TRUE;
    } else {
      id += 1;
    }
    SurfaceSetDelete( SurfaceSetClear( surfSet ) );
  }
  return id;
}

/* Mark the outer boundary of the full model */
extern void SurfaceSetMarkOuterBoundary( 
                  void                       *mesh, 
                  bool_t                     faultsOpen,
                  PointSet_t                 *pointSet, 
                  SurfaceSet_t               *surfaceSet )
{
  bool_t    *outerBound = SurfaceSetOuterBound( mesh, faultsOpen, surfaceSet, pointSet );
  Iterator_t iter;
  Surface_t *surface = SurfaceSetFirst( surfaceSet, &iter );
  while ( surface ) {
    const int    posId = SurfaceSetSignedSurfId( surfaceSet, surface, TRUE  );
    const int    negId = SurfaceSetSignedSurfId( surfaceSet, surface, FALSE );
    SurfaceOuterBoundarySet( surface, surfOuterUnknown );
    if ( outerBound[posId] ) {
      SurfaceOuterBoundarySet( surface, surfOuterPosDir );
    }
    if ( outerBound[negId] ) {
      SurfaceOuterBoundarySet( surface, surfOuterNegDir );
    }
    if ( SurfaceOuterBoundaryGet( surface ) == surfOuterUnknown ) {
      SurfaceOuterBoundarySet( surface, surfOuterNot );
    }
    surface = SurfaceSetNext( surfaceSet, &iter );
  }
  DIFREE( outerBound );
}



static void GetFaultPair(
                  Edge_t                     *edge,
                  EdgeKey_t                  *key,
                  const Surface_t           **front,
                  const Surface_t           **back )
{
  EdgeList_t  *list = EdgeListFirst( edge, key );
  *front = NULL;
  *back  = NULL;
  while ( list ) {
    const Surface_t   *surf = EdgeListSurface( list );
    switch ( SurfaceType( surf ) ) {
      case surfFrontFault:
        *front = surf;
        break;
      case surfBackFault:
        *back = surf;
        break;
      default:
        break;
    }
    list = EdgeListNext( edge, list );
  }
}

/*
 * Analysis of a single edge.  Check if the fault is closed here
 */
extern bool_t SurfaceSetClosedFault(
                  void                       *mesh,
                  SurfaceSet_t               *surfaceSet,
                  Edge_t                     *edge,
                  EdgeKey_t                  *key )
{
  bool_t       isClosed = FALSE;
  int          nFault;
  const int    nEdge = EdgeListSize( edge, key, &nFault );
  const int    nNormal = nEdge - nFault;

  UNUSED(mesh);
  UNUSED(surfaceSet);

  /* Any faults here? */
  if ( nFault == 2 ) {
    if ( EdgeIsolatedFault( edge, key ) ) {
      isClosed = TRUE;
    } else if ( nNormal ) {
      /* Normal->fault transition ? */
      const Surface_t  *front = NULL, *back  = NULL;
      GetFaultPair( edge, key, &front, &back );
      if ( SurfaceFrontBackOfSameFault( front, back ) ) {
        if ( nNormal == 1 ) {
          isClosed = TRUE;
        } else {
          /* OK fault seems to end on normal surface, closed if not on outer boundary */
          EdgeList_t  *list = list = EdgeListFirst( edge, key );
          while ( list && !isClosed ) {
            Surface_t   *surf = EdgeListSurface( list );
            if ( SurfaceType( surf ) == surfNormal && !SurfaceOuterOrOpenFault( surf ) ) {
              isClosed = TRUE;
            } 
            list = EdgeListNext( edge, list );
          }
        }
      }
    }
  }
  return isClosed;
}

extern void SurfaceSetAddBody(
                  SurfaceSet_t               *surfaceSet,
                  Body_t                     *body )
{
  Iterator_t           iter;
  BodySurface_t       *bs = BodyFirst( body, &iter );
  while ( bs ) {
    Surface_t  *surf = BodySurfaceGet( bs );
    if ( !SurfaceSetTest( surfaceSet, surf ) ) {
      SurfaceSetAdd( surfaceSet, surf );
    }
    bs = BodyNext( body, &iter );
  }
}

extern void SurfaceSetAddSurfaceSet(
                  SurfaceSet_t               *to,
                  SurfaceSet_t               *from,
                  bool_t                      deepCopy )
{
  Iterator_t      iter;
  Surface_t      *surface = SurfaceSetFirst( from, &iter );
  while ( surface ) {
    if ( deepCopy ) {
      SurfaceSetAdd( to, SurfaceCopy( surface ) );
    } else {
      SurfaceSetAdd( to, surface );
    }
    surface = SurfaceSetNext( from, &iter );
  }
}


/* Print a surfaceset into seperate Gocad files */
extern void SurfaceSetToGocad(
                  SurfaceSet_t               *surfaceSet,
                  PointSet_t                 *pointSet )

{
  Iterator_t      iter;
  Surface_t      *surface = SurfaceSetFirst( surfaceSet, &iter );
  while ( surface ) {
    SurfaceToGocad( surface, pointSet );
    surface = SurfaceSetNext( surfaceSet, &iter );
  }
}

extern void SurfaceSetPrintConnectivity(
                  SurfaceSet_t               *surfaceSet,
                  FILE                       *file )
{
  RBTree_t     *tree = RBTreeCreate( sizeof(SurfacePair_t),  SurfacePairCompare );
  Edge_t       *edge = SurfaceSetEdge( surfaceSet );
  Iterator_t    iter;
  EdgeKey_t    *key = EdgeKeyFirst( edge, &iter );
  Set_t        *edgeListSet = SetCreate( 100, sizeof(EdgeList_t) );

  while ( key ) {
    if ( !EdgeInternal( edge, key ) ) {
      int   p0, p1;
      const int    numPolygon = EdgeListSize( edge, key, NULL );
      EdgeList_t *edges = SetNew( SetClear(edgeListSet), numPolygon );
      EdgeListGet( edge, key, edges, NULL );
      for ( p0 = 0; p0 < numPolygon; p0++ ) {
        for ( p1 = p0+1; p1 < numPolygon; p1++ ) {
          const Surface_t *surf1 = EdgeListSurface( edges+p0 );
          const Surface_t *surf2 = EdgeListSurface( edges+p1 );
          if ( !SurfaceFrontBackOfSameFault( surf1, surf2 ) ) {
            const int        cmp = SurfaceComp( surf1, surf2 );
            if ( cmp ) {
              SurfacePair_t   surfPair;
              SurfacePairInit( &surfPair, surf1, surf2 );
              if ( !RBTreeFindGet( tree, &surfPair ) ) {
                RBTreeSearch( tree, &surfPair );
              }
            }
          }
        }
      }
    }
    key = EdgeKeyNext( edge, &iter );
  }

  /* Output connectivity */
  {
    const SurfacePair_t   *surfPair = RBTreeFirst( tree, &iter );
    while ( surfPair ) {
      fprintf( file, "Connection between %s and %s\n", SurfaceUserName(surfPair->surf1), SurfaceUserName(surfPair->surf2) );
      surfPair = RBTreeNext( tree, &iter );
    }
  }
  
  SetDelete( edgeListSet );
  RBTreeDelete( tree );
}

extern const Surface_t *SurfaceSetFind(
                  SurfaceSet_t               *surfaceSet,
                  int                         userID,
                  int                         part,
                  SurfaceType_t               type )
{
  Iterator_t            iter;
  const Surface_t      *surface = SurfaceSetFirst( surfaceSet, &iter );
  while ( surface ) {
    if ( SurfaceID( surface ) == userID && SurfacePartGet( surface ) == part && SurfaceType( surface ) == type ) {
      break;
    }
    surface = SurfaceSetNext( surfaceSet, &iter );
  }
  return surface;
}

extern SurfaceSet_t *SurfaceSetSameType( 
                  SurfaceSet_t               *surfaceSet, 
                  SurfaceType_t               surfaceType )
{
   SurfaceSet_t  *result = SurfaceSetCreate();
   Iterator_t     iter;
   Surface_t     *surface = SurfaceSetFirst( surfaceSet, &iter );
   /* Get id's of surfaces */
   while ( surface ) {
      if ( SurfaceType( surface ) == surfaceType ) {
         SurfaceSetAdd( result, surface );
      }
      surface = SurfaceSetNext( surfaceSet, &iter );
   }
   return result;
}


extern void SurfaceSetMeshMessageAdd( 
                  void                       *meshV, 
                  SurfaceSet_t               *surfaceSet )
{
  Iterator_t         iter;
  const Surface_t  *surface = SurfaceSetFirst( surfaceSet, &iter );
  while ( surface ) {
    SurfaceMeshMessageAdd( meshV, surface );
    surface = SurfaceSetNext( surfaceSet, &iter );
  } 
}

