#ifndef _ROTATEDSYSTEM_H_
#define _ROTATEDSYSTEM_H_

namespace geo {
  class IPoint;
  class CPoint;
  class IVector;
  class CVector;
  class IElement;
}

class CRotatedSystem
{
  // The rotation is defined by an azimuth around the Z axis
  // The offset is the local system's origin in global coordinates
public:
  CRotatedSystem(double dAzimuth, const geo::IPoint& ptRotation); // azimuth in deg
  ~CRotatedSystem();

  // convert from global (x, y) to local (xi, eta) system
  geo::CPoint  ToLocal(const geo::IPoint& pt) const;
  geo::CVector ToLocal(const geo::IVector& v) const;

  void ToLocalSet(geo::IPoint& pt) const;
  void ToLocalSet(geo::IVector& v) const;

  // convert from local (xi, eta) to global (x, y) system
  geo::CPoint  ToGlobal(const geo::IPoint& pt) const;
  geo::CVector ToGlobal(const geo::IVector& v) const;

  void ToGlobalSet(geo::IPoint& pt) const;
  void ToGlobalSet(geo::IVector& v) const;

  // get the local (xi, eta) min point (in the local system)
  geo::CPoint LocalMin(const geo::IElement& element) const;

  // get the local (xi, eta) max point (in the local system)
  geo::CPoint LocalMax(const geo::IElement& element) const;

private:
  double m_dAzimuth; // in rad
  geo::CPoint* m_pptRotation;
};

#endif // _ROTATEDSYSTEM_H_
