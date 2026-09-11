/* Copyright (c) 2011 TNO DIANA BV                              Confidential */
// Property.h: interface for the CProperty class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_PROPERTY_H__147419D0_54A9_4FB4_96ED_DB1FDCA550C3__INCLUDED_)
#define AFX_PROPERTY_H__147419D0_54A9_4FB4_96ED_DB1FDCA550C3__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "IPoint.h"
#include <map>
#include <qstring.h>
#include <vector>

#include "FieldValueFileExports.h"

// Last Reviewed: 22 November 2002, tnma

// REVIEW
// Add more comment including a short description desribing the use of the class.
// e.g. what is the use of the TPointMap?
// END REVIEW

class FIELDVALUEFILE_EXPORT CProperty {
public:
  typedef std::vector<double> TPropertyVec;

private:
  QString m_strName;
  QString m_strClassName;
  QString m_strUnitName;
  int m_nSize;
  typedef std::map<const geo::IPoint *, TPropertyVec, geo::ICoordinate::CCoordinateLess> TPointMap;
  TPropertyVec m_vcEmpty;
  TPointMap m_mpValue;

public:
  CProperty(const QString &name);
  virtual ~CProperty();

  // Sizes of the property point
  int Size() const;
  void Size(int nSize);

  // Names of the property
  const QString &Name() const;
  const QString &UnitName() const;
  const QString &ClassName() const;
  void UnitName(const QString &unit_name);
  void ClassName(const QString &unit_name);

  // Get and add values ....
  const TPropertyVec &Value(const geo::IPoint &point) const;

  // REVIEW
  // suggestion: change AddValue in Value?
  // END REVIEW
  void AddValue(const geo::IPoint &point, const TPropertyVec &property);
};

#endif // !defined(AFX_PROPERTY_H__147419D0_54A9_4FB4_96ED_DB1FDCA550C3__INCLUDED_)
