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

#include "gts.h"
#include "gts-private.h"
#include "config.h"

const guint gts_major_version = GTS_MAJOR_VERSION;
const guint gts_minor_version = GTS_MINOR_VERSION;
const guint gts_micro_version = GTS_MICRO_VERSION;
const guint gts_interface_age = GTS_INTERFACE_AGE;
const guint gts_binary_age = GTS_BINARY_AGE;

static gboolean char_in_string (char c, const char * s)
{
  while (*s != '\0')
  if (*(s++) == c)
      return TRUE;
  return FALSE;
}

/**
 * gts_get_token:
 * @fptr: a file pointer.
 * @buf: a buffer in which to return the result.
 * @delimiters: a string of characters used to delimit the fields.
 * @comments: a string of characters defining the start of a comment.
 *
 * Reads characters from @fptr and fills @buf with a string delimited
 * by characters in @delimiters. Comments starts with a character in
 * @comments and ends with a newline character ('\n').
 *
 * Returns: the last character read from @fptr.
 */
gint gts_get_token (FILE * fptr, GString * buf, 
      const char * delimiters,
      const char * comments)
{
  gint c;
  gboolean in_string = FALSE;

  g_return_val_if_fail (fptr != NULL, EOF);
  g_return_val_if_fail (buf != NULL, EOF);
  g_return_val_if_fail (delimiters != NULL, EOF);
  g_return_val_if_fail (comments != NULL, EOF);

  buf->str[0] = '\0';
  buf->len = 0;
  c = fgetc (fptr);

  // TNO in below function calls "c" was changed into "(char)c" to prevent warning in compile time....
  while (c != EOF && (!in_string || !char_in_string ((char)c, delimiters))) {
  if (char_in_string ((char)c, comments))
      gts_get_newline (fptr);
  else if (in_string)
      g_string_append_c (buf, (char)c);
  else if (!char_in_string ((char)c, delimiters)) {
      in_string = TRUE;
      g_string_append_c (buf, (char)c);
  }      
  c = fgetc (fptr);
  }
  return c;
}

/**
 * gts_get_newline:
 * @fptr: a file pointer.
 *
 * Reads characters from @fptr until a '\n' character is met.
 */
void gts_get_newline (FILE * fptr)
{
  gint c;

  g_return_if_fail (fptr != NULL);
  
  c = fgetc (fptr);
  while (c != EOF && c != '\n')
  c = fgetc (fptr);
}

#ifdef DEBUG_FUNCTIONS
static GHashTable * ids = NULL;
static guint next_id = 1;

guint id (gpointer p)
{
  g_return_val_if_fail (p != NULL, 0);
  g_return_val_if_fail (ids != NULL, 0);
  g_assert (g_hash_table_lookup (ids, p));
  return GPOINTER_TO_UINT (g_hash_table_lookup (ids, p));
}

void id_insert (gpointer p)
{
  g_return_if_fail (p != NULL);
  if (ids == NULL) ids = g_hash_table_new (NULL, NULL);
  g_assert (g_hash_table_lookup (ids, p) == NULL);
  g_hash_table_insert (ids, p, GUINT_TO_POINTER (next_id++));
}

void id_remove (gpointer p)
{
  g_assert (g_hash_table_lookup (ids, p));  
  g_hash_table_remove (ids, p);
}

void gts_write_triangle (GtsTriangle * t, 
       GtsPoint * o,
       FILE * fptr)
{
  gdouble xo = o ? o->x : 0.0;
  gdouble yo = o ? o->y : 0.0;
  gdouble zo = o ? o->z : 0.0;

  g_return_if_fail (t != NULL && fptr != NULL);

  fprintf (fptr, "(hdefine geometry \"t%d\" { =\n", id (t));
  fprintf (fptr, "OFF 3 1 0\n"
     "%g %g %g\n%g %g %g\n%g %g %g\n3 0 1 2\n})\n"
     "(geometry \"t%d\" { : \"t%d\"})\n"
     "(normalization \"t%d\" none)\n",
     GTS_POINT (GTS_SEGMENT (t->e1)->v1)->x - xo, 
     GTS_POINT (GTS_SEGMENT (t->e1)->v1)->y - yo,
     GTS_POINT (GTS_SEGMENT (t->e1)->v1)->z - zo,
     GTS_POINT (GTS_SEGMENT (t->e1)->v2)->x - xo, 
     GTS_POINT (GTS_SEGMENT (t->e1)->v2)->y - yo, 
     GTS_POINT (GTS_SEGMENT (t->e1)->v2)->z - zo,
     GTS_POINT (gts_triangle_vertex (t))->x - xo,
     GTS_POINT (gts_triangle_vertex (t))->y - yo,
     GTS_POINT (gts_triangle_vertex (t))->z - zo,
     id (t), id (t), id (t));
}

void gts_write_segment (GtsSegment * s, 
      GtsPoint * o,
      FILE * fptr)
{
  gdouble xo = o ? o->x : 0.0;
  gdouble yo = o ? o->y : 0.0;
  gdouble zo = o ? o->z : 0.0;

  g_return_if_fail (s != NULL && fptr != NULL);

  fprintf (fptr, "(geometry \"s%d\" { =\n", id (s));
  fprintf (fptr, "VECT 1 2 0 2 0 %g %g %g %g %g %g })\n"
     "(normalization \"s%d\" none)\n",
     GTS_POINT (s->v1)->x - xo, 
     GTS_POINT (s->v1)->y - yo, 
     GTS_POINT (s->v1)->z - zo,
     GTS_POINT (s->v2)->x - xo, 
     GTS_POINT (s->v2)->y - yo, 
     GTS_POINT (s->v2)->z - zo,
     id (s));
}
#endif /* DEBUG_FUNCTIONS */
