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


#define HEAP_INSERT_TRIANGLE(h, t) (GTS_OBJECT (t)->reserved =\
                  gts_eheap_insert (h, t))

static gdouble triangle_neighbors (GtsTriangle * t, GtsSurface * s)
{
  return (gdouble)gts_face_neighbor_number (GTS_FACE (t), s);
}

static void create_triangle_heap (GtsTriangle * t, GtsEHeap * heap)
{
  HEAP_INSERT_TRIANGLE (heap, t);
}

static GSList * triangle_neighbors_data (GtsTriangle * t)
{
  GSList * i, * list = NULL;
  GtsEdge * ee[4], ** e1 = ee;

  g_return_val_if_fail (t != NULL, NULL);

  ee[0] = t->e1; ee[1] = t->e2; ee[2] = t->e3; ee[3] = NULL;
  while (*e1) {
  i = (*e1++)->triangles;
  while (i) {
      GtsTriangle * t1 = i->data;
      if (t1 != t && GTS_OBJECT (t1)->reserved)
  list = g_slist_prepend (list, t1);
      i = i->next;
  }
  }
  return list;  
}

static GtsTriangle * most_isolated_neighbor (GtsTriangle * t,
               GtsEHeap * heap)
{
  GSList * neighbors = triangle_neighbors_data (t);
  GSList * same_key = NULL; /* neighbors of t with the same key */
  GSList * i;
  GtsTriangle * tmost = NULL;
  gdouble keymin = G_MAXDOUBLE;
  
  i = neighbors;
  while (i) {
  GtsTriangle * t1 = i->data;
  GtsEHeapPair * p = GTS_OBJECT (t1)->reserved;
  gdouble key = p->key;
  /* find keymin */
  if (key < keymin) {
      keymin = key;
      g_slist_free (same_key);
      same_key = g_slist_prepend (NULL, t1);
  }
  else if (key == keymin)
      same_key = g_slist_prepend (same_key, t1);
  /* decrease key */
  g_assert (key > 0.);
  gts_eheap_decrease_key (heap, p, key - 1.);
  i = i->next;
  }
  g_slist_free (neighbors);
  
  if (same_key == NULL) /* no neighbors */
  return NULL;
  if (same_key->next == NULL) { /* only one neighbor with minimum key */
  tmost = same_key->data;
  g_slist_free (same_key);
  return tmost;
  }

  /* several neighbors have the same minimum key */
  tmost = same_key->data;
  keymin = G_MAXDOUBLE;
  i = same_key;
  while (i) {
  GtsTriangle * t1 = i->data;
  GSList * j;
  j = neighbors = triangle_neighbors_data (t1);
  while (j) {
      GtsTriangle * t2 = j->data;
      gdouble key = ((GtsEHeapPair *) GTS_OBJECT (t2)->reserved)->key;
      if (key < keymin) {
  keymin = key;
  tmost = t1;
      }
      j = j->next;
  }
  g_slist_free (neighbors);
  i = i->next;
  }
  g_slist_free (same_key);
  
  return tmost;
}

/**
 * gts_surface_strip:
 * @s: a #GtsSurface.
 *
 * Decompose @s into triangle strips for fast-rendering.
 *
 * Returns: a list of triangle strips containing all the triangles of @s. 
 * A triangle strip is itself a list of successive triangles having one edge
 * in common.
 */
GSList * gts_surface_strip (GtsSurface * s)
{
  GtsEHeap * heap;
  GtsTriangle * t;
  GSList * strips = NULL;

  g_return_val_if_fail (s != NULL, NULL);

  heap = gts_eheap_new ((GtsKeyFunc) triangle_neighbors, s);
  gts_eheap_freeze (heap);
  gts_surface_foreach_face (s, (GtsFunc) create_triangle_heap, heap);
  gts_eheap_thaw (heap);

  while ((t = gts_eheap_remove_top (heap, NULL))) {
  GSList * newstrip;
  GtsTriangle * tstart, * tmost;
  GTS_OBJECT (t)->reserved = NULL;
  /* start new strip from t */
  newstrip = g_slist_prepend (NULL, t);
  tstart = t;
  while ((tmost = most_isolated_neighbor (tstart, heap))) {
      newstrip = g_slist_prepend (newstrip, tmost);
      gts_eheap_remove (heap, GTS_OBJECT (tmost)->reserved);
      GTS_OBJECT (tmost)->reserved = NULL;
      tstart = tmost;
  }
  /* restart new strip from t (i.e. both ends) */
  newstrip = g_slist_reverse (newstrip);
  tstart = t;
  while ((tmost = most_isolated_neighbor (tstart, heap))) {
      newstrip = g_slist_prepend (newstrip, tmost);
      gts_eheap_remove (heap, GTS_OBJECT (tmost)->reserved);
      GTS_OBJECT (tmost)->reserved = NULL;
      tstart = tmost;
  }
  strips = g_slist_prepend (strips, newstrip);
  }
  
  return strips;
}
