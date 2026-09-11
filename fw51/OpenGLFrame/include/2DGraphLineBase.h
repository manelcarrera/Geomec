 /* Copyright (c) 2011 TNO DIANA BV                              Confidential */
#if !defined(OPENGL_2DGRAPHLINEBASE_H__INCLUDED_)
#define OPENGL_2DGRAPHLINEBASE_H__INCLUDED_

#include "Marker.h"
#include "Value.h"

#include "OpenGLFrameExports.h"

class C2DGraphBase;
class OPENGLFRAME_EXPORT C2DGraphLineBase
{
public:
  typedef std::pair<geo::CPoint, int> TGraphPoint;
  typedef std::vector<TGraphPoint> TSerie;
  typedef std::vector<TSerie> TSerieVec;
  TSerieVec GenerateValues(bool dDelimitY = true) const;
private:
  friend class C2DGraphBase;
  C2DGraphBase& m_parent;
  unsigned long m_Color; 
  unsigned long m_LineWidth;
  QString m_Description;
  bool m_bLineStipple;
  // Internal functions
  enum ePointStatus { INVALID, OUTSIDE_MIN, INSIDE, OUTSIDE_MAX };
  enum ePointStatus Status(const double& dMin, const double& dMax, const geo::CValue& value) const;

  double InterpolateY(int nFirst, int nSecond, const double& dX) const;
  double InterpolateX(int nFirst, int nSecond, const double& dY) const;
  std::pair<geo::CValue, geo::CValue> MinMaxY(const double& dMinX, const double& dMaxX) const;
public:
  C2DGraphLineBase(C2DGraphBase& parent, 
           unsigned long color = qRgb(255,0,0),
           bool bLineStipple = false);

  C2DGraphLineBase(C2DGraphBase& parent, 
           const QString& description,
           unsigned long color = qRgb(255,0,0),
           bool bLineStipple = false);

  virtual ~C2DGraphLineBase();

  virtual void Update();

  virtual int PointSize() const = 0;
  virtual double X(int nIndex) const = 0;
  virtual geo::CValue Y(int nIndex) const = 0;
  virtual CMarker* CreateMarker(int nIndex) const;
  geo::CValue InterpolateY(const double& dX) const;
  
  virtual double MaxX() const;
  virtual double MinX() const;
//	virtual geo::CValue MaxY(const double& dMinX, const double& dMaxX) const;
//	virtual geo::CValue MinY(const double& dMinX, const double& dMaxX) const;

  C2DGraphBase& Parent();
  const C2DGraphBase& Parent() const;
  
  // Line properties
  unsigned long Color() const;
  void Color(unsigned long color);

  unsigned long LineWidth() const;
  void LineWidth(unsigned long linewidth);

  bool LineStipple() const;
  void LineStipple(bool bLinestipple);

  const QString& Description() const;
  void Description(const QString& description);

};

#endif // !defined(OPENGL_2DGRAPHLINEBASE_H__INCLUDED_)
