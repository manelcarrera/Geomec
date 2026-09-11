// 2DCompartment.cpp: implementation of the C2DBody class.
//
//////////////////////////////////////////////////////////////////////

#include "2DVertex.h"
#include "2DSegment.h"
#include "2Dbody.h"
#include "region.h"
#include <cmath>
#include <algorithm>

#ifdef _DEBUG
#ifdef _MSC_VER#undef THIS_FILE
static char THIS_FILE[]=__FILE__;#endif  // _MSC_VER
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Implementation of the polygon class
//////////////////////////////////////////////////////////////////////

C2DBody::CPolygon::CPolygon()
: IPolygon(false)
{
}
    
int C2DBody::CPolygon::NrOfPoints() const
{
  return m_vcVertex.size();
}

const geo::IPoint &C2DBody::CPolygon::Point(const int nIndex) const
{
  return m_vcVertex[nIndex]->Point();
}

const C2DVertex& C2DBody::CPolygon::Vertex(int nIndex) const
{
  return *m_vcVertex[nIndex];
}

void C2DBody::CPolygon::Point(const int nIndex, const geo::IPoint &pt)
{
  assert(false);
}

void C2DBody::CPolygon::Clear()
{
  InvalidateCache();
  m_vcVertex.clear();
}

void C2DBody::CPolygon::PushBack(const C2DVertex& point)
{
  m_vcVertex.push_back(&point);
}

int C2DBody::CPolygon::SegmentSize() const
{
  return NrOfPoints();
}

const C2DSegment& C2DBody::CPolygon::Segment(int nIndex) const
{
  for(int i = 0; i < Vertex(nIndex).SegmentSize(); i++)
  {
    for(int j = 0; j < Vertex((nIndex + 1) % NrOfPoints()).SegmentSize(); j++)
    {
      if(&Vertex(nIndex).Segment(i) == &Vertex((nIndex + 1) % NrOfPoints()).Segment(j))
        return Vertex(nIndex).Segment(i);
    }
  }

  assert(false);
  const C2DSegment *pSegment = 0;
  return *pSegment;
}

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
C2DBody::C2DBody(IModelObject& parent)
: IModelObject(parent), m_pElementSet(0), m_bElementSize(false), m_dMaxElementSize(-1)
{
}

C2DBody::C2DBody(IModelObject& parent, const TIndexVec& vcVertex, const TVertexMap& mpVertex)
: IModelObject(parent), m_pElementSet(0), m_bElementSize(false), m_dMaxElementSize(-1)
{
  for(int i = 0; i < vcVertex.size(); i++)
  {
    ASSERT(mpVertex.find(vcVertex[i]) != mpVertex.end());
    m_polygon.PushBack(*mpVertex.find(vcVertex[i])->second);
  }

}

void C2DBody::SetElementSet(geo::IElementSet &elementset)
{
  assert(!m_pElementSet);
  m_pElementSet = &elementset;
}

void C2DBody::ClearElementSet()
{
//	assert(m_pElementSet);
  m_pElementSet = 0;
}

const geo::IElementSet &C2DBody::ElementSet() const
{
  // this probably won't work, because the elementset should exist, but give zero on Size()
  assert(m_pElementSet != 0);
  return *m_pElementSet;
}

geo::IElementSet &C2DBody::ElementSet()
{
  // this probably won't work, because the elementset should exist, but give zero on Size()
  assert(m_pElementSet != 0);
  return *m_pElementSet;
}

const C2DBody::CPolygon& C2DBody::Polygon() const
{
  return m_polygon;
}
/*
bool C2DBody::MouseRelease(TFrame& frame, TButtonState state, TButtonState button, const TScreenPoint& point, const TObjectVec& vcHit)
{
  if((vcHit.size() > 0) && (button == Qt::RightButton))
  {
    CContextMenuInvoker menuInvoker;
    AppendContextMenu(menuInvoker);
    TScreenPoint global =frame.GlobalScreenPoint(point);
    menuInvoker.TrackMenu(global.x(), global.y(), *AfxGetMainWnd());
  }

  return TRUE;
}

UINT C2DBody::IconId() const
{
  return IDI_COMPARTMENT;
}
*/
int C2DBody::geometrySize() const
{
  return 1;
}

const geo::IObject& C2DBody::geometry(int nIndex) const
{
  if(m_pElementSet)
    return *m_pElementSet;
  return m_polygon;
}
/*
void C2DBody::AppendContextMenu(CContextMenuInvoker &invoker)
{
  typedef CSingleCommandTemplate<C2DBody> TCCommand;
  typedef CSingleCommandTemplate<C2DFormation> TFCommand;
  C2DFormation& formation = dynamic_cast<C2DFormation&>(Formation());
  // Coursen and refine
  if(Formation().ElementSetSize() == 1)
  {
    invoker.AddCommand( _T("&Refine"),*(new TCCommand(*this, &C2DBody::OnRefine, &C2DBody::CanRefine)));
    invoker.AddCommand( _T("&Coarsen"),*(new TCCommand(*this, &C2DBody::OnCoarsen, &C2DBody::CanCoarsen)));
  }
  else
  {
    invoker.AddCommand( _T("&Refine formation"),*(new TFCommand(formation, &C2DFormation::OnRefine, &C2DFormation::CanRefine)));
    invoker.AddCommand( _T("&Coarsen formation"),*(new TFCommand(formation, &C2DFormation::OnCoarsen, &C2DFormation::CanCoarsen)));
    invoker.AddSeparator();
    invoker.AddCommand( _T("&Refine compartment"),*(new TCCommand(*this, &C2DBody::OnRefine, &C2DBody::CanRefine)));
    invoker.AddCommand( _T("&Coarsen compartment"),*(new TCCommand(*this, &C2DBody::OnCoarsen, &C2DBody::CanCoarsen)));
  }
  invoker.AddSeparator();

  // Append copy, paste and delete
  invoker.AddCommand( _T("&Copy"),*(new TGraphNodeCommand(*this, &CGraphNode::OnCopy)));
  invoker.AddCommand( _T("&Paste"),*(new TGraphNodeCommand(*this, &CGraphNode::OnPaste, &CGraphNode::OnCanPaste)));
  invoker.AddSeparator();

  // Append attributes ...
  invoker.AddCommand( _T("&Attributes"),*(new TGraphNodeCommand(*this, &CGraphNode::Properties)));	
}

bool C2DBody::Properties()
{
  CAttri2DCompartment dlg(*this);
  return dlg.DoModal() == IDOK;
}
*/
void C2DBody::SetPolygon(const std::vector<const C2DVertex*>& vcVertex)
{
  m_polygon.Clear();
  for(int i = 0; i < vcVertex.size(); i++)
    m_polygon.PushBack(*vcVertex[i]);
}

void C2DBody::OnCoarsen()
{
  COperation operation(*this);
  ASSERT(CanCoarsen());
  ElementSize(ElementSize() * 1.5);
  geometryChanged();
}

void C2DBody::OnRefine()
{
  if(!ElementSizeEnabled())
    ElementSize(MaxElementSize());
  ElementSize(ElementSize() / 1.5);
  geometryChanged();
}

bool C2DBody::CanCoarsen() const
{
  return ElementSizeEnabled() && (ElementSize() * 1.5 < m_dMaxElementSize);
}

bool C2DBody::CanRefine() const
{
  return true;
}

bool C2DBody::ElementSizeEnabled() const
{
  return m_bElementSize;
}
  
void C2DBody::ElementSize(const double& dSize)
{
  m_dElementSize = dSize;
  m_bElementSize = true;
}

void C2DBody::DisableElementSize()
{
  m_bElementSize = false;
}

const double& C2DBody::ElementSize() const
{
  return m_dElementSize;
}

const double& C2DBody::MaxElementSize() const
{
  return m_dMaxElementSize;
}

void C2DBody::CalculateElementSize()
{
  ASSERT(m_pElementSet);
  for(int i = 0; i < m_pElementSet->ElementSize(); i++)
  {
    const geo::ITriangle* pTriangle = dynamic_cast<const geo::ITriangle*>(&m_pElementSet->Element(i));
    if(pTriangle)
      m_dMaxElementSize = std::_cpp_max(m_dMaxElementSize, pTriangle->Area());
  }
}

int C2DBody::Resemblance(const std::set<IModelObject*>& identifier) const
{
  std::vector<IModelObject*> vcRet;
  std::set_difference(m_identifier.begin(), m_identifier.end(), 
            identifier.begin(), identifier.end(),
            std::back_inserter(vcRet));
  return vcRet.size();
}
/*
long C2DBody::SavedItems() const
{
  return IFormationElementSet::SavedItems() + Polygon().NrOfPoints() + 3;
}

void C2DBody::LoadStream(TSTREAM& stream, CVersion& version, TPROGRESS& progress)
{
  IFormationElementSet::LoadStream(stream, version, progress);

  int iSize;
  stream >> iSize;

  int i;
  for(i = 0; i < iSize; i++)
  {
    int idx;
    stream >> idx;
    m_polygon.PushBack(*((C2DVertex*)GetNodeFromEntry(MD_2D_VERTEX, idx)));

    progress.Step();
  }

  stream >> m_dElementSize;
  progress.Step();

  stream >> i;
  m_bElementSize = (i != 0);
  progress.Step();

  stream >> m_dMaxElementSize;
  progress.Step();
}

void C2DBody::SaveStream(TSTREAM& stream, TPROGRESS& progress)
{
  IFormationElementSet::SaveStream(stream, progress);

  stream << Polygon().NrOfPoints();

  int i;
  for(i = 0; i < Polygon().NrOfPoints(); i++)
  {
    stream << Polygon().Vertex(i).Index();
    progress.Step();
  }

  stream << m_dElementSize;
  progress.Step();

  stream << (int)(m_bElementSize);
  progress.Step();

  stream << m_dMaxElementSize;
  progress.Step();
}
*/