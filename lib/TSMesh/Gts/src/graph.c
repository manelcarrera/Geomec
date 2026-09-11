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
#include <stdlib.h>
#include "gts.h"

/* GtsGNode */

gboolean gts_allow_floating_gnodes = FALSE;

static void gnode_remove_container (GtsContainee * i, GtsContainer * c)
{
  (* GTS_CONTAINEE_CLASS (GTS_OBJECT_CLASS (gts_gnode_class ())->parent_class)->remove_container) (i, c);
  if (GTS_SLIST_CONTAINEE (i)->containers == NULL && !gts_allow_floating_gnodes)
  gts_object_destroy (GTS_OBJECT (i));
}

static void gnode_class_init (GtsGNodeClass * klass)
{
  klass->weight = NULL;

  GTS_CONTAINEE_CLASS (klass)->remove_container = gnode_remove_container;
}

static void gnode_init (GtsGNode * n)
{
  n->level = 0;
}

/**
 * gts_gnode_class:
 * 
 * Returns: the #GtsGNodeClass.
 */
GtsGNodeClass * gts_gnode_class (void)
{
  static GtsGNodeClass * klass = NULL;

  if (klass == NULL) {
  GtsObjectClassInfo gnode_info = {
      "GtsGNode",
      sizeof (GtsGNode),
      sizeof (GtsGNodeClass),
      (GtsObjectClassInitFunc) gnode_class_init,
      (GtsObjectInitFunc) gnode_init,
      (GtsArgSetFunc) NULL,
      (GtsArgGetFunc) NULL
  };
  klass = 
      gts_object_class_new (GTS_OBJECT_CLASS (gts_slist_container_class ()),
        &gnode_info);
  }

  return klass;
}

/**
 * gts_gnode_new:
 * @klass: a #GtsGNodeClass.
 *
 * Returns: a new #GtsGNode.
 */
GtsGNode * gts_gnode_new (GtsGNodeClass * klass)
{
  GtsGNode * object;

  object = GTS_GNODE (gts_object_new (GTS_OBJECT_CLASS (klass)));

  return object;
}

/**
 * gts_gnode_foreach_neighbor:
 * @n: a #GtsGNode.
 * @g: a #GtsGraph or %NULL.
 * @func: a #GtsFunc.
 * @data: user data to be passed to @func.
 *
 * Calls @func for each neighbor #GtsGNode of @n (belonging to @g if
 * @g is not %NULL.  
 */
void gts_gnode_foreach_neighbor (GtsGNode * n, 
         GtsGraph * g,
         GtsFunc func,
         gpointer data)
{
  GSList * i;

  g_return_if_fail (n != NULL);
  g_return_if_fail (func != NULL);

  i = GTS_SLIST_CONTAINER (n)->items;
  while (i) {
  GtsGNode * n1 = GTS_GNODE_NEIGHBOR (n, i->data);
  if (g == NULL || gts_containee_is_contained (GTS_CONTAINEE (n1),
             GTS_CONTAINER (g)))
      (* func) (n1, data);
  i = i->next;
  }
}

/**
 * gts_gnode_foreach_edge:
 * @n: a #GtsGNode.
 * @g: a #GtsGraph or %NULL.
 * @func: a #GtsFunc.
 * @data: user data to be passed to @func.
 *
 * Calls @func for each #GtsGEdge connecting @n to another #GtsGNode
 * (belonging to @g if @g is not %NULL.  
 */
void gts_gnode_foreach_edge (GtsGNode * n, 
           GtsGraph * g,
           GtsFunc func,
           gpointer data)
{
  GSList * i;

  g_return_if_fail (n != NULL);
  g_return_if_fail (func != NULL);

  i = GTS_SLIST_CONTAINER (n)->items;
  while (i) {
  GtsGNode * n1 = GTS_GNODE_NEIGHBOR (n, i->data);
  if (g == NULL || gts_containee_is_contained (GTS_CONTAINEE (n1),
             GTS_CONTAINER (g)))
      (* func) (i->data, data);
  i = i->next;
  }
}

/**
 * gts_gnode_degree:
 * @n: a #GtsGNode.
 * @g: a #GtsGraph or %NULL.
 *
 * Returns: the number of neighbors of @n (belonging to @g if @g is not %NULL).
 */
guint gts_gnode_degree (GtsGNode * n,
      GtsGraph * g)
{
  GSList * i;
  guint nn = 0;

  g_return_val_if_fail (n != NULL, 0);

  i = GTS_SLIST_CONTAINER (n)->items;
  while (i) {
  GtsGNode * n1 = GTS_GNODE_NEIGHBOR (n, i->data);
  if (g == NULL || gts_containee_is_contained (GTS_CONTAINEE (n1),
             GTS_CONTAINER (g)))
      nn++;
  i = i->next;
  }

  return nn;
}

/**
 * gts_gnode_move_cost:
 * @n: a #GtsGNode.
 * @src: a #GtsGraph containing @n.
 * @dst: another #GtsGraph.
 *
 * Returns: the cost (increase in the sum of the weights of the edges cut) of
 * moving @n from @src to @dst.
 */
gfloat gts_gnode_move_cost (GtsGNode * n,
        GtsGraph * src,
        GtsGraph * dst)
{
  GSList * i;
  gfloat cost = 0.;
  
  g_return_val_if_fail (n != NULL, G_MAXFLOAT);
  g_return_val_if_fail (src != NULL, G_MAXFLOAT);
  g_return_val_if_fail (dst != NULL, G_MAXFLOAT);
  g_return_val_if_fail (gts_containee_is_contained (GTS_CONTAINEE (n),
              GTS_CONTAINER (src)),
      G_MAXFLOAT);

  i = GTS_SLIST_CONTAINER (n)->items;
  while (i) {
  GtsGEdge * ge = i->data;
  GtsGNode * neighbor = GTS_GNODE_NEIGHBOR (n, ge);

  if (gts_containee_is_contained (GTS_CONTAINEE (neighbor), 
          GTS_CONTAINER (src)))
      cost += gts_gedge_weight (ge);
  else if (gts_containee_is_contained (GTS_CONTAINEE (neighbor), 
           GTS_CONTAINER (dst)))
      cost -= gts_gedge_weight (ge);
  i = i->next;
  }
  
  return cost;
}

/**
 * gts_gnode_weight:
 * @n: a #GtsGNode.
 *
 * Returns: the weight of @n as defined by the weight() method of the
 * #GtsGNodeClass.  
 */
gfloat gts_gnode_weight (GtsGNode * n)
{
  g_return_val_if_fail (n != NULL, 0.);

  if (GTS_GNODE_CLASS (GTS_OBJECT (n)->klass)->weight)
  return (* GTS_GNODE_CLASS (GTS_OBJECT (n)->klass)->weight) (n);
  return 1.;
}

/* GtsNGNode */

static void ngnode_init (GtsNGNode * n)
{
  n->id = 0;
}

/**
 * gts_ngnode_class:
 *
 * Returns: the #GtsNGNodeClass.
 */
GtsNGNodeClass * gts_ngnode_class (void)
{
  static GtsNGNodeClass * klass = NULL;

  if (klass == NULL) {
  GtsObjectClassInfo ngnode_info = {
      "GtsNGNode",
      sizeof (GtsNGNode),
      sizeof (GtsNGNodeClass),
      (GtsObjectClassInitFunc) NULL,
      (GtsObjectInitFunc) ngnode_init,
      (GtsArgSetFunc) NULL,
      (GtsArgGetFunc) NULL
  };
  klass = gts_object_class_new (GTS_OBJECT_CLASS (gts_gnode_class ()),
          &ngnode_info);
  }

  return klass;
}

/**
 * gts_ngnode_new:
 * @klass: a #GtsNGNodeClass.
 *
 * Returns: a new #GtsNGNode with identity @id.
 */
GtsNGNode * gts_ngnode_new (GtsNGNodeClass * klass,
        guint id)
{
  GtsNGNode * n;

  n = GTS_NGNODE (gts_gnode_new (GTS_GNODE_CLASS (klass)));
  n->id = id;

  return n;
}

/* GtsWGNode */

static gfloat wgnode_weight (GtsGNode * n)
{
  return GTS_WGNODE (n)->weight;
}

static void wgnode_class_init (GtsWGNodeClass * klass)
{
  GTS_GNODE_CLASS (klass)->weight = wgnode_weight;
}

static void wgnode_init (GtsWGNode * n)
{
  n->weight = 1.;
}

/**
 * gts_wgnode_class:
 *
 * Returns: the #GtsWGNodeClass.
 */
GtsWGNodeClass * gts_wgnode_class (void)
{
  static GtsWGNodeClass * klass = NULL;

  if (klass == NULL) {
  GtsObjectClassInfo wgnode_info = {
      "GtsWGNode",
      sizeof (GtsWGNode),
      sizeof (GtsWGNodeClass),
      (GtsObjectClassInitFunc) wgnode_class_init,
      (GtsObjectInitFunc) wgnode_init,
      (GtsArgSetFunc) NULL,
      (GtsArgGetFunc) NULL
  };
  klass = gts_object_class_new (GTS_OBJECT_CLASS (gts_gnode_class ()),
          &wgnode_info);
  }

  return klass;
}

/**
 * gts_wgnode_new:
 * @klass: a #GtsWGNodeClass.
 * @weight: the weight of the #GtsWGNode to create.
 *
 * Returns: a new #GtsWGNode of weight @weight.
 */
GtsWGNode * gts_wgnode_new (GtsWGNodeClass * klass,
        gfloat weight)
{
  GtsWGNode * n;

  n = GTS_WGNODE (gts_gnode_new (GTS_GNODE_CLASS (klass)));
  n->weight = weight;

  return n;
}

/* GtsFNode */

static void fnode_init (GtsFNode * fn)
{
  fn->f = NULL;
}

/**
 * gts_fnode_class:
 *
 * Returns: the #GtsFNodeClass.
 */
GtsFNodeClass * gts_fnode_class (void)
{
  static GtsFNodeClass * klass = NULL;

  if (klass == NULL) {
  GtsObjectClassInfo fnode_info = {
      "GtsFNode",
      sizeof (GtsFNode),
      sizeof (GtsFNodeClass),
      (GtsObjectClassInitFunc) NULL,
      (GtsObjectInitFunc) fnode_init,
      (GtsArgSetFunc) NULL,
      (GtsArgGetFunc) NULL
  };
  klass = gts_object_class_new (GTS_OBJECT_CLASS (gts_gnode_class ()),
          &fnode_info);
  }

  return klass;
}

/**
 * gts_fnode_new:
 * @klass: a #GtsFNodeClass.
 * @f: a #GtsFace.
 *
 * Returns: a new #GtsFNode associated with face @f.
 */
GtsFNode * gts_fnode_new (GtsFNodeClass * klass, GtsFace * f)
{
  GtsFNode * fn;

  g_return_val_if_fail (f != NULL, NULL);

  fn = GTS_FNODE (gts_object_new (GTS_OBJECT_CLASS (klass)));
  fn->f = f;

  return fn;
}

/* GtsGEdge */

static void gedge_destroy (GtsObject * object)
{
  GtsGEdge * ge = GTS_GEDGE (object);

  if (ge->n1)
  gts_container_remove (GTS_CONTAINER (ge->n1), GTS_CONTAINEE (ge));
  if (ge->n2)
  gts_container_remove (GTS_CONTAINER (ge->n2), GTS_CONTAINEE (ge));

  (* GTS_OBJECT_CLASS (gts_gedge_class ())->parent_class->destroy) (object);
}

static void gedge_remove_container (GtsContainee * i, GtsContainer * c)
{
  GtsGEdge * ge = GTS_GEDGE (i);
  GtsGNode * n1 = ge->n1;
  GtsGNode * n2 = ge->n2;

  ge->n1 = ge->n2 = NULL;
  if (n1 != NULL && n2 != NULL) {
  if (GTS_CONTAINER (n1) == c) {
      if (n2) gts_container_remove (GTS_CONTAINER (n2), i);
  }
  else if (GTS_CONTAINER (n2) == c) {
      if (n1) gts_container_remove (GTS_CONTAINER (n1), i);
  }
  else
      g_assert_not_reached ();
  (* GTS_OBJECT_CLASS (gts_gedge_class ())->parent_class->destroy)
      (GTS_OBJECT (i));
  }
}

static gboolean gedge_is_contained (GtsContainee * i, GtsContainer * c)
{
  GtsGEdge * ge = GTS_GEDGE (i);

  if (GTS_CONTAINER (ge->n1) == c || GTS_CONTAINER (ge->n2) == c)
  return TRUE;
  return FALSE;
}

static void gedge_class_init (GtsGEdgeClass * klass)
{
  klass->weight = NULL;

  GTS_CONTAINEE_CLASS (klass)->remove_container = gedge_remove_container;
  GTS_CONTAINEE_CLASS (klass)->is_contained = gedge_is_contained;

  GTS_OBJECT_CLASS (klass)->destroy = gedge_destroy;
}

static void gedge_init (GtsGEdge * object)
{
  object->n1 = object->n2 = NULL;
}

/**
 * gts_gedge_class:
 *
 * Returns: the #GtsGEdgeClass.
 */
GtsGEdgeClass * gts_gedge_class (void)
{
  static GtsGEdgeClass * klass = NULL;

  if (klass == NULL) {
  GtsObjectClassInfo gedge_info = {
      "GtsGEdge",
      sizeof (GtsGEdge),
      sizeof (GtsGEdgeClass),
      (GtsObjectClassInitFunc) gedge_class_init,
      (GtsObjectInitFunc) gedge_init,
      (GtsArgSetFunc) NULL,
      (GtsArgGetFunc) NULL
  };
  klass = gts_object_class_new (GTS_OBJECT_CLASS (gts_containee_class ()),
          &gedge_info);
  }

  return klass;
}

/**
 * gts_gedge_new:
 * @klass: a #GtsGEdgeClass.
 * @n1: a #GtsGNode.
 * @n2: another #GtsGNode.
 *
 * Returns: a new #GtsGEdge linking @n1 and @n2.
 */
GtsGEdge * gts_gedge_new (GtsGEdgeClass * klass, GtsGNode * n1, GtsGNode * n2)
{
  GtsGEdge * object;

  g_return_val_if_fail (n1 != NULL, NULL);
  g_return_val_if_fail (n2 != NULL, NULL);
  g_return_val_if_fail (n1 != n2, NULL);

  object = GTS_GEDGE (gts_object_new (GTS_OBJECT_CLASS (klass)));
  object->n1 = n1;
  gts_container_add (GTS_CONTAINER (n1), GTS_CONTAINEE (object));
  object->n2 = n2;
  gts_container_add (GTS_CONTAINER (n2), GTS_CONTAINEE (object));

  return object;
}

/**
 * gts_gedge_weight:
 * @e: a #GtsGEdge.
 *
 * Returns: the weight of edge @e as defined by the weight() method of
 * #GtsGEdgeClass.  
 */
gfloat gts_gedge_weight (GtsGEdge * e)
{
  g_return_val_if_fail (e != NULL, 0.);

  if (GTS_GEDGE_CLASS (GTS_OBJECT (e)->klass)->weight)
  return (* GTS_GEDGE_CLASS (GTS_OBJECT (e)->klass)->weight) (e);
  return 1.;
}

/* GtsWGEdge */

static gfloat wgedge_weight (GtsGEdge * e)
{
  return GTS_WGEDGE (e)->weight;
}

static void wgedge_class_init (GtsWGEdgeClass * klass)
{
  GTS_GEDGE_CLASS (klass)->weight = wgedge_weight;
}

static void wgedge_init (GtsWGEdge * e)
{
  e->weight = 1.;
}

/**
 * gts_wgedge_class:
 * 
 * Returns: the #GtsWGEdgeClass.
 */
GtsWGEdgeClass * gts_wgedge_class (void)
{
  static GtsWGEdgeClass * klass = NULL;

  if (klass == NULL) {
  GtsObjectClassInfo wgedge_info = {
      "GtsWGEdge",
      sizeof (GtsWGEdge),
      sizeof (GtsWGEdgeClass),
      (GtsObjectClassInitFunc) wgedge_class_init,
      (GtsObjectInitFunc) wgedge_init,
      (GtsArgSetFunc) NULL,
      (GtsArgGetFunc) NULL
  };
  klass = gts_object_class_new (GTS_OBJECT_CLASS (gts_gedge_class ()),
          &wgedge_info);
  }

  return klass;
}

/**
 * gts_wgedge_new:
 * @klass: a #GtsWGEdgeClass.
 * @n1: a #GtsGNode.
 * @n2: another #GtsGNode.
 * @weight: the weight of the new edge.
 *
 * Returns: a new #GtsWGEdge of weight @weight linking @n1 and @n2.
 */ 
GtsWGEdge * gts_wgedge_new (GtsWGEdgeClass * klass,
        GtsGNode * g1,
        GtsGNode * g2,
        gfloat weight)
{
  GtsWGEdge * we;

  we = GTS_WGEDGE (gts_gedge_new (GTS_GEDGE_CLASS (klass), g1, g2));
  we->weight = weight;

  return we;
}

/* GtsGraph */

static void graph_init (GtsGraph * g)
{
  g->node_class = gts_gnode_class ();
  g->edge_class = gts_gedge_class ();
}

static void graph_class_init (GtsGraphClass * klass)
{
  klass->weight = NULL;
}

/**
 * gts_graph_class:
 *
 * Returns: the #GtsGraphClass.
 */
GtsGraphClass * gts_graph_class (void)
{
  static GtsGraphClass * klass = NULL;

  if (klass == NULL) {
  GtsObjectClassInfo graph_info = {
      "GtsGraph",
      sizeof (GtsGraph),
      sizeof (GtsGraphClass),
      (GtsObjectClassInitFunc) graph_class_init,
      (GtsObjectInitFunc) graph_init,
      (GtsArgSetFunc) NULL,
      (GtsArgGetFunc) NULL
  };
  klass = gts_object_class_new (GTS_OBJECT_CLASS (gts_hash_container_class ()),
          &graph_info);
  }

  return klass;
}

/**
 * gts_graph_new:
 * @klass: a #GtsGraphClass.
 * @node_class: a #GtsGNodeClass.
 * @edge_class: a #GtsGEdgeClass.
 *
 * Returns: a new #GtsGraph using @node_class and @edge_class as node types.
 */
GtsGraph * gts_graph_new (GtsGraphClass * klass,
        GtsGNodeClass * node_class,
        GtsGEdgeClass * edge_class)
{
  GtsGraph * g;

  g_return_val_if_fail (klass != NULL, NULL);
  g_return_val_if_fail (node_class != NULL, NULL);
  g_return_val_if_fail (edge_class != NULL, NULL);

  g = GTS_GRAPH (gts_object_new (GTS_OBJECT_CLASS (klass)));

  return g;
}

static void compute_degree (GtsGNode * n, gpointer * data)
{
  GtsGraph * g = data[0];
  GtsRange * degree = data[1];

  gts_range_add_value (degree, (gdouble)gts_gnode_degree (n, g));
}

/**
 * gts_graph_print_stats:
 * @g: a #GtsGraph.
 * @fp: a file pointer.
 *
 * Writes to @fp a summary of the properties of @g.
 */
void gts_graph_print_stats (GtsGraph * g, FILE * fp)
{
  GtsRange degree;
  gpointer data[2];

  g_return_if_fail (g != NULL);
  g_return_if_fail (fp != NULL);

  fprintf (fp, "# nodes: %lld weight: %g\n", 
     gts_container_size (GTS_CONTAINER (g)),
     gts_graph_weight (g));
  fprintf (fp, "#   degree: ");
  gts_range_init (&degree);
  data[0] = g;
  data[1] = &degree;
  gts_container_foreach (GTS_CONTAINER (g), (GtsFunc) compute_degree, data);
  gts_range_update (&degree);
  gts_range_print (&degree, fp);
  fprintf (fp, "\n");
  fprintf (fp, "#   edges cut: %lld edges cut weight: %g\n", 
     gts_graph_edges_cut (g),
     gts_graph_edges_cut_weight (g));
}

struct _GtsGraphTraverse {
  GtsFifo * q;
  GtsGraph * g;
};

static void reset_level (GtsGNode * n)
{
  n->level = 0;
}

/**
 * gts_graph_traverse_new:
 * @g: a #GtsGraph.
 * @n: a #GtsGNode belonging to @g.
 * @type: the type of traversal.
 * @reinit: if %TRUE, the traversal is reinitialized.
 *
 * Returns: a new #GtsGraphTraverse initialized for the traversal of
 * @g of type @type, starting from @n.  
 */
GtsGraphTraverse * gts_graph_traverse_new (GtsGraph * g, 
             GtsGNode * n,
             GtsTraverseType type,
             gboolean reinit)
{
  GtsGraphTraverse * t;

  UNUSED(type);

  g_return_val_if_fail (g != NULL, NULL);
  g_return_val_if_fail (n != NULL, NULL);
  g_return_val_if_fail (gts_containee_is_contained (GTS_CONTAINEE (n), 
              GTS_CONTAINER (g)), 
      NULL);

  if (reinit)
  gts_container_foreach (GTS_CONTAINER (g), (GtsFunc) reset_level, NULL);

  t = g_malloc (sizeof (GtsGraphTraverse));
  t->q = gts_fifo_new ();
  t->g = g;
  n->level = 1;
  gts_fifo_push (t->q, n);

  return t;
}

static void push_neighbor (GtsGNode * n, gpointer * data)
{
  GtsFifo * q = data[0];
  GtsGNode * u = data[1];

  if (n->level == 0) {
  n->level = u->level + 1;
  gts_fifo_push (q, n);
  }
}

/**
 * gts_graph_traverse_next:
 * @t: a #GtsGraphTraverse.
 *
 * Returns: the next #GtsGNode of the traversal defined by @t or %NULL
 * if the traversal is complete.
 */
GtsGNode * gts_graph_traverse_next (GtsGraphTraverse * t) 
{ 
  GtsGNode * u;

  g_return_val_if_fail (t != NULL, NULL);

  u = gts_fifo_pop (t->q);
  if (u) {
  gpointer data[2];

  data[0] = t->q;
  data[1] = u;
  gts_gnode_foreach_neighbor (u, t->g, (GtsFunc) push_neighbor, data);
  }
  
  return u;
}

/**
 * gts_graph_traverse_what_next:
 * @t: a #GtsGraphTraverse.
 *
 * Returns: the next #GtsGNode of the traversal defined by @t or %NULL
 * if the traversal is complete but without advancing the traversal.
 */
GtsGNode * gts_graph_traverse_what_next (GtsGraphTraverse * t)
{
  g_return_val_if_fail (t != NULL, NULL);

  return gts_fifo_top (t->q);
}

/**
 * gts_graph_traverse_destroy:
 * @t: a #GtsGraphTraverse.
 *
 * Frees all the memory allocated for @t.
 */
void gts_graph_traverse_destroy (GtsGraphTraverse * t)
{
  g_return_if_fail (t != NULL);

  gts_fifo_destroy (t->q);
  g_free (t);
}

static void edge_foreach_node (GtsGNode * n, gpointer * info)
{
  GtsFunc func = info[0];
  gpointer data = info[1];
  GHashTable * hash = info[2];
  GSList * i = GTS_SLIST_CONTAINER (n)->items;

  while (i) {
  GtsGEdge * e = i->data;
  if (!g_hash_table_lookup (hash, e)) {
      (* func) (e, data);
      g_hash_table_insert (hash, e, e);
  }
  i = i->next;
  }  
}

/**
 * gts_graph_foreach_edge:
 * @g: a #GtsGraph.
 * @func: a #GtsFunc.
 * @data: user data to be passed to @func.
 *
 * Calls @func for each #GtsEdge of @g.
 */
void gts_graph_foreach_edge (GtsGraph * g, GtsFunc func, gpointer data)
{
  gpointer info[3];
  GHashTable * hash;

  g_return_if_fail (g != NULL);
  g_return_if_fail (func != NULL);

  info[0] = func;
  info[1] = data;
  info[2] = hash = g_hash_table_new (NULL, NULL);
  gts_container_foreach (GTS_CONTAINER (g), (GtsFunc) edge_foreach_node, info);
  g_hash_table_destroy (hash);
}

/**
 * gts_graph_weight:
 * @g: a #GtsGraph.
 *
 * Returns: the weight of graph @g as defined by the weight() method
 * of #GtsGraphClass. 
 */
gfloat gts_graph_weight (GtsGraph * g)
{
  g_return_val_if_fail (g != NULL, 0.);

  if (GTS_GRAPH_CLASS (GTS_OBJECT (g)->klass)->weight)
  return (* GTS_GRAPH_CLASS (GTS_OBJECT (g)->klass)->weight) (g);
  return (gfloat)gts_container_size (GTS_CONTAINER (g));
}

/**
 * gts_graph_distance_sum:
 * @g: a #GtsGraph.
 * @center: a #GtsGNode of @g.
 *
 * Returns: the sum of the distances between all the other #GtsGNode
 * of @g and @center.  
 */
guint gts_graph_distance_sum (GtsGraph * g, GtsGNode * center)
{
  GtsGraphTraverse * t;
  GtsGNode * n;
  guint sum = 0;

  g_return_val_if_fail (g != NULL, 0);
  g_return_val_if_fail (center != NULL, 0);

  t = gts_graph_traverse_new (g, center, GTS_BREADTH_FIRST, TRUE);
  while ((n = gts_graph_traverse_next (t)))
  sum += n->level - 1;
  gts_graph_traverse_destroy (t);

  return sum;
}

/**
 * gts_graph_farthest:
 * @g: a #GtsGraph.
 * @gnodes: a list of #GtsGNode belonging to @g.
 *
 * Returns: the #GtsGNode belonging to @g and farthest from all the nodes in
 * @gnodes (hmmm, definition of "farthest"?).
 */
GtsGNode * gts_graph_farthest (GtsGraph * g, GSList * gnodes)
{
  GtsGNode * farthest = NULL;
  GSList * i;
  gboolean reinit = TRUE, changed = TRUE;
  guint level = 1;

  g_return_val_if_fail (g != NULL, NULL);

  /* initialize traversals */
  i = gnodes;
  while (i) {
  GTS_OBJECT (i->data)->reserved = 
      gts_graph_traverse_new (g, i->data, GTS_BREADTH_FIRST, reinit);
  reinit = FALSE;
  i = i->next;
  }

  while (changed) {
  changed = FALSE;
  i = gnodes;
  while (i) {
      GtsGraphTraverse * t = GTS_OBJECT (i->data)->reserved;
      GtsGNode * n;
      while ((n = gts_graph_traverse_what_next (t)) && n->level == level) {
  changed = TRUE;
  farthest = n;
  gts_graph_traverse_next (t);
      }
      i = i->next;
  }
  level++;
  }

  /* destroy traversals */
  i = gnodes;
  while (i) {
  gts_graph_traverse_destroy (GTS_OBJECT (i->data)->reserved);
  GTS_OBJECT (i->data)->reserved = NULL;
  i = i->next;
  }
  return farthest;
}

static void neighbor_count (GtsGNode * n, gpointer * data)
{
  guint * cuts = data[0];
  GtsGraph * g = data[1];
  
  if (!gts_containee_is_contained (GTS_CONTAINEE (n), GTS_CONTAINER (g)))
  (*cuts)++;
}

static void count_edge_cuts (GtsGNode * n, gpointer * data)
{
  gts_gnode_foreach_neighbor (n, NULL, (GtsFunc) neighbor_count, data);
}

/**
 * gts_graph_edges_cut:
 * @g: a #GtsGraph.
 *
 * Returns: the number of edges of @g connecting nodes belonging to @g to nodes
 * not belonging to @g.
 */
guint gts_graph_edges_cut (GtsGraph * g)
{
  guint cuts = 0;
  gpointer data[2];

  g_return_val_if_fail (g != NULL, 0);

  data[0] = &cuts;
  data[1] = g;
  gts_container_foreach (GTS_CONTAINER (g), (GtsFunc) count_edge_cuts, data);

  return cuts;
}

static void sum_edge_cuts_weight (GtsGNode * n, gpointer * data)
{
  gfloat * weight = data[0];
  GtsGraph * g = data[1];
  GSList * i = GTS_SLIST_CONTAINER (n)->items;

  while (i) {
  GtsGNode * n1 = GTS_GNODE_NEIGHBOR (n, i->data);
  if (!gts_containee_is_contained (GTS_CONTAINEE (n1), GTS_CONTAINER (g)))
      *weight += gts_gedge_weight (i->data);
  i = i->next;
  }
}

/**
 * gts_graph_edges_cut_weight:
 * @g: a #GtsGraph.
 *
 * Returns: the sum of the weights of the edges of @g connecting nodes
 * belonging to @g to nodes not belonging to @g.
 */
gfloat gts_graph_edges_cut_weight (GtsGraph * g)
{
  gfloat weight = 0.;
  gpointer data[2];

  g_return_val_if_fail (g != NULL, 0);

  data[0] = &weight;
  data[1] = g;
  gts_container_foreach (GTS_CONTAINER (g), (GtsFunc) sum_edge_cuts_weight, 
       data);

  return weight;
}

/**
 * gts_graph_read:
 * @g: a #GtsGraph.
 * @fp: a file pointer.
 *
 * Adds to @g the nodes and edges defined in the file pointed to by
 * @fp. This file must use the Jostle "graph" ASCII format.  
 * The nodes created are of type #GtsNGNode and their identities are the
 * line number at which they appear in @fp.
 *
 * Returns: 0 if the lecture was successful, the line number at which
 * an error occured otherwise.  
 */
guint gts_graph_read (GtsGraph * g, FILE * fp)
{
  guint nn, ne, line_number = 1, n;
  GString * buf;
  gint delim;
  GtsGNode ** nodes;
  gboolean syntax_error = FALSE;

  g_return_val_if_fail (g != NULL, 1);
  g_return_val_if_fail (fp != NULL, 1);

  buf = g_string_new ("");

  delim = gts_get_token (fp, buf, " \t\n", "");
  if (delim == EOF || delim == '\n') {
  g_string_free (buf, TRUE);
  return 1;
  }
  nn = atoi (buf->str);
  delim = gts_get_token (fp, buf, " \t\n", "");
  if (delim == EOF) {
  g_string_free (buf, TRUE);
  return 1;
  }
  ne = atoi (buf->str);
  if (delim != '\n')
  gts_get_newline (fp);
  nodes = g_malloc (sizeof (GtsGNode *)*(nn + 1));

  n = 0;
  while (n < nn && !syntax_error) {
  GtsNGNode * node = gts_ngnode_new (gts_ngnode_class (), line_number);
  
  nodes[n++] = GTS_GNODE (node);
  gts_container_add (GTS_CONTAINER (g), GTS_CONTAINEE (node));
  line_number++;
  do {
      delim = gts_get_token (fp, buf, " \t\n", "");
      if (delim == EOF)
  syntax_error = TRUE;
      else {
  guint in = atoi (buf->str);
  if (in > nn || in == n || in == 0)
    syntax_error = TRUE;
  else if (in < n) {
    gts_gedge_new (g->edge_class, GTS_GNODE (node), nodes[in - 1]);
    ne--;
  }
      }
  } while (delim != EOF && delim != '\n' && !syntax_error);
  }
  g_free (nodes);
  g_string_free (buf, TRUE);

  if (n != nn || ne > 0 || syntax_error)
  return line_number;  

  return 0;
}

/* GtsWGraph */

static gfloat wgraph_weight (GtsGraph * g)
{
  return GTS_WGRAPH (g)->weight;
}

static void wgraph_add (GtsContainer * g, GtsContainee * n)
{
  GtsWGraph * wg = GTS_WGRAPH (g);
  gfloat w = gts_gnode_weight (GTS_GNODE (n));

  wg->weight += w;
  
  (* GTS_CONTAINER_CLASS (GTS_OBJECT_CLASS (gts_wgraph_class ())->parent_class)->add) (g, n);
}

static void wgraph_remove (GtsContainer * g, GtsContainee * n)
{
  GTS_WGRAPH (g)->weight -= gts_gnode_weight (GTS_GNODE (n));
  
  (* GTS_CONTAINER_CLASS (GTS_OBJECT_CLASS (gts_wgraph_class ())->parent_class)->remove) (g, n);
}

static void wgraph_class_init (GtsWGraphClass * klass)
{
  GTS_GRAPH_CLASS (klass)->weight = wgraph_weight;

  GTS_CONTAINER_CLASS (klass)->add = wgraph_add;
  GTS_CONTAINER_CLASS (klass)->remove = wgraph_remove;
}

static void wgraph_init (GtsWGraph * g)
{
  g->weight = 0.;
}

/**
 * gts_wgraph_class:
 *
 * Returns: the #GtsWGraphClass.
 */
GtsWGraphClass * gts_wgraph_class (void)
{
  static GtsWGraphClass * klass = NULL;

  if (klass == NULL) {
  GtsObjectClassInfo wgraph_info = {
      "GtsWGraph",
      sizeof (GtsWGraph),
      sizeof (GtsWGraphClass),
      (GtsObjectClassInitFunc) wgraph_class_init,
      (GtsObjectInitFunc) wgraph_init,
      (GtsArgSetFunc) NULL,
      (GtsArgGetFunc) NULL
  };
  klass = gts_object_class_new (GTS_OBJECT_CLASS (gts_graph_class ()),
          &wgraph_info);
  }

  return klass;
}

static void weight_max (GtsGNode * n, gfloat * wmax)
{
  gfloat w = gts_gnode_weight (n);

  if (w > *wmax)
  *wmax = w;
}

/**
 * gts_wgraph_weight_max:
 * @wg: a #GtsWGraph.
 *
 * Returns: the maximum weight of any vertices belonging to @g.
 */
gfloat gts_wgraph_weight_max (GtsWGraph * wg)
{
  gfloat wmax = - G_MAXFLOAT;

  g_return_val_if_fail (wg != NULL, 0.);

  gts_container_foreach (GTS_CONTAINER (wg), (GtsFunc) weight_max, &wmax);

  return wmax;
}

/* Surface graph */

static void create_node (GtsFace * f, GtsGraph * graph)
{
  GtsFNode * fn = gts_fnode_new (gts_fnode_class (), f);

  gts_container_add (GTS_CONTAINER (graph), GTS_CONTAINEE (fn));
  GTS_OBJECT (f)->reserved = fn;
}

static void create_edge (GtsEdge * e, GtsSurface * s)
{
  GSList * i = e->triangles;
  
  while (i) {
  GtsFace * f = i->data;
  if (GTS_IS_FACE (f) && gts_face_has_parent_surface (f, s)) {
      GSList * j = i->next;
      while (j) {
  GtsFace * f1 = j->data;
  if (GTS_IS_FACE (f1) && gts_face_has_parent_surface (f1, s))
    gts_gedge_new (gts_gedge_class (), 
       GTS_OBJECT (f)->reserved,
       GTS_OBJECT (f1)->reserved);
  j = j->next;
      }
  }
  i = i->next;
  }
}

/**
 * gts_surface_graph_new:
 * @klass: a #GtsGraphClass.
 * @s: a #GtsSurface.
 *
 * Returns: a new #GtsGraph representing the connectivity of the faces
 * of @s. This graph uses #GtsFGNode as nodes which allows to store
 * the dependencies between nodes and faces of @s.  
 */
GtsGraph * gts_surface_graph_new (GtsGraphClass * klass,
          GtsSurface * s)
{
  GtsGraph * graph;
  
  g_return_val_if_fail (klass != NULL, NULL);
  g_return_val_if_fail (s != NULL, NULL);

  graph = GTS_GRAPH (gts_object_new (GTS_OBJECT_CLASS (klass)));
  gts_surface_foreach_face (s, (GtsFunc) create_node, graph);
  gts_surface_foreach_edge (s, (GtsFunc) create_edge, s);
  gts_surface_foreach_face (s, (GtsFunc) gts_object_reset_reserved, NULL);

  return graph;
}

static void add_to_surface (GtsGNode * n, GtsSurface * s)
{
  if (GTS_IS_FNODE (n))
  gts_surface_add_face (s, GTS_FNODE (n)->f);
}

/**
 * gts_surface_graph_surface:
 * @surface_graph: a #GtsGraph using #GtsFGNode as nodes.
 * @s: a #GtsSurface.
 *
 * Returns: a new #GtsSurface using the same classes as @s and
 * composed of the faces defined by @surface_graph.
 */
GtsSurface * gts_surface_graph_surface (GtsGraph * surface_graph,
          GtsSurface * s)
{
  GtsSurface * s1;

  g_return_val_if_fail (surface_graph != NULL, NULL);
  g_return_val_if_fail (s != NULL, NULL);
  
  s1 = gts_surface_new (GTS_SURFACE_CLASS (GTS_OBJECT (s)->klass),
      s->face_class,
      s->edge_class,
      s->vertex_class);
  gts_container_foreach (GTS_CONTAINER (surface_graph), 
       (GtsFunc) add_to_surface, s1);
  return s1;
}

