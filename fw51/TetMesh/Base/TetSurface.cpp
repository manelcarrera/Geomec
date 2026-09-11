 /* Copyright (c) 2011 TNO DIANA BV                              Confidential */
#include <cassert>
#include "TetSurface.h"
#include "SurfaceDesc.h"
#include "IBody.h"
#include "IFace.h"
#include "BodyTriangle.h"

#include <tbb/spin_mutex.h>

namespace {

// See comment in Geometry's IFace
// Using a normal spin_mutex here, as we lock in the creating method, not in the callers (because we have quite a few)

tbb::spin_mutex myGlobalCTetSurfaceMutex[16];

}

namespace geo {

/*!
  Contructs of tetrahedron surface for a bodygroup coupled to a surface descriptor.
  \sa CSurfaceDesc
*/
CTetSurface::CTetSurface(CSurfaceDesc& desc, CBodyGroup& body_group)
: CBodyGroup::CSideSurface(body_group), m_descriptor(desc)
{
  desc.m_vcTetSurface.push_back(this);
}

/*!
  Remove surfaces from the descriptor
*/
CTetSurface::~CTetSurface()
{
  // Remove from descriptor and volume
  for(int i = 0; i < m_descriptor.m_vcTetSurface.size(); i++)
  {
  if(m_descriptor.m_vcTetSurface[i] == this)
  {
      m_descriptor.m_vcTetSurface.erase(m_descriptor.m_vcTetSurface.begin() + i);
      break;
  }
  }

}

/*!
  Get the point at the given index
  \sa PointSize()
*/
const IPoint& CTetSurface::Point(int nIndex) const
{
  assert(BodyGroup().BodySize() > 0);

  // Is everthing in place
  if(CSideSurface::FaceSize() == 0)
  FillFaceVec();

  return CSideSurface::Point(nIndex);
}

/*!
  Get the number of points in this surface
  \sa Point()
*/
int CTetSurface::PointSize() const
{
  assert(BodyGroup().BodySize() > 0);

  // Is everthing in place
  if(CSideSurface::FaceSize() == 0)
  FillFaceVec();

  return CSideSurface::PointSize();
}

/*!
  Node indices will build the surface when this is not done
*/
void CTetSurface::CreateSurface(const std::vector<int>& vcNode)
{
  m_vcPointIndex = vcNode;
}


/*!
  Get the face(s) at the given point,
  which must be a node of the surface (e.g. returned by the Point function)
*/
const CPtrArray <IFace> CTetSurface::FacesAtNode(const IPoint &p) const
{
  // Is everthing in place
  if(CSideSurface::FaceSize() == 0)
  FillFaceVec();

  return CSideSurface::FacesAtNode(p);
}

/*!
  Get the face at the given index
*/
const IFace & CTetSurface::Face(int nIndex) const
{
  // Is everthing in place
  if(CSideSurface::FaceSize() == 0)
  FillFaceVec();

  return CSideSurface::Face(nIndex);
}

/*!
  Get the number of faces in this surface
*/
int CTetSurface::FaceSize() const
{
  // Is everthing in place
  if(CSideSurface::FaceSize() == 0)
  FillFaceVec();

  return CSideSurface::FaceSize();
}

/*!
  Internal function for generating a surface from mesh points
  Inverse of FillPointIndexVec
  \sa FillPointIndexVec
*/
void CTetSurface::FillFaceVec() const
{
  int index = (intptr_t)this >> 6 & 0xf;

  tbb::spin_mutex::scoped_lock lock(myGlobalCTetSurfaceMutex[index]);

  if (CSideSurface::FaceSize() > 0)
  return; // some other thread beat us to it

  assert(CSideSurface::FaceSize() == 0);
  assert(BodyGroup().BodySize() > 0);

  // Fill a map with bodies and nodes
  typedef std::set<int> TNodeSet;
  typedef std::map<const IBody*, TNodeSet> TBodyToNodeMap;
  TBodyToNodeMap mpBody;
  for(int i = 0; i < m_vcPointIndex.size(); i++)
  {
  const IElementSet *pElementSet = BodyGroup().Body(0).IndexingElementSet();
  assert(pElementSet);
  const IPoint& point = pElementSet->Point(m_vcPointIndex[i]);
  const std::vector<const IBody*>& vcBody = BodyGroup().PointToBody(point);
  for(int k = 0; k < vcBody.size(); k++)
  {
      std::pair<TBodyToNodeMap::iterator, bool> ret = mpBody.insert(TBodyToNodeMap::value_type(vcBody[k], TNodeSet()));
      ret.first->second.insert(m_vcPointIndex[i]);
  }
  }

  //
  for(TBodyToNodeMap::const_iterator it = mpBody.begin(); it != mpBody.end(); it++)
  {
  const IBody& body = *it->first;
  if(it->second.size() == 3)
  {
      // Find the face from
      for(int nFace = 0; nFace < body.NrOfFaces(); nFace++)
      {
    bool bFound = true;
    const IFace& face = body.Face(nFace);
    std::vector<int> vcFaceNodeIndices = body.FaceNodeIndices(nFace);

    for(int nNode = 0; (nNode < face.NrOfNodes()) && (bFound); nNode++)
    {
          int nNodeIndex = body.PointIndex(vcFaceNodeIndices[nNode]);
          if(it->second.find(nNodeIndex) == it->second.end())
      bFound = false;
    }
    CTetSurface *pSurface = (CTetSurface*)(this);
    if(bFound)
          pSurface->AddFace(face);
      }
  }
  }
}

/*!
  Returns the number of Node indices
*/
int CTetSurface::NrOfNodeIndices() const
{
  if(m_vcPointIndex.size() == 0)
  {
  assert(FaceSize() > 0);
  FillPointIndexVec();
  }

  return (int)m_vcPointIndex.size();
}

/*!
  Returns the index of the node
  \sa NrOfNodesIndices()
*/
int CTetSurface::NodeIndex(int nIndex) const
{
  if(m_vcPointIndex.size() == 0)
  {
  assert(FaceSize() > 0);
  FillPointIndexVec();
  }

  return m_vcPointIndex[nIndex];
}

/*!
  Generates a nodes vec from a known surface. Inverse of FillFaceVec
  \sa FillFaceVec
*/
void CTetSurface::FillPointIndexVec() const
{
  assert(FaceSize() > 0);
  assert(m_vcPointIndex.size() == 0);
  // Create a set of points
  std::set<int> stPoint;
  for(int nFace = 0; nFace < FaceSize(); nFace++)
  {
  const geo::IFace& face = Face(nFace);
  std::vector<int> vcFaceNodeIndices = face.Parent()->FaceNodeIndices(face);

  for(int i = 0; i < face.NrOfPoints(); i++)
  {
      int nPointIndex = face.Parent()->PointIndex(vcFaceNodeIndices[i]);
      if(stPoint.insert(nPointIndex).second)
    m_vcPointIndex.push_back(nPointIndex);
  }
  }
}

}// Name space geo

