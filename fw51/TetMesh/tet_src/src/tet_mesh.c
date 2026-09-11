/*                                         Copyright (c) 2011 TNO DIANA BV */
#include "tet_point.h"
#include "tet_triangle.h"
#include "tet_graph.h"
#include "tet_surface.h"
#include "RBTree.h"
#include "tet_bodyset.h"
#include "tet_mesh.h"
#include "tet_femgv.h"
#include "tet_utils.h"
#include "tet_triangle.h"
#include "tet_surfaceset.h"
#include "tet_edge.h"
#include "BoundBox.h"
#include "tet_xpoint.h"
#include "tet_tetrahedron.h"
#include "tet_meshtriangle.h"
#ifdef MRH_DEVELOP
#include "tet_MeshEdit.h"
#endif

typedef struct SurfIdType_t  {
   int             ID;
   SurfaceType_t   type;
   char            name[BUFSIZ];
} SurfIdType_t;

typedef struct Message_t  {
   int      status;
   char    *text;
} Message_t;

typedef struct BodyMesh_t {
   Body_t           *body;
   SurfaceSet_t     *sideSurfaces;
   size_t            tetrahedronFirst;
   size_t            tetrahedronSize;
} BodyMesh_t;

typedef struct MeshOutput_t {
   RBTree_t          *node2Point;
   Set_t             *bodyMesh;
   TetrahedronSet_t  *tetSet;
} MeshOutput_t;

/* Temporary data for reading in the model */
typedef struct MeshReadBuffer_t {
   Surface_t      *frontSurface;
   Surface_t      *backSurface;
   BoundBoxTree_t *pointTree;
   int            *pointId;
   double         *pointCor;
   Set_t          *workId;
   bool_t          flipFault;
} MeshReadBuffer_t;

/* Original model */
typedef struct MeshModelData_t {
   SurfaceSet_t   *surfaceSet;
   PointSet_t     *pointSet;
   double          pointTolerance;
   RBTree_t       *surfIdTypeTree;
   int             maxInterfaceIdx;  /* Maximal interface idx nr. for faults */
   double          angleWarningCrit; /* Generate message if angle between 2 triangles less than this angle */
} MeshModelData_t;

/* Data derived from the model data */
typedef struct MeshDerivedData_t {
   PointSet_t     *pointSet;
   SurfaceSet_t   *surfaceSet;
   BodySet_t      *bodySet;
   FemGvPath_t     FemGvPath;
   double          shiftScaleFactor;
} MeshDerivedData_t;

/* Data related to error messaging */
typedef struct MeshErrorData_t {
   Set_t          *messages; /* Set of Message_t */
   bool_t          errFlag;
   bool_t          messageOpen;
} MeshErrorData_t;

typedef struct Mesh_t {
   MeshModelData_t   *modelData;
   MeshReadBuffer_t  *readBuffer;
   MeshDerivedData_t *derivedData;
   MeshErrorData_t   *errorData;
   MeshOutput_t      *meshOutput;
} Mesh_t;


#define REGION_INVALID_ID  (-1)
#define FGV_FILE_NAME    "fgvstr.str"
#define MESH_EDIT_SURFACES_NAME    "MeshEditSurfaces.dat"
#define MESH_EDIT_WIREFRAME__NAME    "MeshEditWireFrame.dat"

typedef struct ShiftPoint_t  {
   int             pointId;
   int             orgId;
} ShiftPoint_t;

typedef RBTree_t   ShiftPointSet_t;


static int ShiftPointComp( const void* data0, const void* data1 )
{
   const ShiftPoint_t *sp0 = data0;
   const ShiftPoint_t *sp1 = data1;

   return UTIL_COMPARE( sp0->pointId, sp1->pointId );
}

static ShiftPointSet_t *ShiftPointSetCreate( void )
{
  return RBTreeCreate( sizeof( ShiftPoint_t ), ShiftPointComp );
}

static ShiftPointSet_t *ShiftPointSetDelete( 
                  ShiftPointSet_t            *spSet )
{
  return RBTreeDelete( spSet );
}

static const ShiftPoint_t *ShiftPointSetFind(
                  ShiftPointSet_t            *spSet,
                  int                         pointId )
{
  ShiftPoint_t sp;
  sp.pointId = pointId;
  return RBTreeFindGet( spSet, &sp );
}

/*
#if 1
static void ShiftPointSetPrint(
                  ShiftPointSet_t            *spSet,
                  FILE                       *fp )
{
  Iterator_t          iter;
  const ShiftPoint_t *sp = RBTreeFirst( spSet, &iter );
  fprintf( fp, "Shifted Fault Points: %d\n", (int)RBTreeSize( spSet ) );
  while ( sp ) {
  fprintf( fp, " %d %d\n", sp->pointId, sp->orgId );
  sp = RBTreeNext( spSet, &iter );
  }
}
#endif
*/

static int ShiftPointGetOrgId(
                  const ShiftPoint_t         *sp )
{
  return sp->orgId;
}

static int ShiftPointSetGetOrgId(
                  ShiftPointSet_t            *spSet,
                  int                         pointId )
{
  const ShiftPoint_t *sp = ShiftPointSetFind( spSet, pointId );
  assert( !sp || !ShiftPointSetFind( spSet, ShiftPointGetOrgId( sp ) ) );
  return ( sp ? ShiftPointGetOrgId( sp ) : pointId );
}

static const ShiftPoint_t *ShiftPointSetAdd(
                  ShiftPointSet_t            *spSet,
                  int                         pointId,
                  int                         orgId )
{
  ShiftPoint_t sp;
  assert( pointId >= 0 && orgId >= 0 && pointId >= orgId );
  assert( !ShiftPointSetFind( spSet, pointId ) );
  assert( !ShiftPointSetFind( spSet, orgId ) );
  UTIL_ZERO( &sp );
  sp.pointId = pointId;
  sp.orgId = orgId;
  return RBTreeSearchGet( spSet, &sp );
}

typedef struct BodyInner_t {
   Body_t    *body;
   Set_t     *innerBodies;
   BodySet_t *cavities;
}  BodyInner_t;

static int BodyInnerComp( const void* data0, const void* data1 )
{
   const BodyInner_t *bi0 = data0;
   const BodyInner_t *bi1 = data1;

   return BodyComp( bi0->body, bi1->body );
}

static BodyInner_t *BodyInnerGet( void *bodyInnertree, Body_t *body, int *bodyId )
{
   int           id = -1;
   BodyInner_t   bi;
   bi.body = body;
   id = RBTreeFind( bodyInnertree, &bi );
   assert( id != RBTREE_INVALID_ID );
   if ( bodyId ) *bodyId = id;
   return RBTreeGet( bodyInnertree, id );
}


/* Split the inner bodies in connected clusters */

typedef struct Surf2Bodies_t {
   Surface_t       *surf;
   Set_t           *bodySetIndex;
} Surf2Bodies_t;


static int Surf2BodiesComp( const void* data0, const void* data1 )
{
   const Surf2Bodies_t *sb0 = data0;
   const Surf2Bodies_t *sb1 = data1;
   return SurfaceComp( sb0->surf, sb1->surf );
}

static void Surf2BodiesAdd( RBTree_t *surfBodyTree, Body_t *body, const int bodyId )
{
   Iterator_t          iter;
   BodySurface_t      *bs = BodyFirst( body, &iter );
   while ( bs ) {
      Surface_t       *surf = BodySurfaceGet( bs );
      Surf2Bodies_t    s2b, *s2bP;
      s2b.surf = surf;
      s2bP = RBTreeFindGet( surfBodyTree, &s2b );
      if ( s2bP != NULL ) {
         SetAdd( s2bP->bodySetIndex, &bodyId, 1 );
      } else {
         s2b.bodySetIndex = SetCreate( 1, sizeof( int ) );
         SetAdd( s2b.bodySetIndex, &bodyId, 1 );
         RBTreeSearch( surfBodyTree, &s2b );
      }
      bs = BodyNext( body, &iter );
   }
}

static Set_t *ClusterInnerBodies(  
              void                *mesh,
              Set_t               *innerBodySet )
{
   Set_t     *bodyCluster = SetCreate( 0, sizeof( BodySet_t * ) );
   const int  nInnerBody = SetSize( innerBodySet );

   UNUSED(mesh);

   if ( nInnerBody ) {
      Body_t   **innerBodies = SetGet( innerBodySet );
      RBTree_t  *surfBodyTree = RBTreeCreate( sizeof( Surf2Bodies_t ), Surf2BodiesComp );
      Surf2Bodies_t    *s2bP = NULL;
      TetGraph_t   *graph = TetGraphCreate();
      int       *clusterId = NULL;
      int        n, nCluster;
      Iterator_t iter;
   
      /* Loop over the bodies, and set surface->body connectivity */
      for ( n = 0; n < nInnerBody; n++ ) {
         Body_t             *body = innerBodies[ n ];
         Surf2BodiesAdd( surfBodyTree, body, n );
      }
      
      /* Loop over the surfaces and build a graph of the body-body connectivity */
      s2bP = RBTreeFirst( surfBodyTree, &iter );
      while ( s2bP ) {
         const int nBodyIndex = SetSize( s2bP->bodySetIndex );
         /* This surface connects at least 2 bodies */
         const int  *bodyIds = SetGet( s2bP->bodySetIndex );
         int         n1, n2;
         /* Quadratic loop to set up connectivity */
         for ( n1 = 0; n1 < nBodyIndex; n1++ ) {
      for ( n2 = n1; n2 < nBodyIndex; n2++ ) {
               TetGraphAdd( graph, bodyIds[n1], bodyIds[n2] );
      }
         }
         s2bP->bodySetIndex = SetDelete( s2bP->bodySetIndex );
         s2bP = RBTreeNext( surfBodyTree, &iter );
      }
   
      /* Set up the clusters */
      clusterId = TetGraphGetConnectivity( graph, &nCluster );
      
      /* Convert clusterId to a list of BodySets  */
      for ( n = 0; n < nCluster; n++ ) {
         BodySet_t *bc = BodySetCreate( );
         SetAdd( bodyCluster, &bc, 1 );
      }
      for ( n = 0; n < nInnerBody; n++ ) {
         Body_t         *body = innerBodies[ n ];
         BodySet_t     **bcP = SetElmGet( bodyCluster, clusterId[ n ] );
         BodySetAdd( *bcP, body );
      }
   
      DIFREE( clusterId );
      RBTreeDelete( surfBodyTree );
      TetGraphDelete( graph );
   }
   return bodyCluster;
}

static Body_t *CavityShell(
              void                *mesh,
              BodySet_t           *bodySet )
{
  RBTree_t      *surfBodyTree = RBTreeCreate( sizeof( Surf2Bodies_t ), Surf2BodiesComp );
  const int      nBody = BodySetSize( bodySet );
  Surf2Bodies_t *s2b = NULL;
  Body_t        *cavity = BodyCreate( mesh );
  Iterator_t     iter;
  int            n;

  /* Get surface -> body connectivity */
  for ( n = 0; n < nBody; n++ ) {
  Surf2BodiesAdd( surfBodyTree, BodySetGet( bodySet, n ), n );
  }
  
  /* Put surfaces connected to single body in a new cavity body */
  s2b = RBTreeFirst( surfBodyTree, &iter );
  while ( s2b ) {
  if ( SetSize( s2b->bodySetIndex ) == 1 ) {
      BodyAdd( cavity, s2b->surf, TRUE );
  }
  SetDelete( s2b->bodySetIndex );
  s2b = RBTreeNext( surfBodyTree, &iter );
  }
  RBTreeDelete( surfBodyTree );
  return cavity;
}

static RBTree_t *BodyInnerCreate( 
                  void                       *mesh,
                  BodySet_t                  *bodySet )
{
  RBTree_t    *bodyInnertree = RBTreeCreate( sizeof( BodyInner_t ), BodyInnerComp );
  Iterator_t   iter;
  BodyInner_t *bi = NULL;

  /* Create memory for inner bodies */ 
  Body_t *body = BodySetFirst( bodySet, &iter );
  while ( body ) {
  BodyInner_t   biFind;
  biFind.body = body;
  biFind.innerBodies = SetCreate( 0, sizeof( Body_t * ) );
  biFind.cavities = BodySetCreate();
  RBTreeSearch( bodyInnertree, &biFind );
  body = BodySetNext( bodySet, &iter );
  }


  /* Fill bodies with interior bodies */
  body = BodySetFirst( bodySet, &iter );
  while ( body && !MESH_ERROR( mesh ) ) {
  Body_t *outer = BodyOuterGet( body );
  if ( outer ) {
      bi = BodyInnerGet( bodyInnertree, outer, NULL );
      SetAdd( bi->innerBodies, &body, 1 );
  }
  body = BodySetNext( bodySet, &iter );
  }


  /* Split the inner bodies in connected clusters */
  bi = RBTreeFirst( bodyInnertree, &iter );
  while ( bi ) {
  if ( SetSize( bi->innerBodies ) ) {
      Set_t         *bodyCluster = ClusterInnerBodies( mesh, bi->innerBodies );
      const int      nCluster = SetSize( bodyCluster );
      BodySet_t    **cluster = SetGet( bodyCluster );
      int            n;

      /* Create extra shells for body clusters */
      for ( n = 0; n < nCluster; n++ ) {
    if ( BodySetSize( cluster[ n ] ) ) {
          Body_t  *cavity = CavityShell( mesh, cluster[ n ] );
          BodySetAdd( bi->cavities, cavity );
    }
    BodySetClear( cluster[ n ] );
    BodySetDelete( cluster[ n ] );
      }
      SetDelete( bodyCluster );
  }
  bi = RBTreeNext( bodyInnertree, &iter );
  }

  return bodyInnertree;
}

static RBTree_t *BodyInnerDelete( 
                  RBTree_t                   *bodyInnertree )
{
  if ( bodyInnertree ) {
  Iterator_t   iter;
  BodyInner_t *bi = RBTreeFirst( bodyInnertree, &iter );
  while ( bi ) {
      SetDelete( bi->innerBodies );
      BodySetDelete( bi->cavities );
      bi = RBTreeNext( bodyInnertree, &iter );
  }
  RBTreeDelete( bodyInnertree );
  }
  return NULL;
}

static int SurfIdTypeComp(  const void* data0, const void* data1 )
{
   const SurfIdType_t *s0 = data0;
   const SurfIdType_t *s1 = data1;
   int                 result = UTIL_COMPARE( s0->ID, s1->ID );
   if ( !result ) {
      result = UTIL_COMPARE( s0->type, s1->type );
  }
  return result;
}

static bool_t SurfIdTypeExists(  
              void                  *surfIdTypeTree, 
              int                    ID,
              SurfaceType_t          type ) 
{
   SurfIdType_t  sit;
   bool_t        result = FALSE;
   sit.ID = ID;
   sit.type = type;
   if ( RBTreeFind( surfIdTypeTree, &sit ) != RBTREE_INVALID_ID ) result = TRUE;
  return  result;
}


static void SurfIdTypeAdd(  
              void                  *surfIdTypeTree, 
              const char            *name,
              int                    ID,
              SurfaceType_t          type ) 
{
   SurfIdType_t  sit;
   sit.ID = ID;
   sit.type = type;
   strncpy( sit.name, name, BUFSIZ );
   sit.name[BUFSIZ-1] = '\0';
   RBTreeSearch( surfIdTypeTree, &sit );
}

static MeshReadBuffer_t *MeshReadBufferCreate( void ) 
{
  MeshReadBuffer_t *readBuffer = DIMALLOC( sizeof(*readBuffer) );
  UTIL_ZERO( readBuffer );
  readBuffer->pointTree = BoundBoxTreeCreate();
  return readBuffer;
}

static MeshDerivedData_t *MeshDerivedDataCreate( void )
{
  MeshDerivedData_t *derivedData = DIMALLOC( sizeof(*derivedData) );
  UTIL_ZERO( derivedData );
  return derivedData;
}

static MeshErrorData_t *MeshErrorDataCreate( void )
{
  MeshErrorData_t *errorData = DIMALLOC( sizeof(*errorData) );
  UTIL_ZERO( errorData );
  errorData->messages = SetCreate( 0, sizeof( Message_t ) );
  return errorData;
}

static MeshErrorData_t *MeshErrorDataCopy( 
                  MeshErrorData_t            *from )
{
  MeshErrorData_t *to = DIMALLOC( sizeof(*to) );
  const int        nMessages = SetSize( from->messages );
  int              n;
  /* Shallow copy */
  UTIL_COPY( to, from, 1 );
  to->messages = SetCreate( nMessages, sizeof( Message_t ) );
  /* Deep copy */
  for ( n = 0; n < nMessages; n++ ) {
  const Message_t   *fromMessage = SetElmGet( from->messages, n );
  Message_t         *toMessage = SetNew( to->messages, 1 );
  UTIL_COPY( toMessage, fromMessage, 1 );
  toMessage->text = DIMALLOC( strlen(fromMessage->text)+1 );
  strcpy( toMessage->text, fromMessage->text );
  }
  return to;
}

static MeshModelData_t *MeshModelDataCreate( double pointTolerance )
{
  MeshModelData_t *modelData = DIMALLOC( sizeof(*modelData) );
  UTIL_ZERO( modelData );
  modelData->surfaceSet = SurfaceSetCreate();
  modelData->pointSet = PointSetCreate( pointTolerance/1000 );
  modelData->pointTolerance = pointTolerance; 
  modelData->surfIdTypeTree = RBTreeCreate( sizeof(SurfIdType_t), SurfIdTypeComp );
  modelData->angleWarningCrit = 5. /* Angle in degrees */;
  return modelData;
}

/*
 * MeshCreate
 * This is the main routine that triggers the tetrahedron mesh generator.
 * The model is defined by triangulated surfaces. The triangles of the surfaces
 * are entered in the routines
 * MeshSurfaceOpen()/MeshSurfaceAdd()/MeshSurfaceClose(), and its points by
 * MeshPointAdd().  Triangles may only intersect by edges (or corner points),
 * with the notably exception of the front and back of the same fault.
 *
 * Usually the surfaces are generated from Gocad files and noise in the nodal
 * coordinates is expected. Therefore the nodes are filtered with a parameter
 * pointTolerance; which is an absolute measure.
 * If two nodes are are less than pointTolerance [m?] apart, then they are the
 * same.
 *
 * Surfaces come in 2 flavours: normal surfaces and faults. 
 *
 * A normal surface seperates 2 formations, or it is part of the outer boundary.
 * Normal surfaces need not be fully connected: they may consist of several
 * loose parts.
 * Every triangle edge of a normal surface must also be part of at least one
 * other triangle (normal or fault), otherwise an error is generated
 * (MeshStatusIsolatedEdge). If this is the case either the model is incomplete,
 * or the pointTolerance is too small, or the model is completely.
 * Normal surfaces may enclose lenses that are not connected to other horizons
 * or aide surfaces.
 *
 * Faults have 2 sides: the front and back. These 2 must be specified
 * seperately, because they may have different nodes and/or triangulations,
 * only at their seam they should agree.
 * The nodes are different to allow slipping of the back and front.
 * No tetrehedra should be generated between the front and back of a fault.
 * Fault may also be part of the outer boundary.
 * Faults must be completely connected (by triangle sides), loose parts are an
 * error.
 * After entering the surfaces the mesh generator is triggered: MeshGenerate().
 * First the model is then analyzed for correctness and the data are reshuffled
 * in more convenient form (MeshPrepareModel), and than the tetrahedra are
 * actualy generated.
 * For the mesh generation we use the external tetgen code from simulog. 
 * This code has its limitations; of course it does not know about
 * faults/normal. But it makes the strong assumption that to any edge exactly
 * 2 triangles must be connected. This excludes the case of 'floating' faults:
 *
 *     N-N-N-N-N-N-N-N-N-N-N 
 *     |                   |
 *     N    F_F_F_F_F      N  
 *     |                   |
 *     N-N-N-N-N-N-N-N-N-N-N
 *
 * but also a normal horizon touching the outer boundary is already problematic:
 *
 *     N-N-N-N-N-N-N-N-N 
 *     |               |
 *     N-N-N-N-N-N-N-N_N  
 *     |               |
 *     N-N-N-N-N-N-N-N-N 
 *
 * A lot tearing, shifting and glueing  is necessary to use this wonderful tool
 * for our purposes. This will be described later.
 *
 * In MeshPrepareModel the following steps are taken:
 *   - some obvious test on consistency
 *   - the minimum distance between 2 points is determined, and if that
 *     distance is relatively small, a warning message is generated.
 *   - the smallest angles in the model are tested, and warning messages are
 *     generated.
 *   - front and back of a fault are tested for identity (makes live a lot
 *     simpler!)
 *   - the triangles in front and back of a fault are oriented oppositely. Also
 *     is determined which side of a front or back is active. If the front and
 *     back are identical then there is no unique choice, so a choice is made.
 *     This choice can be influenced by the flipFault flag in MeshSurfaceOpen().
 *   - test for unexpected triangle intersection
 *   - all surfaces are split into 'simple' surfaces: all triangle edges shared
 *     by 2 different are removes, and the conectivity is determined a new. The
 *     reason for doing this is that a surfaces can seperate more than 1 body,
 *     but simple surfaces are either part of the outer boundary, or they
 *     seperate exactly 2 bodies (notice that surface C is not connected):
 *
 *    *-A-A-A-A-A-A-A-*
 *    |               |
 *    C               C
 *    |               |
 *    C               C
 *    |               |
 *    *-D-D-D-D-D-D-D-*
 *    |               |
 *    C               C
 *    |               |
 *    C               C
 *    |               |
 *    *-B-B-B-B-B-B-B-*
 *
 *    is split into
 *
 *    *-A-A-A-A-A-A-A-*
 *    |               |
 *    C1             C3
 *    |               |
 *    C1             C3
 *    |               |
 *    *-D-D-D-D-D-D-D-*
 *    |               |
 *    C2             C4
 *    |               |
 *    C2             C4
 *    |               |
 *    *-B-B-B-B-B-B-B-*
 *
 * Here a letter denotes an edge between 2 triangles of the same surface, a
 * line a triangle and an asterix an edge between two triangles of different
 * surfaces.
 *
 * We are now ready to start the actual mesh generation, which is done with
 * several tries for hidden parameters, until a valid mesh is found
 * (MeshGenerateTry). 
 *
 * Given the limitations of tetgen we create simple bodies from the set of
 * simple surfaces and mesh them one-by-one.  The body construction is done in
 * ConstructBodies(), and this is not a simple task.
 *
 * First we consider the treatment of faults. As simulog doesnot have the
 * concept of a degenerate body we move the interior front and back points
 * apart, thus creating a cavity. This procedure is not a clumsy as it seams,
 * because we want different nodes at the front and back of a node.
 * The concept of interior points is tricky: the basic question is if a fault
 * opens up or not. A valiant attempt is made in the routine
 * SurfaceSetClosedFault().
 *
 * So in the case of a floating fault
 *
 *     N-N-N-N-N-N-N-N-N-N-N 
 *     |                   |
 *     N                   N  
 *     |                   |
 *     N   BF_BF_BF_BF_BF  N  
 *     |                   |
 *     N                   N  
 *     |                   |
 *     N-N-N-N-N-N-N-N-N-N-N
 *
 * the nodes are seperated
 *
 *
 *     N-N-N-N-N-N-N-N-N-N-N 
 *     |                   |
 *     N     B-B-B-B-B     N  
 *     |    /          \   |
 *     N   *            *  N  
 *     |    \          /   |
 *     N     F-F-F-F-F     N  
 *     |                   |
 *     N-N-N-N-N-N-N-N-N-N-N
 *
 * The determination of the shift is also quite tricky, neither for the
 * direction nor for the size there is an obvious choice. Points are related to
 * several triangles, and the shift is computed from the triangle direction
 * (its normal) and size (square root of area is measure of length). The
 * parameter shiftScaleFactor in MeshGenerate controls the amount of shifting.
 * The problem is clear: too little shifting causes problems in tetgen because
 * front and back points seem identical. But too much shifting may cause
 * triangles to cross (especialy where surfaces cross under small angles).
 * The shifting of front/back node splitting/shifting is implemented in
 * MeshExtraFaultPoints(). Roughly speaking there are the following steps:
 *   - extra triangles/nodes are generated near fault corners
 *     (AddExtraMidPoints()) to handle this situation:
 *
 *
 *               1----2-
 *               |   /|
 *               |  / |
 *               | /  |
 *               3 ---4-
 *               |    |
 *
 *     Because the edge 1<->3 and 1<->2 are not shifted (outer edges of fault)
 *     the nodes 1, 2 and 3 are neither doubled nor shifted, but node 4 is!
 *     The triangle 1-2-3 is now both in front and back, and tetgen will
 *     complain strongly about this anomaly. To handle this situation an extra
 *     node 5 is introduced at the mid-point of 2-3:
 *
 *               1----2-
 *               | \ /|
 *               |  5 |
 *               | / \|
 *               3 ---4-
 *               |    |
 * 
 *     Notice that by this triangle bisection the angle is halved (any better
 *     way?). 
 *   - all fault points are duplicated in DuplicateFaultXPoints().
 *   - at (isolated) outer edges of faults the front and back points are joined
 *     (so 1-3 and 1-2)
 *   - Finally in SurfaceSetLinkFaultPoints() fault-surfaces crossings are
 *     analyzed, and here the going gets though.
 *     For example consider a fault-fault crossing:
 *  
 *                bf  
 *                 |
 *                bf  
 *                 |
 *                bf  
 *                 |
 *          BF-BF-**-BF-BF-BF
 *                 |
 *                bf  
 *                 |
 *                bf  
 *                 |
 *                bf  
 *
 *     is split as 
 *   
 *                b  f  
 *                |  |
 *                b  f  
 *                |  |
 *          B-B-B-*  *-B-B-B
 *
 *          F-F-F-*  *-F-F-F
 *                |  |
 *                b  f  
 *                |  |
 *                b  f  
 *
 *     so in 3D this means that the single intersection edge is split in 4!
 *
 *     Next consider a fault-normal crossing:
 *  
 *                bf  
 *                 |
 *                bf  
 *                 |
 *         N-N-N-N-*-N-N-N-N-
 *                 |
 *                bf  
 *                 |
 *                bf  
 * 
 *     Here we also assume that the fault opens, and the the normal points are
 *     also doubled!
 * 
 *  
 *                b  f  
 *                |  |
 *                b  f  
 *                |  |
 *        N-N-N-N-*  *-N-N-N-N-
 *                |  |
 *                b  f  
 *                |  |
 *                b  f  
 * 
 *     An even funnier situation occurs when a fault ends on a normal surface
 *  
 *                bf  
 *                 |
 *                bf  
 *                 |
 *         N-N-N-N-*-N-N-N-N-
 *    
 *     If the normal surface is at the outer boundary then the fault is
 *     opened up,
 *  
 *                b  f  
 *                |  |
 *                b  f  
 *                |  |
 *        N-N-N-N-*  *-N-N-N-    <- outer boundary
 *    
 *     otherwise it is not opened:
 *
 *                b    f  
 *                |    |
 *                b    f  
 *                 \ /  
 *        N-N-N-N-N-*-N-N-N-    <- not outer boundary
 *
 * Clearly these examples do not exhaust all possible situations. Now we go over * to the to the assemble of bodies from the simple surfaces (normal and shifted
 * front/back).
 *
 * Let is start with the simple case, and explain the algorithm:
 *
 *    *-A-A-A-A-A-A-A-*
 *    |               |
 *    C1             C3
 *    |               |
 *    C1             C3
 *    |               |
 *    *-D-D-D-D-D-D-D-*
 *    |               |
 *    C2             C4
 *    |               |
 *    C2             C4
 *    |               |
 *    *-B-B-B-B-B-B-B-*
 *
 * In the algorithm we only consider edges (joints) between triangles of
 * different simple surfaces:
 *
 *    *-A           A-*
 *    |               |
 *    C1             C3
 *
 *    C1             C3
 *    |               |
 *    *-D           D-*
 *    |               |
 *    C2             C4
 *
 *    C2             C4
 *    |               |
 *    *-B           B-*
 *
 * By analyzing these 'joints' in AnalyzeSingleJoint() we deduce which surfaces
 * must bound the same body. 
 * Two surfaces bound the same body if they have a minimum angle, ie. the edges
 * A-* and B-* have * smallest angle with C-* :
 *
 *                    A
 *                    |
 *                 C- *-D
 *                    |
 *                    B
 *
 * For this example we have  A<->C   A<->D   B<->C   B<->D ,
 * but not C<->D or A<->B . 
 * Of course we should here consider the direction of the surfaces: 'upper' C is
 * connected to 'left' A
 *
 * By computing the connectivity of the graph we obtain the bodies. This
 * algorithm has very low complexity because only the 'joints' between surfaces
 * are considered. Hoever it has an unexpected side effect:  for the simple
 * horizon example above, three bodies are generated. Not only the 2 expected
 * ones, but also the the 'outer' body
 *    A <-> C3 <-> C4 <-> B <-> C2 <-> C1
 * This outer body is explicitly removed in SurfaceSetMarkOuterBoundary().
 * Also the hierarchy of the bodies is determined. For example, in the case
 *
 *    *-A-A-A-A-A-A-A-*
 *    |               |
 *    C   *-a-a-*     D
 *    |   |     |     |
 *    C   c     d     D
 *    |   |     |     |
 *    C   c     d     D
 *    |   |     |     |
 *    C   *-b-b-*     D
 *    |               |
 *    *-B-B-B-B-B-B-B-*
 *
 * 2 bodies are generated that are meshed seperately:
 *  - the inner body a <-> b <-> c <-> d 
 *  - the containing body A <-> B <-> C <-> D  with a cavity a <-> b <-> c <-> d
 *
 *
 * We are now ready to mesh the bodies one-by-one in MeshSingleBody(),
 * but there are still some exceptional situations for a single body:
 *
 *
 * - Fault ends on horizon, intersection fault points have not been doubled
 *   (see above), handled in MeshTamperedBody
 *
 *     *-N-N-N-*-N-N-N-N-N-* 
 *     |      / \          |
 *     N     F   B         N
 *     |     |   |         |
 *     N     F   B         N
 *     |      \ /          |
 *     N       *           N  
 *     |                   |
 *     *-N-N-N-N-N-N-N-N-N-*
 *
 * - horizon doesn not divide body, but is enclosed by faults, so no isolated
 *   edges... 
 *   This is an error, the horizon should be a fault
 *
 *     N-N-N-N-N-N-N-N-N-N-N 
 *     |                   |
 *     N                   N  
 *     |                   |
 *     *-H-H-*-BF-BF       N  
 *     |                   |
 *     N                   N  
 *     |                   |
 *     N-N-N-N-N-N-N-N-N-N-N
 *
 *
 * - Self intersecting outer boundary (in 3D this is less strange),
 *   handled in MeshTamperedBody
 *
 *     *-N-N-N-*-N-N-N-N-N-* 
 *     |      / \          |
 *     N     N   N         N
 *     |    /      \       |
 *     |   *-N-N-N-N-*     |
 *     N                   N  
 *     |                   |
 *     *-N-N-N-N-N-N-N-N-N-*
 *
 * After meshing in FemGvMeshBody(), the fault points are unshifted and added to * the full tetrahedron set in MeshAddTetrahedra(). Finally the mesh points and
 * tetrahedra can be retrieved (MeshBodyTetrahedronAt(), MeshBodyCount(), etc.).
 */
extern void *MeshCreate( double pointTolerance )
{
   Mesh_t *mesh = DIMALLOC( sizeof(*mesh) );

   UTIL_ZERO( mesh );

   mesh->modelData = MeshModelDataCreate( pointTolerance );
   mesh->readBuffer = MeshReadBufferCreate( );
   mesh->derivedData = MeshDerivedDataCreate( );
   mesh->errorData = MeshErrorDataCreate( );
   return mesh;
}

static MeshOutput_t *MeshOutputDelete( MeshOutput_t *meshOutput )
{
  if ( meshOutput ) {
  Iterator_t   iter;
  BodyMesh_t *bm = SetFirst( meshOutput->bodyMesh, &iter );
  while ( bm ) {
      SurfaceSetClear( bm->sideSurfaces );
      SurfaceSetDelete( bm->sideSurfaces );
      bm = SetNext( meshOutput->bodyMesh, &iter );
  }
  meshOutput->bodyMesh = SetDelete( meshOutput->bodyMesh );
  meshOutput->node2Point = RBTreeDelete( meshOutput->node2Point );
  meshOutput->tetSet = TetrahedronSetDelete( meshOutput->tetSet );
  DIFREE( meshOutput );
  }
  return NULL;
}

static MeshOutput_t *MeshOutputCreate( void )
{
  MeshOutput_t *meshOutput = DIMALLOC( sizeof(*meshOutput) );
  UTIL_ZERO( meshOutput );
  meshOutput->node2Point = RBTreeCreate( 2*sizeof(int), CompareInt );
  meshOutput->bodyMesh = SetCreate( 0, sizeof(BodyMesh_t) );
  meshOutput->tetSet = TetrahedronSetCreate( );
  return meshOutput;
}

static MeshReadBuffer_t *MeshReadBufferDelete( MeshReadBuffer_t *readBuffer ) 
{
  if ( readBuffer ) {
  SurfaceDelete( readBuffer->frontSurface );
  SurfaceDelete( readBuffer->backSurface  );
  BoundBoxTreeDelete( readBuffer->pointTree );
  DIFREE( readBuffer->pointCor );
  DIFREE( readBuffer->pointId );
  SetDelete( readBuffer->workId );
  }
  DIFREE( readBuffer );
  return NULL;
}

static MeshModelData_t *MeshModelDataDelete( MeshModelData_t *modelData ) 
{
  if ( modelData ) {
  SurfaceSetDelete( modelData->surfaceSet );
  RBTreeDelete( modelData->surfIdTypeTree );
  PointSetDelete( modelData->pointSet );
  }
  DIFREE( modelData );
  return NULL;
}

static MeshDerivedData_t *
MeshDerivedDataDelete( MeshDerivedData_t *derivedData ) 
{
  if ( derivedData ) {
  SurfaceSetDelete( derivedData->surfaceSet );
  BodySetDelete( derivedData->bodySet );
  PointSetDelete(derivedData->pointSet );
  }
  DIFREE( derivedData );
  return NULL;
}

static MeshErrorData_t *MeshErrorDataDelete( MeshErrorData_t  *errorData ) 
{
  if ( errorData ) {
  /* Free message text strings */
  const int  nMessage = SetSize( errorData->messages );
  int        n;
  for ( n = 0; n < nMessage; n++ ) {
      Message_t *msg = SetElmGet( errorData->messages, n );
      DIFREE( msg->text );
  }
  SetDelete( errorData->messages );
  }
  DIFREE( errorData );
  return NULL;
}

extern void *MeshDelete( void *meshv )
{
   Mesh_t* mesh = meshv;
   if ( mesh ) {
      MeshModelDataDelete( mesh->modelData );
      MeshReadBufferDelete( mesh->readBuffer );
      MeshDerivedDataDelete( mesh->derivedData );
      MeshErrorDataDelete( mesh->errorData );
      MeshOutputDelete( mesh->meshOutput );
      DIFREE( mesh );
   }
   return NULL;
}

static void MeshPreparePointNumbering( Mesh_t *mesh )
{
   MeshReadBuffer_t  *readBuffer = mesh->readBuffer;

   if ( !MESH_ERROR( mesh ) && readBuffer->pointId == NULL ) {
      BoundBoxTree_t  *pointTree = readBuffer->pointTree;
      if ( !pointTree || !BoundBoxTreeSize( pointTree ) ) {
         MeshMessageOpen( mesh, MeshStatusNoPoints );
         MeshMessageAdd( mesh, "No points specified" );
         MeshMessageClose( mesh );
      }

      if ( !MESH_ERROR( mesh ) ) {
         TetGraph_t  *graph = TetGraphCreate();
         Set_t    *overlap = NULL;
         const int nPoints = BoundBoxTreeSize( pointTree );
         int       n, numUniquePoint = 0;
         bool_t   *pointDone = NULL;
         for ( n = 0; n < nPoints; n++ ) {
      BoundBox_t bbMem, *bb = BoundBoxTreeGet(  pointTree, n, &bbMem );
      int        p;
      overlap = BoundBoxTreeFind( overlap, pointTree, bb, BoundBoxOverlap );
      for ( p = 0; p < SetSize( overlap ); p++ ) {
               const int *overlapId = SetElmGet( overlap, p );
               TetGraphAdd( graph, n, *overlapId );
      }
         }
         readBuffer->pointId = TetGraphGetConnectivity( graph, &numUniquePoint );
         readBuffer->pointCor = DIMALLOC( 3*numUniquePoint*sizeof(*readBuffer->pointCor) );

         pointDone = DIMALLOC( numUniquePoint*sizeof(*pointDone) );
         UTIL_SET( pointDone, FALSE, numUniquePoint );
         for ( n = 0; n < nPoints; n++ ) {
      const int uniqueId = readBuffer->pointId[ n ];
      if ( !pointDone[ uniqueId ] ) {
               double *cor = readBuffer->pointCor+3*uniqueId;
               BoundBox_t bb;
               BoundBoxTreeGet( pointTree, n, &bb );
               BoundBoxCenter( &bb, cor );
               pointDone[ uniqueId ] = TRUE;
      }
         }
         TetGraphDelete( graph );
         SetDelete( overlap );
         DIFREE( pointDone );
      }
   }
}


extern void MeshSurfaceOpen( void       *meshv,
                             int         surfaceID,
                             const char *userName,
                             bool_t      bFault )
{
  Mesh_t            *mesh = meshv;
  MeshModelData_t   *modelData = mesh->modelData;
  MeshReadBuffer_t  *readBuffer = mesh->readBuffer;
  bool_t             normalExists =
       SurfIdTypeExists( modelData->surfIdTypeTree, surfaceID, surfNormal );
  bool_t             frontExists =
       SurfIdTypeExists( modelData->surfIdTypeTree, surfaceID, surfFrontFault );
  bool_t             backExists  =
       SurfIdTypeExists( modelData->surfIdTypeTree, surfaceID, surfBackFault );
  assert( readBuffer->frontSurface == NULL &&
          readBuffer->backSurface  == NULL );

  if ( !bFault ) {
  if ( normalExists || frontExists || backExists ) {
      char buf[ BUFSIZ ];
      MeshMessageOpen( mesh, MeshStatusSurfaceIdExists );
      sprintf( buf, "Failure to create normal surface USERNAME=%s;"
          " ID=%d exists already ", userName, surfaceID );
    MeshMessageAdd( mesh, buf );
    MeshMessageClose( mesh );
  }
  } else {
  if ( normalExists || frontExists || backExists ) {
      char buf[ BUFSIZ ];
      MeshMessageOpen( mesh, MeshStatusSurfaceIdExists );
      sprintf( buf, "Failure to create front of fault USERNAME=%s;"
          " ID=%d exists already ", userName, surfaceID );
      MeshMessageAdd( mesh, buf );
      MeshMessageClose( mesh );
  }
  }
  if ( MESH_ERROR( mesh ) ) return;
   
  MeshPreparePointNumbering( mesh );
  if ( MESH_ERROR( mesh ) ) return;

  if ( bFault ) {
  SurfIdTypeAdd( modelData->surfIdTypeTree,
                   userName,
                   surfaceID,
                   surfFrontFault );
  SurfIdTypeAdd( modelData->surfIdTypeTree,
                   userName,
                   surfaceID,
                   surfBackFault );
  readBuffer->frontSurface = SurfaceCreate( surfaceID,
                                              0,
                                              surfFrontFault,
                                              frontUnknownDir,
                                              userName,
                                              surfOuterUnknown );
  readBuffer->backSurface  = SurfaceCreate( surfaceID,
                                              0,
                                              surfBackFault,
                                              frontUnknownDir,
                                              userName,
                                              surfOuterUnknown );
  } else {
  SurfIdTypeAdd( modelData->surfIdTypeTree, userName, surfaceID, surfNormal );
  readBuffer->frontSurface = SurfaceCreate( surfaceID,
                                              0,
                                              surfNormal,
                                              frontUnknownDir,
                                              userName,
                                              surfOuterUnknown );
  }
}

static void MeshCloseEachSurface( void *meshv, Surface_t *surface )
{
   Mesh_t*     mesh = meshv;
   MeshModelData_t   *modelData = mesh->modelData;
   if ( SurfaceSize( surface ) ) {
      SurfaceSet_t *surfSet = SurfaceSetCreate();
      SurfaceSet_t *simpleSurfSet = NULL;

      /* Edge may appear in at most 2 triangles */
      if ( !MESH_ERROR( mesh ) ) {
         Edge_t   *edge = SurfaceTriangleConnectivity( surface );
         Iterator_t iter;
         EdgeKey_t *key = EdgeKeyFirst( edge, &iter );
         while ( key && !MESH_ERROR( mesh ) ) {
      if( EdgeListSize( edge, key, NULL ) > 2 ) {
               int   point1, point2;
               EdgeKeyPoints( key, &point1, &point2 );
               MeshMessageOpen( mesh, MeshStatusEdgeOverConnected );
               MeshMessageAdd( mesh, "Edge appears in at least 3 triangles, coordinates:\n"  );
               PointMeshMessageAdd( mesh, modelData->pointSet, point1 );
               PointMeshMessageAdd( mesh, modelData->pointSet, point2 );
               MeshMessageClose( mesh );
      }
      key = EdgeKeyNext( edge, &iter );
         }
      }

      if ( !MESH_ERROR( mesh ) ) {
         const bool_t   synchronizeFrontBack = FALSE;
         SurfaceSetAdd( surfSet, SurfaceCopy( surface) );
         simpleSurfSet = SurfaceSetSplitIntoSimpleSurface( mesh, surfSet, synchronizeFrontBack );
      }

      /* Fault surfaces must be simply connected, contain a least 2 triangles */
      if ( !MESH_ERROR( mesh ) && SurfaceIsFault(surface) ) {
         if ( SurfaceSize( surface ) <= 1 ) {
      MeshMessageOpen( mesh, MeshStatusFault2Triangle );
      MeshMessageAdd( mesh, "Fault surface must contain at least 2 triangles:\n" );
      SurfaceMeshMessageAdd( mesh, surface );
      MeshMessageClose( mesh );
         }
         if ( SurfaceSetSize(simpleSurfSet) > 1 ) {
      MeshMessageOpen( mesh, MeshStatusDisconnectedFault );
      MeshMessageAdd( mesh, "Fault surface consists of more than 1 part. Give different labels to different parts of the fault:\n" );
      SurfaceMeshMessageAdd( mesh, surface );
      MeshMessageClose( mesh );
         }
      }

      if ( !MESH_ERROR( mesh ) ) {
         Iterator_t    iter;
         Surface_t    *simpleSurf = SurfaceSetFirst( simpleSurfSet, &iter );
         while ( simpleSurf && !MESH_ERROR( mesh ) ) {
      Surface_t *newSurf = SurfaceCopy( simpleSurf );
      if ( !MESH_ERROR( mesh ) ) {
               SurfaceOrientate( newSurf );
      }
 //           if ( !MESH_ERROR( mesh ) && readBuffer->flipFault ) {
   //            SurfaceFlip( newSurf );
//            }
      SurfaceSetAdd( modelData->surfaceSet, newSurf );
      simpleSurf = SurfaceSetNext( simpleSurfSet, &iter );
         }
      }
      SurfaceSetDelete( surfSet );
      SurfaceSetDelete( simpleSurfSet );
   }
}

extern void MeshSurfaceClose( void *meshv )
{
   Mesh_t            *mesh = meshv;
   MeshReadBuffer_t  *readBuffer = mesh->readBuffer;
   assert( readBuffer->frontSurface != NULL );
   MeshCloseEachSurface( meshv, readBuffer->frontSurface );
   readBuffer->frontSurface = SurfaceDelete( readBuffer->frontSurface );
   if( readBuffer->backSurface ) {
     MeshCloseEachSurface( meshv, readBuffer->backSurface );
     readBuffer->backSurface = SurfaceDelete( readBuffer->backSurface );
   }

   readBuffer->flipFault = FALSE;
}


extern void MeshPointAdd( void *meshV, const double p0, const double p1, 
                          const double p2 )
{
   Mesh_t     *mesh = meshV;
   BoundBox_t  bb;
   MeshReadBuffer_t   *readBuffer = mesh->readBuffer;
   const int   id = BoundBoxTreeSize( readBuffer->pointTree );
   double     cor[3], tolerance = MeshTolerance( mesh );
   BoundBoxInit( &bb );
   cor[0] = p0 - tolerance;
   cor[1] = p1 - tolerance;
   cor[2] = p2 - tolerance;
   BoundBoxAdd( &bb, cor );
   cor[0] = p0 + tolerance;
   cor[1] = p1 + tolerance;
   cor[2] = p2 + tolerance;
   BoundBoxAdd( &bb, cor );
   BoundBoxTreeAdd( readBuffer->pointTree, id, &bb );
}

static int MeshGetPointId(  Mesh_t *mesh, const double *cor ) 
{
   BoundBox_t          bb;
   const double       *uniqueCor = NULL;
   const int          *id = NULL;
   MeshReadBuffer_t   *readBuffer = mesh->readBuffer;
   MeshModelData_t   *modelData = mesh->modelData;
   BoundBoxAdd( BoundBoxInit( &bb ), cor );
   readBuffer->workId = BoundBoxTreeFind( readBuffer->workId, readBuffer->pointTree, &bb, BoundBoxOverlap );
   id = SetElmGet( readBuffer->workId, 0 );
   uniqueCor = readBuffer->pointCor + 3*readBuffer->pointId[ *id ];
   return PointSetAdd( modelData->pointSet, uniqueCor[0], uniqueCor[1], uniqueCor[2] );
}

static Triangle_t* MeshTriangleCreate( void *meshv, Surface_t* surface, int *points, int interfaceIdx )
{
   Mesh_t*     mesh = meshv;
   MeshModelData_t   *modelData = mesh->modelData;
   Triangle_t *triangle = NULL;
   double      normal[3];

   triangle = TriangleCreate( points, interfaceIdx );
   if ( TriangleUnitNormal( triangle, modelData->pointSet, normal ) ) {
      MeshMessageOpen( mesh, MeshStatusDegenerateTriangle );
      MeshMessageAdd( mesh, "Degenerate triangle\n" );
      TriangleMeshMessageAdd( mesh, modelData->pointSet, triangle );
      MeshMessageAdd( mesh, "in surface\n" );
      SurfaceMeshMessageAdd( mesh, surface );
      MeshMessageClose( mesh );
      triangle = TriangleDelete( triangle );
   } else {
      SurfaceAdd( surface, triangle );
   }

   return triangle;
}

extern void MeshSurfaceAdd( void *meshv, const double *p1, const double *p2, 
              const double *p3 )
{
   Mesh_t*     mesh = meshv;
   int         corners[4];
   MeshReadBuffer_t   *readBuffer = mesh->readBuffer;

   corners[0] = MeshGetPointId( mesh, p1 );
   corners[1] = MeshGetPointId( mesh, p2 );
   corners[2] = MeshGetPointId( mesh, p3 );
   corners[3] = POINT_INVALID_ID;
   assert( readBuffer->frontSurface != NULL );

   if( readBuffer->backSurface ) {
     int interfaceIdx = mesh->modelData->maxInterfaceIdx++;
     MeshTriangleCreate( meshv, readBuffer->backSurface, corners, interfaceIdx );
     MeshTriangleCreate( meshv, readBuffer->frontSurface, corners, interfaceIdx );
   } else {
     MeshTriangleCreate( meshv, readBuffer->frontSurface, corners, -1 );
   }
}

static void OrientFault( 
                  Mesh_t                     *mesh, 
                  PointSet_t                 *pointSet, 
                  Surface_t                  *front, 
                  Surface_t                  *back )
{
   bool_t            ready = FALSE;
   Edge_t           *edge = EdgeCreate( );
   const EdgeKey_t  *edgeKey = NULL;
   Iterator_t        keyIter;


   assert( SurfaceType( front ) == surfFrontFault );
   assert( SurfaceType( back ) == surfBackFault );
   assert( SurfaceID( front ) == SurfaceID( back ) );
   
   EdgeAdd( edge, front );
   EdgeAdd( edge, back );

   /* Check if common parts in edge of front and back */
   edgeKey = EdgeKeyFirst( edge, &keyIter );
   while ( edgeKey && !ready ) {
      if ( EdgeListSize( edge, edgeKey, NULL ) == 2 ) {
         EdgeList_t  *edge0 = EdgeListFirst( edge, edgeKey );
         EdgeList_t  *edge1 = EdgeListNext( edge, edge0 );
         Surface_t   *surf0 = EdgeListSurface( edge0 );
         Surface_t   *surf1 = EdgeListSurface( edge1 );
         if ( SurfaceType(surf0) != SurfaceType(surf1) ) {
      int         triangleId0 = EdgeListTriangleId( edge0 );
      int         triangleId1 = EdgeListTriangleId( edge1 );
      Triangle_t *triangle0 = SurfaceGet( surf0, triangleId0 );
      Triangle_t *triangle1 = SurfaceGet( surf1, triangleId1 );
      int         edge0_p1, edge0_p2, edge1_p1, edge1_p2;
      TriangleSide( triangle0, edge0->edge, &edge0_p1, &edge0_p2 );
      TriangleSide( triangle1, edge1->edge, &edge1_p1, &edge1_p2 );
      if ( edge0_p1 == edge1_p1 ) {
               assert( edge0_p2 == edge1_p2 );
               ready = TRUE;
      } else if ( edge0_p1 == edge1_p2 ) {
               assert(  edge0_p2 == edge1_p1 );
               SurfaceFlip( back );
               ready = TRUE;
               break;
      } else {
               assert( FALSE );
      }
         }
      }
      edgeKey = EdgeKeyNext( edge, &keyIter );
   }

   /* Plan B */
   if ( !ready ) {
      double avNormalFront[3];
      double avNormalBack[3];
      double inprod = 0;
      SurfaceNormal( front, pointSet, avNormalFront );
      SurfaceNormal( back,  pointSet, avNormalBack );
      inprod = VecInprod( avNormalFront, avNormalBack, 3 );
      if ( inprod > 0 ) {
         /* Orientation same */
         ready = TRUE;
      } else if ( inprod < 0 ) {
         SurfaceFlip( back );
         ready = TRUE;
      } 
   }

   if ( !ready ) {
      MeshMessageOpen( mesh, MeshStatusFaultOrientation );
      MeshMessageAdd( mesh, "Failed to determine orientation of fault\n" );
      SurfaceMeshMessageAdd( mesh, front );
      MeshMessageClose( mesh );
   }

   EdgeDelete( edge );
}


static void  MeshOrientFaults( 
                  Mesh_t                     *mesh,
                  PointSet_t                 *pointSet,
                  SurfaceSet_t               *surfaceSet )
{
  int               n;
   Set_t           *allIdSet = SurfaceSetAllID( surfaceSet );
   const int        numID = SetSize( allIdSet );
   const int       *allID = SetGet( allIdSet );
   
   for ( n = 0; n < numID && !MESH_ERROR( mesh ); n++ ) {
      SurfaceSet_t *surfSet =  SurfaceSetSameID( surfaceSet, allID[n] );
      Iterator_t    iter;
      Surface_t    *surf0 = SurfaceSetFirst( surfSet, &iter ); 
      if ( surf0 ) {
         if ( SurfaceIsFault( surf0 ) ) {
      Surface_t  *surf1 = SurfaceSetNext( surfSet, &iter ); 
      assert( SurfaceSetSize( surfSet ) == 2 ); /* Single front and back */
      if ( SurfaceType( surf0 ) == surfFrontFault ) {
               OrientFault( mesh, pointSet, surf0, surf1 );
      } else {
               OrientFault( mesh, pointSet, surf1, surf0 );
      }
         }
      }
      SurfaceSetDelete( SurfaceSetClear( surfSet ) );
   }
   SetDelete( allIdSet );
}

static bool_t UnexpectedIntersection(
                  const Triangle_t           *triangle1,
                  const Triangle_t           *triangle2,
                  PointSet_t                 *pointSet )
{
  bool_t isWrong = FALSE;
  if ( triangle1 != triangle2 ) {
  if ( !TriangleCommonPoint( triangle1, triangle2, NULL ) ) {
      if ( TriangleIntersect( triangle1, triangle2, pointSet ) ) {
    isWrong = TRUE;
      }
  } 
  }
  return isWrong;
}

/* 
 * Check if any 2 triangles have an unexpected intersection.
 * Problem: faults may introduce overlap beteen 'normal' triangles, therefore we now only test
 * the faults. This usualy causes the problems in Geomec 
 */
static void MeshCheckSurfaceIntersection( 
                  Mesh_t                     *mesh, 
                  const bool_t               *surfTypeActive,
                  SurfaceSet_t               *surfaceSet, 
                  PointSet_t                 *pointSet,
                  ShiftPointSet_t            *spSet )
{
   Iterator_t      surfSetIter;
   Surface_t      *surf = SurfaceSetFirst( surfaceSet, &surfSetIter );
   BoundBoxTree_t *bbTree = BoundBoxTreeCreate();
   const int       nTriangle = SurfaceSetTriangleSize( surfaceSet );
   Surface_t     **id2Surface = DIMALLOC( nTriangle * sizeof(*id2Surface) );
   Triangle_t    **id2Triangle = DIMALLOC( nTriangle * sizeof(*id2Triangle) );
   Set_t          *intersect = SetCreate( 100, sizeof( int ) );
   int             id = 0;
   RBTree_t        *surfPairMessaged = RBTreeCreate( 2*sizeof(int),  CompareInt2 );

   UNUSED(spSet);
   
  /* Build tree of triangle bounding boxes */
  while ( surf ) {
  Iterator_t    iter;
  Triangle_t   *triangle = SurfaceFirst( surf, NULL, &iter );
  while ( surfTypeActive[SurfaceType( surf)-surfTypeFirst] && triangle ) {
      BoundBox_t   bb;
      TriangleBoundBox( triangle, pointSet, BoundBoxInit( &bb ) );
      BoundBoxTreeAdd( bbTree, id, &bb );
      id2Surface[id] = surf;
      id2Triangle[id] = triangle;
      id += 1;
      triangle = SurfaceNext( surf, NULL, &iter );
  }
  surf = SurfaceSetNext( surfaceSet, &surfSetIter );
  }
  assert( id <= nTriangle );

  /* Loop over all triangles in all faults and check for intersection */ 
  surf = SurfaceSetFirst( surfaceSet, &surfSetIter );
  while ( surf ) {
  Iterator_t    iter;
  Triangle_t   *triangle = SurfaceFirst( surf, NULL, &iter );
  while ( surfTypeActive[SurfaceType( surf)-surfTypeFirst] && triangle ) {
      BoundBox_t   bb;
      int          n;
      TriangleBoundBox( triangle, pointSet, BoundBoxInit( &bb ) );
      BoundBoxTreeFind( intersect, bbTree, &bb, BoundBoxOverlap );
      for ( n = 0; n < SetSize( intersect ); n++ ) {
    const int        *idFound = SetElmGet( intersect, n );
    const Triangle_t *triangleFound = id2Triangle[*idFound];
    const Surface_t  *surfFound = id2Surface[*idFound];
    if ( UnexpectedIntersection( triangle, triangleFound, pointSet ) ) {
          /* Surface intersection ?? */
          int   surfIdPair[2];
          const int  id1 = SurfaceID( surf );
          const int  id2 = SurfaceID( surfFound );
          surfIdPair[0] = MIN( id1, id2 );
          surfIdPair[1] = MAX( id1, id2 );

          if ( !RBTreeFindGet( surfPairMessaged, surfIdPair ) ) {
#ifdef MRH_DEVELOP
  MeshEditTwoTriangles( "intersectingTriangles.dat", triangle, triangleFound, pointSet );
#endif

      MeshMessageOpen( mesh, MeshStatusTriangle2Surfaces );
      MeshMessageAdd( mesh, "Two triangles intersect in the surfaces\n" );
      SurfaceMeshMessageAdd( mesh, surf );
      MeshMessageAdd( mesh, "and\n" );
      SurfaceMeshMessageAdd( mesh, surfFound );
      MeshMessageAdd( mesh, "Triangle coordinates\n" );
      TriangleMeshMessageAdd( mesh, pointSet, triangle );
      MeshMessageAdd( mesh, "and\n" );
      TriangleMeshMessageAdd( mesh, pointSet, triangleFound );
      MeshMessageAdd( mesh, "\n\n" );
      MeshMessageClose( mesh );

      /* Don't message this surface pair again */
      RBTreeSearch( surfPairMessaged, surfIdPair ) ;
          }
    }
      }
      triangle = SurfaceNext( surf, NULL, &iter );
  }
  surf = SurfaceSetNext( surfaceSet, &surfSetIter );
  }

  SetDelete( intersect );
  DIFREE( id2Triangle );
  DIFREE( id2Surface );
  BoundBoxTreeDelete( bbTree );
  RBTreeDelete( surfPairMessaged );
}

static void MeshFrontBackCheck( Mesh_t *mesh )
{
   void               *sitTree = mesh->modelData->surfIdTypeTree;
   Iterator_t          iter;
   const SurfIdType_t *sit = RBTreeFirst( sitTree, &iter );
   while ( sit && !MESH_ERROR( mesh ) ) {
      switch ( sit->type ) {
         case surfNormal:
      break;
         case surfFrontFault:
      if ( !SurfIdTypeExists(  sitTree, sit->ID, surfBackFault ) ) {
               char   buf[ BUFSIZ ];
               MeshMessageOpen( mesh, MeshStatusNoFrontOrBack );
               sprintf( buf, "Back of fault %s (ID=%d) not specified", sit->name, sit->ID );
               MeshMessageAdd( mesh, buf );
               MeshMessageClose( mesh );
      }
      break;
         case surfBackFault:
      if ( !SurfIdTypeExists(  sitTree, sit->ID, surfFrontFault ) ) {
               char     buf[ BUFSIZ ];
               MeshMessageOpen( mesh, MeshStatusNoFrontOrBack );
               sprintf( buf, "Front of fault %s (ID=%d) not specified", sit->name, sit->ID );
               MeshMessageAdd( mesh, buf );
               MeshMessageClose( mesh );
      }
      break;
         default:
      assert( FALSE );
      break;
      }
      sit = RBTreeNext( sitTree, &iter );
  }
}

static void MeshIsolatedEdges( Mesh_t       *mesh,
                               SurfaceSet_t *surfaceSet,
                               PointSet_t   *pointSet )
{
  Edge_t       *edge = SurfaceSetEdge( surfaceSet );
  Iterator_t    keyIter;
  EdgeKey_t    *key = EdgeKeyFirst( edge, &keyIter );
  while ( key && !MESH_ERROR( mesh ) ) {
  if ( EdgeListSize( edge, key, NULL ) == 1 ) {
      int              p1, p2;
      EdgeList_t      *list = EdgeListFirst( edge, key );
      const Surface_t *surf = EdgeListSurface( list );
      MeshMessageOpen( mesh, MeshStatusIsolatedEdge );
      EdgeKeyPoints( key, &p1, &p2 );
      MeshMessageAdd( mesh, "Isolated edge found in surface\n" );
      SurfaceMeshMessageAdd( mesh, surf );
      MeshMessageAdd( mesh, "Coordinates:\n" );
      PointMeshMessageAdd( mesh, pointSet, p1 );
      PointMeshMessageAdd( mesh, pointSet, p2 );
      MeshMessageClose( mesh );
  }
  key = EdgeKeyNext( edge, &keyIter );
  }
}

static void MeshCheckTriangleAngles( Mesh_t       *mesh,
                                     PointSet_t   *pointSet,
                                     SurfaceSet_t *surfaceSet )
{
   double            smallestAngle = M_PI;
   const Surface_t  *inSurf = NULL;
   const Triangle_t *inTriangle = NULL;
   Iterator_t        surfaceSetIter;
   Surface_t        *surf = SurfaceSetFirst( surfaceSet, &surfaceSetIter );

   while ( surf ) {
      Iterator_t        surfaceIter;
      const Triangle_t *triangle = SurfaceFirst( surf, NULL, &surfaceIter );
      while ( triangle ) {
         const double angle = TriangleSmallestAngle( triangle, pointSet );
         if ( angle < smallestAngle ) {
      smallestAngle = angle;
      inSurf = surf;
      inTriangle = triangle;
         }
         triangle = SurfaceNext( surf, NULL, &surfaceIter );
      }
      surf = SurfaceSetNext( surfaceSet, &surfaceSetIter );
   }
   
   /* Convert angle from radians to degrees */
   smallestAngle = smallestAngle*180/M_PI;

   if ( smallestAngle < MeshAngleGetWarningCrit( mesh ) ) {
      char            buf[ BUFSIZ ];
      MeshMessageOpen( mesh, MeshStatusSmallAngle );
      sprintf( buf, "Smallest angle in any surface triangle: %e [DEG]. Surface:\n", smallestAngle );
      MeshMessageAdd( mesh, buf );
      SurfaceMeshMessageAdd( mesh, inSurf );
      MeshMessageAdd( mesh, "Triangle:\n" );
      TriangleMeshMessageAdd( mesh, pointSet, inTriangle );
      MeshMessageClose( mesh );
   }
}

/* Check smallest angle between 2 triangles with common edge */
static void MeshCheckTriangleAngleBetween( 
                  Mesh_t                     *mesh,
                  PointSet_t                 *pointSet,
                  SurfaceSet_t               *surfaceSet )
{
  Edge_t           *edge = SurfaceSetEdge( surfaceSet );
  Iterator_t        keyIter;
  EdgeKey_t        *key = EdgeKeyFirst( edge, &keyIter );
  const Surface_t  *surf1 = NULL, *surf2 = NULL;
  const Triangle_t *triangle1 = NULL, *triangle2 = NULL;
  double            minAngle = M_PI;
  Set_t            *edgeListSet = SetCreate( 0, sizeof(EdgeList_t) );

  while ( key ) {
  /* Get angle between all different triangles */
  int       t1, t2;
  const int edgeSize = EdgeListSize( edge, key, NULL );
  EdgeList_t *edges = SetNew( SetClear(edgeListSet), edgeSize );
  EdgeListGet( edge, key, edges, NULL );

  for ( t1 = 0; t1 < edgeSize; t1++ ) {
      const Surface_t *edgeSurface1 = EdgeListSurface( edges+t1 );
      const int        triangleId1 = EdgeListTriangleId( edges+t1 );
      for ( t2 = t1+1; t2 < edgeSize; t2++ ) {
    const Surface_t *edgeSurface2 = EdgeListSurface( edges+t2 );
    if ( !SurfaceFrontBackOfSameFault( edgeSurface1, edgeSurface2 ) ) {
          const int        triangleId2 = EdgeListTriangleId( edges+t2 );
          const double angle = TriangleAngleBetween( 
                                   SurfaceGet( edgeSurface1, triangleId1 ),
                                   SurfaceGet( edgeSurface2, triangleId2 ),
                                   pointSet );
          assert( angle >= 0 );
          if ( angle < minAngle ) {
      minAngle = angle;
      surf1 = edgeSurface1;
      surf2 = edgeSurface2;
      triangle1 = SurfaceGet( edgeSurface1, triangleId1 );
      triangle2 = SurfaceGet( edgeSurface2, triangleId2 );
          }
    }
      }
  }
  key = EdgeKeyNext( edge, &keyIter );
  }

  /* Convert angle from radians to degrees */
  minAngle = minAngle*180/M_PI;

  if ( minAngle < MeshAngleGetWarningCrit( mesh ) ) {
  char   buf[ BUFSIZ ];
  MeshMessageOpen( mesh, MeshStatusJointDegeneracy );
  sprintf( buf, "Smallest angle between two triangles: %e [DEG].\n", minAngle );
  MeshMessageAdd( mesh, buf );
  MeshMessageAdd( mesh, "Triangle 1\n" );
  TriangleMeshMessageAdd( mesh, pointSet, triangle1 );
  MeshMessageAdd( mesh, "in surface\n" );
  SurfaceMeshMessageAdd( mesh, surf1 );
  MeshMessageAdd( mesh, "and triangle 2\n" );
  TriangleMeshMessageAdd( mesh, pointSet, triangle2 );
  MeshMessageAdd( mesh, "in surface\n" );
  SurfaceMeshMessageAdd( mesh, surf2 );
  MeshMessageClose( mesh );
  }

  SetDelete( edgeListSet );
}

static void InvalidateIsolatedFaultEdgeXPoints( Mesh_t       *mesh,
                        SurfaceSet_t *surfaceSet,
                        XPointSet_t  *xpointSet )
{
  Edge_t       *edge = SurfaceSetEdge( surfaceSet );
  Iterator_t    keyIter;
  EdgeKey_t    *key = EdgeKeyFirst( edge, &keyIter );
   
  while ( key && !MESH_ERROR( mesh ) ) {
  /* Analyze fault edge */
  if ( SurfaceSetClosedFault( mesh, surfaceSet, edge, key ) ) {
      EdgeList_t  *list = EdgeListFirst( edge, key );
      int          p0, p1;

      EdgeKeyPoints( key, &p0, &p1 );

      /* Loop over the fault surfaces */
      while( list ) {
    const Surface_t *fault = EdgeListSurface( list );
    if ( SurfaceIsFault( fault ) ) {
          /* Unshift the 2 points */
          XPointSetUnshift( xpointSet, p0, fault, EdgeListTriangleId( list ) );
          XPointSetUnshift( xpointSet, p1, fault, EdgeListTriangleId( list ) );
    }
    list = EdgeListNext( edge, list );
      }
  }
  key = EdgeKeyNext( edge, &keyIter );
  }
}

static void SetFaultPointShift( PointSet_t       *pointSet,
                const Surface_t  *surf,
                const Triangle_t *triangle,
                double           *shift )
{
  const bool_t  isFront = ( SurfaceType( surf ) == surfFrontFault );
  const bool_t  frontPos = ( SurfaceGetFrontDir( surf ) == frontPosDir );
  const double  signDir = ( isFront == frontPos ? 1 : -1 );
  const double  area = TriangleArea( triangle, pointSet );
  const double  len = sqrt( area );
  const double  shiftSize = signDir * len;
  double        unitNormal[ 3 ];
  assert( SurfaceIsFault( surf ) );
  TriangleUnitNormal( triangle, pointSet, unitNormal );
  VecOperScal( shift, EqlPos, unitNormal, OperTimes, shiftSize, 3 );
}

static void DuplicateFaultXPoints( PointSet_t   *pointSet, 
                                   SurfaceSet_t *surfaceSet,
                                   XPointSet_t  *xpointSet )
{
  Iterator_t    surfaceSetIter;
  Surface_t    *surf = SurfaceSetFirst( surfaceSet, &surfaceSetIter );
  /* Double all fault points */
  while ( surf ) {
  if ( SurfaceIsFault( surf ) ) {
      int         triangleId;
      Iterator_t  surfaceIter;
      Triangle_t *triangle = SurfaceFirst( surf, &triangleId, &surfaceIter );
      while ( triangle ) {
    const int    *pointId = TrianglePoints( triangle );
    double       shift[ 3 ];
    int             n;
    assert( TriangleSize( triangle ) == 3 );
    SetFaultPointShift( pointSet, surf, triangle, shift );
    for ( n = 0; n < 3; n++ ) {
          XPointSetAddFault( xpointSet, pointId[n], surf, triangleId, shift );
    }
    triangle = SurfaceNext( surf, &triangleId, &surfaceIter );
      }
  }
  surf = SurfaceSetNext( surfaceSet, &surfaceSetIter );
  }
}

static int ShiftNewPoint( Mesh_t       *mesh,
                          int           orgId,
                          const double *baseShift,
                          PointSet_t   *pointSet )
{
  int          newPointId = -1;
  double       org[3];
  bool_t       ready = FALSE;
  double       shiftScaleFactor = 1;
 
  if ( VecL2Norm( baseShift, 3 ) > DBL_MIN ) {
  PointSetFind( pointSet, orgId, org+0, org+1, org+2 );
  while ( !ready ) {
      const int    pointIdExpect = PointSetSize( pointSet );
      double       cor[3];
      UTIL_COPY( cor, org, 3 );
      VecOperScal( cor, EqlAdd, baseShift, OperTimes, shiftScaleFactor, 3 ); 
      newPointId = PointSetAdd( pointSet, cor[0], cor[1], cor[2] );
#if 0
fprintf( stderr, "ShiftNewPoint orgId=%d newPointId=%d\n", orgId, newPointId ); 
#endif
      ready = ( newPointId == pointIdExpect );
      shiftScaleFactor *= 1.5;
  }
  } else {
  MeshMessageOpen( mesh, MeshStatusFaultShift );
  MeshMessageAdd( mesh, "Unable to determine shift of fault point. Please contact support.\n\n" );
  MeshMessageClose( mesh );
  }
  return  newPointId;
}

static void AddExtraFaultPoints( Mesh_t          *mesh,
                                 PointSet_t      *pointSet,
                                 XPointSet_t     *xpointSet,
                                 ShiftPointSet_t *spSet )
{
  Iterator_t   iter;
  XPoint_t    *xp = XPointSetFirst( xpointSet, &iter );
  while ( xp && !MESH_ERROR( mesh ) ) {
  if ( xp->type == xpointFault ) {
      double baseShift[3];
      const int pointId = ShiftNewPoint( mesh, xp->orgId, 
                                         XPointGetShift( pointSet, xpointSet, xp, MeshShiftScaleFactor( mesh ), baseShift ), 
                                         pointSet );
      if ( !MESH_ERROR( mesh ) ) {
    xp->data.faultPoint.pointId = pointId;
    if ( !ShiftPointSetFind( spSet, pointId ) ) {
          ShiftPointSetAdd( spSet, pointId, xp->orgId );
    }
      }
  }
  xp = XPointSetNext( xpointSet, &iter );
  }
}

static void ChangeTriangles (
                  SurfaceSet_t               *surfaceSet,
                  XPointSet_t                *xpointSet )
{
  Iterator_t   surfaceSetIter;
  Surface_t   *surf = SurfaceSetFirst( surfaceSet, &surfaceSetIter );

  /* Used shifted fault points in triangles */
  while ( surf ) {
  int          triangleId;
  Iterator_t   surfaceIter;
  Triangle_t  *triangle = SurfaceFirst( surf, &triangleId, &surfaceIter );
  while ( triangle ) {
      const int *points = TrianglePoints( triangle );
      int        newPoints[ 3 ];
      bool_t     isChanged = FALSE;
      int        p;

      for ( p = 0; p < 3; p++ ) {
    const  XPoint_t    *xp = XPointSetFind( xpointSet, points[p], surf, triangleId ); 
    if ( xp ) {
          const int           pointId = XPointSetPointId( xpointSet, xp );
          if ( pointId != points[p] ) {
      if ( !isChanged ) {
              UTIL_COPY( newPoints, points, 3 );
      }
      newPoints[p] = pointId;
      isChanged = TRUE;
          }
    }
      }

      if ( isChanged ) TriangleChange( triangle, newPoints );
      triangle = SurfaceNext( surf, &triangleId, &surfaceIter );
  }
  surf = SurfaceSetNext( surfaceSet, &surfaceSetIter );
  }

  /* Delete connectivity */
  SurfaceSetPurge( surfaceSet );
}

typedef struct {
   Surface_t   *fault0[2];
   Surface_t   *fault1[2];
   int          triangle0[2];
   int          triangle1[2];
   int          point0;
   int          point1;
} FaultCorner_t;

static bool_t FaultCornerSet(
              RBTree_t                  *edges,
              EdgeKey_t                 *key,
              const EdgeList_t         **list,
              Set_t                     *edgeList )
{
  bool_t     canSplit = TRUE;
  int        side, i;
  FaultCorner_t  fc;
  UTIL_ZERO( &fc );

  EdgeKeyPoints( key, &fc.point0, &fc.point1 );
  for(side = 0; side < 2; side++) {
  fc.fault0[side] = EdgeListSurface( list[side] );
  fc.fault1[side] = EdgeListSurface( list[side + 2] );

  fc.triangle0[side] = EdgeListTriangleId( list[side] );
  fc.triangle1[side] = EdgeListTriangleId( list[side + 2] );
  }

  for(i = 0; i < 4; i++) {
  Surface_t*  surface          = EdgeListSurface( list[i] );
  int               triangleId = EdgeListTriangleId( list[i] );
  const Triangle_t *triangle   = SurfaceGet( surface, triangleId );

  // Could we split the triangles or are they already used?
  for( side = 0; side < 3; side++ ) {
      int p2[2], p0, p1;
      TriangleSide( triangle, side, &p0, &p1 );
      p2[0] = MIN( p0, p1 );
      p2[1] = MAX( p0, p1 );
      if ( RBTreeFindGet( edges, p2 ) ) {
    canSplit = FALSE;
      }
  }    
  }

  if ( canSplit ) {
  /* Mark the 4 edges of the 2 triangles as non-splittable */
#if 0
fprintf( stderr, "\n\n SPLIT %d %d \n\n", fc.point0, fc.point1 );
#endif
  for( i = 0; i < 4; i++ ) {
      const EdgeList_t* edge       = list[i];
      const Surface_t*  surface    = EdgeListSurface( edge );
      int               triangleId = EdgeListTriangleId( edge );
      const Triangle_t *triangle   = SurfaceGet( surface, triangleId );
      int  s;
      for ( s = 0; s < 3; s++ ) {
    int p2[2], p0, p1;
    TriangleSide( triangle, s, &p0, &p1 );
    p2[0] = MIN( p0, p1 );
    p2[1] = MAX( p0, p1 );
    RBTreeSearch( edges, p2 );
      }
  }
  /* Add to the list of triangle pairs that will be split */
  SetAdd( edgeList, &fc, 1 );
  }
  return canSplit;
}

static void AnalyzeFaultTriangle(
                  const EdgeKey_t            *key,
                  const EdgeList_t           *list,
                  int                        *other,
                  bool_t                     *orient )
{
  const Surface_t  *fault = EdgeListSurface( list );
  const Triangle_t *triangle = SurfaceGet( fault, EdgeListTriangleId( list ) );
  int               p0, p1;
  assert( SurfaceIsFault( fault ) );
  EdgeKeyPoints( key, &p0, &p1 );
  TriangleLocate2( triangle, p0, p1, other, orient );

  /* Flip if wong orientation */
  if ( SurfaceFaultDir( fault, TRUE ) == frontNegDir ) {
  *orient = !*orient;
  }
}

static void SplitTriangle(
                  Surface_t                  *surf,
                  int                         triangleId,
                  int                         interfaceIdx,
                  int                         point0,
                  int                         point1,
                  int                         pointX )
{
  Triangle_t  *triangle = SurfaceGet( surf, triangleId );
  const int   *points = TrianglePoints( triangle );
  Triangle_t  *triangleNew = NULL;
  int          trgp0[3], trgp1[3];
  int          n;

  UTIL_COPY( trgp0, points, 3 );
  UTIL_COPY( trgp1, points, 3 );
  
  for ( n = 0; n < 3; n++ ) {
  if ( trgp0[n] == point0 ) trgp0[n] = pointX;
  if ( trgp1[n] == point1 ) trgp1[n] = pointX;
  }

  TriangleChange( triangle, trgp0 );
  triangleNew = TriangleCreate( trgp1, interfaceIdx );
  SurfaceAdd( surf, triangleNew );
}

/*
static bool_t SameFault(const EdgeList_t *list)
{
  int i;
  int nFaultID =  SurfaceID( EdgeListSurface( &list[0] ) );
  for(i = 1; i < 4; i++) {
     if( nFaultID != SurfaceID( EdgeListSurface( &list[i] ) ) )
       return FALSE;
  }

  return TRUE;
}
*/

static bool_t SameFaultSide(
                  const EdgeKey_t            *key,
                  const EdgeList_t           *list1,
                  const EdgeList_t           *list2 )
{
  bool_t  sameSide = !SurfaceFrontBackOfSameFault( EdgeListSurface( list1 ),
                                                   EdgeListSurface( list2 ) );
  if ( sameSide ) {
  int     other1, other2;
  bool_t  orient1, orient2;
  AnalyzeFaultTriangle( key, list1, &other1, &orient1 );
  AnalyzeFaultTriangle( key, list2, &other2, &orient2 );

  /* Two different triangles, with different orientation */
  sameSide = ( ( other1 != other2 ) && ( orient1 != orient2 ));
  }
  return sameSide;
}

/*!
  Returns the back of a fault triangle given the interface idx
*/
const EdgeList_t* FindBackTriangle( Edge_t    *edge,
                  EdgeKey_t *key,
                  int        nInterfaceIdx )
{
  const EdgeList_t *list = EdgeListFirst( edge, key );
  while( list ) {
  Surface_t* surface = EdgeListSurface( list );
  if( SurfaceType( surface ) == surfBackFault ) {
      const Triangle_t* triangle = SurfaceGet( surface, EdgeListTriangleId( list ) );
      if( TriangleInterfaceIdx( triangle ) == nInterfaceIdx ) {
    return list;
      }
  }

  list = EdgeListNext( edge, list );
  }

  return list;
}

/*!
  Returns false when the routine is unable to sort the list
  Returns true when the following order is found:
  TRI1F1 TRI1F2 TRI2F3 TRI2F4
  where F1 - F3  F2 - F4 are on the same fault side
  
*/
static bool_t SortEdgeList(Edge_t            *edge,
                           EdgeKey_t         *key ,
                           const EdgeList_t **ret )
{
  int i;
  const EdgeList_t *list = EdgeListFirst( edge, key );
  /* Set the list to zero */
  for(i = 0; i < 4; i++) {
  ret[i] = 0;
  }

  /* Fill list */
  while( list ) {
  Surface_t* surface = EdgeListSurface( list );
  if( SurfaceType( surface ) == surfFrontFault ) {
      // Get the corresponding back edge
      const Triangle_t* triangle = SurfaceGet( surface, EdgeListTriangleId( list ) );
      const EdgeList_t* back = FindBackTriangle( edge, key, TriangleInterfaceIdx( triangle ));
      if ( !back ) return FALSE; /* Back not found */
      if( ret[0] ) {
    if( SameFaultSide( key, ret[0], list ) ) {
          ret[2] = list;
          ret[3] = back;
    } else {
          ret[2] = back;
          ret[3] = list;
    }
      } else {
          ret[0] = list;
          ret[1] = back;
      }
  }
  list = EdgeListNext( edge, list );
  }

  if( ret[0] && ret[1] && ret[2] && ret[3] &&
      SameFaultSide( key, ret[0], ret[2] ) &&  SameFaultSide( key, ret[1], ret[3] ) )
  return TRUE;

  return FALSE;
}

/* 
 * Exception: both points 'unshifted' and not on the edge 
 * of the fault surface: add extra midPoint.
 * Example (the o's are unshifted):
 *
 *        x   o    x
 *          x ?  x
 *        x   o    x
 */
static bool_t AddExtraMidPoints(
                  Mesh_t                     *mesh,
                  PointSet_t                 *pointSet,
                  SurfaceSet_t               *surfaceSet )
{
  Edge_t     *edge = SurfaceSetEdge( surfaceSet );
  RBTree_t   *edgePoints = RBTreeCreate( sizeof(int), CompareInt );
  Set_t      *edgeList = SetCreate( 0, sizeof( FaultCorner_t ) );
  int         el;
  bool_t      ready = TRUE;
  RBTree_t   *edges = RBTreeCreate( 2*sizeof(int), CompareInt2 );
  Iterator_t  iter;
  EdgeKey_t  *key = EdgeKeyFirst( edge, &iter );

  /* Get list of unshifted fault points */
  while ( key && !MESH_ERROR( mesh ) ) {
  if ( SurfaceSetClosedFault( mesh, surfaceSet, edge, key ) ) {
      int       p0, p1;
      EdgeKeyPoints( key, &p0, &p1 );
      RBTreeSearch( edgePoints, &p0 );
      RBTreeSearch( edgePoints, &p1 );
  }
  key = EdgeKeyNext( edge, &iter );
  }

  /* Get list of edges of for which an extra mid point is needed */
  key = EdgeKeyFirst( edge, &iter );
  while ( key && !MESH_ERROR( mesh ) ) {
  int       nFault;
  const int edgeSize = EdgeListSize( edge, key, &nFault );
  if ( edgeSize == nFault && edgeSize == 4 ) {
      if ( !EdgeIsolatedFault( edge, key ) ) {
    int           p0, p1;
    EdgeKeyPoints( key, &p0, &p1 );
    if ( RBTreeFindGet( edgePoints, &p0 ) && RBTreeFindGet( edgePoints, &p1 ) ) {
          // All edges should come from the same fault
          const EdgeList_t*   list[4];
          if( SortEdgeList( edge, key, list ) ) {
      /* Tricky: triangle can be split already; skip these triangles, and treat them in next sweep */
      if ( !FaultCornerSet( edges, key, list, edgeList ) ) ready = FALSE;
          }
    }
      }
  }
  key = EdgeKeyNext( edge, &iter );
  }
  
  for ( el = 0; el < SetSize( edgeList ) && !MESH_ERROR( mesh ); el++ ) {
  const FaultCorner_t *fc = SetElmGet( edgeList, el );
  int interfaceIdx0 = mesh->modelData->maxInterfaceIdx++;
  int interfaceIdx1 = mesh->modelData->maxInterfaceIdx++;
  int side;
  for( side = 0; side < 2; side++ ) {
      int midPointId;
  
      /* Mark this edge as split */
      RBTreeSearch( edges, &fc->triangle0[side] );
      RBTreeSearch( edges, &fc->triangle1[side] );

      /* Create mid-point, notice that the point is not shifted! */
      midPointId = PointSetCreateMidPoint( pointSet, fc->point0, fc->point1 );

#if 0
fprintf( stderr, "AddExtraMidPoints old0=%d old1=%d new=%d (%f %f %f)\n",
      fc->point0, fc->point1,midPointId, pointX[0], pointX[1], pointX[2] ); 
#endif
      /* Split triangles in 2 */
      SplitTriangle( fc->fault0[side], fc->triangle0[side], interfaceIdx0, fc->point0, fc->point1, midPointId );
      SplitTriangle( fc->fault1[side], fc->triangle1[side], interfaceIdx1, fc->point0, fc->point1, midPointId );
  }
  }

  if ( SetSize( edgeList ) ) {
  /* Connectivity is lost */
  SurfaceSetPurge( surfaceSet );
  }
  RBTreeDelete( edgePoints );
  RBTreeDelete( edges );
  SetDelete( edgeList );
  return ready;
}

/* Double the fault points... */
static void MeshExtraFaultPoints( Mesh_t          *mesh, 
                                  ShiftPointSet_t *spSet,
                                  PointSet_t      *pointSet, 
                                  SurfaceSet_t    *surfaceSet )
{
  XPointSet_t  *xpointSet = XPointSetCreate( );
  bool_t        ready;

  if ( !MESH_ERROR( mesh ) ) {
  /* Add extra mid-points on fault edges near corners */
  do {
      ready = AddExtraMidPoints( mesh, pointSet, surfaceSet );
  } while ( !ready && !MESH_ERROR( mesh ) );
  }

  /* Duplicate all the fault points */
  if ( !MESH_ERROR( mesh ) ) {
  DuplicateFaultXPoints( pointSet, surfaceSet, xpointSet );
  }

  /* Points along isolated edges of faults are not shifted */
  if ( !MESH_ERROR( mesh ) ) {
  InvalidateIsolatedFaultEdgeXPoints( mesh, surfaceSet, xpointSet );
  }

  /* Join fault points */
  if ( !MESH_ERROR( mesh ) ) {
  const bool_t skipOuter2Outer = FALSE;
  SurfaceSetLinkFaultPoints( mesh, pointSet, xpointSet, surfaceSet, skipOuter2Outer );
  }

  /* Add the extra fault points to the mesh */
  if ( !MESH_ERROR( mesh ) ) {
  AddExtraFaultPoints( mesh, pointSet, xpointSet, spSet );
  }

  /* Change triangles to use the extra points */
  if ( !MESH_ERROR( mesh ) ) {
  ChangeTriangles( surfaceSet, xpointSet );
  }

  XPointSetDelete( xpointSet );
}

typedef struct SharedTriangle_t  {
   const Triangle_t   *triangle;
   const Surface_t    *surface;
} SharedTriangle_t;


static int CompSharedTriangle( const void* data0, const void* data1 )
{
   const SharedTriangle_t *s0 = data0;
   const SharedTriangle_t *s1 = data1;
   return TriangleComp( s0->triangle, s1->triangle );
}


static void MeshSharedTriangles( Mesh_t *mesh, SurfaceSet_t *surfSet, PointSet_t *pointSet )
{
   Iterator_t  surfaceSetIter;
   Surface_t  *surf = SurfaceSetFirst( surfSet, &surfaceSetIter );
   RBTree_t   *tree = RBTreeCreate( sizeof(SharedTriangle_t), CompSharedTriangle );

   while ( surf && !MESH_ERROR( mesh ) ) {
      if ( SurfaceType( surf ) == surfNormal ) {
         Iterator_t        surfaceIter;
         const Triangle_t *triangle = SurfaceFirst( surf, NULL, &surfaceIter );
         while ( triangle && !MESH_ERROR( mesh ) ) {
      SharedTriangle_t st;
      st.triangle = triangle;
      st.surface  = NULL;
      if ( RBTreeFind( tree, &st ) == RBTREE_INVALID_ID ) {
               st.surface = surf;
               RBTreeSearch( tree, &st );
      } else {
               SharedTriangle_t *found = RBTreeSearchGet( tree, &st );
               if ( found->surface != surf ) {
                  /* Triangle in 2 normal surfaces: error */
                  MeshMessageOpen( mesh, MeshStatusTriangle2Surfaces );
                  MeshMessageAdd( mesh, "Identical triangle\n" );
                  TriangleMeshMessageAdd( mesh, pointSet, triangle );
                  MeshMessageAdd( mesh, "in surfaces\n" );
                  SurfaceMeshMessageAdd( mesh, found->surface );
                  SurfaceMeshMessageAdd( mesh, surf );
                  MeshMessageClose( mesh );
               } else {
                  /* Triangle twice in same surface, stupid but OK */
               }
      }
      triangle = SurfaceNext( surf, NULL, &surfaceIter );
         }
      }
      surf = SurfaceSetNext( surfSet, &surfaceSetIter );
   }
   RBTreeDelete( tree );
}

static int MeshOutputNode2Offset(
                  const MeshOutput_t         *meshOutput,
                  int                         nodeId )
{
  int n2p[2];
  n2p[0] = nodeId;
  return RBTreeFind( meshOutput->node2Point, n2p ); 
}

static int Node2PointAdd(
                  RBTree_t                   *node2Point,
                  ShiftPointSet_t            *spSet,
                  int                         nodeId )
{
  int   nodePoint[2], *np;
  nodePoint[0] = nodeId;
  nodePoint[1] = -1;
  np = RBTreeSearchGet( node2Point, nodePoint );
  if ( np[1] < 0 ) {
  np[1] = ShiftPointSetGetOrgId( spSet, nodeId );
  }
  return np[1];
}

static void MeshAddTetrahedra(
                  Mesh_t                     *mesh, 
                  Body_t                     *body,
                  SurfaceSet_t               *activeSurfaces,
                  ShiftPointSet_t            *spSet,
                  TetrahedronSet_t           *tetSet,
                  MeshOutput_t               *meshOutput )
{
  Iterator_t           iter;
  BodyMesh_t           bm;
  const Tetrahedron_t *tet = NULL;

  UNUSED(mesh);

  /* Update statistics for this body */
  UTIL_ZERO( &bm );
  bm.body = body;
  bm.tetrahedronFirst = TetrahedronSetSize( meshOutput->tetSet );
  bm.tetrahedronSize = TetrahedronSetSize( tetSet );

  /* Store all side surfaces of the body */
  bm.sideSurfaces = SurfaceSetCreate();
  SurfaceSetAddSurfaceSet( bm.sideSurfaces, activeSurfaces, FALSE );

  SetAdd( meshOutput->bodyMesh, &bm, 1 );

  /* Add tetrahedra to the full mesh */
  tet = TetrahedronSetFirst( tetSet, &iter );
  while ( tet ) {
  int   p0, p1, p2, p3;
  TetrahedronGet( tet, &p0, &p1, &p2, &p3 );
  /* Add these nodes to the node set */
  Node2PointAdd( meshOutput->node2Point, spSet, p0 );
  Node2PointAdd( meshOutput->node2Point, spSet, p1 );
  Node2PointAdd( meshOutput->node2Point, spSet, p2 );
  Node2PointAdd( meshOutput->node2Point, spSet, p3 );
  /* Add this tetrahedron to the full set of tetrahedra */
  TetrahedronSetAdd( meshOutput->tetSet, p0, p1, p2, p3 );
  tet = TetrahedronSetNext( tetSet, &iter );
  }
}

static SurfaceSet_t  *GetActiveSurfaces(
                  Body_t                     *body,
                  BodySet_t                  *cavities )
{
  
  SurfaceSet_t *actSurfaces = SurfaceSetCreate();
  Iterator_t    iter;
  Body_t       *cavity = BodySetFirst( cavities, &iter );

  SurfaceSetAddBody( actSurfaces, body );

  while ( cavity ) {
  SurfaceSetAddBody( actSurfaces, cavity );
  cavity = BodySetNext( cavities, &iter );
  }
  return actSurfaces;
}

static MeshTriangleSet_t *BuildMeshTriangleSet( 
                  TetrahedronSet_t           *tetSet )
{
  MeshTriangleSet_t    *mtSet = MeshTriangleSetCreate();
  Iterator_t            iter;
  const Tetrahedron_t  *tet = TetrahedronSetFirst( tetSet, &iter );
  while ( tet ) {
  int  p0, p1, p2, p3;
  TetrahedronGet( tet, &p0, &p1, &p2, &p3 );
  /* Add the 4 triangles to the set */
  MeshTriangleSetAdd( mtSet, p0, p1, p2, p3 );
  MeshTriangleSetAdd( mtSet, p1, p2, p3, p0 );
  MeshTriangleSetAdd( mtSet, p2, p3, p0, p1 );
  MeshTriangleSetAdd( mtSet, p3, p0, p1, p2 );
  tet = TetrahedronSetNext( tetSet, &iter );
  }
  return mtSet;
}

/* Connect triangles in the mesh to surfaces */
static bool_t MeshTriangleSurfaces(
                  MeshTriangleSet_t          *mtSet,
                  PointSet_t                 *pointSet,
                  SurfaceSet_t               *surfaceSet )
{
  bool_t     isCorrect = TRUE;
  Iterator_t surfaceSetIter;
  Surface_t *surface = SurfaceSetFirst( surfaceSet, &surfaceSetIter );
  UNUSED(pointSet);
  while ( surface && isCorrect ) {
  Iterator_t   surfaceIter;
  Triangle_t  *triangle = SurfaceFirst( surface, NULL, &surfaceIter );
  while ( triangle && isCorrect ) {
      const int *points = TrianglePoints( triangle );
      MeshTriangle_t *mtFound = MeshTriangleSetFind( mtSet, points[0], points[1], points[2] );
      if ( mtFound ) {
    if ( !MeshTriangleGetSurface( mtFound ) ) {
          MeshTriangleSetSurface( mtFound, surface, TriangleInterfaceIdx( triangle ) );
    }
      } else {
    isCorrect = FALSE;
      }
      triangle = SurfaceNext( surface, NULL, &surfaceIter );
  }
  surface = SurfaceSetNext( surfaceSet, &surfaceSetIter );
  }
  return isCorrect;
}

static bool_t MeshBodyIsCorrect(
                  TetrahedronSet_t           *tetSet,
                  PointSet_t                 *pointSet,
                  SurfaceSet_t               *surfaceSet )
{
  /* Any tetrahedra ? */
  bool_t  isCorrect = ( TetrahedronSetSize( tetSet ) > 0 );

  if ( isCorrect ) {
  /* Colllect triangle data */
  MeshTriangleSet_t    *mtSet = BuildMeshTriangleSet( tetSet );
  const MeshTriangle_t *mt = NULL;
  Iterator_t            iter;
  
  /* Connect surfaces to triangles */
  isCorrect = MeshTriangleSurfaces( mtSet, pointSet, surfaceSet );

  /* All triangles connected to at most 2 tetrahedra? */
  mt = MeshTriangleSetFirst( mtSet, &iter );
  while ( mt && isCorrect ) {
      const int size = MeshTriangleSize( mt );
      if ( size == 1 ) {
    /* Connected to surface? */
    isCorrect = MeshTriangleGetSurface( mt ) != NULL;
      } else if ( size == 2 ) {
#if 0
    /* Not Connected to surface? My fail in case of tampered body */
    isCorrect = MeshTriangleGetSurface( mt ) == NULL;
#endif
      } else {
    isCorrect = FALSE;
      }
      mt = MeshTriangleSetNext( mtSet, &iter );
  }
  MeshTriangleSetDelete( mtSet );
  }
  return isCorrect;
}

static void SplitTriangleList( Mesh_t                     *mesh,
                               Edge_t                     *edge,
                               EdgeKey_t                  *key,
                               int                         p0,
                               int                         p1,
                               int                         midPoint )
{
  const EdgeList_t *list = EdgeListFirst( edge, key );
  
  // Order fault parts and normal parts
  while ( list ) {
  Surface_t* surface = EdgeListSurface( list );
  if( SurfaceType( surface ) == surfNormal  ) {
      SplitTriangle( surface, EdgeListTriangleId( list ), -1, p0, p1, midPoint);
  } else {
      // Do only fronts and search for back
      if( SurfaceType( surface ) == surfFrontFault  ) {
    int newIdx        = mesh->modelData->maxInterfaceIdx++;
    int nInterfaceIdx =
      TriangleInterfaceIdx( SurfaceGet( EdgeListSurface( list ),
                                              EdgeListTriangleId( list ) ) );
    const EdgeList_t* back = FindBackTriangle( edge, key, nInterfaceIdx );
    if(back)
    {
          SplitTriangle( EdgeListSurface( list ),
                         EdgeListTriangleId( list ), newIdx, p0, p1, midPoint );
          SplitTriangle( EdgeListSurface( back ),
                         EdgeListTriangleId( back ), newIdx, p0, p1, midPoint );
    }
      }
  }
  list = EdgeListNext( edge, list );
  }
}


/* Split surfaces consisting of a single triangle */
static bool_t SplitSingleEdgeSeam( Mesh_t       *mesh,
                                   SurfaceSet_t *surfaceSet,
                                   PointSet_t   *pointSet )
{
  bool_t             ready = TRUE;
  const int          nPoints = PointSetSize( pointSet );
  int               *multiplicity = DIMALLOC( nPoints*sizeof(*multiplicity) );
  Edge_t            *edge = SurfaceSetEdge( surfaceSet );
  Iterator_t         iter;
  EdgeKey_t         *key = EdgeKeyFirst( edge, &iter );

  UTIL_SET( multiplicity, 0, nPoints );

  /* Set multiplicity of fault-normal crossings */
  while ( key ) {
  int        nFault;
  const int  nEdge = EdgeListSize( edge, key, &nFault );
  if ( nFault && nFault != nEdge ) {
      int p0, p1;
      EdgeKeyPoints( key, &p0, &p1 );
      multiplicity[p0] += 1;
      multiplicity[p1] += 1;
  }
  key = EdgeKeyNext( edge, &iter );
  }

  /* Isolated fault edges are special */
  key = EdgeKeyFirst( edge, &iter );
  while ( key ) {
  if ( EdgeIsolatedFault( edge, key ) ) {
      int p0, p1;
      EdgeKeyPoints( key, &p0, &p1 );
      multiplicity[p0] = 1;
      multiplicity[p1] = 1;
  }
  key = EdgeKeyNext( edge, &iter );
  }

  /* Split too small normal-fault crossings */
  key = EdgeKeyFirst( edge, &iter );
  while ( key ) {
  int        nFault;
  const int  nEdge = EdgeListSize( edge, key, &nFault );
  if ( nFault && nFault != nEdge ) {
      int p0, p1;
      EdgeKeyPoints( key, &p0, &p1 );
      if ( multiplicity[p0]  == 1 && multiplicity[p1] == 1 ) {
    const int midPoint = PointSetCreateMidPoint( pointSet, p0, p1 );
    SplitTriangleList( mesh, edge, key, p0, p1, midPoint );
    ready = FALSE;
    SurfaceSetPurge( surfaceSet );
    break;
      }
  }
      
  key = EdgeKeyNext( edge, &iter );
  }
  DIFREE( multiplicity );
  return ready;
}

static void ConstructBodies( Mesh_t          *mesh, 
                             ShiftPointSet_t *spSet,
                             PointSet_t      *pointSet, 
                             SurfaceSet_t    *surfaceSet, 
                             BodySet_t       *bodySet )
{
  /* Mark the outer surfaces of the model */
  if ( !MESH_ERROR( mesh ) ) {
  SurfaceSetMarkOuterBoundary( mesh, TRUE, pointSet, surfaceSet ); 
  }

  /* fault-normal seams consisting of a single edge */
  if ( !MESH_ERROR( mesh ) ) {
  bool_t   ready = FALSE;
  while ( !ready ) {
      ready = SplitSingleEdgeSeam( mesh, surfaceSet, pointSet );
  }
  }

  /* Add extra fault points */
  if ( !MESH_ERROR( mesh ) ) {
  MeshExtraFaultPoints( mesh, spSet, pointSet, surfaceSet );
  }

  /* Construct bodies */
  if ( !MESH_ERROR( mesh ) ) {
  SurfaceSetFillBodySet( mesh, surfaceSet, pointSet, bodySet );
  }
 
  /* Set relation between degenerate body and containing body */
  if ( !MESH_ERROR( mesh ) ) {
  BodySetHierarchy( mesh, bodySet, pointSet );
  }
}

static bool_t UnshiftPoints(
                  ShiftPointSet_t            *spSet,
                  int                         nPoints,
                  const int                  *oldPoints,
                  int                        *newPoints )
{
  bool_t     changed = FALSE;
  int        n;
  for ( n = 0; n < nPoints; n++ ) {
  newPoints[n] = ShiftPointSetGetOrgId( spSet, oldPoints[n] );
  /* This point is shifted? */
  if ( oldPoints[n] != newPoints[n] ) changed = TRUE;
  }
  return changed;
}


static bool_t UnshiftTriangle(
                  ShiftPointSet_t            *spSet,
                  Triangle_t                 *triangle )
{
  bool_t     changed = FALSE;
  int        newPoints[3];
  if ( UnshiftPoints( spSet, 3, TrianglePoints( triangle ), newPoints ) ) {
  changed = TRUE;
  TriangleChange( triangle, newPoints );
  }
  return changed;
}

/* Unshift the surface points to their 'original' position */
static void UnshiftSurfaces(
                  ShiftPointSet_t            *spSet,
                  SurfaceSet_t               *surfaceSet )
{
  bool_t     changed = FALSE;
  Iterator_t surfaceSetIter;
  Surface_t *surf = SurfaceSetFirst( surfaceSet, &surfaceSetIter );
  while ( surf ) {
  Iterator_t  surfaceIter;
  Triangle_t *triangle = SurfaceFirst( surf, NULL, &surfaceIter );
  while ( triangle ) {
      if ( UnshiftTriangle( spSet, triangle ) ) {
    changed = TRUE;
      }
      triangle = SurfaceNext( surf, NULL, &surfaceIter );
  }
  surf = SurfaceSetNext( surfaceSet, &surfaceSetIter );
  }

  /* Delete connectivity */
  if ( changed ) {
  SurfaceSetPurge( surfaceSet );
  }
}


/* Unshift the mesh points to their 'original' position, a new mesh is created! */
static TetrahedronSet_t *UnshiftTetrahedra(
                  ShiftPointSet_t            *spSet,
                  TetrahedronSet_t           *tetSet )
{
  TetrahedronSet_t    *result = TetrahedronSetCreate();
  Iterator_t           iter;
  const Tetrahedron_t *tet = TetrahedronSetFirst( tetSet, &iter );
  while ( tet ) {
  int     oldPoints[4], newPoints[4];
  TetrahedronGet( tet, oldPoints+0, oldPoints+1, oldPoints+2, oldPoints+3 );
  UnshiftPoints( spSet, 4, oldPoints, newPoints );
  TetrahedronSetAdd( result, newPoints[0],  newPoints[1],  newPoints[2],  newPoints[3] );
  tet = TetrahedronSetNext( tetSet, &iter );
  }
  return result;
}

static TetrahedronSet_t *UnshiftSurfacesAndMesh(
                  ShiftPointSet_t            *spSet,
                  SurfaceSet_t               *surfaceSet,
                  TetrahedronSet_t           *tetSet )
{
  UnshiftSurfaces( spSet, surfaceSet );
  return UnshiftTetrahedra( spSet, tetSet );
}

static void MeshBodyFailure(
                  Mesh_t                     *mesh,
                  SurfaceSet_t               *activeSurfaces )
{
  MeshMessageOpen( mesh, MeshStatusBodyMeshFailure );
  MeshMessageAdd( mesh, "Failed to mesh body bounded by:\n" );
  SurfaceSetMeshMessageAdd( mesh, activeSurfaces );
  MeshMessageClose( mesh );
}

static SurfaceSet_t *MarkTrueOuterBoundary(
                  Mesh_t                     *mesh,
                  ShiftPointSet_t            *spSet,
                  PointSet_t                 *pointSet,
                  SurfaceSet_t               *surfaceSet )
{
  SurfaceSet_t     *outerBoundSet = SurfaceSetCreate();
  const int         nSurface = SurfaceSetSize( surfaceSet );
  int               surfCount;

  /* Make a copy of the surfaceset set, and replace faults by normal surfaces */
  for ( surfCount = 0; surfCount < nSurface; surfCount++ ) {
  Surface_t          *surf = SurfaceSetGet( surfaceSet, surfCount );
  const SurfaceType_t surfType = surfNormal;
  const int           ID = SurfaceID(surf);
  const int           part = SurfacePartGet(surf);
  const SurfOuterBoundary_t outer = SurfaceType(surf) == surfNormal ? SurfaceOuterBoundaryGet(surf) : surfOuterNot;

  /* Make sure that front and back do not occur twice */
  if ( !SurfaceSetFind( outerBoundSet, ID, part, surfType ) ) {
      Surface_t        *newSurf = SurfaceCreate( ID, part, surfType, frontUnknownDir, SurfaceUserName(surf), outer );
      /* Copy surface */
      Iterator_t  surfaceIter;
      Triangle_t *triangle = SurfaceFirst( surf, NULL, &surfaceIter );
      while ( triangle ) {
    const int *oldPoints = TrianglePoints( triangle );
    int        newPoints[3];
    int        n;
    /* Unshift the points */
    for ( n = 0; n < 3; n++ ) {
          newPoints[n] = ShiftPointSetGetOrgId( spSet, oldPoints[n] );
    }
    SurfaceAdd( newSurf, TriangleCreate( newPoints, TriangleInterfaceIdx( triangle ) ) );
    triangle = SurfaceNext( surf, NULL, &surfaceIter );
      }
      SurfaceSetAdd( outerBoundSet, newSurf );
  }
  }

  if ( !MESH_ERROR( mesh ) ) {
  /* Mark true outer boundary of model */
  SurfaceSetMarkOuterBoundary( mesh, TRUE, pointSet, outerBoundSet ); 
  }
  return outerBoundSet;
}

static void SetPointShift(
                  PointSet_t                 *pointSet,
                  const Surface_t            *surf,
                  const Triangle_t           *triangle,
                  double                     *shift )
{
  /* Inward shift */
  const double  signDir = ( SurfaceOuterBoundaryGet( surf ) == surfOuterPosDir ? -1 : 1 );
  const double  area = TriangleArea( triangle, pointSet );
  const double  len = sqrt( area );
  double        unitNormal[ 3 ];

  assert( !SurfaceIsFault( surf ) && SurfaceAtOuterBoundary( surf )  );
  TriangleUnitNormal( triangle, pointSet, unitNormal );
  VecOperScal( shift, EqlPos, unitNormal, OperTimes, signDir * len, 3 );
}

static void ShiftIntersectingOuterBoundaryPoint(
                  Surface_t                  *fault,
                  PointSet_t                 *pointSet,
                  XPointSet_t                *xpointSet,
                  Edge_t                     *edge,
                  EdgeKey_t                  *key,
                  int                         point )
{
  EdgeList_t        *list = EdgeListFirst( edge, key );
  while ( list ) {
  const Surface_t *surf = EdgeListSurface( list );
  if ( SurfaceAtOuterBoundary( surf ) ) {
      /* Add this triangle to the fault */
      const Triangle_t *triangle = SurfaceGet( EdgeListSurface( list ), EdgeListTriangleId( list ) );
      const int         triangleId = SurfaceAdd( fault, TriangleCopy( triangle ) );
      double            shift[ 3 ];
      const  XPoint_t  *xp = NULL;

      SetPointShift( pointSet, surf, triangle, shift );

      /* Create a shifted fault point... */
      XPointSetAddFault( xpointSet, point, fault, triangleId, shift );
      /* ... and link the normal surface(=outer boundary) to it */
      xp = XPointSetFind( xpointSet, point, fault, triangleId );
      XPointSetLinkNormal( xpointSet, point, surf, EdgeListTriangleId( list ), xp );
  }
  list = EdgeListNext( edge, list );
  }
}

static void OpenSelfIntersectingOuterBoundary( 
                  Mesh_t                     *mesh,
                  ShiftPointSet_t            *spSet,
                  PointSet_t                 *pointSet,
                  SurfaceSet_t               *surfaceSet )
{
  Edge_t       *edge = SurfaceSetEdge( surfaceSet );
  Iterator_t    iter;
  EdgeKey_t    *key = EdgeKeyFirst( edge, &iter );
  int           nPoints = PointSetSize( pointSet );
  int          *shiftCount = DIMALLOC( nPoints * sizeof(*shiftCount ) );
  int           n;
  bool_t        ready = TRUE;

  /* Count the number of overconnected boundary keys in which this point lives */
  UTIL_SET( shiftCount, 0, nPoints );

  /* Count the number of outer boundaries in this edge */
  while ( key && !MESH_ERROR( mesh ) ) {
  EdgeList_t        *list = EdgeListFirst( edge, key );
  int                outerCount = 0;
  while ( list ) {
      const Surface_t *surf = EdgeListSurface( list );
      if ( SurfaceAtOuterBoundary( surf ) ) {
    outerCount += 1;
      }
      list = EdgeListNext( edge, list );
  }
  if ( outerCount > 2 ) {
      int p0, p1;

      list = EdgeListFirst( edge, key );
      while ( list ) {
    /* No faults here!! */
    assert( SurfaceType( EdgeListSurface(list) ) == surfNormal );
    list = EdgeListNext( edge, list );
      }
      /* Store mark points */
      EdgeKeyPoints( key, &p0, &p1 );
      shiftCount[p0] += 1;
      shiftCount[p1] += 1;
      ready = FALSE;
  }
  key = EdgeKeyNext( edge, &iter );
  }

  if ( !ready ) {
  XPointSet_t  *xpointSet = XPointSetCreate( );
  /* Create a dummy fault surface */
  const int    faultID =  SurfaceSetFreeID( surfaceSet );
  const SurfOuterBoundary_t isOuter = surfOuterUnknown;
  Surface_t     *fault = SurfaceCreate( faultID, 1, surfFrontFault, frontPosDir, "DummyFault", isOuter );

  /* Keep it simple */
  for ( n = 0; n < nPoints; n++ ) {
  assert( shiftCount[n] <= 2 );
  }

  /* Shift the self intersection points */
  key = EdgeKeyFirst( edge, &iter );
  while ( key && !MESH_ERROR( mesh ) ) {
      int p0, p1;
      EdgeKeyPoints( key, &p0, &p1 );
      if ( shiftCount[p0] > 1 || shiftCount[p1] > 1 ) {
    int        nFault;
    EdgeListSize( edge, key, &nFault );
    assert( !nFault );
    if ( shiftCount[p0] > 1 ) {
          ShiftIntersectingOuterBoundaryPoint( fault, pointSet, xpointSet, edge, key, p0 );
    }
    if ( shiftCount[p1] > 1 ) {
          ShiftIntersectingOuterBoundaryPoint( fault, pointSet, xpointSet, edge, key, p1 );
    }
      }
      key = EdgeKeyNext( edge, &iter );
  }

  /* Link the outer boundary points to the dummy fault */
  if ( !MESH_ERROR( mesh ) ) {
      SurfaceSet_t  *surfOuterOnly = SurfaceSetCreate();
      Surface_t     *surf = SurfaceSetFirst( surfaceSet, &iter );
      const bool_t   skipOuter2Outer = TRUE;
      while ( surf ) {
    if ( SurfaceAtOuterBoundary( surf ) ) {
          SurfaceSetAdd( surfOuterOnly, surf );
    }
    surf = SurfaceSetNext( surfaceSet, &iter );
      }

      SurfaceSetLinkFaultPoints( mesh, pointSet, xpointSet, surfOuterOnly, skipOuter2Outer );
      SurfaceSetDelete( SurfaceSetClear( surfOuterOnly ) );
  }

  /* Add the extra fault points to the mesh */
  if ( !MESH_ERROR( mesh ) ) {
      AddExtraFaultPoints( mesh, pointSet, xpointSet, spSet );
  }

  /* Change triangles to use the extra points */
  if ( !MESH_ERROR( mesh ) ) {
      ChangeTriangles( surfaceSet, xpointSet );
  }
  XPointSetDelete( xpointSet );
  SurfaceDelete( fault );
  }

  DIFREE( shiftCount );
}


/* Try to mesh the body, with the outer fault surfaces replaced by normal surfaces */
static TetrahedronSet_t *MeshTamperedBody(
                  Mesh_t                     *mesh,
                  ShiftPointSet_t            *oldSpSet,
                  PointSet_t                 *pointSet,
                  SurfaceSet_t               *surfaceSet,
                  Body_t                     *body,
                  BodySet_t                  *cavities,
          const FemGvPath_t          *FemGvPath )
{
  TetrahedronSet_t *result = NULL;
  ShiftPointSet_t  *spSet = ShiftPointSetCreate( );
  const int         nSurface = SurfaceSetSize( surfaceSet );
  BodySet_t        *bodySet = BodySetCreate();
  RBTree_t         *innerBodies = NULL;
  SurfaceSet_t     *newSurfaceSet = SurfaceSetCreate();

  /* Mark true outer boundary of model */
  SurfaceSet_t     *outerBoundSet = MarkTrueOuterBoundary( mesh, oldSpSet, pointSet, surfaceSet );

  if ( !MESH_ERROR( mesh ) ) {
  int               surfCount;
  /* Replace outer boundary faults by normal surfaces */
  for ( surfCount = 0; surfCount < nSurface; surfCount++ ) {
      Surface_t          *surf = SurfaceSetGet( surfaceSet, surfCount );
      const int           ID = SurfaceID(surf);
      const int           part = SurfacePartGet(surf);
      const Surface_t    *outerSurf = SurfaceSetFind( outerBoundSet, ID, part, surfNormal );
      Surface_t          *newSurf = NULL;

      assert( outerSurf != NULL );
    
      if ( SurfaceAtOuterBoundary( outerSurf ) ) {
    /* Make sure that front and back do not occur twice, and turn fault into normal */
    if ( !SurfaceSetFind( newSurfaceSet, ID, part, surfNormal ) ) {
          newSurf = SurfaceCreate( ID, part, surfNormal, frontUnknownDir, SurfaceUserName(surf), SurfaceOuterBoundaryGet(outerSurf) );
    }
      } else {
    /* This is not a copy: the outer boundary info is taken from outerSurf !! */
    newSurf = SurfaceCreate( ID, part, SurfaceType( surf ), SurfaceGetFrontDir(surf), SurfaceUserName(surf), SurfaceOuterBoundaryGet(outerSurf));
      }
      if ( newSurf ) {
    /* Copy surface */
    SurfaceAddSurface( newSurf, surf );
    SurfaceSetAdd( newSurfaceSet, newSurf );
      }
  }
  }

  outerBoundSet = SurfaceSetDelete( outerBoundSet );

  /* Mark cavities: the faults should open here */
  if ( !MESH_ERROR( mesh ) ) {
  Iterator_t        bodyIter;
  Body_t  *cavity = BodySetFirst( cavities, &bodyIter );
  while ( cavity ) {
      Iterator_t          surfIter;
      BodySurface_t      *bs = BodyFirst( cavity, &surfIter );
      while ( bs ) {
    Surface_t       *surf = BodySurfaceGet( bs );
    SurfaceSetOpenFault( surf, TRUE );
    bs = BodyNext( cavity, &surfIter );
      }
      cavity = BodySetNext( cavities, &bodyIter );
  }
  }

  OpenSelfIntersectingOuterBoundary( mesh, spSet, pointSet, newSurfaceSet );

  /* Agin shift the fault points: fault touching normal will be opened now */
  if ( !MESH_ERROR( mesh ) ) {
  BodyInner_t      *bi = NULL;
  Iterator_t        iter;
  ConstructBodies( mesh, spSet, pointSet, newSurfaceSet, bodySet );
  innerBodies = BodyInnerCreate( mesh, bodySet );
  /* Pick up single outer body */
  body = NULL;
  bi = RBTreeFirst( innerBodies, &iter );
  while ( bi && !MESH_ERROR( mesh ) ) {
      if ( !BodyOuterGet( bi->body ) ) {
    /* Single body ???? */
    if ( body ) {
          MeshBodyFailure( mesh, newSurfaceSet );

    }
    body = bi->body;
    cavities = bi->cavities;
      }
      bi = RBTreeNext( innerBodies, &iter );
  }
  }
  /* Generate a mesh */
  if ( !MESH_ERROR( mesh ) ) {
  result = FemGvMeshBody( pointSet, newSurfaceSet, body, cavities, FemGvPath );
  } else {
  /* Return empty set to indicate error */
  result = TetrahedronSetCreate();
  }

  /* Restore outer boundary and cavity information */
  if ( !MESH_ERROR( mesh ) ) {
  int               surfCount;
  for ( surfCount = 0; surfCount < nSurface; surfCount++ ) {
      SurfaceSetOpenFault( SurfaceSetGet( surfaceSet, surfCount ), FALSE );
  }
  }

  /* Unshift the surfaces and mesh */
  if ( !MESH_ERROR( mesh ) && TetrahedronSetSize( result ) ) {
  TetrahedronSet_t  *tetSet = UnshiftSurfacesAndMesh( spSet, newSurfaceSet, result );
  /* Overwrite result */
  TetrahedronSetDelete( result );
  result = tetSet;
  }

  spSet = ShiftPointSetDelete( spSet );
  BodySetDelete( bodySet );
  innerBodies = BodyInnerDelete( innerBodies );
  SurfaceSetDelete( newSurfaceSet );
  return result;
}

static void MeshCheckSingleSideHorizonAddBody( 
                  Mesh_t                     *mesh,
                  Body_t                     *bodyAll,
                  Body_t                     *body )
{
  Iterator_t     iter;
  BodySurface_t *bs = BodyFirst( body, &iter );
  while ( bs && !MESH_ERROR( mesh ) ) {
  Surface_t  *surf = BodySurfaceGet( bs );
  if ( SurfaceType( surf ) == surfNormal ) {
      const bool_t      posDir = BodySurfacePosDir( bs );
      if ( !BodyContains( bodyAll, surf, !posDir ) ) {
    BodyAdd( bodyAll, surf, posDir );
      } else {
      MeshMessageOpen( mesh, MeshStatusNotHorizon );
      MeshMessageAdd( mesh, "Horizon\n" );
      SurfaceMeshMessageAdd( mesh,  surf );
      MeshMessageAdd( mesh, "does not separate 2 formations.  Turn it into a fault.\n" );
      MeshMessageClose( mesh );
      }
  }
  bs = BodyNext( body, &iter );
  }
}

static void MeshCheckSingleSideHorizon(
                  Mesh_t                     *mesh,
                  Body_t                     *body,
                  BodySet_t                  *cavities )
{
  Body_t *bodyAll = BodyCreate( mesh );
  UNUSED(cavities);
  MeshCheckSingleSideHorizonAddBody( mesh, bodyAll, body );
  BodyDelete( bodyAll );
}


static TetrahedronSet_t *MeshSingleBody( 
                  Mesh_t                     *mesh,
                  ShiftPointSet_t            *spSet,
                  PointSet_t                 *pointSet,
                  SurfaceSet_t               *surfaceSet,
                  Body_t                     *body,
                  BodySet_t                  *cavities,
                  const FemGvPath_t          *FemGvPath)
{
  TetrahedronSet_t *result = NULL;
  Edge_t       *edge = SurfaceSetEdge( surfaceSet );
  Iterator_t    iter;
  EdgeKey_t    *key = EdgeKeyFirst( edge, &iter );
  bool_t        isOK = TRUE;

#ifdef MRH_DEVELOP
  MeshEditWrite( "MESurfaces.dat", "MEWireFrame.dat", mesh, pointSet, surfaceSet );
#endif

  while ( key && !MESH_ERROR( mesh ) ) {
  int        nFault;
  const int  nEdge = EdgeListSize( edge, key, &nFault );
  switch ( nEdge ) {
      case 2:
    /* Normal case */
    break;
      case 1:
    MeshMessageOpen( mesh, MeshStatusIsolatedEdgeAfterShift );
    MeshMessageAdd( mesh, "Isolated edge found after fault shift. Please contact support.\n\n" );
    MeshMessageClose( mesh );
    break;
      default:
    {
          /* Repairable? */
          isOK = FALSE;
    }
    break;
  }
  key = EdgeKeyNext( edge, &iter );
  }

  if ( !MESH_ERROR( mesh ) ) {
  if ( isOK ) {
      /* Normal case */
      result = FemGvMeshBody( pointSet, surfaceSet, body, cavities, FemGvPath );
  } else {
      /* Try to fix the body by cutting and glueing */
      result = MeshTamperedBody( mesh, spSet, pointSet, surfaceSet, body, cavities, FemGvPath );
  }
  }
  return result;
}

static void MeshAddSingleBody(
                  Mesh_t                     *mesh,
                  ShiftPointSet_t            *spSet,
                  Body_t                     *body,
                  BodySet_t                  *cavities,
                  PointSet_t                 *pointSet,
                  MeshOutput_t               *meshOutput,
                  const FemGvPath_t          *FemGvPath)
{
  if ( !MESH_ERROR( mesh ) ) {
  MeshCheckSingleSideHorizon( mesh, body, cavities );
  }

  if ( !MESH_ERROR( mesh ) ) {
  SurfaceSet_t     *activeSurfaces = GetActiveSurfaces( body, cavities );
  TetrahedronSet_t *tetSet = MeshSingleBody( mesh, spSet, pointSet, activeSurfaces, body, cavities, FemGvPath );

  if ( !MESH_ERROR( mesh ) ) {
      if ( MeshBodyIsCorrect( tetSet, pointSet, activeSurfaces ) ) {
    MeshAddTetrahedra( mesh, body, activeSurfaces, spSet, tetSet, meshOutput );
      } else if ( SurfaceSetSize( activeSurfaces ) ) {
    MeshBodyFailure( mesh, activeSurfaces );
      }
  }
  SurfaceSetClear( activeSurfaces );
  SurfaceSetDelete( activeSurfaces );
  TetrahedronSetDelete( tetSet );
  }
}

static void MeshIsCorrect( 
                  Mesh_t                     *mesh,
                  TetrahedronSet_t           *tetSet,
                  PointSet_t                 *pointSet,
                  SurfaceSet_t               *surfaceSet )
{
  MeshTriangleSet_t    *mtSet = BuildMeshTriangleSet( tetSet );

  /* Connect surfaces to triangles */
  int                   nError = 0;
  const MeshTriangle_t *mt = NULL;


  if ( !nError ) {
  Iterator_t    iter;
  MeshTriangleSurfaces( mtSet, pointSet, surfaceSet );
  mt = MeshTriangleSetFirst( mtSet, &iter );
  while ( mt && !nError ) {
      const int size = MeshTriangleSize( mt );
      if ( size == 1 ) {
    /* Connected to special surface? */
    const Surface_t *surf = MeshTriangleGetSurface( mt );
    if ( surf ) {
          if ( !SurfaceAtOuterBoundary( surf ) && SurfaceType( surf ) == surfNormal ) {
      nError += 1;
          }
    } else {
          nError += 1;
    }
      } else if ( size != 2 ){
    nError += 1;
      }
      mt = MeshTriangleSetNext( mtSet, &iter );
  }
  }

  /* All surface triangles in mesh? */
  if ( !nError ) {
  Iterator_t surfaceSetIter;
  Surface_t *surface = SurfaceSetFirst( surfaceSet, &surfaceSetIter );
  while ( surface && !nError ) {
      Iterator_t   surfaceIter;
      Triangle_t  *triangle = SurfaceFirst( surface, NULL, &surfaceIter );
      while ( triangle && !nError ) {
    const int *points = TrianglePoints( triangle );
    if ( !MeshTriangleSetFind( mtSet, points[0], points[1], points[2] ) ) {
          nError += 1;
    }
    triangle = SurfaceNext( surface, NULL, &surfaceIter );
      }
      surface = SurfaceSetNext( surfaceSet, &surfaceSetIter );
  }
  }

  if ( nError ) {
  MeshMessageOpen( mesh, MeshStatusMeshFailure );
  MeshMessageAdd( mesh, "Inconsistent mesh generated" );
  MeshMessageClose( mesh );
  }

  MeshTriangleSetDelete( mtSet );
}

/* 
 * Check if the edges of 2 faults intersect in a single point. 
 * This cannot happen (?) with normal surfaces unless isolated edges occur. 
 */

typedef struct PointFault_t  {
   int              pointId;
   const Surface_t *fault;
   bool_t           isOK;
} PointFault_t;

static int PointFaultComp( const void* data0, const void* data1 )
{
  const PointFault_t *pf0 = data0;
  const PointFault_t *pf1 = data1;
  int   result = UTIL_COMPARE( pf0->pointId, pf1->pointId );
  if ( !result ) {
  if ( pf0->fault && pf1->fault ) {
      result = SurfaceComp( pf0->fault, pf1->fault );
  }
  }
  return result;
}

static void MeshSinglePointSeamIntersection( Mesh_t        *mesh,
                                             SurfaceType_t  faultType,
                                             SurfaceSet_t  *surfaceSet,
                                             PointSet_t    *pointSet )
{
  int                nPoints = PointSetSize( pointSet );
  bool_t            *pointOK = DIMALLOC( nPoints * sizeof(*pointOK ) );
  RBTree_t          *tree = RBTreeCreate( sizeof(PointFault_t), PointFaultComp );
  Edge_t            *edge = SurfaceSetEdge( surfaceSet );
  Iterator_t         iter;
  EdgeKey_t         *key = EdgeKeyFirst( edge, &iter );
  const PointFault_t*pf = NULL;

  while ( key ) {
  if ( EdgeIsolatedFault( edge, key ) ) {
      int                points[2], n;
      const Surface_t   *fault = NULL;
      EdgeList_t        *edge0 = EdgeListFirst( edge, key );
      EdgeList_t        *edge1 = EdgeListNext( edge, edge0 );
      const Surface_t   *fault0 = EdgeListSurface( edge0 );
      const Surface_t   *fault1 = EdgeListSurface( edge1 );
      if ( SurfaceType(fault0) == faultType ) {
    fault = fault0;
      } else if ( SurfaceType(fault1) == faultType ) {
    fault = fault1;
      }
      assert( fault && EdgeListSize( edge, key, NULL ) == 2 );
      EdgeKeyPoints( key, points+0, points+1 );
      for ( n = 0; n < 2; n++ ) {
    PointFault_t   fp;
    UTIL_ZERO( &fp );
    fp.pointId = points[n];
    fp.fault = fault;
    RBTreeSearch( tree, &fp );
      }
  }
  key = EdgeKeyNext( edge, &iter );
  }

  /* All points not on isolated fault edge are OK, we are not so sure about
     points in more than 1 fault */
  UTIL_SET( pointOK, TRUE, nPoints );
  pf = RBTreeFirst( tree, &iter );
  while ( pf ) {
  PointFault_t  pfMatch;
  UTIL_ZERO( &pfMatch );
  pfMatch.pointId = pf->pointId;
  pointOK[pf->pointId] = ( RBTreeCountMatch( tree, &pfMatch ) == 1 );
  pf = RBTreeNext( tree, &iter );
  }

  /* Now check if there are edges in which the suspected points occur */
  key = EdgeKeyFirst( edge, &iter );
  while ( key ) {
  int p0, p1;
  EdgeKeyPoints( key, &p0, &p1 );
  if ( !pointOK[p0] || !pointOK[p1] ) {
      EdgeList_t        *list = EdgeListFirst( edge, key );
      SurfaceSet_t      *faultSet = SurfaceSetCreate();
      while ( list ) {
    Surface_t *surf = EdgeListSurface( list );
    if ( SurfaceType( surf ) == faultType ) {
          if ( !SurfaceSetTest( faultSet, surf ) ) {
      SurfaceSetAdd( faultSet, surf );
          }
    }
    list = EdgeListNext( edge, list );
      }
      /* At least 2 faults cross here, assume this is a real crossing */
      if ( SurfaceSetSize( faultSet ) > 1 ) {
    int       n;
    for ( n = 0; n < SurfaceSetSize( faultSet ); n++ ) {
          Surface_t *fault = SurfaceSetGet( faultSet, n );
          int        np;
          for ( np = 0; np < 2; np++ ) {
      PointFault_t  pfMatch, *found = NULL;
      pfMatch.pointId = ( np == 0 ? p0 : p1 );
      if  ( !pointOK[pfMatch.pointId] ) {
              pfMatch.fault = fault;
              found = RBTreeFindGet( tree, &pfMatch );
              if ( found ) found->isOK = TRUE;
      }
          }
    }
      }
      SurfaceSetDelete( SurfaceSetClear( faultSet ) );
  }
  key = EdgeKeyNext( edge, &iter );
  }

  /* Treat wrong points */
  {
  int   n;
  for ( n = 0; n < nPoints; n++ ) {
      /* Multiple errors allowed here: only a few expected in worst case */
      if( !pointOK[n] ) {
    PointFault_t  pfMatch;
    bool_t        isOK = TRUE;

    UTIL_ZERO( &pfMatch );
    pfMatch.pointId = n;
    pf = RBTreeFirstMatch( tree, &pfMatch, &iter );
    while ( pf ) {
          if ( !pf->isOK ) isOK = FALSE;
          pf = RBTreeNextMatch( tree, &pfMatch, &iter );
    }

    if ( !isOK ) {
          MeshMessageOpen( mesh, MeshStatusFaultPoint1Intersect );
          pf = RBTreeFirstMatch( tree, &pfMatch, &iter );
          MeshMessageAdd( mesh, "The faults\n" );
          while ( pf ) {
      SurfaceMeshMessageAdd( mesh, pf->fault );
      pf = RBTreeNextMatch( tree, &pfMatch, &iter );
          }
          MeshMessageAdd( mesh,
                          "intersect in a single point with coordinates\n" );
          PointMeshMessageAdd( mesh, pointSet, n );
          MeshMessageAdd( mesh, "The mesher cannot handle this situation,"
                " please change your model.\n" );
          MeshMessageClose( mesh );
    }
      }
  }
  }
  DIFREE( pointOK );
  RBTreeDelete( tree );
}

/* Check the model for self-intersection, if the front and back of faults are
   not identical only limited testing is possible */
static void MeshCheckIntersection( Mesh_t       *mesh,
                                   PointSet_t   *pointSet,
                                   SurfaceSet_t *surfaceSet )
{
  bool_t        surfTypeActive[surfTypeLast];
  ShiftPointSet_t *spSet = ShiftPointSetCreate( );

  /* We can check all surfaces for self intersection in 1 go */
  UTIL_SET( surfTypeActive, TRUE, surfTypeLast );
  MeshCheckSurfaceIntersection( mesh,
                surfTypeActive,
                surfaceSet,
                pointSet,
                spSet );

  ShiftPointSetDelete( spSet );
}

static void  MeshPrepareModel( Mesh_t *mesh )
{
  PointSet_t   *pointSet = mesh->modelData->pointSet;
  SurfaceSet_t *surfaceSet = mesh->modelData->surfaceSet;

  if ( !MESH_ERROR( mesh ) && PointSetSize( pointSet ) > 1 ) {
      int          id1, id2;
      double       measure;
      const double dist =
                      PointSetMinimumDistance( pointSet, &id1, &id2, &measure );
      const double crit = 1.e-3;
      if ( dist/measure < crit ) {
         char    buf[ BUFSIZ ];
         MeshMessageOpen( mesh, MeshStatusClosePoints );
         sprintf( buf,
                  "Smallest distance between two points in the model: %f\n",
                  dist );
         MeshMessageAdd( mesh, buf );
         PointMeshMessageAdd( mesh, pointSet, id1 );
         PointMeshMessageAdd( mesh, pointSet, id2 );
         MeshMessageClose( mesh );
      }
   }

   /* Any surfaces specified? */
   if ( !MESH_ERROR( mesh ) ) {
      if ( !SurfaceSetSize( surfaceSet ) ) {
         MeshMessageOpen( mesh, MeshStatusNoSurfaces );
         MeshMessageAdd( mesh, "No surfaces specified" );
         MeshMessageClose( mesh );
      }
   }

   /* Check smallest angle in any surface triangle */
   if ( !MESH_ERROR( mesh ) ) {
      MeshCheckTriangleAngles( mesh, pointSet, surfaceSet );
   }

   /* Front and back specified? */
   if ( !MESH_ERROR( mesh ) ) {
      MeshFrontBackCheck( mesh );
   }

   /* Give front and back side of fault **same** orientation */
   if ( !MESH_ERROR( mesh ) ) {
      MeshOrientFaults( mesh, pointSet, surfaceSet );
   }

   /* Set direction of all faults */
   if ( !MESH_ERROR( mesh ) ) {
      SurfaceSetFaultDirection( mesh, surfaceSet, pointSet );
   }

   /* Normal surfaces with isolated edges? */
   if ( !MESH_ERROR( mesh ) ) {
      MeshIsolatedEdges( mesh, surfaceSet, pointSet );
   }

   /* Same triangle in 2 normal surfaces? */
   if ( !MESH_ERROR( mesh ) ) {
      MeshSharedTriangles( mesh, surfaceSet, pointSet );
   }

   /* Check smallest angle between 2 triangles with common edge */
   if ( !MESH_ERROR( mesh ) ) {
      MeshCheckTriangleAngleBetween( mesh, pointSet, surfaceSet );
   }

#if 0
     SurfaceSetPrintConnectivity( mesh->modelData->surfaceSet, stderr );
     assert( 0 );
#endif

   /* Check for self-intersection */
   if ( !MESH_ERROR( mesh ) ) {
     MeshCheckIntersection( mesh, pointSet, surfaceSet );
   }

   /* Cut surfaces into pieces */
   if ( !MESH_ERROR( mesh ) ) {
      const bool_t    synchronizeFrontBack = TRUE;
      SurfaceSet_t   *surfSet = SurfaceSetSplitIntoSimpleSurface( mesh, mesh->modelData->surfaceSet, synchronizeFrontBack ); 
      SurfaceSetDelete( mesh->modelData->surfaceSet );
      mesh->modelData->surfaceSet = surfSet;
      surfaceSet = surfSet;
   }


#ifdef MRH_DEVELOP
   if ( !MESH_ERROR( mesh ) ) {
      /* Output the model as specified by client, including surface joints */
      SurfaceSetMarkOuterBoundary( mesh, TRUE, pointSet, surfaceSet ); 
      MeshEditWrite( MESH_EDIT_SURFACES_NAME, MESH_EDIT_WIREFRAME__NAME, mesh, pointSet, surfaceSet );
   }
#endif

   /* Single point intersection between the edges of 2 faults */
   if ( !MESH_ERROR( mesh ) ) {
      MeshSinglePointSeamIntersection( mesh, surfFrontFault, surfaceSet, pointSet );
   }
   if ( !MESH_ERROR( mesh ) ) {
     MeshSinglePointSeamIntersection( mesh, surfBackFault, surfaceSet, pointSet );
   }

   if ( !MESH_ERROR( mesh ) ) {
     SurfaceSetPurge( mesh->modelData->surfaceSet );
   }
 }

static void MeshGenerateTry( Mesh_t *mesh )
{
  ShiftPointSet_t *spSet = ShiftPointSetCreate( );
  PointSet_t   *pointSet = mesh->derivedData->pointSet;
  SurfaceSet_t *surfaceSet = mesh->derivedData->surfaceSet;

  MeshOutputDelete( mesh->meshOutput );

#if 0
   SurfaceSetToGocad( surfaceSet, pointSet );
#endif

   if ( !MESH_ERROR( mesh ) ) {
     mesh->derivedData->bodySet = BodySetCreate();
     ConstructBodies( mesh, spSet, pointSet, surfaceSet, mesh->derivedData->bodySet );
   }


   if ( !MESH_ERROR( mesh ) ) {
     bool_t   surfTypeActive[surfTypeLast];
     surfTypeActive[surfNormal-surfTypeFirst]     = FALSE;
     surfTypeActive[surfFrontFault-surfTypeFirst] = TRUE;
     surfTypeActive[surfBackFault-surfTypeFirst]  = TRUE;
     MeshCheckSurfaceIntersection( mesh, surfTypeActive, surfaceSet, pointSet, spSet );
   }

   /* Save some memory */
   if ( !MESH_ERROR( mesh ) ) {
      SurfaceSetPurge( surfaceSet );
   }

   /* Any normal bodies? */
   if ( !MESH_ERROR( mesh ) ) {
      int        nBody = 0;
      Iterator_t iter;
      Body_t    *body = BodySetFirst( mesh->derivedData->bodySet, &iter );
      while ( body ) {
    if ( BodyTypeGet( body ) == bodyNormal ) {
          nBody += 1;
    }
    body = BodySetNext( mesh->derivedData->bodySet, &iter );
      }
      if ( !nBody ) {
         MeshMessageOpen( mesh, MeshStatusNoBodies );
         MeshMessageAdd( mesh, "No bodies found" );
         MeshMessageClose( mesh );
      }
   }

  /* Mesh body by body */
  if ( !MESH_ERROR( mesh ) ) {
  RBTree_t    *innerBodies = BodyInnerCreate( mesh, mesh->derivedData->bodySet );
  Iterator_t   iter;
  BodyInner_t *bi = RBTreeFirst( innerBodies, &iter );
  mesh->meshOutput = MeshOutputCreate( );
  while ( bi && !MESH_ERROR( mesh ) ) {
      Body_t  *body = bi->body;
      if ( !MESH_ERROR( mesh ) && BodyTypeGet( body ) != bodyDegenerate ) {
    MeshAddSingleBody( mesh, spSet, body, bi->cavities, pointSet, mesh->meshOutput, &(mesh->derivedData->FemGvPath) );
      }
      bi = RBTreeNext( innerBodies, &iter );
  }

  if ( !MESH_ERROR( mesh ) ) {
      MeshIsCorrect( mesh, mesh->meshOutput->tetSet, pointSet, surfaceSet );
  }

  BodyInnerDelete( innerBodies );
  }

  ShiftPointSetDelete( spSet );
}

extern void MeshGenerate(const char* sDianaPath, 
             const char* sDiaSharePath,
             const char* sDianaLibPath,
             const char* sDianaSLibPath,
             const char* sFgvExeName, 
             void *meshV )
{
  Mesh_t   *mesh = meshV;

  /* Clean up the read buffer: not used anymore */
  mesh->readBuffer = MeshReadBufferDelete( mesh->readBuffer );

  /* Carry out various checks on the model as specified by the user */
  if ( !MESH_ERROR( mesh ) ) {
  MeshPrepareModel( mesh );
  }

  if ( !MESH_ERROR( mesh ) ) {
  int      n;

  double   shiftScaleFactor = 0.01;

  /* Store the warnings generated so far */
  MeshErrorData_t *orgMessages = mesh->errorData;
  mesh->errorData = NULL;

  for ( n = 0; n < 6; n++ ) {

      /* Pick up the model warnings */
      MeshErrorDataDelete( mesh->errorData );
      mesh->errorData = MeshErrorDataCopy( orgMessages );

      /* Duplicate surfaceSet and pointset */
      MeshDerivedDataDelete( mesh->derivedData );
      mesh->derivedData =  MeshDerivedDataCreate( );
      if ( !MESH_ERROR( mesh ) ) {
    mesh->derivedData->surfaceSet = SurfaceSetCopy( mesh->modelData->surfaceSet ); 
    mesh->derivedData->pointSet = PointSetCopy( mesh->modelData->pointSet );
      }

      if ( !MESH_ERROR( mesh ) ) {
    mesh->derivedData->FemGvPath.sDianaPath     = sDianaPath; 
    mesh->derivedData->FemGvPath.sDiaSharePath  = sDiaSharePath;
    mesh->derivedData->FemGvPath.sDianaLibPath  = sDianaLibPath;
    mesh->derivedData->FemGvPath.sDianaSLibPath = sDianaSLibPath;
    mesh->derivedData->FemGvPath.sFgvExeName    = sFgvExeName;

    /* Attempt to generate mesh */
    mesh->derivedData->shiftScaleFactor = shiftScaleFactor;
    MeshGenerateTry( mesh );
    if ( !MESH_ERROR( mesh ) ) {
          /* Succesfull */
          break;
    } else {
          shiftScaleFactor /= 2;
    }
      }
  }
  MeshErrorDataDelete( orgMessages );
  }
}

/* Angle in [DEG] */
extern double MeshAngleGetWarningCrit( const void *meshv )
{
   const Mesh_t* mesh = meshv;
   return mesh->modelData->angleWarningCrit;   
}


extern double   MeshTolerance( const void *meshv )
{
   const Mesh_t* mesh = meshv;
   return mesh->modelData->pointTolerance;   
}

extern int MeshMaxUserPointId( const void *meshv )
{
  const Mesh_t* mesh = meshv;
  return PointSetSize( mesh->modelData->pointSet );
}


extern double   MeshShiftScaleFactor( const void *meshv )
{
   const Mesh_t* mesh = meshv;
   return mesh->derivedData->shiftScaleFactor;   
}

/* Error handling */
extern int MeshErrorFlag( const void *meshV )
{
   const Mesh_t* mesh = meshV;
   return mesh->errorData->errFlag;
}

extern void MeshMessageOpen( void *meshV, const int status )
{
   Mesh_t *mesh = meshV;
   Message_t    *msg = SetNew( mesh->errorData->messages, 1 );
   assert( !mesh->errorData->messageOpen );
   mesh->errorData->messageOpen = TRUE;
   if ( status > 0 ) mesh->errorData->errFlag = TRUE;
   UTIL_ZERO( msg );
   msg->status = status;
}

extern void MeshMessageClose( void *meshV )
{
   Mesh_t *mesh = meshV;
   assert( mesh->errorData->messageOpen );
   mesh->errorData->messageOpen = FALSE;
}

extern void MeshMessageAdd( void *meshV, const char *line )
{
   Mesh_t      *mesh = meshV;
   const ssize_t nMessage = SetSize( mesh->errorData->messages );
   Message_t    *msg = SetElmGet( mesh->errorData->messages, nMessage-1 );
   const size_t  lineLen = strlen( line );
   assert( mesh->errorData->messageOpen );
   if ( msg->text ) {
      const size_t  newLen = strlen( line ) + strlen( msg->text ) + 1;
      msg->text = DIREALLOC( msg->text, 0, newLen );
      strcat( msg->text, line );
   } else {
      msg->text = DIMALLOC( lineLen+1 );
      strcpy( msg->text, line );
   }
}

extern int  MeshMessageStatus( const void *meshV, const int msgId )
{
   const Mesh_t      *mesh = meshV;
   const Message_t   *msg = SetElmGet( mesh->errorData->messages, msgId );
   assert( msg != NULL );
   return msg->status;
}


extern const char * MeshMessageText( const void *meshV, const int msgId )
{
   const Mesh_t      *mesh = meshV;
   const Message_t   *msg = SetElmGet( mesh->errorData->messages, msgId );
   assert( msg != NULL );
   return msg->text;
}

extern int MeshMessageSize( const void *meshV )
{
   const Mesh_t      *mesh = meshV;
   return SetSize( mesh->errorData->messages );
}

/* Output of the tet-mesher */
/* Nodes */
extern int NodeCount( 
                  const void                 *meshV )
{
  const Mesh_t* mesh = meshV;
  return (int)RBTreeSize( mesh->meshOutput->node2Point );
}

extern const double *NodeAt(
                  int                         nIndex,
                  const void                 *meshV )
{
  const Mesh_t* mesh = meshV;
  const int    *node2Point = RBTreeGet( mesh->meshOutput->node2Point, nIndex );
  return PointSetGet( mesh->derivedData->pointSet, node2Point[1] );
}

/* Tetrahedron elements */
extern int TetrahedronCount(
                  const void                 *meshV )
{
  const Mesh_t* mesh = meshV;
  return TetrahedronSetSize( mesh->meshOutput->tetSet );
}

extern int TetrahedronNodeAt( 
                  int                         nElementIndex, 
                  int                         nLocalNodeIndex, 
                  const void                 *meshV )
{
  const Mesh_t        *mesh = meshV;
  const Tetrahedron_t *tet = TetrahedronSetAt( mesh->meshOutput->tetSet, nElementIndex );
  int                  p[4];
  TetrahedronGet( tet, p+0, p+1, p+2, p+3 );
  assert( nLocalNodeIndex >= 0 && nLocalNodeIndex < 4 );
  return MeshOutputNode2Offset( mesh->meshOutput, p[nLocalNodeIndex] );
}

/* Mesh bodies */
extern int MeshBodyCount(
                  const void                 *meshV )
{
  const Mesh_t        *mesh = meshV;
  return ( mesh->meshOutput ? SetSize( mesh->meshOutput->bodyMesh ) : 0 );
}

extern int  MeshBodyTetrahedronCount( 
                  int                         nBodyIndex, 
                  const void                 *meshV )
{
  const Mesh_t     *mesh = meshV;
  const BodyMesh_t *bm = SetElmGet( mesh->meshOutput->bodyMesh, nBodyIndex );
  return (int)bm->tetrahedronSize;
}

extern int  MeshBodyTetrahedronAt( 
                  int                         nBodyIndex, 
                  int                         nElementIndex, 
                  const void                 *meshV )
{
  const Mesh_t     *mesh = meshV;
  const BodyMesh_t *bm = SetElmGet( mesh->meshOutput->bodyMesh, nBodyIndex );
  assert( nElementIndex >= 0 && nElementIndex < bm->tetrahedronSize );
  return (int)bm->tetrahedronFirst + nElementIndex;
}

extern int MeshBodySideSurfaceCount( 
                  int                         nBodyIndex, 
                  const void                 *meshV )
{
  const Mesh_t     *mesh = meshV;
  const BodyMesh_t *bm = SetElmGet( mesh->meshOutput->bodyMesh, nBodyIndex );
  return SurfaceSetSize( bm->sideSurfaces );
}

static Surface_t *MeshBodySideSurfaceGet( 
                  int                         nBodyIndex, 
                  int                         nSideSurfaceIndex,
                  const void                 *meshV )
{
  const Mesh_t     *mesh = meshV;
  const BodyMesh_t *bm = SetElmGet( mesh->meshOutput->bodyMesh, nBodyIndex );
  return SurfaceSetGet( bm->sideSurfaces, nSideSurfaceIndex );
}

extern int  MeshBodySideSurfaceParentIndex(
                  int                         nBodyIndex, 
                  int                         nSideSurfaceIndex,
                  const void                 *meshV )
{
  const Surface_t *surf = MeshBodySideSurfaceGet( nBodyIndex, nSideSurfaceIndex, meshV );
  return SurfaceID( surf );
}

extern int  MeshBodySideSurfaceParentType(
                  int                         nBodyIndex, 
                  int                         nSideSurfaceIndex,
                  const void                 *meshV )
{
  const Surface_t *surf = MeshBodySideSurfaceGet( nBodyIndex, nSideSurfaceIndex, meshV );
  return SurfaceType( surf );
}

extern int  MeshBodySideSurfaceTriangleCount(
                  int                         nBodyIndex, 
                  int                         nSideSurfaceIndex,
                  const void                 *meshV )
{
  const Surface_t *surf = MeshBodySideSurfaceGet( nBodyIndex, nSideSurfaceIndex, meshV );
  return SurfaceSize( surf );
}

extern int  MeshBodySideSurfaceTriangleInterfaceIdx( int nBodyIndex, int nSideSurfaceIndex, int nTriangleIndex, const void* meshV )
{
  const Mesh_t     *mesh = meshV;
  const Surface_t  *surf = MeshBodySideSurfaceGet( nBodyIndex, nSideSurfaceIndex, mesh );
  const Triangle_t *triangle = SurfaceGet( surf, nTriangleIndex );
  return TriangleInterfaceIdx( triangle );
}

extern int  MeshBodySideSurfaceTriangleNode( 
                  int                         nBodyIndex, 
                  int                         nSideSurfaceIndex, 
                  int                         nTriangleIndex, 
                  int                         nLocalNodeIndex, 
                  const void                 *meshV )
{
  const Mesh_t     *mesh = meshV;
  const Surface_t  *surf = MeshBodySideSurfaceGet( nBodyIndex, nSideSurfaceIndex, mesh );
  const Triangle_t *triangle = SurfaceGet( surf, nTriangleIndex );
  const int        *points = TrianglePoints( triangle );
  assert( nLocalNodeIndex >= 0 && nLocalNodeIndex < 3 );
  return MeshOutputNode2Offset( mesh->meshOutput, points[nLocalNodeIndex] );
}
