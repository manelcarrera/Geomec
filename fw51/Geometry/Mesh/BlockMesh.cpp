/* Copyright (c) 2011 TNO DIANA BV                              Confidential */
#include "BlockMesh.h"
#include "Hexahedron.h"
#include "dimple.h"
#include <math.h>

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

namespace geo {
CBlockMesh::CBlockMesh()
    : m_dXbase(0), m_dYbase(0), m_dZbase(0), m_spacing_x(0), m_spacing_y(0), m_spacing_z(0), m_length_division(0),
      m_width_division(0), m_height_division(0) {}

CBlockMesh &CBlockMesh::operator=(const CBlockMesh &rhs) {
  m_dXbase = rhs.m_dXbase;
  m_dYbase = rhs.m_dYbase;
  m_dZbase = rhs.m_dZbase;
  m_height_division = rhs.m_height_division;
  m_length_division = rhs.m_length_division;
  m_width_division = rhs.m_width_division;
  m_spacing_x = rhs.m_spacing_x;
  m_spacing_y = rhs.m_spacing_y;
  m_spacing_z = rhs.m_spacing_z;

  BuildMesh();

  return *this;
}

CBlockMesh::~CBlockMesh() {}

void CBlockMesh::Clear() {
  m_mpMeshPoints.clear();
  CMesh::Clear();
}

void CBlockMesh::BuildMesh() {
  Clear();

  std::vector<const IPoint *> points;

  int nPtIndex;
  int NrOfPointsX = m_length_division + 1;
  int NrOfPointsY = m_width_division + 1;

  int p0, p1, p2, p3, p4, p5, p6, p7;
  int i, j, k;

  // Point creation ************************************************************************

  for (i = 0; i < m_height_division + 1; i++) {
    for (j = 0; j < m_width_division + 1; j++) {
      for (k = 0; k < m_length_division + 1; k++) {
        nPtIndex = i * NrOfPointsX * NrOfPointsY + j * NrOfPointsX + k;

        CPoint *pPoint = new CPoint(m_dXbase + (double)k * m_spacing_x, m_dYbase + (double)j * m_spacing_y,
                                    m_dZbase + (double)i * m_spacing_z);
        int index = RegisterNode(*pPoint);

        if (k == 0 && j == 0 && i == 0)
          m_base_node = index;

        if (k == 0)
          m_vcLeftFace.push_back(index);
        if (k == m_length_division)
          m_vcRightFace.push_back(index);

        if (j == 0)
          m_vcFrontFace.push_back(index);
        if (j == m_width_division)
          m_vcBackFace.push_back(index);

        if (i == 0)
          m_vcBottomFace.push_back(index);
        if (i == m_height_division)
          m_vcTopFace.push_back(index);

        m_mpMeshPoints.insert(std::map<int, int>::value_type(nPtIndex, index));
      }
    }
  }

  // ***************************************************************************************

  // Element creation **********************************************************************

  std::map<int, int>::iterator it;
  for (i = 0; i < m_height_division; i++) {
    for (j = 0; j < m_width_division; j++) {
      for (k = 0; k < m_length_division; k++) {
        points.clear();
        nPtIndex = i * NrOfPointsX * NrOfPointsY + j * NrOfPointsX + k;

        it = m_mpMeshPoints.find(nPtIndex);
        assert(it != m_mpMeshPoints.end());
        p0 = (*it).second;

        it = m_mpMeshPoints.find(nPtIndex + 1);
        assert(it != m_mpMeshPoints.end());
        p1 = (*it).second;

        it = m_mpMeshPoints.find(nPtIndex + NrOfPointsX + 1);
        assert(it != m_mpMeshPoints.end());
        p2 = (*it).second;

        it = m_mpMeshPoints.find(nPtIndex + NrOfPointsX);
        assert(it != m_mpMeshPoints.end());
        p3 = (*it).second;

        it = m_mpMeshPoints.find(nPtIndex + NrOfPointsX * NrOfPointsY);
        assert(it != m_mpMeshPoints.end());
        p4 = (*it).second;

        it = m_mpMeshPoints.find(nPtIndex + 1 + NrOfPointsX * NrOfPointsY);
        assert(it != m_mpMeshPoints.end());
        p5 = (*it).second;

        it = m_mpMeshPoints.find(nPtIndex + NrOfPointsX + 1 + NrOfPointsX * NrOfPointsY);
        assert(it != m_mpMeshPoints.end());
        p6 = (*it).second;

        it = m_mpMeshPoints.find(nPtIndex + NrOfPointsX + NrOfPointsX * NrOfPointsY);
        assert(it != m_mpMeshPoints.end());
        p7 = (*it).second;

        p4 = p0 + NrOfPointsX * NrOfPointsY;
        p5 = p1 + NrOfPointsX * NrOfPointsY;
        p6 = p2 + NrOfPointsX * NrOfPointsY;
        p7 = p3 + NrOfPointsX * NrOfPointsY;

        points.push_back(&Point(p0));
        points.push_back(&Point(p1));
        points.push_back(&Point(p2));
        points.push_back(&Point(p3));
        points.push_back(&Point(p4));
        points.push_back(&Point(p5));
        points.push_back(&Point(p6));
        points.push_back(&Point(p7));

        new CHexahedron(*this, points);
      }
    }
  }

  // ***************************************************************************************
}

void CBlockMesh::SetMesh(const IPoint &minpoint, const double &length, const double &width, const double &height,
                         int length_division, int width_division, int height_division) {
  Clear();

  m_dXbase = minpoint.X();
  m_dYbase = minpoint.Y();
  m_dZbase = minpoint.Z();
  m_length_division = length_division;
  m_width_division = width_division;
  m_height_division = height_division;
  m_spacing_x = length / (double)length_division;
  m_spacing_y = width / (double)width_division;
  m_spacing_z = height / (double)height_division;

  BuildMesh();
}

bool CBlockMesh::operator==(const CBlockMesh &rhs) const {
  if (fabs(m_dXbase - rhs.XBase()) > compareTolerance())
    return false;
  if (fabs(m_dYbase - rhs.YBase()) > compareTolerance())
    return false;
  if (fabs(m_dZbase - rhs.ZBase()) > compareTolerance())
    return false;

  if (fabs(m_spacing_x - rhs.XSpace()) > compareTolerance())
    return false;
  if (fabs(m_spacing_y - rhs.YSpace()) > compareTolerance())
    return false;
  if (fabs(m_spacing_z - rhs.ZSpace()) > compareTolerance())
    return false;

  if (m_width_division != rhs.WidthDivision())
    return false;
  if (m_length_division != rhs.LengthDivision())
    return false;
  if (m_height_division != rhs.HeightDivision())
    return false;

  return true;
}

} // namespace geo
