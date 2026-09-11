#include "InterfaceBoundary.h"
#include "ModelBase.h"
#include "MeshBase.h"
#include "MeshProgress.h"
#include "HorizonBase.h"
#include "ElementGroup.h"
#include "InterfaceElement.h"
#include "DisplacementSupportNode.h"
#include "PressureSupportNode.h"

CInterfaceBoundary::CInterfaceBoundary(const geo::IPoint& ptMin, 
			                                 const geo::IPoint& ptMax,
			                                 CFemAppModel& model,	  
			                                 BOUNDARY_STATE state)
: CBoundaryBase(ptMin, ptMax, model, state),
  m_bCreateInterfaces(model.parentModel() != 0),
  m_pInterfaceElements(0),
  m_pBoundaryInterfaceDef(new CBoundaryInterfaceDef(*this))
{
	CreateChildren();
}

CInterfaceBoundary::CInterfaceBoundary(const CInterfaceBoundary &rhs)
: CBoundaryBase(rhs),
  m_bCreateInterfaces(rhs.m_bCreateInterfaces),
  m_pInterfaceElements(0),
  m_pBoundaryInterfaceDef(new CBoundaryInterfaceDef(*rhs.m_pBoundaryInterfaceDef)),
  m_pDisplacementSupportNode(rhs.m_pDisplacementSupportNode),
  m_pPressureSupportNode(rhs.m_pPressureSupportNode)
{
}

CInterfaceBoundary::~CInterfaceBoundary()
{
  delete m_pBoundaryInterfaceDef;
}

bool CInterfaceBoundary::operator==(const CInterfaceBoundary& rhs) const
{
  if(!CBoundaryBase::operator==(rhs))
    return false;

	if(m_bCreateInterfaces != rhs.m_bCreateInterfaces)
		return false;

  if(!(*m_pBoundaryInterfaceDef == *rhs.m_pBoundaryInterfaceDef))
    return false;

  return true;
}

CInterfaceBoundary& CInterfaceBoundary::operator=(const CInterfaceBoundary& rhs)
{
  CBoundaryBase::operator=(rhs);
	m_bCreateInterfaces = rhs.m_bCreateInterfaces;
  *m_pBoundaryInterfaceDef = *rhs.m_pBoundaryInterfaceDef;

  return *this;
}

void CInterfaceBoundary::OnNeighbourModified(const CGraphNode& node, enum ModifiedHint /*uHint*/)
{
  CModelBase& model = static_cast<CModelBase&>(Model());
	if(&node == &model.Mesh())
	{
		// inform children
		m_pDisplacementSupportNode->OnMeshModified();
		m_pPressureSupportNode->OnMeshModified();

		if(m_pInterfaceElements && !model.Mesh().IsMesh())
		{
			m_pInterfaceElements = 0;
			m_vcInterfaceNode.clear();
      m_mpInterfaceElement2BoundarySurface.clear();
		}
  }
}

void CInterfaceBoundary::LoadStream(TSTREAM& stream, CStreamVersion& version, TPROGRESS& progress)
{
  CBoundaryBase::LoadStream(stream, version, progress);
  LoadProperties(stream, version, progress);
  LoadSupportNodes(stream, version, progress);
}

void CInterfaceBoundary::SaveStream(TSTREAM& stream, TPROGRESS& progress)
{
  CBoundaryBase::SaveStream(stream, progress);

	// save the settings for the interface material
  stream << int(m_bCreateInterfaces ? 1 : 0);
  m_pBoundaryInterfaceDef->SaveStream(stream, progress);
  m_pDisplacementSupportNode->SaveStream(stream, progress);
  m_pPressureSupportNode->SaveStream(stream, progress);
}

long CInterfaceBoundary::SavedItems() const
{
  long lRet = CBoundaryBase::SavedItems();
  lRet += m_pBoundaryInterfaceDef->SavedItems();
	lRet += m_pDisplacementSupportNode->SavedItems();
	lRet += m_pPressureSupportNode->SavedItems();

  return lRet;
}

void CInterfaceBoundary::LoadSupportNodes(TSTREAM& stream, CStreamVersion& version, TPROGRESS& progress)
{
  m_pDisplacementSupportNode->LoadStream(stream, version, progress);
  m_pPressureSupportNode->LoadStream(stream, version, progress);
}

void CInterfaceBoundary::LoadPre412Stream(TSTREAM& stream, CStreamVersion& version, TPROGRESS& progress)
{
  LoadProperties(stream, version, progress);
}

void CInterfaceBoundary::CreateInterfaces(bool bVal)
{
	CModelBase &model = (CModelBase&)(Model());

  // should be root model, or in some special situations when loading/importing
  assert(!model.parentModel() || model.Loading());
	if(bVal == m_bCreateInterfaces)
		return;

	m_bCreateInterfaces = bVal;

  bool bMesh = model.Mesh().IsMesh();
	model.Mesh().InvalidateMesh();
  if(bMesh && model.Mesh().CanCreateMesh())
    model.Mesh().CreateMesh();

	Modified();
}

const CBoundaryInterfaceDef& CInterfaceBoundary::InterfaceDefinition() const
{
  return *m_pBoundaryInterfaceDef;
}

CBoundaryInterfaceDef& CInterfaceBoundary::InterfaceDefinition()
{
  return *m_pBoundaryInterfaceDef;
}

void CInterfaceBoundary::ToggleInterfaces()
{
  CreateInterfaces(!CreateInterfaces());
}

bool CInterfaceBoundary::CreateInterfaces() const
{
	return m_bCreateInterfaces;
}

bool CInterfaceBoundary::HasInterfaces() const
{
  return CreateInterfaces();
}

bool CInterfaceBoundary::IsBoundaryInterface(const geo::CInterfaceElement &iface) const
{
  if(m_pInterfaceElements)
  {
    for(int i = 0; i < m_pInterfaceElements->ElementSize(); ++i)
    {
      if(&m_pInterfaceElements->Element(i) == &iface)
        return true;
    }
  }

  return false;
}

const CBoundaryInterfaceMaterial& CInterfaceBoundary::InterfaceMaterial(const geo::CInterfaceElement& iface) const
{
  assert(IsBoundaryInterface(iface));

  if(m_mpInterfaceElement2BoundarySurface.empty())
    CreateInterfaceElement2BoundarySurfaceMap();

  TInterfaceElement2BoundarySurfaceMap::iterator it = m_mpInterfaceElement2BoundarySurface.find(&iface);
  assert(it != m_mpInterfaceElement2BoundarySurface.end());

  switch(it->second)
  {
  case CBoundaryInterfaceDef::BSURF_TOP:
    return m_pBoundaryInterfaceDef->InterfaceMaterialTop(iface);
  case CBoundaryInterfaceDef::BSURF_BOTTOM:
    return m_pBoundaryInterfaceDef->InterfaceMaterialBottom(iface);
  case CBoundaryInterfaceDef::BSURF_SIDE:
    break;
  }

  assert(it->second == CBoundaryInterfaceDef::BSURF_SIDE);
  return m_pBoundaryInterfaceDef->InterfaceMaterialSides(iface);
}

void CInterfaceBoundary::LoadProperties(TSTREAM& stream, CStreamVersion& version, TPROGRESS& progress)
{
	int temp;
	stream >> temp;
	m_bCreateInterfaces = (temp != 0);

  m_pBoundaryInterfaceDef->LoadStream(stream, version, progress);
}

void CInterfaceBoundary::CreateInterfaceElement2BoundarySurfaceMap() const
{
  assert(m_pInterfaceElements);
  if(!m_pInterfaceElements)
    return;

  TFace2BoundarySurfaceMap mpFace2BoundarySurface;

  int i;
  for(i = 0; i < GetTopHorizon().BodyFaceSize(); ++i)
  {
    const geo::IFace& face = GetTopHorizon().BodyFace(i);
    mpFace2BoundarySurface.insert(TFace2BoundarySurfaceMap::value_type(&face, CBoundaryInterfaceDef::BSURF_TOP));
  }

  for(i = 0; i < GetBottomHorizon().BodyFaceSize(); ++i)
  {
    const geo::IFace& face = GetBottomHorizon().BodyFace(i);
    mpFace2BoundarySurface.insert(TFace2BoundarySurfaceMap::value_type(&face, CBoundaryInterfaceDef::BSURF_BOTTOM));
  }
/*
  const CModelBase& model = static_cast<const CModelBase&>(Model());
  const CTetraMesh& tetmesh = static_cast<const CTetraMesh&>(model.Mesh());

  for(i = 0; i < tetmesh.InputSurfaceSize(); ++i)
  {
    CTetraMesh::TInputSurface is = tetmesh.InputSurface(i);
    if(is.second == this) // side surface
    {
      const geo::CSurfaceDesc& surfdesc = *is.first;
      int j;
      for(j = 0; j < surfdesc.TetSurfaceSize(); ++j)
      {
        const geo::CTetSurface& tetsurf = surfdesc.TetSurface(j);
        int k;
        for(k = 0; k < tetsurf.FaceSize(); ++k)
        {
          const geo::IFace& face = tetsurf.Face(k);
          mpFace2BoundarySurface.insert(TFace2BoundarySurfaceMap::value_type(&face, CBoundaryInterfaceDef::BSURF_SIDE));
        }
      }
    }
  }
*/
  for(i = 0; i < m_pInterfaceElements->ElementSize(); ++i)
  {
    assert(dynamic_cast<const geo::CInterfaceElement*>(&m_pInterfaceElements->Element(i)));
    const geo::CInterfaceElement& iface = static_cast<const geo::CInterfaceElement&>(m_pInterfaceElements->Element(i));
    assert(iface.FrontFace() == iface.BackFace());
    TFace2BoundarySurfaceMap::iterator it = mpFace2BoundarySurface.find(iface.FrontFace());
    CBoundaryInterfaceDef::TBoundarySurface bsurf = CBoundaryInterfaceDef::BSURF_SIDE;
    if(it != mpFace2BoundarySurface.end())
      bsurf = it->second;

    m_mpInterfaceElement2BoundarySurface.insert(TInterfaceElement2BoundarySurfaceMap::value_type(&iface, bsurf));
  }
}

int CInterfaceBoundary::InterfaceNodeSize() const
{
  if(!m_pInterfaceElements)
    return 0;	// There are no nodes

  if(m_vcInterfaceNode.empty())
  {
    std::set<int> stNode;
    for(int i = 0; i < m_pInterfaceElements->ElementSize(); i++)
    {
	    const geo::IElement& element = m_pInterfaceElements->Element(i);
      int nNodes = element.NrOfNodes();
	    for(int j = nNodes / 2; j < nNodes; j++)
      {
	      if(stNode.insert(element.Node(j).Index()).second)
		      m_vcInterfaceNode.push_back(&element.Node(j));
	    }
    }
  }

  return m_vcInterfaceNode.size();
}

void CInterfaceBoundary::AddInterfaceElement(geo::CInterfaceElement& interface_element)
{
  m_vcInterfaceNode.clear();
  m_mpInterfaceElement2BoundarySurface.clear();
  if(!m_pInterfaceElements)
  {
	  CModelBase &model = dynamic_cast<CModelBase&>(Model());
	  assert(&model);
    m_pInterfaceElements = new geo::CElementGroup(model.Mesh().Mesh());
  }

  m_pInterfaceElements->AddMeshElement( interface_element );
}

const geo::CElementGroup* CInterfaceBoundary::InterfaceElements() const
{
	return m_pInterfaceElements;
}

const geo::INode& CInterfaceBoundary::InterfaceNode(int nIndex) const
{
	assert(m_pInterfaceElements);
	assert(InterfaceNodeSize());
  return *m_vcInterfaceNode[nIndex];
}

void CInterfaceBoundary::CreateChildren()
{
	m_pDisplacementSupportNode = new CDisplacementSupportNode(*this);
	m_pPressureSupportNode = new CPressureSupportNode(*this);
}
