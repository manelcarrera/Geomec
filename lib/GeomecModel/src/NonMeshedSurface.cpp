#include "NonMeshedSurface.h"

#include "GeoSurface.h"
#include "StreamVersion.h"
#include "ModelBase.h"
#include "BaseEntryTypes.h"
#include "NonMeshedSurfaceParametersNode.h"
#include "NonMeshedSurfacePressure.h"

CNonMeshedSurface::CGeoSurface::CGeoSurface(const geo::CTriSurface& surface, const CNonMeshedSurface& parent)
: m_vcTriangle(surface.FaceSize()),
  m_vcPoint(surface.PointSize()),
  m_parent(parent)
{
  // indexing of elements and points is equal to the source surface...

  for(int i = 0; i < surface.PointSize(); ++i)
  m_vcPoint[i] = surface.Point(i);

  for(int i = 0; i < surface.FaceSize(); ++i)
  {
  const geo::IFace& face = surface.Face(i);
  std::vector<int> vcNodes = surface.Nodes(face);
  assert(vcNodes.size() == 3);

  m_vcTriangle[i] = new geo::CTriangle(*this, i, vcNodes[0], vcNodes[1], vcNodes[2]);

  for(int j = 0; j < 3; ++j)
  {
      TPointToFacesMap::iterator it =
    m_mpPointToFaces.insert(TPointToFacesMap::value_type(&m_vcPoint[vcNodes[j]], geo::CPtrArray<geo::IFace>())).first;
      it->second.PushBack(*m_vcTriangle[i]);
  }
  }
}

CNonMeshedSurface::CGeoSurface::~CGeoSurface()
{
}

const geo::CPtrArray<geo::IFace> CNonMeshedSurface::CGeoSurface::FacesAt(const geo::IPoint &/*p*/) const
{
  assert(false); // not implemented...
  return geo::CPtrArray<geo::IFace>();
}

const geo::CPtrArray<geo::IFace> CNonMeshedSurface::CGeoSurface::FacesAtNode(const geo::IPoint &p) const
{
  TPointToFacesMap::const_iterator it = m_mpPointToFaces.find(&p);
  assert(it != m_mpPointToFaces.end());
  return it->second;
}

const geo::CTriangle &CNonMeshedSurface::CGeoSurface::Face(int nIndex) const
{
  return *m_vcTriangle[nIndex];
}

int CNonMeshedSurface::CGeoSurface::FaceSize() const
{
  return m_vcTriangle.size();
}

const geo::CPoint& CNonMeshedSurface::CGeoSurface::Point(int nIndex) const
{
  return m_vcPoint[nIndex];
}

void CNonMeshedSurface::CGeoSurface::Clear()
{
  m_vcPoint.clear();
  m_vcTriangle.clear();
  m_mpPointToFaces.clear();
}

int CNonMeshedSurface::CGeoSurface::PointSize() const
{
  return m_vcPoint.size();
}

std::vector<int> CNonMeshedSurface::CGeoSurface::Nodes(const geo::IElement &element) const
{
  assert(element.IndexingElementSet() == this);
  std::vector<int> vcRet(element.NrOfPoints());
  for(int i = 0; i < element.NrOfPoints(); ++i)
  vcRet[i] = element.PointIndex(i);
  return vcRet;
}

const CNonMeshedSurface& CNonMeshedSurface::CGeoSurface::Parent() const
{
  return m_parent;
}

geo::CPoint &CNonMeshedSurface::CGeoSurface::PointAt(int nIndex)
{
  return m_vcPoint[nIndex];
}


///////

CNonMeshedSurface::CNonMeshedSurface(CFemAppModel& model)
: CColorNode(model),
  m_pSurface(0),
  m_pGeoSurface(0),
  m_pParametersNode(0),
  m_pPressure(0)
{
}

CNonMeshedSurface::CNonMeshedSurface(CSurfaceBase& surface)
: CColorNode(surface.Name(), surface.Model(), surface.Color()),
  m_pSurface(0),
  m_pGeoSurface(0),
  m_pParametersNode(0),
  m_pPressure(0)
{
  LinkTo(surface);
  new CNonMeshedSurfaceParametersNode(*this);
  new CNonMeshedSurfacePressure(*this);
  reParent(surface.Model().GraphEntry(MD_BASE_NONMESHEDSURFACE));
}

CNonMeshedSurface::~CNonMeshedSurface()
{
  if(!IsCopy())
  delete m_pGeoSurface;
}

unsigned int CNonMeshedSurface::IconId() const
{
  assert(m_pSurface);
  return m_pSurface->IconId();
}

unsigned int CNonMeshedSurface::TypeId() const
{
  assert(m_pSurface);
  return m_pSurface->TypeId();
}

QString CNonMeshedSurface::TypeName() const
{
  return "Non-meshed surfaces";
}

int CNonMeshedSurface::DisplayListSize() const
{
  if(m_pSurface)
  return 1;

  return 0;
}

const geo::IObject& CNonMeshedSurface::DisplayList(int /*nIndex*/) const
{
  assert(m_pSurface);
  if(!m_pGeoSurface)
  {
  assert(dynamic_cast<const geo::CTriSurface*>(&m_pSurface->Surface()));
  m_pGeoSurface = new CGeoSurface(static_cast<const geo::CTriSurface&>(m_pSurface->Surface()), *this);
  }

  return *m_pGeoSurface;
}

long CNonMeshedSurface::SavedItems() const
{
  return 1;
}

void CNonMeshedSurface::LoadStream(TSTREAM& stream, CStreamVersion& version, TPROGRESS& progress)
{
  int nIndex;
  stream >> nIndex;
  if(nIndex >= 0)
  {
  CSurfaceEntry& entry = (CSurfaceEntry&)*Model().GraphEntry(MD_BASE_SURFACE);
  entry.LinkNodeToIndex(*this, nIndex);

  new CNonMeshedSurfaceParametersNode(*this);
  assert(m_pParametersNode);
  m_pParametersNode->LoadStream(stream, version, progress);

  new CNonMeshedSurfacePressure(*this);
  assert(m_pPressure);
  m_pPressure->LoadStream(stream, version, progress);

  reParent(Model().GraphEntry(MD_BASE_NONMESHEDSURFACE));
  }

  progress.Step();

  CColorNode::LoadStream(stream, version, progress);
}

void CNonMeshedSurface::SaveStream(TSTREAM& stream, TPROGRESS& progress)
{
  stream << (m_pSurface ? m_pSurface->Index() : -1);
  if(m_pSurface)
  {
  m_pParametersNode->SaveStream(stream, progress);
  m_pPressure->SaveStream(stream, progress);
  }
  progress.Step();

  CColorNode::SaveStream(stream, progress);
}

const CNonMeshedSurfaceParametersNode& CNonMeshedSurface::ParametersNode() const
{
  assert(m_pParametersNode);
  return *m_pParametersNode;
}

const CNonMeshedSurfacePressure& CNonMeshedSurface::Pressure() const
{
  assert(m_pPressure);
  return *m_pPressure;
}

int CNonMeshedSurface::FaceSize() const
{
  if(m_pSurface)
  return m_pSurface->Surface().FaceSize();

  return 0;
}

const geo::IFace& CNonMeshedSurface::Face(int nIndex) const
{
  assert(m_pSurface);
  return m_pSurface->Surface().Face(nIndex);
}

void CNonMeshedSurface::OnNewNeighbour(const CGraphNode& node)
{
  const CSurfaceBase* pSurface = dynamic_cast<const CSurfaceBase*>(&node);
  if(pSurface)
  {
  assert(!m_pSurface);
  assert(!m_pGeoSurface);
  m_pSurface = pSurface;
  Modified();
  }

  const CNonMeshedSurfaceParametersNode* pParametersNode = dynamic_cast<const CNonMeshedSurfaceParametersNode*>(&node);
  if(pParametersNode)
  m_pParametersNode = const_cast<CNonMeshedSurfaceParametersNode*>(pParametersNode);

  const CNonMeshedSurfacePressure* pPressure = dynamic_cast<const CNonMeshedSurfacePressure*>(&node);
  if(pPressure)
  m_pPressure = const_cast<CNonMeshedSurfacePressure*>(pPressure);
}

void CNonMeshedSurface::OnNeighbourModified(const CGraphNode& node, enum ModifiedHint uHint)
{
  if(&node == m_pSurface)
  Modified();

  if(&node == m_pParametersNode)
  Modified();

  if(&node == m_pPressure)
  Modified();

  CColorNode::OnNeighbourModified(node, uHint);
}

void CNonMeshedSurface::OnNeighbourDeleted(const CGraphNode& node)
{
  if(&node == m_pSurface)
  {
  delete this;
  return;
  }

  if(&node == m_pParametersNode)
  m_pParametersNode = 0;

  if(&node == m_pPressure)
  m_pPressure = 0;

  CColorNode::OnNeighbourDeleted(node);
}


////////

CNonMeshedSurfaceEntry::CNonMeshedSurfaceEntry(CFemAppModel& model)
: CStorageNodeEntry<CNonMeshedSurface>::CStorageNodeEntry(MD_BASE_NONMESHEDSURFACE,
                                                          model.GraphEntry(MD_BASE_SURFACE)->IconId(),
                                                          "Non-meshed surfaces",
                                                          model)
{
}

bool CNonMeshedSurfaceEntry::ConnectItem(const CGraphNode& item)
{
  if(CanConnectItem(item))
  {
  const CSurfaceBase& surface = static_cast<const CSurfaceBase&>(item);
  new CNonMeshedSurface(const_cast<CSurfaceBase&>(surface));
  return true;
  }

  return false;
}

bool CNonMeshedSurfaceEntry::CanConnectItem(const CGraphNode& item) const
{
  return (dynamic_cast<const CSurfaceBase*>(&item) != 0);
}
