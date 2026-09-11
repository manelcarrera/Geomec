#ifndef NWEWWELLPOINT_H
#define NWEWWELLPOINT_H

#include "AngleQuantity.h"
#include "IElement.h"
#include "IPoint.h"
#include <list>

class CNewWellPathBase;
class CFormationBase;

class CNewWellPoint : public geo::IPoint {
public:
  CNewWellPoint(const CNewWellPathBase &wellPath, const double &TMD);
  CNewWellPoint(const CNewWellPathBase &wellPath, const double &northing, const double &easting, const double &tvd,
                const double &TMD);
  CNewWellPoint(const CNewWellPathBase &wellPath, const geo::IPoint &pt, const double &TMD);

  CNewWellPoint(const CNewWellPoint &rhs);

  virtual ~CNewWellPoint();

  CNewWellPoint &operator=(const CNewWellPoint &rhs);
  bool operator==(const CNewWellPoint &rhs) const;
  bool operator!=(const CNewWellPoint &rhs) const;
  bool operator<(const CNewWellPoint &rhs) const;
  bool operator>(const CNewWellPoint &rhs) const;
  virtual const double &X() const;
  virtual const double &Y() const;
  virtual const double &Z() const;
  virtual void X(const double & /*dX*/) { assert(false); }
  virtual void Y(const double & /*dY*/) { assert(false); }
  virtual void Z(const double & /*dZ*/) { assert(false); }
  virtual void AssertValid() const { assert(m_pWellPath); }
  virtual bool Empty() const { return m_pWellPath == 0; }
  virtual bool Defined() const;

  static bool less(const CNewWellPoint &first, const CNewWellPoint &second);
  static bool equal(const CNewWellPoint &first, const CNewWellPoint &second);

  double TMD() const { return m_dTMD; }
  void TMD(const double &d);
  double Northing() const { return m_dNorthing; }
  void Northing(const double &d) { m_dNorthing = d; }
  double Easting() const { return m_dEasting; }
  void Easting(const double &d) { m_dEasting = d; }
  double TVD() const { return m_dTVD; }
  void TVD(const double &d) { m_dTVD = d; }

  const CNewWellPathBase &WellPath() const;
  CNewWellPathBase &WellPath();

  void addElement(const geo::IElement *pElement) const; // mutable
  void setElements(const std::set<const geo::IElement *> &stElements) const;
  const std::set<const geo::IElement *> &getElements() const;

  static void GetUpperLower(const std::list<CNewWellPoint> &lstPoints, const double &TMD, const CNewWellPoint *MinPoint,
                            const CNewWellPoint *MaxPoint);
  bool EqualPointExist(const std::list<CNewWellPoint> &list) const;
  void AddPoint(std::list<CNewWellPoint> &list) const;

  const CFormationBase *Formation() const;
  void Formation(CFormationBase *) const;

  void AzimuthInclination(CAngleQuantity &Azimuth, CAngleQuantity &Inclination) const;

  void AzimuthInclination(double Azimuth, double Inclination);

  // waij TFS 74145
  // direction vector in this point
  geo::CVector GetDirection() const;

protected:
  mutable CFormationBase *m_pFormation;

private:
  static const double m_dEpsilon;
  const CNewWellPathBase *m_pWellPath;
  double m_dNorthing;
  double m_dEasting;
  double m_dTVD;
  double m_dTMD;
  mutable double m_dX, m_dY, m_dZ; // to be able to return a reference
  mutable std::set<const geo::IElement *> m_stElements;
};

#endif // NWEWWELLPOINT_H
