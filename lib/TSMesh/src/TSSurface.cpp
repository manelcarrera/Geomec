#include <cmath>
#include <fstream>
#include "gts.h"
#include "SurfaceDetail.h"
#include "TSSurface.h"


//##ModelId=3BBD944902C1

GtsEdge* CTSSurface::InsertEdge(TIndexToEdgeMap& mpEdge, int nFirst, int nSecond)
{
  assert(nFirst != nSecond);
  std::pair<int, int> prEdge(nFirst, nSecond);
  if(nFirst < nSecond)
  {
    prEdge.first = nSecond;
    prEdge.second = nFirst;
  }

  // Search in the in the map
  TIndexToEdgeMap::iterator it = mpEdge.find(prEdge);
  if(it == mpEdge.end())
  {
    GtsEdge *pEdge = gts_edge_new(gts_edge_class(),
                    m_vcNodes[nFirst]->Vertex(),
                    m_vcNodes[nSecond]->Vertex());
    it = mpEdge.insert(TIndexToEdgeMap::value_type(prEdge, pEdge)).first;
  }

  return it->second;
}


void CTSSurface::ConstructUsingFaces(const std::vector<const geo::IFace*> &vcFace, ITSProgressBase &progress)
{
  m_pCopyPointCloud = NULL;

  m_pGtsSurface = gts_surface_new(gts_surface_class(),
          gts_face_class(),
          gts_edge_class(),
          gts_vertex_class());

  TIndexToEdgeMap mpEdge;

  std::vector<int> vcPoint(3);

  // The set is used to determine whether there are nodes that are shared by different faces, but
  // that are not the same object (pointer)
  std::set<const geo::IPoint*, geo::ICoordinate::CCoordinateLess> point_set;
  std::pair<std::set<const geo::IPoint*, geo::ICoordinate::CCoordinateLess>::iterator, bool> ptset_pr;
  const geo::IPoint *ptCurrent = 0;

  // progress must have been initialized with proper size
  for(size_t i = 0; i < vcFace.size(); i++)
  {
    progress.Step();	// Do progress step ...
    assert(vcFace[i]->NrOfPoints() == 3); // only allow triangles

    GtsVertex *v[3];

    // Walk over the nodes of the element
    for (int nPoint = 0; nPoint < 3; nPoint++)
    {
      const geo::IPoint &point = vcFace[i]->Point(nPoint);

      ptset_pr = point_set.insert(&point);
      if(ptset_pr.second)
        ptCurrent = *(ptset_pr.first);
      else
        ptCurrent = &point;

      TPointMap::iterator it = m_mpPoint.find(ptCurrent);
      if(it == m_mpPoint.end())
      {
        // Create a gts-vertex and a point
        GtsVertex *pVertex = gts_vertex_new(gts_vertex_class(), 
                           (gdouble) ptCurrent->X(), 
                           (gdouble) ptCurrent->Y(), 
                           (gdouble) ptCurrent->Z());
        m_pPointCloud = g_slist_append(m_pPointCloud, (gpointer) v[nPoint]);
        
        CTSNode *pNode = new CTSNode(pVertex);
        it = m_mpPoint.insert(TPointMap::value_type(pNode, m_vcNodes.size())).first;
        m_vcNodes.push_back(pNode);
      }
    
      // Store index
      vcPoint[nPoint] = it->second;
      
    }

    // make face of triangle
    GtsFace *f = gts_face_new(gts_face_class(), InsertEdge(mpEdge, vcPoint[0], vcPoint[1]),
                          InsertEdge(mpEdge, vcPoint[1], vcPoint[2]),
                          InsertEdge(mpEdge, vcPoint[2], vcPoint[0]));
    // add face to surface
    gts_surface_add_face(m_pGtsSurface, f);

    geo::CTriangle *pFace = new geo::CTriangle(*this,
                         m_vcElements.size(),
                         vcPoint[0],
                         vcPoint[1],
                         vcPoint[2]);
    m_mpFaceMap.insert(TFaceMap::value_type(f, m_vcElements.size()));
    m_vcElements.push_back(pFace);	
  }

  m_pGtsBBox = gts_bbox_surface(gts_bbox_class(), m_pGtsSurface);
}


CTSSurface::CTSSurface(const std::vector<const geo::IFace*> &vcFace, ITSProgressBase &progress)
:m_bIsDirty(true), 
 m_pPointCloud(0), 
 m_pRegions(0), 
 m_pGtsBBox(0),
 bPlanar(false)
{
  progress.Begin(vcFace.size());
  ConstructUsingFaces(vcFace, progress);
  progress.End();
}

// This constructor does have problems because it constructs for all faces all vertices.
CTSSurface::CTSSurface(const geo::ISurface &surface, ITSProgressBase &progress)
: m_bIsDirty(true), 
  m_pPointCloud(0),
  m_pCopyPointCloud(0),
  m_vcElements(surface.FaceSize()),
  m_vcNodes(surface.PointSize()),
  m_pRegions(0),
  m_pGtsBBox(0),
  bPlanar(false)	
{
  progress.Begin(surface.PointSize() + surface.FaceSize());
  
  // Construct surface
  m_pGtsSurface = gts_surface_new(gts_surface_class(),
          gts_face_class(),
          gts_edge_class(),
          gts_vertex_class());
    
  for(int nPoint = 0; nPoint < surface.PointSize(); nPoint++)
  {
    // Create a gts-vertex and a point
    GtsVertex *pVertex = gts_vertex_new(gts_vertex_class(), 
                       (gdouble) surface.Point(nPoint).X(), 
                       (gdouble) surface.Point(nPoint).Y(), 
                       (gdouble) surface.Point(nPoint).Z());
    m_pPointCloud = g_slist_append(m_pPointCloud, (gpointer) pVertex);
    
    CTSNode *pNode = new CTSNode(pVertex);
    bool bSucces = m_mpPoint.insert(TPointMap::value_type(pNode, nPoint)).second;
    m_vcNodes[nPoint] = pNode;
    assert(bSucces);

    progress.Step();
  }

  TIndexToEdgeMap mpEdge;

  // progress must have been initialized with proper size
  int nPoint0, nPoint1, nPoint2;
  for(int i = 0; i < surface.FaceSize(); i++)
  {
    progress.Step();	// Do progress step ...
    assert(surface.Face(i).NrOfPoints() == 3);
    if(surface.Face(i).IndexingElementSet() != &surface)
    {
      nPoint0 = m_mpPoint.find(&surface.Face(i).Point(0))->second;
      nPoint1 = m_mpPoint.find(&surface.Face(i).Point(1))->second;
      nPoint2 = m_mpPoint.find(&surface.Face(i).Point(2))->second;
    }
    else
    {
      nPoint0 = surface.Face(i).PointIndex(0);
      nPoint1 = surface.Face(i).PointIndex(1);
      nPoint2 = surface.Face(i).PointIndex(2);
    }

    // make face of triangle
    GtsFace *f = gts_face_new(gts_face_class(), InsertEdge(mpEdge, nPoint0, nPoint1),
                          InsertEdge(mpEdge, nPoint1, nPoint2),
                          InsertEdge(mpEdge, nPoint2, nPoint0));

//		// add face to surface
    gts_surface_add_face(m_pGtsSurface, f);

    geo::CTriangle *pFace = new geo::CTriangle(*this,
                         m_vcElements.size(),
                         nPoint0,
                         nPoint1,
                         nPoint2);
    m_mpFaceMap.insert(TFaceMap::value_type(f, i));
    m_vcElements[i] = pFace;	
  }	
  
  m_pGtsBBox = gts_bbox_surface(gts_bbox_class(), m_pGtsSurface);


  progress.End();
}


CTSSurface::CTSSurface(const CTSSurface &surface)
:m_bIsDirty(true),
 m_pPointCloud(0), 
 m_pRegions(0), 
 m_pGtsBBox(0)

{
  bPlanar = surface.Planar();
  m_pCopyPointCloud = NULL;

  m_pGtsSurface = gts_surface_new(gts_surface_class(),
          gts_face_class(),
          gts_edge_class(),
          gts_vertex_class());
  
  gts_surface_copy (m_pGtsSurface, surface.m_pGtsSurface);
  
  BuildInterface();
}

CTSSurface::CTSSurface(const std::vector<geo::CPoint>& vcPoint, 
             const std::vector<std::vector<int> >& vcTriangle,
             ITSProgressBase &progress)
: m_bIsDirty(true), 
  m_pPointCloud(0),
  m_pCopyPointCloud(0),
  m_vcElements(vcTriangle.size()),
  m_vcNodes(vcPoint.size()),
  m_pRegions(0), 
  m_pGtsBBox(0),
  bPlanar(false)	
{
  progress.Begin(vcPoint.size() + vcTriangle.size());
  
  // Construct surface
  m_pGtsSurface = gts_surface_new(gts_surface_class(),
          gts_face_class(),
          gts_edge_class(),
          gts_vertex_class());
    
  for(int nPoint = 0; nPoint < vcPoint.size(); nPoint++)
  {
    // Create a gts-vertex and a point
    GtsVertex *pVertex = gts_vertex_new(gts_vertex_class(), 
                       (gdouble) vcPoint[nPoint].X(), 
                       (gdouble) vcPoint[nPoint].Y(), 
                       (gdouble) vcPoint[nPoint].Z());
    m_pPointCloud = g_slist_append(m_pPointCloud, (gpointer) pVertex);
    
    CTSNode *pNode = new CTSNode(pVertex);
    bool bSucces = m_mpPoint.insert(TPointMap::value_type(pNode, nPoint)).second;
    m_vcNodes[nPoint] = pNode;
    assert(bSucces);

    progress.Step();
  }

  TIndexToEdgeMap mpEdge;

  // progress must have been initialized with proper size
  for(size_t i = 0; i < vcTriangle.size(); i++)
  {
    progress.Step();	// Do progress step ...
    const std::vector<int>& vcPoint = vcTriangle[i];
    assert(vcTriangle[i].size() == 3); // only allow triangles


    // make face of triangle
    GtsFace *f = gts_face_new(gts_face_class(), InsertEdge(mpEdge, vcPoint[0], vcPoint[1]),
                          InsertEdge(mpEdge, vcPoint[1], vcPoint[2]),
                          InsertEdge(mpEdge, vcPoint[2], vcPoint[0]));

//		// add face to surface
    gts_surface_add_face(m_pGtsSurface, f);

    geo::CTriangle *pFace = new geo::CTriangle(*this,
                         m_vcElements.size(),
                         vcPoint[0],
                         vcPoint[1],
                         vcPoint[2]);
    m_mpFaceMap.insert(TFaceMap::value_type(f, i));
    m_vcElements[i] = pFace;	
  }	
  
  m_pGtsBBox = gts_bbox_surface(gts_bbox_class(), m_pGtsSurface);


  progress.End();
}


CTSSurface::CTSSurface(const geo::CPtrArray<geo::IPoint> &points, ITSProgressBase &progress)
:m_bIsDirty(true), 
 m_pGtsSurface(0),
 m_pCopyPointCloud(0),
 m_pRegions(0), 
 m_pGtsBBox(0),
 bPlanar(false)
{
  assert(points.Size() > 0);

  m_pPointCloud = g_slist_alloc();

  int i;
  geo::CPtrArray<geo::IPoint>::const_iterator it;

  int size = points.Size();
  progress.Begin(2 * size);

  for(i = 0, it = points.begin(); it != points.end(); it++, i++)
  {
    GtsVertex *v;
    v = gts_vertex_new(gts_vertex_class(), (gdouble) (*it)->X(), (gdouble) (*it)->Y(), (gdouble) (*it)->Z());

    if(!i) m_pPointCloud->data = (gpointer) v;
    else   g_slist_append(m_pPointCloud, (gpointer) v);

    progress.Step();
  }

  CreateGtsSurfaceObjectFromPoints(m_pPointCloud, &progress);
  
  BuildInterface();

  progress.End();
}

CTSSurface::CTSSurface(const geo::CArray<geo::CPoint> &points, ITSProgressBase &progress)
:m_bIsDirty(true), 
 m_pGtsSurface(0),
 m_pCopyPointCloud(0),
 m_pRegions(0),
 m_pGtsBBox(0),
 bPlanar(false)
{
  assert(points.Size() > 0);

  m_pPointCloud = g_slist_alloc();

  int i;
  geo::CArray<geo::CPoint>::const_iterator it;

  progress.Begin(2 * points.Size());

  for(i = 0, it = points.begin(); it != points.end(); it++, i++)
  {
    GtsVertex *v;
    v = gts_vertex_new(gts_vertex_class(), (gdouble) (*it).X(), (gdouble) (*it).Y(), (gdouble) (*it).Z());

    if(!i) m_pPointCloud->data = (gpointer) v;
    else   g_slist_append(m_pPointCloud, (gpointer) v);

    progress.Step();
  }

  CreateGtsSurfaceObjectFromPoints(m_pPointCloud, &progress); 

  BuildInterface();

  progress.End();
}

// This constructor can be used to construct a concave surface. Supply all the points (also on the edge
// of the surface) and a polygon describing the edge if UsePolyPoints is false else only supply the points inside the polygon.
CTSSurface::CTSSurface(const geo::CPtrArray<geo::IPoint> &points, ITSProgressBase &progress, const geo::CPolygon &Polygon, bool UsePolyPoints /* = false */)
: m_bIsDirty(true), 
  m_pRegions(0), 
  m_pGtsBBox(0),
  bPlanar(false)
{
  GtsVertex *v;
  m_pCopyPointCloud = NULL;
  m_pPointCloud = g_slist_alloc();

  int i, j;
  geo::CPtrArray<geo::IPoint>::const_iterator pointit;

  int progsize = 2 * points.Size();
  if(UsePolyPoints) progsize += Polygon.NrOfPoints();
  progress.Begin(progsize);

  for(i = 0, pointit = points.begin(); pointit != points.end(); pointit++, i++)
  {
    v = gts_vertex_new(gts_vertex_class(), (gdouble) (*pointit)->X(), (gdouble) (*pointit)->Y(), (gdouble) (*pointit)->Z());
    if(!i) m_pPointCloud->data = (gpointer) v;
    else   g_slist_append(m_pPointCloud, (gpointer) v);

    progress.Step();
  }

  if(UsePolyPoints)
  {
    for(j = 0; j < Polygon.NrOfPoints(); j++)
    {
      geo::CPoint point(Polygon.Point(j));
      v = gts_vertex_new(gts_vertex_class(), (gdouble) point.X(), (gdouble) point.Y(), (gdouble) point.Z());

      if(!i) 
      {
        m_pPointCloud->data = (gpointer) v;
        i++;
      }
      else   g_slist_append(m_pPointCloud, (gpointer) v);

      progress.Step();
    }
  }

  CreateGtsSurfaceObjectFromPoints(m_pPointCloud, &progress);

  RemoveRedundantFaces(Polygon);
  
  BuildInterface();
}

//Private constructor, only used in ChangeSurfaceDetail()....
CTSSurface::CTSSurface(GtsSurface *surface):
 m_bIsDirty(true), 
 m_pPointCloud(0), 
 m_pRegions(0), 
 m_pGtsBBox(0),
 bPlanar(false)
{
  m_pCopyPointCloud = NULL;

  assert(surface);
  m_pGtsSurface = gts_surface_new(gts_surface_class(),
                  gts_face_class(),
                  gts_edge_class(),
                  gts_vertex_class());

  gts_surface_copy(m_pGtsSurface, surface);

  gts_object_destroy(GTS_OBJECT(surface));

  BuildInterface();
}


//##ModelId=3BBD944902C2
CTSSurface::~CTSSurface()
{
  InvalidateCache();

  if(m_pGtsSurface) gts_object_destroy(GTS_OBJECT(m_pGtsSurface));
  if(m_pPointCloud) g_slist_free(m_pPointCloud);
  if(m_pCopyPointCloud) g_slist_free(m_pCopyPointCloud);
  gts_object_destroy(GTS_OBJECT(m_pGtsBBox));

  TMeshSetIt itzones;
  for(itzones = m_stMeshZones.begin(); itzones != m_stMeshZones.end(); itzones++)
  {
    delete const_cast <CMeshZone&> (*itzones).Region();
  }
  if(m_pRegions) g_slist_free(m_pRegions);

  geo::CPtrArray<geo::CTriangle>::iterator itelement;
  for(itelement = m_vcElements.begin(); itelement != m_vcElements.end(); itelement++)
  {
    delete *itelement;
  }

  geo::CPtrArray<CTSNode>::iterator itnodes; 
  for(itnodes = m_vcNodes.begin(); itnodes != m_vcNodes.end(); itnodes++)
  {
    delete *itnodes;
  }
}

static gint RedundantFacesCallBack(gpointer item, gpointer data)
{
  GtsFace *face = (GtsFace*) item;
  geo::CPolygon *pPoly = (geo::CPolygon*) data;

  GtsSegment *seg1 = &(face->triangle.e1->segment);
  GtsSegment *seg2 = &(face->triangle.e2->segment);
  GtsSegment *seg3 = &(face->triangle.e3->segment);

  GtsVertex *v1 =	gts_segment_midvertex(seg1, gts_vertex_class());
  GtsVertex *v2 =	gts_segment_midvertex(seg2, gts_vertex_class());
  GtsVertex *v3 =	gts_segment_midvertex(seg3, gts_vertex_class());

  geo::CPoint p1(v1->p.x, v1->p.y, v1->p.z);
  geo::CPoint p2(v2->p.x, v2->p.y, v2->p.z);
  geo::CPoint p3(v3->p.x, v3->p.y, v3->p.z);

  if(pPoly->IFace::Contains(p1, true) && pPoly->IFace::Contains(p2, true) && pPoly->IFace::Contains(p3, true))
    return 0;
  else
    return 1;
}

//void CTSSurface::RemoveRedundantSegments(const geo::CPtrArray<geo::IPoint> &edgepoints, GSList *edgevertices)
void CTSSurface::RemoveRedundantFaces(const geo::CPolygon &Polygon)
{
  geo::CPolygon *pPoly = const_cast<geo::CPolygon*>(&Polygon);
  gts_surface_foreach_face_remove(m_pGtsSurface, RedundantFacesCallBack, pPoly);
}

const geo::IPoint &CTSSurface::Point(int nIndex) const
{
  assert(nIndex >= 0 && nIndex < m_vcNodes.size());
  return *m_vcNodes[nIndex];
}

geo::IPoint &CTSSurface::PointAt(int nIndex)
{
  assert(nIndex >= 0 && nIndex < m_vcNodes.size());
  return *m_vcNodes[nIndex];
}

//return: If element is empty the end of the sequence is reached.
const geo::IFace &CTSSurface::Face(int nIndex) const
{
  assert(nIndex >= 0 && nIndex < m_vcElements.size());
  return *m_vcElements[nIndex];
}

//Returns the nr. of elements in the surface...
int CTSSurface::FaceSize() const
{
  return m_vcElements.size();
}

//Returns the nr. of nodes in the surface...
int CTSSurface::PointSize() const
{
  return m_vcNodes.size();
}

struct _FacesAtPointData
{
  geo::CPtrArray <geo::IFace> vcFaces;
  GtsPoint point;
  const CTSSurface::TFaceMap *pmpFaces;
  const std::vector<geo::CTriangle*> *pvcFaces;
};

static gint FacesAtCallback(gpointer item, gpointer data)
{
  GtsTriangle *pTriangle = (GtsTriangle *) item;
  struct _FacesAtPointData *pData = (struct _FacesAtPointData *) data;
  
  if(gts_point_is_in_triangle((&pData->point), pTriangle) == GTS_IN || gts_point_is_in_triangle((&pData->point), pTriangle) == GTS_ON)
  {
    // Find triangle in faces ....
    CTSSurface::TFaceMap::const_iterator it = pData->pmpFaces->find((GtsFace*)(pTriangle));
    assert(it != pData->pmpFaces->end());
    pData->vcFaces.PushBack(*pData->pvcFaces->operator[](it->second));
  }

  return 0;
}

// Returns an array of IFaces at the specified point in the surface....
// REMARK : THIS FUNCTION WORKS BUT IS VERY SLOW! A better way to do this is to make
//          use of the gts_point_locate function!!! This function only returns 1 face.
const geo::CPtrArray <geo::IFace> CTSSurface::FacesAt(const geo::IPoint &p) const
{
  struct _FacesAtPointData Data;
    
  Data.point.x = p.X();
  Data.point.y = p.Y();
  Data.point.z = p.Z();
  
  Data.pvcFaces = &m_vcElements;
  Data.pmpFaces = &m_mpFaceMap;

  gts_surface_foreach_face(m_pGtsSurface, FacesAtCallback, (gpointer) &Data);

  return Data.vcFaces;
}

//AverageNormal() calculates the average normal of the surface by summing the normals of all the separate normals
//from the elements in the surface.
const geo::CVector CTSSurface::AverageNormal() const
{	
  if(m_bIsDirty)
  {
    m_vcAverage=geo::CVector::NullVector;
    for(int i = 0; i < FaceSize(); i++)
    {
      m_vcAverage.X(m_vcAverage.X() + Face(i).Normal().X());
      m_vcAverage.Y(m_vcAverage.Y() + Face(i).Normal().Y());
      m_vcAverage.Z(m_vcAverage.Z() + Face(i).Normal().Z());
    }

    m_bIsDirty = false;
    return m_vcAverage;
  }
  else 
    return m_vcAverage;
}

//FacesAtNode() needs a point that is a node in the surface!! If you have an arbitrary point use FacesAt(). Returns
//an array of IFaces at the specified node in the surface....
const geo::CPtrArray <geo::IFace> CTSSurface::FacesAtNode(const geo::IPoint &p) const
{
  geo::CPtrArray<geo::IFace> FaceArray;

  // We must find the vetex in the point map
  TPointMap::const_iterator it = m_mpPoint.find(&p);

  if(it != m_mpPoint.end())
  {
    const CTSNode& node = *m_vcNodes[it->second];

    GSList *list = g_slist_alloc();
    GSList *FaceList = gts_vertex_faces(node.m_pVertex, m_pGtsSurface, list);
    
    while(FaceList && FaceList->data)
    {
      GtsFace *f = (GtsFace*) FaceList->data;

      TFaceMap::const_iterator it = m_mpFaceMap.find(f);
      FaceArray.PushBack(*m_vcElements[it->second]);

      FaceList = FaceList->next;
    }
  }

  return FaceArray;
}

//Structure required by BuilPointCloud().....
struct _BuildPointCloudData
{
  std::vector<CTSNode*> *pNodes;
  std::map<GtsVertex*, int> *pmpVertex;
  int idx;
  CTSSurface *pTSSurface;
  GSList *pPointCloud;
  CTSSurface::TPointMap* pmpPoint;
};

static gint BuildPointCloudCallback(gpointer item, gpointer data)
{
  GtsVertex *pVertex = (GtsVertex *) item;
  struct _BuildPointCloudData *pData = (struct _BuildPointCloudData*) data;

  // Create on the nNodeIndex entry
  CTSNode *pNode = new CTSNode(pVertex);
  int nNodeIndex = pData->idx;

  // Create point index
  pData->pmpPoint->insert(CTSSurface::TPointMap::value_type(pNode, nNodeIndex));

  // set the element in the list of nodes
  assert(nNodeIndex < pData->pNodes->size());
  pData->pNodes->operator[](pData->idx++) = pNode;
  
  (*pData->pmpVertex).insert(std::map<GtsVertex*, int>::value_type(pVertex, nNodeIndex));
  if (pData->pPointCloud)
    pData->pPointCloud = g_slist_append(pData->pPointCloud, (gpointer) pVertex);

   return 0;
}

//Structure required by BuilElementList()...
struct _BuildElementListData
{
  std::vector<geo::CTriangle*> *pElements;
  CTSSurface::TFaceMap *pmpElements;
  std::map<GtsVertex*, int> *pmpNodes;
  CTSSurface *pSurface;
  int idx;
};


//Callback function for gts_surface_foreach_face() in BuildElementList()....
static gint BuildElementListCallback(gpointer item, gpointer data)
{
  GtsFace *pFace = (GtsFace *) item;
  struct _BuildElementListData *pData = (struct _BuildElementListData *) data;
  GtsVertex *v1, *v2, *v3;
  
  // get the vertices from the triangle in the current face
  gts_triangle_vertices(&pFace->triangle, &v1, &v2, &v3);

  std::map<GtsVertex*, int>::const_iterator it1 = pData->pmpNodes->find(v1);
  std::map<GtsVertex*, int>::const_iterator it2 = pData->pmpNodes->find(v2);
  std::map<GtsVertex*, int>::const_iterator it3 = pData->pmpNodes->find(v3);

  // create the nodes from the vertices, supplying the element to the constructor
  geo::CTriangle *pElement = new geo::CTriangle(*pData->pSurface, pData->idx, it1->second, it2->second, it3->second);

  // set the element in the list of elements
  int nElementIndex = pData->idx++;
  assert(nElementIndex == pElement->Index());
  assert(nElementIndex < pData->pElements->size());
  pData->pElements->operator[](nElementIndex) = pElement;
  
  //put the element pair in the map
  (*pData->pmpElements).insert(CTSSurface::TFaceMap::value_type(pFace, nElementIndex));

  return 0;
}

void CTSSurface::InvalidateCache()
{
  m_mpFaceMap.clear();
  m_mpPoint.clear();
  for(size_t i = 0; i < m_vcNodes.size(); i++)
    delete m_vcNodes[i];
  m_vcNodes.clear();
  for(size_t i = 0; i < m_vcElements.size(); i++)
    delete m_vcElements[i];
  m_vcElements.clear();

  // Call the base
  ISurface::InvalidateCache();
}

//Creates all the required CTS2DElements. Fills the geo::CPtrArray m_vcElements as well as the TFaceMap m_mpFaceMap... 
void CTSSurface::BuildInterface()
{
  assert(m_pGtsSurface);
  assert(m_vcElements.empty());
  assert(m_vcNodes.empty());
  // Size interface vectors and and create a temporary pointmap
  m_vcNodes.resize((int) gts_surface_vertex_number(m_pGtsSurface));
  m_vcElements.resize((int) gts_surface_face_number(m_pGtsSurface));
  std::map<GtsVertex*, int> mpNodeMap;

  // Fill point map
  struct _BuildPointCloudData NodeData;

  NodeData.pmpVertex = &mpNodeMap;
  NodeData.pNodes = &m_vcNodes;
  NodeData.idx = 0;
  NodeData.pmpPoint = &m_mpPoint;
  NodeData.pTSSurface = this;
  if (m_pPointCloud)
    NodeData.pPointCloud = 0;
  else
    NodeData.pPointCloud = g_slist_alloc();
  gts_surface_foreach_vertex(m_pGtsSurface, BuildPointCloudCallback, (gpointer) &NodeData);
  if (!m_pPointCloud)
    m_pPointCloud = NodeData.pPointCloud;
  
  // Fill element map ..
  struct _BuildElementListData Data;
  Data.pElements = &m_vcElements;
  Data.pmpElements = &m_mpFaceMap;
  Data.pmpNodes = &mpNodeMap;
  Data.pSurface = this;
  Data.idx = 0;

  gts_surface_foreach_face(m_pGtsSurface, BuildElementListCallback, (gpointer) &Data);

  if(m_pGtsBBox) gts_object_destroy(GTS_OBJECT(m_pGtsBBox));
  m_pGtsBBox = gts_bbox_surface(gts_bbox_class(), m_pGtsSurface);
}

// This function will find the points that are shared between the incoming surface and the 'this' surface.
// Points from the 'this' surface are put in 'OwnPoints' and points from the incoming surface are put in
// 'OtherPoints' so that the user can decide what to do with them.
void CTSSurface::SharedPoints(std::set<geo::IPoint*> &OwnPoints, std::set<geo::IPoint*> &OtherPoints, CTSSurface &Surface) const
{
  int i;
  std::set<geo::CPoint> TempSet;
  for(i = 0; i < PointSize(); i++)
  {
    TempSet.insert(Point(i));
  }

  assert(TempSet.size() == PointSize());
  std::pair<std::set<geo::CPoint>::iterator, bool> ItPair;
  for(i = 0; i < Surface.PointSize(); i++)
  {
    ItPair = TempSet.insert(Surface.Point(i));
    if(!ItPair.second)
    {// Point is shared
      OwnPoints.insert(const_cast <geo::CPoint*> (&(*(ItPair.first))));
      OtherPoints.insert((geo::IPoint*)(&(Surface.Point(i))));
    }
  }
}

//If a meshzone is created in the client application it should be added with this function. A Region is created that
//is put in the m_pRegions GSList. The zone is inserted in the set m_stMeshZones....
void CTSSurface::AddMeshZone(CMeshZone &meshzone)
{//AT THE MOMENT I ASSUME THAT THE m_dEdgeSquared (Criterium()) IS LARGER THAN ZERO!! 
  if(!m_pRegions) m_pRegions = g_slist_alloc();
  assert(!meshzone.Empty() && meshzone.Criterium() > 0);

  Region *pRegion = new Region;
  pRegion->nPoint = meshzone.NrOfPoints();
  pRegion->dX = (double*)malloc(pRegion->nPoint*sizeof(double));
  pRegion->dY = (double*)malloc(pRegion->nPoint*sizeof(double));
  
  meshzone.Region(pRegion);
  int i;
  for(i = 0; i < pRegion->nPoint; i++)
  {
    pRegion->dX[i] = meshzone.Point(i).X();
    pRegion->dY[i] = meshzone.Point(i).Y();
  }

  pRegion->stop_data = meshzone.Criterium();

  m_stMeshZones.insert(meshzone);

  g_slist_append(m_pRegions, (gpointer)pRegion);
}

//This function removes a MeshZone from the m_stMeshZones set and the accompanying Region from the m_pRegions GSList..
void CTSSurface::RemoveMeshZone(CMeshZone &meshzone)
{
  m_pRegions = g_slist_remove(m_pRegions, meshzone.Region());
  delete meshzone.Region();

  TMeshSetIt it = m_stMeshZones.find(meshzone);
  m_stMeshZones.erase(it);
}

//This function copies the m_pGtsSurface and returns a new CTSSurface of which the detail is changed.
//IF "TYPE" IS "COARSEN" OR "REFINE" THE m_pRegions LIST IS SET TO ZERO AND ALL THE MESHZONES ARE LOST!!
CTSSurface *CTSSurface::ChangeSurfaceDetail(double *DetailData, double &MinimumAngle, eTypeOfDetailChange Type)
{
  GtsSurface *tempsurface = gts_surface_new(gts_surface_class(),
                  gts_face_class(),
                  gts_edge_class(),
                  gts_vertex_class());
  switch(Type)
  {
  case COARSEN://Coarsen whole surface.....
    gts_surface_copy(tempsurface, m_pGtsSurface);
    gts_surface_coarsen(tempsurface, NULL, NULL, NULL, NULL, CoarsenStopFunc, (gpointer)DetailData, 
              (gdouble)MinimumAngle);
    RemoveRegionsAndMeshZones();
    break;
  case REFINE://Refine whole surface.....
    gts_surface_copy(tempsurface, m_pGtsSurface);
    gts_surface_refine(tempsurface, NULL, RefineStopFunc, (gpointer)DetailData);
    RemoveRegionsAndMeshZones();
    break;
  case ZONES://Coarsen and/or refine in the defined zones.....
    assert(m_pRegions);
    tempsurface = (SurfaceDetail(m_pGtsSurface, NULL, NULL, NULL, NULL, CoarsenStopFunc, 
                  (gdouble)MinimumAngle, NULL, RefineStopFunc, m_pRegions));
    break;
  }

  CTSSurface *NewCTSSurface = new CTSSurface(tempsurface);
  return NewCTSSurface;
}

//Removes all meshzones and accompanying regions...
void CTSSurface::RemoveRegionsAndMeshZones()
{
  TMeshSetIt it;
  for(it = m_stMeshZones.begin(); it != m_stMeshZones.end(); it++)
  {
    m_pRegions = g_slist_remove(m_pRegions,
      const_cast <CMeshZone&> (*it).Region());
    delete const_cast <CMeshZone&> (*it).Region();
  }
  
  m_stMeshZones.clear();
}

//used for tests...
void CTSSurface::WriteSurface(FILE *OutFile)
{
  gts_surface_write(m_pGtsSurface, OutFile);
}

CTSSurface::CTSSurfaceStat CTSSurface::Statistics() const
{
  GtsSurfaceQualityStats stats;
  gts_surface_quality_stats(m_pGtsSurface,&stats);
  return CTSSurfaceStat(stats.face_area.mean * stats.face_area.n,stats.face_area.mean,stats.edge_length.mean,stats.edge_angle.mean);
}

// This callback functions is used by the method CalcAverageEdgeSize. It calculates the size of
// one edge using the 3D phytagoras function and add it to the total.
static gint CalcAverageEdgeSizeCallback(gpointer item, gpointer data)
{
  GtsEdge *pEdge = (GtsEdge *)item;
  double *pdSize = (double *)data;
  *pdSize = sqrt(pow(fabs(pEdge->segment.v1->p.x) - fabs(pEdge->segment.v2->p.x),2) + 
          pow(fabs(pEdge->segment.v1->p.y) - fabs(pEdge->segment.v2->p.y),2) + 
          pow(fabs(pEdge->segment.v1->p.z) - fabs(pEdge->segment.v2->p.z),2)) +
       *pdSize;

  return 0;
}

// Calculate the average edge size for the whole surface.
double CTSSurface::CalcAverageEdgeSize()
{
  double dAverageEdgeSize = 0;
  // Use the callbackfunction to do all the calculations.
  gts_surface_foreach_edge(m_pGtsSurface, CalcAverageEdgeSizeCallback, (gpointer)&dAverageEdgeSize);
  // Calculate the average.
  dAverageEdgeSize = dAverageEdgeSize / gts_surface_edge_number(m_pGtsSurface);
  return dAverageEdgeSize;
}

//=============================================================================
// Resize the surface with the offset given in X and Y direction and with an offset for precision.
// This method makes use of the Resize method where an box is given.
bool CTSSurface::Increase(double dXNOffset, double dYNOffset, double dXMOffset, double dYMOffset, double dOffset /* = 0 */)
{
  // Calculate the resizing box.
  bool ret = Increase(geo::CBox(geo::CPoint(m_pGtsBBox->x1-dXNOffset, m_pGtsBBox->y1-dYNOffset), 
                geo::CPoint(m_pGtsBBox->x2+dXMOffset, m_pGtsBBox->y2+dYMOffset)), dOffset);

  return ret;
}

void CTSSurface::InsertEdge(TVertexMap &mpVertices, GtsEdge *pEdge, GtsVertex *v) const
{
  std::pair<TVertexMap::iterator, bool> prInsert = mpVertices.insert(TVertexMap::value_type(v, std::vector<GtsEdge*> ()));
  prInsert.first->second.push_back(pEdge);
}

GtsVertex *CTSSurface::OtherVertex(GtsEdge *pEdge, GtsVertex *pVertex) const
{
  assert(pVertex == pEdge->segment.v1 || pVertex == pEdge->segment.v2);

  if(pVertex == pEdge->segment.v1) return pEdge->segment.v2;
  return pEdge->segment.v1;
}

GtsEdge *CTSSurface::OtherEdge(GtsVertex *pVertex, GtsEdge *pEdge, const TVertexMap &mpVertices) const
{
  TVertexMap::const_iterator it = mpVertices.find(pVertex);
  assert(it != mpVertices.end());

  std::vector<GtsEdge *> vcEdges = it->second;
  assert(vcEdges.size() == 2);
  assert(pEdge == vcEdges[0] || pEdge == vcEdges[1]);

  if(pEdge == vcEdges[0]) return vcEdges[1];
  return vcEdges[0];
}

void CTSSurface::ProcessEdgeRing(std::vector<GtsVertex*> &vcVertices, const TVertexMap &mpVertices,
                 GtsVertex *pFirstVertex, GtsEdge *pFirstEdge) const
{
  GtsVertex *pVertex = pFirstVertex;
  GtsEdge *pEdge = pFirstEdge;

  do
  {
    vcVertices.push_back(pVertex);

    pVertex = OtherVertex(pEdge, pVertex);
    pEdge = OtherEdge(pVertex, pEdge, mpVertices);
  } while(pVertex != pFirstVertex);

  assert(vcVertices.size() == mpVertices.size());
}

std::vector<GtsVertex*> CTSSurface::SurfaceBoundary() const
{
  GSList *pBoundaryEdges = gts_surface_boundary(m_pGtsSurface);
  g_slist_length(pBoundaryEdges);
  TVertexMap mpVertices;

  GSList *l = pBoundaryEdges;
  while(l)
  {
    GtsEdge *pEdge = (GtsEdge *) l->data;
    assert(pEdge != 0);
    GtsVertex *v1 = pEdge->segment.v1;
    assert(v1 != 0);
    GtsVertex *v2 = pEdge->segment.v2;
    assert(v2 != 0);

    InsertEdge(mpVertices, pEdge, v1);
    InsertEdge(mpVertices, pEdge, v2);

    l = l->next;
  }

  std::vector<GtsVertex*> ret;
  assert(mpVertices.size() != 0);

  TVertexMap::iterator itBegin = mpVertices.begin();
  GtsVertex *pFirst = itBegin->first;
  std::vector<GtsEdge*> vcEdges = itBegin->second;
  assert(vcEdges.size() == 2);

  // choose an edge
  GtsEdge *pEdge = vcEdges[0];

  ProcessEdgeRing(ret, mpVertices, pFirst, pEdge);
  assert(ret.size() == g_slist_length(pBoundaryEdges));

  return ret;
}

double CTSSurface::BoundaryArea(const std::vector<GtsVertex*> &vcBoundary) const
{
  // calculate the (2D) area of the boundary given by the polygon in the vector
  int sz = vcBoundary.size();

  assert(sz > 2);

  double dArea = 0;
  GtsVertex *p1;
  GtsVertex *p2;

  for(int i = 1; i < sz; i++)
  {
    p1 = vcBoundary[i-1];
    p2 = vcBoundary[i];
    dArea += ((p1->p.x + p2->p.x) / 2) * (p2->p.y - p1->p.y);
  }

  p1 = vcBoundary[sz-1];
  p2 = vcBoundary[0];
  dArea += ((p1->p.x + p2->p.x) / 2) * (p2->p.y - p1->p.y);

  return dArea;
}

geo::CVector CTSSurface::BoundaryIncreaseVector(GtsVertex *p1, GtsVertex *p2, GtsVertex *p3, double length) const
{
  geo::CVector v1(p2->p.x - p1->p.x, p2->p.y - p1->p.y);
  v1 = v1.UnitVector();
  geo::CVector v2(p3->p.x - p2->p.x, p3->p.y - p2->p.y);
  v2 = v2.UnitVector();
  geo::CVector sum = v1 + v2;
  geo::CVector ret = sum.GetNormal();
  ret = ret.UnitVector();
  ret = ret * length;

  return ret;
}

void CTSSurface::GenerateBoundaryIncreaseVectors(const std::vector<GtsVertex*> &vcBoundary, bool bWindingRight,
                         std::vector<geo::CVector> &vcVectors, double length) const
{
  GtsVertex *p1;
  GtsVertex *p2;
  GtsVertex *p3;
  int sz = vcBoundary.size();
  assert(sz > 2);

  vcVectors.clear();
  geo::CVector vecNew;

  for(int i = 0; i < sz - 2; i++)
  {
    p1 = vcBoundary[i];
    p2 = vcBoundary[i+1];
    p3 = vcBoundary[i+2];

    vecNew = BoundaryIncreaseVector(p1, p2, p3, length);
    if(bWindingRight) vecNew.Flip();
    vcVectors.push_back(vecNew);
  }

  // and the last two
  p1 = vcBoundary[sz - 2];
  p2 = vcBoundary[sz - 1];
  p3 = vcBoundary[0];

  vecNew = BoundaryIncreaseVector(p1, p2, p3, length);
  if(bWindingRight) vecNew.Flip();
  vcVectors.push_back(vecNew);

  p1 = vcBoundary[sz - 1];
  p2 = vcBoundary[0];
  p3 = vcBoundary[1];

  vecNew = BoundaryIncreaseVector(p1, p2, p3, length);
  if(bWindingRight) vecNew.Flip();
  vcVectors.push_back(vecNew);

  assert(vcVectors.size() == sz);
}

bool CTSSurface::NextInBBox(const GtsVertex *p, const geo::CVector &v, const geo::CBox &box) const
{
  if(p->p.x + v.X() < box.Min().X() + EPS) return false;
  if(p->p.x + v.X() > box.Max().X() - EPS) return false;
  if(p->p.y + v.Y() < box.Min().Y() + EPS) return false;
  if(p->p.y + v.Y() > box.Max().Y() - EPS) return false;

  return true;
}

GtsVertex *CTSSurface::GtsVertexFromPoint(const geo::IPoint &point) const
{
  return gts_vertex_new(gts_vertex_class(), point.X(), point.Y(), point.Z());
}

GtsVertex *CTSSurface::CreateVertexOnBBox(const GtsVertex *p, const geo::CVector &v, const geo::CBox &box) const
{
  // don't process points outside bounding box
  if(p->p.x < box.Min().X() || p->p.x > box.Max().X()) return 0;
  if(p->p.y < box.Min().Y() || p->p.y > box.Max().Y()) return 0;

  assert(!v.Empty() && !(v == geo::CVector::NullVector));
  geo::CPoint ptRef(p->p.x, p->p.y, 0);
  geo::CLine l(ptRef, v);

  geo::CLine left  (geo::CPoint(box.Min().X(), box.Min().Y()), geo::CPoint(box.Min().X(), box.Max().Y()));
  geo::CLine right (geo::CPoint(box.Max().X(), box.Min().Y()), geo::CPoint(box.Max().X(), box.Max().Y()));
  geo::CLine bottom(geo::CPoint(box.Min().X(), box.Min().Y()), geo::CPoint(box.Max().X(), box.Min().Y()));
  geo::CLine top   (geo::CPoint(box.Min().X(), box.Max().Y()), geo::CPoint(box.Max().X(), box.Max().Y()));

  geo::CPoint pt;

  pt = l.Intersection(left);
  if(!pt.Empty() && pt.Y() > box.Min().Y() && pt.Y() < box.Max().Y() && l.InDirectionOf(ptRef, v, pt) &&
    pt.Distance(ptRef) > EPS)
  {
    pt.Z(p->p.z);
    return GtsVertexFromPoint(pt);
  }

  pt = l.Intersection(right);
  if(!pt.Empty() && pt.Y() > box.Min().Y() && pt.Y() < box.Max().Y() && l.InDirectionOf(ptRef, v, pt) &&
    pt.Distance(ptRef) > EPS)
  {
    pt.Z(p->p.z);
    return GtsVertexFromPoint(pt);
  }

  pt = l.Intersection(bottom);
  if(!pt.Empty() && pt.X() > box.Min().X() && pt.X() < box.Max().X() && l.InDirectionOf(ptRef, v, pt) &&
    pt.Distance(ptRef) > EPS)
  {
    pt.Z(p->p.z);
    return GtsVertexFromPoint(pt);
  }

  pt = l.Intersection(top);
  if(!pt.Empty() && pt.X() > box.Min().X() && pt.X() < box.Max().X() && l.InDirectionOf(ptRef, v, pt) &&
    pt.Distance(ptRef) > EPS)
  {
    pt.Z(p->p.z);
    return GtsVertexFromPoint(pt);
  }

  return 0;
}

void CTSSurface::InsertBoxCorner(const double &x, const double &y, const std::vector<GtsVertex*> &vcBoxEdgeVertices)
{
  if(!vcBoxEdgeVertices.size()) return;

  size_t iNearest = 0;

  for(size_t i = 1; i < vcBoxEdgeVertices.size(); i++)
  {
    double deltaX = x - vcBoxEdgeVertices[i]->p.x;
    double deltaY = y - vcBoxEdgeVertices[i]->p.y;

    double deltaXnearest = x - vcBoxEdgeVertices[iNearest]->p.x;
    double deltaYnearest = y - vcBoxEdgeVertices[iNearest]->p.y;

    if(deltaX * deltaX + deltaY * deltaY < deltaXnearest * deltaXnearest + deltaYnearest * deltaYnearest)
      iNearest = i;
  }

  double dX = fabs(x - vcBoxEdgeVertices[iNearest]->p.x);
  double dY = fabs(y - vcBoxEdgeVertices[iNearest]->p.y);

  // the box corner may already be there
  // don't create it again then, simply return
  if(dX < EPS && dY < EPS) return;

  GtsVertex *vNew = gts_vertex_new(gts_vertex_class(), x, y, vcBoxEdgeVertices[iNearest]->p.z);
  g_slist_append(m_pCopyPointCloud, (gpointer) vNew);
}

void CTSSurface::InsertExtensionPoints(const std::vector<GtsVertex*> &vcBoundary,
                     const std::vector<geo::CVector> &vcVectors,
                     const geo::CBox &box)
{
  std::vector<GtsVertex*> vcBoxEdgeVertices;

  int sz = vcBoundary.size();

  assert(sz > 2);
  assert(vcVectors.size() == sz);

  for(int i = 0; i < sz; i++)
  {
    // we have a vertex on the edge of the surface
    // we now want the 2D vector pointing outside the surface
    // we will then create new points at the depth of the current vertex along
    // the line described by the vector until we are at the given bounding box
    // at a distance given by dOffset

    // This is only possible because we are dealing with convex surfaces, so
    // the new points will not conflict
    GtsVertex *p = vcBoundary[i];
    const geo::CVector &v = vcVectors[i];
    double dZ = p->p.z;

    while(NextInBBox(p, v, box))
    {
      p = gts_vertex_new(gts_vertex_class(), p->p.x + v.X(), p->p.y + v.Y(), dZ);
      g_slist_append(m_pCopyPointCloud, (gpointer) p);
    }

    // create last point exactly on the box
    GtsVertex *pNew = CreateVertexOnBBox(p, v, box);
    if(pNew)
    {
      g_slist_append(m_pCopyPointCloud, (gpointer) pNew);
      vcBoxEdgeVertices.push_back(pNew);
    }
    else vcBoxEdgeVertices.push_back(p);
  }

  // finally, create the corner points of the box
  assert(vcBoxEdgeVertices.size() > 0);
  InsertBoxCorner(box.Min().X(), box.Min().Y(), vcBoxEdgeVertices);
  InsertBoxCorner(box.Max().X(), box.Min().Y(), vcBoxEdgeVertices);
  InsertBoxCorner(box.Max().X(), box.Max().Y(), vcBoxEdgeVertices);
  InsertBoxCorner(box.Min().X(), box.Max().Y(), vcBoxEdgeVertices);
}

void CTSSurface::CreateGtsSurfaceObjectFromPoints(GSList *pPointCloud, ITSProgressBase* pProgress)
{
  GtsTriangle *t;
  GtsVertex *v1, *v2, *v3;

  // Add all the new points to the surface and create a new surface.
  t = gts_triangle_enclosing(gts_triangle_class(), pPointCloud, 100.);
  gts_triangle_vertices(t, &v1, &v2, &v3);

  // Destroy the surface because we are about to build a new one..
  gts_allow_floating_vertices = TRUE;
  if(m_pGtsSurface) gts_object_destroy(GTS_OBJECT(m_pGtsSurface));
  gts_allow_floating_vertices = FALSE;

  m_pGtsSurface = gts_surface_new(gts_surface_class(),
         gts_face_class(),
         gts_edge_class(),
         gts_vertex_class());

  gts_surface_add_face(m_pGtsSurface, gts_face_new(gts_face_class(), t->e1, t->e2, t->e3));

  GSList *l = pPointCloud;
  while (l)
  {
    gts_delaunay_add_vertex(m_pGtsSurface, (GtsVertex *) l->data, NULL);
    l = l->next;
    if(pProgress) pProgress->Step();
  }

  // Destroy all the temporary objects.
  gts_allow_floating_vertices = TRUE;
  gts_object_destroy(GTS_OBJECT(v1));
  gts_object_destroy(GTS_OBJECT(v2));
  gts_object_destroy(GTS_OBJECT(v3));
  gts_allow_floating_vertices = FALSE;
}


// Resize the surface to the size given by the box parameter and with an offset for the precision.
bool CTSSurface::Increase(geo::CBox box, double dOffset /* = 0 */)
{
  bool ret = false;

  geo::CPoint minpoint = box.Min();
  geo::CPoint maxpoint = box.Max();

  // No offset is given so lets calculate the average edge size and use it instead.
  if (dOffset == 0) dOffset = CalcAverageEdgeSize();

  double dGtsBBoxX1 = m_pGtsBBox->x1;
  double dGtsBBoxY1 = m_pGtsBBox->y1;
  double dGtsBBoxX2 = m_pGtsBBox->x2;
  double dGtsBBoxY2 = m_pGtsBBox->y2;
  double dHalfEps = EPS / 2;

  // Check if the resizing box is larger then the bounding box and the precision offset is larger then 0.
  if ((dOffset > 0) && ((dGtsBBoxX1 - minpoint.X() > dHalfEps) || (dGtsBBoxY1 - minpoint.Y() > dHalfEps) || (dGtsBBoxX2 - maxpoint.X() < -dHalfEps) || (dGtsBBoxY2 - maxpoint.Y() < -dHalfEps)))
  {
    InvalidateCache();

    // Delete the copy of the pointclound if present.
    if(m_pCopyPointCloud) 
    { 
      g_slist_free(m_pCopyPointCloud); 
      m_pCopyPointCloud = NULL; 
    }

    // Create a copy of the original pointclound.
    GSList *l = m_pPointCloud;
    while (l)
    {
      GtsVertex *v = gts_vertex_new(gts_vertex_class(),((GtsVertex *)l->data)->p.x,((GtsVertex *)l->data)->p.y,((GtsVertex *)l->data)->p.z);
      m_pCopyPointCloud = g_slist_append(m_pCopyPointCloud, (gpointer) v);
      l = l->next;
    }

    // recreate the surface, so Gts functions will work properly
    CreateGtsSurfaceObjectFromPoints(m_pCopyPointCloud);

    // get (sorted) vertices on the boundary
    std::vector<GtsVertex*> vcBoundary = SurfaceBoundary();

    // compute area so we know whether winding order is right or left
    double dArea = BoundaryArea(vcBoundary);
    assert(fabs(dArea) > EPS);

    std::vector<geo::CVector> vcVectors;
    GenerateBoundaryIncreaseVectors(vcBoundary, (dArea < 0), vcVectors, dOffset);
    InsertExtensionPoints(vcBoundary, vcVectors, box);

    CreateGtsSurfaceObjectFromPoints(m_pCopyPointCloud);

    BuildInterface();

#ifdef _DEBUG
    geo::CPoint surfmin = Min();
    geo::CPoint surfmax = Max();
    assert(Min().X() < box.Min().X() + EPS);
    assert(Max().X() > box.Max().X() - EPS);
    assert(Min().Y() < box.Min().Y() + EPS);
    assert(Max().Y() > box.Max().Y() - EPS);
#endif

    ret = true;
  }

  return ret;
}
/*
bool CTSSurface::Decrease(double dXNOffset, double dYNOffset, double dXMOffset, double dYMOffset)
{
  bool ret = false;

  // Check if the resizing box is smaller then the bounding box.
  if ((m_pGtsBBox->x2-m_pGtsBBox->x1 > dXNOffset+dXMOffset) && (m_pGtsBBox->y2-m_pGtsBBox->y1 > dYNOffset+dYMOffset))
  {
    double dAverageEdgeSize = CalcAverageEdgeSize();

    // Delete the copy of the pointclound if present.
    if(m_pCopyPointCloud) 
    { 
      g_slist_free(m_pCopyPointCloud); 
      m_pCopyPointCloud = NULL; 
    }

    GtsVertex *v;
    GSList *l = m_pPointCloud;
    while (l)
    {
      // Check if points are inside the new boundary.
      if ((((GtsVertex *)l->data)->p.x > m_pGtsBBox->x1+dXNOffset) && (((GtsVertex *)l->data)->p.x < m_pGtsBBox->x2-dXMOffset) &&
        (((GtsVertex *)l->data)->p.y > m_pGtsBBox->y1+dYNOffset) && (((GtsVertex *)l->data)->p.y < m_pGtsBBox->y2-dYMOffset))
      {
        v = gts_vertex_new(gts_vertex_class(),((GtsVertex *)l->data)->p.x,((GtsVertex *)l->data)->p.y,((GtsVertex *)l->data)->p.z);
        m_pCopyPointCloud = g_slist_append(m_pCopyPointCloud, (gpointer) v);
      }
      l = l->next;
    }

    // Create the corners of the boundary.
    double z;
    if (InterpolateValue(geo::CPoint(m_pGtsBBox->x1+dXNOffset,m_pGtsBBox->y1+dYNOffset,0), &z))
    {
      v = gts_vertex_new(gts_vertex_class(),m_pGtsBBox->x1+dXNOffset,m_pGtsBBox->y1+dYNOffset, z);
      m_pCopyPointCloud = g_slist_append(m_pCopyPointCloud, (gpointer) v);
    }
    if (InterpolateValue(geo::CPoint(m_pGtsBBox->x2-dXMOffset,m_pGtsBBox->y1+dYNOffset,0), &z))
    {
      v = gts_vertex_new(gts_vertex_class(),m_pGtsBBox->x2-dXMOffset,m_pGtsBBox->y1+dYNOffset, z);
      m_pCopyPointCloud = g_slist_append(m_pCopyPointCloud, (gpointer) v);
    }
    if (InterpolateValue(geo::CPoint(m_pGtsBBox->x1+dXNOffset,m_pGtsBBox->y2-dYMOffset,0), &z))
    {
      v = gts_vertex_new(gts_vertex_class(),m_pGtsBBox->x1+dXNOffset,m_pGtsBBox->y2-dYMOffset, z);
      m_pCopyPointCloud = g_slist_append(m_pCopyPointCloud, (gpointer) v);
    }
    if (InterpolateValue(geo::CPoint(m_pGtsBBox->x2-dXMOffset,m_pGtsBBox->y2-dYMOffset,0), &z))
    {
      v = gts_vertex_new(gts_vertex_class(),m_pGtsBBox->x2-dXMOffset,m_pGtsBBox->y2-dYMOffset, z);
      m_pCopyPointCloud = g_slist_append(m_pCopyPointCloud, (gpointer) v);
    }

    // Create the sides of the boundary.
    int iNum;
    double dExtra, dDistance;

    dDistance = fabs(m_pGtsBBox->x2-dXMOffset) - fabs(m_pGtsBBox->x1+dXNOffset);
    iNum = (int)floor(dDistance / dAverageEdgeSize);
    dDistance = dAverageEdgeSize + (dDistance - (iNum * dAverageEdgeSize))/iNum;
    dExtra = dDistance;
    while (dExtra < m_pGtsBBox->x2-dXMOffset)
    {
      if (InterpolateValue(geo::CPoint(m_pGtsBBox->x1+dXNOffset+dExtra ,m_pGtsBBox->y1+dYNOffset,0), &z))
      {
        v = gts_vertex_new(gts_vertex_class(),m_pGtsBBox->x1+dXNOffset+dExtra,m_pGtsBBox->y1+dYNOffset, z);
        m_pCopyPointCloud = g_slist_append(m_pCopyPointCloud, (gpointer) v);
      }
      if (InterpolateValue(geo::CPoint(m_pGtsBBox->x1+dXNOffset+dExtra ,m_pGtsBBox->y2-dYMOffset,0), &z))
      {
        v = gts_vertex_new(gts_vertex_class(),m_pGtsBBox->x1+dXNOffset+dExtra,m_pGtsBBox->y2-dYMOffset, z);
        m_pCopyPointCloud = g_slist_append(m_pCopyPointCloud, (gpointer) v);
      }
      dExtra = dExtra + dDistance;
    }

    dDistance = fabs(m_pGtsBBox->y2-dYMOffset) - fabs(m_pGtsBBox->y1+dYNOffset);
    iNum = (int)floor(dDistance / dAverageEdgeSize);
    dDistance = dAverageEdgeSize + (dDistance - (iNum * dAverageEdgeSize))/iNum;
    dExtra = dDistance;
    while (dExtra < m_pGtsBBox->y2-dYMOffset)
    {
      if (InterpolateValue(geo::CPoint(m_pGtsBBox->x1+dXNOffset, m_pGtsBBox->y1+dYNOffset+dExtra,0), &z))
      {
        v = gts_vertex_new(gts_vertex_class(), m_pGtsBBox->x1+dXNOffset, m_pGtsBBox->y1+dYNOffset+dExtra, z);
        m_pCopyPointCloud = g_slist_append(m_pCopyPointCloud, (gpointer) v);
      }
      if (InterpolateValue(geo::CPoint(m_pGtsBBox->x2-dXMOffset, m_pGtsBBox->y1+dYNOffset+dExtra,0), &z))
      {
        v = gts_vertex_new(gts_vertex_class(), m_pGtsBBox->x2-dXMOffset, m_pGtsBBox->y1+dYNOffset+dExtra, z);
        m_pCopyPointCloud = g_slist_append(m_pCopyPointCloud, (gpointer) v);
      }
      dExtra = dExtra + dDistance;
    }

    // Destroy the surface because we are about to build a new one..
    gts_allow_floating_vertices = TRUE;
    if(m_pGtsSurface) gts_object_destroy(GTS_OBJECT(m_pGtsSurface));
    gts_allow_floating_vertices = FALSE;

    // Add all the new points to the surface and create a new surface.
    GtsTriangle *t;
    GtsVertex *v1, *v2, *v3;
    t = gts_triangle_enclosing(gts_triangle_class(), m_pCopyPointCloud, 100.);
    gts_triangle_vertices(t, &v1, &v2, &v3);

    m_pGtsSurface = gts_surface_new(gts_surface_class(),
           gts_face_class(),
           gts_edge_class(),
           gts_vertex_class());

    gts_surface_add_face(m_pGtsSurface, gts_face_new(gts_face_class(), t->e1, t->e2, t->e3));

    l = m_pCopyPointCloud;
    while (l)
    {
      gts_delaunay_add_vertex(m_pGtsSurface, (GtsVertex *) l->data, NULL);
      l = l->next;
    }

    // Destroy all the temporary objects.
    gts_allow_floating_vertices = TRUE;
    gts_object_destroy(GTS_OBJECT(v1));
    gts_object_destroy(GTS_OBJECT(v2));
    gts_object_destroy(GTS_OBJECT(v3));
    gts_allow_floating_vertices = FALSE;

    m_vcElements.clear();
    m_mpFaceMap.clear();
    m_mpPoint.clear();
    InvalidateCache();

    BuildInterface();
    
    ret = true;
  }

  return ret;
}
*/
bool CTSSurface::Original()
{
  bool ret = false;

  // Delete the copy of the pointclound if present. If it is not present then there is
  // no need to rebuild the original surface.
  if(m_pCopyPointCloud) 
  { 
    g_slist_free(m_pCopyPointCloud); 
    m_pCopyPointCloud = NULL;

    // Destroy the surface because we are about to build a new one..
    gts_allow_floating_vertices = TRUE;
    if(m_pGtsSurface) gts_object_destroy(GTS_OBJECT(m_pGtsSurface));
    gts_allow_floating_vertices = FALSE;

    // Add all the original points to the surface and create a new surface.
    GtsTriangle *t;
    GtsVertex *v1, *v2, *v3;
    t = gts_triangle_enclosing(gts_triangle_class(), m_pPointCloud, 100.);
    gts_triangle_vertices(t, &v1, &v2, &v3);

    m_pGtsSurface = gts_surface_new(gts_surface_class(),
             gts_face_class(),
             gts_edge_class(),
             gts_vertex_class());

    gts_surface_add_face(m_pGtsSurface, gts_face_new(gts_face_class(), t->e1, t->e2, t->e3));

    GSList *l = m_pPointCloud;
    while (l)
    {
      gts_delaunay_add_vertex(m_pGtsSurface, (GtsVertex *) l->data, NULL);
      l = l->next;
    }

    // Destroy all the temporary objects.
    gts_allow_floating_vertices = TRUE;
    gts_object_destroy(GTS_OBJECT(v1));
    gts_object_destroy(GTS_OBJECT(v2));
    gts_object_destroy(GTS_OBJECT(v3));
    gts_allow_floating_vertices = FALSE;

    InvalidateCache();

    BuildInterface();

    ret = true;
  }

  return ret;
}

void CTSSurface::Rotate(const geo::IVector &vec, const double &dAngleDeg)
{
  for(int i = 0; i < PointSize(); i++)
  {
    PointAt(i).Rotate(vec, dAngleDeg);
  }
}

std::vector<int> CTSSurface::Nodes(const geo::IElement &element) const
{
  std::vector<int> vcRet;
  assert(element.IndexingElementSet() == this);
  for(int i = 0; i < 3; i++)
  {
    vcRet.push_back(element.PointIndex(i));
  }
  return vcRet;
}
