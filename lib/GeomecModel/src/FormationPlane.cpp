// FormationPlane.cpp: implementation of the CFormationPlane class.
//
//////////////////////////////////////////////////////////////////////

#include "FormationPlane.h"
#include "HexaFormation.h"
#include "ValueTypes.h"
#include "BodyGroup.h"
#include "Hexahedron.h"

CFormationPlane::plane_face::plane_face(const CPlaneSurface &surface, int p1, int p2, int p3, int p4) : m_surface(surface)
{
  m_pointindices[0] = p1;
  m_pointindices[1] = p2;
  m_pointindices[2] = p3;
  m_pointindices[3] = p4;
}

const geo::IPoint &CFormationPlane::plane_face::Point(const int nIndex) const
{
  assert(nIndex >= 0 && nIndex < 4);
  assert(m_pointindices[nIndex] >= 0 && m_pointindices[nIndex] < m_surface.PointSize());
  return m_surface.Point(m_pointindices[nIndex]);
}

void CFormationPlane::plane_face::Point(const int /*nIndex*/, const geo::IPoint &/*pt*/)
{
  assert(false);
}

int CFormationPlane::plane_face::NrOfPoints() const
{
  return 4;
}

double CFormationPlane::plane_face::Size() const
{
  assert(false);
  return 0;
}

geo::CValue CFormationPlane::plane_face::InterpolateValue(const geo::IPoint &/*point*/, const std::vector<geo::CValue> &/*values*/) const
{
  assert(false);
  return geo::CValue();
}

int CFormationPlane::plane_face::PointIndex(int nIndex) const
{
  assert(nIndex >= 0 && nIndex < 4);
  return m_pointindices[nIndex];
}

//// CPlaneSurface implementation
CFormationPlane::CPlaneSurface::CPlaneSurface()
{
}

CFormationPlane::CPlaneSurface::~CPlaneSurface()
{
  for(size_t i = 0; i < m_vcFaces.size(); i++)
  delete m_vcFaces[i];
}

// give the points to be inserted, it returns the indices of those points in the surface
std::vector<int> CFormationPlane::CPlaneSurface::InsertPoints(const std::vector<geo::CPoint> &points)
{
  std::vector<int> indices(4);
  for(int i = 0; i < 4; i++)
  {
    std::pair<TPointMap::iterator, bool> prInsert = m_mpPoints.insert(std::make_pair(points[i], -1));
    if(prInsert.second)
    {
      int sz = m_vcPoints.size();
      m_vcPoints.push_back(&prInsert.first->first);
      prInsert.first->second = sz;
    }

    assert(prInsert.first->second >= 0);
    indices[i] = prInsert.first->second;
  }

  return indices;
}

void CFormationPlane::CPlaneSurface::InsertFace(const std::vector<int> &pointindices, const geo::IFace *pFace)
{
  m_vcFaces.push_back(pFace);
  VERIFY(m_mpFaceToPoints.insert(std::make_pair(pFace, pointindices)).second);

  for(int i = 0; i < 4; i++)
  {
    std::pair<TPointToFaceMap::iterator, bool> prInsert = m_mpPointToFace.insert(std::make_pair(pointindices[i], std::vector<const geo::IFace*> ()));
    prInsert.first->second.push_back(pFace);
  }
}

void CFormationPlane::CPlaneSurface::CreateFace(const std::vector<geo::CPoint> &points)
{
  assert(points.size() == 4);

  std::vector<int> indices = InsertPoints(points);
  assert(indices.size() == 4);

  plane_face *p = new plane_face(*this, indices[0], indices[1], indices[2], indices[3]);

  InsertFace(indices, p);
}

const geo::CPtrArray<geo::IFace> CFormationPlane::CPlaneSurface::FacesAt(const geo::IPoint &/*p*/) const
{
  assert(false);
  return geo::CPtrArray<geo::IFace> ();
}

const geo::CPtrArray<geo::IFace> CFormationPlane::CPlaneSurface::FacesAtNode(const geo::IPoint &p) const
{
  TPointMap::const_iterator itPoint = m_mpPoints.find(p);
  assert(itPoint != m_mpPoints.end());
  int index = itPoint->second;
  assert(index >= 0);
  TPointToFaceMap::const_iterator itFaces = m_mpPointToFace.find(index);
  assert(itFaces != m_mpPointToFace.end());
  geo::CPtrArray<geo::IFace> arFaces;
  for(size_t i = 0; i < itFaces->second.size(); i++)
  {
    const geo::IFace *pFace = itFaces->second[i];
    arFaces.PushBack(*(const_cast<geo::IFace*> (pFace)));
  }

  return arFaces;
}

const geo::IFace &CFormationPlane::CPlaneSurface::Face(int nIndex) const
{
  assert(nIndex >= 0 && nIndex < FaceSize());
  return *m_vcFaces[nIndex];
}

int CFormationPlane::CPlaneSurface::FaceSize() const
{
  return m_vcFaces.size();
}

geo::IPoint &CFormationPlane::CPlaneSurface::PointAt(int /*nIndex*/)
{
  assert(false);
  geo::IPoint *p = 0;
  return *p;
}

const geo::IPoint& CFormationPlane::CPlaneSurface::Point(int nIndex) const
{
  assert(nIndex >= 0 && nIndex < m_vcPoints.size());
  return *m_vcPoints[nIndex];
}

int CFormationPlane::CPlaneSurface::PointSize() const
{
  return m_vcPoints.size();
}

// get the node indices connected to the given element
std::vector<int> CFormationPlane::CPlaneSurface::Nodes(const geo::IElement &element) const
{
  const geo::IFace *pFace = dynamic_cast<const geo::IFace*> (&element);
  assert(pFace != 0);
  TFaceToPointsMap::const_iterator it = m_mpFaceToPoints.find(pFace);
  assert(it != m_mpFaceToPoints.end());

  return it->second;
}

//=============================================================================
CFormationPlane::CFormationPlane(ePlaneType type, CHexaFormation &formation)
: CColorNode("", formation.Model(), formation.Color()), m_pSurface(0), m_type(type), m_formation(formation)
{
  // Give the formation plane a proper name
  QString sName;
  switch(type)
  {
  case FP_BOTTOM:
    sName = getStringTableEntry(IDS_PLANE_BOTTOM);
    break;
  case FP_MIDDLE:
    sName = getStringTableEntry(IDS_PLANE_MIDDLE);
    break;
  case FP_TOP:
    sName = getStringTableEntry(IDS_PLANE_TOP);
    break;
  default:
    break;
  }
  Name(sName);

  // Link to the formation
  reParent(&formation);
  
  assert(Dirty());
}

CFormationPlane::CFormationPlane(const CFormationPlane& rhs)
:CColorNode(rhs), m_pSurface(0), m_type(rhs.m_type), m_formation(rhs.m_formation)
{
}

bool CFormationPlane::operator==(const CFormationPlane& plane) const
{
  if(!CColorNode::operator ==(plane))
    return false;

  return true;
}
  
CFormationPlane& CFormationPlane::operator=(const CFormationPlane& plane)
{
  CColorNode::operator =(plane);

  return *this;
}

bool CFormationPlane::UpdatePlane() const
{
  if(m_formation.ElementSetSize() <1)
    return false;

  // Default we delete no faces
  const CModelBase& model = dynamic_cast<const CModelBase&>(Model());
  if(!model.IsMesh())
    return false;	// Cannot mesh, because mesh is locked or for other reasons

  // Initialize start
  int nStart = 0;
  switch(m_type)
  {
  case FP_TOP:
    nStart = 0;
    break;
  case FP_MIDDLE:
    nStart = m_formation.Elements() / 2;
    break;
  case FP_BOTTOM:
    nStart = m_formation.Elements() - 1;
    break;
  }

  std::set<const geo::IPoint*, CPointLess> stPoints;
  std::pair<std::set<const geo::IPoint*, CPointLess>::iterator, bool> prItBool;
  stPoints.clear();

  if(m_pSurface) delete m_pSurface;
  bool bMustDeleteFaces = (m_type == FP_MIDDLE && (div(m_formation.Elements(), 2).rem != 0));

  if(bMustDeleteFaces)
    m_pSurface = new CPlaneSurface;
  else
  m_pSurface = new CMeshSurface;

  // The hexahedrons are ordered from top to bottom so skip m_nElements for the next hexahedron.
  for(int i = nStart; i < m_formation.Volume(0).Volume().ElementSize(); i+=m_formation.Elements())
  {
    const geo::CHexahedron *hex = dynamic_cast<const geo::CHexahedron*>(&m_formation.Volume(0).Volume().Element(i));
    assert(hex);

    switch(m_type)
    {
    case FP_TOP:
      // The upper formation plane or halfway the formation but upper face of the hexahedron.
      assert(dynamic_cast<CMeshSurface*>(m_pSurface) != 0);
      (static_cast<CMeshSurface*>(m_pSurface))->AddFace(hex->Face(HEXA_UPPERFACE));
      break;
    case FP_MIDDLE:
      if(div(m_formation.Elements(), 2).rem != 0)
      {
        // The formation plane is halfway a formation and a hexahedron.
        std::vector<geo::CPoint> points(4);
        const geo::IFace *face1, *face2;
        face1 = &hex->Face(HEXA_LOWERFACE);
        face2 = &hex->Face(HEXA_UPPERFACE);

        points[0] = geo::CPoint(face1->Point(0).X(), face1->Point(0).Y(), (face1->Point(0).Z() + face2->Point(1).Z()) / 2);
        points[1] = geo::CPoint(face1->Point(1).X(), face1->Point(1).Y(), (face1->Point(1).Z() + face2->Point(0).Z()) / 2),
        points[2] = geo::CPoint(face1->Point(2).X(), face1->Point(2).Y(), (face1->Point(2).Z() + face2->Point(3).Z()) / 2),
        points[3] = geo::CPoint(face1->Point(3).X(), face1->Point(3).Y(), (face1->Point(3).Z() + face2->Point(2).Z()) / 2);

    assert(dynamic_cast<CPlaneSurface*>(m_pSurface) != 0);
        (static_cast<CPlaneSurface*>(m_pSurface))->CreateFace(points);
      }
      else
      {
    assert(dynamic_cast<CMeshSurface*>(m_pSurface) != 0);
        (static_cast<CMeshSurface*>(m_pSurface))->AddFace(hex->Face(HEXA_UPPERFACE));
      }
      break;
    case FP_BOTTOM:
      assert(dynamic_cast<CMeshSurface*>(m_pSurface) != 0);
      (static_cast<CMeshSurface*>(m_pSurface))->AddFace(hex->Face(HEXA_LOWERFACE));
      break;
    default:
      assert(false);
    }
  }

  return true;
}

CFormationPlane::~CFormationPlane()
{
  Clear();
  assert(Dirty());
}

unsigned int CFormationPlane::IconId() const
{
  return IDI_FORMATIONPLANE;
}

unsigned int CFormationPlane::TypeId() const
{
  return IDT_FORMATIONPLANE;
}

QString CFormationPlane::TypeName() const
{
  return getStringTableEntry(IDS_FORMATIONPLANE);
}

int CFormationPlane::DisplayListSize() const
{ 
  if(Dirty())
  {
    if(UpdatePlane())
      return 1;
    return 0;
  }
  return 1;
}

const geo::IObject& CFormationPlane::DisplayList(int nIndex) const
{
  assert(nIndex == 0);
  return PlaneSurface();
}

bool CFormationPlane::Less(const CGraphNode &node) const
{
  const CFormationPlane* pPlane = dynamic_cast<const CFormationPlane*> (&node);
  if(pPlane)
  {
    return m_type > pPlane->m_type;
  }

  return CColorNode::Less(node);
}

void CFormationPlane::OnNeighbourModified(const CGraphNode &node, enum ModifiedHint uHint)
{
  if(&m_formation == &node)
    Clear();

  CColorNode::OnNeighbourModified(node, uHint);
}

void CFormationPlane::OnNeighbourDeleted(const CGraphNode& node)
{
  if(&m_formation == &node)
    delete this;

  CColorNode::OnNeighbourDeleted(node);
}

int CFormationPlane::NrOfFaces() const
{
  return m_pSurface ? m_pSurface->FaceSize() : 0;
}

bool CFormationPlane::Dirty() const
{
  return NrOfFaces() == 0;
}

void CFormationPlane::Clear()
{
  if(m_pSurface) delete m_pSurface;
  m_pSurface = 0;
  Modified();
}

CHexaFormation &CFormationPlane::Formation()
{
  return m_formation;
}

const CHexaFormation &CFormationPlane::Formation() const
{
  return m_formation;
}

CFormationPlane::ePlaneType CFormationPlane::PlaneType() const
{
  return m_type;
}

bool CFormationPlane::CanDestroy() const
{
  return false;
}

const geo::ISurface& CFormationPlane::PlaneSurface() const
{
  if(Dirty())
    UpdatePlane();
  assert(!Dirty());

  return *m_pSurface;
}



