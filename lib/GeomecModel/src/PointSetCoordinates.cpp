#include "PointSetCoordinates.h"
#include "LengthQuantity.h"
#include "ModelBase.h"
#include "GeomecStringTable.h"
#include "BoundaryBase.h"
#include "unitnode.h"
#include "ModelBase.h"
#include "resourceIDS.h"

CPointSetCoordinates::CPointSetCoordinates(const CModelBase& modelBase,
  const CUnitNode& unitNode, bool is2D)
: m_modelBase(modelBase)
, m_unitNode(unitNode)
, m_coordinates(createCoordinates(m_modelBase, m_unitNode, is2D))
{
}

void CPointSetCoordinates::initialize(bool is2D)
{
  m_coordinates = createCoordinates(m_modelBase, m_unitNode, is2D);
}

std::vector <CPointSetCoordinates::CCoordinate>&
  CPointSetCoordinates::operator () ()
{
  return m_coordinates;
}

// private

CPointSetCoordinates::CCoordinate CPointSetCoordinates::createCoordinate(
  unsigned int stringTableEntry, int index, double minimumValue,
  double maximumValue, const CUnitNode& unitNode)
{
  CLengthQuantity lengthQuantity;
  CCoordinate coordinate;

  coordinate.m_name = getStringTableEntry(stringTableEntry);
  coordinate.m_density = 10;
  coordinate.m_index = index;
  coordinate.m_min =
  lengthQuantity.Convert(minimumValue, unitNode.Unit(), CQuantity::SI_UNIT);
  coordinate.m_max =
  lengthQuantity.Convert(maximumValue, unitNode.Unit(), CQuantity::SI_UNIT);

  return coordinate;
}

std::vector <CPointSetCoordinates::CCoordinate> CPointSetCoordinates::
  createCoordinates(const CModelBase& modelBase, const CUnitNode& unitNode,
  bool is2D)
{
  std::vector <CCoordinate> coordinates;

  // Init the X coordinate

  coordinates.push_back(createCoordinate(IDS_RC_NORTHING,
  0, modelBase.Boundary().Min().X(), modelBase.Boundary().Max().X(),
  unitNode));

  // Init the Y coordinate

  coordinates.push_back(createCoordinate(IDS_RC_EASTING,
  1, modelBase.Boundary().Min().Y(), modelBase.Boundary().Max().Y(),
  unitNode));

  if (!is2D)
  {
  // Init the Y coordinate

  coordinates.push_back(createCoordinate(
      IDS_RC_DEPTH, 2, modelBase.Boundary().Min().Z(),
      modelBase.Boundary().Max().Z(), unitNode));
  }

  return coordinates;
}
