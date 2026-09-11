// PressureSupportNode.cpp: implementation of the CPressureSupportNode class.
//
//////////////////////////////////////////////////////////////////////

#include "PressureSupportNode.h"
#include "TetraMesh.h"
#include "TetraBoundary.h"
#include "DepletionStage.h"

#include "DisplacementSupportNode.h"
#include "SurfaceDesc.h"
#include "TetSurface.h"
#include "resourceIDP.h"
#include "GlobalMessage.h"
#include "resourceIDI.h"
#include "StreamVersion.h"
#include "ModelBase.h"
#include "resourceIDS.h"
#include "TetraHorizonBase.h"
#include "ValueType.h"
#include "ValueTypes.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CPressureSupportNode::CPressureSupportNode(CInterfaceBoundary &boundary) :
	C3DSupportNode(IDS_PRESSURE_SUPPORT_NODE, boundary), m_pSelectedSurface(0)
{

}

CPressureSupportNode::CPressureSupportNode(const CPressureSupportNode &rhs) :
	C3DSupportNode(rhs)
{
}

CPressureSupportNode::~CPressureSupportNode()
{
}

bool CPressureSupportNode::Less(const CGraphNode &rhs) const
{
	if(dynamic_cast<const CDisplacementSupportNode *> (&rhs) != 0) return true;

	return false;
}

unsigned int CPressureSupportNode::IconId() const
{
  if (Mesh().IsMesh())
  {
    const CModelBase& model = static_cast<const CModelBase&>(Model());
    const CInterfaceBoundary* pBound = dynamic_cast<const CInterfaceBoundary*>(&model.Boundary());

    if (pBound)
    {
      const CValueType* pIniStresses = DistriValues(model.InitialDepletionStage());
      if (!pIniStresses && model.parentModel())
        return IDI_FACESUPPORT_PARENT;
    }

    return IDI_FACESUPPORT;
  }
	else return IDI_FACESUPPORT_DISABLED;
}

bool CPressureSupportNode::Empty() const
{
	return false;
}

long CPressureSupportNode::SavedItems() const
{
	return m_Supports.size() + C3DSupportNode::SavedItems();
}

int CPressureSupportNode::DisplayListSize() const
{
  if(m_vcSurfaces.empty())
  {
	  BuildSurfaceEdgeVector();
	  BuildSurfaceVector();
  }
	return C3DSupportNode::DisplayListSize() + m_vcSurfaces.size();
}

const geo::IObject& CPressureSupportNode::DisplayList(int nIndex) const
{
	if(nIndex < C3DSupportNode::DisplayListSize())
		return C3DSupportNode::DisplayList(nIndex);

	nIndex -= C3DSupportNode::DisplayListSize();

	return *m_vcSurfaces[nIndex].first;
}

void CPressureSupportNode::LoadStream(TSTREAM& stream, CStreamVersion& version, TPROGRESS& progress)
{
	if(CStreamVersion(3, 0, 5) < version) // new in version 3.0.6
	{
		CModelBase *pModel = static_cast<CModelBase *> (&Model());
		assert(pModel != 0);
		int sz;
		stream >> sz;
		for(int i = 0; i < sz; i++)
		{
			CSurfaceDef sd;
			sd.LoadStream(stream, version, progress);

			std::pair<TStageSupportMap::iterator, bool> prInsert = m_Supports.insert(std::make_pair(sd, CFaceSupportDef(pModel->Mesh())));

			assert(prInsert.second);
			prInsert.first->second.LoadStream(stream, version, progress, *pModel);

			progress.Step();
		}

		C3DSupportNode::LoadStream(stream, version, progress);
	}

	BuildSurfaceEdgeVector();
	BuildSurfaceVector();
}

void CPressureSupportNode::SaveStream(TSTREAM& stream, TPROGRESS& progress)
{
	int sz = m_Supports.size();
	stream << sz;
	for(TStageSupportMap::iterator it = m_Supports.begin(); it != m_Supports.end(); it++)
	{
		it->first.SaveStream(stream, progress);
		it->second.SaveStream(stream, progress);

		progress.Step();
	}

	C3DSupportNode::SaveStream(stream, progress);
}

void CPressureSupportNode::OnMeshModified()
{
  BuildSurfaceEdgeVector();
  BuildSurfaceVector();
  m_pSelectedSurface = 0;
  if(Mesh().IsMesh())
  {
	  BuildStageSupportMap();
  }

  Modified();
}

void CPressureSupportNode::BuildSurfaceVector() const
{
	m_vcSurfaces.clear();

	if(Mesh().IsMesh())
	{
    std::vector<const geo::ISurface*> vcSurfaces = Boundary().GetSideMeshSurfaces();

		for(size_t i = 0; i < vcSurfaces.size(); ++i)
			m_vcSurfaces.push_back(std::make_pair(vcSurfaces[i], SL_SIDE));

		for(int i = 0; i < Boundary().GetTopHorizon().MeshedSurfaceSize(); i++)
      m_vcSurfaces.push_back(std::make_pair(&Boundary().GetTopHorizon().MeshedSurface(i), SL_TOP));

		for(int i = 0; i < Boundary().GetBottomHorizon().MeshedSurfaceSize(); i++)
      m_vcSurfaces.push_back(std::make_pair(&Boundary().GetBottomHorizon().MeshedSurface(i), SL_BOTTOM));
	}

  if(m_Supports.empty())
    BuildStageSupportMap();

	for(size_t i = 0; i < m_vcSurfaces.size(); ++i)
	{
		const geo::ISurface& surf = *m_vcSurfaces[i].first;
		TSurfaceLocation SurfaceLocation = m_vcSurfaces[i].second;
		CSurfaceDef surfdef(surf);

		TStageSupportMap::iterator it = m_Supports.find(surfdef);
    if(it != m_Supports.end())
    {
  		CFaceSupportDef& fsd = it->second;
		  fsd.SurfaceLocation(SurfaceLocation);
    }
	}
}

COpenGLNode::CDrawDef* CPressureSupportNode::OnCreateDrawDef(const geo::IObject& object) const
{
	if(&object == m_pSelectedSurface)
	{
		return new CFilledSurfaceDrawDef(*this);
	}

	for(size_t i = 0; i < m_vcSurfaces.size(); i++)
	{
		if(&object == m_vcSurfaces[i].first)
		{
			return new CTransparentSurfaceDrawDef(*this);
		}
	}

	return new CDefaultDrawDef(*this);
}

void CPressureSupportNode::BuildStageSupportMap() const
{
	CModelBase *pModel = (CModelBase*)(&Model());
	assert(pModel);

	// any nodes removed?
	TStageSupportMap::iterator it = m_Supports.begin();
	while(it != m_Supports.end())
	{
		bool bFound = false;
		for(size_t i = 0; i < m_vcSurfaces.size(); i++)
		{
			if(it->first == CSurfaceDef(*m_vcSurfaces[i].first))
			{
				bFound = true;
				break;
			}
		}
		if(bFound) it++;
		else
		{
			TStageSupportMap::iterator temporary = it++;

			m_Supports.erase(temporary);
		}
	}

	// any nodes added?
	for(size_t i = 0; i < m_vcSurfaces.size(); i++)
	{
		m_Supports.insert(std::make_pair(CSurfaceDef(*m_vcSurfaces[i].first), CFaceSupportDef(m_vcSurfaces[i].second, pModel->Mesh())));
	}

	UpdateStageSupportMap();
}

void CPressureSupportNode::UpdateStageSupportMap() const
{
	const CModelBase *pModel = dynamic_cast<const CModelBase*> (&Model());
	assert(pModel);
	const CDepletionStage &InitStage = pModel->InitialDepletionStage();

	for(TStageSupportMap::iterator it = m_Supports.begin(); it != m_Supports.end(); it++)
	{
		it->second.Update(InitStage);
	}
}

void CPressureSupportNode::OnNeighbourModified(const CGraphNode &node, enum ModifiedHint uHint)
{
	const TDepletionStageEntry *pEntry = dynamic_cast<const TDepletionStageEntry *> (&node);
	if(pEntry)
	{
		if(pEntry->EntryNodes().size() > 0)
		{
			// number of depletion stages might have changed, update map
			UpdateStageSupportMap();
		}
	}

	C3DSupportNode::OnNeighbourModified(node, uHint);
}

bool CPressureSupportNode::UsingGlobalTensor(const CDepletionStage &Stage) const
{
	for(TStageSupportMap::iterator ito = m_Supports.begin(); ito != m_Supports.end(); ito++)
	{
		if(ito->second.SupportDef(Stage).PressureType() == CFaceSupportDef::CSupportDef::PT_GLOBAL) return true;
	}

	return false;
}

void CPressureSupportNode::OnGlobalTensorInputUndefined(const CDepletionStage &Stage) const
{
	// The global tensor is undefined for the given depletion stage
	// Reset any definitions using the global tensor to 'Free' (user has been warned)
	for(TStageSupportMap::iterator ito = m_Supports.begin(); ito != m_Supports.end(); ito++)
	{
		if(ito->second.SupportDef(Stage).PressureType() == CFaceSupportDef::CSupportDef::PT_GLOBAL)
		{
			ito->second.SupportDef(Stage).PressureType(CFaceSupportDef::CSupportDef::PT_UNLOADED);
		}
	}
}

// This function should only be called if a distribution (valuetype) is attached for the given depletion stage
// A normal vector is necessary since the tensor needs to be converted to a face load. The vector is mulitplied
// with 1e6 since Diana only knowa about Pa, not MPa.
// the equation is: Tensor * NormVec * 1e6, where the tensor is in a 3x3 matrix form
geo::CVector CPressureSupportNode::StressFromDistribution(const geo::IPoint &pt, const CDepletionStage &stage, const geo::IVector &VecNorm) const
{
	const CValueType *pVT = DistriValues(stage);

	if(pVT)
	{
		unsigned int i;
		std::vector<geo::CValue> interpolated;

		for(i = 0; i < pVT->ComponentSize(); i++)
		{
			interpolated.push_back(pVT->Component(i).ScalarData().ValuePoint(pt));
		}
		assert(interpolated.size() == 6);
		CTensor t(interpolated[0].Value(),
		    	  interpolated[1].Value(),
				  interpolated[2].Value(),
				  interpolated[3].Value(),
				  interpolated[4].Value(),
				  interpolated[5].Value());
		return geo::CVector(-(t.Matrix() * VecNorm * 1e6));
	}	
	return geo::CVector();
}

bool CPressureSupportNode::IsValueTypeAllowed(const CDepletionStage& stage) const
{
  // always allow distribution for the initial stage
  return stage.Initial() || C3DSupportNode::IsValueTypeAllowed(stage);
}

bool CPressureSupportNode::IsValidValueTypeId(unsigned int uValueType) const
{
  return (uValueType == IDT_VALUETYPE_STRESS_TENSOR);
}

CPressureSupportNode::TSurfaceVec CPressureSupportNode::getSurfaces() const
{
  return m_vcSurfaces;
}

void CPressureSupportNode::setSelectedSurface(
  const geo::ISurface* selectedSurface)
{
  m_pSelectedSurface = selectedSurface;
}

const geo::ISurface* CPressureSupportNode::getSelectedSurface() const
{
  return m_pSelectedSurface;
}

CPressureSupportNode::TStageSupportMap&
  CPressureSupportNode::getSupports() const
{
  return m_Supports;
}

const CPressureSupportNode::CFaceSupportDef &CPressureSupportNode::FaceSupportDef(const CSurfaceDef &SurfaceDef) const
{
  if(m_vcSurfaces.empty())
  {
	  BuildSurfaceEdgeVector();
	  BuildSurfaceVector();
  }

  BuildStageSupportMap();

	TStageSupportMap::const_iterator it = m_Supports.find(SurfaceDef);
	assert(it != m_Supports.end());

	return it->second;
}

// CDefaultDrawDef implementation
CPressureSupportNode::CDefaultDrawDef::CDefaultDrawDef(const COpenGLNode& node) :
	CDrawDef(node)
{
}

CPressureSupportNode::CDefaultDrawDef::DrawDecisionFloat
  CPressureSupportNode::CDefaultDrawDef::LineWidth() const
{
	return std::make_pair <DrawDecisionFloat::first_type, DrawDecisionFloat::second_type> (true, 2.0);
}

// CFilledSurfaceDrawDef implementation
CPressureSupportNode::CFilledSurfaceDrawDef::CFilledSurfaceDrawDef(const COpenGLNode& node) :
	CDrawDef(node)
{
}

CPressureSupportNode::CFilledSurfaceDrawDef::DrawDecisionVector
  CPressureSupportNode::CFilledSurfaceDrawDef::Color(const geo::IObject &/*object*/) const
{
	std::vector<TColor> c(1);
	c[0] = qRgb(255, 255, 0);

	return std::make_pair(true, c);
}

CPressureSupportNode::CFilledSurfaceDrawDef::DrawDecisionBool
  CPressureSupportNode::CFilledSurfaceDrawDef::PolyFillFront() const
{
	return std::make_pair(true, true);
}

CPressureSupportNode::CFilledSurfaceDrawDef::DrawDecisionBool
  CPressureSupportNode::CFilledSurfaceDrawDef::PolyFillBack() const
{
	return std::make_pair(true, true);
}

// CTransparentSurfaceDrawDef implementation
CPressureSupportNode::CTransparentSurfaceDrawDef::CTransparentSurfaceDrawDef(const COpenGLNode& node) :
	CDrawDef(node)
{
}

CPressureSupportNode::CTransparentSurfaceDrawDef::DrawDecisionBool
  CPressureSupportNode::CTransparentSurfaceDrawDef::PolyFillFront() const
{
	return std::make_pair(true, true);
}

CPressureSupportNode::CTransparentSurfaceDrawDef::DrawDecisionBool
  CPressureSupportNode::CTransparentSurfaceDrawDef::PolyFillBack() const
{
	return std::make_pair(true, true);
}

CPressureSupportNode::CTransparentSurfaceDrawDef::DrawDecisionVector
  CPressureSupportNode::CTransparentSurfaceDrawDef::Color(const geo::IObject &/*object*/) const
{
	std::vector<IDrawDef::TColor> c(1);
	c[0] = qRgb(0, 0, 0);

	return std::make_pair(true, c);
}

// CSupportDef implementation
CPressureSupportNode::CFaceSupportDef::CSupportDef::CSupportDef()
: m_nPressureType(PT_UNLOADED), m_vecLocal(0, 0, 0)
{
}

CPressureSupportNode::CFaceSupportDef::CSupportDef::~CSupportDef()
{
}

bool CPressureSupportNode::CFaceSupportDef::CSupportDef::operator==(const CSupportDef &rhs) const
{
	if(m_nPressureType != rhs.m_nPressureType) return false;

	if(m_nPressureType == PT_LOCAL && m_vecLocal != rhs.m_vecLocal) return false;

	return true;
}

CPressureSupportNode::CFaceSupportDef::CSupportDef::TPressureType CPressureSupportNode::CFaceSupportDef::CSupportDef::PressureType() const
{
	return m_nPressureType;
}

void CPressureSupportNode::CFaceSupportDef::CSupportDef::PressureType(TPressureType nType)
{
	m_nPressureType = nType;
}

const geo::IVector &CPressureSupportNode::CFaceSupportDef::CSupportDef::LocalVector() const
{
	return m_vecLocal;
}

geo::IVector &CPressureSupportNode::CFaceSupportDef::CSupportDef::LocalVector()
{
	return m_vecLocal;
}

void CPressureSupportNode::CFaceSupportDef::CSupportDef::LoadStream(TSTREAM &stream, CStreamVersion &/*version*/, TPROGRESS &/*progress*/)
{
	int dum;
	double x, y, z;

	stream >> dum;
	m_nPressureType = (TPressureType) dum;

	stream >> x;
	stream >> y;
	stream >> z;

	m_vecLocal.X(x);
	m_vecLocal.Y(y);
	m_vecLocal.Z(z);
}

void CPressureSupportNode::CFaceSupportDef::CSupportDef::SaveStream(TSTREAM &stream, TPROGRESS &/*progress*/)
{
	stream << (int) m_nPressureType;
	stream << m_vecLocal.X();
	stream << m_vecLocal.Y();
	stream << m_vecLocal.Z();
}

// CFaceSupportDef implementation
CPressureSupportNode::CFaceSupportDef::CFaceSupportDef(CMeshBase& mesh)
: m_nSupportType(ST_NORMAL),
//  m_bPreScribedDisplacement(false),
  m_SurfaceLocation(SL_SIDE),
  m_mesh(mesh)
{
}

CPressureSupportNode::CFaceSupportDef::CFaceSupportDef(TSurfaceLocation SurfaceLocation, CMeshBase& mesh)
: m_nSupportType(ST_NORMAL),
//  m_bPreScribedDisplacement(false),
  m_SurfaceLocation(SurfaceLocation),
  m_mesh(mesh)
{
  // default depends on surface location
  if(SurfaceLocation == SL_TOP)
	  m_nSupportType = ST_UNSUPPORTED;
  else if(SurfaceLocation == SL_SIDE)
	  m_nSupportType = ST_HORIZONTAL;
}

CPressureSupportNode::CFaceSupportDef::CFaceSupportDef(const CFaceSupportDef &rhs)
: m_SupportMap(rhs.m_SupportMap),
  m_nSupportType(rhs.m_nSupportType), 
//  m_bPreScribedDisplacement(rhs.m_bPreScribedDisplacement),
  m_SurfaceLocation(rhs.m_SurfaceLocation),
  m_mesh(rhs.m_mesh)
{
}

CPressureSupportNode::CFaceSupportDef &CPressureSupportNode::CFaceSupportDef::operator=(const CFaceSupportDef &rhs)
{
	m_SupportMap = rhs.m_SupportMap;
//	m_bPreScribedDisplacement = rhs.m_bPreScribedDisplacement;

	if((m_SurfaceLocation == SL_BOTTOM && rhs.m_nSupportType != ST_HORIZONTAL && rhs.m_nSupportType != ST_UNSUPPORTED && rhs.m_nSupportType != ST_VERTICAL) ||
	   (m_SurfaceLocation == SL_SIDE && rhs.m_nSupportType != ST_UNSUPPORTED && rhs.m_nSupportType != ST_VERTICAL) ||
     (m_SurfaceLocation == SL_TOP && (rhs.m_nSupportType == ST_UNSUPPORTED || rhs.m_nSupportType == ST_VERTICAL || rhs.m_nSupportType == ST_FULL)))
	{
		m_nSupportType = rhs.m_nSupportType;
	}

	return *this;
}

bool CPressureSupportNode::CFaceSupportDef::operator==(const CFaceSupportDef &rhs) const
{
	if(m_nSupportType != rhs.m_nSupportType) return false;
//	if(m_bPreScribedDisplacement != rhs.m_bPreScribedDisplacement) return false;
	if(m_SurfaceLocation != rhs.m_SurfaceLocation) return false;

	for(TSupportMap::const_iterator it = m_SupportMap.begin(); it != m_SupportMap.end(); it++)
	{
		TSupportMap::const_iterator itrhs = rhs.m_SupportMap.find(it->first);
		if(itrhs == rhs.m_SupportMap.end()) return false; // not even the same depletion stages
		if(!(it->second == itrhs->second)) return false; // not the same supportdef
	}

	return true;
}

CPressureSupportNode::CFaceSupportDef::TSupportType CPressureSupportNode::CFaceSupportDef::SupportType() const
{
	return m_nSupportType;
}

void CPressureSupportNode::CFaceSupportDef::SupportType(TSupportType nType)
{
  if(m_nSupportType != nType)
  {
	  m_nSupportType = nType;
    m_mesh.InvalidateSupports();
  }
}

CPressureSupportNode::TSurfaceLocation CPressureSupportNode::CFaceSupportDef::SurfaceLocation() const
{
	return m_SurfaceLocation;
}

void CPressureSupportNode::CFaceSupportDef::SurfaceLocation(TSurfaceLocation SurfaceLocation)
{
	m_SurfaceLocation = SurfaceLocation;

	// make sure support type is valid for the surface location
	if(m_SurfaceLocation == SL_SIDE)
	{
		if(m_nSupportType == ST_UNSUPPORTED || m_nSupportType == ST_VERTICAL)
			m_nSupportType = ST_HORIZONTAL;
	}
	else if(m_SurfaceLocation == SL_TOP)
	{
    if(m_nSupportType != ST_UNSUPPORTED && m_nSupportType != ST_VERTICAL && m_nSupportType != ST_FULL)
		  m_nSupportType = ST_UNSUPPORTED;
	}
	else if(m_SurfaceLocation == SL_BOTTOM)
	{
		if(m_nSupportType == ST_HORIZONTAL || m_nSupportType == ST_UNSUPPORTED || m_nSupportType == ST_VERTICAL)
			m_nSupportType = ST_NORMAL;
	}
}

const CPressureSupportNode::CFaceSupportDef::CSupportDef &CPressureSupportNode::CFaceSupportDef::SupportDef(const CDepletionStage &stage) const
{
	TSupportMap::const_iterator it = m_SupportMap.find(&stage);
	assert(it != m_SupportMap.end());

	return it->second;
}

CPressureSupportNode::CFaceSupportDef::CSupportDef &CPressureSupportNode::CFaceSupportDef::SupportDef(const CDepletionStage &stage)
{
	TSupportMap::iterator it = m_SupportMap.find(&stage);
	assert(it != m_SupportMap.end());

	return it->second;
}
/*
bool CPressureSupportNode::CFaceSupportDef::PreScribedDisplacement() const
{
	return m_bPreScribedDisplacement;
}

void CPressureSupportNode::CFaceSupportDef::PreScribedDisplacement(bool bEnable)
{
	m_bPreScribedDisplacement = bEnable;
}
*/
void CPressureSupportNode::CFaceSupportDef::Update(const CDepletionStage &Initial)
{
	typedef std::set<const CDepletionStage *> TStageSet;
	TStageSet stStages;

	assert(Initial.Initial());

	const CDepletionStage *pStage = &Initial;
	if(pStage->Last()) pStage = 0;
	else pStage = &pStage->Next();

	while(pStage)
	{
		VERIFY(stStages.insert(pStage).second);
		if(!pStage->Last())
			pStage = &pStage->Next();
		else
			pStage = 0;
	}

	TSupportMap::iterator it = m_SupportMap.begin();
	while(it != m_SupportMap.end())
	{
		if(stStages.find(it->first) == stStages.end())
		{
			TSupportMap::iterator temporary = it++;

			// stage doesn't exist anymore, delete it from the map
			m_SupportMap.erase(temporary);
		}
		else it++;
	}

	for(TStageSet::iterator its = stStages.begin(); its != stStages.end(); its++)
	{
		m_SupportMap.insert(std::make_pair(*its, CSupportDef()));
	}
}

void CPressureSupportNode::CFaceSupportDef::LoadStream(TSTREAM &stream, CStreamVersion &version, TPROGRESS &progress, const CModelBase &model)
{
	int sz;
	stream >> sz;

	const CDepletionStage *pStage = &model.InitialDepletionStage();
	if(pStage->Last()) pStage = 0;
	else pStage = &pStage->Next();

	for(int i = 0; i < sz; i++)
	{
		CSupportDef def;
		std::pair<TSupportMap::iterator, bool> prInsert = m_SupportMap.insert(std::make_pair(pStage, def));
		assert(prInsert.second);
		prInsert.first->second.LoadStream(stream, version, progress);

		assert(pStage != 0);

		if(pStage->Last()) pStage = 0;
		else pStage = &pStage->Next();
	}

	int dum;
	stream >> dum;
	m_nSupportType = (TSupportType) dum;

	// wedx 20-02-2007: disabled STRESS support mode
	// The gm42 kernel was modified such that initial stresses are calculated there.
	// Initial stress input is no longer done, but stress supports used that input.
	// Until gm42 supports setting this there, stress support mode is disabled.
	if(m_nSupportType == ST_STRESS)
	{
		if(!model.isPressureSupportNodeLoadWarningDone())
		{
			_m()->msg(IDP_NO_STRESS_SUPPORT);
			(const_cast<CModelBase&>(model)).setPressureSupportNodeLoadWarningDone();
		}
		m_nSupportType = ST_NORMAL;
	}

	if(CStreamVersion(3, 0, 64) < version && version < CStreamVersion(3, 0, 104))
  {
    // not used anymore
    int bDummy;
		stream >> bDummy;
  }
}

void CPressureSupportNode::CFaceSupportDef::SaveStream(TSTREAM &stream, TPROGRESS &progress)
{
	int sz = m_SupportMap.size();
	stream << sz;
	for(TSupportMap::iterator it = m_SupportMap.begin(); it != m_SupportMap.end(); it++)
	{
		// ignore depletion stages, just retrieve them when loading
		it->second.SaveStream(stream, progress);
	}

	stream << (int) m_nSupportType;
//	stream << m_bPreScribedDisplacement;
}

// CSurfaceDef implementation
CPressureSupportNode::CSurfaceDef::CSurfaceDef()
{
}

CPressureSupportNode::CSurfaceDef::CSurfaceDef(const geo::ISurface &s)
:	m_ptMin(s.Min()),
	m_ptMax(s.Max())
{
}

bool CPressureSupportNode::CSurfaceDef::operator<(const CSurfaceDef &rhs) const
{
	if(m_ptMin < rhs.m_ptMin) return true;
	if(rhs.m_ptMin < m_ptMin) return false;

	return (m_ptMax < rhs.m_ptMax);
}

bool CPressureSupportNode::CSurfaceDef::operator==(const CSurfaceDef &rhs) const
{
	return (m_ptMin == rhs.m_ptMin && m_ptMax == rhs.m_ptMax);
}

void CPressureSupportNode::CSurfaceDef::LoadStream(TSTREAM &stream, CStreamVersion &/*version*/, TPROGRESS &/*progress*/)
{
	double x, y, z;
	stream >> x;
	stream >> y;
	stream >> z;
	m_ptMin = geo::CPoint(x, y, z);
	stream >> x;
	stream >> y;
	stream >> z;
	m_ptMax = geo::CPoint(x, y, z);
}

void CPressureSupportNode::CSurfaceDef::SaveStream(TSTREAM &stream, TPROGRESS &/*progress*/) const
{
	stream << m_ptMin.X();
	stream << m_ptMin.Y();
	stream << m_ptMin.Z();
	stream << m_ptMax.X();
	stream << m_ptMax.Y();
	stream << m_ptMax.Z();
}
