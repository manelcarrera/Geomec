#include "SurfaceDetail.h"
#include <math.h>


#define HEAP_INSERT_EDGE(h, e) (GTS_OBJECT (e)->reserved = gts_eheap_insert (h, e))
#define HEAP_REMOVE_EDGE(h, e) (gts_eheap_remove (h, GTS_OBJECT (e)->reserved),\
                                GTS_OBJECT (e)->reserved = NULL)


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//POINT IN POLY/////////////////////////////////////////////////////////////////////////////////////////////////////////
int PointInPoly(int npol, double *xp, double *yp, double x, double y, int IncLines)
{
	int i;
	int j;
	int c1 = 0;
	int c2 = 0;

	// trace line to the right
	for (i = 0, j = npol-1; i < npol; j = i++)
	{
		if(yp[i] == yp[j] && y == yp[i] && ((x <= xp[i] && x >= xp[j]) || (x >= xp[i] && x <= xp[j])))
		{
			// point is on horizontal line
			return IncLines;
		}
		if( (((yp[i] <= y) && (y < yp[j])) ||
			((yp[j] <= y) && (y < yp[i]))) &&
			(x < (xp[j] - xp[i]) * (y - yp[i]) / (yp[j] - yp[i]) + xp[i]))
		{
			c1 = !c1;
		}
	}
	
	// trace line to the left
	for (i = 0, j = npol-1; i < npol; j = i++)
	{
		if((((yp[i] <= y) && (y < yp[j])) ||
			((yp[j] <= y) && (y < yp[i]))) &&
			(x > (xp[j] - xp[i]) * (y - yp[i]) / (yp[j] - yp[i]) + xp[i]))
		c2 = !c2;
	}
	
	if( c1 != c2 ) return IncLines; // point is on a line
	
	return c1;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//COARSENING////////////////////////////////////////////////////////////////////////////////////////////////////////////
static GtsVertex *EdgeCollapse (GtsEdge *e,
									GtsEHeap *heap,
									GtsCoarsenFunc coarsen_func,
									gpointer coarsen_data,
									GtsVertexClass *klass,
									gdouble maxcosine2)
{
	GSList *i;
	GtsVertex  *v1 = GTS_SEGMENT (e)->v1;
	GtsVertex *v2 = GTS_SEGMENT (e)->v2;
	GtsVertex *mid;
	
	/* if the edge is degenerate (i.e. v1 == v2), destroy and return */
	if (v1 == v2) 
	{
		gts_object_destroy (GTS_OBJECT (e));
		return NULL;
	}

	if (!gts_edge_collapse_is_valid (e)) 
	{
		GTS_OBJECT (e)->reserved = 	gts_eheap_insert_with_key (heap, e, G_MAXDOUBLE);
		return NULL;
	}

	mid = (*coarsen_func) (e, klass, coarsen_data);

	/* In order to make sure that the boundary of the surface is not altered, the following check is performed.*/
	if( (gts_vertex_is_boundary (GTS_SEGMENT (e)->v1, NULL)) || (gts_vertex_is_boundary (GTS_SEGMENT (e)->v2, NULL)) )
	{
		GTS_OBJECT (e)->reserved = 
		gts_eheap_insert_with_key (heap, e, G_MAXDOUBLE);
		return NULL;
	}


	if (gts_edge_collapse_creates_fold (e, mid, maxcosine2)) 
	{
		GTS_OBJECT (e)->reserved = 
		gts_eheap_insert_with_key (heap, e, G_MAXDOUBLE);
		gts_object_destroy (GTS_OBJECT (mid));
		return NULL;
	}

	gts_object_destroy (GTS_OBJECT (e));

	gts_vertex_replace (v1, mid);
	gts_object_destroy (GTS_OBJECT (v1));
	gts_vertex_replace (v2, mid);
	gts_object_destroy (GTS_OBJECT (v2));

	/* destroy duplicate edges */
	i = mid->segments;
	while (i) 
	{
		GtsEdge * e1 = i->data;
		GtsEdge * duplicate;
		while ((duplicate = gts_edge_is_duplicate (e1))) 
		{
			gts_edge_replace (duplicate, GTS_EDGE (e1));
			HEAP_REMOVE_EDGE (heap, duplicate);
			gts_object_destroy (GTS_OBJECT (duplicate));
		}

		i = i->next;

		if (!e1->triangles) 
		{
			/* e1 is the result of the collapse of one edge of a pair of identical
			faces (it should not happen unless duplicate triangles are present in
			the initial surface) */
			g_warning ("file %s: line %d (%s): probably duplicate triangle.",
				__FILE__, __LINE__, G_GNUC_PRETTY_FUNCTION);
			HEAP_REMOVE_EDGE (heap, e1);
			gts_object_destroy (GTS_OBJECT (e1));
		if (i == NULL) /* mid has been destroyed */
			mid = NULL;
		}
	}

	return mid;
}



static void Update2ndClosestNeighbors (GtsVertex *v, GtsEHeap *heap)
{
	GSList *i = v->segments;
	GSList *list = NULL;
  
	while (i) 
	{
		GtsSegment *s = i->data;
		if (GTS_IS_EDGE (s)) 
		{
			GtsVertex *v1 = s->v1 == v ? s->v2 : s->v1;
			GSList *j = v1->segments;
			while (j) 
			{
				GtsSegment *s1 = j->data;
				if (GTS_IS_EDGE (s1) && !g_slist_find (list, s1))
					list = g_slist_prepend (list, s1);
	
				j = j->next;
			}
		}
    
		i = i->next;
	}

	i = list;
	
	while (i) 
	{
		GtsEdge *e = i->data;

		HEAP_REMOVE_EDGE (heap, e);
		HEAP_INSERT_EDGE (heap, e);
		i = i->next;
	}
   
	g_slist_free (list);
}

static gdouble EdgeLength2 (GtsEdge *e)
{
	return gts_point_distance2 (GTS_POINT (GTS_SEGMENT (e)->v1), 
			      GTS_POINT (GTS_SEGMENT (e)->v2));
}

static void CreateHeapCoarsen(GtsEdge *e, GtsEHeap *heap)
{
	HEAP_INSERT_EDGE (heap, e);
}

//An implementation of the stop function...//////////////////////////////////
gboolean CoarsenStopFunc(gdouble cost,
				         guint nedge,
						 gpointer data)
{
	UNUSED(nedge);

	if(cost > *( (double*)data ) )
		return TRUE;
	
	return FALSE;
}
/////////////////////////////////////////////////////////////////////////////

static Region *FindRegionCoarsen(GtsEdge *e, GtsEHeap *heap, GSList *regions_list, GtsSurface *surface)
{
	Region *region_temp;
	GSList *l = regions_list;
	UNUSED(surface);
	l = l->next;
	while(l)
	{
		region_temp = l->data;
		if( PointInPoly(region_temp->nPoint, region_temp->dX, region_temp->dY, e->segment.v1->p.x, e->segment.v1->p.y, 0) &&
			PointInPoly(region_temp->nPoint, region_temp->dX, region_temp->dY, e->segment.v2->p.x, e->segment.v2->p.y, 0)) 
			return region_temp;
		l = l->next;
	}

	GTS_OBJECT (e)->reserved = 
      gts_eheap_insert_with_key (heap, e, G_MAXDOUBLE);

	return NULL;
}

void RegionCoarsen (GtsSurface *surface,
						 GtsKeyFunc cost_func,
						 gpointer cost_data,
						 GtsCoarsenFunc coarsen_func,
						 gpointer coarsen_data,
						 GtsStopFunc stop_func,
						 GSList *regions_list,
						 gdouble minangle)
{
	GtsEHeap *heap;
	GtsEdge *e;
	GtsVertex *v;
	gdouble top_cost;
	gdouble maxcosine2;
	Region *region_temp;

	g_return_if_fail (surface != NULL);
	g_return_if_fail (stop_func != NULL);

	if (cost_func == NULL)
		cost_func = (GtsKeyFunc) EdgeLength2;
	if (coarsen_func == NULL)
		coarsen_func = (GtsCoarsenFunc) gts_segment_midvertex;

	heap = gts_eheap_new (cost_func, cost_data);
	maxcosine2 = cos (minangle); maxcosine2 *= maxcosine2;

	gts_eheap_freeze (heap);
	gts_surface_foreach_edge (surface, (GtsFunc) CreateHeapCoarsen, heap);
	gts_eheap_thaw (heap);
	/* we want to control edge destruction manually */
	gts_allow_floating_edges = TRUE;
	while ((e = gts_eheap_remove_top (heap, &top_cost)) && (top_cost < G_MAXDOUBLE))
	{
		if((region_temp = FindRegionCoarsen(e, heap, regions_list, surface)) && !(*stop_func) (top_cost, gts_eheap_size (heap) - 
			gts_edge_face_number (e, surface), &(region_temp->stop_data)))
		{
			v = EdgeCollapse (e, heap, coarsen_func, coarsen_data,
				surface->vertex_class, maxcosine2);

			if (v != NULL)
			{
				Update2ndClosestNeighbors (v, heap);
			}
		}
	}

	gts_allow_floating_edges = FALSE;

	/* set reserved field of remaining edges back to NULL */
	if (e) 
		GTS_OBJECT (e)->reserved = NULL;
	
	gts_eheap_foreach (heap, (GFunc) gts_object_reset_reserved, NULL);
	gts_eheap_destroy (heap);
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//REFINEMENT////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void midvertex_insertion (GtsEdge *e,
								 GtsSurface *surface,
								 GtsHeap *heap,
								 GtsVertexClass *vertex_class,
								 GtsEdgeClass *edge_class)
{
	GtsVertex *midvertex;
	GtsEdge *e1;
	GtsEdge *e2;
	GSList *i;

	midvertex = gts_segment_midvertex (GTS_SEGMENT (e), vertex_class);

	e1 = gts_edge_new (edge_class, GTS_SEGMENT (e)->v1, midvertex);
	gts_heap_insert (heap, e1);
	e2 = gts_edge_new (edge_class, GTS_SEGMENT (e)->v2, midvertex);
	gts_heap_insert (heap, e2);
  
	/* creates new faces and modifies old ones */
	i = e->triangles;
	while (i) 
	{
		GtsTriangle *t = i->data;
		GtsVertex *v1;
		GtsVertex *v2;
		GtsVertex *v3;
		GtsEdge *te2;
		GtsEdge *te3;
		GtsEdge *ne;
		GtsEdge *tmp;

		gts_triangle_vertices_edges (t, e, &v1, &v2, &v3, &e, &te2, &te3);
		ne = gts_edge_new (edge_class, midvertex, v3);
		gts_heap_insert (heap, ne);
		if (GTS_SEGMENT (e1)->v1 == v2) 
		{
			tmp = e1; 
			e1 = e2; 
			e2 = tmp;
		}
		e1->triangles = g_slist_prepend (e1->triangles, t);
		ne->triangles = g_slist_prepend (ne->triangles, t);
		te2->triangles = g_slist_remove (te2->triangles, t);
		t->e1 = e1; t->e2 = ne; t->e3 = te3;
		gts_surface_add_face (surface, 
			  gts_face_new (surface->face_class, e2, te2, ne));
		i = i->next;
	}
	/* destroys edge */
	g_slist_free (e->triangles);
	e->triangles = NULL;
	gts_object_destroy (GTS_OBJECT (e));
}

static void CreateHeapRefine (GtsEdge *e, GtsHeap *heap)
{
	gts_heap_insert (heap, e);
}

static gint EdgeIsLonger (GtsSegment *s1, GtsSegment *s2)
{
	if (gts_point_distance2 (GTS_POINT (s1->v1), GTS_POINT (s1->v2)) >
		gts_point_distance2 (GTS_POINT (s2->v1), GTS_POINT (s2->v2)))
			return -1;

	return 1;
}


//An implementation of the stopfunction///////////////////////////////////
gboolean RefineStopFunc(gdouble cost,
						guint nedge,
						gpointer data)
{
	UNUSED(nedge);

	if(cost < *( (double*)data ) )
		return TRUE;
	
	return FALSE;
}
//////////////////////////////////////////////////////////////////////////

static Region *FindRegionRefine(GtsEdge *e, GSList *regions_list)
{
	Region *region_temp;
	GSList *l = regions_list;
	l = l->next;
	while(l)
	{
		region_temp = l->data;
		if( PointInPoly(region_temp->nPoint, region_temp->dX, region_temp->dY, e->segment.v1->p.x, e->segment.v1->p.y, 0) &&
			PointInPoly(region_temp->nPoint, region_temp->dX, region_temp->dY, e->segment.v2->p.x, e->segment.v2->p.y, 0))
			return region_temp;
		l = l->next;
	}

	return NULL;
}

void RegionRefine (GtsSurface *surface,
				   GCompareFunc compare_func,
				   GtsStopFunc stop_func,
				   GSList *regions_list)
{
	GtsHeap *heap;
	GtsEdge *e;
	Region *region_temp;

	g_return_if_fail (surface != NULL);
	g_return_if_fail (stop_func != NULL);

	if (compare_func == NULL)
		compare_func = (GCompareFunc) EdgeIsLonger;

	heap = gts_heap_new (compare_func);
	gts_heap_freeze (heap);
	gts_surface_foreach_edge (surface, (GtsFunc)CreateHeapRefine, heap);
	gts_heap_thaw (heap);
	while ((e = gts_heap_remove_top (heap)))
	{
		if((region_temp = FindRegionRefine(e, regions_list)) && !(*stop_func) (gts_point_distance (GTS_POINT (GTS_SEGMENT (e)->v1), 
					    GTS_POINT (GTS_SEGMENT (e)->v2)),
						gts_heap_size (heap) + 
						gts_edge_face_number (e, surface) + 2,
						&(region_temp->stop_data)))
		{
			midvertex_insertion (e, surface, heap, 
								 surface->vertex_class, surface->edge_class);
		}
	}

	gts_heap_destroy (heap);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

GtsSurface *SurfaceDetail(GtsSurface *surface, 
				   GtsKeyFunc cost_func_coarsen,
				   gpointer cost_data_coarsen,
				   GtsCoarsenFunc coarsen_func,
				   gpointer coarsen_data,
				   GtsStopFunc stop_func_coarsen,
				   gdouble minangle_coarsen,
				   GCompareFunc compare_func_refine,
				   GtsStopFunc stop_func_refine,
				   GSList *regions_list)
{
	GtsSurface *surfacetemp;
	surfacetemp = gts_surface_new(gts_surface_class(),
					gts_face_class(),
					gts_edge_class(),
					gts_vertex_class());

	gts_surface_copy(surfacetemp, surface);

	RegionRefine(surfacetemp, compare_func_refine, stop_func_refine, regions_list);
	RegionCoarsen(surfacetemp, cost_func_coarsen, cost_data_coarsen,
						coarsen_func, coarsen_data, stop_func_coarsen, regions_list, minangle_coarsen);

	return surfacetemp;
}

