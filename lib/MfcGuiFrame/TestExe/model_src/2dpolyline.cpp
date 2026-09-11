// 2DVertexContainer.cpp: implementation of the C2DVertexContainer class.
//
//////////////////////////////////////////////////////////////////////

#include "2DPolyLine.h"
#include "2DVertex.h"
#include "2DDocument.h"
#include "2DSegment.h"

//////////////////////////////////////////////////////////////////////
// The polyline implementation
//////////////////////////////////////////////////////////////////////

C2DPolyLine::CPolyLine::CPolyLine(C2DPolyLine& polyline)
: m_polyline(polyline)
{
}

C2DPolyLine::CPolyLine::~CPolyLine()
{
}

geo::IPoint &C2DPolyLine::CPolyLine::PointAt(int nIndex)
{
	return Vertex(nIndex).Point();
}

void C2DPolyLine::CPolyLine::PushBack(C2DSegment& segment)
{
	assert(segment.isReferenced(m_polyline) == -1);
	m_vcSegment.push_back(&segment);
	m_polyline.insertReference(segment);

	InvalidateCache();
}

void C2DPolyLine::CPolyLine::Insert(int nIndex, C2DSegment& segment)
{
	assert(segment.isReferenced(m_polyline));
	assert(nIndex >= 0 && nIndex < m_vcSegment.size());

	// std::vector::insert inserts before given iterator, so add one
	TSegmentVec::iterator it = m_vcSegment.begin() + nIndex;
	m_vcSegment.insert(it, &segment);

	m_polyline.insertReference(segment);

	InvalidateCache();

//	m_polyline.geometryChanged();
}

void C2DPolyLine::CPolyLine::Remove(C2DSegment& segment)
{
	assert(m_polyline.isReferenced(segment));

	int i;
	for(i = 0; i < m_vcSegment.size(); i++)
	{
		if(m_vcSegment[i] == &segment)
		{
			m_vcSegment.erase(m_vcSegment.begin() + i);
			m_polyline.removeReference(segment);
			return;
		}
	}

	assert(false); // not found
}

std::pair<int, bool> C2DPolyLine::CPolyLine::SegmentIndex(const C2DSegment& segment) const
{
	for(int i = 0; i < LineSize(); i++)
	{
		const C2DVertex& first = Vertex(i);
		const C2DVertex& second = Vertex(i + 1);
		if((&segment.FirstVertex() == &first) && (&segment.SecondVertex() == &second))
			return std::pair<int, bool>(i, true);
		if((&segment.FirstVertex() == &second) && (&segment.SecondVertex() == &first))
			return std::pair<int, bool>(i, false);
	}

	return std::pair<int, bool>(-1, false);
}

const geo::IPoint &C2DPolyLine::CPolyLine::Point(int nIndex) const
{
	return Vertex(nIndex).Point();
}

const C2DVertex& C2DPolyLine::CPolyLine::Vertex(int nIndex) const
{
	ASSERT(LineSize() > 0);
	if((LineSize() == 1) && (nIndex == 0))
		return Segment(0).FirstVertex();

	if((LineSize() == 1) && (nIndex == 1))
		return Segment(0).SecondVertex();

	if(nIndex == 0)
	{
		if((&Segment(0).FirstVertex() == &Segment(1).FirstVertex()) || (&Segment(0).FirstVertex() == &Segment(1).SecondVertex()))
			return Segment(0).SecondVertex();
		return Segment(0).FirstVertex();
	}

	if(nIndex == LineSize())
	{
		if((&Segment(LineSize() - 1).FirstVertex() == &Segment(LineSize() - 2).FirstVertex()) || 
			(&Segment(LineSize() - 1).FirstVertex() == &Segment(LineSize() - 2).SecondVertex()))
			return Segment(LineSize() - 1).SecondVertex();
		return Segment(LineSize() - 1).FirstVertex();
	}

	if((&Segment(nIndex).FirstVertex() == &Segment(nIndex - 1).FirstVertex()) || 
		(&Segment(nIndex).FirstVertex() == &Segment(nIndex - 1).SecondVertex()))
			return Segment(nIndex).FirstVertex();
	return Segment(nIndex).SecondVertex();
}

C2DVertex& C2DPolyLine::CPolyLine::Vertex(int nIndex)
{
	assert(nIndex >= 0 && nIndex < PointSize());
	if(nIndex == LineSize()) return Segment(nIndex - 1).SecondVertex();

	return Segment(nIndex).FirstVertex();
}

int C2DPolyLine::CPolyLine::PointSize() const
{
	if(m_vcSegment.empty()) return 0;
	return m_vcSegment.size() + 1;
}

const geo::ILine &C2DPolyLine::CPolyLine::Line(int nIndex) const
{
	assert(nIndex >= 0 && nIndex < LineSize());
	return Segment(nIndex).Segment();
}

int C2DPolyLine::CPolyLine::LineSize() const
{
	return m_vcSegment.size();
}

void C2DPolyLine::CPolyLine::Swap(int nIndex1, int nIndex2)
{
	assert(false);
}

void C2DPolyLine::CPolyLine::AssertValid() const
{
	for(int i = 0; i < LineSize(); i++)
	{
		ASSERT(m_polyline.isReferenced(Segment(i)));
	}
}
	
std::vector<int> C2DPolyLine::CPolyLine::Nodes(const geo::IElement &element) const
{
	assert(false);
	return std::vector<int>();
}

const C2DSegment& C2DPolyLine::CPolyLine::Segment(int nIndex) const
{
	assert(nIndex >= 0 && nIndex < LineSize());
	return *m_vcSegment[nIndex];
}

C2DSegment& C2DPolyLine::CPolyLine::Segment(int nIndex)
{
	assert(nIndex >= 0 && nIndex < LineSize());
	return *m_vcSegment[nIndex];
}

//////////////////////////////////////////////////////////////////////
// The point insert
//////////////////////////////////////////////////////////////////////


C2DPolyLine::CPointInsert::CPointInsert(C2DPolyLine& polyline, const geo::IPoint& point)
: m_polyline(polyline), m_point(point)
{
}

bool C2DPolyLine::CPointInsert::CanInsert() const
{
	for(int i = 0; i < m_polyline.PolyLine().LineSize(); i++)
	{
		geo::CPoint ptProject = m_polyline.PolyLine().Line(i).Project(m_point);
		if(m_polyline.PolyLine().Segment(i).CanSplit(ptProject))
			return true;
	}
	
	return false;
}

void C2DPolyLine::CPointInsert::Insert()
{
	ASSERT(CanInsert());
	for(int i = 0; i < m_polyline.PolyLine().LineSize(); i++)
	{
		geo::CPoint ptProject = m_polyline.PolyLine().Line(i).Project(m_point);
		if(m_polyline.PolyLine().Segment(i).CanSplit(ptProject))
		{
			m_polyline.PolyLine().Segment(i).Split(ptProject);
			return;
		}
	}
}

//////////////////////////////////////////////////////////////////////
// The polyline implementation
//////////////////////////////////////////////////////////////////////
C2DPolyLine::C2DPolyLine(IModelObject& parent)
: IModelObject(parent)
{
}

C2DPolyLine::C2DPolyLine(const char* name, IModelObject& parent)
: IModelObject(name, parent)
{
	m_pPolyLine = new CPolyLine(*this);
}

C2DPolyLine::~C2DPolyLine()
{
	delete m_pPolyLine;
}

const C2DPolyLine::CPolyLine& C2DPolyLine::PolyLine() const
{
	return *m_pPolyLine;
}
	
C2DPolyLine::CPolyLine& C2DPolyLine::PolyLine()
{
	return *m_pPolyLine;
}
/*
UINT C2DPolyLine::IconId() const
{
	return 0;
}

UINT C2DPolyLine::TypeId() const
{
	return 0;
}
*/
const geo::IElementSet &C2DPolyLine::ElementSet() const
{
	return *m_pPolyLine;
}

geo::IElementSet &C2DPolyLine::ElementSet()
{
	return *m_pPolyLine;
}

int C2DPolyLine::geometrySize() const
{
	return 1;
}

const geo::IObject& C2DPolyLine::geometry(int nIndex) const
{
	return *m_pPolyLine;
}

/*		
C2DPolyLine::DIMENSION C2DPolyLine::Dimension() const
{
	return DIM_1D;
}

const COpenGLNode& C2DPolyLine::Parent() const
{
	ASSERT(m_pParent);
	return *m_pParent;
}

COpenGLNode& C2DPolyLine::Parent()
{
	ASSERT(m_pParent);
	return *m_pParent;
}

void C2DPolyLine::OnNewNeighbour(const CGraphNode &node)
{
	if((dynamic_cast<const C2DBoundary*>(&node) != 0) ||
		(dynamic_cast<const C2DHorizonBase*>(&node) != 0))
	{
		m_pParent = (COpenGLNode*)(&node);
	}
	IElementSet::OnNewNeighbour(node);
}

void C2DPolyLine::OnNeighbourModified(const CGraphNode& node, UINT uHint)
{
	for(int i = 0; i < m_pPolyLine->LineSize(); i++)
	{
		if(&m_pPolyLine->Segment(i) == &node)
			Modified();
	}

	IElementSet::OnNeighbourModified(node, uHint);
}

void C2DPolyLine::OnNeighbourDeleted(const CGraphNode &node)
{
	if(m_pParent == &node)
		m_pParent = 0;
	for(int i = 0; i < m_pPolyLine->LineSize(); i++)
	{
		if(&m_pPolyLine->Segment(i) == &node)
		{
			m_pPolyLine->m_vcSegment.erase(m_pPolyLine->m_vcSegment.begin() + i);
			IElementSet::OnNeighbourDeleted(node);
			Modified();
			return;
		}
	}


	IElementSet::OnNeighbourDeleted(node);
}
*/
/*
int C2DPolyLine::NodeSize() const
{
	const CModelBase& model = dynamic_cast<const CModelBase&>(Model());
	if(model.IsMesh())
	{
		if(m_vcEdge.size() == 0) CreateEdgeList();
		return m_vcEdge.size() + 1;
	}

	return 0;
}

int C2DPolyLine::NodeIndex(int nIndex) const
{
	ASSERT(EdgeSize() > 0);
	if((EdgeSize() == 1) && (nIndex == 0))
		return Edge(0).FirstIndex();

	if((EdgeSize() == 1) && (nIndex == 1))
		return Edge(0).SecondIndex();

	if(nIndex == 0)
	{
		if((Edge(0).FirstIndex() == Edge(1).FirstIndex()) || (Edge(0).FirstIndex() == Edge(1).SecondIndex()))
			return Edge(0).SecondIndex();
		return Edge(0).FirstIndex();
	}

	if(nIndex == EdgeSize())
	{
		if((Edge(EdgeSize() - 1).FirstIndex() == Edge(EdgeSize() - 2).FirstIndex()) || 
			(Edge(EdgeSize() - 1).FirstIndex() == Edge(EdgeSize() - 2).SecondIndex()))
			return Edge(EdgeSize() - 1).SecondIndex();
		return Edge(EdgeSize() - 1).FirstIndex();
	}

	if((Edge(nIndex).FirstIndex() == Edge(nIndex - 1).FirstIndex()) || 
		(Edge(nIndex).FirstIndex() == Edge(nIndex - 1).SecondIndex()))
			return Edge(nIndex).FirstIndex();
	return Edge(nIndex).SecondIndex();	
}

const geo::IPoint& C2DPolyLine::Node(int nIndex) const
{
	const CModelBase& model = dynamic_cast<const CModelBase&>(Model());
	return model.Mesh().Mesh().Point(NodeIndex(nIndex));
}
	
int C2DPolyLine::EdgeSize() const
{
	const CModelBase& model = dynamic_cast<const CModelBase&>(Model());
	if(model.IsMesh())
	{
		if(m_vcEdge.size() == 0) CreateEdgeList();
		return m_vcEdge.size();
	}

	return 0;
}

const C2DMesher::CEdgeSurrogate& C2DPolyLine::Edge(int nIndex) const
{
	if(m_vcEdge.size() == 0) CreateEdgeList();
	assert(nIndex >= 0 && nIndex < EdgeSize());
	return *m_vcEdge[nIndex];
}

void C2DPolyLine::ClearEdges()
{
	m_vcEdge.clear();
}

void C2DPolyLine::CreateEdgeList() const
{
	ASSERT(PolyLine().LineSize() > 0);
	const CModelBase& model = dynamic_cast<const CModelBase&>(Model());
	assert(model.IsMesh());
	// Collect all edges in a multimap
	typedef std::multimap<int, const C2DMesher::CEdgeSurrogate*> TEdgeMap;
	TEdgeMap mpEdge;
	for(int i = 0; i < PolyLine().LineSize(); i++)
	{
		for(int j = 0; j < PolyLine().Segment(i).EdgeSize(); j++)
		{
			const C2DMesher::CEdgeSurrogate& edge = PolyLine().Segment(i).Edge(j);
			mpEdge.insert(TEdgeMap::value_type(edge.FirstIndex(), &edge));
			mpEdge.insert(TEdgeMap::value_type(edge.SecondIndex(), &edge));
		}
	}

	// Find the begin and end point
	const geo::IPoint& first_point = PolyLine().Point(0);
	const geo::IPoint& last_point = PolyLine().Point(PolyLine().PointSize() - 1);
	int nCurrentPoint = -1;
	int nLastPoint = -1;
	for(i = 0; i < PolyLine().Segment(0).EdgeSize(); i++)
	{
		const C2DMesher::CEdgeSurrogate& edge = PolyLine().Segment(0).Edge(i);
		if(first_point == edge.First())
			nCurrentPoint = edge.FirstIndex();
		if(first_point == edge.Second())
			nCurrentPoint = edge.SecondIndex();
	}

	for(i = 0; i < PolyLine().Segment(PolyLine().LineSize() - 1).EdgeSize(); i++)
	{
		const C2DMesher::CEdgeSurrogate& edge = PolyLine().Segment(PolyLine().LineSize() - 1).Edge(i);
		if(last_point == edge.First())
			nLastPoint = edge.FirstIndex();
		if(last_point == edge.Second())
			nLastPoint = edge.SecondIndex();
	}
	ASSERT(nCurrentPoint > -1);
	ASSERT(nLastPoint > -1);

	// Walk over chain
	const C2DMesher::CEdgeSurrogate* pCurrentEdge = 0;
	while(nCurrentPoint != nLastPoint)
	{
		// Find a new edge
		TEdgeMap::iterator it = mpEdge.find(nCurrentPoint);
		ASSERT(it != mpEdge.end());
		while(it->second == pCurrentEdge) it++;
		ASSERT(it->first == nCurrentPoint);
		pCurrentEdge = it->second;
		m_vcEdge.push_back(pCurrentEdge);
		if(pCurrentEdge->FirstIndex() == nCurrentPoint)
			nCurrentPoint = pCurrentEdge->SecondIndex();
		else
			nCurrentPoint = pCurrentEdge->FirstIndex();
	};
}

BOOL C2DPolyLine::Empty() const
{
	return (m_vcEdge.size() == 0) || (m_pPolyLine == 0);
}

long C2DPolyLine::SavedItems() const
{
	return IElementSet::SavedItems() + PolyLine().LineSize();
}

void C2DPolyLine::LoadStream(TSTREAM& stream, CVersion& version, TPROGRESS& progress)
{
	m_pPolyLine = new CPolyLine(*this);

	IElementSet::LoadStream(stream, version, progress);

	int iSize;
	stream >> iSize;
	int i;
	for(i = 0; i < iSize; i++)
	{
		int idx;
		stream >> idx;
		m_pPolyLine->PushBack(*((C2DSegment*)GetNodeFromEntry(MD_2D_SEGMENT, idx)));
		progress.Step();
	}

	ASSERT(Model().GraphEntry(MD_2D_POLYLINE));
	LinkTo(*Model().GraphEntry(MD_2D_POLYLINE));
}

void C2DPolyLine::SaveStream(TSTREAM& stream, TPROGRESS& progress)
{
	IElementSet::SaveStream(stream, progress);

	stream << PolyLine().LineSize();
	int i;
	for(i = 0; i < PolyLine().LineSize(); i++)
	{
		stream << PolyLine().Segment(i).Index();
		progress.Step();
	}
}
*/
//////////////////////////////////////////////////////////////////////
// The CLine implementation
//////////////////////////////////////////////////////////////////////

