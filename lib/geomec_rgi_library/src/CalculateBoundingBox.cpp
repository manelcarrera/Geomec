#include <complex>

#include "CalculateBoundingBox.h"

namespace
{

void adjustBoundaries(double& positive, double& negative, double coordinate)
{
  if (positive < coordinate)
  {
  positive = coordinate;
  }

  if (coordinate < negative)
  {
  negative = coordinate;
  }
}

GeomecRGI::TBoundingBox extrapolateBoundingBox(
  const GeomecRGI::TSharedBodyTriangleMap& triangleMap)
{
  GeomecRGI::TBoundingBox boundingBox;

  boundingBox.front =
  (*triangleMap.begin()).second->getBodyTriangle()->Point(0).X();
  boundingBox.back =
  (*triangleMap.begin()).second->getBodyTriangle()->Point(0).X();
  boundingBox.right =
  (*triangleMap.begin()).second->getBodyTriangle()->Point(0).Y();
  boundingBox.left =
  (*triangleMap.begin()).second->getBodyTriangle()->Point(0).Y();
  boundingBox.top =
  (*triangleMap.begin()).second->getBodyTriangle()->Point(0).Z();
  boundingBox.bottom =
  (*triangleMap.begin()).second->getBodyTriangle()->Point(0).Z();

  for (GeomecRGI::TSharedBodyTriangleMap::const_iterator triangle =
  triangleMap.begin(); triangle != triangleMap.end(); ++triangle)
  {
  for (int point = 0;
      point < (*triangle).second->getBodyTriangle()->NrOfPoints(); ++point)
  {
      adjustBoundaries(boundingBox.front, boundingBox.back,
    (*triangle).second->getBodyTriangle()->Point(point).X());
      adjustBoundaries(boundingBox.right, boundingBox.left,
    (*triangle).second->getBodyTriangle()->Point(point).Y());
      adjustBoundaries(boundingBox.top, boundingBox.bottom,
    (*triangle).second->getBodyTriangle()->Point(point).Z());
  }
  }

  return boundingBox;
}

} // anonymous namespace

namespace GeomecRGI
{

CCalculateBoundingBox::CCalculateBoundingBox(geo::IElementSet& elementSet)
: CCalculateBoundingSurfaces(elementSet)
, m_BoundingBox(extrapolateBoundingBox(m_SharedBodyTriangleMap))
{
}

namespace
{

const double ERROR_MARGIN = 1.01;

const size_t NUMBER_OF_SURFACES = 6;

const size_t SURFACE_BOTTOM = 0;
const size_t SURFACE_TOP = 1;
const size_t SURFACE_FRONT = 2;
const size_t SURFACE_BACK = 3;
const size_t SURFACE_RIGHT = 4;
const size_t SURFACE_LEFT = 5;

} // anonymous namespace

TTriangleSurfaces
  CCalculateBoundingBox::collectTriangleSurfaces()
{
  // Retrieve the vector with all unique triangles.

  TTriangleMap triangleMap = getTriangleMap();

  double middleX = (m_BoundingBox.front + m_BoundingBox.back) / 2,
  middleY = (m_BoundingBox.right + m_BoundingBox.left) / 2,
  middleZ = (m_BoundingBox.top + m_BoundingBox.bottom) / 2;
  TTriangleSurfaces
  triangleSurfaces(NUMBER_OF_SURFACES);

  for (TTriangleMap::const_iterator triangle = triangleMap.begin();
  triangle != triangleMap.end(); ++triangle)
  {
  if ((*triangle).second == 0)
  {
      double x = (*triangle).first->MidPoint().X();
      double y = (*triangle).first->MidPoint().Y();
      double z = (*triangle).first->MidPoint().Z();

      if ((middleX <= x) && (x <= m_BoundingBox.front))
      {
    double Ab = std::abs((m_BoundingBox.top - m_BoundingBox.bottom) *
          (m_BoundingBox.right - m_BoundingBox.left));  // Area at base
    double h = (m_BoundingBox.front - middleX); // Height
    double h2 = h * h;  // Height squared
    double zh = h - (x - middleX);
    double Az = Ab * (((h - zh) * (h - zh)) / h2);  // Area at z
    double scaleSquared = Ab / Az;
    double scale = std::sqrt(scaleSquared);
    double distanceZ = ERROR_MARGIN *
          (((m_BoundingBox.top - m_BoundingBox.bottom) / scale) / 2);
    double distanceY = ERROR_MARGIN *
          (((m_BoundingBox.right - m_BoundingBox.left) / scale) / 2);

    if (((middleZ - distanceZ) <= z) && (z <= (middleZ + distanceZ)) &&
          ((middleY - distanceY) <= y) && (y <= (middleY + distanceY)))
    {
          triangleSurfaces[SURFACE_FRONT].push_back((*triangle).first);
          continue;
    }
      }
      else if ((m_BoundingBox.back <= x) && (x <= middleX))
      {
    double Ab = std::abs((m_BoundingBox.top - m_BoundingBox.bottom) *
          (m_BoundingBox.right - m_BoundingBox.left));  // Area at base
    double h = (middleX - m_BoundingBox.back);  // Height
    double h2 = h * h;  // Height squared
    double zh = x - m_BoundingBox.back;
    double Az = Ab * (((h - zh) * (h - zh)) / h2);  // Area at z
    double scaleSquared = Ab / Az;
    double scale = std::sqrt(scaleSquared);
    double distanceZ = ERROR_MARGIN *
          (((m_BoundingBox.top - m_BoundingBox.bottom) / scale) / 2);
    double distanceY = ERROR_MARGIN *
          (((m_BoundingBox.right - m_BoundingBox.left) / scale) / 2);

    if (((middleZ - distanceZ) <= z) && (z <= (middleZ + distanceZ)) &&
          ((middleY - distanceY) <= y) && (y <= (middleY + distanceY)))
    {
          triangleSurfaces[SURFACE_BACK].push_back((*triangle).first);
          continue;
    }
      }

      if ((middleY <= y) && (y <= m_BoundingBox.right))
      {
    double Ab = std::abs((m_BoundingBox.top - m_BoundingBox.bottom) *
          (m_BoundingBox.front - m_BoundingBox.back));  // Area at base
    double h = (m_BoundingBox.right - middleY); // Height
    double h2 = h * h;  // Height squared
    double zh = h - (y - middleY);
    double Az = Ab * (((h - zh) * (h - zh)) / h2);  // Area at z
    double scaleSquared = Ab / Az;
    double scale = std::sqrt(scaleSquared);
    double distanceZ = ERROR_MARGIN *
          (((m_BoundingBox.top - m_BoundingBox.bottom) / scale) / 2);
    double distanceX = ERROR_MARGIN *
          (((m_BoundingBox.front - m_BoundingBox.back) / scale) / 2);

    if (((middleZ - distanceZ) <= z) && (z <= (middleZ + distanceZ)) &&
          ((middleX - distanceX) <= x) && (x <= (middleX + distanceX)))
    {
          triangleSurfaces[SURFACE_RIGHT].push_back((*triangle).first);
          continue;
    }
      }
      else if ((m_BoundingBox.left <= y) && (y <= middleY))
      {
    double Ab = std::abs((m_BoundingBox.top - m_BoundingBox.bottom) *
          (m_BoundingBox.front - m_BoundingBox.back));  // Area at base
    double h = (middleY - m_BoundingBox.left);  // Height
    double h2 = h * h;  // Height squared
    double zh = y - m_BoundingBox.left;
    double Az = Ab * (((h - zh) * (h - zh)) / h2);  // Area at z
    double scaleSquared = Ab / Az;
    double scale = std::sqrt(scaleSquared);
    double distanceZ = ERROR_MARGIN *
          (((m_BoundingBox.top - m_BoundingBox.bottom) / scale) / 2);
    double distanceX = ERROR_MARGIN *
          (((m_BoundingBox.front - m_BoundingBox.back) / scale) / 2);

    if (((middleZ - distanceZ) <= z) && (z <= (middleZ + distanceZ)) &&
          ((middleX - distanceX) <= x) && (x <= (middleX + distanceX)))
    {
          triangleSurfaces[SURFACE_LEFT].push_back((*triangle).first);
          continue;
    }
      }

      if ((middleZ <= z) && (z <= m_BoundingBox.top))
      {
    double Ab = std::abs((m_BoundingBox.front - m_BoundingBox.back) *
          (m_BoundingBox.right - m_BoundingBox.left));  // Area at base
    double h = (m_BoundingBox.top - middleZ); // Height
    double h2 = h * h;  // Height squared
    double zh = h - (z - middleZ);
    double Az = Ab * (((h - zh) * (h - zh)) / h2);  // Area at z
    double scaleSquared = Ab / Az;
    double scale = std::sqrt(scaleSquared);
    double distanceX = ERROR_MARGIN *
          (((m_BoundingBox.front - m_BoundingBox.back) / scale) / 2);
    double distanceY = ERROR_MARGIN *
          (((m_BoundingBox.right - m_BoundingBox.left) / scale) / 2);

    if (((middleX - distanceX) <= x) && (x <= (middleX + distanceX)) &&
          ((middleY - distanceY) <= y) && (y <= (middleY + distanceY)))
    {
          triangleSurfaces[SURFACE_TOP].push_back((*triangle).first);
          continue;
    }
      }
      else if ((m_BoundingBox.bottom <= z) && (z <= middleZ))
      {
    double Ab = std::abs((m_BoundingBox.front - m_BoundingBox.back) *
          (m_BoundingBox.right - m_BoundingBox.left));  // Area at base
    double h = (middleZ - m_BoundingBox.bottom);  // Height
    double h2 = h * h;  // Height squared
    double zh = z - m_BoundingBox.bottom;
    double Az = Ab * (((h - zh) * (h - zh)) / h2);  // Area at z
    double scaleSquared = Ab / Az;
    double scale = std::sqrt(scaleSquared);
    double distanceX = ERROR_MARGIN *
          (((m_BoundingBox.front - m_BoundingBox.back) / scale) / 2);
    double distanceY = ERROR_MARGIN *
          (((m_BoundingBox.right - m_BoundingBox.left) / scale) / 2);

    if (((middleX - distanceX) <= x) && (x <= (middleX + distanceX)) &&
          ((middleY - distanceY) <= y) && (y <= (middleY + distanceY)))
    {
          triangleSurfaces[SURFACE_BOTTOM].push_back((*triangle).first);
          continue;
    }
      }

      assert(false);
  }
  }

  return triangleSurfaces;
}

// private

TTriangleMap CCalculateBoundingBox::getTriangleMap() const
{
  std::set <const geo::CBodyTriangle*> bodyTriangleSet;

  for (TSharedBodyTriangleMap::const_iterator
  triangle = m_SharedBodyTriangleMap.begin();
  triangle != m_SharedBodyTriangleMap.end(); ++triangle)
  {
  bodyTriangleSet.insert((*triangle).second->getBodyTriangle());
  }

  assert(bodyTriangleSet.size() == (m_SharedBodyTriangleMap.size() / 6));

  TTriangleMap triangleMap;

  for (std::set <const geo::CBodyTriangle*> ::const_iterator bodyTriangle =
  bodyTriangleSet.begin(); bodyTriangle != bodyTriangleSet.end();
  ++bodyTriangle)
  {
  std::pair <TTriangleMap::iterator, bool> result =
      triangleMap.insert(std::make_pair(*bodyTriangle, 0));

  assert(result.second);
  }
  
  assert(triangleMap.size() == bodyTriangleSet.size());

  return triangleMap;
}

} // namespace GeomecRGI
