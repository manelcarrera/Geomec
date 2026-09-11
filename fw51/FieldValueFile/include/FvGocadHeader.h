/* Copyright (c) 2011 TNO DIANA BV                              Confidential */
#if !defined(AFX_GOCADHEADER_H__E1681716_8B68_4581_86C3_25F929467DD4__INCLUDED_)
#define AFX_GOCADHEADER_H__E1681716_8B68_4581_86C3_25F929467DD4__INCLUDED_

#include "FvProperty.h"

#include "FieldValueFileExports.h"

class FIELDVALUEFILE_EXPORT CGoCadHeader {
  QString m_sName;
  std::vector<CProperty *> m_vcProperty;
  std::vector<const geo::IPoint *> m_vcPoints;

public:
  CGoCadHeader(const QString &sName);
  virtual ~CGoCadHeader();
  const QString &Name() const;

  // Properties ....
  int PropertySize() const;
  int AddProperty(const QString &name);
  const CProperty &Property(int nIndex) const;
  CProperty &Property(int nIndex);
  void AddPoint(const geo::IPoint *pPoint);
};

#endif // !defined(AFX_GOCADHEADER_H__E1681716_8B68_4581_86C3_25F929467DD4__INCLUDED_)
