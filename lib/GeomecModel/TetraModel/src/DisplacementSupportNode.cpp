// DisplacementSupportNode.cpp: implementation of the CDisplacementSupportNode class.
//
//////////////////////////////////////////////////////////////////////

#include "DisplacementSupportNode.h"
#include "TetraMesh.h"
#include "HexaMesh.h"
#include "DepletionStage.h"
#include "Line.h"
#include "resourceIDS.h"
#include "resourceIDI.h"
#include "StreamVersion.h"
#include "ModelBase.h"
#include "ValueType.h"
#include "ValueTypes.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CDisplacementSupportNode::CDisplacementSupportNode(CInterfaceBoundary &boundary)
:	C3DSupportNode(IDS_DISPLACEMENT_SUPPORT_NODE, boundary),
  m_pSelectedPoint(0),
  m_pReferencePoint(0)
{
}

CDisplacementSupportNode::~CDisplacementSupportNode()
{
}

bool CDisplacementSupportNode::Less(const CGraphNode &/*rhs*/) const
{
	return false;
}

unsigned int CDisplacementSupportNode::IconId() const
{
  if (Mesh().IsMesh())
  {
    const CModelBase& model = static_cast<const CModelBase&>(Model());

    // always use distributions if there's no parent model
    bool bUserLoads = (model.parentModel() == 0);

    if (!bUserLoads)
    {
      const CDepletionStage *pStage = &model.InitialDepletionStage();

      while (pStage)
      {
        if (Distributed(*pStage))
        {
          bUserLoads = true;
          break;
        }

        pStage = pStage->Last() ? 0 : &pStage->Next();
      }
    }

    if (bUserLoads)
      return IDI_NODALSUPPORT;
    else
      return IDI_NODALSUPPORT_PARENT;
  }
	else return IDI_NODALSUPPORT_DISABLED;
}

int CDisplacementSupportNode::DisplayListSize() const
{
  if(m_vcCornerNodes.empty())
  {
	  BuildSurfaceEdgeVector();
	  GetCornerNodes();
  }
	return C3DSupportNode::DisplayListSize() + m_vcCornerNodes.size();
}

const geo::IObject& CDisplacementSupportNode::DisplayList(int nIndex) const
{
	if(nIndex < C3DSupportNode::DisplayListSize())
		return C3DSupportNode::DisplayList(nIndex);

	nIndex -= C3DSupportNode::DisplayListSize();

	return *m_vcCornerNodes[nIndex];
}

bool CDisplacementSupportNode::Empty() const
{
	return false;
}

long CDisplacementSupportNode::SavedItems() const
{
	return m_Supports.size() + C3DSupportNode::SavedItems();
}

void CDisplacementSupportNode::LoadStream(TSTREAM& stream, CStreamVersion& version, TPROGRESS& progress)
{
	if(CStreamVersion(3, 0, 5) < version) // new in version 3.0.6
	{
		const CModelBase *pModel = dynamic_cast<const CModelBase *> (&Model());
		assert(pModel != 0);
		int sz;
		stream >> sz;
		for(int i = 0; i < sz; i++)
		{
			double x, y, z;
			stream >> x;
			stream >> y;
			stream >> z;
			geo::CPoint pt(x, y, z);

			std::pair<TStageSupportMap::iterator, bool> prInsert = m_Supports.insert(std::make_pair(pt, CNodalSupportDef()));
			assert(prInsert.second);
			prInsert.first->second.LoadStream(stream, version, progress, *pModel);

			progress.Step();
		}

		C3DSupportNode::LoadStream(stream, version, progress);
	}

	BuildSurfaceEdgeVector();
	GetCornerNodes();
}

void CDisplacementSupportNode::SaveStream(TSTREAM& stream, TPROGRESS& progress)
{
	int sz = m_Supports.size();
	stream << sz;
	for(TStageSupportMap::iterator it = m_Supports.begin(); it != m_Supports.end(); it++)
	{
		stream << it->first.X();
		stream << it->first.Y();
		stream << it->first.Z();

		it->second.SaveStream(stream, progress);

		progress.Step();
	}

	C3DSupportNode::SaveStream(stream, progress);
}

void CDisplacementSupportNode::DetermineReferencePoint(const geo::IPoint *pt) const
{
	if(!m_pReferencePoint) m_pReferencePoint = pt;
	else
	{
		if(     pt->X() < m_pReferencePoint->X()) m_pReferencePoint = pt;
		else if(pt->Y() < m_pReferencePoint->Y()) m_pReferencePoint = pt;
		else if(pt->Z() > m_pReferencePoint->Z()) m_pReferencePoint = pt;
	}
}

void CDisplacementSupportNode::GetCornerNodes() const
{
	std::set<const geo::ILine*, CLineLess> stLines;

  if(m_vcSurfaceEdges.empty())
    BuildSurfaceEdgeVector();

	// first get the unique lines
	for(TSurfaceEdgeVec::iterator ite = m_vcSurfaceEdges.begin(); ite != m_vcSurfaceEdges.end(); ite++)
	{
    for(size_t i = 0; i < ite->Size(); i++)
		{
			const geo::ILine &l = ite->Object(i);
			stLines.insert(&l);
		}
	}

	
	TPtLinesMap mpLines;
	TPtLinesMap::iterator it;
	m_vcCornerNodes.clear();
	m_pReferencePoint = 0;

	for(std::set<const geo::ILine*, CLineLess>::iterator its = stLines.begin(); its != stLines.end(); its++)
	{
		const geo::ILine *pl = *its;
	
		it = mpLines.insert(std::make_pair(&pl->First(), std::vector<const geo::ILine*>())).first;
		it->second.push_back(pl);

		it = mpLines.insert(std::make_pair(&pl->Second(), std::vector<const geo::ILine*>())).first;
		it->second.push_back(pl);

	/*	TLineMap::iterator it = mpLines.insert(std::make_pair(&pl->First(), 0)).first;
		it->second++;
		it = mpLines.insert(std::make_pair(&pl->Second(), 0)).first;
		it->second++;*/
	}

  bool bIsHexa = dynamic_cast<const CHexaMesh *>(&Mesh());
	
	for(it = mpLines.begin(); it != mpLines.end(); it++)
	{
    bool bTry = bIsHexa ? it->second.size() == 3 : it->second.size() > 2;
    if (bTry)
		{
			const geo::IPoint *ppt = &(*it->first);
			m_vcCornerNodes.push_back(ppt);
			DetermineReferencePoint(ppt);
		}
	}
/*
	if(m_vcCornerNodes.size() == 0) // apparently a surface connected to itself, open loop, or a closed loop....
	{ 
		for(it = mpLines.begin(); it != mpLines.end(); it++)
		{
			assert(!(it->second.size() > 3));
			if(it->second.size() > 1) // we need two lines to calculate an angle.... 
			{
				const geo::ILine *pl1 = it->second[0];
				const geo::ILine *pl2 = it->second[1];

				geo::CVector vc1(pl1->First(), pl1->Second());
				geo::CVector vc2(pl2->First(), pl2->Second());

				double dAngle = vc1.AngleDeg(vc2);
				if(dAngle > 20. && dAngle < 160.)
				{
					m_vcCornerNodes.push_back(it->first);
					DetermineReferencePoint(it->first);
				}
			}
		}
	}
*/
/*	if(m_vcCornerNodes.size() == 0) // still no nodes????
	{ // brute force, we take em all!!
		for(it = mpLines.begin(); it != mpLines.end(); it++)
		{
			m_vcCornerNodes.push_back(it->first);
			DetermineReferencePoint(it->first);
		}
	}
*/
	if(!m_pReferencePoint)
  {
    DetermineGenericReferencePoint(mpLines);
    if(m_pReferencePoint)
      m_vcCornerNodes.push_back(m_pReferencePoint);
  }
}

void CDisplacementSupportNode::DetermineGenericReferencePoint(TPtLinesMap &mpLines) const
{
	// no reference point was found because there are no corner points
	// now find the best one from the line end points
	for(TPtLinesMap::iterator it = mpLines.begin(); it != mpLines.end(); it++)
	{
		const geo::IPoint *ppt = &(*it->first);
		DetermineReferencePoint(ppt);
	}

	// now we really should have a reference point
	assert(!Mesh().IsMesh() || m_pReferencePoint != 0);
}

void CDisplacementSupportNode::UpdateStageSupportMap() const
{
	const CModelBase *pModel = dynamic_cast<const CModelBase*> (&Model());
	assert(pModel);
	const CDepletionStage &InitStage = pModel->InitialDepletionStage();

	for(TStageSupportMap::iterator it = m_Supports.begin(); it != m_Supports.end(); it++)
	{
		it->second.Update(InitStage);
	}
}

void CDisplacementSupportNode::OnNeighbourModified(const CGraphNode &node, enum ModifiedHint uHint)
{
	const TDepletionStageEntry *pEntry = dynamic_cast<const TDepletionStageEntry *> (&node);
	if(pEntry)
	{
		if(pEntry->EntryNodes().size() > 0)
		{
			// number of depletion stages might have changed, update map
			UpdateStageSupportMap();
//			CreateChildren();
		}
	}

	C3DSupportNode::OnNeighbourModified(node, uHint);
}

void CDisplacementSupportNode::BuildStageSupportMap() const
{
	const CModelBase *pModel = dynamic_cast<const CModelBase *> (&Model());
	assert(pModel);

	// any nodes removed?
	TStageSupportMap::iterator it = m_Supports.begin();
	while(it != m_Supports.end())
	{
		bool bFound = false;
		for(size_t i = 0; i < m_vcCornerNodes.size(); i++)
		{
			if(it->first == *m_vcCornerNodes[i])
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
	for(size_t i = 0; i < m_vcCornerNodes.size(); i++)
	{
		m_Supports.insert(std::make_pair(geo::CPoint(*m_vcCornerNodes[i]), CNodalSupportDef()));
	}

	UpdateStageSupportMap();
}

void CDisplacementSupportNode::OnMeshModified()
{
	BuildSurfaceEdgeVector();
	GetCornerNodes();
  m_pSelectedPoint = 0;
	if(Mesh().IsMesh())
	{
		BuildStageSupportMap();
	}

	Modified();
}

COpenGLNode::CDrawDef* CDisplacementSupportNode::OnCreateDrawDef(const geo::IObject& object) const
{
	if(&object == m_pSelectedPoint)
	{
		return new CSelectedNodeDrawDef(*this);
	}

	if(&object == &ReferencePoint())
	{
		return new CReferenceNodeDrawDef(*this);
	}

	if(dynamic_cast<const geo::IPoint*> (&object) != 0)
	{
		return new CNodeDrawDef(*this);
	}

	return C3DSupportNode::OnCreateDrawDef(object);
}

bool CDisplacementSupportNode::UsingGlobalTensor(const CDepletionStage &Stage) const
{
	for(TStageSupportMap::iterator ito = m_Supports.begin(); ito != m_Supports.end(); ito++)
	{
		if(ito->second.SupportDef(Stage).SupportType() == CNodalSupportDef::CSupportDef::ST_GLOBAL) return true;
	}

	return false;
}

void CDisplacementSupportNode::OnGlobalTensorInputUndefined(const CDepletionStage &Stage) const
{
	// The global tensor is undefined for the given depletion stage
	// Reset any definitions using the global tensor to 'Free' (user has been warned)
	for(TStageSupportMap::iterator ito = m_Supports.begin(); ito != m_Supports.end(); ito++)
	{
		if(ito->second.SupportDef(Stage).SupportType() == CNodalSupportDef::CSupportDef::ST_GLOBAL)
		{
			ito->second.SupportDef(Stage).SupportType(CNodalSupportDef::CSupportDef::ST_NONE);
		}
	}
}

// returns 0 if not existing for the given point
const CDisplacementSupportNode::CNodalSupportDef *CDisplacementSupportNode::NodalSupportDef(const geo::IPoint &pt) const
{
	TStageSupportMap::const_iterator it = m_Supports.find(geo::CPoint(pt));
	if(it == m_Supports.end()) return 0;

	return &it->second;
}

const geo::IPoint &CDisplacementSupportNode::ReferencePoint() const
{
	if(!m_pReferencePoint)
    GetCornerNodes();

	return *m_pReferencePoint;
}

geo::CVector CDisplacementSupportNode::DisplacementFromDistribution(const geo::IPoint &pt, const CDepletionStage &stage) const
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
		assert(interpolated.size() == 3);
		return geo::CVector(interpolated[0].Value(), interpolated[1].Value(), interpolated[2].Value());
	}	
	return geo::CVector();
}

geo::CVector CDisplacementSupportNode::DisplacementFromStrainTensor(const geo::IPoint &pt, const geo::IPoint &ptReference, const geo::IMatrix &MatPreMult)
{
	geo::CVector vec = pt - ptReference;
	geo::CVector vecDispla = -(MatPreMult * vec);

	return vecDispla;
}

bool CDisplacementSupportNode::IsValidValueTypeId(unsigned int uValueType) const
{
  return (uValueType == IDT_VALUETYPE_DISPLACEMENT);
}

CDisplacementSupportNode::TCornerNodeVec CDisplacementSupportNode::getCornerNodes() const
{
  return m_vcCornerNodes;
}

void CDisplacementSupportNode::setSelectedPoint(
  const geo::IPoint *selectedPoint)
{
  m_pSelectedPoint = selectedPoint;
}

const geo::IPoint *CDisplacementSupportNode::getSelectedPoint() const
{
  return m_pSelectedPoint;
}

geo::CMatrix CDisplacementSupportNode::CreateGlobalTensorPreMultMatrix(const CDepletionStage& stage) const
{
	// create the premultiplication matrix for the global tensor displacement derivation

	// return empty matrix if we don't have a global tensor defined
	if(!GlobalTensorDefined(stage)) return geo::CMatrix();

	// global tensor
	const ITensor &strain = GlobalTensor(stage);

	geo::CMatrix MatGlobalStrain(3, 3);
	MatGlobalStrain.Value(0, 0,     strain.XX());
	MatGlobalStrain.Value(0, 1, 2 * strain.XY());
	MatGlobalStrain.Value(0, 2, 2 * strain.XZ());
	MatGlobalStrain.Value(1, 1,     strain.YY());
	MatGlobalStrain.Value(1, 2, 2 * strain.YZ());
	MatGlobalStrain.Value(2, 2,     strain.ZZ());

	return MatGlobalStrain;
}

CDisplacementSupportNode::TStageSupportMap&
  CDisplacementSupportNode::getSupports() const
{
  return m_Supports;
}

// CSelectedNodeDrawDef implementation
CDisplacementSupportNode::CSelectedNodeDrawDef::CSelectedNodeDrawDef(const COpenGLNode& node)
:	CDrawDef(node)
{
}

CDisplacementSupportNode::CSelectedNodeDrawDef::DrawDecisionVector
  CDisplacementSupportNode::CSelectedNodeDrawDef::Color(const geo::IObject &/*object*/) const
{
	std::vector<TColor> c(1);
	c[0] = qRgb(255, 0, 0);

	return std::make_pair(true, c);
}

CDisplacementSupportNode::CSelectedNodeDrawDef::DrawDecisionFloat
  CDisplacementSupportNode::CSelectedNodeDrawDef::PointSize() const
{
	return std::make_pair <DrawDecisionFloat::first_type, DrawDecisionFloat::second_type> (true, 10.0);
}

// CReferenceNodeDrawDef implementation
CDisplacementSupportNode::CReferenceNodeDrawDef::CReferenceNodeDrawDef(const COpenGLNode& node)
:	CDrawDef(node)
{
}

CDisplacementSupportNode::CReferenceNodeDrawDef::DrawDecisionVector
  CDisplacementSupportNode::CReferenceNodeDrawDef::Color(const geo::IObject &/*object*/) const
{
	std::vector<TColor> c(1);
	c[0] = qRgb(0, 255, 0);

	return std::make_pair(true, c);
}

CDisplacementSupportNode::CReferenceNodeDrawDef::DrawDecisionFloat
  CDisplacementSupportNode::CReferenceNodeDrawDef::PointSize() const
{
	return std::make_pair <DrawDecisionFloat::first_type, DrawDecisionFloat::second_type> (true, 7.0);
}

// CNodeDrawDef implementation
CDisplacementSupportNode::CNodeDrawDef::CNodeDrawDef(const COpenGLNode& node) :
	CDrawDef(node)
{
}

CDisplacementSupportNode::CNodeDrawDef::DrawDecisionVector
  CDisplacementSupportNode::CNodeDrawDef::Color(const geo::IObject &/*object*/) const
{
	std::vector<TColor> c(1);
	c[0] = qRgb(0, 0, 255);

	return std::make_pair(true, c);
}

CDisplacementSupportNode::CNodeDrawDef::DrawDecisionFloat
  CDisplacementSupportNode::CNodeDrawDef::PointSize() const
{
	return std::make_pair <DrawDecisionFloat::first_type, DrawDecisionFloat::second_type> (true, 5.0);
}

// CSupportDef implementation
CDisplacementSupportNode::CNodalSupportDef::CSupportDef::CSupportDef()
: m_nSupportType(ST_NONE), m_vecLocal(0, 0, 0)
{
}

CDisplacementSupportNode::CNodalSupportDef::CSupportDef::~CSupportDef()
{
}

bool CDisplacementSupportNode::CNodalSupportDef::CSupportDef::operator==(const CSupportDef &rhs) const
{
	if(m_nSupportType != rhs.m_nSupportType) return false;

	if(m_nSupportType == ST_LOCAL && m_vecLocal != rhs.m_vecLocal) return false;

	return true;
}

CDisplacementSupportNode::CNodalSupportDef::CSupportDef::TSupportType CDisplacementSupportNode::CNodalSupportDef::CSupportDef::SupportType() const
{
	return m_nSupportType;
}

void CDisplacementSupportNode::CNodalSupportDef::CSupportDef::SupportType(TSupportType nType)
{
	m_nSupportType = nType;
}

geo::IVector &CDisplacementSupportNode::CNodalSupportDef::CSupportDef::LocalVector()
{
	return m_vecLocal;
}

const geo::IVector &CDisplacementSupportNode::CNodalSupportDef::CSupportDef::LocalVector() const
{
	return m_vecLocal;
}

// always returns a vector based on the support type
// the matrix and points are used if ST_GLOBAL is set
geo::CVector CDisplacementSupportNode::CNodalSupportDef::CSupportDef::Vector(
		const geo::IPoint &pt,
		const geo::IMatrix &MatPreMult,
		const geo::IPoint &ptReference) const
{
	switch(SupportType())
	{
	case ST_NONE:
		return geo::CVector(0, 0, 0);
		break;
	case ST_LOCAL:
		return geo::CVector(LocalVector());
		break;
	case ST_GLOBAL:
		return CDisplacementSupportNode::DisplacementFromStrainTensor(pt, ptReference, MatPreMult);
		break;
	default:
		assert(false);
	}
	geo::CVector ret;
	return ret;
}

CDisplacementSupportNode::CNodalSupportDef::CSupportDef CDisplacementSupportNode::CNodalSupportDef::CSupportDef::Interpolate(
		const CSupportDef &rhs,
		const double &dist,
		const double &distrhs,
		const geo::IPoint &pt,
		const geo::IMatrix &MatPreMult,
		const geo::IPoint &ptReference) const
{
	CSupportDef ret;

	geo::CVector vecThis = Vector(pt, MatPreMult, ptReference);
	geo::CVector vecRhs = rhs.Vector(pt, MatPreMult, ptReference);

	// always set to ST_LOCAL
	ret.m_nSupportType = ST_LOCAL;

	double facthis = double(distrhs) / double(dist + distrhs);
	double facrhs = double(dist) / double(dist + distrhs);

	// interpolate vector
	ret.m_vecLocal = geo::CVector(
		facthis * vecThis.X() + facrhs * vecRhs.X(),
		facthis * vecThis.Y() + facrhs * vecRhs.Y(),
		facthis * vecThis.Z() + facrhs * vecRhs.Z());

	return ret;
}

void CDisplacementSupportNode::CNodalSupportDef::CSupportDef::LoadStream(TSTREAM &stream, CStreamVersion &/*version*/, TPROGRESS &/*progress*/)
{
	int dum;
	double x, y, z;
	stream >> dum;
	m_nSupportType = (TSupportType) dum;
	stream >> x;
	stream >> y;
	stream >> z;
	m_vecLocal.X(x);
	m_vecLocal.Y(y);
	m_vecLocal.Z(z);
}

void CDisplacementSupportNode::CNodalSupportDef::CSupportDef::SaveStream(TSTREAM &stream, TPROGRESS &/*progress*/)
{
	stream << (int) m_nSupportType;
	stream << m_vecLocal.X();
	stream << m_vecLocal.Y();
	stream << m_vecLocal.Z();
}

// CNodalSupportDef implementation
CDisplacementSupportNode::CNodalSupportDef::CNodalSupportDef()
{
}

CDisplacementSupportNode::CNodalSupportDef::CNodalSupportDef(const CNodalSupportDef &rhs)
	: m_SupportMap(rhs.m_SupportMap)
{
}

CDisplacementSupportNode::CNodalSupportDef &CDisplacementSupportNode::CNodalSupportDef::operator=(const CNodalSupportDef &rhs)
{
	m_SupportMap = rhs.m_SupportMap;

	return *this;
}

bool CDisplacementSupportNode::CNodalSupportDef::operator==(const CNodalSupportDef &rhs) const
{
	for(TSupportMap::const_iterator it = m_SupportMap.begin(); it != m_SupportMap.end(); it++)
	{
		TSupportMap::const_iterator itrhs = rhs.m_SupportMap.find(it->first);
		if(itrhs == rhs.m_SupportMap.end()) return false;
		if(!(it->second == itrhs->second)) return false;
	}

	return true;
}

// interpolate a new definition from distances (between this and rhs)
CDisplacementSupportNode::CNodalSupportDef CDisplacementSupportNode::CNodalSupportDef::Interpolate(
		const CNodalSupportDef &rhs,
		const double &dist,
		const double &distrhs,
		const geo::IPoint &pt,
		const geo::IMatrix &MatPreMult,
		const geo::IPoint &ptReference) const
{
	CNodalSupportDef ret;

	assert(m_SupportMap.size() == rhs.m_SupportMap.size());

	for(TSupportMap::const_iterator itthis = m_SupportMap.begin(); itthis != m_SupportMap.end(); itthis++)
	{
		const CDepletionStage &stage = *itthis->first;
		TSupportMap::const_iterator itrhs = rhs.m_SupportMap.find(&stage);
		assert(itrhs != rhs.m_SupportMap.end());

		const CSupportDef &SupDefThis = itthis->second;
		const CSupportDef &SupDefRhs = itrhs->second;

		CSupportDef interpolated = SupDefThis.Interpolate(SupDefRhs, dist, distrhs, pt, MatPreMult, ptReference);

		VERIFY(ret.m_SupportMap.insert(std::make_pair(&stage, interpolated)).second);
	}

	return ret;
}

const CDisplacementSupportNode::CNodalSupportDef::CSupportDef &CDisplacementSupportNode::CNodalSupportDef::SupportDef(const CDepletionStage &stage) const
{
	TSupportMap::const_iterator it = m_SupportMap.find(&stage);
	assert(it != m_SupportMap.end());

	return it->second;
}

CDisplacementSupportNode::CNodalSupportDef::CSupportDef &CDisplacementSupportNode::CNodalSupportDef::SupportDef(const CDepletionStage &stage)
{
	TSupportMap::iterator it = m_SupportMap.find(&stage);
	assert(it != m_SupportMap.end());

	return it->second;
}

void CDisplacementSupportNode::CNodalSupportDef::Update(const CDepletionStage &Initial)
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

void CDisplacementSupportNode::CNodalSupportDef::LoadStream(TSTREAM &stream, CStreamVersion &version, TPROGRESS &progress, const CModelBase &model)
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
}

void CDisplacementSupportNode::CNodalSupportDef::SaveStream(TSTREAM &stream, TPROGRESS &progress)
{
	int sz = m_SupportMap.size();
	stream << sz;
	for(TSupportMap::iterator it = m_SupportMap.begin(); it != m_SupportMap.end(); it++)
	{
		// ignore depletion stages, just retrieve them when loading
		it->second.SaveStream(stream, progress);
	}
}
