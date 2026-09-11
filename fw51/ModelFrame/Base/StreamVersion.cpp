/* Copyright (c) 2011 TNO DIANA BV                              Confidential */
#include "StreamVersion.h"
#include <IModelStream.h>

CStreamVersion::CStreamVersion() : m_nMajor(-1), m_nMinor(-1), m_nRevision(-1) {}

CStreamVersion::CStreamVersion(int nMajor, int nMinor, int nRevision)
    : m_nMajor(nMajor), m_nMinor(nMinor), m_nRevision(nRevision) {}

CStreamVersion::CStreamVersion(int nMajor, int nMinor) : m_nMajor(nMajor), m_nMinor(nMinor), m_nRevision(0) {
  // Construct
}

CStreamVersion::~CStreamVersion() {}

int CStreamVersion::majorNr() const { return m_nMajor; }

int CStreamVersion::minorNr() const { return m_nMinor; }

int CStreamVersion::revisionNr() const { return m_nRevision; }

void CStreamVersion::restore(TStream &stream) {
  stream >> m_nMajor;
  stream >> m_nMinor;
  stream >> m_nRevision;
}

void CStreamVersion::store(TStream &stream) {
  stream << majorNr();
  stream << minorNr();
  stream << revisionNr();
}

bool CStreamVersion::operator!=(const CStreamVersion &rhs) const {
  if (rhs < *this)
    return true;

  if (*this < rhs)
    return true;

  return false;
}

bool CStreamVersion::operator==(const CStreamVersion &rhs) const {
  if (rhs < *this)
    return false;

  if (*this < rhs)
    return false;

  return true;
}

bool CStreamVersion::operator>(const CStreamVersion &rhs) const {
  if (rhs < *this)
    return true;

  return false;
}

bool CStreamVersion::operator>=(const CStreamVersion &rhs) const {
  if (*this < rhs)
    return false;

  return true;
}

bool CStreamVersion::operator<=(const CStreamVersion &rhs) const {
  if (rhs < *this)
    return false;

  return true;
}

bool CStreamVersion::operator<(const CStreamVersion &rhs) const {
  if (m_nMajor < rhs.m_nMajor)
    return true;

  if (m_nMajor > rhs.m_nMajor)
    return false;

  // Major is equal

  if (m_nMinor < rhs.m_nMinor)
    return true;

  if (m_nMinor > rhs.m_nMinor)
    return false;

  // Major and Minor is equal

  if (m_nRevision < rhs.m_nRevision)
    return true;

  if (m_nRevision > rhs.m_nRevision)
    return false;

  // Major and Minor and Revision are equal, hence all equal;

  return false;
}

QString CStreamVersion::GetVersionString() const {
  return QString("%1.%2.%3").arg(m_nMajor).arg(m_nMinor).arg(m_nRevision);
}
