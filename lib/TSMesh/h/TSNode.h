#ifndef _TSNODE_H_
#define _TSNODE_H_

#include "IPoint.h"
#include <vector>

#ifndef BUILDING_TSMESH
#define GtsVertex void
#define gint int
#define gpointer void *
#endif

// ##ModelId=3BBD944902EF
class CTSNode : public geo::IPoint {
  friend class CTSSurface;

public:
  CTSNode();
  CTSNode(GtsVertex *pVertex);

  // ##ModelId=3BBD944902F2
  ~CTSNode();

  // from CCoordinate
  // ##ModelId=3BBD94490300
  virtual const double &X() const;
  // ##ModelId=3BBD94490302
  virtual const double &Y() const;
  // ##ModelId=3BBD94490304
  virtual const double &Z() const;
  // ##ModelId=3BBD94490306
  virtual void X(const double &dX);
  // ##ModelId=3BBD94490310
  virtual void Y(const double &dY);
  // ##ModelId=3BBD94490313
  virtual void Z(const double &dZ);

  // void Rotate(const geo::IVector &vec, const double &AngleDeg);
  //  from CGeoObject
  // ##ModelId=3BBD9449031D
  virtual void AssertValid() const;
  // ##ModelId=3BBD9449031F
  virtual bool Empty() const;

  const GtsVertex *Vertex() const;
  GtsVertex *Vertex();

private:
  GtsVertex *m_pVertex;
};

class CCSNode : public geo::IPoint {
  std::vector<double> m_vcCoor;

public:
  CCSNode();
  CCSNode(const geo::IPoint &point, const void *pHorizon);

  ~CCSNode();

  virtual const double &X() const;
  virtual const double &Y() const;
  virtual const double &Z() const;
  virtual void X(const double &dX);
  virtual void Y(const double &dY);
  virtual void Z(const double &dZ);

  virtual void AssertValid() const;
  virtual bool Empty() const;
};

#endif /* _TSNODE_H_ */
