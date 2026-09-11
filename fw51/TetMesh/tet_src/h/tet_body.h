 /* Copyright (c) 2011 TNO DIANA BV                              Confidential */
#ifndef BODY_H
#define BODY_H

#include "tet_utils.h"
#include "Set.h"
#include "tet_surface.h"
#include "RBTree.h"
#include "BoundBox.h"

typedef struct BodySurface_t {
   Surface_t  *surface;
   int         dir;
} BodySurface_t;

typedef enum {
     bodyNormal
   , bodyDegenerate
} BodyType_t;

typedef struct Body_t {
   Set_t         *bodySurface;
   struct Body_t *outer;
   BodyType_t     type;
   size_t         objId;
   int            printId; /* id withing bodySet */
   int            group;
} Body_t;

#define BODY_PRINTID_UNDEFINED 0

extern Body_t *BodyCreate( void * );
extern Body_t *BodyDelete( Body_t * );
extern Body_t *BodyCopy( void *, const Body_t * );
extern CompareFunction_t BodyComp;
extern void  BodyAdd( Body_t *, Surface_t *, bool_t );
extern BodySurface_t *BodyFirst( Body_t *, Iterator_t * );
extern BodySurface_t *BodyNext( Body_t *, Iterator_t * );
extern void BodyPrint( Body_t *, FILE * );
extern int BodySize( const Body_t * );
extern void BodyOuterSet( Body_t *, Body_t * );
extern Body_t *BodyOuterGet( Body_t * );
extern void BodyTypeSet( Body_t * );
extern BodyType_t BodyTypeGet( const Body_t * );
extern BodySurface_t *BodyGet( Body_t *body, int id );

extern BoundBox_t *BodyBoundBox( Body_t *, PointSet_t *, BoundBox_t * );

extern bool_t BodySurfacePosDir( const BodySurface_t * );
extern Surface_t *BodySurfaceGet( BodySurface_t * );
extern int BodyGroupGet( const Body_t * );
extern void BodyGroupSet( Body_t *, int );
extern int BodyPrintIdGet( const Body_t * );
extern void BodyPrintIdSet( Body_t *, int );
extern bool_t BodyContains( Body_t *, const Surface_t *, bool_t );

#endif
