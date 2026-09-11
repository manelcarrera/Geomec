/* Copyright (c) 2011 TNO DIANA BV                              Confidential */
#include "FvGocadHeader.h"
#include <cassert>

CGoCadHeader::CGoCadHeader(const QString &sName) : m_sName(sName) {}

CGoCadHeader::~CGoCadHeader() {
  for (size_t i = 0; i < m_vcProperty.size(); i++)
    delete m_vcProperty[i];

  // now it is safe to delete the points (which are related to the properties)
  for (size_t i = 0; i < m_vcPoints.size(); i++)
    delete m_vcPoints[i];
}

const QString &CGoCadHeader::Name() const { return m_sName; }

int CGoCadHeader::PropertySize() const { return m_vcProperty.size(); }

int CGoCadHeader::AddProperty(const QString &name) {
  int nRet = PropertySize();
  CProperty *pProperty = new CProperty(name);
  m_vcProperty.push_back(pProperty);
  return nRet;
}

const CProperty &CGoCadHeader::Property(int nIndex) const {
  assert(nIndex > -1);
  assert(nIndex < PropertySize());
  return *m_vcProperty[nIndex];
}

CProperty &CGoCadHeader::Property(int nIndex) {
  assert(nIndex > -1);
  assert(nIndex < PropertySize());
  return *m_vcProperty[nIndex];
}

void CGoCadHeader::AddPoint(const geo::IPoint *pPoint) { m_vcPoints.push_back(pPoint); }
