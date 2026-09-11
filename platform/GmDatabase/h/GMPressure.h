// GMPressure.h: interface for the CPressure class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_GMPRESSURE_H__0883638C_492D_4C23_BF85_8689E6D59D5E__INCLUDED_)
#define AFX_GMPRESSURE_H__0883638C_492D_4C23_BF85_8689E6D59D5E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <vector>

#include <qstring.h>

#include "point.h"

namespace gm {

class CPressure {
public:
  enum PressureType { UNDEFINED, CONSTANT, DISTRIBUTED, OWCGWC };
  typedef std::pair<geo::CPoint, double> TPressurePoint;

protected:
  PressureType m_type;
  double m_dReferenceDepth;
  double m_dReferencePressure;
  std::vector<double> m_vcGradient;
  std::vector<TPressurePoint> m_vcPoint;
  std::vector<geo::CPoint> m_vcInterval;
  QString m_strSourceFileName;

public:
  CPressure();
  virtual ~CPressure();

  // Pressure type
  bool UnDefined() const;
  bool GWC() const;
  bool Constant() const;
  bool Distributed() const;
  enum PressureType Type() const;

  // Constant and GWC Access
  const double &ReferenceDepth() const;
  const double &ReferencePressure() const;

  // Constant
  const double &Gradient() const;

  // GWC
  const double &UpperGradient() const;
  const double &LowerGradient() const;

  // Distributed pressure access
  int PointSize() const;
  const TPressurePoint &Point(int nIndex) const;

  int IntervalSize() const;
  const geo::IPoint &Interval(int nIndex) const;

  const QString &SourceFileName() const;
};

} // namespace gm

#endif // !defined(AFX_GMPRESSURE_H__0883638C_492D_4C23_BF85_8689E6D59D5E__INCLUDED_)
