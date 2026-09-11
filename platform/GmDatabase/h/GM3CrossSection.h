// CrossSection.h: interface for the CCrossSection class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CROSSSECTION_H__37164AAD_3AE3_4280_B079_DDA905417FDA__INCLUDED_)
#define AFX_CROSSSECTION_H__37164AAD_3AE3_4280_B079_DDA905417FDA__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <set>

#include <qstring.h>

#include "point.h"

class IProgressBase;
namespace gm 
{
class C3DDatabase;
class C3DCrossSection  
{
  std::pair<geo::CPoint, geo::CPoint> m_point;
  QString m_strName;
public:
  C3DCrossSection();
  virtual ~C3DCrossSection();

  void ReadCrossSection(C3DDatabase &db, const long lCrossSectionKey, IProgressBase &callback);

  const QString& Name() const;
  const geo::IPoint& First() const;
  const geo::IPoint& Second() const;
};

}

#endif // !defined(AFX_CROSSSECTION_H__37164AAD_3AE3_4280_B079_DDA905417FDA__INCLUDED_)
