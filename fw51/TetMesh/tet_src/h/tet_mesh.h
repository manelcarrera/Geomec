 /* Copyright (c) 2011 TNO DIANA BV                              Confidential */
#ifndef MESH_H
#define MESH_H


#ifdef __cplusplus
extern "C" {
#endif

/* Create and deletion of the mesh */
void  *MeshCreate( double );
void  *MeshDelete( void * );

/* Surface input of the tet-mesher */
void     MeshPointAdd( void *, const double, const double, const double );
void     MeshSurfaceOpen( void *, int, const char*, bool_t );
void     MeshSurfaceClose( void * );
void     MeshSurfaceAdd( void *, const double *, const double *, const double * );

/* Generate function */
void     MeshGenerate(const char* sDianaPath, 
            const char* sDiaSharePath,
            const char* sDianaLibPath,
            const char* sDianaSLibPath,
            const char* sFgvExeName, 
            void * );

/* Private functions */
double   MeshTolerance( const void * );
double   MeshShiftScaleFactor( const void * );
int      MeshSize( const void * );
double   MeshAngleGetWarningCrit( const void * );
int      MeshMaxUserPointId( const void * );

#define MESH_ERROR(mesh) ( MeshErrorFlag(mesh) )

enum {
      MeshFirstWarning = -100000 /* Large enough to keep warnings negative */
   ,  MeshStatusJointDegeneracy
   ,  MeshStatusSmallAngle
   ,  MeshStatusClosePoints
   ,  MeshStatusFaultDirection
   ,  MeshStatusOK = 0 /* Never change */
   ,  MeshStatusFaultWarped
   ,  MeshStatusSurfaceRead
   ,  MeshStatusBodyNesting
   ,  MeshStatusSurfaceIdExists
   ,  MeshStatusFault2Triangle
   ,  MeshStatusDisconnectedFault
   ,  MeshStatusDegenerateTriangle
   ,  MeshStatusFaultOrientation
   ,  MeshStatusNoFrontOrBack
   ,  MeshStatusIsolatedEdge
   ,  MeshStatusNoSurfaces
   ,  MeshStatusNoBodies
   ,  MeshStatusNoPoints
   ,  MeshStatusTriangle2Surfaces
   ,  MeshStatusEdgeOverConnected
   ,  MeshStatusSurfaceNotInBody
   ,  MeshStatusBodyMeshFailure
   ,  MeshStatusMeshFailure
   ,  MeshStatusFaultPoint1Intersect
   ,  MeshStatusNotHorizon
   ,  MeshStatusFaultShift
   ,  MeshStatusOuterBoundary
   ,  MeshStatusIsolatedEdgeAfterShift
   ,  MeshStatusGocadUnknownPoint
   ,  MeshStatusGocadNoTriangle
   ,  MeshStatusGocadNoCoordinate
   ,  MeshStatusGocadNoReadFile
};

/* Error handling */
extern int  MeshErrorFlag( const void * );
extern void MeshMessageOpen( void *, const int  );
extern void MeshMessageClose( void * );
extern void MeshMessageAdd( void *meshV, const char *line );
extern void MeshMessageAddSurface( void *, const void * );
extern int  MeshMessageStatus( const void *, const int );
extern const char * MeshMessageText( const void *, const int );
extern int MeshMessageSize( const void * );

/* Output of the tet-mesher */
/* Nodes */
extern int NodeCount( const void *meshV );
extern const double* NodeAt( int nIndex, const void* meshV );

/* Tetrahedron elements */
extern int TetrahedronCount( const void *meshV );
extern int TetrahedronNodeAt( int nElementIndex, int nLocalNodeIndex, const void *meshV );

/* Mesh bodies */
extern int	MeshBodyCount( const void * );
extern int  MeshBodyTetrahedronCount( int nBodyIndex, const void* );
extern int  MeshBodyTetrahedronAt( int nBodyIndex, int nElementIndex, const void* );
extern int  MeshBodySideSurfaceCount( int nBodyIndex, const void* );
extern int  MeshBodySideSurfaceParentIndex( int nBodyIndex, int nSideSurfaceIndex, const void* );
extern int  MeshBodySideSurfaceParentType( int nBodyIndex, int nSideSurfaceIndex, const void* );
extern int  MeshBodySideSurfaceTriangleCount( int nBodyIndex, int nSideSurfaceIndex, const void* );
extern int  MeshBodySideSurfaceTriangleInterfaceIdx( int nBodyIndex, int nSideSurfaceIndex, int nTriangleIndex, const void* );
extern int  MeshBodySideSurfaceTriangleNode( int nBodyIndex, int nSideSurfaceIndex, int nTriangleIndex, int nLocalNodeIndex, const void* );

#ifdef __cplusplus
}
#endif


#endif
