#ifndef _ECLIPSEBODY_H_
#define _ECLIPSEBODY_H_

#include "IHexahedron.h"

class CEclipseBody : public geo::IHexahedron {
  /*
    class CEclipseBodyFace : public geo::IBodyFace
    {
    public:
      CEclipseBodyFace(const CEclipseBody &parent, int n1, int n2, int n3, int n4, const char *name);
      virtual geo::CValue InterpolateValue(const geo::IPoint &point, const std::vector<geo::CValue> &values) const;
    };

    typedef const double &(geo::IPoint::*Coord)(void) const; // the get function type for the coordinate

    std::vector<geo::IBodyFace *> m_vcBodyFaces;

    void CreateFaces();
    double ShapeFunction(const double &xi, const double &eta, const double &zeta, Coord func) const;
    double ShapeFunction(const double &xi, const double &eta, const double &zeta, const std::vector<geo::CValue> &vals)
    const; double ShapeFunctionDerivedXi(const double &eta, const double &zeta, Coord func) const; double
    ShapeFunctionDerivedEta(const double &xi, const double &zeta, Coord func) const; double
    ShapeFunctionDerivedZeta(const double &xi, const double &eta, Coord func) const; virtual void Jacobian(geo::IMatrix
    &matrix, const double &xi, const double &eta, const double &zeta) const;
  */
  geo::CPoint GlobalFromIsoparametric(const double &xi, const double &eta, const double &zeta) const;
  geo::CPoint IsoparametricFromGlobal(const geo::IPoint &point) const;

  geo::IMesh &m_mesh;
  std::vector<int> m_vcNodes;

public:
  CEclipseBody(geo::IMesh &mesh, const std::vector<int> &points);
  ~CEclipseBody();

  //	virtual const geo::IFace &Face(int nIndex) const;
  //	virtual int NrOfFaces() const;
  //	virtual geo::CValue InterpolateValue(const geo::IPoint &point, const std::vector<geo::CValue> &values) const;
  //	virtual double Volume() const;
  //	virtual bool Contains(const geo::IPoint &point, bool bIncludeEdge) const;

  virtual const geo::IPoint &Point(int nIndex) const;
  virtual void Point(int nIndex, const geo::IPoint &pt);
  //	virtual int NrOfPoints() const;

  virtual size_t Order() const { return 1; }
  const char *FaceName(int /*nIndex*/) const { return 0; }

  // checks whether the element is degenerate (e.g. concave)
  virtual bool Degenerate() const;
};

#endif // _ECLIPSEBODY_H_
