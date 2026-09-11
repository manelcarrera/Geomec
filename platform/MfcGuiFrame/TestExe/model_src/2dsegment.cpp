// 2DSegment.cpp: implementation of the C2DSegment class.
//
//////////////////////////////////////////////////////////////////////

#include "2DPolyLine.h"
#include "2DSegment.h"
#include "2DVertex.h"
#include "2DDocument.h"

/////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

C2DSegment::C2DSegment(IModelObject& parent)
: IModelObject(parent), m_segment(*this), m_prVertices(0, 0)
{
}

C2DSegment::C2DSegment(IModelObject& parent, C2DVertex &first, C2DVertex &second)
: IModelObject(parent), m_segment(*this), m_prVertices(&first, &second)
{
  insertReference(first);
  insertReference(second);
  first.insertReference(*this);
  second.insertReference(*this);
}

C2DSegment::~C2DSegment()
{
}

const C2DVertex &C2DSegment::FirstVertex() const
{
  return *m_prVertices.first;
}

C2DVertex &C2DSegment::FirstVertex()
{
  return *m_prVertices.first;
}

const C2DVertex &C2DSegment::SecondVertex() const
{
  return *m_prVertices.second;
}

C2DVertex &C2DSegment::SecondVertex()
{
  return *m_prVertices.second;
}

C2DSegment::FirstVertex(C2DVertex& vertex)
{
  removeReference(FirstVertex());
  FirstVertex().removeReference(*this);
  insertReference(vertex);
  m_prVertices.first = &vertex;
  geometryChanged();
}

C2DSegment::SecondVertex(C2DVertex& vertex)
{
  removeReference(SecondVertex());
  SecondVertex().removeReference(*this);
  insertReference(vertex);
  m_prVertices.second = &vertex;
  geometryChanged();
}

const C2DVertex &C2DSegment::OtherVertex(const C2DVertex &vertex) const
{
  if(&FirstVertex() == &vertex)
    return SecondVertex();
  ASSERT(&SecondVertex() == &vertex);
  return FirstVertex();
}

C2DVertex &C2DSegment::OtherVertex(const C2DVertex &vertex)
{
  if(&FirstVertex() == &vertex)
    return SecondVertex();
  ASSERT(&SecondVertex() == &vertex);
  return FirstVertex();
}

const geo::ISegment &C2DSegment::Segment() const
{
  return m_segment;
}
/*
UINT C2DSegment::IconId() const
{
  return 0;
}

UINT C2DSegment::TypeId() const
{
  return 0;
}
void C2DSegment::OnNewNeighbour(const CGraphNode &node)
{
  const C2DPolyLine* pPolyLine = dynamic_cast<const C2DPolyLine*>(&node);
  if(pPolyLine)
    m_vc2DPolyLines.push_back((C2DPolyLine*)(&node));
  
  CGraphNode::OnNewNeighbour(node);
}

void C2DSegment::OnNeighbourModified(const CGraphNode& node, UINT uHint)
{
  if((m_prVertices.first == &node) || (m_prVertices.second == &node))
    Modified();
  CGraphNode::OnNeighbourModified(node, uHint);
}

void C2DSegment::OnNeighbourDeleted(const CGraphNode &node)
{
  for(int i = 0; i < m_vc2DPolyLines.size(); i++)
  {
    if(m_vc2DPolyLines[i] == &node)
    {
      m_vc2DPolyLines.erase(m_vc2DPolyLines.begin() + i);
    }
  }

  if(PolyLineSize() == 0)
    delete this;
  else
    CGraphNode::OnNeighbourDeleted(node);
}

BOOL C2DSegment::Empty() const
{
  return (m_prVertices.first == 0) && (m_prVertices.second == 0);
}

long C2DSegment::SavedItems() const
{
  return CStorageNode::SavedItems() + 2 + m_vcEdge.size();
}

void C2DSegment::LoadStream(TSTREAM& stream, CVersion& version, TPROGRESS& progress)
{
  CStorageNode::LoadStream(stream, version, progress);
  
  // Load first vertex
  int nIndex;
  stream >> nIndex;
  m_prVertices.first = (C2DVertex*)GetNodeFromEntry(MD_2D_VERTEX, nIndex);
  progress.Step();
  LinkTo(*m_prVertices.first);

  /// Load second vertex
  stream >> nIndex;
  m_prVertices.second = (C2DVertex*)GetNodeFromEntry(MD_2D_VERTEX, nIndex);
  progress.Step();
  LinkTo(*m_prVertices.second);

  // Load edge references
  int nSize;
  stream >> nSize;
  m_vcEdge.resize(nSize);
  for(int i = 0; i < nSize; i++)
  {
    stream >> m_vcEdge[i];
    progress.Step();
  }
    
  ASSERT(Model().GraphEntry(MD_2D_SEGMENT));
  LinkTo(*Model().GraphEntry(MD_2D_SEGMENT));
}

void C2DSegment::SaveStream(TSTREAM& stream, TPROGRESS& progress)
{
  // The polylines will be restored by saving routing of the polylines
  // The mesher will restore the edges
  CStorageNode::SaveStream(stream, progress);

  stream << m_prVertices.first->Index();
  progress.Step();
  stream << m_prVertices.second->Index();
  progress.Step();

  stream << m_vcEdge.size();
  for(int i = 0; i < m_vcEdge.size(); i++)
  {
    stream << m_vcEdge[i];
    progress.Step();
  }
}
*/
int C2DSegment::PolyLineSize() const
{
  return m_vc2DPolyLines.size();
}

const C2DPolyLine& C2DSegment::PolyLine(int nIndex) const
{
  return *m_vc2DPolyLines[nIndex];
}

C2DPolyLine& C2DSegment::PolyLine(int nIndex)
{
  return *m_vc2DPolyLines[nIndex];
}
/*
int C2DSegment::EdgeSize() const
{
  return m_vcEdge.size();
}

const C2DMesher::CEdgeSurrogate& C2DSegment::Edge(int nIndex) const
{
  const CModelBase& model = dynamic_cast<const CModelBase&>(Model());
  const C2DMesher& mesher = dynamic_cast<const C2DMesher&>(model.Mesh().Mesh());
  return mesher.Edge(m_vcEdge[nIndex]);
}

void C2DSegment::ClearEdges()
{
  m_vcEdge.clear();
  for(int i = 0; i < PolyLineSize(); i++)
    PolyLine(i).ClearEdges();
}

void C2DSegment::AddEdge(int nIndex)
{
  m_vcEdge.push_back(nIndex);
}
*/
bool C2DSegment::CanSplit(const geo::IPoint& point) const
{
  return m_segment.Contains(point, true);
}

void C2DSegment::Split(const geo::IPoint& point)
{
  ASSERT(CanSplit(point));

  // Get the position of the current edge in polylines
  std::vector<std::pair<int, bool> > vcIndex;
  for(int i = 0; i < PolyLineSize(); i++)
    vcIndex.push_back(PolyLine(i).PolyLine().SegmentIndex(*this));
  ASSERT(vcIndex.size() == PolyLineSize());

  // We introduce a new point and segment
  C2DDocument& doc = dynamic_cast<C2DDocument&>(document());
  C2DVertex *pVertex = new C2DVertex(point, doc.Vertices());
  C2DSegment *pSegment = new C2DSegment(doc.Segments(), *pVertex, SecondVertex());
  SecondVertex(*pVertex);

  // Introduce the new segment in the containing polylines
  for(i = 0; i < PolyLineSize(); i++)
  {
    if(vcIndex[i].second)
      PolyLine(i).PolyLine().Insert(i + 1, *pSegment);
    else
      PolyLine(i).PolyLine().Insert(i, *pSegment);
  }
}

void C2DSegment::InsertSegment(C2DPolyLine& poly_line, C2DSegment& old_segment, C2DSegment& new_segment)
{
  // Find index of old segment
  int nIndex = -1;
  for(int i = 0; i < poly_line.PolyLine().LineSize(); i++)
  {
    if(&poly_line.PolyLine().Segment(i) == &old_segment)
      nIndex = i;
  }
  ASSERT(nIndex != -1);

  // Insert before or after
  bool bInsertBefore = true;
  if(nIndex > 0)
  {
    

  }


}

//// CSegment
C2DSegment::CSegment::CSegment(const C2DSegment &segment)
: m_segment(segment)
{
}

C2DSegment::CSegment::~CSegment()
{
}

C2DSegment::CSegment &C2DSegment::CSegment::operator=(const CSegment &rhs)
{
  assert(&m_segment == &rhs.m_segment);

  return *this;
}

const C2DSegment& C2DSegment::CSegment::Segment() const
{
  return m_segment;
}

const geo::ISegmentPoint &C2DSegment::CSegment::FirstSegmentPoint() const
{
  return Segment().FirstVertex().Point();
}

const geo::ISegmentPoint &C2DSegment::CSegment::SecondSegmentPoint() const
{
  return Segment().SecondVertex().Point();
}

void C2DSegment::CSegment::First(const geo::IPoint& point)
{
  assert(false);
}

void C2DSegment::CSegment::Second(const geo::IPoint& point)
{
  assert(false);
}

// Segment entry
C2DSegmentContainer::C2DSegmentContainer(C2DDocument& document)
: CModelContainer<C2DSegment>("", document)
{
}

std::vector<C2DSegmentContainer::TIntersection> C2DSegmentContainer::Intersection(geo::ILine& line) const
{
  std::vector<TIntersection> vcRet;
  for(int i = 0; i < size(); i++)
  {
    geo::CPoint point = at(i).Segment().Intersection(line);
    if(!point.Empty())
    {
      if(line.Contains(point) && at(i).Segment().Contains(point))
        vcRet.push_back(TIntersection(&at(i), point));
    }
  }
  return vcRet;	
}


