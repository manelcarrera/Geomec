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
#include <math.h>
#include <stdlib.h>

static void destroy_foreach_face(GtsFace *f, GtsSurface *s) {
  f->surfaces = g_slist_remove(f->surfaces, s);
  if (!GTS_OBJECT_DESTROYED(f) && !gts_allow_floating_faces && f->surfaces == NULL)
    gts_object_destroy(GTS_OBJECT(f));
}

static void surface_destroy(GtsObject *object) {
  GtsSurface *surface = GTS_SURFACE(object);

  gts_surface_foreach_face(surface, (GtsFunc)destroy_foreach_face, surface);
  g_hash_table_destroy(surface->faces);

  (*GTS_OBJECT_CLASS(gts_surface_class())->parent_class->destroy)(object);
}

static void surface_write(GtsObject *object, FILE *fptr) {
  fprintf(fptr, " %s %s %s %s", object->klass->info.name, GTS_OBJECT_CLASS(GTS_SURFACE(object)->face_class)->info.name,
          GTS_OBJECT_CLASS(GTS_SURFACE(object)->edge_class)->info.name,
          GTS_OBJECT_CLASS(GTS_SURFACE(object)->vertex_class)->info.name);
}

static void surface_class_init(GtsSurfaceClass *klass) {
  GTS_OBJECT_CLASS(klass)->destroy = surface_destroy;
  GTS_OBJECT_CLASS(klass)->write = surface_write;
  klass->add_face = NULL;
  klass->remove_face = NULL;
}

static void surface_init(GtsSurface *surface) {
  surface->faces = g_hash_table_new(NULL, NULL);
  surface->vertex_class = gts_vertex_class();
  surface->edge_class = gts_edge_class();
  surface->face_class = gts_face_class();
  surface->keep_faces = FALSE;
}

/**
 * gts_surface_class:
 *
 * Returns: the #GtsSurfaceClass.
 */
GtsSurfaceClass *gts_surface_class(void) {
  static GtsSurfaceClass *klass = NULL;

  if (klass == NULL) {
    GtsObjectClassInfo surface_info = {"GtsSurface",
                                       sizeof(GtsSurface),
                                       sizeof(GtsSurfaceClass),
                                       (GtsObjectClassInitFunc)surface_class_init,
                                       (GtsObjectInitFunc)surface_init,
                                       (GtsArgSetFunc)NULL,
                                       (GtsArgGetFunc)NULL};
    klass = gts_object_class_new(gts_object_class(), &surface_info);
  }

  return klass;
}

/**
 * gts_surface_new:
 * @klass: a #GtsSurfaceClass.
 * @face_class: a #GtsFaceClass.
 * @edge_class: a #GtsEdgeClass.
 * @vertex_class: a #GtsVertexClass.
 *
 * Returns: a new empty #GtsSurface.
 */
GtsSurface *gts_surface_new(GtsSurfaceClass *klass, GtsFaceClass *face_class, GtsEdgeClass *edge_class,
                            GtsVertexClass *vertex_class) {
  GtsSurface *s;

  s = GTS_SURFACE(gts_object_new(GTS_OBJECT_CLASS(klass)));
  s->vertex_class = vertex_class;
  s->edge_class = edge_class;
  s->face_class = face_class;

  return s;
}

/**
 * gts_surface_add_face:
 * @s: a #GtsSurface.
 * @f: a #GtsFace.
 *
 * Adds face @f to surface @s.
 */
void gts_surface_add_face(GtsSurface *s, GtsFace *f) {
  g_return_if_fail(s != NULL);
  g_return_if_fail(f != NULL);

  g_assert(s->keep_faces == FALSE);

  if (!g_hash_table_lookup(s->faces, f)) {
    f->surfaces = g_slist_prepend(f->surfaces, s);
    g_hash_table_insert(s->faces, f, f);
  }

  if (GTS_SURFACE_CLASS(GTS_OBJECT(s)->klass)->add_face)
    (*GTS_SURFACE_CLASS(GTS_OBJECT(s)->klass)->add_face)(s, f);
}

/**
 * gts_surface_remove_face:
 * @s: a #GtsSurface.
 * @f: a #GtsFace.
 *
 * Removes face @f from surface @s.
 */
void gts_surface_remove_face(GtsSurface *s, GtsFace *f) {
  g_return_if_fail(s != NULL);
  g_return_if_fail(f != NULL);

  g_assert(s->keep_faces == FALSE);

  g_hash_table_remove(s->faces, f);
  f->surfaces = g_slist_remove(f->surfaces, s);

  if (GTS_SURFACE_CLASS(GTS_OBJECT(s)->klass)->remove_face)
    (*GTS_SURFACE_CLASS(GTS_OBJECT(s)->klass)->remove_face)(s, f);

  if (!GTS_OBJECT_DESTROYED(f) && !gts_allow_floating_faces && f->surfaces == NULL)
    gts_object_destroy(GTS_OBJECT(f));
}

/**
 * gts_surface_read:
 * @surface: a #GtsSurface.
 * @fptr: a file pointer.
 *
 * Add to @surface the data read from @fptr. The format of the file pointed to
 * by @fptr is as described in gts_surface_write().
 *
 * Returns: 0 if successful or the line number at which the parsing stopped in
 * case of error.
 */
guint gts_surface_read(GtsSurface *surface, FILE *fptr) {
  GtsVertex **vertices;
  GtsEdge **edges;
  guint n, nv, ne, nf, line_number = 1;
  gboolean syntax_error = FALSE;
  GString *buf;
  gint delim;

  g_return_val_if_fail(surface != NULL, 1);
  g_return_val_if_fail(fptr != NULL, 1);

  buf = g_string_new("");

  delim = gts_get_token(fptr, buf, " \t\n", GTS_COMMENTS);
  if (delim == EOF || delim == '\n') {
    g_string_free(buf, TRUE);
    return 1;
  }
  nv = atoi(buf->str);
  delim = gts_get_token(fptr, buf, " \t\n", GTS_COMMENTS);
  if (delim == EOF || delim == '\n') {
    g_string_free(buf, TRUE);
    return 1;
  }
  ne = atoi(buf->str);
  delim = gts_get_token(fptr, buf, " \t\n", GTS_COMMENTS);
  if (delim == EOF) {
    g_string_free(buf, TRUE);
    return 1;
  }
  nf = atoi(buf->str);
  if (delim != '\n')
    gts_get_newline(fptr);

  if (nf <= 0) {
    g_string_free(buf, TRUE);
    return 0;
  }

  /* allocate nv + 1 just in case nv == 0 */
  vertices = g_malloc((nv + 1) * sizeof(GtsVertex *));
  edges = g_malloc((ne + 1) * sizeof(GtsEdge *));

  n = 0;
  while (n < nv && !syntax_error) {
    GtsObject *new_vertex = gts_object_new(GTS_OBJECT_CLASS(surface->vertex_class));

    line_number++;
    delim = (*GTS_OBJECT_CLASS(surface->vertex_class)->read)(&new_vertex, fptr);
    if (delim == EOF)
      syntax_error = TRUE;
    else if (delim != '\n')
      gts_get_newline(fptr);
    vertices[n++] = GTS_VERTEX(new_vertex);
  }
  if (syntax_error)
    nv = n;

  n = 0;
  while (n < ne && !syntax_error) {
    guint p1, p2;

    line_number++;
    delim = gts_get_token(fptr, buf, " \t\n", GTS_COMMENTS);
    if (delim == EOF || delim == '\n')
      syntax_error = TRUE;
    else {
      p1 = atoi(buf->str);
      delim = gts_get_token(fptr, buf, " \t\n", GTS_COMMENTS);
      if (delim == EOF)
        syntax_error = TRUE;
      else {
        p2 = atoi(buf->str);
        if (p1 == 0 || p2 == 0 || p1 > nv || p2 > nv)
          syntax_error = TRUE;
        else {
          GtsEdge *new_edge = gts_edge_new(surface->edge_class, vertices[p1 - 1], vertices[p2 - 1]);
          if (delim != '\n') {
            if (GTS_OBJECT_CLASS(surface->edge_class)->read)
              delim = (*GTS_OBJECT_CLASS(surface->edge_class)->read)((GtsObject **)&new_edge, fptr);
            if (delim != '\n')
              gts_get_newline(fptr);
          }
          edges[n++] = new_edge;
        }
      }
    }
  }
  if (syntax_error)
    ne = n;

  n = 0;
  while (n < nf && !syntax_error) {
    guint s1, s2, s3;

    line_number++;
    delim = gts_get_token(fptr, buf, " \t\n", GTS_COMMENTS);
    if (delim == EOF || delim == '\n')
      syntax_error = TRUE;
    else {
      s1 = atoi(buf->str);
      delim = gts_get_token(fptr, buf, " \t\n", GTS_COMMENTS);
      if (delim == EOF || delim == '\n')
        syntax_error = TRUE;
      else {
        s2 = atoi(buf->str);
        delim = gts_get_token(fptr, buf, " \t\n", GTS_COMMENTS);
        if (delim == EOF)
          syntax_error = TRUE;
        else {
          s3 = atoi(buf->str);
          if (s1 == 0 || s2 == 0 || s3 == 0 || s1 > ne || s2 > ne || s3 > ne)
            syntax_error = TRUE;
          else {
            GtsFace *new_face = gts_face_new(surface->face_class, edges[s1 - 1], edges[s2 - 1], edges[s3 - 1]);
            if (delim != '\n') {
              if (GTS_OBJECT_CLASS(surface->face_class)->read)
                delim = (*GTS_OBJECT_CLASS(surface->face_class)->read)((GtsObject **)&new_face, fptr);
              if (delim != '\n')
                gts_get_newline(fptr);
            }
            gts_surface_add_face(surface, new_face);
            n++;
          }
        }
      }
    }
  }

  if (syntax_error) {
    gts_allow_floating_vertices = TRUE;
    while (nv)
      gts_object_destroy(GTS_OBJECT(vertices[nv-- - 1]));
    gts_allow_floating_vertices = FALSE;
    gts_object_destroy(GTS_OBJECT(surface));
  }

  g_free(vertices);
  g_free(edges);
  g_string_free(buf, TRUE);

  if (syntax_error)
    return line_number;
  return 0;
}

static void sum_area(GtsFace *f, gdouble *area) { *area += gts_triangle_area(GTS_TRIANGLE(f)); }

/**
 * gts_surface_area:
 * @s: a #GtsSurface.
 *
 * Returns: the area of @s obtained as the sum of the signed areas of its
 * faces.
 */
gdouble gts_surface_area(GtsSurface *s) {
  gdouble area = 0.0;
  gts_surface_foreach_face(s, (GtsFunc)sum_area, &area);
  return area;
}

/**
 * gts_range_init:
 * @r: a #GtsRange.
 *
 * Initializes a #GtsRange.
 */
void gts_range_init(GtsRange *r) {
  g_return_if_fail(r != NULL);

  r->max = -G_MAXDOUBLE;
  r->min = G_MAXDOUBLE;
  r->mean = r->stddev = 0.0;
  r->n = 0;
}

/**
 * gts_range_reset:
 * @r: a #GtsRange.
 *
 * Sets all the fields of @r to 0.
 */
void gts_range_reset(GtsRange *r) {
  g_return_if_fail(r != NULL);

  r->max = 0.0;
  r->min = 0.0;
  r->mean = r->stddev = 0.0;
  r->n = 0;
}

/**
 * gts_range_add_value:
 * @r: a #GtsRange.
 * @val: a value to add to @r.
 *
 * Adds @val to @r.
 */
void gts_range_add_value(GtsRange *r, gdouble val) {
  g_return_if_fail(r != NULL);

  if (val < r->min)
    r->min = val;
  if (val > r->max)
    r->max = val;
  r->mean += val;
  r->stddev += val * val;
  r->n++;
}

/**
 * gts_range_update:
 * @r: a #GtsRange.
 *
 * Updates the fields of @r.
 */
void gts_range_update(GtsRange *r) {
  g_return_if_fail(r != NULL);
  if (r->n > 0) {
    if (r->stddev - r->mean * r->mean / (gdouble)r->n >= 0.)
      r->stddev = sqrt((r->stddev - r->mean * r->mean / (gdouble)r->n) / (gdouble)r->n);
    else
      r->stddev = 0.;
    r->mean /= (gdouble)r->n;
  } else
    r->min = r->max = 0.0;
}

/**
 * gts_range_print:
 * @r: a #GtsRange.
 * @fptr: a file pointer.
 *
 * Writes a text representation of @r in @fptr.
 */
void gts_range_print(GtsRange *r, FILE *fptr) {
  g_return_if_fail(r != NULL);
  g_return_if_fail(fptr != NULL);
  fprintf(fptr, "min: %g mean: %g | %g max: %g", r->min, r->mean, r->stddev, r->max);
}

static void stats_foreach_vertex(GtsVertex *v, GtsSurfaceStats *stats) {
  GSList *i = v->segments;
  guint nedges = 0;

  while (i) {
    if (GTS_IS_EDGE(i->data) && gts_edge_has_parent_surface(i->data, stats->parent))
      nedges++;
    i = i->next;
  }
  gts_range_add_value(&stats->edges_per_vertex, (gdouble)nedges);
}

static void stats_foreach_edge(GtsEdge *e, GtsSurfaceStats *stats) {
  guint nt = gts_edge_face_number(e, stats->parent);

  if (gts_segment_is_duplicate(GTS_SEGMENT(e)))
    stats->n_duplicate_edges++;
  if (nt == 1)
    stats->n_boundary_edges++;
  else if (nt > 2)
    stats->n_non_manifold_edges++;
  gts_range_add_value(&stats->faces_per_edge, (gdouble)nt);
}

static void stats_foreach_face(GtsTriangle *t, GtsSurfaceStats *stats) {
  GSList *i;
  gboolean incompatible = FALSE;

  i = t->e1->triangles;
  while (i && !incompatible) {
    if (i->data != t && GTS_IS_FACE(i->data) && gts_face_has_parent_surface(i->data, stats->parent) &&
        !gts_triangles_are_compatible(t, i->data, t->e1))
      incompatible = TRUE;
    i = i->next;
  }
  i = t->e2->triangles;
  while (i && !incompatible) {
    if (i->data != t && GTS_IS_FACE(i->data) && gts_face_has_parent_surface(i->data, stats->parent) &&
        !gts_triangles_are_compatible(t, i->data, t->e2))
      incompatible = TRUE;
    i = i->next;
  }
  i = t->e3->triangles;
  while (i && !incompatible) {
    if (i->data != t && GTS_IS_FACE(i->data) && gts_face_has_parent_surface(i->data, stats->parent) &&
        !gts_triangles_are_compatible(t, i->data, t->e3))
      incompatible = TRUE;
    i = i->next;
  }
  if (incompatible)
    stats->n_incompatible_faces++;
  if (gts_triangle_is_duplicate(t))
    stats->n_duplicate_faces++;
  stats->n_faces++;
}

/**
 * gts_surface_stats:
 * @s: a #GtsSurface.
 * @stats: a #GtsSurfaceStats.
 *
 * Fills @stats with the statistics relevant to surface @s.
 */
void gts_surface_stats(GtsSurface *s, GtsSurfaceStats *stats) {
  g_return_if_fail(s != NULL);
  g_return_if_fail(stats != NULL);

  stats->parent = s;
  stats->n_faces = 0;
  stats->n_incompatible_faces = 0;
  stats->n_duplicate_faces = 0;
  stats->n_duplicate_edges = 0;
  stats->n_boundary_edges = 0;
  stats->n_non_manifold_edges = 0;
  gts_range_init(&stats->edges_per_vertex);
  gts_range_init(&stats->faces_per_edge);

  gts_surface_foreach_vertex(s, (GtsFunc)stats_foreach_vertex, stats);
  gts_surface_foreach_edge(s, (GtsFunc)stats_foreach_edge, stats);
  gts_surface_foreach_face(s, (GtsFunc)stats_foreach_face, stats);

  gts_range_update(&stats->edges_per_vertex);
  gts_range_update(&stats->faces_per_edge);
}

static void quality_foreach_edge(GtsSegment *s, GtsSurfaceQualityStats *stats) {
  GSList *i = GTS_EDGE(s)->triangles;

  gts_range_add_value(&stats->edge_length, gts_point_distance(GTS_POINT(s->v1), GTS_POINT(s->v2)));
  while (i) {
    GSList *j = i->next;
    while (j) {
      gts_range_add_value(&stats->edge_angle, fabs(gts_triangles_angle(i->data, j->data)));
      j = j->next;
    }
    i = i->next;
  }
}

static void quality_foreach_face(GtsTriangle *t, GtsSurfaceQualityStats *stats) {
  gts_range_add_value(&stats->face_quality, gts_triangle_quality(t));
  gts_range_add_value(&stats->face_area, gts_triangle_area(t));
}

/**
 * gts_surface_quality_stats:
 * @s: a #GtsSurface.
 * @stats: a #GtsSurfaceQualityStats.
 *
 * Fills @stats with quality statistics relevant to surface @s.
 */
void gts_surface_quality_stats(GtsSurface *s, GtsSurfaceQualityStats *stats) {
  g_return_if_fail(s != NULL);
  g_return_if_fail(stats != NULL);

  stats->parent = s;
  gts_range_init(&stats->face_quality);
  gts_range_init(&stats->face_area);
  gts_range_init(&stats->edge_length);
  gts_range_init(&stats->edge_angle);

  gts_surface_foreach_edge(s, (GtsFunc)quality_foreach_edge, stats);
  gts_surface_foreach_face(s, (GtsFunc)quality_foreach_face, stats);

  gts_range_update(&stats->face_quality);
  gts_range_update(&stats->face_area);
  gts_range_update(&stats->edge_length);
  gts_range_update(&stats->edge_angle);
}

/**
 * gts_surface_print_stats:
 * @s: a #GtsSurface.
 * @fptr: a file pointer.
 *
 * Writes in the file pointed to by @fptr the statistics for surface @s.
 */
void gts_surface_print_stats(GtsSurface *s, FILE *fptr) {
  GtsSurfaceStats stats;
  GtsSurfaceQualityStats qstats;

  g_return_if_fail(s != NULL);
  g_return_if_fail(fptr != NULL);

  gts_surface_stats(s, &stats);
  gts_surface_quality_stats(s, &qstats);

  fprintf(fptr,
          "# vertices: %llu edges: %llu faces: %llu\n"
          "# Connectivity statistics\n"
          "#   incompatible faces: %llu\n"
          "#   duplicate faces: %llu\n"
          "#   boundary edges: %llu\n"
          "#   duplicate edges: %llu\n"
          "#   non-manifold edges: %llu\n",
          stats.edges_per_vertex.n, stats.faces_per_edge.n, stats.n_faces, stats.n_incompatible_faces,
          stats.n_duplicate_faces, stats.n_boundary_edges, stats.n_duplicate_edges, stats.n_non_manifold_edges);
  fputs("#   edges per vertex: ", fptr);
  gts_range_print(&stats.edges_per_vertex, fptr);
  fputs("\n#   faces per edge: ", fptr);
  gts_range_print(&stats.faces_per_edge, fptr);
  fputs("\n# Geometric statistics\n#   face quality: ", fptr);
  gts_range_print(&qstats.face_quality, fptr);
  fputs("\n#   face area  : ", fptr);
  gts_range_print(&qstats.face_area, fptr);
  fputs("\n#   edge length : ", fptr);
  gts_range_print(&qstats.edge_length, fptr);
  fputc('\n', fptr);
}

static void write_vertex(GtsPoint *p, gpointer *data) {
  (*GTS_OBJECT(p)->klass->write)(GTS_OBJECT(p), (FILE *)data[0]);
  fputc('\n', (FILE *)data[0]);
  GTS_OBJECT(p)->reserved = GUINT_TO_POINTER(++(*((guint *)data[1])));
}

static void write_edge(GtsSegment *s, gpointer *data) {
  fprintf((FILE *)data[0], "%llu %llu", GPOINTER_TO_UINT(GTS_OBJECT(s->v1)->reserved),
          GPOINTER_TO_UINT(GTS_OBJECT(s->v2)->reserved));
  if (GTS_OBJECT(s)->klass->write)
    (*GTS_OBJECT(s)->klass->write)(GTS_OBJECT(s), (FILE *)data[0]);
  fputc('\n', (FILE *)data[0]);
  GTS_OBJECT(s)->reserved = GUINT_TO_POINTER(++(*((guint *)data[1])));
}

static void write_face(GtsTriangle *t, FILE *fp) {
  fprintf(fp, "%llu %llu %llu", GPOINTER_TO_UINT(GTS_OBJECT(t->e1)->reserved),
          GPOINTER_TO_UINT(GTS_OBJECT(t->e2)->reserved), GPOINTER_TO_UINT(GTS_OBJECT(t->e3)->reserved));
  if (GTS_OBJECT(t)->klass->write)
    (*GTS_OBJECT(t)->klass->write)(GTS_OBJECT(t), fp);
  fputc('\n', fp);
}

static void reset_reserved(GtsTriangle *t) {
  GtsSegment *s1 = GTS_SEGMENT(t->e1), *s2 = GTS_SEGMENT(t->e2), *s3 = GTS_SEGMENT(t->e3);

  GTS_OBJECT(s1)->reserved = NULL;
  GTS_OBJECT(s2)->reserved = NULL;
  GTS_OBJECT(s3)->reserved = NULL;
  GTS_OBJECT(s1->v1)->reserved = GTS_OBJECT(s1->v2)->reserved = NULL;
  GTS_OBJECT(s2->v1)->reserved = GTS_OBJECT(s2->v2)->reserved = NULL;
  GTS_OBJECT(s3->v1)->reserved = GTS_OBJECT(s3->v2)->reserved = NULL;
}

/**
 * gts_surface_write:
 * @s: a #GtsSurface.
 * @fptr: a file pointer.
 *
 * Writes in the file @fptr an ASCII representation of @s. The file
 * format is as follows.
 *
 * All the lines beginning with #GTS_COMMENTS are ignored. The first line
 * contains three unsigned integers separated by spaces. The first
 * integer is the number of vertices, nv, the second is the number of
 * edges, ne and the third is the number of faces, nf.
 *
 * Follows nv lines containing the x, y and z coordinates of the
 * vertices.  Follows ne lines containing the two indices (starting
 * from one) of the vertices of each edge. Follows nf lines containing
 * the three ordered indices (also starting from one) of the edges of
 * each face.
 *
 * The format described above is the least common denominator to all
 * GTS files.  Consistent with an object-oriented approach, the GTS
 * file format is extensible. Each of the lines of the file can be
 * extended with user-specific attributes accessible through the
 * read() and write() virtual methods of each of the objects written
 * (surface, vertices, edges or faces). When read with different
 * object classes, these extra attributes are just ignored.
 */
void gts_surface_write(GtsSurface *s, FILE *fptr) {
  guint n;
  gpointer data[2];
  GtsSurfaceStats stats;

  g_return_if_fail(s != NULL);
  g_return_if_fail(fptr != NULL);

  data[0] = fptr;
  data[1] = &n;

  gts_surface_stats(s, &stats);
  fprintf(fptr, "%llu %llu %llu", stats.edges_per_vertex.n, stats.faces_per_edge.n, stats.n_faces);
  if (GTS_OBJECT(s)->klass->write)
    (*GTS_OBJECT(s)->klass->write)(GTS_OBJECT(s), fptr);
  fputc('\n', fptr);
  n = 0;
  gts_surface_foreach_vertex(s, (GtsFunc)write_vertex, data);
  n = 0;
  gts_surface_foreach_edge(s, (GtsFunc)write_edge, data);
  gts_surface_foreach_face(s, (GtsFunc)write_face, fptr);
  gts_surface_foreach_face(s, (GtsFunc)reset_reserved, NULL);
}

static void write_vertex_oogl(GtsPoint *p, gpointer *data) {
  FILE *fp = data[0];

  fprintf(fp, "%g %g %g", p->x, p->y, p->z);
  if (GTS_OBJECT(p)->klass->color) {
    GtsColor c = (*GTS_OBJECT(p)->klass->color)(GTS_OBJECT(p));
    fprintf(fp, " %g %g %g 1.0\n", c.r, c.g, c.b);
  } else
    fputc('\n', fp);
  GTS_OBJECT(p)->reserved = GUINT_TO_POINTER((*((guint *)data[1]))++);
}

static void write_face_oogl(GtsTriangle *t, FILE *fp) {
  GtsVertex *v1, *v2, *v3;
  gts_triangle_vertices(t, &v1, &v2, &v3);
  fprintf(fp, "3 %llu %llu %llu", GPOINTER_TO_UINT(GTS_OBJECT(v1)->reserved),
          GPOINTER_TO_UINT(GTS_OBJECT(v2)->reserved), GPOINTER_TO_UINT(GTS_OBJECT(v3)->reserved));
  if (GTS_OBJECT(t)->klass->color) {
    GtsColor c = (*GTS_OBJECT(t)->klass->color)(GTS_OBJECT(t));
    fprintf(fp, " %g %g %g\n", c.r, c.g, c.b);
  } else
    fputc('\n', fp);
}

/**
 * gts_surface_write_oogl:
 * @s: a #GtsSurface.
 * @fptr: a file pointer.
 *
 * Writes in the file @fptr an OOGL (Geomview) representation of @s.
 */
void gts_surface_write_oogl(GtsSurface *s, FILE *fptr) {
  guint n = 0;
  gpointer data[2];
  GtsSurfaceStats stats;

  g_return_if_fail(s != NULL);
  g_return_if_fail(fptr != NULL);

  data[0] = fptr;
  data[1] = &n;

  gts_surface_stats(s, &stats);
  if (GTS_OBJECT_CLASS(s->vertex_class)->color)
    fputs("COFF ", fptr);
  else
    fputs("OFF ", fptr);
  fprintf(fptr, "%llu %llu %llu\n", stats.edges_per_vertex.n, stats.n_faces, stats.faces_per_edge.n);
  gts_surface_foreach_vertex(s, (GtsFunc)write_vertex_oogl, data);
  gts_surface_foreach_face(s, (GtsFunc)write_face_oogl, fptr);
  gts_surface_foreach_vertex(s, (GtsFunc)gts_object_reset_reserved, NULL);
}

static void write_edge_oogl_boundary(GtsSegment *s, gpointer *data) {
  if (!gts_edge_is_boundary(GTS_EDGE(s), data[1]))
    return;

  if (GTS_OBJECT(s)->klass->color) {
    GtsColor c = (*GTS_OBJECT(s)->klass->color)(GTS_OBJECT(s));
    fprintf(data[0], "VECT 1 2 1 2 1 %g %g %g %g %g %g %g %g %g 1.\n", GTS_POINT(s->v1)->x, GTS_POINT(s->v1)->y,
            GTS_POINT(s->v1)->z, GTS_POINT(s->v2)->x, GTS_POINT(s->v2)->y, GTS_POINT(s->v2)->z, c.r, c.g, c.b);
  } else
    fprintf(data[0], "VECT 1 2 0 2 0 %g %g %g %g %g %g\n", GTS_POINT(s->v1)->x, GTS_POINT(s->v1)->y,
            GTS_POINT(s->v1)->z, GTS_POINT(s->v2)->x, GTS_POINT(s->v2)->y, GTS_POINT(s->v2)->z);
}

/**
 * gts_surface_write_oogl_boundary:
 * @s: a #GtsSurface.
 * @fptr: a file pointer.
 *
 * Writes in the file @fptr an OOGL (Geomview) representation of the
 * boundary of @s.
 */
void gts_surface_write_oogl_boundary(GtsSurface *s, FILE *fptr) {
  gpointer data[2];

  g_return_if_fail(s != NULL);
  g_return_if_fail(fptr != NULL);

  data[0] = fptr;
  data[1] = s;
  fputs("LIST {\n", fptr);
  gts_surface_foreach_edge(s, (GtsFunc)write_edge_oogl_boundary, data);
  fputs("}\n", fptr);
}

static void vertex_foreach_face(GtsTriangle *t, gpointer t_data, gpointer *info) {
  GHashTable *hash = info[0];
  gpointer data = info[1];
  GtsFunc func = info[2];
  GtsSegment *s1 = GTS_SEGMENT(t->e1);

  UNUSED(t_data);

  if (!g_hash_table_lookup(hash, s1->v1)) {
    (*func)(s1->v1, data);
    g_hash_table_insert(hash, s1->v1, GINT_TO_POINTER(-1));
  }
  if (!g_hash_table_lookup(hash, s1->v2)) {
    (*func)(s1->v2, data);
    g_hash_table_insert(hash, s1->v2, GINT_TO_POINTER(-1));
  }
  if (!g_hash_table_lookup(hash, gts_triangle_vertex(t))) {
    (*func)(gts_triangle_vertex(t), data);
    g_hash_table_insert(hash, gts_triangle_vertex(t), GINT_TO_POINTER(-1));
  }
}

/**
 * gts_surface_foreach_vertex:
 * @s: a #GtsSurface.
 * @func: a #GtsFunc.
 * @data: user data to be passed to @func.
 *
 * Calls @func once for each vertex of @s.
 */
void gts_surface_foreach_vertex(GtsSurface *s, GtsFunc func, gpointer data) {
  gpointer info[3];

  g_return_if_fail(s != NULL);
  g_return_if_fail(func != NULL);

  /* forbid removal of faces */
  s->keep_faces = TRUE;
  info[0] = g_hash_table_new(NULL, NULL);
  info[1] = data;
  info[2] = func;
  g_hash_table_foreach(s->faces, (GHFunc)vertex_foreach_face, info);
  g_hash_table_destroy(info[0]);
  /* allow removal of faces */
  s->keep_faces = FALSE;
}

static void edge_foreach_face(GtsTriangle *t, gpointer t_data, gpointer *info) {
  GHashTable *hash = info[0];
  gpointer data = info[1];
  GtsFunc func = info[2];

  UNUSED(t_data);

  if (!g_hash_table_lookup(hash, t->e1)) {
    (*func)(t->e1, data);
    g_hash_table_insert(hash, t->e1, GINT_TO_POINTER(-1));
  }
  if (!g_hash_table_lookup(hash, t->e2)) {
    (*func)(t->e2, data);
    g_hash_table_insert(hash, t->e2, GINT_TO_POINTER(-1));
  }
  if (!g_hash_table_lookup(hash, t->e3)) {
    (*func)(t->e3, data);
    g_hash_table_insert(hash, t->e3, GINT_TO_POINTER(-1));
  }
}

/**
 * gts_surface_foreach_edge:
 * @s: a #GtsSurface.
 * @func: a #GtsFunc.
 * @data: user data to be passed to @func.
 *
 * Calls @func once for each edge of @s.
 */
void gts_surface_foreach_edge(GtsSurface *s, GtsFunc func, gpointer data) {
  gpointer info[3];

  g_return_if_fail(s != NULL);
  g_return_if_fail(func != NULL);

  /* forbid removal of faces */
  s->keep_faces = TRUE;
  info[0] = g_hash_table_new(NULL, NULL);
  info[1] = data;
  info[2] = func;
  g_hash_table_foreach(s->faces, (GHFunc)edge_foreach_face, info);
  g_hash_table_destroy(info[0]);
  /* allow removal of faces */
  s->keep_faces = FALSE;
}

#if 0

/* Due to an unkown reason g_hash_table_foreach does not allow to stop 
 * the loop, hence the redefinition. I hope they don't change
 * the GHashTable, GHashNode structures ... */
typedef struct _GHashNode      GHashNode;

struct _GHashNode
{
  gpointer key;
  gpointer value;
  GHashNode *next;
};

struct _GHashTable
{
  gint size;
  gint nnodes;
  guint frozen;
  GHashNode **nodes;
  GHashFunc hash_func;
  GCompareFunc key_compare_func;
};

/**
 * gts_surface_foreach_face:
 * @s: a #GtsSurface.
 * @func: a #GtsFunc.
 * @data: user data to be passed to @func.
 *
 * Calls @func once for each face of @s.
 */
void gts_surface_foreach_face (GtsSurface * s,
             GtsFunc func, 
             gpointer data)
{
  guint i;
  GHashNode * node;
  gboolean stop = FALSE;

  g_return_if_fail (s != NULL);
  g_return_if_fail (func != NULL);

  /* forbid removal of faces */
  s->keep_faces = TRUE;
  for (i = 0; i < s->faces->size && !stop; i++)
  for (node = s->faces->nodes[i]; node && !stop; node = node->next)
      stop = !(* func) (node->key, data);
  /* allow removal of faces */
  s->keep_faces = FALSE;
}

#else

static void foreach_face(GtsFace *f, gpointer t_data, gpointer *info) {
  UNUSED(t_data);
  (*((GtsFunc)info[0]))(f, info[1]);
}

/**
 * gts_surface_foreach_face:
 * @s: a #GtsSurface.
 * @func: a #GtsFunc.
 * @data: user data to be passed to @func.
 *
 * Calls @func once for each face of @s.
 */
void gts_surface_foreach_face(GtsSurface *s, GtsFunc func, gpointer data) {
  gpointer info[2];

  g_return_if_fail(s != NULL);
  g_return_if_fail(func != NULL);

  /* forbid removal of faces */
  s->keep_faces = TRUE;
  info[0] = func;
  info[1] = data;
  g_hash_table_foreach(s->faces, (GHFunc)foreach_face, info);
  /* allow removal of faces */
  s->keep_faces = FALSE;
}

#endif

static gboolean foreach_face_remove(GtsFace *f, gpointer t_data, gpointer *info) {
  UNUSED(t_data);
  if ((*((GtsFunc)info[0]))(f, info[1])) {
    GtsSurface *s = info[2];

    f->surfaces = g_slist_remove(f->surfaces, s);
    if (!GTS_OBJECT_DESTROYED(f) && !gts_allow_floating_faces && f->surfaces == NULL)
      gts_object_destroy(GTS_OBJECT(f));

    if (GTS_SURFACE_CLASS(GTS_OBJECT(s)->klass)->remove_face)
      (*GTS_SURFACE_CLASS(GTS_OBJECT(s)->klass)->remove_face)(s, f);

    return TRUE;
  }
  return FALSE;
}

/**
 * gts_surface_foreach_face_remove:
 * @s: a #GtsSurface.
 * @func: a #GtsFunc.
 * @data: user data to be passed to @func.
 *
 * Calls @func once for each face of @s. If @func returns %TRUE the
 * corresponding face is removed from @s (and destroyed if it does not
 * belong to any other surface and #gts_allow_floating_faces is set to
 * %FALSE).
 *
 * Returns: the number of faces removed from @s.
 */
guint gts_surface_foreach_face_remove(GtsSurface *s, GtsFunc func, gpointer data) {
  gpointer info[3];
  guint n;

  g_return_val_if_fail(s != NULL, 0);
  g_return_val_if_fail(func != NULL, 0);

  /* forbid removal of faces */
  s->keep_faces = TRUE;
  info[0] = func;
  info[1] = data;
  info[2] = s;
  n = g_hash_table_foreach_remove(s->faces, (GHRFunc)foreach_face_remove, info);
  /* allow removal of faces */
  s->keep_faces = FALSE;

  return n;
}

static void midvertex_insertion(GtsEdge *e, GtsSurface *surface, GtsHeap *heap, GtsVertexClass *vertex_class,
                                GtsEdgeClass *edge_class) {
  GtsVertex *midvertex;
  GtsEdge *e1, *e2;
  GSList *i;

  midvertex = gts_segment_midvertex(GTS_SEGMENT(e), vertex_class);
  e1 = gts_edge_new(edge_class, GTS_SEGMENT(e)->v1, midvertex);
  gts_heap_insert(heap, e1);
  e2 = gts_edge_new(edge_class, GTS_SEGMENT(e)->v2, midvertex);
  gts_heap_insert(heap, e2);

  /* creates new faces and modifies old ones */
  i = e->triangles;
  while (i) {
    GtsTriangle *t = i->data;
    GtsVertex *v1, *v2, *v3;
    GtsEdge *te2, *te3, *ne, *tmp;

    gts_triangle_vertices_edges(t, e, &v1, &v2, &v3, &e, &te2, &te3);
    ne = gts_edge_new(edge_class, midvertex, v3);
    gts_heap_insert(heap, ne);
    if (GTS_SEGMENT(e1)->v1 == v2) {
      tmp = e1;
      e1 = e2;
      e2 = tmp;
    }
    e1->triangles = g_slist_prepend(e1->triangles, t);
    ne->triangles = g_slist_prepend(ne->triangles, t);
    te2->triangles = g_slist_remove(te2->triangles, t);
    t->e1 = e1;
    t->e2 = ne;
    t->e3 = te3;
    gts_surface_add_face(surface, gts_face_new(surface->face_class, e2, te2, ne));
    i = i->next;
  }
  /* destroys edge */
  g_slist_free(e->triangles);
  e->triangles = NULL;
  gts_object_destroy(GTS_OBJECT(e));
}

static gint edge_is_longer(GtsSegment *s1, GtsSegment *s2) {
  if (gts_point_distance2(GTS_POINT(s1->v1), GTS_POINT(s1->v2)) >
      gts_point_distance2(GTS_POINT(s2->v1), GTS_POINT(s2->v2)))
    return -1;
  return 1;
}

static void create_heap_refine(GtsEdge *e, GtsHeap *heap) { gts_heap_insert(heap, e); }

/**
 * gts_surface_refine:
 * @surface: a #GtsSurface.
 * @compare_func: a GCompareFunc.
 * @stop_func: a #GtsStopFunc.
 * @data: user data to be passed to @stop_func.
 *
 * Refine @surface using a midvertex insertion technique. All the
 * edges of @surface are ordered according to @compare_func. The edges
 * are then processed in order until @stop_func returns %TRUE. Each
 * edge is split in two and new edges and faces are created.
 *
 * If @compare_func is set to %NULL an internal function comparing the
 * squared length of the edges is used.
 */
void gts_surface_refine(GtsSurface *surface, GCompareFunc compare_func, GtsStopFunc stop_func, gpointer data) {
  GtsHeap *heap;
  GtsEdge *e;

  g_return_if_fail(surface != NULL);
  g_return_if_fail(stop_func != NULL);

  if (compare_func == NULL)
    compare_func = (GCompareFunc)edge_is_longer;

  heap = gts_heap_new(compare_func);
  gts_heap_freeze(heap);
  gts_surface_foreach_edge(surface, (GtsFunc)create_heap_refine, heap);
  gts_heap_thaw(heap);
  while ((e = gts_heap_remove_top(heap)) &&
         !(*stop_func)(gts_point_distance(GTS_POINT(GTS_SEGMENT(e)->v1), GTS_POINT(GTS_SEGMENT(e)->v2)),
                       gts_heap_size(heap) + gts_edge_face_number(e, surface) + 2, data))
    midvertex_insertion(e, surface, heap, surface->vertex_class, surface->edge_class);
  gts_heap_destroy(heap);
}

static GSList *edge_triangles(GtsEdge *e1, GtsEdge *e) {
  GSList *i = e1->triangles;
  GSList *triangles = NULL;

  while (i) {
    GtsTriangle *t = i->data;
    if (t->e1 == e || t->e2 == e || t->e3 == e) {
      GtsEdge *e2;
      GSList *j;
      if (t->e1 == e) {
        if (t->e2 == e1)
          e2 = t->e3;
        else
          e2 = t->e2;
      } else if (t->e2 == e) {
        if (t->e3 == e1)
          e2 = t->e1;
        else
          e2 = t->e3;
      } else {
        if (t->e2 == e1)
          e2 = t->e1;
        else
          e2 = t->e2;
      }
      j = e2->triangles;
      while (j) {
        GtsTriangle *t = j->data;
        if (t->e1 != e && t->e2 != e && t->e3 != e)
          triangles = g_slist_prepend(triangles, t);
        j = j->next;
      }
    } else
      triangles = g_slist_prepend(triangles, t);
    i = i->next;
  }
  return triangles;
}

static void replace_vertex(GSList *i, GtsVertex *v1, GtsVertex *v) {
  while (i) {
    GtsSegment *s = i->data;
    if (s->v1 == v1)
      s->v1 = v;
    else
      s->v2 = v;
    i = i->next;
  }
}

/**
 * gts_edge_collapse_creates_fold:
 * @e: a #GtsEdge.
 * @v: a #GtsVertex.
 * @max:  the maximum value of the square of the cosine of the angle between
 * two triangles.
 *
 * Returns: %TRUE if collapsing edge @e to vertex @v would create
 * faces making an angle the cosine squared of which would be larger than max,
 * %FALSE otherwise.
 */
gboolean gts_edge_collapse_creates_fold(GtsEdge *e, GtsVertex *v, gdouble max) {
  GtsVertex *v1, *v2;
  GtsSegment *s;
  GSList *i;
  gboolean folded = FALSE;

  g_return_val_if_fail(e != NULL, TRUE);
  g_return_val_if_fail(v != NULL, TRUE);

  s = GTS_SEGMENT(e);
  v1 = s->v1;
  v2 = s->v2;
  replace_vertex(v1->segments, v1, v);
  replace_vertex(v2->segments, v2, v);

  i = v1->segments;
  while (i && !folded) {
    GtsSegment *s = i->data;
    if (GTS_IS_EDGE(s)) {
      GtsEdge *e1 = GTS_EDGE(s);
      if (e1 != e) {
        GSList *triangles = edge_triangles(e1, e);
        folded = gts_triangles_are_folded(triangles, s->v1, s->v2, max);
        g_slist_free(triangles);
      }
    }
    i = i->next;
  }

  i = v2->segments;
  while (i && !folded) {
    GtsSegment *s = i->data;
    if (GTS_IS_EDGE(s)) {
      GtsEdge *e1 = GTS_EDGE(s);
      if (e1 != e) {
        GSList *triangles = edge_triangles(e1, e);
        folded = gts_triangles_are_folded(triangles, s->v1, s->v2, max);
        g_slist_free(triangles);
      }
    }
    i = i->next;
  }
#if 1
  if (!folded) {
    GSList *triangles = gts_vertex_triangles(v1, NULL);
    i = triangles = gts_vertex_triangles(v2, triangles);
    while (i && !folded) {
      GtsTriangle *t = i->data;
      if (t->e1 != e && t->e2 != e && t->e3 != e) {
        GtsEdge *e1 = gts_triangle_edge_opposite(t, v);
        g_assert(e1);
        folded = gts_triangles_are_folded(e1->triangles, GTS_SEGMENT(e1)->v1, GTS_SEGMENT(e1)->v2, max);
      }
      i = i->next;
    }
    g_slist_free(triangles);
  }
#endif
  replace_vertex(v1->segments, v, v1);
  replace_vertex(v2->segments, v, v2);
  return folded;
}

/**
 * gts_edge_collapse_is_valid:
 * @e: a #GtsEdge.
 *
 * An implementation of the topological constraints described in the
 * "Mesh Optimization" article of Hoppe et al (1993).
 *
 * Returns: %TRUE if @e can be collapsed without violation of the topological
 * constraints, %FALSE otherwise.
 */
gboolean gts_edge_collapse_is_valid(GtsEdge *e) {
  GSList *i;

  g_return_val_if_fail(e != NULL, FALSE);

  i = GTS_SEGMENT(e)->v1->segments;
  while (i) {
    GtsEdge *e1 = i->data;
    if (e1 != e && GTS_IS_EDGE(e1)) {
      GtsEdge *e2 = NULL;
      GSList *j =
          GTS_SEGMENT(e1)->v1 == GTS_SEGMENT(e)->v1 ? GTS_SEGMENT(e1)->v2->segments : GTS_SEGMENT(e1)->v1->segments;
      while (j && !e2) {
        GtsEdge *e1 = j->data;
        if (GTS_IS_EDGE(e1) && (GTS_SEGMENT(e1)->v1 == GTS_SEGMENT(e)->v2 || GTS_SEGMENT(e1)->v2 == GTS_SEGMENT(e)->v2))
          e2 = e1;
        j = j->next;
      }
      if (e2 && !gts_triangle_use_edges(e, e1, e2))
        return FALSE;
    }
    i = i->next;
  }

  if (gts_edge_is_boundary(e, NULL)) {
    GtsTriangle *t = e->triangles->data;
    if (gts_edge_is_boundary(t->e1, NULL) && gts_edge_is_boundary(t->e2, NULL) && gts_edge_is_boundary(t->e3, NULL))
      return FALSE;
  } else {
    if (gts_vertex_is_boundary(GTS_SEGMENT(e)->v1, NULL) && gts_vertex_is_boundary(GTS_SEGMENT(e)->v2, NULL))
      return FALSE;
    if (gts_edge_belongs_to_tetrahedron(e))
      return FALSE;
  }

  return TRUE;
}

#define HEAP_INSERT_EDGE(h, e) (GTS_OBJECT(e)->reserved = gts_eheap_insert(h, e))
#define HEAP_REMOVE_EDGE(h, e) (gts_eheap_remove(h, GTS_OBJECT(e)->reserved), GTS_OBJECT(e)->reserved = NULL)

static GtsVertex *edge_collapse(GtsEdge *e, GtsEHeap *heap, GtsCoarsenFunc coarsen_func, gpointer coarsen_data,
                                GtsVertexClass *klass, gdouble maxcosine2) {
  GSList *i;
  GtsVertex *v1 = GTS_SEGMENT(e)->v1, *v2 = GTS_SEGMENT(e)->v2, *mid;

  /* if the edge is degenerate (i.e. v1 == v2), destroy and return */
  if (v1 == v2) {
    gts_object_destroy(GTS_OBJECT(e));
    return NULL;
  }

  if (!gts_edge_collapse_is_valid(e)) {
    GTS_OBJECT(e)->reserved = gts_eheap_insert_with_key(heap, e, G_MAXDOUBLE);
    return NULL;
  }

  mid = (*coarsen_func)(e, klass, coarsen_data);

  if (gts_edge_collapse_creates_fold(e, mid, maxcosine2)) {
    GTS_OBJECT(e)->reserved = gts_eheap_insert_with_key(heap, e, G_MAXDOUBLE);
    gts_object_destroy(GTS_OBJECT(mid));
    return NULL;
  }

  gts_object_destroy(GTS_OBJECT(e));

  gts_vertex_replace(v1, mid);
  gts_object_destroy(GTS_OBJECT(v1));
  gts_vertex_replace(v2, mid);
  gts_object_destroy(GTS_OBJECT(v2));

  /* destroy duplicate edges */
  i = mid->segments;
  while (i) {
    GtsEdge *e1 = i->data;
    GtsEdge *duplicate;
    while ((duplicate = gts_edge_is_duplicate(e1))) {
      gts_edge_replace(duplicate, GTS_EDGE(e1));
      HEAP_REMOVE_EDGE(heap, duplicate);
      gts_object_destroy(GTS_OBJECT(duplicate));
    }
    i = i->next;
    if (!e1->triangles) {
      /* e1 is the result of the collapse of one edge of a pair of identical
   faces (it should not happen unless duplicate triangles are present in
   the initial surface) */
      g_warning("file %s: line %d (%s): probably duplicate triangle.", __FILE__, __LINE__, G_GNUC_PRETTY_FUNCTION);
      HEAP_REMOVE_EDGE(heap, e1);
      gts_object_destroy(GTS_OBJECT(e1));
      if (i == NULL) /* mid has been destroyed */
        mid = NULL;
    }
  }

  return mid;
}

/*
static void update_closest_neighbors (GtsVertex * v, GtsEHeap * heap)
{
  GSList * i = v->segments;

  while (i) {
  GtsSegment * s = i->data;
  if (GTS_IS_EDGE (s)) {
      HEAP_REMOVE_EDGE (heap, GTS_EDGE (s));
      HEAP_INSERT_EDGE (heap, GTS_EDGE (s));
  }
  i = i->next;
  }
}
*/

static void update_2nd_closest_neighbors(GtsVertex *v, GtsEHeap *heap) {
  GSList *i = v->segments;
  GSList *list = NULL;

  while (i) {
    GtsSegment *s = i->data;
    if (GTS_IS_EDGE(s)) {
      GtsVertex *v1 = s->v1 == v ? s->v2 : s->v1;
      GSList *j = v1->segments;
      while (j) {
        GtsSegment *s1 = j->data;
        if (GTS_IS_EDGE(s1) && !g_slist_find(list, s1))
          list = g_slist_prepend(list, s1);
        j = j->next;
      }
    }
    i = i->next;
  }

  i = list;
  while (i) {
    GtsEdge *e = i->data;
    HEAP_REMOVE_EDGE(heap, e);
    HEAP_INSERT_EDGE(heap, e);
    i = i->next;
  }

  g_slist_free(list);
}

static gdouble edge_length2(GtsEdge *e) {
  return gts_point_distance2(GTS_POINT(GTS_SEGMENT(e)->v1), GTS_POINT(GTS_SEGMENT(e)->v2));
}

static void create_heap_coarsen(GtsEdge *e, GtsEHeap *heap) { HEAP_INSERT_EDGE(heap, e); }

/**
 * gts_surface_coarsen:
 * @surface: a #GtsSurface.
 * @cost_func: a function returning the cost for a given edge.
 * @cost_data: user data to be passed to @cost_func.
 * @coarsen_func: a #GtsCoarsenVertexFunc.
 * @coarsen_data: user data to be passed to @coarsen_func.
 * @stop_func: a #GtsStopFunc.
 * @stop_data: user data to be passed to @stop_func.
 * @minangle: minimum angle between two neighboring triangles.
 *
 * The edges of @surface are sorted according to @cost_func to
 * create a priority heap (a #GtsEHeap). The edges are extracted in
 * turn from the top of the heap and collapsed (i.e. the vertices are
 * replaced by the vertex returned by the @coarsen_func function)
 * until the @stop_func functions returns %TRUE.
 *
 * If @cost_func is set to %NULL, the edges are sorted according
 * to their length squared (the shortest is on top).
 *
 * If @coarsen_func is set to %NULL gts_segment_midvertex() is used.
 *
 * The minimum angle is used to avoid introducing faces which would be folded.
 */
void gts_surface_coarsen(GtsSurface *surface, GtsKeyFunc cost_func, gpointer cost_data, GtsCoarsenFunc coarsen_func,
                         gpointer coarsen_data, GtsStopFunc stop_func, gpointer stop_data, gdouble minangle) {
  GtsEHeap *heap;
  GtsEdge *e;
  gdouble top_cost;
  gdouble maxcosine2;

  g_return_if_fail(surface != NULL);
  g_return_if_fail(stop_func != NULL);

  if (cost_func == NULL)
    cost_func = (GtsKeyFunc)edge_length2;
  if (coarsen_func == NULL)
    coarsen_func = (GtsCoarsenFunc)gts_segment_midvertex;

  heap = gts_eheap_new(cost_func, cost_data);
  maxcosine2 = cos(minangle);
  maxcosine2 *= maxcosine2;

  gts_eheap_freeze(heap);
  gts_surface_foreach_edge(surface, (GtsFunc)create_heap_coarsen, heap);
  gts_eheap_thaw(heap);
  /* we want to control edge destruction manually */
  gts_allow_floating_edges = TRUE;
  while ((e = gts_eheap_remove_top(heap, &top_cost)) && (top_cost < G_MAXDOUBLE) &&
         !(*stop_func)(top_cost, gts_eheap_size(heap) - gts_edge_face_number(e, surface), stop_data)) {
    GtsVertex *v = edge_collapse(e, heap, coarsen_func, coarsen_data, surface->vertex_class, maxcosine2);
    if (v != NULL)
      update_2nd_closest_neighbors(v, heap);
  }
  gts_allow_floating_edges = FALSE;

  /* set reserved field of remaining edges back to NULL */
  if (e)
    GTS_OBJECT(e)->reserved = NULL;
  gts_eheap_foreach(heap, (GFunc)gts_object_reset_reserved, NULL);

  gts_eheap_destroy(heap);
}

/**
 * gts_coarsen_stop_number:
 * @cost: the cost of the edge collapse considered.
 * @nedge: the current number of edges of the surface being simplified.
 * @min_number: a pointer to the minimum number of edges desired for the
 * surface being simplified.
 *
 * This function is to be used as the @stop_func argument of
 * gts_surface_coarsen() or gts_psurface_new().
 *
 * Returns: %TRUE if the edge collapse would create a surface with a smaller
 * number of edges than given by @min_number, %FALSE otherwise.
 */
gboolean gts_coarsen_stop_number(gdouble cost, guint nedge, guint *min_number) {
  UNUSED(cost);

  g_return_val_if_fail(min_number != NULL, TRUE);

  if (nedge < *min_number)
    return TRUE;
  return FALSE;
}

/**
 * gts_coarsen_stop_cost:
 * @cost: the cost of the edge collapse considered.
 * @nedge: the current number of edges of the surface being simplified.
 * @max_cost: a pointer to the maximum cost allowed for an edge collapse.
 *
 * This function is to be used as the @stop_func argument of
 * gts_surface_coarsen() or gts_psurface_new().
 *
 * Returns: %TRUE if the cost of the edge collapse considered is larger than
 * given by @max_cost, %FALSE otherwise.
 */
gboolean gts_coarsen_stop_cost(gdouble cost, guint nedge, gdouble *max_cost) {
  UNUSED(nedge);

  g_return_val_if_fail(max_cost != NULL, TRUE);

  if (cost > *max_cost)
    return TRUE;
  return FALSE;
}

static void foreach_vertex_copy(GtsPoint *p, GtsVertexClass *klass) {
  GTS_OBJECT(p)->reserved = gts_vertex_new(klass, p->x, p->y, p->z);
}

static void foreach_edge_copy(GtsSegment *s, GtsEdgeClass *klass) {
  GTS_OBJECT(s)->reserved = gts_edge_new(klass, GTS_OBJECT(s->v1)->reserved, GTS_OBJECT(s->v2)->reserved);
}

static void foreach_face_copy(GtsTriangle *t, GtsSurface *s) {
  gts_surface_add_face(s, gts_face_new(s->face_class, GTS_OBJECT(t->e1)->reserved, GTS_OBJECT(t->e2)->reserved,
                                       GTS_OBJECT(t->e3)->reserved));
}

/**
 * gts_surface_copy:
 * @s1: a #GtsSurface.
 * @s2: a #GtsSurface.
 *
 * Add a copy of all the faces, edges and vertices of @s2 to @s1.
 *
 * Returns: @s1.
 */
GtsSurface *gts_surface_copy(GtsSurface *s1, GtsSurface *s2) {
  g_return_val_if_fail(s1 != NULL, NULL);
  g_return_val_if_fail(s2 != NULL, NULL);

  gts_surface_foreach_vertex(s2, (GtsFunc)foreach_vertex_copy, s1->vertex_class);
  gts_surface_foreach_edge(s2, (GtsFunc)foreach_edge_copy, s1->edge_class);
  gts_surface_foreach_face(s2, (GtsFunc)foreach_face_copy, s1);

  gts_surface_foreach_vertex(s2, (GtsFunc)gts_object_reset_reserved, NULL);
  gts_surface_foreach_edge(s2, (GtsFunc)gts_object_reset_reserved, NULL);

  return s1;
}

static void merge_foreach_face(GtsFace *f, GtsSurface *s) { gts_surface_add_face(s, f); }

/**
 * gts_surface_merge:
 * @s: a #GtsSurface.
 * @with: another #GtsSurface.
 *
 * Adds all the faces of @with which do not already belong to @s
 * to @s.
 */
void gts_surface_merge(GtsSurface *s, GtsSurface *with) {
  g_return_if_fail(s != NULL);
  g_return_if_fail(with != NULL);

  gts_surface_foreach_face(with, (GtsFunc)merge_foreach_face, s);
}

static void manifold_foreach_edge(GtsEdge *e, gpointer *data) {
  gboolean *is_manifold = data[0];

  if (*is_manifold) {
    if (gts_edge_face_number(e, data[1]) > 2)
      *is_manifold = FALSE;
  }
}

/**
 * gts_surface_is_manifold:
 * @s: a #GtsSurface.
 *
 * Returns: %TRUE if the surface is a manifold, %FALSE otherwise.
 */
gboolean gts_surface_is_manifold(GtsSurface *s) {
  gboolean is_manifold = TRUE;
  gpointer data[2];

  g_return_val_if_fail(s != NULL, FALSE);

  data[0] = &is_manifold;
  data[1] = s;
  gts_surface_foreach_edge(s, (GtsFunc)manifold_foreach_edge, data);
  return is_manifold;
}

static void closed_foreach_edge(GtsEdge *e, gpointer *data) {
  gboolean *is_closed = data[0];

  if (*is_closed) {
    if (gts_edge_face_number(e, data[1]) != 2)
      *is_closed = FALSE;
  }
}

/**
 * gts_surface_is_closed:
 * @s: a #GtsSurface.
 *
 * Returns: %TRUE if @s is a closed surface, %FALSE otherwise. Note that a
 * closed surface is also a manifold.
 */
gboolean gts_surface_is_closed(GtsSurface *s) {
  gboolean is_closed = TRUE;
  gpointer data[2];

  g_return_val_if_fail(s != NULL, FALSE);

  data[0] = &is_closed;
  data[1] = s;
  gts_surface_foreach_edge(s, (GtsFunc)closed_foreach_edge, data);
  return is_closed;
}

static void orientable_foreach_edge(GtsEdge *e, gpointer *data) {
  gboolean *is_orientable = data[0];

  if (*is_orientable) {
    GtsSurface *surface = data[1];
    GtsFace *f1 = NULL, *f2 = NULL;
    GSList *i = e->triangles;
    while (i && *is_orientable) {
      GtsFace *f = i->data;
      if (GTS_IS_FACE(f) && gts_face_has_parent_surface(f, surface)) {
        if (!f1)
          f1 = f;
        else if (!f2)
          f2 = f;
        else
          *is_orientable = FALSE;
      }
      i = i->next;
    }
    if (f1 && f2 && !gts_triangles_are_compatible(GTS_TRIANGLE(f1), GTS_TRIANGLE(f2), e))
      *is_orientable = FALSE;
  }
}

/**
 * gts_surface_is_orientable:
 * @s: a #GtsSurface.
 *
 * Returns: %TRUE if all the faces of @s have compatible orientation
 * as checked by gts_faces_are_compatible(), %FALSE otherwise. Note that
 * an orientable surface is also a manifold.
 */
gboolean gts_surface_is_orientable(GtsSurface *s) {
  gboolean is_orientable = TRUE;
  gpointer data[2];

  g_return_val_if_fail(s != NULL, FALSE);

  data[0] = &is_orientable;
  data[1] = s;
  gts_surface_foreach_edge(s, (GtsFunc)orientable_foreach_edge, data);
  return is_orientable;
}

static void volume_foreach_face(GtsTriangle *t, gdouble *volume) {
  GtsVertex *va, *vb, *vc;
  GtsPoint *pa, *pb, *pc;

  gts_triangle_vertices(t, &va, &vb, &vc);
  pa = GTS_POINT(va);
  pb = GTS_POINT(vb);
  pc = GTS_POINT(vc);

  *volume += (pa->x * (pb->y * pc->z - pb->z * pc->y) + pb->x * (pc->y * pa->z - pc->z * pa->y) +
              pc->x * (pa->y * pb->z - pa->z * pb->y));
}

/**
 * gts_surface_volume:
 * @s: a #GtsSurface.
 *
 * Returns: the signed volume of the domain bounded by the surface @s. It
 * makes sense only if @s is a closed and orientable manifold.
 */
gdouble gts_surface_volume(GtsSurface *s) {
  gdouble volume = 0.0;

  g_return_val_if_fail(s != NULL, 0.0);

  gts_surface_foreach_face(s, (GtsFunc)volume_foreach_face, &volume);

  return volume / 6.;
}

static void number_foreach(gpointer data, guint *n) {
  UNUSED(data);
  (*n)++;
}

/**
 * gts_surface_vertex_number:
 * @s: a #GtsSurface.
 *
 * Returns: the number of vertices of @s.
 */
guint gts_surface_vertex_number(GtsSurface *s) {
  guint n = 0;

  g_return_val_if_fail(s != NULL, 0);

  gts_surface_foreach_vertex(s, (GtsFunc)number_foreach, &n);

  return n;
}

/**
 * gts_surface_edge_number:
 * @s: a #GtsSurface.
 *
 * Returns: the number of edges of @s.
 */
guint gts_surface_edge_number(GtsSurface *s) {
  guint n = 0;

  g_return_val_if_fail(s != NULL, 0);

  gts_surface_foreach_edge(s, (GtsFunc)number_foreach, &n);

  return n;
}

/**
 * gts_surface_face_number:
 * @s: a #GtsSurface.
 *
 * Returns: the number of faces of @s
 */
guint gts_surface_face_number(GtsSurface *s) {
  g_return_val_if_fail(s != NULL, 0);

  return g_hash_table_size(s->faces);
}

static void build_list_face(GtsTriangle *t, GSList **list) {
  *list = g_slist_prepend(*list, gts_bbox_triangle(gts_bbox_class(), t));
}

static void build_list_boundary(GtsEdge *e, GSList **list) {
  if (gts_edge_is_boundary(e, NULL))
    *list = g_slist_prepend(*list, gts_bbox_segment(gts_bbox_class(), GTS_SEGMENT(e)));
}

/**
 * gts_surface_distance:
 * @s1: a #GtsSurface.
 * @s2: a #GtsSurface.
 * @delta: a spatial increment defined as the percentage of the diagonal
 * of the bounding box of @s2.
 * @face_range: a #GtsRange.
 * @boundary_range: a #GtsRange.
 *
 * Using the gts_bb_tree_surface_distance() and
 * gts_bb_tree_surface_boundary_distance() functions fills @face_range
 * and @boundary_range with the min, max and average Euclidean
 * (minimum) distances between the faces of @s1 and the faces of @s2
 * and between the boundary edges of @s1 and @s2.
 */
void gts_surface_distance(GtsSurface *s1, GtsSurface *s2, gdouble delta, GtsRange *face_range,
                          GtsRange *boundary_range) {
  GNode *face_tree, *boundary_tree;
  GSList *bboxes;

  g_return_if_fail(s1 != NULL);
  g_return_if_fail(s2 != NULL);
  g_return_if_fail(delta > 0. && delta < 1.);
  g_return_if_fail(face_range != NULL);
  g_return_if_fail(boundary_range != NULL);

  bboxes = NULL;
  gts_surface_foreach_face(s2, (GtsFunc)build_list_face, &bboxes);
  if (bboxes != NULL) {
    face_tree = gts_bb_tree_new(bboxes);
    g_slist_free(bboxes);

    gts_bb_tree_surface_distance(face_tree, s1, (GtsBBoxDistFunc)gts_point_triangle_distance, delta, face_range);
    gts_bb_tree_destroy(face_tree, TRUE);

    bboxes = NULL;
    gts_surface_foreach_edge(s2, (GtsFunc)build_list_boundary, &bboxes);
    if (bboxes != NULL) {
      boundary_tree = gts_bb_tree_new(bboxes);
      g_slist_free(bboxes);

      gts_bb_tree_surface_boundary_distance(boundary_tree, s1, (GtsBBoxDistFunc)gts_point_segment_distance, delta,
                                            boundary_range);
      gts_bb_tree_destroy(boundary_tree, TRUE);
    } else
      gts_range_reset(boundary_range);
  } else {
    gts_range_reset(face_range);
    gts_range_reset(boundary_range);
  }
}

static void surface_boundary(GtsEdge *e, gpointer *data) {
  GSList **list = data[0];

  if (gts_edge_is_boundary(e, data[1]))
    *list = g_slist_prepend(*list, e);
}

/**
 * gts_surface_boundary:
 * @surface: a #GtsSurface.
 *
 * Returns: a list of #GtsEdge boundary of @surface.
 */
GSList *gts_surface_boundary(GtsSurface *surface) {
  GSList *list = NULL;
  gpointer data[2];

  g_return_val_if_fail(surface != NULL, NULL);

  data[0] = &list;
  data[1] = surface;
  gts_surface_foreach_edge(surface, (GtsFunc)surface_boundary, data);

  return list;
}

struct _GtsSurfaceTraverse {
  GtsFifo *q;
  GtsSurface *s;
};

/**
 * gts_surface_traverse_new:
 * @s: a #GtsSurface.
 * @f: a #GtsFace belonging to @s.
 *
 * Returns: a new #GtsSurfaceTraverse, initialized to start traversing
 * from face @f of surface @s.
 */
GtsSurfaceTraverse *gts_surface_traverse_new(GtsSurface *s, GtsFace *f) {
  GtsSurfaceTraverse *t;

  g_return_val_if_fail(s != NULL, NULL);
  g_return_val_if_fail(f != NULL, NULL);
  g_return_val_if_fail(gts_face_has_parent_surface(f, s), NULL);

  t = g_malloc(sizeof(GtsSurfaceTraverse));
  t->q = gts_fifo_new();
  t->s = s;
  GTS_OBJECT(f)->reserved = GUINT_TO_POINTER(1);
  gts_fifo_push(t->q, f);
  return t;
}

static void push_neighbor(GtsFace *v, gpointer *data) {
  if (!GTS_OBJECT(v)->reserved) {
    GTS_OBJECT(v)->reserved = GUINT_TO_POINTER(GPOINTER_TO_UINT(GTS_OBJECT(data[1])->reserved) + 1);
    gts_fifo_push(data[0], v);
  }
}

/**
 * gts_surface_traverse_next:
 * @t: a #GtsSurfaceTraverse.
 * @level: a pointer to a guint or %NULL.
 *
 * Returns: the next face of the traversal in breadth-first order or
 * %NULL if no faces are left. If @level if not %NULL, it is filled
 * with the level of the returned face (0 for the initial face, 1 for
 * its neighbors and so on).
 */
GtsFace *gts_surface_traverse_next(GtsSurfaceTraverse *t, guint *level) {
  GtsFace *u;

  g_return_val_if_fail(t != NULL, NULL);

  u = gts_fifo_pop(t->q);
  if (u) {
    gpointer data[2];

    if (level)
      *level = GPOINTER_TO_UINT(GTS_OBJECT(u)->reserved);
    data[0] = t->q;
    data[1] = u;
    gts_face_foreach_neighbor(u, t->s, (GtsFunc)push_neighbor, data);
  }
  return u;
}

/**
 * gts_surface_traverse_destroy:
 * @t: a #GtsSurfaceTraverse.
 *
 * Frees all the memory allocated for @t.
 */
void gts_surface_traverse_destroy(GtsSurfaceTraverse *t) {
  g_return_if_fail(t != NULL);

  gts_surface_foreach_face(t->s, (GtsFunc)gts_object_reset_reserved, NULL);
  gts_fifo_destroy(t->q);
  g_free(t);
}
