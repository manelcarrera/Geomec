// 3DHorizon.cpp: implementation of the C3DHorizon class.
//
//////////////////////////////////////////////////////////////////////

#include "3DHorizon.h"
#include "BoundaryBase.h"
#include "ModelBase.h"
#include "TSSurfaceProgress.h"
#include "BaseEntryTypes.h"
#include "FvGocadFile.h"
#include "MeshBase.h"
#include "InterfaceElement.h"
#include "ifaceelmmat.h"
#include "BranchState.h"
#include "3dmodel.h"
#include "FaultParametersNode.h"
#include "resourceIDI.h"
#include "Global.h"
#include "IProgressFactory.h"
#include "MeasuredTopDisplacementsNode.h"
#include "ElementGroup.h"
#include "TSSurface.h"
#include "StreamVersion.h"
#include "FormationBase.h"
#include "TiedBodyTriangle.h"

C3DHorizon::CElementSet::CTriangle::CTriangle(const CElementSet& elementset, const geo::IPoint& p1, const geo::IPoint& p2, const geo::IPoint& p3, int nIndex)
: m_elementset(elementset),
  m_vcPoint(3),
  m_nIndex(nIndex)
{
	m_vcPoint[0] = &p1;
	m_vcPoint[1] = &p2;
	m_vcPoint[2] = &p3;
}

const geo::IElementSet* C3DHorizon::CElementSet::CTriangle::IndexingElementSet() const
{
	return &m_elementset;
}

const geo::IPoint &C3DHorizon::CElementSet::CTriangle::Point(int nIndex) const
{
	assert(nIndex >= 0 && nIndex < m_vcPoint.size());
	return *m_vcPoint[nIndex];
}

void C3DHorizon::CElementSet::CTriangle::Point(int /*nIndex*/, const geo::IPoint &/*pt*/)
{
	assert(false);
}

int C3DHorizon::CElementSet::CTriangle::NrOfPoints() const
{
	return m_vcPoint.size();
}

size_t C3DHorizon::CElementSet::CTriangle::Order() const
{
	return 1;
}

int C3DHorizon::CElementSet::CTriangle::PointIndex(int nIndex) const
{
	std::vector<int> vcNodeIndex = m_elementset.Nodes(*this);

	assert(&m_elementset.Point(vcNodeIndex[0]) == m_vcPoint[0]);
	assert(&m_elementset.Point(vcNodeIndex[1]) == m_vcPoint[1]);
	assert(&m_elementset.Point(vcNodeIndex[2]) == m_vcPoint[2]);

	assert(nIndex >= 0 && nIndex < vcNodeIndex.size());
	assert(vcNodeIndex[nIndex] >= 0 && vcNodeIndex[nIndex] < m_elementset.PointSize());
	return vcNodeIndex[nIndex];
}

int C3DHorizon::CElementSet::CTriangle::Index() const
{
	return m_nIndex;
}


/////

C3DHorizon::CElementSet::CElementSet(const C3DHorizon& horizon)
: m_horizon(horizon)
{
	const CModelBase& model = static_cast<const CModelBase&>(horizon.Model());
	assert(model.Mesh().IsMesh());

	int i;
	for(i = 0; i < horizon.DisplayListSize(); ++i)
	{
		const geo::IObject& dl = horizon.DisplayList(i);
		const geo::IElementSet* pElSet = dynamic_cast<const geo::IElementSet*>(&dl);
		assert(pElSet);
		if(pElSet)
		{
			int j;
			for(j = 0; j < pElSet->ElementSize(); ++j)
				AddElement(pElSet->Element(j));
		}
	}
}

C3DHorizon::CElementSet::~CElementSet()
{
	for(size_t i = 0; i < m_vcOwnElements.size(); ++i)
		delete m_vcOwnElements[i];
}

void C3DHorizon::CElementSet::AssertValid() const
{
}

const geo::IPoint &C3DHorizon::CElementSet::Point(int nIndex) const
{
	assert(nIndex >= 0 && nIndex < m_vcPoint.size());
	return *m_vcPoint[nIndex];
}

int C3DHorizon::CElementSet::PointSize() const
{
	return m_vcPoint.size();
}

const geo::IElement &C3DHorizon::CElementSet::Element(int nIndex) const
{
	assert(nIndex >= 0 && nIndex < m_vcElement.size());
	return *m_vcElement[nIndex];
}

int C3DHorizon::CElementSet::ElementSize() const
{
	return m_vcElement.size();
}

std::vector<int> C3DHorizon::CElementSet::Nodes(const geo::IElement &element) const
{
	std::vector<int> vcRet(element.NrOfPoints());

	int i;
	for(i = 0; i < element.NrOfPoints(); ++i)
	{
		TPointIndexMap::iterator it = m_mpPointIndexMap.find(&element.Point(i));
		assert(it != m_mpPointIndexMap.end());

		vcRet[i] = it->second;
	}

	return vcRet;
}

geo::IPoint &C3DHorizon::CElementSet::PointAt(int nIndex)
{
	assert(false);
	return const_cast<geo::IPoint&>(Point(nIndex));
}

void C3DHorizon::CElementSet::AddElement(const geo::IElement& element)
{
	assert(element.NrOfPoints() == 3 || element.NrOfPoints() == 4);

	if(element.NrOfPoints() == 4)
	{
		// make copies of the points
		geo::CPoint* points[4];

		int i;
		for(i = 0; i < 4; ++i)
		{
			TPointMap::iterator it = m_mpOwnPoints.insert(TPointMap::value_type(&element.Point(i), geo::CPoint(element.Point(i)))).first;
			points[i] = &it->second;
		}

		// it's a quad, register two (own) triangles
		CTriangle* pTri;
		
		pTri = new CTriangle(*this, *points[0], *points[1], *points[2], m_vcElement.size());
		m_vcOwnElements.push_back(pTri);
		RegisterElement(*pTri);

		pTri = new CTriangle(*this, *points[0], *points[2], *points[3], m_vcElement.size());
		m_vcOwnElements.push_back(pTri);
		RegisterElement(*pTri);
	}
	else
	{
		// just register
		RegisterElement(element);
	}
}

void C3DHorizon::CElementSet::RegisterElement(const geo::IElement& element)
{
	m_vcElement.push_back(&element);

	int i;
	for(i = 0; i < element.NrOfPoints(); ++i)
	{
		const geo::IPoint& pt = element.Point(i);

		bool bInserted = m_mpPointIndexMap.insert(TPointIndexMap::value_type(&pt, m_vcPoint.size())).second;
		if(bInserted)
			m_vcPoint.push_back(&pt);
	}
}

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
//Default constructor...
C3DHorizon::C3DHorizon(CFemAppModel &model)
:CHorizonBase(model), 
  m_pConstDepthSurface(0),
  m_pDepth(0), 
  m_nDivisionNorthing(4),
  m_nDivisionEasting(4),
  m_pElementSetCache(0)
{
}

//##ModelId=3BC55D620087
C3DHorizon::C3DHorizon(const QString& strInstanceName, const double& dDepth, CFemAppModel& model, bool bAttachToEntry)
:CHorizonBase(strInstanceName, model, bAttachToEntry), 
  m_pConstDepthSurface(0),
  m_pDepth(0), 
  m_nDivisionNorthing(4),
  m_nDivisionEasting(4),
  m_pElementSetCache(0)
{
	// Create a constant depth
	m_pDepth = new CLengthQuantity(dDepth);
	
	// Define a constant depth ...
	Depth(dDepth);
}

//##ModelId=3BC55D620085
C3DHorizon::C3DHorizon(CSurfaceBase &surface, CFemAppModel& model, bool bSlip, bool bAttachToEntry)
: CHorizonBase(surface.Name().toStdString().c_str(), model, bAttachToEntry), 
  m_pConstDepthSurface(0),
  m_pDepth(0), 
  m_nDivisionNorthing(4),
  m_nDivisionEasting(4),
  m_pElementSetCache(0)
{
	LinkTo(surface);

	if(bSlip)
		Slip(true);
}

C3DHorizon::C3DHorizon(const QString& strInstanceName, CFemAppModel& model, bool bAttachToEntry)
: CHorizonBase(strInstanceName, model, bAttachToEntry), 
  m_pConstDepthSurface(0),
  m_pDepth(0), 
  m_nDivisionNorthing(4),
  m_nDivisionEasting(4),
  m_pElementSetCache(0)
{
}

//##ModelId=3BC55D620095
C3DHorizon::C3DHorizon(const C3DHorizon &rhs)
: CHorizonBase(rhs), 
  m_vcSurfaces(rhs.m_vcSurfaces),
  m_pConstDepthSurface(0),
  m_pDepth(0), 
  m_nDivisionNorthing(rhs.m_nDivisionNorthing),
  m_nDivisionEasting(rhs.m_nDivisionEasting),
  m_pElementSetCache(0)
{
	if(rhs.ConstantDepth())
	{
		m_pDepth = new CLengthQuantity(rhs.Depth().Value());
		InvalidateConstantDepthSurface();
	}

	CSurfaceEntry& entry = (CSurfaceEntry&)*Model().GraphEntry(MD_BASE_SURFACE);
	CSurfaceEntry::TNodeSet stNode = entry.EntryNodes();
	for(CSurfaceEntry::TNodeSet::iterator it = stNode.begin(); it != stNode.end(); it++)
		new CSurfaceBase(**it);
}
	
//##ModelId=3BC55D6200D4
C3DHorizon::~C3DHorizon()
{
	delete m_pDepth;
	delete m_pConstDepthSurface;

	InvalidateElementSetCache();

	if(IsCopy())
	{
		CSurfaceEntry& entry = (CSurfaceEntry&)*Model().GraphEntry(MD_BASE_SURFACE);
		CSurfaceEntry::TNodeSet stNode = entry.EntryNodes();
		for(CSurfaceEntry::TNodeSet::iterator it = stNode.begin(); it != stNode.end(); it++)
		{
			assert((*it)->IsCopied());
			delete &(*it)->Copy();
		}
	}
}

void C3DHorizon::MergeSurface(const CSurfaceBase& surface)
{
	assert(SurfaceSize() == 1);
	assert(&Surface(0) != &surface);

	// Fix the hourglas, because this operation can take some time
	std::auto_ptr <IProgressBase> wait(_g->prog()->create(eProgress::Wait));

	// We first make a list of unique points for the merged surface
	CSurfaceBase *pMerged = Surface(0).MergeSurface(surface, false);
	assert(pMerged);

	// Unlink old surface and link new one
	UnLink(Surface(0));
	LinkTo(*pMerged);
}


//##ModelId=3BC55D6200D6
unsigned int C3DHorizon::IconId() const
{
	if(ConstantDepth())
	{
		if(Slip())
			return IDI_SLIP_CONST_HORIZON;
		else
			return IDI_HORIZON_CONST_DEPTH;
	}

	return CHorizonBase::IconId();
}

void C3DHorizon::OnNeighbourModified(const CGraphNode &item, enum ModifiedHint uHint) 
{
	// Get boundary ...
	const CBoundaryBase* pBoundary =  dynamic_cast<const CBoundaryBase*>(&item);

	if(pBoundary && ConstantDepth() && m_pConstDepthSurface)
	{
		if((m_pConstDepthSurface->Min().X() != pBoundary->Min().X()) ||
		   (m_pConstDepthSurface->Max().X() != pBoundary->Max().X()) ||
		   (m_pConstDepthSurface->Min().Y() != pBoundary->Min().Y()) ||
		   (m_pConstDepthSurface->Max().Y() != pBoundary->Max().Y()) ||
		   (m_pConstDepthSurface->Min().Z() != Depth().Value()))
		{
      COperation oper(Model());
			InvalidateConstantDepthSurface();
      Modified(uHint);
		}
	}

	CModelBase &model = dynamic_cast<CModelBase&>(Model());
	if(&model.Mesh() == &item)
	{
		InvalidateElementSetCache();
    Modified(uHint);
	}

	// Search for surfaces
	for(size_t i = 0; i < m_vcSurfaces.size(); i++)
	{
		if(m_vcSurfaces[i] == &item)
			Modified(uHint);
	}

  UpdateTopHorizonState();

	CHorizonBase::OnNeighbourModified(item, uHint);
}

void C3DHorizon::OnNeighbourDeleted(const CGraphNode &item) 
{
  if(!IsCopy())
  {
	  // Search for the item in the surfaces. We cannot dynamic_cast to the surface so simply find it
	  for(size_t i = 0; i < m_vcSurfaces.size(); i++)
	  {
		  if(m_vcSurfaces[i] == &item)
		  {
			  m_vcSurfaces.erase(m_vcSurfaces.begin() + i);
        CModelBase& model = static_cast<CModelBase&>(Model());
        model.Mesh().InvalidateMesh();
			  Modified();
			  CColorNode::OnNeighbourDeleted(item);
			  return;
		  }
	  }

    if(dynamic_cast<const CMeasuredTopDisplacementsNode*>(&item))
      Modified();
    else
      UpdateTopHorizonState();
  }

	CHorizonBase::OnNeighbourDeleted(item);
}

int C3DHorizon::SurfaceSize() const
{
	return m_vcSurfaces.size();
}

const CSurfaceBase& C3DHorizon::Surface(int nIndex) const
{
	assert(nIndex >= 0);
	assert(nIndex < SurfaceSize());
	if(IsCopy() && m_vcSurfaces[nIndex]->IsCopied())
		return ((const CSurfaceBase&)(m_vcSurfaces[nIndex]->Copy()));

	return *m_vcSurfaces[nIndex];
}

CSurfaceBase& C3DHorizon::Surface(int nIndex)
{
	assert(nIndex >= 0);
	assert(nIndex < SurfaceSize());
	if(IsCopy() && m_vcSurfaces[nIndex]->IsCopied())
		return ((CSurfaceBase&)(m_vcSurfaces[nIndex]->Copy()));

	return *m_vcSurfaces[nIndex];
}

const CLengthQuantity& C3DHorizon::Depth() const
{
	assert(m_pDepth != 0);
	return *m_pDepth;
}

bool C3DHorizon::IsDepthValid(const double& depth, const CLengthQuantity::UNIT unit)
{
	if(IsCopy())
		return ((C3DHorizon&)Source()).IsDepthValid(depth, unit);

	THorizonBaseEntry& entry = (THorizonBaseEntry&)*Model().GraphEntry(MD_BASE_HORIZON);
	THorizonBaseEntry::TNodeSet stNode = entry.EntryNodes();
	for(THorizonBaseEntry::TNodeSet::const_iterator it = stNode.begin(); it != stNode.end(); it++)
	{
		C3DHorizon *pHorizon = dynamic_cast<C3DHorizon*>(*it);
		if(pHorizon)
		{
			if(pHorizon->ConstantDepth() && (pHorizon != this))
			{
				if(fabs(depth - pHorizon->Depth().Value(unit)) < EPS)
					return false;
			}
		}
	}
	return true;
}

bool C3DHorizon::ConstantDepth() const
{
	return m_pDepth != 0;
}
/*
bool C3DHorizon::Destroy()
{
	// Invalidate the mesh ...
	CModelBase* pModel = dynamic_cast<CModelBase*>(&Model());
	assert(pModel);
	pModel->InvalidateMesh();
	
	delete this;

	return true;
}
*/
bool C3DHorizon::operator==(const C3DHorizon& rhs) const
{
	if(!CHorizonBase::operator==(rhs))
		return false;

	if(ConstantDepth() != rhs.ConstantDepth())
		return false;

	if(Slip() != rhs.Slip())
		return false;
	
	if(ConstantDepth())
	{
		if((Depth().Value() != rhs.Depth().Value()) ||
		   (m_nDivisionNorthing != rhs.m_nDivisionNorthing) ||
		   (m_nDivisionEasting != rhs.m_nDivisionEasting))
			return false;
	}

	return true;
}

C3DHorizon& C3DHorizon::operator=(const C3DHorizon& rhs)
{	
	AssertValid();
	rhs.AssertValid();

	CModelBase *pModel = dynamic_cast<CModelBase*> (&Model());
	assert(pModel);

	bool bInvalidateMesh((m_vcSurfaces != rhs.m_vcSurfaces) || 
		                 (Slip() != rhs.Slip()) ||
						 (ConstantDepth() != rhs.ConstantDepth()) || 
						 (m_pDepth && rhs.m_pDepth && 
						 ((m_pDepth->Value() != rhs.m_pDepth->Value()) ||
						 (m_nDivisionNorthing != rhs.m_nDivisionNorthing) ||
						 (m_nDivisionEasting != rhs.m_nDivisionEasting))));

	// Assign the copies of the surfaces
	CSurfaceEntry& entry = (CSurfaceEntry&)*Model().GraphEntry(MD_BASE_SURFACE);
	CSurfaceEntry::TNodeSet stNode = entry.EntryNodes();
	for(CSurfaceEntry::TNodeSet::iterator it = stNode.begin(); it != stNode.end(); it++)
		(**it)=(CSurfaceBase&)((*it)->Copy());

	CHorizonBase::operator =(rhs);
	m_vcSurfaces = rhs.m_vcSurfaces;

	if(rhs.ConstantDepth())
	{
		m_nDivisionNorthing = rhs.m_nDivisionNorthing;
		m_nDivisionEasting = rhs.m_nDivisionEasting;
		Depth(rhs.Depth().Value());
	}
	else
	{
		// If we had a constant depth in the source kill it
		if(ConstantDepth())
		{
			delete m_pDepth;
			m_pDepth = 0;
			InvalidateConstantDepthSurface();
		}
	}

	AssertValid();
	rhs.AssertValid();

	if(bInvalidateMesh)
	{

		pModel->InvalidateMesh();
	}

	return *this;
}

int C3DHorizon::DisplayListSize() const
{
	// Use the source for getting information ...
	if(IsCopy())
	{
		const C3DHorizon* pHorizon = (const C3DHorizon*)(&Source());
		return pHorizon->DisplayListSize();
	}

	// We have a mesh ... when not slipping point to side surfaces
	CModelBase& model = (CModelBase&)Model();
	if(Slip() && model.Mesh().IsMesh())
		return 1;

	if(ConstantDepth())
		return 1;
	
	return SurfaceSize();
}

const geo::IObject& C3DHorizon::DisplayList(int nIndex) const
{
	// We have a mesh ... when not slipping point to side surfaces
	CModelBase& model = (CModelBase&)Model();
	if(Slip() && model.Mesh().IsMesh())
	{
		assert(InterfaceElementGroup());
		return *InterfaceElementGroup();
	}

	if(IsCopy())
	{
		const C3DHorizon* pHorizon = (const C3DHorizon*)(&Source());
		return pHorizon->DisplayList(nIndex);
	}

	if(ConstantDepth())
	{
		assert(nIndex == 0);
		if(!m_pConstDepthSurface)
			OnCreateConstantDepthSurface();
		return *m_pConstDepthSurface;
	}

	return Surface(nIndex).Surface();
}

bool C3DHorizon::Less(const CGraphNode &node) const
{
	const C3DHorizon* pHorizon = dynamic_cast<const C3DHorizon*>(&node);
	if(pHorizon)
	{
		if(!Min().Empty() && !pHorizon->Min().Empty())
    {
      if(Min().Z() < pHorizon->Min().Z() || pHorizon->Min().Z() < Min().Z())
  			return Min().Z() < pHorizon->Min().Z();
    }
	}

	return CColorNode::Less(node);
}

//##ModelId=3BE7AEC10256
void C3DHorizon::Depth(const double& depth, const CQuantity::UNIT unit)
{	
	// Unlink all surfaces ...
	for(int i = 0; i < SurfaceSize(); i++)
	{
		if(IsCopy())
			UnLink(m_vcSurfaces[i]->Copy());
		else
			UnLink(*m_vcSurfaces[i]);
	}

	// Create an constant depht if not defined ...
	if(!m_pDepth)
		m_pDepth = new CLengthQuantity(depth, unit);
	else
		m_pDepth->Value(depth, unit);

	// Create virtual surface for visualisation 
	InvalidateConstantDepthSurface();
}

int C3DHorizon::DivisionNorthing() const
{
	return m_nDivisionNorthing;
}

int C3DHorizon::DivisionEasting() const
{
	return m_nDivisionEasting;
}

void C3DHorizon::Division(int nNorthing, int nEasting)
{
	m_nDivisionNorthing = nNorthing;
	m_nDivisionEasting = nEasting;
}

//##ModelId=3BE7E0C60279
void C3DHorizon::InvalidateConstantDepthSurface()
{
	delete m_pConstDepthSurface;
	m_pConstDepthSurface = 0;
}

void C3DHorizon::OnCreateConstantDepthSurface() const
{
	assert(ConstantDepth());		// Must have an constant depth .....
	assert(!m_pConstDepthSurface);

	// Boundary is the surface equal to the boundary? Do we need to update?
	CBoundaryBase* pBoundary = &((CModelBase&) Model()).Boundary();
	geo::CPoint ptMin = pBoundary->Min();
	geo::CPoint ptMax = pBoundary->Max();

	// Create visualisation points
	geo::CPtrArray<geo::IPoint> vcPoint;
	for(int x = 0; x < (m_nDivisionNorthing + 1); x++)
	{
		for(int y = 0; y < (m_nDivisionEasting + 1); y++)
		{
			vcPoint.PushBack(*(new geo::CPoint(ptMin.X() + ((ptMax.X() - ptMin.X()) * x)/ m_nDivisionNorthing,
											   ptMin.Y() + ((ptMax.Y() - ptMin.Y()) * y)/ m_nDivisionEasting,
											   Depth().Value())));
		}
	}

	CTSSurfaceProgress SurfaceProgress;
	CTSSurface temp(vcPoint, SurfaceProgress);
	m_pConstDepthSurface = new geo::CTriSurface(temp);

	// Destroy temporary nodes
	for(size_t i = 0; i < vcPoint.Size(); i++)
	{
		delete &vcPoint.Object(i);
	}
}

void C3DHorizon::InvalidateElementSetCache()
{
	if(m_pElementSetCache)
	{
		delete m_pElementSetCache;
		m_pElementSetCache = 0;
	}
}

void C3DHorizon::CreateElementSetCache() const
{
	assert(!m_pElementSetCache);
	m_pElementSetCache = new CElementSet(*this);
}

void C3DHorizon::UpdateTopHorizonState()
{
  if(IsCopy())
    return;

  assert(dynamic_cast<C3DModel*>(&Model()));
  C3DModel& model = static_cast<C3DModel&>(Model());
  CMeasuredTopDisplacementsNode& mtdn = model.MeasuredTopDisplacementsNode();
  if(IsTopHorizon())
  {
    if(!IsLinkedTo(mtdn))
      LinkTo(mtdn);
  }
  else
  {
    if(IsLinkedTo(mtdn))
      UnLink(mtdn);
  }
}

const geo::ISurface& C3DHorizon::GeneratedConstantDepthSurface() const
{
	assert(ConstantDepth());
		if(!m_pConstDepthSurface)
			OnCreateConstantDepthSurface();
	return *m_pConstDepthSurface;
}

const geo::IElementSet& C3DHorizon::AsElementSet() const
{
	if(!m_pElementSetCache)
		CreateElementSetCache();

	return *m_pElementSetCache;
}

bool C3DHorizon::CanConnectItem(const CGraphNode& item) const
{
	if(IsCopy())
		return Source().CanConnectItem(item);

	const CSurfaceBase* pSurface = dynamic_cast<const CSurfaceBase*>(&item);
	if(pSurface)
		return !pSurface->Used() && !(static_cast<const CModelBase&>(Model())).BranchState().IsBranch();

	return CHorizonBase::CanConnectItem(item);
}

void C3DHorizon::OnNewNeighbour(const CGraphNode &node)
{
	// Handling surfaces
	const CSurfaceBase* pSurf = dynamic_cast<const CSurfaceBase*>(&node);
	if(pSurf)
	{
		if(ConstantDepth())
		{
			delete m_pDepth;
			m_pDepth = 0;
			InvalidateConstantDepthSurface();
		}

		m_vcSurfaces.push_back(const_cast<CSurfaceBase*>(pSurf));
    CModelBase& model = static_cast<CModelBase&>(Model());
    model.Mesh().InvalidateMesh();
		Modified();
	}

  if(!dynamic_cast<const CMeasuredTopDisplacementsNode*>(&node))
    UpdateTopHorizonState();

	CHorizonBase::OnNewNeighbour(node);
}

#ifdef _DEBUG
void C3DHorizon::AssertValid() const
{

	for(size_t i = 0; i < referenceSize(); i++)
	{
		const CSurfaceBase *pSurface = dynamic_cast<const CSurfaceBase*> (&referenceAt(i));
		if(pSurface)
			{
			if(IsCopy())
			{
				if(pSurface->IsCopied())
					assert(IsLinkedTo(pSurface->Copy()));
			}
			else
			{
				assert(IsLinkedTo(*pSurface));
			}
		}

	}

	CHorizonBase::AssertValid();
}
#endif //_DEBUG

void C3DHorizon::LoadStream(TSTREAM& stream, CStreamVersion& version, TPROGRESS& progress)
{
	CSurfaceEntry& surface_entry = (CSurfaceEntry&)*Model().GraphEntry(MD_BASE_SURFACE);
	assert(!m_pConstDepthSurface);
	// Load base first (Loads name, color and index)
	if(version < CStreamVersion(3, 0, 43))
	{
		CColorNode::LoadStream(stream, version, progress);
		// Set l'indicateur du progress
		std::string pr("Loading Horizon ");
		pr += Name().toStdString();
		progress.StatusMessage(pr.c_str());
	}
	else
		CHorizonBase::LoadStream(stream, version, progress);

	// Const or not
	int bConstant;
	stream >> bConstant;
	if(bConstant)
	{
		double dDepth;
		stream >> dDepth;
		m_pDepth = new CLengthQuantity(dDepth);
		if(CStreamVersion(3, 0, 26) < version)
		{
			stream >> m_nDivisionNorthing;
			stream >> m_nDivisionEasting;
		}
	}
	else
	{
		// Load distributed surfaces
		int nSurfaceSize;
		stream >> nSurfaceSize;
		for(int i = 0; i < nSurfaceSize; i++)
		{
			int nSurfaceIndex;
			stream >> nSurfaceIndex;
			CSurfaceBase *pSurface = surface_entry.FindIndex(nSurfaceIndex);
			assert(pSurface);
			LinkTo(*pSurface);
		}
	}

	if(version < CStreamVersion(3, 0, 43))
	{
		// Slip
		int bSlip;
		stream >> bSlip;
		if(bSlip)
		{
			double dCohesion, dFrictionAngle;
			stream >> dCohesion;
			stream >> dFrictionAngle;

			Slip(true);
      const CModelBase& model = static_cast<const CModelBase&>(Model());
      FaultParameters(model.InitialDepletionStage()).Cohesion(dCohesion);
      FaultParameters(model.InitialDepletionStage()).FrictionAngle(dFrictionAngle);
		}

		if(CStreamVersion(3, 0, 13) < version)
		{
			int nDistributedSize;
			stream >> nDistributedSize;
			for(int i = 0; i < nDistributedSize; i++)
			{
				TValueCompositeEntry& composite_entry = (TValueCompositeEntry&)*Model().GraphEntry(MD_BASE_VALUE_COMPOSITE);
				assert(Slip());
				int nIndex;
				stream >> nIndex;
				assert(composite_entry.FindIndex(nIndex));
				LinkTo(*composite_entry.FindIndex(nIndex));
			}
		}

		progress.Step();

		assert(Model().GraphEntry(MD_BASE_HORIZON));
		LinkTo(*Model().GraphEntry(MD_BASE_HORIZON));
	}
}

void C3DHorizon::ExportConstantDepthSurface(const QString& fileName)
{
		std::vector<const IValueComponentBase*> vcNodal;	// Dummy value vector
		std::vector<const IValueComponentBase*> vcElement;	// Dummy value vector

		// Export to gocad
		geo::CMesh mesh;
		CGoCadFile file(mesh);
		file.AppendSurface(GeneratedConstantDepthSurface(), Name());
		std::auto_ptr <IProgressBase> prog;
		try {
      prog.reset(_g->prog()->create( eProgress::Geo, ""));
			file.Save(fileName, *prog);
		} catch (CProgressCancel* p)
		{
			delete p;
			file.Close();
		}
}

void C3DHorizon::SaveStream(TSTREAM& stream, TPROGRESS& progress)
{
	// Save base first
	CHorizonBase::SaveStream(stream, progress);

	std::string pr("Saving Horizon ");
	pr += Name().toStdString();

	progress.StatusMessage(pr.c_str());

	// Save const depth
	if (ConstantDepth())
	{
		stream << (int) TRUE; // Qt\4.6.1\src\corelib\global\qglobal.h, line 919.
		stream << m_pDepth->Value();
		stream << m_nDivisionNorthing;
		stream << m_nDivisionEasting;
	}
	else
	{
		stream << (int) FALSE;  // Qt\4.6.1\src\corelib\global\qglobal.h, line 920.
	
		// Save surfaces
		int nSize = SurfaceSize();
		stream << nSize;
		for(size_t i = 0; i < m_vcSurfaces.size(); i++)
		{
			stream << m_vcSurfaces[i]->Index();
		}
	}
}

long C3DHorizon::SavedItems() const
{
	return CColorNode::SavedItems() + 1;
}

const dia::IMaterial &C3DHorizon::InterfaceMaterial(const geo::IElement &element, const CDepletionStage& stage) const
{
	const geo::CInterfaceElement *pIface = dynamic_cast<const geo::CInterfaceElement*> (&element);
	assert(pIface != 0);
	assert(pIface->FrontFace());
	assert(pIface->BackFace());

	std::vector<double> vcCohesion = FaultParameters(stage).Cohesion(element, CQuantity::SI_UNIT);
	assert(vcCohesion.size() == element.NrOfPoints());
	std::vector<double> vcFriction = FaultParameters(stage).FrictionAngle(element, CQuantity::SI_UNIT);
	assert(vcFriction.size() == element.NrOfPoints());

	// use average value inside the element
	double dAvCohesion = 0;
	for(size_t i = 0; i < vcCohesion.size(); i++) dAvCohesion += vcCohesion[i];
	dAvCohesion /= vcCohesion.size();

	// use average value inside the element
	double dAvFriction = 0;
	for(size_t i = 0; i < vcFriction.size(); i++) dAvFriction += vcFriction[i];
	dAvFriction /= vcFriction.size();

  double dDStiffNormal = FaultParameters(stage).DStiffNormal();
  double dDStiffShear = FaultParameters(stage).DStiffShear();

	CInterfaceElementMaterial *pMat = new CInterfaceElementMaterial(dAvCohesion, dAvFriction, dDStiffNormal, dDStiffShear, SlipType());

	std::pair<TMaterialSet::iterator, bool> prInsert = m_stMaterials.insert(pMat);
	if(!prInsert.second) delete pMat;

	return *(*prInsert.first);
}

double C3DHorizon::CalculateAverageDStif() const
{
  const geo::CElementGroup* pInterfaces = InterfaceElementGroup();
  assert(pInterfaces);

  if(pInterfaces->ElementSize() == 0)
    return 0;

  double dDStiffSum = 0;

  int i;
  for(i = 0; i < pInterfaces->ElementSize(); ++i)
  {
    const geo::IElement& element = pInterfaces->Element(i);
    const geo::CInterfaceElement* pIface = dynamic_cast<const geo::CInterfaceElement*>(&element);
	  assert(pIface != 0);
	  assert(pIface->FrontFace());
	  assert(pIface->BackFace());

    // get maximum Young's modulus for neighbouring elements
    // formations
    const CModelBase &model = dynamic_cast<const CModelBase &> (Model());

    double dYoung1 = 0;
    double dYoung2 = 0;

    if (pIface->FrontFace()->Parent())
    {
      const CFormationBase &For1 = *model.Mesh().Formation(*pIface->FrontFace()->Parent());
      dYoung1 = For1.ActiveMaterial().Material(*pIface->FrontFace()->Parent()).ParameterValue(IDT_VALUETYPE_YOUNGS_MODULUS);
    }
    else // TiedBodyTriangle
    {
      assert(dynamic_cast<const geo::CTiedBodyTriangle *>(pIface->FrontFace()));
      const geo::CTiedBodyTriangle *pTiedBodyTriangle = static_cast<const geo::CTiedBodyTriangle *>(pIface->FrontFace());

      for (int j = 0; j < 3; ++j)
      {
        const CFormationBase &For1 = *model.Mesh().Formation(*pTiedBodyTriangle->BodyTriangle(j)->Parent());
        dYoung1 += For1.ActiveMaterial().Material(*pTiedBodyTriangle->BodyTriangle(j)->Parent()).ParameterValue(IDT_VALUETYPE_YOUNGS_MODULUS);
      }

      dYoung1 /= 3;
    }
    
    if (pIface->BackFace()->Parent())
    {
      const CFormationBase &For2 = *model.Mesh().Formation(*pIface->BackFace()->Parent());
      dYoung2 = For2.ActiveMaterial().Material(*pIface->BackFace()->Parent()).ParameterValue(IDT_VALUETYPE_YOUNGS_MODULUS);
    }
    else // TiedBodyTriangle
    {
      assert(dynamic_cast<const geo::CTiedBodyTriangle *>(pIface->BackFace()));
      const geo::CTiedBodyTriangle *pTiedBodyTriangle = static_cast<const geo::CTiedBodyTriangle *>(pIface->BackFace());

      for (int j = 0; j < 3; ++j)
      {
        const CFormationBase &For2 = *model.Mesh().Formation(*pTiedBodyTriangle->BodyTriangle(j)->Parent());
        dYoung2 += For2.ActiveMaterial().Material(*pTiedBodyTriangle->BodyTriangle(j)->Parent()).ParameterValue(IDT_VALUETYPE_YOUNGS_MODULUS);
      }

      dYoung2 /= 3;
    }
    
    // wedx 15012008: Change request by Shell/Gerd-Jan Schreppers
    // use SQRT(area) of the interface element as representative length instead
    // of the distance between point 1 and point 2
    const geo::IElement* pFront = pIface->FrontFace();
    assert(pFront);
    double dIFaceArea = pFront->Size();
    assert(dIFaceArea > 0);
    double dRepLength = sqrt(dIFaceArea); // representative length

    double dDStiff = (1e7 * std::max(dYoung1, dYoung2)) / dRepLength;
    dDStiffSum += dDStiff;
  }

  dDStiffSum /= pInterfaces->ElementSize();

  return dDStiffSum;
}

bool C3DHorizon::IsTopHorizon() const
{
  //assert(false);
  return false;
}
