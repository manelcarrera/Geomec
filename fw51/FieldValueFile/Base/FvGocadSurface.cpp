 /* Copyright (c) 2011 TNO DIANA BV                              Confidential */
#include "IMesh.h"
#include "Line.h"
#include "FvGocadSurface.h"

CSurfaceHeader::CSurfaceHeader(const QString &sName)
: CGoCadHeader(sName)
{
}

CGoCadSurface::CTriGoCadFace::CTriGoCadFace(CGoCadSurface& surface, int nPoint1, int nPoint2, int nPoint3)
: m_surface(surface)
{
  m_nodes[0] = nPoint1;
  m_nodes[1] = nPoint2;
  m_nodes[2] = nPoint3;
  surface.Mesh().RegisterElement(*this);
  surface.AddFace(*this);
}

geo::CValue CGoCadSurface::CTriGoCadFace::InterpolateValue(const geo::IPoint &/*point*/, const std::vector<geo::CValue> &/*values*/) const
{
  assert(false);
  return geo::CValue();
}

int CGoCadSurface::CTriGoCadFace::PointIndex(int nIndex) const
{
  assert(nIndex >= 0 && nIndex < NrOfNodes());
  return m_nodes[nIndex];
}

size_t CGoCadSurface::CTriGoCadFace::NrOfNodes() const
{
  return 3;
}

const geo::INode& CGoCadSurface::CTriGoCadFace::Node(int nIndex) const
{
  assert(nIndex >= 0 && nIndex < NrOfNodes());
  return m_surface.Mesh().Node(PointIndex(nIndex));
}

void CGoCadSurface::CTriGoCadFace::Node(int nIndex, const geo::IPoint& point)
{
  assert(nIndex >= 0 && nIndex < NrOfNodes());
  m_surface.Mesh().Node(PointIndex(nIndex), point);
}

const geo::IElementSet* CGoCadSurface::CTriGoCadFace::IndexingElementSet() const
{
  return &m_surface.Mesh();
}

/*virtual*/ std::string CGoCadSurface::CTriGoCadFace::Type() const
{
  return "T6EPS";
}


CGoCadSurface::CGoCadSurface(geo::IMesh& mesh, const geo::ISurface &surface, const QString &sName)
: CFaceGroup(mesh)
{
  typedef std::map<geo::CPoint, int> TPointToIndex;
  TPointToIndex mpPoint;                // Maps points to index ...

  // Register points
  int i;
  for(i = 0; i < surface.PointSize(); i++)
  {
  if(mpPoint.insert(TPointToIndex::value_type(geo::CPoint(surface.Point(i)), i)).second) 
    InsertPoint(i, surface.Point(i));

  }

  // Insert triangles ...
  for(i = 0; i < surface.FaceSize(); i++)
  {
  // Test in debug mode of the points of the face exists ...
  assert(surface.Face(i).NrOfPoints() == 3);
#ifdef _DEBUG
  for(int n = 0; n < 3; n++)
      assert(mpPoint.find(surface.Face(i).Point(n)) != mpPoint.end());
#endif //_DEBUG

  // Insert triangle
  InsertTriangle(mpPoint.find(surface.Face(i).Point(0))->second,
             mpPoint.find(surface.Face(i).Point(1))->second,
             mpPoint.find(surface.Face(i).Point(2))->second);

  }

  // Create hdr ...
  Header(sName);
}

CGoCadSurface::CGoCadSurface(geo::IMesh& mesh)
: CFaceGroup(mesh)
{
}

CGoCadSurface::~CGoCadSurface()
{
  delete m_pHeader;
}

bool CGoCadSurface::InsertPoint(int nIndex, const geo::IPoint& pt)
{
  return InsertPoint(nIndex, pt.X(), pt.Y(), pt.Z());
}

bool CGoCadSurface::InsertPoint(int nIndex, const double &x, const double &y, const double &z)
{
  int nMeshIndex = Mesh().RegisterNode(geo::CPoint(x, y, z), false);
  if(!m_mpPoint.insert(TPointMap::value_type(nIndex, nMeshIndex)).second)
  return false;
  return true;
}

/*!
  Returns true when the point index exists
*/
bool CGoCadSurface::isPoint(int nIndex) const
{
  return m_mpPoint.find(nIndex) != m_mpPoint.end();
}

bool CGoCadSurface::InsertTriangle(int nV1, int nV2, int nV3)
{
  if(isPoint(nV1) && isPoint(nV2) && isPoint(nV3) ) {
/*    const geo::CPoint p1(Mesh().Point(m_mpPoint[nV1]));
  const geo::CPoint p2(Mesh().Point(m_mpPoint[nV2]));
  const geo::CPoint p3(Mesh().Point(m_mpPoint[nV3]));
  if(p1.Distance(p2) < EPS ||
       p2.Distance(p3) < EPS ||
       p3.Distance(p1) < EPS)
       return false; // triangle is degenerate.

  geo::CLine line(p1, p2);
  if(line.Distance(p3) < 0.1)
      return false;
*/
  // The face does not exist yet.
  new CTriGoCadFace(*this, m_mpPoint[nV1], m_mpPoint[nV2], m_mpPoint[nV3]);

  return true;

  }

  // The face can not be constructed since (not all of) the incoming indices are present.
  return false;
}

const CSurfaceHeader& CGoCadSurface::Header() const
{
  assert(m_pHeader);
  return *m_pHeader;
}

CSurfaceHeader& CGoCadSurface::Header()
{
  assert(m_pHeader);
  return *m_pHeader;
}

void CGoCadSurface::Header(const QString &sName)
{
  m_pHeader = new CSurfaceHeader(sName);
}


void CGoCadSurface::MergeSurface(CGoCadSurface &surf)
{
  int i, j, k;
  std::vector<int> vcPtIndices;
  for(i = 0; i < surf.FaceSize(); i++)
  {
  vcPtIndices.clear();
  const geo::IFace &face = surf.Face(i);

  for(j = 0; j < 3; j++)
  {
      const geo::IPoint &pt1 = face.Point(j);
      bool point_present = false;

      for(k = 0; k < PointSize(); k++)
      {
    const geo::IPoint &pt2 = Point(k);
    if(pt1.Distance(pt2) < EPS)
    {
          point_present = true;
          vcPtIndices.push_back(Mesh().NodeIndex(pt2)[0]);// since points can only be present once, just take the first index in the vector.
          break;
    }
      }

      if(!point_present)
      {
    vcPtIndices.push_back(Mesh().RegisterNode(geo::CPoint(pt1.X(), pt1.Y(), pt1.Z()), false));
      }
  }

  new CTriGoCadFace(*this, vcPtIndices[0], vcPtIndices[1], vcPtIndices[2]);
  }
}

