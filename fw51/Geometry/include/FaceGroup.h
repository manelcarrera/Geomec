/* Copyright (c) 2011 TNO DIANA BV                              Confidential */
// FaceGroup.h: interface for the CFaceGroup class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_FACEGROUP_H__2A116F9B_465F_4420_B7D0_DA88281C7775__INCLUDED_)
#define AFX_FACEGROUP_H__2A116F9B_465F_4420_B7D0_DA88281C7775__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "ISurface.h"

#include "GeometryExports.h"

namespace geo {
class IFace;
class IMesh;
class GEOMETRY_EXPORT CFaceGroup : public ISurface {
  std::vector<const IFace *> m_vcFace;
  typedef std::map<const IPoint *, int, ICoordinate::CCoordinateLess> TPointMap;
  TPointMap m_mpPoint;
  IMesh &m_mesh;
  typedef std::vector<int> TFaceIndexVec;
  typedef std::pair<const IPoint *, TFaceIndexVec> TSurfacePoint;
  std::vector<TSurfacePoint> m_vcPoint;

protected:
  virtual IPoint &PointAt(int nIndex);

public:
  // Side surface constructor
  CFaceGroup(IMesh &mesh);
  virtual ~CFaceGroup();

  bool AddFace(const IFace &face);

  // get the mesh
  const IMesh &Mesh() const;
  IMesh &Mesh();

  // get the point at the given index
  virtual const IPoint &Point(int nIndex) const;
  // get the number of points in this surface
  virtual int PointSize() const;

  // get the node indices connected to the given element
  virtual std::vector<int> Nodes(const IElement &element) const;

  // get the face(s) at the given (arbitrary) point
  virtual const CPtrArray<IFace> FacesAt(const IPoint &p) const;
  // get the face(s) at the given point,
  // which must be a node of the surface (e.g. returned by the Point function)
  virtual const CPtrArray<IFace> FacesAtNode(const IPoint &p) const;

  // get the face at the given index
  virtual const IFace &Face(int nIndex) const;
  // get the number of faces in this surface
  virtual int FaceSize() const;
};

} // namespace geo

#endif // !defined(AFX_FACEGROUP_H__2A116F9B_465F_4420_B7D0_DA88281C7775__INCLUDED_)
