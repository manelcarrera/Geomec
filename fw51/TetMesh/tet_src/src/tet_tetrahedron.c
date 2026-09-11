 /*                                         Copyright (c) 2011 TNO DIANA BV */
#include "tet_tetrahedron.h"

extern TetrahedronSet_t *TetrahedronSetCreate( void )
{
  return SetCreate( 0, sizeof( Tetrahedron_t ) );
}

extern TetrahedronSet_t *TetrahedronSetDelete( 
                  TetrahedronSet_t           *tetSet )
{
  return SetDelete( tetSet );
}

static Tetrahedron_t *TetrahedronInit( 
                  Tetrahedron_t              *tet,
                  int                         p0, 
                  int                         p1, 
                  int                         p2, 
                  int                         p3 )
{
  tet->nodes[0] = p0;
  tet->nodes[1] = p1;
  tet->nodes[2] = p2;
  tet->nodes[3] = p3;
  return tet;
}

extern void TetrahedronSetAdd( 
                  TetrahedronSet_t           *tetSet, 
                  int                         p0, 
                  int                         p1, 
                  int                         p2, 
                  int                         p3 )
{
  Tetrahedron_t tet;
  TetrahedronInit( &tet, p0, p1, p2, p3 );
  SetAdd( tetSet, &tet, 1 );
}

extern int TetrahedronSetSize( 
                  const TetrahedronSet_t     *tetSet )
{
  return SetSize( tetSet );
}

extern const Tetrahedron_t *TetrahedronSetFirst( 
                  TetrahedronSet_t           *tetSet,
                  Iterator_t                 *iter )
{
  return SetFirst( tetSet, iter );
}

extern const Tetrahedron_t *TetrahedronSetNext( 
                  TetrahedronSet_t           *tetSet,
                  Iterator_t                 *iter )
{
  return SetNext( tetSet, iter );
}

extern void TetrahedronGet(  
                  const Tetrahedron_t        *tet,
                  int                        *p0, 
                  int                        *p1, 
                  int                        *p2, 
                  int                        *p3 )
{
  *p0 = tet->nodes[0];
  *p1 = tet->nodes[1];
  *p2 = tet->nodes[2];
  *p3 = tet->nodes[3];
}

extern void TetrahedronPrint(
                  const Tetrahedron_t        *tet,
                  FILE                       *fp )
{
  int  p0, p1, p2,p3;
  TetrahedronGet( tet, &p0, &p1, &p2, &p3 );
  fprintf( fp, "TETRAHEDRON: %d %d %d %d\n", p0, p1, p2 ,p3 );
}

extern void TetrahedronSetPrint(
                  TetrahedronSet_t           *tetSet, 
                  FILE                       *fp )
{
  Iterator_t           iter;
  const Tetrahedron_t *tet = TetrahedronSetFirst( tetSet, &iter );
  fprintf( fp, "\nTETRAHEDRON-SET: szie=%d\n\n", TetrahedronSetSize( tetSet ) );
  while ( tet ) {
  TetrahedronPrint( tet, fp );
  tet = TetrahedronSetNext( tetSet, &iter );
  }
}

extern double TetrahedronVolume( 
                  const Tetrahedron_t        *tet, 
                  PointSet_t                 *pointSet )
{
  const double  *p0 = PointSetGet( pointSet, tet->nodes[0] );
  const double  *p1 = PointSetGet( pointSet, tet->nodes[1] );
  const double  *p2 = PointSetGet( pointSet, tet->nodes[2] );
  const double  *p3 = PointSetGet( pointSet, tet->nodes[3] );
  const double   volume = TetrahedronVolumeFromPoints( p0, p1, p2, p3 );
  return volume;
}

extern double TetrahedronSetVolume( 
                  TetrahedronSet_t           *tetSet, 
                  PointSet_t                 *pointSet )
{
   double               volume = 0;
  Iterator_t            iter;
   const Tetrahedron_t *tet = TetrahedronSetFirst( tetSet, &iter );
   while ( tet ) {
     volume += TetrahedronVolume( tet, pointSet );
     tet = TetrahedronSetNext( tetSet, &iter );
   }
   return volume;
}

extern TetrahedronSet_t *TetrahedronSetRenumber( 
                  TetrahedronSet_t           *oldSet, 
                  const int                  *old2new )
{
  Iterator_t        iter;
  TetrahedronSet_t *result = TetrahedronSetCreate( );
  const Tetrahedron_t *tet = TetrahedronSetFirst( oldSet, &iter );
  while ( tet ) {
  int  p0, p1, p2, p3;
  TetrahedronGet( tet, &p0, &p1, &p2, &p3 );
  TetrahedronSetAdd( result, old2new[p0], old2new[p1], old2new[p2], old2new[p3] );
  tet = TetrahedronSetNext( oldSet, &iter );
  }
  return result;
}

extern const Tetrahedron_t *TetrahedronSetAt( 
                  const TetrahedronSet_t     *tetSet, 
                  int                         ofset )
{
  return SetElmGet( tetSet, ofset );
}

extern double TetrahedronVolumeFromPoints( 
                  const double               *p0, 
                  const double               *p1, 
                  const double               *p2, 
                  const double               *p3 )
{
   double         s1[3], s2[3], s3[3], outp[3], vol;

   VecOperVec( s1, EqlPos, p1, OperMinus, p0, 3 );
   VecOperVec( s2, EqlPos, p2, OperMinus, p0, 3 );
   VecOperVec( s3, EqlPos, p3, OperMinus, p0, 3 );
   OuterProduct( s1, s2, outp );
   vol = VecInprod( outp, s3, 3 ) / 6.;

   return ABS( vol );
}
