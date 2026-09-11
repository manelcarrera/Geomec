// 2DVertex.cpp: implementation of the C2DVertex class.
//
//////////////////////////////////////////////////////////////////////

#include "2DVertex.h"
#include "2DDocument.h"
#include "2DPolyLine.h"
#include "2DSegment.h"
#include <qcolor.h>

#ifdef _DEBUG
#ifdef _MSC_VER
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif // _MSC_VER
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// The internal segmentpoint
//////////////////////////////////////////////////////////////////////

C2DVertex::CSegmentPoint::CSegmentPoint(C2DVertex &vertex) : m_vertex(vertex) {
  m_coor[0] = 0.0;
  m_coor[1] = 0.0;
  m_coor[2] = 0.0;
}

C2DVertex::CSegmentPoint::CSegmentPoint(const CSegmentPoint &rhs) : m_vertex(rhs.m_vertex) {
  for (int i = 0; i < 3; i++)
    m_coor[i] = rhs.m_coor[i];
}

bool C2DVertex::CSegmentPoint::operator==(const CSegmentPoint &rhs) const {
  for (int i = 0; i < 3; i++) {
    if (m_coor[i] != rhs.m_coor[i])
      return false;
  }

  return true;
}

C2DVertex::CSegmentPoint &C2DVertex::CSegmentPoint::operator=(const CSegmentPoint &rhs) {
  for (int i = 0; i < 3; i++)
    m_coor[i] = rhs.m_coor[i];
  return *this;
}

const double &C2DVertex::CSegmentPoint::X() const { return m_coor[0]; }

const double &C2DVertex::CSegmentPoint::Y() const { return m_coor[1]; }

const double &C2DVertex::CSegmentPoint::Z() const { return m_coor[2]; }

void C2DVertex::CSegmentPoint::X(const double &dX) { m_coor[0] = dX; }

void C2DVertex::CSegmentPoint::Y(const double &dY) { m_coor[1] = dY; }

void C2DVertex::CSegmentPoint::Z(const double &dZ) { m_coor[2] = dZ; }

int C2DVertex::CSegmentPoint::SegmentSize() const { return m_vertex.SegmentSize(); }

const geo::ISegment &C2DVertex::CSegmentPoint::Segment(int nIndex) const { return m_vertex.Segment(nIndex).Segment(); }

void C2DVertex::CSegmentPoint::AssertValid() const {}

bool C2DVertex::CSegmentPoint::Empty() const { return false; }

C2DVertex &C2DVertex::CSegmentPoint::Vertex() { return m_vertex; }

const C2DVertex &C2DVertex::CSegmentPoint::Vertex() const { return m_vertex; }
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

C2DVertex::C2DVertex(const geo::IPoint &point, IModelObject &parent) : IModelObject(""), m_point(*this) {
  m_point.X(point.X());
  m_point.Y(point.Y());
  reParent(parent);
  color(qRgb(255, 0, 0));
}

C2DVertex::C2DVertex(const double &dX, const double &dY, IModelObject &parent) : IModelObject(""), m_point(*this) {
  m_point.X(dX);
  m_point.Y(dY);
  reParent(parent);
  color(qRgb(255, 0, 0));
}

C2DVertex::C2DVertex(IModelObject &parent) : IModelObject(parent), m_point(*this) {}
/*
C2DVertex::C2DVertex(const C2DVertex& rhs)
: COpenGLNode(rhs), m_point(rhs.m_point), m_vcContainer(rhs.m_vcContainer)
{
}
*/
C2DVertex::~C2DVertex() {}
/*
bool C2DVertex::operator==(const C2DVertex& rhs) const
{
  return m_point == rhs.m_point;
}

C2DVertex& C2DVertex::operator=(const C2DVertex& rhs)
{
  COpenGLNode::operator =(rhs);
  m_point = rhs.m_point;
  return *this;
}
*/
C2DVertex::TColor C2DVertex::Color() const { return qRgb(255, 0, 0); }
/*
BOOL C2DVertex::Empty() const
{
  return m_point.Empty();
}

long C2DVertex::SavedItems() const
{
  return COpenGLNode::SavedItems() + 1;
}

void C2DVertex::LoadStream(TSTREAM& stream, CVersion& version, TPROGRESS& progress)
{
  COpenGLNode::LoadStream(stream, version, progress);

  double dT, dZ;
  stream >> dT;
  stream >> dZ;

  T2D(m_point, dT);
  Z2D(m_point, dZ);

  progress.Step();
}

void C2DVertex::SaveStream(TSTREAM& stream, TPROGRESS& progress)
{
  COpenGLNode::SaveStream(stream, progress);

  stream << T2D(m_point);
  stream << Z2D(m_point);

  progress.Step();
}

UINT C2DVertex::IconId() const
{
  return 0;
}

UINT C2DVertex::TypeId() const
{
  return 0;
}
*/
int C2DVertex::geometrySize() const { return 1; }

const geo::IObject &C2DVertex::geometry(int nIndex) const { return m_point; }

void C2DVertex::referenceInserted(IModelObject &ref) {
  const C2DSegment *pSegment = dynamic_cast<const C2DSegment *>(&ref);

  if (pSegment) {
#ifdef _DEBUG
    int iDbg;
    for (iDbg = 0; iDbg < m_vcContainer.size(); iDbg++)
      assert(m_vcContainer[iDbg] != pSegment);
#endif

    m_vcContainer.push_back(const_cast<C2DSegment *>(pSegment));
  }

  IModelObject::referenceInserted(ref);
}

void C2DVertex::referenceRemoved(IModelObject &ref) {
  int i;
  for (i = 0; i < m_vcContainer.size(); i++) {
    if (m_vcContainer[i] == &ref) {
      m_vcContainer.erase(m_vcContainer.begin() + i);
      return;
    }
  }

  IModelObject::referenceRemoved(ref);
}

const geo::ISegmentPoint &C2DVertex::Point() const { return m_point; }

geo::ISegmentPoint &C2DVertex::Point() { return m_point; }

int C2DVertex::SegmentSize() const { return m_vcContainer.size(); }

const C2DSegment &C2DVertex::Segment(int nIndex) const {
  assert(nIndex >= 0 && nIndex < SegmentSize());
  return *m_vcContainer[nIndex];
}

C2DSegment &C2DVertex::Segment(int nIndex) {
  assert(nIndex >= 0 && nIndex < SegmentSize());
  return *m_vcContainer[nIndex];
}

bool C2DVertex::CanDestroy() const { return SegmentSize() < 3; }

bool C2DVertex::Destroy() {
  ASSERT(CanDestroy());
  C2DDocument &doc = dynamic_cast<C2DDocument &>(document());
  // When the vertex has one segment just delete
  if (SegmentSize() == 1) {
    // Delete segment and then ourselves
    delete &Segment(0);
    delete this;
    //		model.RecalculateCompartments();
    //		model.InvalidateMesh();
    return true;
  }

  if (SegmentSize() == 2) {
    C2DSegment &first = Segment(0);
    C2DSegment &second = Segment(1);
    if (&first.FirstVertex() == this)
      first.FirstVertex(second.OtherVertex(*this));
    else
      first.SecondVertex(second.OtherVertex(*this));
    delete &second;
    delete this;
    //		model.RecalculateCompartments();
    //		model.InvalidateMesh();
    return true;
  }

  return false;
}

bool C2DVertex::CanMove(const geo::IPoint &point) const {
  // Find adjacent vertices
  std::vector<geo::CLine> vcLine;
  std::vector<geo::CPoint> vcConnect;
  for (int i = 0; i < SegmentSize(); i++) {
    if (&Segment(i).FirstVertex() == this) {
      vcLine.push_back(geo::CLine(point, Segment(i).SecondVertex().Point()));
      vcConnect.push_back(Segment(i).SecondVertex().Point());
    } else {
      vcLine.push_back(geo::CLine(point, Segment(i).FirstVertex().Point()));
      vcConnect.push_back(Segment(i).FirstVertex().Point());
    }
  }

  for (i = 0; i < vcLine.size(); i++) {
    C2DDocument &doc = (C2DDocument &)document();
    std::vector<C2DSegmentContainer::TIntersection> vcIntersection = doc.Segments().Intersection(vcLine[i]);
    for (int j = 0; j < vcIntersection.size(); j++) {
      bool bIntersecting = true;
      for (int k = 0; k < SegmentSize(); k++) {
        if (vcIntersection[j].first == &Segment(k))
          bIntersecting = false;
      }
      for (k = 0; k < vcConnect.size(); k++) {
        if (vcIntersection[j].second == vcConnect[k])
          bIntersecting = false;
      }

      if (bIntersecting)
        return false;
    }
  }

  return true;
}

bool C2DVertex::CanMove(const double &dX, const double &dY) const {
  geo::CPoint point(dX, dY);

  return CanMove(point);
}

bool C2DVertex::Move(const geo::IPoint &point) {
  if (CanMove(point)) {
    Point().X(point.X());
    Point().Z(point.Z());
    Point().Y(point.Y());
    return true;
  }

  return false;
}

bool C2DVertex::Move(const double &dX, const double &dY) {
  geo::CPoint point(dX, dY);
  return Move(point);
}
/*
bool C2DVertex::MouseRelease(TFrame& frame, TButtonState state, TButtonState button, const TScreenPoint& point, const
TObjectVec& vcHit)
{
  if(button == Qt::RightButton)
  {
    if(vcHit.size() > 0)
    {
      CContextMenuInvoker menuInvoker;
      AppendContextMenu(menuInvoker);
      TScreenPoint global = frame.GlobalScreenPoint(point);
      menuInvoker.TrackMenu(global.x(), global.y(), *AfxGetMainWnd());
    }
  }

  return true;
}

void C2DVertex::AppendContextMenu(CContextMenuInvoker &invoker)
{
  // Append copy, paste and delete
  invoker.AddCommand( _T("&Copy"),*(new TGraphNodeCommand(*this, &CGraphNode::OnCopy)));
  invoker.AddCommand( _T("&Paste"),*(new TGraphNodeCommand(*this, &CGraphNode::OnPaste, &CGraphNode::OnCanPaste)));
  invoker.AddCommand( _T("&Delete"),*(new TGraphNodeCommand(*this, &CGraphNode::Destroy, &CGraphNode::CanDestroy)));
  invoker.AddSeparator();

  // Append attributes ...
  invoker.AddCommand( _T("&Attributes"),*(new TGraphNodeCommand(*this, &CGraphNode::Properties)));
}

bool C2DVertex::Properties()
{
  CAttri2DVertex dlg(*this);
  return dlg.DoModal() == IDOK;
}
*/

C2DVertexContainer::C2DVertexContainer(const char *name, C2DDocument &document)
    : CModelContainer<C2DVertex>(name, document), m_vertices(10) {}

void C2DVertexContainer::childInserted(IModelObject &child) {
  C2DVertex &vertex = (C2DVertex &)child;
  VERIFY(m_vertices.insert(TVertexMap::value_type(&vertex.Point(), &vertex)).second);
}

void C2DVertexContainer::childRemoved(IModelObject &child) {
  C2DVertex &vertex = (C2DVertex &)child;
  m_vertices.erase(&vertex.Point());
}

C2DVertex *C2DVertexContainer::NearTo(const geo::IPoint &point) const {
  TVertexMap::const_iterator it = m_vertices.find(&point);
  if (it == m_vertices.end())
    return 0;
  return it->second;
}