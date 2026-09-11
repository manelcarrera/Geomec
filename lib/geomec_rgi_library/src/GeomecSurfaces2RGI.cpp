
#include <stdexcept>

#include "GeomecSurfaces2RGI.h"
#include "ModelBase.h"
#include "BaseEntryTypes.h"
#include "FormationBase.h"
#include "HorizonBase.h"
#include "TetraModel.h"
#include "TetraBoundary.h"
#include "SurfaceDesc.h"
#include "RockMechProcessor.h"
#include "RGInterface.h"
#include "GeoSurface.h"
#include "ElementGroup.h"
#include "IInterfaceElement.h"
#include "TetSurface.h"
#include "DCasingDefines.h"
#include "HexaModel.h"
#include "HexaBoundary.h"
#include "HexaEntryTypes.h"
#include "HexaFormation.h"

#include <algorithm>


namespace GeomecRGI
{

CGeomecSurfaces2RGI::CGeomecSurfaces2RGI(RGInterface& rgi,
  const CRockMechProcessor& rmp, const CModelBase& modelBase)
: m_rgi(rgi)
, m_rmp(rmp)
, m_modelBase(modelBase)
{
}

CGeomecSurfaces2RGI::~CGeomecSurfaces2RGI()
{
}

namespace
{

const QString MODEL_NOT_SUPPORTED =
  "The saving of surface data is only supported for hexahedron and tetrahedron models";

} // anonymous namespace

void CGeomecSurfaces2RGI::saveSurfaces()
{
  if (dynamic_cast<const CTetraModel *>(&m_modelBase) || dynamic_cast<const CHexaModel *>(&m_modelBase))
  {
  saveFaults();
  saveHorizons();
  saveSideSurfaces();
  }
  else
  {
  m_rmp.AddLogLine(MODEL_NOT_SUPPORTED);
  }
}

// private

namespace
{

const QString CASTING_ERROR = "Caught casting error '%1'";

template <typename S>
  void createSurface(RGSurface& rgSurface, RGInterface& rgi, S& surface)
{
  try
  {
  for (int e = 0; e < surface.ElementSize(); ++e)
  {
      const geo::IElement& element = surface.Element(e);
      RGPolygon rgPolygon;

      for (int n = 0; n < element.NrOfNodes(); ++n)
      {
    const geo::INode& node = element.Node(n);
    RGNode rgNode(node.X(), node.Y(), node.Z());
    RGNodeId rgNodeId = rgi.addNode(rgNode);

    rgPolygon.push_back(rgNodeId);
      }

      RGPolygonId rgPolygonId = rgi.addPolygon(rgPolygon);

      rgSurface.addPolygon(rgPolygonId);
  }
  }

  catch (const std::bad_cast& b)
  {
  QString l = CASTING_ERROR.arg(b.what());

  throw std::runtime_error(l.toStdString());
  }
}

class CSurfaceWrapper
{
  public:
  enum Side
  {
      FRONT = 1,
      BACK
  };

  CSurfaceWrapper(const geo::CElementGroup* elementGroup, Side side);

  int ElementSize() const;
  const geo::IElement& Element(int index) const;

  private:
  CSurfaceWrapper(const CSurfaceWrapper& rhs);
  CSurfaceWrapper& operator = (const CSurfaceWrapper& rhs);

  const geo::CElementGroup* m_elementGroup;
  Side m_side;
};

CSurfaceWrapper::CSurfaceWrapper(const geo::CElementGroup* elementGroup,
  Side side)
: m_elementGroup(elementGroup)
, m_side(side)
{
}

int CSurfaceWrapper::ElementSize() const
{
  return m_elementGroup->ElementSize();
}

const geo::IElement& CSurfaceWrapper::Element(int index) const
{
  const geo::IInterfaceElement& interfaceElement =
  dynamic_cast <const geo::IInterfaceElement&> (
      m_elementGroup->Element(index));

  if (m_side == FRONT)
  {
  return interfaceElement.Front();
  }
  else  // (m_side == BACK)
  {
  return interfaceElement.Back();
  }
}

const std::string SPACE = " ";
const std::string SURFACE_BACK = "back";
const std::string SURFACE_FRONT = "front";

} // anonymous namespace

std::vector <int> CGeomecSurfaces2RGI::retrieveSurfaceNodeIndices(
  const geo::ISurface& surface) const
{
  std::vector <int> surfaceIndices;

  for (int e = 0; e < surface.ElementSize(); ++e)
  {
  const geo::IElement& element = surface.Element(e);

  for (int n = 0; n < element.NrOfNodes(); ++n)
  {
      surfaceIndices.push_back(element.Node(n).Index());
  }
  }

  std::sort(surfaceIndices.begin(), surfaceIndices.end());

  return surfaceIndices;
}

void CGeomecSurfaces2RGI::saveFaults()
{

  if (dynamic_cast<const CTetraBoundary *>(&m_modelBase.Boundary()))
  {

  const THorizonBaseEntry* horizonBaseEntry = dynamic_cast <const THorizonBaseEntry*>(m_modelBase.GraphEntry(MD_BASE_HORIZON));
  THorizonBaseEntry::TSortedNodeSet stNodes = horizonBaseEntry->SortedEntryNodes();

  for (THorizonBaseEntry::TSortedNodeSet::const_iterator horizon = stNodes.begin(); horizon != stNodes.end(); ++horizon)
  {
      if ((*horizon)->Slip())
      {
    const geo::CElementGroup* elementGroup = (*horizon)->InterfaceElementGroup();

    {
          RGSurface rgSurface((*horizon)->Name().toStdString() + SPACE +
      SURFACE_FRONT, RGSurface::FaultFront, RGSurface::Regular);
          CSurfaceWrapper surfaceWrapper(elementGroup, CSurfaceWrapper::FRONT);

          createSurface(rgSurface, m_rgi, surfaceWrapper);

          m_rgi.addHorizon(rgSurface);
    }

    {
          RGSurface rgSurface((*horizon)->Name().toStdString() + SPACE +
      SURFACE_BACK, RGSurface::FaultBack, RGSurface::Regular);
          CSurfaceWrapper surfaceWrapper(elementGroup, CSurfaceWrapper::BACK);

          createSurface(rgSurface, m_rgi, surfaceWrapper);

          m_rgi.addHorizon(rgSurface);
    }
      }
  }
  }
  else
  {
  assert(dynamic_cast<const CHexaModel *>(&m_modelBase));
  assert(dynamic_cast<const CHexaBoundary*>(&(m_modelBase.Boundary())) != 0);

  const CHexaBoundary& hexaBoundary = static_cast<const CHexaBoundary&>(m_modelBase.Boundary());

  const CHexaHorizon& topHorizon = hexaBoundary.GetTopHorizon();
  const CHexaHorizon& botHorizon = hexaBoundary.GetBottomHorizon();

  const CHexaFormationEntry* hexaEntry = dynamic_cast<const CHexaFormationEntry *>(m_modelBase.GraphEntry(MD_HEXA_FORMATION));
  if (hexaEntry)
  {
      CHexaFormationEntry::TNodeSet stNodes = hexaEntry->EntryNodes();

      //std::vector<CHexaFormation *> formations;
      //formations.reserve(stNodes.size());

      CHexaFormation *topFormation = 0;
      CHexaFormation *botFormation = 0;

      for (CHexaFormationEntry::TNodeSet::iterator it = stNodes.begin(); it != stNodes.end(); ++it)
      {
    if ((*it)->UpperHorizon() == topHorizon)
          topFormation = *it;

    if ((*it)->LowerHorizon() == botHorizon)
          botFormation = *it;

    //if ((*it) != topFormation)
    //  formations.push_back(*it);
      }

      const CHexaFormation *formation = topFormation->LowerFormation();

      while (formation)
      {
    const CHexaHorizon& horizon = formation->UpperHorizon();

    if (horizon.Slip())
    {
          const geo::CElementGroup* elementGroup = horizon.InterfaceElementGroup();

          {
      RGSurface rgSurface(horizon.Name().toStdString() + SPACE +
              SURFACE_FRONT, RGSurface::FaultFront, RGSurface::Regular);
      CSurfaceWrapper surfaceWrapper(elementGroup, CSurfaceWrapper::FRONT);

      createSurface(rgSurface, m_rgi, surfaceWrapper);

      m_rgi.addHorizon(rgSurface);
          }

          {
      RGSurface rgSurface(horizon.Name().toStdString() + SPACE +
              SURFACE_BACK, RGSurface::FaultBack, RGSurface::Regular);
      CSurfaceWrapper surfaceWrapper(elementGroup, CSurfaceWrapper::BACK);

      createSurface(rgSurface, m_rgi, surfaceWrapper);

      m_rgi.addHorizon(rgSurface);
          }
    }
    formation = formation->LowerFormation();
      }
  }
  }
}

void CGeomecSurfaces2RGI::saveHorizon(const C3DHorizon* horizon,
  RGSurface::Type type, RGSurface::Attribute attribute)
{
  static std::vector <std::vector <int> > savedMeshedSurfaces;

  for (int s = 0; s < horizon->MeshedSurfaceSize(); ++s)
  {
  const geo::ISurface& surface = horizon->MeshedSurface(s);
  std::vector <int> meshedSurfaceNodeIndices =
      retrieveSurfaceNodeIndices(surface);

  if (std::find(savedMeshedSurfaces.begin(), savedMeshedSurfaces.end(),
      meshedSurfaceNodeIndices) == savedMeshedSurfaces.end())
  {
      RGSurface rgSurface(horizon->Name().toStdString(), type, attribute);

      createSurface(rgSurface, m_rgi, surface);

      m_rgi.addHorizon(rgSurface);
      savedMeshedSurfaces.push_back(meshedSurfaceNodeIndices);
  }
  }
}


void CGeomecSurfaces2RGI::saveHorizon(const CHexaHorizon *horizon, const CFormationPlane *plane, RGSurface::Type type, RGSurface::Attribute attribute)
{
  RGSurface rgSurface(horizon->Name().toStdString(), type, attribute);

  createSurface(rgSurface, m_rgi, plane->PlaneSurface());

  m_rgi.addHorizon(rgSurface);
}



void CGeomecSurfaces2RGI::saveHorizons()
{
  if (dynamic_cast<const CTetraBoundary *>(&m_modelBase.Boundary()))
  {
  const CTetraBoundary& tetraBoundary =
      dynamic_cast <const CTetraBoundary&> (m_modelBase.Boundary());
  const CTetraHorizonBase* topHorizon = tetraBoundary.TopHorizon();
  const CTetraHorizonBase* bottomHorizon = tetraBoundary.BottomHorizon();
  const THorizonBaseEntry* horizonBaseEntry =
      dynamic_cast <const THorizonBaseEntry*> (
      m_modelBase.GraphEntry(MD_BASE_HORIZON));

  saveHorizon(topHorizon, RGSurface::Horizon, RGSurface::Top);
  saveHorizon(bottomHorizon, RGSurface::Horizon, RGSurface::Bottom);

  THorizonBaseEntry::TSortedNodeSet stNodes = horizonBaseEntry->SortedEntryNodes();

  for (THorizonBaseEntry::TNodeSet::const_iterator horizon = stNodes.begin(); horizon != stNodes.end(); ++horizon)
  {
      if ((*horizon != topHorizon) && (*horizon != bottomHorizon) &&
    !(*horizon)->Slip())
      {
    saveHorizon(dynamic_cast <const C3DHorizon*> (*horizon),
          RGSurface::Horizon, RGSurface::Regular);
      }
  }
  }
  else
  {
  assert(dynamic_cast<const CHexaModel *>(&m_modelBase));
  assert(dynamic_cast<const CHexaBoundary*>(&(m_modelBase.Boundary())) != 0);

  const CHexaBoundary& hexaBoundary = static_cast<const CHexaBoundary&>(m_modelBase.Boundary());

  const CHexaHorizon& topHorizon = hexaBoundary.GetTopHorizon();
  const CHexaHorizon& botHorizon = hexaBoundary.GetBottomHorizon();

  const CHexaFormationEntry* hexaEntry = dynamic_cast<const CHexaFormationEntry *>(m_modelBase.GraphEntry(MD_HEXA_FORMATION));
  if (hexaEntry)
  {
      CHexaFormationEntry::TNodeSet stNodes = hexaEntry->EntryNodes();

      //std::vector<CHexaFormation *> formations;
      //formations.reserve(stNodes.size());

      CHexaFormation *topFormation = 0;
      CHexaFormation *botFormation = 0;

      for (CHexaFormationEntry::TNodeSet::iterator it = stNodes.begin(); it != stNodes.end(); ++it)
      {
    if ((*it)->UpperHorizon() == topHorizon)
          topFormation = *it;
    
    if ((*it)->LowerHorizon() == botHorizon)
          botFormation = *it;

    //if ((*it) != topFormation)
    //  formations.push_back(*it);
      }

      saveHorizon(&topFormation->UpperHorizon(), topFormation->FormationPlane(CFormationPlane::FP_TOP).data(), RGSurface::Horizon, RGSurface::Top);
      saveHorizon(&botFormation->LowerHorizon(), botFormation->FormationPlane(CFormationPlane::FP_BOTTOM).data(), RGSurface::Horizon, RGSurface::Bottom);

      const CHexaFormation *formation = topFormation->LowerFormation();

      while (formation)
      {
    if (!formation->UpperHorizon().Slip())
    {
          saveHorizon(&formation->UpperHorizon(), formation->FormationPlane(CFormationPlane::FP_TOP).data(), RGSurface::Horizon, RGSurface::Regular);
    }
    formation = formation->LowerFormation();
      }
  }
  }
}

void CGeomecSurfaces2RGI::saveSideSurfaces()
{
  if (dynamic_cast<const CTetraBoundary *>(&m_modelBase.Boundary()))
  {
  const CTetraBoundary& tetraBoundary =
      dynamic_cast <const CTetraBoundary&> (m_modelBase.Boundary());

  for (int s = 0; s < tetraBoundary.SideSurfaceSize(); ++s)
  {
      const geo::CSurfaceDesc& surfaceDesc = tetraBoundary.SideSurfaceDesc(s);

      for (int t = 0; t < surfaceDesc.TetSurfaceSize(); ++t)
      {
    const geo::CTetSurface& surface = surfaceDesc.TetSurface(t);
    std::string surfaceName = (surfaceDesc.TetSurfaceSize() > 1) ?
          surfaceDesc.Name() + SPACE + lexical_cast <std::string, int> (t) :
          surfaceDesc.Name();
    RGSurface rgSurface(surfaceName, RGSurface::Horizon, RGSurface::Side);

    createSurface(rgSurface, m_rgi, surface);

    m_rgi.addHorizon(rgSurface);
      }
  }
  }
  else
  {
  assert(dynamic_cast <const CHexaBoundary*> (&(m_modelBase.Boundary())) != 0);

  const CHexaBoundary& hexaBoundary = static_cast<const CHexaBoundary&>(m_modelBase.Boundary());

  std::vector<const geo::ISurface*> surfaces = hexaBoundary.GetSideMeshSurfaces();

  for (int s = 0; s < surfaces.size(); ++s)
  {
      const geo::ISurface *surface = surfaces[s];

      RGSurface rgSurface("Side surface " + lexical_cast <std::string, int> (s), RGSurface::Horizon, RGSurface::Side);

      createSurface(rgSurface, m_rgi, *surface);

      m_rgi.addHorizon(rgSurface);
  }
  }
}

} // namespace GeomecRGI
