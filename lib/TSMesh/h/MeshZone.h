#ifndef _MESHZONE_H_
#define _MESHZONE_H_

struct _Region;

#include "IFace.h"

/*#ifndef BUILDING_TSMESH
#define Region void
#endif
*/
class CMeshZone: public geo::IFace
{
  std::vector<geo::CPoint> m_vcElementPoints;
  _Region *m_pMateRegion;
  double m_dEdgeSquared;
public:
  CMeshZone();
  CMeshZone(const geo::CPtrArray<geo::IPoint> &points);
  CMeshZone(const geo::IElement &element);
  CMeshZone(const CMeshZone &meshzone);
  ~CMeshZone();

  virtual const geo::IPoint &Point(int nIndex) const;
  virtual void Point(int nIndex, const geo::IPoint &pt);
  virtual int NrOfPoints() const;

  virtual double Size() const {assert(false); return 0;}

  virtual geo::CValue InterpolateValue(const geo::IPoint &point, const std::vector<geo::CValue> &values) const;
  double Criterium() {return m_dEdgeSquared;}
  void Criterium(double dValue) {m_dEdgeSquared = dValue;}
  void Region(_Region *pRegion) {m_pMateRegion = pRegion;}
  _Region* Region() {return m_pMateRegion;}

  virtual size_t Order() const { return 1; }

//	bool operator < (const CMeshZone &rhs) const;
};

#endif /* _MESHZONE_H_ */
