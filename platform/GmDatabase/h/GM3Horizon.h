// Horizon.h: interface for the CHorizon class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_HORIZON_H__B18C5818_72FD_41A4_B71E_82F78B7931E3__INCLUDED_)
#define AFX_HORIZON_H__B18C5818_72FD_41A4_B71E_82F78B7931E3__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <qstring.h>

#include "IProgressBase.h"
#include "vectortempl.h"

namespace gm {
class C3DDatabase;
// ##ModelId=3BA8A1C801AA
class C3DHorizon {
  // ##ModelId=3BA8A1C801AB
  long m_lColor;
  geo::CPoint m_ptMin, m_ptMax;

public:
  // ##ModelId=3BA8A1C801DC
  typedef geo::CArray<geo::CPoint> TPointVec;

private:
  // ##ModelId=3BA8A1C801BA
  TPointVec m_vcPoint;
  // ##ModelId=3BA8A1C801BE
  BOOL m_bConstantDepth;
  // ##ModelId=3BA8A1C801BF
  QString m_strName;

public:
  int Color() const;
  const geo::IPoint &Max() const;
  const geo::IPoint &Min() const;
  const TPointVec &Vertices() const;
  // ##ModelId=3BA8A1C801C9
  BOOL ConstantDepth() const;
  // ##ModelId=3BA8A1C801CB
  void LoadHorizon(const long nIndex, C3DDatabase &db, IProgressBase &callback);
  // ##ModelId=3BA8A1C801CE
  const QString &Name() const;
  // ##ModelId=3BA8A1C801D9
  C3DHorizon();
  // ##ModelId=3BA8A1C801DA
  virtual ~C3DHorizon();
};

} // namespace gm

#endif // !defined(AFX_HORIZON_H__B18C5818_72FD_41A4_B71E_82F78B7931E3__INCLUDED_)
