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

/*#define DEBUG
  #define DEBUG_NAMED*/

#ifdef DEBUG
static void write_edges_vertices (GSList * edges, 
				  GSList * vertices,
				  FILE * fptr)
{
  GSList * i = vertices;
  guint v = 1;
  
  fprintf (fptr, "%u %u 0\n", 
	   g_slist_length (vertices), 
	   g_slist_length (edges));
  while (i) {
    GtsPoint * p = i->data;
    fprintf (fptr, "%.30g %.30g %.30g %p:%s\n",
	     p->x, p->y, p->z, p, GTS_NVERTEX (p)->name);
    GTS_OBJECT (p)->reserved = GUINT_TO_POINTER (v++);
    i = i->next;
  }
  i = edges;
  while (i) {
    GtsSegment * s = i->data;
    fprintf (fptr, "%u %u %p:%s\n", 
	     GPOINTER_TO_UINT (GTS_OBJECT (s->v1)->reserved),
	     GPOINTER_TO_UINT (GTS_OBJECT (s->v2)->reserved),
	     s, GTS_NEDGE (s)->name);
    i = i->next;
  }
  i = vertices;
  while (i) {
    GTS_OBJECT (i->data)->reserved = NULL;
    i = i->next;
  }
}
#endif

static void surface_inter_destroy (GtsObject * object)
{
  GtsSurfaceInter * si = GTS_SURFACE_INTER (object);

  gts_object_destroy (GTS_OBJECT (si->s1));
  gts_object_destroy (GTS_OBJECT (si->s2));
  g_slist_free (si->edges);

  (* GTS_OBJECT_CLASS (gts_surface_inter_class ())->parent_class->destroy)
    (object);
}

static void surface_inter_class_init (GtsObjectClass * klass)
{
  klass->destroy = surface_inter_destroy;
}

static void surface_inter_init (GtsSurfaceInter * si)
{
  si->s1 = si->s2 = NULL;
  si->edges = NULL;
}

/**
 * gts_surface_inter_class:
 *
 * Returns: the #GtsSurfaceInterClass.
 */
GtsSurfaceInterClass * gts_surface_inter_class (void)
{
  static GtsSurfaceInterClass * klass = NULL;

  if (klass == NULL) {
    GtsObjectClassInfo surface_inter_info = {
      "GtsSurfaceInter",
      sizeof (GtsSurfaceInter),
      sizeof (GtsSurfaceInterClass),
      (GtsObjectClassInitFunc) surface_inter_class_init,
      (GtsObjectInitFunc) surface_inter_init,
      (GtsArgSetFunc) NULL,
      (GtsArgGetFunc) NULL
    };
    klass = gts_object_class_new (gts_object_class (), &surface_inter_info);
  }

  return klass;
}

/* see gts_segment_triangle_intersection() for a description.
 * @cut is set to the edge of @t exactly cut by @s or %NULL otherwise.
 */
static GtsPoint * segment_triangle_intersection (GtsSegment * s,
						 GtsTriangle * t,
						 GtsPointClass * klass,
						 GtsEdge ** cut)
{
  GtsPoint * A, * B, * C, * D, * E;
  gdouble ABCE, ABCD, ADCE, ABDE, BCDE;
  GtsPoint * pi = NULL;
  GtsEdge * AB, * BC, * CA;

  g_return_val_if_fail (s != NULL, NULL);
  g_return_val_if_fail (t != NULL, NULL);
  g_return_val_if_fail (klass != NULL, NULL);
  g_return_val_if_fail (cut != NULL, NULL);

  gts_triangle_vertices_edges (t, NULL, 
			       (GtsVertex **) &A, 
			       (GtsVertex **) &B, 
			       (GtsVertex **) &C, 
			       &AB, &BC, &CA);
  D = GTS_POINT (s->v1);
  E = GTS_POINT (s->v2);
  *cut = NULL;

  ABCE = gts_point_orientation_3d (A, B, C, E);
  ABCD = gts_point_orientation_3d (A, B, C, D);
  if (ABCE < 0.0 || ABCD > 0.0) {
    GtsPoint * tmpp;
    gdouble tmp;
    tmpp = E; E = D; D = tmpp;
    tmp = ABCE; ABCE = ABCD; ABCD = tmp;
  }
  if (ABCE < 0.0 || ABCD > 0.0)
    return NULL;
  ADCE = gts_point_orientation_3d (A, D, C, E);
  if (ADCE < 0.)
    return NULL;
  ABDE = gts_point_orientation_3d (A, B, D, E);
  if (ABDE < 0.)
    return NULL;
  BCDE = gts_point_orientation_3d (B, C, D, E);
  if (BCDE < 0.)
    return NULL;
  if (ABCE == 0.0) {
    if (ABCD == 0.0)
      /* s is contained in the plane defined by t*/
      return NULL;
    pi = E;
  }
  else if (ABCD == 0.0)
    pi = D;
  if (ABDE == 0.) {
    if (ADCE == 0.) {
      if (pi && pi != A) {
	gts_vertex_replace (GTS_VERTEX (A), GTS_VERTEX (pi));
	gts_object_destroy (GTS_OBJECT (A));
	return pi;
      }
      return A;
    }
    if (BCDE == 0.) {
      if (pi && pi != B) {
	gts_vertex_replace (GTS_VERTEX (B), GTS_VERTEX (pi));
	gts_object_destroy (GTS_OBJECT (B));
	return pi;
      }
      return B;
    }
    *cut = AB;
  }
  else if (BCDE == 0.) {
    if (ADCE == 0.) {
      if (pi && pi != C) {
	gts_vertex_replace (GTS_VERTEX (C), GTS_VERTEX (pi));
	gts_object_destroy (GTS_OBJECT (C));
	return pi;
      }
      return C;
    }
    *cut = BC;
  }
  else if (ADCE == 0.)
    *cut = CA;
  if (pi == NULL) {
    gdouble c = ABCE/(ABCE - ABCD);
    pi = gts_point_new (klass,
			E->x + c*(D->x - E->x),
			E->y + c*(D->y - E->y),
			E->z + c*(D->z - E->z));
  }
  return pi;
}

#define prepend_unique(list, data) (g_slist_find (list, data) ?\
				    list :\
				    g_slist_prepend (list, data))

static GtsVertex * intersects (GtsEdge * e,
			       GtsTriangle * t,
			       GtsSurface * surface)
{
  GtsPoint * pi;
  GtsVertex * vi = NULL;
  GSList * vertices = GTS_OBJECT (e)->reserved, * i;
  GtsEdge * cut;
  GtsSegment * s;
  GtsVertex * v1, * v2, * v3;

  gts_triangle_vertices (t, &v1, &v2, &v3);
  i = vertices;
  while (i) {
    GtsVertex * v = i->data;
    gpointer cut = GTS_OBJECT (v)->reserved;
    if (cut == t || 
	cut == t->e1 || cut == t->e2 || cut == t->e3 ||
	v == v1 || v == v2 || v == v3)
      return v;
    i = i->next;
  }
  s = GTS_SEGMENT (e);
  /* Warning: wild cast! */
  pi = segment_triangle_intersection (s, t,
		  GTS_POINT_CLASS (surface->vertex_class), &cut);
  if (pi) {
    vi = GTS_VERTEX (pi);
    if (cut) {
      GtsObject * o = GTS_OBJECT (cut);

      i = o->reserved;
      while (i) {
	GtsVertex * v = i->data;
	if (v != vi && GTS_OBJECT (v)->reserved == e) {
	  gts_object_destroy (GTS_OBJECT (vi));
	  vi = v;
	}
	i = i->next;
      }
      GTS_OBJECT (vi)->reserved = cut;
      o->reserved = prepend_unique (o->reserved, vi);
#ifdef DEBUG_NAMED
      if (GTS_NVERTEX (vi)->name[0] == '\0')
	g_snprintf (GTS_NVERTEX (vi)->name, GTS_NAME_LENGTH, "%s|%s",
		    GTS_NEDGE (e)->name, GTS_NEDGE (cut)->name);
#endif
    }
    else {
      GTS_OBJECT (vi)->reserved = t;
#ifdef DEBUG_NAMED
      if (GTS_NVERTEX (vi)->name[0] == '\0')
	g_snprintf (GTS_NVERTEX (vi)->name, GTS_NAME_LENGTH, "%s|%s",
		    GTS_NEDGE (e)->name, GTS_NFACE (t)->name);
#endif
    }
    if (vi != s->v1 && vi != s->v2)
      GTS_OBJECT (s)->reserved = prepend_unique (vertices, vi);
  }
  return vi;
}

/* see figure misc/orientation.fig */
static gdouble intersection_orientation (GtsTriangle * t1, 
					 GtsEdge * e,
					 GtsTriangle * t2)
{
  GtsVertex * v1, * v2, * v3;
  GtsEdge * e2, * e3;
  GtsVertex * v4, * v5, * v6;
  gdouble o1;

  gts_triangle_vertices_edges (t1, e, &v1, &v2, &v3, &e, &e2, &e3);
  gts_triangle_vertices (t2, &v4, &v5, &v6);

  o1 = 
    gts_point_orientation_3d (GTS_POINT (v4), GTS_POINT (v5), GTS_POINT (v6),
			      GTS_POINT (v2));
  if (o1 != 0.)
    return o1;
  return
    - gts_point_orientation_3d (GTS_POINT (v4), GTS_POINT (v5), GTS_POINT (v6),
				GTS_POINT (v1));
}

#define UPDATE_ORIENTATION if (o > 0.) vi2 = v; else if (o < 0.) {\
                             vi2 = vi1; vi1 = v; } else coincident = TRUE;

static void intersect_edges (GtsBBox * bb1, GtsBBox * bb2,
			     GtsSurfaceInter * si)
{
  GtsSurface * surface = si->s1;
  GtsTriangle * t1, * t2;
  GtsVertex * v, * vi1 = NULL, * vi2 = NULL;
  gboolean coincident = FALSE;

  t1 = bb1->bounded;
  t2 = bb2->bounded;

  vi1 = intersects (t2->e1, t1, surface);
  v = intersects (t2->e2, t1, surface);
  if (!vi1)
    vi1 = v;
  else if (v && v != vi1) {
    gdouble o = intersection_orientation (t2, t2->e2, t1);
    UPDATE_ORIENTATION;
  }
  if (!vi2) {
    v = intersects (t2->e3, t1, surface);
    if (!vi1)
      vi1 = v;
    else if (v && v != vi1) {
      gdouble o = intersection_orientation (t2, t2->e3, t1);
      UPDATE_ORIENTATION;
    }
  }
  if (!vi2) {
    v = intersects (t1->e1, t2, surface);
    if (!vi1)
      vi1 = v;
    else if (v && v != vi1) {
      gdouble o = - intersection_orientation (t1, t1->e1, t2);
      UPDATE_ORIENTATION;
    }
  }
  if (!vi2) {
    v = intersects (t1->e2, t2, surface);
    if (!vi1)
      vi1 = v;
    else if (v && v != vi1) {
      gdouble o = - intersection_orientation (t1, t1->e2, t2);
      UPDATE_ORIENTATION;
    }
  }
  if (!vi2) {
    v = intersects (t1->e3, t2, surface);
    if (!vi1)
      vi1 = v;
    else if (v && v != vi1) {
      gdouble o = - intersection_orientation (t1, t1->e3, t2);
      UPDATE_ORIENTATION;
    }
  }

  if ((vi1 || vi2) && !coincident) {
    gts_surface_add_face (si->s1, GTS_FACE (t1));
    gts_surface_add_face (si->s2, GTS_FACE (t2));
  }
  if (vi1 && vi2 && !coincident) {
    GtsEdge * e;
    GtsSegment * s = gts_vertices_are_connected (vi1, vi2);

    if (!GTS_IS_EDGE (s)) {
      e = gts_edge_new (si->s1->edge_class, vi1, vi2);
#ifdef DEBUG_NAMED
      g_snprintf (GTS_NEDGE (e)->name, GTS_NAME_LENGTH, "%s:%s",
		  GTS_NVERTEX (vi1)->name, GTS_NVERTEX (vi2)->name);
#endif  
    }
    else {
      GtsEdge * duplicate;
      e = GTS_EDGE (s);
      while ((duplicate = gts_edge_is_duplicate (e))) {
	gts_edge_replace (duplicate, e);
	gts_object_destroy (GTS_OBJECT (duplicate));
      }
      if (s->v1 != vi1) {
	s->v1 = vi1;
	s->v2 = vi2;
      }
    }
    GTS_OBJECT (t1)->reserved = prepend_unique (GTS_OBJECT (t1)->reserved, e);
    GTS_OBJECT (t2)->reserved = prepend_unique (GTS_OBJECT (t2)->reserved, e);
    si->edges = prepend_unique (si->edges, e);
  }
}

static void segment_detach_endpoints (GtsSegment * s)
{
  s->v1->segments = g_slist_remove (s->v1->segments, s);
  s->v2->segments = g_slist_remove (s->v2->segments, s);
}

static void segment_attach_endpoints (GtsSegment * s)
{
  s->v1->segments = g_slist_prepend (s->v1->segments, s);
  s->v2->segments = g_slist_prepend (s->v2->segments, s);
  g_assert (!gts_segment_is_duplicate (s));
}

static void triangulate_face (GtsTriangle * t, GtsSurface * sn)
{
  GSList * edges, * vertices = NULL, * i;
  GtsSurface * s = gts_surface_new (gts_surface_class (), 
				    sn->face_class,
				    sn->edge_class,
				    sn->vertex_class);
  GtsTriangle * enclosing;
  GtsVertex * v1, * v2, * v3;
  GtsMatrix * project, * unproject;
  GtsVector * coords;
  guint nvertices, n;
#ifdef DEBUG
  static guint nt = 0;
  char name[80];
  FILE * fptr;

  fprintf (stderr, 
	   "=============== triangulation t.%d: %p:%d %p->%p->%p\n", 
	   nt, t, id (t), t->e1, t->e2, t->e3);
  sprintf (name, "t.%d", nt++);
  fptr = fopen (name, "wt");
#endif

  /* build constraints list */
  i = edges = GTS_OBJECT (t)->reserved;
  GTS_OBJECT (t)->reserved = NULL;

  if (GTS_OBJECT (t->e1)->reserved) {
    vertices = g_slist_concat (g_slist_copy (GTS_OBJECT (t->e1)->reserved), 
			       vertices);
    /* deconnecte t->e1 from its endpoints to avoid it being used
       during triangulation */
    segment_detach_endpoints (GTS_SEGMENT (t->e1));
  }
  else
    edges = prepend_unique (edges, t->e1);
  if (GTS_OBJECT (t->e2)->reserved) {
    vertices = g_slist_concat (g_slist_copy (GTS_OBJECT (t->e2)->reserved), 
			       vertices);
    segment_detach_endpoints (GTS_SEGMENT (t->e2));
  }
  else
    edges = prepend_unique (edges, t->e2);
  if (GTS_OBJECT (t->e3)->reserved) {
    vertices = g_slist_concat (g_slist_copy (GTS_OBJECT (t->e3)->reserved), 
			       vertices);
    segment_detach_endpoints (GTS_SEGMENT (t->e3));
  }
  else
    edges = prepend_unique (edges, t->e3);

  /* add triangle summits */
  vertices = g_slist_prepend (vertices, GTS_SEGMENT (t->e1)->v1);
  vertices = g_slist_prepend (vertices, GTS_SEGMENT (t->e1)->v2);
  vertices = g_slist_prepend (vertices, gts_triangle_vertex (t));

  /* add edges endpoints */
  i = edges;
  while (i) {
    GtsSegment * s = i->data;
    vertices = prepend_unique (vertices, s->v1);
    vertices = prepend_unique (vertices, s->v2);
    i = i->next;
  }

  /* if there is only three vertices, they are the summits of @t */
  nvertices = g_slist_length (vertices);
  if (nvertices == 3) {
    g_slist_free (vertices);
    g_slist_free (edges);
    gts_surface_add_face (sn, GTS_FACE (t));
    return;
  }

  /* reset reserved field of vertices */
  g_slist_foreach (vertices, (GFunc) gts_object_reset_reserved, NULL);

  /* project vertices on t */
  unproject = gts_matrix_projection (t);
  project = gts_matrix_transpose (unproject);
  gts_matrix_destroy (unproject);
  coords = g_malloc (sizeof (GtsVector)*nvertices);
  n = 0;
  i = vertices;
  while (i) {
    GtsPoint * p = i->data;
    coords[n][0] = p->x;
    coords[n][1] = p->y;
    coords[n++][2] = p->z;
    gts_point_transform (p, project);
    i = i->next;
  }
  gts_matrix_destroy (project);
  
  /* build enclosing triangle */
  enclosing = gts_triangle_enclosing (gts_triangle_class (), vertices, 10.);
  gts_triangle_vertices (enclosing, &v1, &v2, &v3);
  gts_surface_add_face (s, gts_face_new (gts_face_class (),
					 enclosing->e1,
					 enclosing->e2,
					 enclosing->e3));
#ifdef DEBUG
  write_edges_vertices (edges, vertices, fptr);
  fclose (fptr);
#endif

  /* set edges as constraints, saving there initial class in reserved */
  i = edges;
  while (i) {
    GtsObject * e = i->data;
#ifdef DEBUG
    fprintf (stderr, "constraint: %p->%d\n", e, id (e));
#endif
    g_assert (!e->reserved);
    e->reserved = e->klass;
    e->klass = GTS_OBJECT_CLASS (gts_constraint_class ());
    i = i->next;
  }

  /* add vertices */
  i = vertices;
  while (i) {
#ifdef DEBUG
    fprintf (stderr, "adding %p->%d: (%.20g,%.20g,%.20g)\n",
	     i->data, id (i->data),
	     GTS_POINT (i->data)->x,
	     GTS_POINT (i->data)->y,
	     GTS_POINT (i->data)->z);
#endif
    g_assert (gts_delaunay_add_vertex (s, i->data, NULL) == NULL);
    i = i->next;
  }

  /* add remaining constraints */
  i = edges;
  while (i) {
#ifdef DEBUG
    GSList * inter;
    fprintf (stderr, "adding constraint %p->%d: %p:%d->%p:%d\n",
	     i->data, id (i->data),
	     GTS_SEGMENT (i->data)->v1, id (GTS_SEGMENT (i->data)->v1),
	     GTS_SEGMENT (i->data)->v2, id (GTS_SEGMENT (i->data)->v2));
    inter = gts_delaunay_add_constraint (s, i->data);
    if (inter != NULL) {
      fprintf (stderr, "  intersects with:\n");
      i = inter;
      while (i) {
	GtsSegment * s = i->data;
	fprintf (stderr, "  %p->%d: %p:%d->%p:%d\n", 
		 s, id (s), s->v1, id (s->v1), s->v2, id (s->v2));
	i = i->next;
      }
      g_assert_not_reached ();
    }
#else
    g_assert (gts_delaunay_add_constraint (s, i->data) == NULL);
#endif
    i = i->next;
  }
  
  /* undo projection */
  i = vertices;
  n = 0;
  while (i) {
    GtsPoint * p = i->data;
    p->x = coords[n][0];
    p->y = coords[n][1];
    p->z = coords[n++][2];
    i = i->next;
  }
  g_free (coords);
  g_slist_free (vertices);

  /* restore edges classes */
  i = edges;
  while (i) {
    GtsObject * e = i->data;
    g_assert (e->reserved);
    e->klass = e->reserved;
    e->reserved = NULL;
    i = i->next;
  }
  g_slist_free (edges);

  /* destroy enclosing triangle */
  gts_allow_floating_vertices = TRUE;
  gts_object_destroy (GTS_OBJECT (v1));
  gts_object_destroy (GTS_OBJECT (v2));
  gts_object_destroy (GTS_OBJECT (v3));
  gts_allow_floating_vertices = FALSE;

  /* add new faces to si */
  gts_surface_merge (sn, s);
  gts_object_destroy (GTS_OBJECT (s));

  /* restore edges of t */
  if (GTS_OBJECT (t->e1)->reserved)
    segment_attach_endpoints (GTS_SEGMENT (t->e1));
  if (GTS_OBJECT (t->e2)->reserved)
    segment_attach_endpoints (GTS_SEGMENT (t->e2));
  if (GTS_OBJECT (t->e3)->reserved)
    segment_attach_endpoints (GTS_SEGMENT (t->e3));
}

static void free_vertex_list (GtsObject * o)
{
  g_slist_free (o->reserved);
  o->reserved = NULL;
}

/**
 * gts_surface_inter_new:
 * @klass: a #GtsSurfaceInterClass.
 * @s1: a #GtsSurface.
 * @s2: a #GtsSurface.
 * @faces_tree1: a bounding box tree (see gts_bb_tree_new()) for
 * the faces of @s1.
 * @faces_tree2: a bounding box tree for the faces of @s2.
 *
 * Returns: a new #GtsSurfaceInter describing the intersection of @s1
 * and @s2.  
 */
GtsSurfaceInter* gts_surface_inter_new (GtsSurfaceInterClass * klass,
					GtsSurface * s1,
					GtsSurface * s2,
					GNode * faces_tree1,
					GNode * faces_tree2)
{
  GtsSurfaceInter * si;
  GtsSurface * s;

  g_return_val_if_fail (klass != NULL, NULL);
  g_return_val_if_fail (s1 != NULL, NULL);
  g_return_val_if_fail (s2 != NULL, NULL);
  g_return_val_if_fail (faces_tree1 != NULL, NULL);
  g_return_val_if_fail (faces_tree2 != NULL, NULL);

  si = GTS_SURFACE_INTER (gts_object_new (GTS_OBJECT_CLASS (klass)));
  si->s1 = gts_surface_new (gts_surface_class (),
			    s1->face_class,
			    s1->edge_class,
			    s1->vertex_class);
  si->s2 = gts_surface_new (gts_surface_class (),
			    s2->face_class,
			    s2->edge_class,
			    s2->vertex_class);
  gts_bb_tree_traverse_overlapping (faces_tree1, faces_tree2,
				    (GtsBBTreeTraverseFunc) intersect_edges, 
				    si);

  s = gts_surface_new (gts_surface_class (),
		       s1->face_class,
		       s1->edge_class,
		       s1->vertex_class);
  gts_surface_foreach_face (si->s1, (GtsFunc) triangulate_face, s);
  gts_surface_foreach_edge (si->s1, (GtsFunc) free_vertex_list, NULL);
  gts_object_destroy (GTS_OBJECT (si->s1));
  si->s1 = s;
  GTS_OBJECT (si->s1)->reserved = s1;

  s = gts_surface_new (gts_surface_class (),
		       s2->face_class,
		       s2->edge_class,
		       s2->vertex_class);
  gts_surface_foreach_face (si->s2, (GtsFunc) triangulate_face, s);
  gts_surface_foreach_edge (si->s2, (GtsFunc) free_vertex_list, NULL);
  gts_object_destroy (GTS_OBJECT (si->s2));
  si->s2 = s;
  GTS_OBJECT (si->s2)->reserved = s2;

  si->s1->keep_faces = si->s2->keep_faces = TRUE;

  return si;
}

/**
 * gts_surface_inter_check:
 * @si: a #GtsSurfaceInter.
 *
 * Returns: %TRUE if the curve described by @si is an orientable
 * manifold, %FALSE otherwise.
 */
gboolean gts_surface_inter_check (GtsSurfaceInter * si)
{
  GSList * i; gboolean ok = TRUE;

  g_return_val_if_fail (si != NULL, FALSE);

  /* mark edges as used by si */
  i = si->edges;
  while (i) {
    GTS_OBJECT (i->data)->reserved = si;
    i = i->next;
  }

  i = si->edges;
  while (i && ok) {
    GtsSegment * s = i->data;
    GSList * j = s->v1->segments;
    while (j && ok) {
      GtsSegment * s1 = j->data;
      if (s1 != s && GTS_OBJECT (s1)->reserved == si && s1->v2 != s->v1)
	ok = FALSE;
      j = j->next;
    }
    i = i->next;
  }

  g_slist_foreach (si->edges, (GFunc) gts_object_reset_reserved, NULL);

  return ok;
}

/* Given @e and @f returns a #GtsFace compatible with @f and belonging to
   @s1 or @s2 */
static GtsFace * next_compatible_face (GtsEdge * e, 
				       GtsFace * f, 
				       GtsSurface * s1,
				       GtsSurface * s2,
				       GtsSurface * ssss)
{
  GSList * i = e->triangles;
  GtsFace * f2 = NULL, * f3 = NULL;

  UNUSED(ssss);

  while (i) {
    GtsFace * f1 = i->data;
    if (f1 != f && GTS_IS_FACE (f1)) {
      if (gts_face_has_parent_surface (f1, s1))
	return f1;
      if (gts_face_has_parent_surface (f1, s2)) {
	if (f2 == NULL) f2 = f1;
	else if (f3 == NULL) f3 = f1;
	else g_assert_not_reached (); /* s2 is a non-manifold surface */
      }
    }
    i = i->next;
  }
  if (f3 == NULL) {
    if (gts_edge_is_boundary (e, s2))
      return NULL;
    return f2; 
  }
  if (gts_triangles_are_compatible (GTS_TRIANGLE (f), GTS_TRIANGLE (f2), e))
    return f2;
  return f3;
}

static void walk_faces (GtsEdge * e, GtsFace * f, 
			GtsSurface * s1,
			GtsSurface * s2,
			GtsSurface * s)
{
  GtsTriangle * t;

  if (f == NULL || GTS_OBJECT (f)->reserved)
    return;

  gts_surface_add_face (s, f);
  GTS_OBJECT (f)->reserved = s;
  t = GTS_TRIANGLE (f);
  if (t->e1 != e && !GTS_OBJECT (t->e1)->reserved)
    walk_faces (t->e1, next_compatible_face (t->e1, f, s1, s2, s), s1, s2, s);
  if (t->e2 != e && !GTS_OBJECT (t->e2)->reserved)
    walk_faces (t->e2, next_compatible_face (t->e2, f, s1, s2, s), s1, s2, s);
  if (t->e3 != e && !GTS_OBJECT (t->e3)->reserved)
    walk_faces (t->e3, next_compatible_face (t->e3, f, s1, s2, s), s1, s2, s);
}

static gint triangle_orientation (GtsTriangle * t, GtsEdge * e)
{
  GtsSegment * s = GTS_SEGMENT (t->e1 == e ? t->e2 
				: 
				t->e2 == e ? t->e3 
				: 
				t->e1);
  GtsVertex * v2 = GTS_SEGMENT (e)->v2;

  if (s->v1 == v2 || s->v2 == v2)
    return 1;
  return -1;
}

/**
 * gts_surface_inter_boolean:
 * @si: a #GtsSurfaceInter.
 * @surface: a #GtsSurface.
 * @op: a #GtsBooleanOperation.
 *
 * Adds to @surface the part of the surface described by @si and @op.
 */
void gts_surface_inter_boolean (GtsSurfaceInter * si,
				GtsSurface * surface,
				GtsBooleanOperation op)
{
  GSList * i;
  GtsSurface * s = NULL;
  gint orient = 1;

  g_return_if_fail (si != NULL);
  g_return_if_fail (surface != NULL);

  switch (op) {
  case GTS_1_OUT_2: s = si->s1; orient = 1; break;
  case GTS_1_IN_2: s = si->s1; orient = -1; break;
  case GTS_2_OUT_1: s = si->s2; orient = -1; break;
  case GTS_2_IN_1: s = si->s2; orient = 1; break;
  default: g_assert_not_reached ();
  }

  /* mark edges as belonging to intersection */
  i = si->edges;
  while (i) {
    GTS_OBJECT (i->data)->reserved = si;
    i = i->next;
  }

  i = si->edges;
  while (i) {
    GtsEdge * e = i->data;
    GSList * j = e->triangles;

    while (j) {
      if (gts_face_has_parent_surface (j->data, s) &&
	  orient*triangle_orientation (j->data, e) > 0) {
	walk_faces (e, j->data, s, GTS_OBJECT (s)->reserved, surface);
	break;
      }
      j = j->next;
    }
    i = i->next;
  }

  g_slist_foreach (si->edges, (GFunc) gts_object_reset_reserved, NULL);
  gts_surface_foreach_face (surface, 
			    (GtsFunc) gts_object_reset_reserved, NULL);
}
