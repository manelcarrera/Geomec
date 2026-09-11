 /* Copyright (c) 2011 TNO DIANA BV                              Confidential */

#ifndef ISPLINE_H
#define ISPLINE_H

#include "IElement.h"
#include "IQuadrilateral.h"

#include "GeometryExports.h"

namespace geo {

class GEOMETRY_EXPORT  ISpline : public IElement
{
  class GEOMETRY_EXPORT CSplineFace : public IQuadrilateral
  {
  const ISpline& m_parent;
  int m_nIndex;

  public:
  CSplineFace(const ISpline& parent, int nIndex);
  ~CSplineFace();

  const ISpline& Spline() const;

  virtual size_t NrOfNodes() const;
  virtual const INode &Node(int nIndex) const;
  virtual void Node(int nIndex, const IPoint &pt);

  virtual size_t Order() const;
  int ParentNodeIndex(int nIndex) const;

  int Index() const;
  };

  class GEOMETRY_EXPORT CSplineLine : public ILine
  {
  const ISpline& m_parent;
  int m_Nodes[2];

  public:
  CSplineLine(const ISpline& parent, int nFirst, int nSecond);
  ~CSplineLine();

  const ISpline& localParent() const;

  virtual size_t NrOfNodes() const;
  virtual const INode& Node(int nIndex) const;
  virtual void Node(int nIdex, const IPoint& pt);

  virtual size_t Order() const;
  int ParentNodeIndex(int nIndex) const;
  };

  void BuildFaceCache() const;
  void DestroyFaceCache() const;
  mutable std::vector<CSplineFace*> m_vcFaces;

  void BuildLineCache() const;
  void DestroyLineCache() const;
  mutable std::vector<CSplineLine*> m_vcLines;

  CValue InterpolateValue(const IPoint& point, const std::vector<CValue>& values, int iField) const;

public:
  ISpline();
  virtual ~ISpline();

  virtual int NrOfPoints() const;
  virtual const IPoint &Point(int nIndex) const;
  virtual void Point(int nIndex, const IPoint &pt);

  virtual int NrOfFaces() const;
  virtual const IQuadrilateral& Face(int nIndex) const;
  virtual std::vector<int> FaceNodeIndices(int nIndex) const;
  virtual std::vector<int> FacePointIndices(int nIndex) const;

  virtual int NrOfLines() const;
  virtual const ILine &Line(int nIndex) const;
  virtual std::vector<int> LinePointIndices(int nIndex) const;
  virtual std::vector<int> LineNodeIndices(int nIndex) const;

  virtual IElement::TDoubleVec ShapeFunction(const IElement::TDoubleVec& isocoords) const;

  virtual CMatrix ShapeFunctionDerived(const IElement::TDoubleVec& isocoords) const;
  virtual std::vector<IElement::TDoubleVec> IsoCoordinates() const;

  virtual size_t Order() const { return 1; }
  virtual double RepresentativeLength() const;

  // integration points
  virtual int IntegrationPointSize() const;
  virtual const TDoubleVec& IntegrationPointCoords(int nIndex) const;
  virtual const double& IntegrationPointWeight(int nIndex) const;

  virtual double Size() const;

  virtual CValue InterpolateValue(const TDoubleVec& isocoords, const std::vector<CValue> &values) const;
  virtual CValue InterpolateValue(const IPoint& point, const std::vector<CValue>& values) const;
};

} // namespace geo

#endif
