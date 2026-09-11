#include <gts.h>

#ifdef __cplusplus
extern "C" {
#endif

struct _Region{
	double * dX;
	double * dY;
	double stop_data;
	int nPoint;
};

typedef struct _Region  Region;

int PointInPoly(int npol, double *xp, double *yp, double x, double y, int IncLines);

gboolean RefineStopFunc(gdouble cost,
					   guint nedge,
					   gpointer data);

void RegionRefine (GtsSurface *surface,
			 GCompareFunc compare_func,
			 GtsStopFunc stop_func,
			// gpointer data,
			 GSList *regions_list);

gboolean CoarsenStopFunc(gdouble cost,
					   guint nedge,
					   gpointer data);

void RegionCoarsen (GtsSurface *surface,
			  GtsKeyFunc cost_func,
			  gpointer cost_data,
			  GtsCoarsenFunc coarsen_func,
			  gpointer coarsen_data,
			  GtsStopFunc stop_func,
			  GSList *regions_list,
			  //gpointer stop_data,
			  //Region * region_data,
			  gdouble minangle);

GtsPSurface * gts_pregion_new (GtsPSurfaceClass * klass,
				GtsSurface * surface,
				GtsSplitClass * split_class,
				GtsKeyFunc cost_func,
				gpointer cost_data,
				GtsCoarsenFunc coarsen_func,
				gpointer coarsen_data,
				GtsStopFunc stop_func,
				GSList * regions_list,
//				gpointer stop_data,
//				Region * region_data,
				gdouble minangle);

GtsSurface *SurfaceDetail(GtsSurface *surface, 
				   GtsKeyFunc cost_func_coarsen,
				   gpointer cost_data_coarsen,
				   GtsCoarsenFunc coarsen_func,
				   gpointer coarsen_data,
				   GtsStopFunc stop_func_coarsen,
				   gdouble minangle_coarsen,
				   GCompareFunc compare_func_refine,
				   GtsStopFunc stop_func_refine,
				   GSList *regions_list);

#ifdef __cplusplus
}
#endif