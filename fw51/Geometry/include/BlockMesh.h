/* Confidential Source Code Copyright (c) 2011 TNO DIANA BV                              Confidential */
// BlockMesh.h: interface for the CBlockMesh class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_BLOCKMESH_H__55AC00EE_BA3B_4D20_9643_8BFB3C1AFF1F__INCLUDED_)
#define AFX_BLOCKMESH_H__55AC00EE_BA3B_4D20_9643_8BFB3C1AFF1F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Mesh.h"

#include "GeometryExports.h"


namespace geo{
class GEOMETRY_EXPORT  CBlockMesh : public CMesh
{
  std::map<int, int> m_mpMeshPoints; // map from my indices to indices created when registering the points

  double m_dXbase;
  double m_dYbase;
  double m_dZbase;
  double m_spacing_x;
  double m_spacing_y;
  double m_spacing_z;
  int m_length_division;
  int m_width_division;
  int m_height_division;

  int m_base_node; // node with minimum coordinates

  // vectors with indices to points making up the side surfaces of the mesh (boundary)
  std::vector<int> m_vcLeftFace;	// minimum X value
  std::vector<int> m_vcRightFace; // maximum X value
  std::vector<int> m_vcFrontFace;	// minimum Y value
  std::vector<int> m_vcBackFace;	// maximum Y value
  std::vector<int> m_vcBottomFace;// minimum Z value
  std::vector<int> m_vcTopFace;	// maximum Z value

  void BuildMesh();
public:
  CBlockMesh();
  virtual ~CBlockMesh();

  void SetMesh(const IPoint &minpoint,
         const double &length, 
         const double &width, 
         const double &height, 
         int length_division,
         int width_division,
         int height_division);

  CBlockMesh& operator = (const CBlockMesh &rhs);

  double XBase() const { return m_dXbase; }
  double YBase() const { return m_dYbase; }
  double ZBase() const { return m_dZbase; }

  double XSpace() const { return m_spacing_x; }
  double YSpace() const { return m_spacing_y; }
  double ZSpace() const { return m_spacing_z; }

  int LengthDivision() const { return m_length_division; }
  int WidthDivision() const { return m_width_division; }
  int HeightDivision() const { return m_height_division; }

  virtual void Clear();

  const std::vector<int> &LeftFace() const { return m_vcLeftFace; }
  const std::vector<int> &RightFace() const { return m_vcRightFace; }
  const std::vector<int> &FrontFace() const { return m_vcFrontFace; }
  const std::vector<int> &BackFace() const { return m_vcBackFace; }
  const std::vector<int> &BottomFace() const { return m_vcBottomFace; }
  const std::vector<int> &TopFace() const { return m_vcTopFace; }

  int BaseNode() const { return m_base_node; }

  bool operator==(const CBlockMesh &rhs) const;
};
}
#endif // !defined(AFX_BLOCKMESH_H__55AC00EE_BA3B_4D20_9643_8BFB3C1AFF1F__INCLUDED_)
