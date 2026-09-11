/* GTS - Library for the manipulation of triangulated surfaces
 * Copyright (C) 1999 Stéphane Popinet
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.	 See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public
 * License along with this library; if not, write to the
 * Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 * Boston, MA 02111-1307, USA.
 */

#include <math.h>
#include "gts.h"

/**
 * gts_matrix_new:
 * @a00: element [0][0].
 * @a01: element [0][1].
 * @a02: element [0][2].
 * @a10: element [1][0].
 * @a11: element [1][1].
 * @a12: element [1][2].
 * @a20: element [2][0].
 * @a21: element [2][1].
 * @a22: element [2][2].
 *
 * Allocates memory and initializes a new #GtsMatrix.
 *
 * Returns: a pointer to the newly created #GtsMatrix.
 */
GtsMatrix * gts_matrix_new (gdouble a00, gdouble a01, gdouble a02,
        gdouble a10, gdouble a11, gdouble a12,
        gdouble a20, gdouble a21, gdouble a22)
{
  GtsMatrix * m;

  m = g_malloc (3*sizeof (GtsVector));

  m[0][0] = a00; m[1][0] = a10; m[2][0] = a20;
  m[0][1] = a01; m[1][1] = a11; m[2][1] = a21;
  m[0][2] = a02; m[1][2] = a12; m[2][2] = a22;

  return m;
}

/**
 * gts_matrix_assign:
 * @m: a #GtsMatrix.
 * @a00: element [0][0].
 * @a01: element [0][1].
 * @a02: element [0][2].
 * @a10: element [1][0].
 * @a11: element [1][1].
 * @a12: element [1][2].
 * @a20: element [2][0].
 * @a21: element [2][1].
 * @a22: element [2][2].
 *
 * Set values of matrix elements.
 */
void gts_matrix_assign (GtsMatrix * m,
      gdouble a00, gdouble a01, gdouble a02,
      gdouble a10, gdouble a11, gdouble a12,
      gdouble a20, gdouble a21, gdouble a22)
{
  g_return_if_fail (m != NULL);

  m[0][0] = a00; m[1][0] = a10; m[2][0] = a20;
  m[0][1] = a01; m[1][1] = a11; m[2][1] = a21;
  m[0][2] = a02; m[1][2] = a12; m[2][2] = a22;
}

/**
 * gts_matrix_projection:
 * @t: a #GtsTriangle.
 *
 * Creates a new #GtsMatrix representing the projection onto a plane of normal
 * given by @t.
 *
 * Returns: a pointer to the newly created #GtsMatrix.
 */
GtsMatrix * gts_matrix_projection (GtsTriangle * t)
{
  GtsVertex * v1, * v2, * v3;
  GtsEdge * e1, * e2, * e3;
  GtsMatrix * m;
  gdouble x1, y1, z1, x2, y2, z2, x3, y3, z3, l;
  
  g_return_val_if_fail (t != NULL, NULL);

  m = g_malloc (3*sizeof (GtsVector));
  gts_triangle_vertices_edges (t, NULL, &v1, &v2, &v3, &e1, &e2, &e3);

  x1 = GTS_POINT (v2)->x - GTS_POINT (v1)->x; 
  y1 = GTS_POINT (v2)->y - GTS_POINT (v1)->y; 
  z1 = GTS_POINT (v2)->z - GTS_POINT (v1)->z;
  x2 = GTS_POINT (v3)->x - GTS_POINT (v1)->x; 
  y2 = GTS_POINT (v3)->y - GTS_POINT (v1)->y; 
  z2 = GTS_POINT (v3)->z - GTS_POINT (v1)->z;
  x3 = y1*z2 - z1*y2; y3 = z1*x2 - x1*z2; z3 = x1*y2 - y1*x2;
  x2 = y3*z1 - z3*y1; y2 = z3*x1 - x3*z1; z2 = x3*y1 - y3*x1;

  g_assert ((l = sqrt (x1*x1 + y1*y1 + z1*z1)) > 0.0);
  m[0][0] = x1/l; m[1][0] = y1/l; m[2][0] = z1/l;
  g_assert ((l = sqrt (x2*x2 + y2*y2 + z2*z2)) > 0.0);
  m[0][1] = x2/l; m[1][1] = y2/l; m[2][1] = z2/l;
  g_assert ((l = sqrt (x3*x3 + y3*y3 + z3*z3)) > 0.0);
  m[0][2] = x3/l; m[1][2] = y3/l; m[2][2] = z3/l;
  return m;
}

/**
 * gts_matrix_transpose:
 * @m: a #GtsMatrix.
 *
 * Returns: a pointer to a newly created #GtsMatrix transposed of @m.
 */
GtsMatrix * gts_matrix_transpose (GtsMatrix * m)
{
  GtsMatrix * mi;

  g_return_val_if_fail (m != NULL, NULL);

  mi = g_malloc (3*sizeof (GtsVector));

  mi[0][0] = m[0][0]; mi[1][0] = m[0][1]; mi[2][0] = m[0][2]; 
  mi[0][1] = m[1][0]; mi[1][1] = m[1][1]; mi[2][1] = m[1][2]; 
  mi[0][2] = m[2][0]; mi[1][2] = m[2][1]; mi[2][2] = m[2][2]; 

  return mi;
}

/**
 * gts_matrix_determinant:
 * @m: a #GtsMatrix.
 *
 * Returns: the value of the det(@m).
 */
gdouble gts_matrix_determinant (GtsMatrix * m)
{
  g_return_val_if_fail (m != NULL, 0.0);

  return (m[0][0]*(m[1][1]*m[2][2] - m[2][1]*m[1][2]) - 
    m[0][1]*(m[1][0]*m[2][2] - m[2][0]*m[1][2]) + 
    m[0][2]*(m[1][0]*m[2][1] - m[2][0]*m[1][1]));
}

/**
 * gts_matrix_inverse:
 * @m: a #GtsMatrix.
 *
 * Returns: a pointer to a newly created #GtsMatrix inverse of @m or %NULL
 * if @m is not invertible.
 */
GtsMatrix * gts_matrix_inverse (GtsMatrix * m)
{
  GtsMatrix * mi;
  gdouble det;

  g_return_val_if_fail (m != NULL, NULL);
  
  det = gts_matrix_determinant (m);
  if (det == 0.0)
  return NULL;

  mi = g_malloc (3*sizeof (GtsVector));

  mi[0][0] = (m[1][1]*m[2][2] - m[1][2]*m[2][1])/det; 
  mi[0][1] = (m[2][1]*m[0][2] - m[0][1]*m[2][2])/det;
  mi[0][2] = (m[0][1]*m[1][2] - m[1][1]*m[0][2])/det; 
  mi[1][0] = (m[1][2]*m[2][0] - m[1][0]*m[2][2])/det; 
  mi[1][1] = (m[0][0]*m[2][2] - m[2][0]*m[0][2])/det; 
  mi[1][2] = (m[1][0]*m[0][2] - m[0][0]*m[1][2])/det; 
  mi[2][0] = (m[1][0]*m[2][1] - m[2][0]*m[1][1])/det; 
  mi[2][1] = (m[2][0]*m[0][1] - m[0][0]*m[2][1])/det; 
  mi[2][2] = (m[0][0]*m[1][1] - m[0][1]*m[1][0])/det; 

  return mi;
}

/**
 * gts_matrix_print:
 * @m: a #GtsMatrix.
 * @fptr: a file descriptor.
 * 
 * Print @m to file @fptr.
 */
void gts_matrix_print (GtsMatrix * m, FILE * fptr)
{
  g_return_if_fail (m != NULL);
  g_return_if_fail (fptr != NULL);

  fprintf (fptr, 
     "[[%15.7g %15.7g %15.7g ]\n"
     " [%15.7g %15.7g %15.7g ]\n"
     " [%15.7g %15.7g %15.7g ]]\n",
     m[0][0], m[0][1], m[0][2],
     m[1][0], m[1][1], m[1][2],
     m[2][0], m[2][1], m[2][2]);
}

/**
 * gts_vector_print:
 * @v: a #GtsVector.
 * @fptr: a file descriptor.
 * 
 * Print @s to file @fptr.
 */
void gts_vector_print (GtsVector v, FILE * fptr)
{
  g_return_if_fail (fptr != NULL);

  fprintf (fptr, 
     "[%15.7g %15.7g %15.7g ]\n",
     v[0], v[1], v[2]);
}

/* [cos(alpha)]^2 */
#define COSALPHA2 0.999695413509 /* alpha = 1 degree */
/* [sin(alpha)]^2 */
#define SINALPHA2 3.04586490453e-4 /* alpha = 1 degree */

/**
 * gts_matrix_compatible_row:
 * @A: a #GtsMatrix.
 * @b: a #GtsVector.
 * @n: the number of previous constraints of @A.x=@b.
 * @A1: a #GtsMatrix.
 * @b1: a #GtsVector.
 *
 * Given a system of @n constraints @A.x=@b adds to it the compatible
 * constraints defined by @A1.x=@b1. The compatibility is determined
 * by insuring that the resulting system is well-conditioned (see
 * Lindstrom and Turk (1998, 1999)).
 *
 * Returns: the number of constraints of the resulting system.  
 */
guint gts_matrix_compatible_row (GtsMatrix * A,
         GtsVector b,
         guint n,
         GtsVector A1,
         gdouble b1)
{
  gdouble na1;
  
  g_return_val_if_fail (A != NULL, 0);

  na1 = gts_vector_scalar (A1, A1);
  if (na1 == 0.0)
  return n;

  /* normalize row */
  na1 = sqrt (na1);
  A1[0] /= na1; A1[1] /= na1; A1[2] /= na1; b1 /= na1;

  if (n == 1) {
  gdouble a0a1 = gts_vector_scalar (A[0], A1);
  if (a0a1*a0a1 >= COSALPHA2)
      return 1;
  }
  else if (n == 2) {
  GtsVector V;
  gdouble s;
  
  gts_vector_cross (V, A[0], A[1]);
  s = gts_vector_scalar (V, A1);
  if (s*s <= gts_vector_scalar (V, V)*SINALPHA2)
      return 2;
  }

  A[n][0] = A1[0]; A[n][1] = A1[1]; A[n][2] = A1[2]; b[n] = b1;
  return n + 1;
}

/**
 * gts_matrix_quadratic_optimization:
 * @A: a #GtsMatrix.
 * @b: a #GtsVector.
 * @n: the number of constraints (must be smaller than 3).
 * @H: a symmetric positive definite Hessian.
 * @c: a #GtsVector.
 *
 * Solve a quadratic optimization problem: Given a quadratic objective function
 * f which can be written as: f(x) = x^t.@H.x + @c^t.x + k, where @H is the 
 * symmetric positive definite Hessian of f and k is a constant, find the
 * minimum of f subject to the set of @n prior linear constraints, defined by
 * the first @n rows of @A and @b (@A.x = @b). The new constraints given by
 * the minimization are added to @A and @b only if they are linearly
 * independent as determined by gts_matrix_compatible_row().
 *
 * Returns: the new number of constraints defined by @A and @b.
 */
guint gts_matrix_quadratic_optimization (GtsMatrix * A,
           GtsVector b,
           guint n,
           GtsMatrix * H,
           GtsVector c)
{
  g_return_val_if_fail (A != NULL, 0);
  g_return_val_if_fail (b != NULL, 0);
  g_return_val_if_fail (n < 3, 0);
  g_return_val_if_fail (H != NULL, 0);

  switch (n) {
  case 0: {
  n = gts_matrix_compatible_row (A, b, n, H[0], - c[0]);
  n = gts_matrix_compatible_row (A, b, n, H[1], - c[1]);
  n = gts_matrix_compatible_row (A, b, n, H[2], - c[2]);
  return n;
  }
  case 1: {
  GtsVector Q0 = {0., 0., 0.};
  GtsVector Q1 = {0., 0., 0.};
  GtsVector A1;
  gdouble max = A[0][0]*A[0][0];
  guint d = 0;

  /* build a vector orthogonal to the constraint */
  if (A[0][1]*A[0][1] > max) { max = A[0][1]*A[0][1]; d = 1; }
  if (A[0][2]*A[0][2] > max) { max = A[0][2]*A[0][2]; d = 2; }
  switch (d) {
  case 0: Q0[0] = - A[0][2]/A[0][0]; Q0[2] = 1.0; break;
  case 1: Q0[1] = - A[0][2]/A[0][1]; Q0[2] = 1.0; break;
  case 2: Q0[2] = - A[0][0]/A[0][2]; Q0[0] = 1.0; break;
  }

  /* build a second vector orthogonal to the first and to the constraint */
  gts_vector_cross (Q1, A[0], Q0);

  A1[0] = gts_vector_scalar (Q0, H[0]);
  A1[1] = gts_vector_scalar (Q0, H[1]);
  A1[2] = gts_vector_scalar (Q0, H[2]);

  n = gts_matrix_compatible_row (A, b, n, A1, - gts_vector_scalar (Q0, c));
  
  A1[0] = gts_vector_scalar (Q1, H[0]);
  A1[1] = gts_vector_scalar (Q1, H[1]);
  A1[2] = gts_vector_scalar (Q1, H[2]);

  n = gts_matrix_compatible_row (A, b, n, A1, - gts_vector_scalar (Q1, c));

  return n;
  }
  case 2: {
  /* build a vector orthogonal to the two constraints */
  GtsVector A1, Q;

  gts_vector_cross (Q, A[0], A[1]);
  A1[0] = gts_vector_scalar (Q, H[0]);
  A1[1] = gts_vector_scalar (Q, H[1]);
  A1[2] = gts_vector_scalar (Q, H[2]);
  
  n = gts_matrix_compatible_row (A, b, n, A1, - gts_vector_scalar (Q, c));

  return n;
  }
  default:
  g_assert_not_reached ();
  }
  return 0;
}

/**
 * gts_matrix_destroy:
 * @m: a #GtsMatrix.
 *
 * Free all the memory allocated for @m.
 */
void gts_matrix_destroy (GtsMatrix * m)
{
  g_free (m);
}

/**
 * gts_matrix_product:
 * @m1: a #GtsMatrix.
 * @m2: another #GtsMatrix.
 *
 * Returns: a new #GtsMatrix, product of @m1 and @m2.
 */
GtsMatrix * gts_matrix_product (GtsMatrix * m1, GtsMatrix * m2)
{
  guint i, j;
  GtsMatrix * m;

  g_return_val_if_fail (m1 != NULL, NULL);
  g_return_val_if_fail (m2 != NULL, NULL);
  g_return_val_if_fail (m1 != m2, NULL);

  m = gts_matrix_new (0., 0., 0.,
          0., 0., 0.,
          0., 0., 0.);
  for (i = 0; i < 3; i++)
  for (j = 0; j < 3; j++)
      m[i][j] = m1[i][0]*m2[0][j] + m1[i][1]*m2[1][j] + m1[i][2]*m2[2][j];
  return m;
}
