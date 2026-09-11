 /*                                         Copyright (c) 2011 TNO DIANA BV */
#include "tet_utils.h"
#include "tet_mesh.h"
#include "tet_body.h"

static int bodyId;  /* For easy sorting */

extern Body_t *BodyCreate( void *mesh )
{
   Body_t *result = DIMALLOC( sizeof(*result) );
   UNUSED(mesh);
   UTIL_ZERO( result );
   result->bodySurface = SetCreate( 0, sizeof( BodySurface_t ) );
   result->type = bodyNormal;
   result->group = -1; /* Undefined */
   result->printId = BODY_PRINTID_UNDEFINED;
   result->objId = bodyId++;
   return result;
}

extern Body_t *BodyDelete( Body_t *body )
{
   if ( body ) {
      SetDelete( body->bodySurface );
      DIFREE( body );
   }
   return body;
}

extern void BodyAdd( Body_t *body, Surface_t *surface, bool_t posDir )
{
   BodySurface_t   *bodySurface = SetNew( body->bodySurface, 1 );
   bodySurface->surface = surface;
   bodySurface->dir = posDir;
}

extern int BodyComp( const void *body0V, const void *body1V )
{
   const Body_t  *body0 = body0V;
   const Body_t  *body1 = body1V;
   return UTIL_COMPARE( body0->objId,  body1->objId );
}

extern Body_t *BodyCopy( void *mesh, const Body_t *body )
{
   Body_t *result = BodyCreate( mesh );
   result->type = BodyTypeGet( body );
   SetAdd( result->bodySurface, SetGet( body->bodySurface ), SetSize( body->bodySurface ) );
   BodyGroupSet( result, BodyGroupGet( body ) );
   BodyPrintIdSet( result, BodyPrintIdGet( body ) );
   return result;
}

extern BodySurface_t *BodyGet( Body_t *body, int id )
{
   BodySurface_t   *result = NULL;
   assert( id >= 0 );
   if ( id < SetSize( body->bodySurface ) ) {
      result = SetElmGet( body->bodySurface,  id );
   }
   return result;
}

extern BodySurface_t *BodyFirst( Body_t *body, Iterator_t *iter )
{
   return SetFirst( body->bodySurface, iter );
}


extern BodySurface_t *BodyNext( Body_t *body, Iterator_t *iter )
{
   return SetNext( body->bodySurface, iter );
}

extern void BodyPrint( Body_t *body, FILE *fp )
{
   Iterator_t       iter;
   BodySurface_t   *surf = BodyFirst( body, &iter );
   int              size = 0;
   fprintf( fp, "\nBODY ID=%d\t", BodyPrintIdGet( body ) );
   if ( BodyTypeGet( body ) == bodyNormal ) {
      fprintf( fp, "(normal" );
   } else if ( BodyTypeGet( body ) == bodyDegenerate ) {
      fprintf( fp, "(degenerate" );
   }
   if ( body->outer ) fprintf( fp, ", subbody of ID=%d ", BodyPrintIdGet( body->outer) );
   fprintf( fp, ")\n" );
   while ( surf ) {
      const Surface_t *s = BodySurfaceGet( surf );
      char             surfBuf[ BUFSIZ ];
      fprintf( fp, "BODY_SURFACE %d: %s\n", size, SurfaceUserInfo( surfBuf, s ) );
      size += 1;
      surf = BodyNext( body, &iter );
   }
}

extern int BodySize( const Body_t *body)
{
   return SetSize( body->bodySurface );
}

extern void BodyTypeSet( Body_t *body )
{
  BodyType_t     type = bodyDegenerate;
  Iterator_t     iter;
  BodySurface_t *bSurf = BodyFirst( body, &iter );
  while ( bSurf ) {
    if ( SurfaceType( BodySurfaceGet( bSurf ) ) == surfNormal ) {
      type = bodyNormal;
      break;
    }
    bSurf = BodyNext( body, &iter );
  }
   body->type = type;
}

extern BodyType_t BodyTypeGet( const Body_t *body )
{
   assert( body != NULL );
   return body->type;
}

extern bool_t BodySurfacePosDir( const BodySurface_t *bs )
{
   return ( bs->dir != 0 );
}

extern Surface_t *BodySurfaceGet( BodySurface_t *bs )
{
   return bs->surface;
}

extern BoundBox_t *BodyBoundBox( Body_t *body, PointSet_t *pointSet, BoundBox_t *boundBox )
{
  Iterator_t      iter;
   BodySurface_t *bs = BodyFirst( body, &iter );
   while ( bs ) {
      Surface_t *surf = BodySurfaceGet( bs );
      boundBox = SurfaceBoundBox( surf, pointSet, boundBox );
      bs = BodyNext( body, &iter );
   }
   return boundBox;
}

extern void BodyOuterSet( Body_t *body, Body_t *outerBody )
{
   assert( body != NULL );
   body->outer = outerBody;
}

extern Body_t *BodyOuterGet( Body_t *body )
{
   assert( body != NULL );
   return body->outer;
}

static bool_t BodyGroupIsDefined( const Body_t *body )
{
  return ( body->group >= 0 );
}

extern int BodyGroupGet( const Body_t *body )
{
  assert( BodyGroupIsDefined( body ) );
  return body->group;
}

extern void BodyGroupSet( Body_t *body, int group )
{
  body->group = group;
}

extern int BodyPrintIdGet( const Body_t *body )
{
  return body->printId;
}

extern void BodyPrintIdSet( Body_t *body, int printId )
{
  body->printId = printId;
}

extern bool_t BodyContains( 
                  Body_t                     *body, 
                  const Surface_t            *surface, 
                  bool_t                      posDir )
{
  bool_t isContained = FALSE;
  int    id;
  for ( id = 0; !isContained && id < BodySize( body ); id++ ) {
    BodySurface_t *bs = BodyGet( body, id );
    isContained = (  !SurfaceComp( surface, BodySurfaceGet( bs ) ) && ( !BodySurfacePosDir( bs ) == !posDir ) );
  }
  return isContained;
}
