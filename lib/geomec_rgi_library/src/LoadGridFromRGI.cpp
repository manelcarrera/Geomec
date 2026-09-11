#include <stdexcept>
#include <limits>
#include <iomanip>

#include "LoadGridFromRGI.h"
#include "ModelBase.h"
#include "MeshBase.h"
#include "TetMeshBase.h"
#include "TetraSubBoundary.h"
#include "TetraModel.h"
#include "MeshSurface.h"
#include "TetraSurface.h"
#include "TetraSubHorizon.h"
#include "TetraMesh.h"
#include "CalculateBoundingSurfaces.h"
#include "TetraFormation.h"
#include "TetSurface.h"
#include "RGInterface.h"
#include "ISupport.h"
#include "Tetrahedron.h"
#include "GeoSurface.h"
#include "Surface.h"
#include "SurfaceDesc.h"
#include "RockMechProcessor.h"
#include "propertyValues.h"
#include "InterfaceElement.h"

namespace
{

void orderPolygon(const GeomecRGI::CLoadGridFromRGI::TIndices& input,
  GeomecRGI::CLoadGridFromRGI::TIndices& output)
{
  size_t pivot = 0;
  for (size_t i = 1; i < input.size(); ++i)
  if (input[i] < input[pivot])
      pivot = i;

  output.resize(input.size());

  for (size_t i = 0; i < input.size(); ++i)
  output[i] = input[(i + pivot) % input.size()];
}

} // anonymous namespace

namespace GeomecRGI
{

CRGIGridDump::CRGIGridDump(bool bDoDump) : valid(false)
{
  if (bDoDump)
  {
  m_output.open("grid_dump.txt", std::ios::out | std::ios::trunc | std::ios::binary);

  if (m_output.is_open())
  {
      valid = true;

      m_output << std::setprecision(6);

      m_output << ">>> START Grid Load" << std::endl;
  }
  }
}

CRGIGridDump::~CRGIGridDump()
{
  if (valid)
  {
  m_output << "<<< END Grid Load" << std::endl;
  m_output.close();
  }
}

void CRGIGridDump::section(const char *title)
{
  if (valid)
  m_output << title << std::endl;
}

void CRGIGridDump::size(size_t s)
{
  if (valid)
  m_output << "size: " << s << std::endl;
}

void CRGIGridDump::node(size_t index, geo::CPoint& point)
{
  if (valid)
  {
  m_output << "node " << index << ": " << point.X() << " " << point.Y() << " " << point.Z() << std::endl;
  std::vector<int> p(3);
  p[0] = (int)point.X();
  p[1] = (int)point.Y();
  p[2] = (int)point.Z();
  m_points.push_back(p);
  }
}

void CRGIGridDump::_nodes(std::vector<int> &nodes)
{
  if (valid)
  {
  m_output << "nodes[" << nodes.size() << "](";
  if (nodes.size() > 0)
  {
      m_output << nodes[0];
      for (size_t i = 1; i < nodes.size(); ++i)
    m_output << ", " << nodes[i];
  }
  m_output << ")";
  }
}

void CRGIGridDump::element(size_t index, std::vector<int> &nodes)
{
  if (valid)
  {
  m_output << "element " << index << ": " << index << " " << (nodes.size() == 4 ? "TE12L" : "T18IF") << " ";
  _nodes(nodes);
  m_output << std::endl;
  }
}

void CRGIGridDump::polygon(size_t index, std::vector<int> &nodes)
{
  if (valid)
  {
  m_output << "polygon " << index << ": " << index << " ";
  _nodes(nodes);

  m_output << " ->  ";

  for (size_t i = 0; i < 3; ++i)
      m_output << m_points[nodes[i]][0] << "," << m_points[nodes[i]][1] << "," << m_points[nodes[i]][2] << "  ";

  m_output << std::endl;
  }
}

void CRGIGridDump::surface(const RGSurface& surface)
{
  if (valid)
  {
  m_output << "surface " << surface.getName() << " [";
  switch(surface.getType())
  {
  case RGSurface::Horizon:
      m_output << "Horizon";
      break;
  case RGSurface::FaultFront:
      m_output << "FaultFront";
      break;
  case RGSurface::FaultBack:
      m_output << "FaultBack";
      break;
  default:
      m_output << "Undefined " << surface.getType() << "";
      break;
  }
  m_output << ",";
  switch(surface.getAttribute())
  {
  case RGSurface::Regular:
      m_output << "Regular";
      break;
  case RGSurface::Top:
      m_output << "Top";
      break;
  case RGSurface::Bottom:
      m_output << "Bottom";
      break;
  case RGSurface::Side:
      m_output << "Side";
      break;
  default:
      m_output << "Undefined " << surface.getAttribute();
      break;
  }
  m_output << "]:" << std::endl;
  for (size_t i = 0; i < surface.getSize(); ++i)
  {
      m_output << "    polygon " << surface.getPolygonN(i) << std::endl;
  }
  }
}



CLoadGridFromRGI::PolygonInfo::PolygonInfo() : face(0), ifElement(-1)
{
}

void CLoadGridFromRGI::PolygonInfo::setPolygon(TIndices &polygon)
{
  orderPolygon(polygon, nodes);
}



CLoadGridFromRGI::CLoadGridFromRGI(CModelBase& model, RGInterface& rgi,
  const CRockMechProcessor& rmp)
: m_model(model)
, m_mesh(static_cast <geo::CTetMeshBase&> (m_model.Mesh().Mesh()))
, m_rgi(rgi)
, m_rmp(rmp)
, m_geomecFormations()
, m_dump(getenv("RGIDDUMP") != 0)
{
}

bool CLoadGridFromRGI::operator () ()
{
  m_model.Mesh().Lock();

  // For loading the data we follow the procedure out-lined in
  // CGocadMeshImporter::Import().

  // Nodes, nodeIndices[RGI node index] = Geomec node index

  TIndices nodeIndices;
  insertNodes(nodeIndices);

  // Polygons, polygonInfo[RGI polygon index] = information about which faces map to this polygon

  TPolygonInfo polygonInfo;
  TPolygonMap polygonMap;

  insertPolygons(polygonInfo, polygonMap);

  // Elements, elementIndices[RGI element index] = Geomec element index

  TIndices elementIndices;
  insertElements(elementIndices, nodeIndices, polygonInfo, polygonMap);

  // Formations

  std::vector <QString> formationNames;
  std::map <int, int> formationVolume2VolumeMapping;

  constructFormations(formationNames, formationVolume2VolumeMapping,
  elementIndices);

  std::vector <TSharedBodyTriangleMap>
  trianglesPerVolume = getTrianglesPerVolume();
  TNodes2Triangles nodes2Triangles =
  createNodes2TrianglesMapping(trianglesPerVolume);

  /*
   *  While creating horizons and the enclosing surfaces the surface
   *  descriptors (for the horizons and surfaces) are
   *  also created. To later be able to map any surface to the proper surface
   *  descriptor we assemble a map linking faces with their surface descriptor.
   */

  TFaceDescriptorMap linkFace2Horizon;
  TFaceDescriptorMap linkFace2SurfaceDescriptor;

  // Faults

  constructFaults(linkFace2Horizon, linkFace2SurfaceDescriptor,
  nodes2Triangles, nodeIndices, polygonInfo);

  // Horizons

  constructHorizons(linkFace2Horizon, linkFace2SurfaceDescriptor,
  nodes2Triangles, nodeIndices);

  // Side Surfaces

  constructSideSurfaces(linkFace2SurfaceDescriptor, nodes2Triangles,
  nodeIndices);

  // Volumes

  constructFormationVolumes(linkFace2Horizon, linkFace2SurfaceDescriptor,
  formationVolume2VolumeMapping, trianglesPerVolume, formationNames);

  m_model.Mesh().Unlock();

  return true;
}

std::vector <CFormationBase*> CLoadGridFromRGI::getGeomecFormations() const
{
  return m_geomecFormations;
}

// private

void CLoadGridFromRGI::insertNodes(TIndices& nodeIndices)
{
  size_t nodes = m_rgi.getNumNodes();
  nodeIndices.resize(nodes);

  m_dump.section("Nodes");
  m_dump.size(nodes);

  for (size_t node = 0; node < nodes; ++node)
  {
  const RGNode& rgNode = m_rgi.getNodeN(node);
  geo::CPoint point(rgNode[0], rgNode[1], rgNode[2]);

  nodeIndices[node] = m_mesh.RegisterNodeInMainMesh(point);

  m_dump.node(node, point);
  }
}

void CLoadGridFromRGI::insertPolygons(TPolygonInfo& polygonInfo, TPolygonMap& polygonMap)
{
  m_dump.section("Polygons");
  m_dump.size(m_rgi.getNumPolygons());

  polygonInfo.resize(m_rgi.getNumPolygons());

  for (size_t i = 0; i < m_rgi.getNumPolygons(); ++i)
  {
  const RGPolygon rgPolygon = m_rgi.getPolygonN(i);

  TIndices polygon(rgPolygon.size());

  for (size_t j = 0; j < rgPolygon.size(); ++j)
      polygon[j] = rgPolygon[j];

  polygonInfo[i].setPolygon(polygon);

  polygonMap[polygonInfo[i].nodes] = i;

  m_dump.polygon(i, polygon);
  }

}


namespace
{

const QString ONLY_TE12L_OR_T18IF =
  "Only elements of type 'typeTE12L' or 'typeT18IF' are allowed (and can be processed)!";
const QString NO_DEGENERATES = "Element %1 refers to node %2 more than once,"
  " degenerate elements are not allowed";
const QString INVALID_FORMATION_COUNT = "The number of loaded formation ID's "
  "(%1, loaded with loadProperty(...)) does not equal the number of retrieved"
  " elements (%2)!";
const QString INVALID_FORMATION_VOLUME_COUNT = "The number of loaded formation "
  "volume ID's (%1, loaded with loadProperty(...)) does not equal the number "
  "of retrieved elements (%2)!";
const QString PROPERTY_BODY_INDEX = "The values of property 'propBodyIndex' "
  "are (probably) not defined, using the values from property "
  "'propElementFormation' instead.";

CLoadGridFromRGI::TIndices insertElementTE12L(const RGInterface& rgi,
  const CLoadGridFromRGI::TIndices& nodeIndices, const RGElement& rgElement,
  size_t element)
{
  const size_t nodesInElement = rgElement.getNumNodes();
  CLoadGridFromRGI::TIndices tetrahedron(nodesInElement);

  assert(nodesInElement == 4);

  std::set <RGNodeId> nodeIds;

  for (int i = 0; i < 4; ++i)
  {
  if (!nodeIds.insert(rgElement.getNodeN(i)).second)
  {
      QString message =
    NO_DEGENERATES.arg(element).arg(rgElement.getNodeN(i));

      throw std::runtime_error(message.toStdString());
  }
  }

  geo::CPoint point[4];

  for (int i = 0; i < 4; ++i)
  {
  tetrahedron[i] = nodeIndices[rgElement.getNodeN(i)];

  const RGNode& rgNode = rgi.getNodeN(rgElement.getNodeN(i));

  point[i] = geo::CPoint(rgNode[0], rgNode[1], rgNode[2]);
  }

  if (!geo::ITetrahedron::CheckOrientation(point[0], point[1], point[2],
  point[3]))
  {
  int tmp = tetrahedron[3];

  tetrahedron[3] = tetrahedron[2];
  tetrahedron[2] = tmp;
  }

  return tetrahedron;
}

} // anonymous namespace

void CLoadGridFromRGI::insertElements(TIndices& elementIndices, TIndices& nodeIndices, TPolygonInfo& polygonInfo, TPolygonMap& polygonMap)
{
  // we're going to fill the polygonInfo, so that we have a mapping from RGPolygon to body faces and where appropriate a reference to an interface element

  size_t elements = m_rgi.getNumElements();
  elementIndices.resize(elements);

  m_dump.section("Elements");
  m_dump.size(elements);
  
  for (size_t element = 0; element < elements; ++element)
  {
  const RGElement& rgElement(m_rgi.getElementN(element));

  // Only linear tetra-elements and interface elements for the time being

  if (rgElement.getType() == RGElementType::typeTE12L)
  {
      TIndices tetrahedron = insertElementTE12L(m_rgi, nodeIndices, rgElement, element);

      elementIndices[element] = m_mesh.RegisterTetrahedronInMainMesh(tetrahedron);

      m_dump.element(element, tetrahedron);


      const geo::CTetrahedron& tetraElt = static_cast<const geo::CTetrahedron&>(m_mesh.Element(element));

      for (int face = 0; face < tetraElt.NrOfFaces(); ++face)
      {
    const geo::CBodyTriangle* bodyTriangle = static_cast<const geo::CBodyTriangle*>(&tetraElt.Face(face));

    int polyIndex = -1;
    TIndices polygon(bodyTriangle->NrOfNodes());

    for (int n = 0; n < bodyTriangle->NrOfNodes(); ++n)
          polygon[n] = bodyTriangle->Node(n).Index();

    TIndices orderedPolygon;
    orderPolygon(polygon, orderedPolygon);

    for (size_t j = 0; j < 2; ++j)
    {
          TPolygonMap::const_iterator it = polygonMap.find(orderedPolygon);

          if (it != polygonMap.end())
          {
      polyIndex = it->second;
      polygonInfo[polyIndex].ifElement = elementIndices[element];
          }

          if (polyIndex >= 0)
      break;

          size_t tmp = orderedPolygon[1];
          orderedPolygon[1] = orderedPolygon[2];
          orderedPolygon[2] = tmp;
    }

    if (polyIndex >= 0)
          polygonInfo[polyIndex].face = const_cast<geo::CBodyTriangle*>(bodyTriangle);
      }
  }
  else if(rgElement.getType() == RGElementType::typeT18IF)
  {
      TIndices ifElement(rgElement.getNumNodes());
      for (size_t i = 0; i < rgElement.getNumNodes(); ++i)
    ifElement[i] = rgElement.getNodeN(i);

      geo::CInterfaceElement *pIfElt = new geo::CInterfaceElement(m_mesh, 0, 0, ifElement);
      assert(pIfElt);

      elementIndices[element] = pIfElt->Index();

      m_dump.element(element, ifElement);

      for (size_t i = 0; i < 2; ++i)
      {
    int polyIndex = -1;
    TIndices polygon(3);

    for (size_t j = 0; j < 3; ++j)
          polygon[j] = rgElement.getNodeN(3 * i + j);

    TIndices orderedPolygon;
    orderPolygon(polygon, orderedPolygon);

    for (size_t j = 0; j < 2; ++j)
    {
          TPolygonMap::const_iterator it = polygonMap.find(orderedPolygon);

          if (it != polygonMap.end())
          {
      polyIndex = it->second;
      polygonInfo[polyIndex].ifElement = elementIndices[element];
          }

          if (polyIndex >= 0)
      break;

          size_t tmp = orderedPolygon[1];
          orderedPolygon[1] = orderedPolygon[2];
          orderedPolygon[2] = tmp;
    }

    assert(polyIndex >= 0);
      }

  }
  else
  {
      throw std::runtime_error(ONLY_TE12L_OR_T18IF.toStdString());
  }
  }

}

namespace
{

std::map <int, int> retrieveCounts(
  const CLoadGridFromRGI::TIndices& elementIndices,
  const CLoadGridFromRGI::TIndices& IDs)
{
  std::map <int, int> counts;

  for (size_t element = 0; element < elementIndices.size(); ++element)
  {
  counts[IDs[element]]++;
  }

  return counts;
}

void getFormationNames(std::vector <QString>& formationNames,
  const std::map <int, int>& formationSize,
  const CLoadGridFromRGI::TIndices& elementIndices,
  const CLoadGridFromRGI::TIndices& formationID,
  const CLoadGridFromRGI::TIndices& formationIndex, const RGInterface& rgi)
{
  formationNames.resize(formationSize.size());

  for (std::map <int, int>::const_iterator iterator = formationSize.begin();
  iterator != formationSize.end(); ++iterator)
  {
  bool formationNameFilled = false;

  for (size_t element = 0; element < elementIndices.size(); ++element)
  {
      if (formationID[element] == iterator->first)
      {
    if (!formationNameFilled)
    {
          formationNames[formationIndex[formationID[element]]] =
      rgi.getFormationName(formationID[element]).c_str();
          formationNameFilled = true;
    }
      }
  }
  }
}

CLoadGridFromRGI::TIndices loadFormationID(RGInterface& rgi,
  const CLoadGridFromRGI::TIndices& elementIndices)
{
  CLoadGridFromRGI::TIndices formationID;

  rgi.loadProperty(RGPropertyType::propElementFormation, formationID);
  propertyValuesMayNotContainNull(RGPropertyType::propElementFormation,
  formationID);

  // The assumption is that the size of the vector formationID and
  // the number of elements are equal!

  if (formationID.size() != elementIndices.size())
  {
  QString message = INVALID_FORMATION_COUNT.arg(formationID.size()).
      arg(elementIndices.size());

  throw std::runtime_error(message.toStdString());
  }

  return formationID;
}

CLoadGridFromRGI::TIndices loadFormationVolumeID(RGInterface& rgi,
  CLoadGridFromRGI::TIndices& formationID, const CRockMechProcessor& rmp,
  const CLoadGridFromRGI::TIndices& elementIndices)
{
  CLoadGridFromRGI::TIndices formationVolumeID;

  rgi.loadProperty(RGPropertyType::propBodyIndex, formationVolumeID);
  propertyValuesMayNotContainNull(RGPropertyType::propBodyIndex,
  formationVolumeID);

  // The assumption is that the size of the vector formationVolumeID and
  // the number of elements are equal if and only if the size of the vector
  // formationVolumeID is greater than zero!

  if (formationVolumeID.size() > 0)
  {
  if (formationVolumeID.size() != elementIndices.size())
  {
      QString message = INVALID_FORMATION_VOLUME_COUNT.arg(formationID.size()).
    arg(elementIndices.size());

      throw std::runtime_error(message.toStdString());
  }
  }
  else
  {
  rmp.AddLogLine(PROPERTY_BODY_INDEX);

  // Initialize the vector formationVolumeID with the values from formationID.

  formationVolumeID = formationID;
  }

  return formationVolumeID;
}

} // anonymous namespace

void CLoadGridFromRGI::constructFormations(
  std::vector <QString>& formationNames,
  std::map <int, int>& formationVolume2VolumeMapping,
  const TIndices& elementIndices)
{
  TIndices formationID = loadFormationID(m_rgi, elementIndices);
  std::map <int, int> formationSize =
  retrieveCounts(elementIndices, formationID);
  TIndices formationVolumeID =
  loadFormationVolumeID(m_rgi, formationID, m_rmp, elementIndices);
  std::map <int, int> formationVolumeSize =
  retrieveCounts(elementIndices, formationVolumeID);

  int minimumID = std::numeric_limits <int> ::max();
  int maximumID = std::numeric_limits <int> ::min();

  for (std::map <int, int>::const_iterator iterator = formationSize.begin();
  iterator != formationSize.end(); ++iterator)
  {
  minimumID = std::min(iterator->first, minimumID);
  maximumID = std::max(iterator->first, maximumID);
  }

  TIndices formationIndex(maximumID + 1);
  size_t index = 0;

  for (std::map <int, int>::const_iterator iterator = formationSize.begin();
  iterator != formationSize.end(); ++iterator, ++index)
  {
  formationIndex[iterator->first] = index;
  }

  for (std::map <int, int>::const_iterator iterator =
  formationVolumeSize.begin(); iterator != formationVolumeSize.end();
  ++iterator)
  {
  TIndices formationVolumeIndices(iterator->second);
  index = 0;
  int volume = 0;

  for (size_t element = 0; element < elementIndices.size(); ++element)
  {
      if (formationVolumeID[element] == iterator->first)
      {
    formationVolumeIndices[index++] = elementIndices[element];
    volume = formationID[element];
      }
  }

  assert(volume >= minimumID);

  if (iterator->first >= 0)
      formationVolume2VolumeMapping.insert(std::make_pair(
    m_mesh.RegisterTetVolume(formationVolumeIndices), volume - minimumID));
  }

  getFormationNames(formationNames, formationSize, elementIndices, formationID,
  formationIndex, m_rgi);
}

std::vector <TSharedBodyTriangleMap>
  CLoadGridFromRGI::getTrianglesPerVolume()
{
  std::vector <TSharedBodyTriangleMap> trianglesPerVolume(m_mesh.NrOfVolumes());

  for (int volume = 0; volume < m_mesh.NrOfVolumes(); ++volume)
  {
  geo::CBodyGroup& bodyGroup = m_mesh.Volume(volume);
  CCalculateBoundingSurfaces calculateBoundingSurfaces(bodyGroup);

  trianglesPerVolume[volume] =
      calculateBoundingSurfaces.getSharedBodyTriangleMap();
  }

  return trianglesPerVolume;
}

namespace
{

TSharedBodyTriangle findMatchingTriangle(
  std::vector <TSharedBodyTriangleMap>& trianglesPerVolume,
  const geo::CTetMeshBase& mesh, const QString& key, int volume)
{
  TSharedBodyTriangleMap::iterator removeMatch;
  TSharedBodyTriangle match(0);
  int removeVolume = 0;

  for (int v = (volume + 1); v < mesh.NrOfVolumes(); ++v)
  {
  TSharedBodyTriangleMap::iterator opposite =
      trianglesPerVolume[v].find(key);

  if (opposite != trianglesPerVolume[v].end())
  {
      if (match)
      {
    assert(false);
      }

      removeMatch = opposite;
      removeVolume = v;
      match = (*opposite).second;
  }
  }

  if (match)
  {
  trianglesPerVolume[removeVolume].erase(removeMatch);
  }

  return match;
}

} // anonymous namespace

// Create a map, mapping Geomec node indices to one or two Geomec triangles.
// The argument 'trianglesPerVolume' is passed by value to this function. The
// original has to be maintained and since it is manipulated 'const' is
// not an option.

CLoadGridFromRGI::TNodes2Triangles
  CLoadGridFromRGI::createNodes2TrianglesMapping(
  std::vector <TSharedBodyTriangleMap> trianglesPerVolume) const
{
  TNodes2Triangles nodes2Triangles;

  for (int volume = 0; volume < m_mesh.NrOfVolumes(); ++volume)
  {
  for (TSharedBodyTriangleMap::const_iterator triangle =
      trianglesPerVolume[volume].begin();
      triangle != trianglesPerVolume[volume].end(); ++triangle)
  {
      TSharedBodyTriangle match = findMatchingTriangle(
    trianglesPerVolume, m_mesh, (*triangle).first, volume);
      std::pair <TNodes2Triangles::iterator, bool> result =
    nodes2Triangles.insert(std::make_pair((*triangle).first,
          std::make_pair((*triangle).second, match)));

      assert(result.second);
  }
  }

  return nodes2Triangles;
}

void CLoadGridFromRGI::constructFaults(TFaceDescriptorMap& linkFace2Horizon,
  TFaceDescriptorMap& linkFace2SurfaceDescriptor,
  const TNodes2Triangles& nodes2Triangles, const TIndices& nodeIndices, const TPolygonInfo& polygonInfo)
{
  for (size_t f = 0; f < m_rgi.getNumFaults(); ++f)
  {
  const RGSurface& back = m_rgi.getFaultBackN(f);
  const RGSurface& front = m_rgi.getFaultFrontN(f);

  assert(back.getType() == RGSurface::FaultBack);
  assert(front.getType() == RGSurface::FaultFront);

  assert(back.getAttribute() == RGSurface::Regular);
  assert(front.getAttribute() == RGSurface::Regular);

  assert(back.getName() == front.getName());
  assert(back.getSize() == front.getSize());

  constructFault(linkFace2Horizon, linkFace2SurfaceDescriptor,
      nodes2Triangles, nodeIndices, polygonInfo, back, front);
  }
}

void CLoadGridFromRGI::constructHorizons(TFaceDescriptorMap& linkFace2Horizon,
  TFaceDescriptorMap& linkFace2SurfaceDescriptor,
  const TNodes2Triangles& nodes2Triangles, const TIndices& nodeIndices)
{
  for (size_t h = 0; h < m_rgi.getNumHorizons(); ++h)
  {
  const RGSurface& horizon = m_rgi.getHorizonN(h);

  assert(horizon.getType() == RGSurface::Horizon);

  if (horizon.getAttribute() == RGSurface::Regular)
  {
      constructRegularHorizon(linkFace2Horizon, linkFace2SurfaceDescriptor,
    nodes2Triangles, nodeIndices, horizon);
  }
  else if ((horizon.getAttribute() == RGSurface::Top) ||
      (horizon.getAttribute() == RGSurface::Bottom))
  {
      constructTopBottomHorizon(linkFace2Horizon, linkFace2SurfaceDescriptor,
    nodes2Triangles, nodeIndices, horizon);
  }
  }
}

void CLoadGridFromRGI::constructSideSurfaces(
  TFaceDescriptorMap& linkFace2SurfaceDescriptor,
  const TNodes2Triangles& nodes2Triangles, const TIndices& nodeIndices)
{
  for (size_t s = 0; s < m_rgi.getNumHorizons(); ++s)
  {
  const RGSurface& sideSurface = m_rgi.getHorizonN(s);

  assert(sideSurface.getType() == RGSurface::Horizon);

  if (sideSurface.getAttribute() == RGSurface::Side)
  {
      CTetraSubBoundary& subBoundary =
    static_cast <CTetraModel&> (m_model).SubBoundary();
      CMeshSurface* meshSurface =
    constructMeshSurface(sideSurface, nodes2Triangles, nodeIndices);

      constructFaceDescriptorMap(linkFace2SurfaceDescriptor, meshSurface);

      m_mesh.AddSurface(*meshSurface, sideSurface.getName(), false);

      CTetraSurface* tetraSurface =
    new CTetraSurface(sideSurface.getName().c_str(), *meshSurface, m_model);

      subBoundary.LinkTo(*tetraSurface);
      static_cast <CTetraMesh&> (m_model.Mesh()).
    m_vcSurfaceSource.push_back(&subBoundary);
  }
  }
}

namespace
{

const QString BODY = "BODY-%1";
const QString FORMAT = "%1-%2";

std::vector <QString> constructFormationVolumeNames(
  std::map <int, int>& formationVolume2VolumeMapping,
  const std::vector <QString>& formationNames, size_t volumes)
{
  std::vector <QString> formationVolumeNames(volumes);

  for (size_t volume = 0; volume < volumes; ++volume)
  {
  if (formationNames[formationVolume2VolumeMapping[volume]].isEmpty())
  {
      formationVolumeNames[volume] = BODY.arg(volume);
  }
  else
  {
      formationVolumeNames[volume] = FORMAT.
    arg(formationNames[formationVolume2VolumeMapping[volume]]).arg(volume);
  }
  }

  return formationVolumeNames;
}

const QString FORMATION = "FORMATION-%1";

void validateFormationNames(std::vector <QString>& formationNames)
{
  size_t formation = 0;

  for (std::vector <QString>::iterator formationName = formationNames.begin();
  formationName != formationNames.end(); ++formationName)
  {
  if ((*formationName).isEmpty())
  {
      QString uniqueName;

      do
      {
    uniqueName = FORMATION.arg(formation++);
      }
      while (std::find(formationNames.begin(), formationNames.end(),
    uniqueName) != formationNames.end());

      *formationName = uniqueName;
  }
  }
}

} // anonymous namespace

void CLoadGridFromRGI::constructFormationVolumes(
  TFaceDescriptorMap& /*linkFace2Horizon*/,
  TFaceDescriptorMap& linkFace2SurfaceDescriptor,
  std::map <int, int>& formationVolume2VolumeMapping,
  const std::vector <TSharedBodyTriangleMap>& trianglesPerVolume,
  std::vector <QString>& formationNames)
{
  std::vector <CTetraFormation*> formations(formationNames.size());

  m_geomecFormations.resize(formationNames.size());

  // significant ordering (empty formation names are significant)!

  std::vector <QString> formationVolumeNames = constructFormationVolumeNames(
  formationVolume2VolumeMapping, formationNames, m_mesh.NrOfVolumes());

  validateFormationNames(formationNames);

  for (int volume = 0; volume < m_mesh.NrOfVolumes(); ++volume)
  {
  const geo::CBodyGroup& bodyGroup = m_mesh.Volume(volume);

  if (formations[formationVolume2VolumeMapping[volume]] == 0)
  {
      formations[formationVolume2VolumeMapping[volume]] =
    new CTetraFormation(
          formationNames[formationVolume2VolumeMapping[volume]], m_model);
      m_geomecFormations[formationVolume2VolumeMapping[volume]] =
    formations[formationVolume2VolumeMapping[volume]];
  }

  CTetraFormation* tetraFormation =
      formations[formationVolume2VolumeMapping[volume]];
  CTetraFormationVolume* tetraFormationVolume =
      new CTetraFormationVolume(*tetraFormation,
    const_cast <geo::CBodyGroup&> (bodyGroup));
  std::vector <CMeshSurface*> meshSurface = extrapolateMeshSurfaces(
      trianglesPerVolume[volume], linkFace2SurfaceDescriptor);
  int surfaceIndex = 0;

  tetraFormationVolume->Name(formationVolumeNames[volume]);
  tetraFormationVolume->reParent(tetraFormation);

  for (size_t surface = 0; surface < meshSurface.size(); ++surface)
  {
      TFaceDescriptorMap::const_iterator iterator =
    linkFace2SurfaceDescriptor.find(&meshSurface[surface]->Face(0));

      if (iterator != linkFace2SurfaceDescriptor.end())
      {
    surfaceIndex = (*iterator).second;
      }
      else
      {
    assert(false);
      }

      /*geo::CTetSurface* tetSurface =*/ new geo::CTetSurface(
    const_cast <geo::CSurfaceDesc&> (m_mesh.SurfaceDesc(surfaceIndex)),
    const_cast <geo::CBodyGroup&> (bodyGroup));
      geo::CBodyGroup::CSideSurface& sideSurface =
    const_cast <geo::CBodyGroup&> (bodyGroup).
          SideSurface(bodyGroup.SideSurfaceSize() - 1);

      for (int face = 0; face < meshSurface[surface]->FaceSize(); ++face)
      {
    sideSurface.AddFace(meshSurface[surface]->Face(face));
      }
  }
  }
}

void CLoadGridFromRGI::constructFaceDescriptorMap(
  TFaceDescriptorMap& linkFace2SurfaceDescriptor,
  const CMeshSurface* meshSurface) const
{
  // The assumption is that meshSurface will be added to m_mesh next!

  for (int face = 0; face < meshSurface->FaceSize(); ++face)
  {
  std::pair <TFaceDescriptorMap::const_iterator, bool> result =
      linkFace2SurfaceDescriptor.insert(
    std::make_pair <const geo::IFace*, int> (&meshSurface->Face(face),
          m_mesh.NrOfSurfaces()));

  assert(result.second);
  }
}

namespace
{

const TSharedBodyTriangle getSharedBodyTriangle(const RGPolygonId& polygonId,
  const RGInterface& rgi, const CLoadGridFromRGI::TIndices& nodeIndices,
  const CLoadGridFromRGI::TNodes2Triangles& nodes2Triangles)
{
  const RGPolygon& polygon = rgi.getPolygonN(polygonId);

  assert(polygon.size() == 3);

  QString key = QString("%1.%2.%3").arg(nodeIndices[polygon[0]]).
  arg(nodeIndices[polygon[1]]).arg(nodeIndices[polygon[2]]);
  const CLoadGridFromRGI::TNodes2Triangles::const_iterator
  result = nodes2Triangles.find(key);

  if (result == nodes2Triangles.end())
  {
  QString s = QString("Cannot find polygon %1 (nodes %2, %3, %4) in our collection of body faces.").arg(polygonId).arg(polygon[0]).arg(polygon[1]).arg(polygon[2]);
  throw std::runtime_error(s.toStdString());
  }

  return (*result).second.first;
}

const geo::CBodyTriangle* getBodyTriangle(const RGPolygonId& polygonId,
  const RGInterface& rgi, const CLoadGridFromRGI::TIndices& nodeIndices,
  const CLoadGridFromRGI::TNodes2Triangles& nodes2Triangles)
{
  return getSharedBodyTriangle(polygonId, rgi, nodeIndices, nodes2Triangles)->
  getBodyTriangle();
}

const QString NO_INTERFACE_ELEMENT =
  "Encountered a non-interface element while constructing a fault";

} // anonymous namespace

void CLoadGridFromRGI::constructFault(TFaceDescriptorMap& /*linkFace2Horizon*/,
  TFaceDescriptorMap& /*linkFace2SurfaceDescriptor*/,
  const TNodes2Triangles& /*nodes2Triangles*/, const TIndices& /*nodeIndices*/, const TPolygonInfo& polygonInfo,
  const RGSurface& back, const RGSurface& front) const
{
  CSurfaceEntry& surfaceEntry =
  dynamic_cast <CSurfaceEntry&> (*(m_model.GraphEntry(MD_BASE_SURFACE)));
  QSharedPointer <geo::CSurface> geoSurface(new geo::CSurface);

  for (size_t p = 0; p < back.getSize(); ++p)
  {
  if (polygonInfo[back.getPolygonN(p)].face != 0)
      geoSurface->AddFace(*polygonInfo[back.getPolygonN(p)].face);
  }

  CSurfaceBase* surfaceBase =
  surfaceEntry.InsertSurface(back.getName().c_str(), *geoSurface);
  CTetraSubHorizon* tetraSubHorizon =
  new CTetraSubHorizon(back.getName().c_str(), TRUE, m_model);
  geo::CSurfaceDesc* surfaceDesc = new geo::CSurfaceDesc(m_mesh,
  surfaceBase->Surface(), std::string(), true);

  static_cast <CTetraMesh&> (
  m_model.Mesh()).m_vcSurfaceSource.push_back(surfaceBase);
  tetraSubHorizon->LinkTo(*surfaceBase);

  for (size_t p = 0; p < back.getSize(); ++p)
  {
  int ifEltIndex = polygonInfo[back.getPolygonN(p)].ifElement;
  assert(ifEltIndex >= 0);

  geo::IElement& elm = const_cast<geo::IElement&>(m_mesh.Element(ifEltIndex));

  if (dynamic_cast<geo::CInterfaceElement*>(&elm) == 0)
  {
      throw std::runtime_error(NO_INTERFACE_ELEMENT.toStdString());
  }

  geo::CInterfaceElement& iface = static_cast<geo::CInterfaceElement&>(elm);

  iface.setBack(polygonInfo[back.getPolygonN(p)].face);
  }

  for (size_t p = 0; p < front.getSize(); ++p)
  {
  int ifEltIndex = polygonInfo[front.getPolygonN(p)].ifElement;
  assert(ifEltIndex >= 0);

  geo::IElement& elm = const_cast<geo::IElement&>(m_mesh.Element(ifEltIndex));
  assert(dynamic_cast<geo::CInterfaceElement*>(&elm));
  geo::CInterfaceElement& iface = static_cast<geo::CInterfaceElement&>(elm);

  iface.setFront(polygonInfo[front.getPolygonN(p)].face);

  surfaceDesc->AddInterfaceElement(iface);
  }
}

CMeshSurface* CLoadGridFromRGI::constructMeshSurface(const RGSurface& surface,
  const TNodes2Triangles& nodes2Triangles,
  const TIndices& nodeIndices) const
{
  CMeshSurface* meshSurface = new CMeshSurface();

  const_cast<CRGIGridDump&>(m_dump).surface(surface);

  for (size_t p = 0; p < surface.getSize(); ++p)
  {
  meshSurface->AddFace(*getBodyTriangle(surface.getPolygonN(p), m_rgi,
      nodeIndices, nodes2Triangles));
  }

  return meshSurface;
}

void CLoadGridFromRGI::constructRegularHorizon(
  TFaceDescriptorMap& linkFace2Horizon,
  TFaceDescriptorMap& linkFace2SurfaceDescriptor,
  const TNodes2Triangles& nodes2Triangles, const TIndices& nodeIndices,
  const RGSurface& horizon) const
{
  CMeshSurface* meshSurface =
  constructMeshSurface(horizon, nodes2Triangles, nodeIndices);

  constructFaceDescriptorMap(linkFace2Horizon, meshSurface);
  constructFaceDescriptorMap(linkFace2SurfaceDescriptor, meshSurface);

  m_mesh.AddSurface(*meshSurface, horizon.getName(), false);

  CTetraSurface* tetraSurface =
  new CTetraSurface(horizon.getName().c_str(), *meshSurface, m_model);
  CTetraSubHorizon* tetraSubHorizon =
  new CTetraSubHorizon(horizon.getName().c_str(), FALSE, m_model);

  tetraSubHorizon->LinkTo(*tetraSurface);
  static_cast <CTetraMesh&> (m_model.Mesh()).
  m_vcSurfaceSource.push_back(tetraSurface);
}

void CLoadGridFromRGI::constructTopBottomHorizon(
  TFaceDescriptorMap& /*linkFace2Horizon*/,
  TFaceDescriptorMap& linkFace2SurfaceDescriptor,
  const TNodes2Triangles& nodes2Triangles, const TIndices& nodeIndices,
  const RGSurface& horizon) const
{
  CMeshSurface* meshSurface =
  constructMeshSurface(horizon, nodes2Triangles, nodeIndices);

  constructFaceDescriptorMap(linkFace2SurfaceDescriptor, meshSurface);

  m_mesh.AddSurface(*meshSurface, horizon.getName(), false);

  CTetraSubBoundary& subBoundary =
  static_cast <CTetraModel&> (m_model).SubBoundary();
  CTetraSubHorizon* topHorizon = subBoundary.TopHorizon().Horizon();
  CTetraSubHorizon* bottomHorizon = subBoundary.BottomHorizon().Horizon();

  CTetraSurface* tetraSurface =
  new CTetraSurface(horizon.getName().c_str(), *meshSurface, m_model);

  if (horizon.getAttribute() == RGSurface::Top)
  {
  topHorizon->LinkTo(*tetraSurface);
  static_cast <CTetraMesh&> (m_model.Mesh()).
      m_vcSurfaceSource.push_back(tetraSurface);
  }
  else  // horizon.getAttribute() == RGSurface::Bottom
  {
  bottomHorizon->LinkTo(*tetraSurface);
  static_cast <CTetraMesh&> (m_model.Mesh()).
      m_vcSurfaceSource.push_back(tetraSurface);
  }
}

namespace
{

CLoadGridFromRGI::TSurfaceCollection
  collectSurfaces(const TSharedBodyTriangleMap& triangles,
  const CLoadGridFromRGI::TFaceDescriptorMap& linkFace2SurfaceDescriptor)
{
  CLoadGridFromRGI::TSurfaceCollection surfaces;

  for (TSharedBodyTriangleMap::const_iterator triangle = triangles.begin();
  triangle != triangles.end(); ++triangle)
  {
  CLoadGridFromRGI::TFaceDescriptorMap::const_iterator result =
      linkFace2SurfaceDescriptor.find((*triangle).second->getBodyTriangle());

  if (result != linkFace2SurfaceDescriptor.end())
  {
      std::set <const geo::IFace*> facesSet;

      facesSet.insert((*result).first);
      std::pair <CLoadGridFromRGI::TSurfaceCollection::iterator, bool> surface =
    surfaces.insert(std::make_pair((*result).second, facesSet));

      if (!surface.second)
      {
    (*(surface.first)).second.insert((*result).first);
      }
  }
  else
  {
      // internal triangles are not included in 'linkFace2SurfaceDescriptor'
  }
  }

  return surfaces;
}

} // anonymous namespace

std::vector <CMeshSurface*> CLoadGridFromRGI::extrapolateMeshSurfaces(
  const TSharedBodyTriangleMap& triangles,
  const TFaceDescriptorMap& linkFace2SurfaceDescriptor) const
{
  TSurfaceCollection surfaces =
  collectSurfaces(triangles, linkFace2SurfaceDescriptor);
  std::vector <CMeshSurface*> meshSurfaces;

  for (TSurfaceCollection::const_iterator
  surface = surfaces.begin(); surface != surfaces.end(); ++surface)
  {
  CMeshSurface* meshSurface = new CMeshSurface();
  
  for (std::set <const geo::IFace*>::const_iterator
      face = (*surface).second.begin(); face != (*surface).second.end(); ++face)
  {
      meshSurface->AddFace(*(*face));
  }

  if (meshSurface->FaceSize() > 0)
  {
      meshSurfaces.push_back(meshSurface);
  }
  }

  return meshSurfaces;
}

} // namespace GeomecRGI
