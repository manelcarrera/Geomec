/* Copyright (c) 2011 TNO DIANA BV                              Confidential */
#ifndef STREAM_VERSION_H
#define STREAM_VERSION_H

#include "dimple.h"
#include <qstring.h>

#include "ModelFrameExports.h"

class IModelStream;

class MODELFRAME_EXPORT CStreamVersion {
  int m_nMajor;
  int m_nMinor;
  int m_nRevision;

public:
  CStreamVersion();
  CStreamVersion(int nMajor, int nMinor, int nRevision);
  CStreamVersion(int nMajor, int nMinor);
  virtual ~CStreamVersion();

  int majorNr() const;
  int minorNr() const;
  int revisionNr() const;

  typedef IModelStream TStream;
  void restore(TStream &stream);
  void store(TStream &stream);

  QString GetVersionString() const;

  bool operator!=(const CStreamVersion &rhs) const;
  bool operator==(const CStreamVersion &rhs) const;
  bool operator>(const CStreamVersion &rhs) const;
  bool operator>=(const CStreamVersion &rhs) const;
  bool operator<=(const CStreamVersion &rhs) const;
  bool operator<(const CStreamVersion &rhs) const;
};

#endif // STREAM_VERSION_H
