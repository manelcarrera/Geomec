/* Copyright (c) 2011 TNO DIANA BV                              Confidential */
#if !defined(AFX_VOXET_H__D3EFC9A8_7513_4E51_95F0_40331AAF2C74__INCLUDED_)
#define AFX_VOXET_H__D3EFC9A8_7513_4E51_95F0_40331AAF2C74__INCLUDED_

#include "FvArrayTemp.h"
#include "IHexahedron.h"

#include "FieldValueFileExports.h"

class CEclipseCellFace;
class FIELDVALUEFILE_EXPORT CEclipseCell : public geo::IHexahedron {

  // array_temp<geo::CPoint> m_array;
  array_temp<geo::CPoint *> m_array;
  //  mutable std::vector<CEclipseCellFace*> m_vcFace;
  //  void CreateFaces() const;
  const geo::IPoint &InternalPoint(int nIndex) const;

public:
  // Construction / Destruction
  CEclipseCell();
  CEclipseCell(const CEclipseCell &rhs);
  ~CEclipseCell();

  CEclipseCell &operator=(const CEclipseCell &rhs);

  // IBody interface
  virtual const geo::IPoint &Point(int nIndex) const;
  virtual void Point(int nIndex, const geo::IPoint &pt);

  virtual double Volume() const;

  virtual size_t Order() const { return 1; }
  virtual const char *FaceName(int /*nIndex*/) const { return 0; }

  virtual bool Contains(const geo::IPoint &point, bool bIncludeEdge) const;
  using geo::IHexahedron::InterpolateValue;
  virtual geo::CValue InterpolateValue(const geo::IPoint &point, const std::vector<geo::CValue> &values) const;
  using geo::IHexahedron::Intersection;
  virtual std::set<geo::CPoint> Intersection(const geo::IPlane &plane) const;

  geo::CPoint CentreOfGravity() const;

  void SetAt(int x, int y, int z, const geo::IPoint *point);
  const geo::IPoint &GetAt(int x, int y, int z) const;
  geo::IPoint &GetAt(int x, int y, int z);
  bool IsPinchOut() const;
  std::vector<const geo::IPoint *> Points() const;
  bool PinchOut() const;
};

#endif // !defined(AFX_VOXET_H__D3EFC9A8_7513_4E51_95F0_40331AAF2C74__INCLUDED_)
